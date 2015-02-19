/*
 * Regmap for remote mcuio devices (not living on this machine)
 * Presently, all mcuio devices are remote devices except for the ho
 * controller. Code comes from regmap-mmio
 */

#include <linux/err.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/regmap.h>

#include <linux/mcuio.h>
#include <linux/mcuio-proto.h>

#define MAX_RETRIES 3

/**
 * mcuio bus context
 * @hc: pointer to host controller
 * @dev: device number of mcuio device
 * @func: function number of mcuio device
 */
struct regmap_mcuio_context {
	struct mcuio_device *hc;
	unsigned dev;
	unsigned func;
	unsigned val_bytes;
};

static int regmap_mcuio_gather_write(void *context,
				     const void *reg, size_t reg_size,
				     const void *val, size_t val_size)
{
	struct regmap_mcuio_context *ctx = context;
	struct mcuio_request r;
	u32 offset;
	unsigned t;
	int ret = 0;
	int retries = MAX_RETRIES;

	BUG_ON(reg_size != 4);

	offset = *(u32 *)reg;

	r.hc = ctx->hc;
	r.dev = ctx->dev;
	r.func = ctx->func;
	r.offset = offset;

	switch (ctx->val_bytes) {
	case 1:
		t = mcuio_type_wrb;
		break;
	case 2:
		t = mcuio_type_wrw;
		break;
	case 4:
		t = mcuio_type_wrdw;
		break;
	case 8:
		t = mcuio_type_wrq;
		break;
	default:
		BUG();
	}

	while (val_size && retries) {
		int sz = ctx->val_bytes, fill = 0;

		if (val_size >= sizeof(u64)) {
			fill = 1;
			sz = sizeof(u64);
		}
		memcpy(r.data, val, sz);
		mcuio_init_request(&r, ctx->hc, ctx->dev, ctx->func,
				   t, fill, offset, 0xffff);
		ret = mcuio_submit_request(&r);
		if (ret == -ETIMEDOUT) {
			retries--;
			continue;
		}
		if (ret)
			break;
		val_size -= sz;
		val += sz;
		offset += sz;
	}

	return ret;
}

static int regmap_mcuio_write(void *context, const void *data, size_t count)
{
	BUG_ON(count < 4);

	return regmap_mcuio_gather_write(context, data, 4, data + 4, count - 4);
}

static int regmap_mcuio_read(void *context,
			     const void *reg, size_t reg_size,
			     void *val, size_t val_size)
{
	struct regmap_mcuio_context *ctx = context;
	struct mcuio_request r;
	union mcuio_packet_data data;
	u32 offset = *(u32 *)reg;
	int ret = 0;
	unsigned t;
	int retries = MAX_RETRIES;

	BUG_ON(reg_size != 4);
	
	switch (ctx->val_bytes) {
	case 1:
		t = mcuio_type_rdb;
		break;
	case 2:
		t = mcuio_type_rdw;
		break;
	case 4:
		t = mcuio_type_rddw;
		break;
	case 8:
		t = mcuio_type_rdq;
		break;
	default:
		return -EINVAL;
	}
	while (val_size && retries) {
		int sz = ctx->val_bytes, fill = 0;

		fill = 0;
		if (val_size >= sizeof(u64)) {
			fill = 1;
			sz = sizeof(u64);
		}
		mcuio_init_request(&r, ctx->hc, ctx->dev, ctx->func, t,
				   fill, offset, 0xffff);
		r.data = &data;
		ret = mcuio_submit_request(&r);
		if (ret == -ETIMEDOUT) {
			retries--;
			continue;
		}
		if (ret)
			break;
		memcpy(val, r.data, sz);
		val_size -= sz;
		val += sz;
		offset += sz;
	}
	return ret;
}


static void regmap_mcuio_free_context(void *context)
{
	struct regmap_mcuio_context *ctx = context;
	kfree(ctx);
}

static struct regmap_bus regmap_mcuio = {
	.write = regmap_mcuio_write,
	.read = regmap_mcuio_read,
	.free_context = regmap_mcuio_free_context,
	.reg_format_endian_default = REGMAP_ENDIAN_NATIVE,
	.val_format_endian_default = REGMAP_ENDIAN_NATIVE,
};

static struct regmap_mcuio_context *
regmap_mcuio_setup_context(struct mcuio_device *mdev,
			   const struct regmap_config *config)
{
	struct mcuio_device *hc = to_mcuio_dev(mdev->dev.parent);
	struct regmap_mcuio_context *ctx;
	int min_stride;

	if (config->reg_bits != 32)
		return ERR_PTR(-EINVAL);

	switch (config->val_bits) {
	case 8:
		/* The core treats 0 as 1 */
		min_stride = 0;
		break;
	case 16:
		min_stride = 2;
		break;
	case 32:
		min_stride = 4;
		break;
#ifdef CONFIG_64BIT
	case 64:
		min_stride = 8;
		break;
#endif
		break;
	default:
		return ERR_PTR(-EINVAL);
	}

	ctx = kzalloc(sizeof(*ctx), GFP_KERNEL);
	if (!ctx)
		return ERR_PTR(-ENOMEM);

	ctx->hc = hc;
	ctx->dev = mdev->device;
	ctx->func = mdev->fn;
	ctx->val_bytes = config->val_bits / 8;
	return ctx;
}


/**
 * regmap_init_mcuio(): Initialise mcuio register map
 *
 * @dev: Device that will be interacted with
 * @hc: mcuio system controller
 * @config: Configuration for register map
 *
 * The return value will be an ERR_PTR() on error or a valid pointer to
 * a struct regmap.
 */
struct regmap *regmap_init_mcuio(struct mcuio_device *mdev,
				 const struct regmap_config *config)
{
	struct regmap_mcuio_context *ctx;
	ctx = regmap_mcuio_setup_context(mdev, config);
	if (IS_ERR(ctx))
		return ERR_CAST(ctx);

	return regmap_init(&mdev->dev, &regmap_mcuio, ctx, config);
}
EXPORT_SYMBOL_GPL(regmap_init_mcuio);

/**
 * devm_regmap_init_mcuio(): Initialise mcuio register map, device manage
 * version
 *
 * @dev: Device that will be interacted with
 * @hc: mcuio system controller
 * @config: Configuration for register map
 *
 * The return value will be an ERR_PTR() on error or a valid pointer to
 * a struct regmap.
 */
struct regmap *devm_regmap_init_mcuio(struct mcuio_device *mdev,
				      const struct regmap_config *config)
{
	struct regmap_mcuio_context *ctx;
	ctx = regmap_mcuio_setup_context(mdev, config);
	if (IS_ERR(ctx))
		return ERR_CAST(ctx);

	return devm_regmap_init(&mdev->dev, &regmap_mcuio, ctx, config);
}
EXPORT_SYMBOL_GPL(devm_regmap_init_mcuio);

MODULE_AUTHOR("Davide Ciminaghi");
MODULE_DESCRIPTION("MCUIO bus regmap implementation");
MODULE_LICENSE("GPL v2");
