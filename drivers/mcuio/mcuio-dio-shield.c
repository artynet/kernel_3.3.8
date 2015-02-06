/*
 * Copyright 2014 Dog Hunter SA
 * Author: Aurelio Colosimo <aurelio@aureliocolosimo.it>
 *
 * GNU GPLv2 or later
 */

#define DEBUG

/* mcuio driver for DigitalIO shield */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/regmap.h>
#include <linux/workqueue.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/i2c/pca953x.h>

#include <linux/mcuio.h>
#include <linux/mcuio_ids.h>
#include <linux/mcuio-proto.h>

#include "mcuio-internal.h"
#include "mcuio-shields.h"

static unsigned short pca9555_addr = 0x27;
static unsigned int pca9555_base = 216;
module_param(pca9555_addr, ushort, 0644);
module_param(pca9555_base, uint, 0644);

static struct pca953x_platform_data pca9555_plat;

static struct mcuio_shld_i2c_info i2c_lst[] = {
	MCUIO_SHLD_I2C_DEV("pca9555", &pca9555_addr, &pca9555_plat, 104),
};

static int mcuio_dio_probe(struct mcuio_device *mdev)
{
	struct mcuio_shld_i2c_info *i;
	int cnt;
	int ret;
	struct mcuio_shld_data *data;

	dev_dbg(&mdev->dev, "%s entered\n", __func__);

	data = devm_kzalloc(&mdev->dev, sizeof(*data), GFP_KERNEL);
	if (!data)
		return -ENOMEM;

	dev_set_drvdata(&mdev->dev, data);

	/* Apply module_param values to platform_data when needed */
	pca9555_plat.gpio_base = pca9555_base;

	data->i2c_adap = mcuio_get_i2c_adapter(mdev);

	if (!data->i2c_adap) {
		dev_err(&mdev->dev, "error setting up i2c adapter\n");
		return -ENODEV;
	}

	data->i2c_info = i2c_lst;
	data->i2c_cnt = ARRAY_SIZE(i2c_lst);

	/* Register all devices in Digital IO shield */
	for (cnt = 0; cnt < data->i2c_cnt; cnt++) {
		i = &data->i2c_info[cnt];
		i->info.addr = *i->paddr;

		/* HACK this is needed to enable pullup */
		ret = devm_gpio_request_one(&mdev->dev, i->gpio_irq, GPIOF_DIR_IN,
				    "digitalio-shield");
		if (ret < 0)
			return ret;
		gpio_direction_output(i->gpio_irq, 1);
		gpio_direction_input(i->gpio_irq);
		devm_gpio_free(&mdev->dev, i->gpio_irq);

		i->info.irq = (i->gpio_irq >= 0) ?
			gpio_to_irq(i->gpio_irq) : 0;

		i->i2c_client = i2c_new_device(data->i2c_adap, &i->info);
		if (!i->i2c_client)
			dev_err(&mdev->dev,
				"i2c_new_device %s failed\n", i->info.type);
	}

	dev_dbg(&mdev->dev, "%s returns ok\n", __func__);

	return 0;
}

static int mcuio_dio_remove(struct mcuio_device *mdev)
{
	struct mcuio_shld_i2c_info *i;
	struct mcuio_shld_data *data;

	data = dev_get_drvdata(&mdev->dev);

	/* Unregister all devices in Digital IO shield, in opposite order as they
	 * had been registered */
	for (i = &data->i2c_info[data->i2c_cnt - 1];
	     data->i2c_cnt; i--, data->i2c_cnt--) {
		if (i->i2c_client) {
			i2c_unregister_device(i->i2c_client);
			i->i2c_client = NULL;
		}
	}

	return 0;
}

static const struct mcuio_device_id dio_drv_ids[] = {
	{
		.vendor = MCUIO_VENDOR_DOGHUNTER,
		.device = MCUIO_DEVICE_DIGITALIO_SHIELD,
	},
	/* Terminator */
	{
		.device = MCUIO_NO_DEVICE,
		.class = MCUIO_CLASS_UNDEFINED,
	},
};

static struct mcuio_driver mcuio_dio_driver = {
	.driver = {
		.name = "mcuio-digitalio-shield",
	},
	.id_table = dio_drv_ids,
	.probe = mcuio_dio_probe,
	.remove = mcuio_dio_remove,
};

static int __init mcuio_dio_init(void)
{
	return mcuio_driver_register(&mcuio_dio_driver, THIS_MODULE);
}

static void __exit mcuio_dio_exit(void)
{
	return mcuio_driver_unregister(&mcuio_dio_driver);
}

subsys_initcall(mcuio_dio_init);
module_exit(mcuio_dio_exit);

MODULE_AUTHOR("Aurelio Colosimo");
MODULE_DESCRIPTION("MCUIO driver for Digital IO shield");
MODULE_LICENSE("GPL v2");
