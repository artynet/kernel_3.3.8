/*
 * Copyright 2014 Dog Hunter SA
 * Author: Aurelio Colosimo <aurelio@aureliocolosimo.it>
 *
 * GNU GPLv2 or later
 */

#define DEBUG

/* mcuio module for manual probe of shields */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/ctype.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/mcuio.h>
#include <linux/mcuio_ids.h>

#include "mcuio-internal.h"

struct shld_probe_info {
	char *name;
	unsigned int dev_nr;
	struct mcuio_device *mdev;
	struct mcuio_device_id mdev_id;
	int probed;
};

#define DH_SHLD(n, did, dnr) { \
	.name = n, \
	.dev_nr = dnr, \
	.mdev_id = { \
		.device = did, \
		.vendor = MCUIO_VENDOR_DOGHUNTER, \
		.class = MCUIO_CLASS_SHIELD, \
		.class_mask = 0xffffffff,\
	}\
}

struct shld_probe_info shld_list[] = {
	DH_SHLD("lucky", MCUIO_DEVICE_LUCKY_SHIELD, 32),
	DH_SHLD("digitalio", MCUIO_DEVICE_DIGITALIO_SHIELD, 33),
	DH_SHLD("dogoled", MCUIO_DEVICE_DOGOLED_SHIELD, 34),
	{.name = NULL},
};

int shld_register(struct shld_probe_info *info)
{
	int ret = -ENOMEM;
	struct mcuio_device *mdev;
	struct mcuio_device *hc;

	/* FIXME shld_register always acts on bus=0
	 * (only one bus is supported at present) */
	hc = mcuio_bus_find_hc(0);
	if (IS_ERR(hc))
		return -ENODEV;

	mdev = kzalloc(sizeof(*mdev), GFP_KERNEL);
	if (!mdev)
		return -ENOMEM;

	mdev->id = info->mdev_id;
	mdev->device = info->dev_nr;

	pr_debug("mcuio shield: device = 0x%04x, vendor = 0x%04x, "
		 "class = 0x%04x\n", mdev->id.device,
		 mdev->id.vendor, mdev->id.class);

	if (mcuio_device_register(mdev, NULL, &hc->dev) < 0) {
		dev_err(&hc->dev,
			"error registering device %u:%u.%u\n",
			hc->bus, mdev->device, mdev->fn);
		goto err0;
	}

	info->mdev = mdev;

	return 0;

err0:
	kfree(mdev);
	return ret;
}

void shld_unregister(struct shld_probe_info *info)
{
	mcuio_device_unregister(info->mdev);
	info->mdev = NULL;
}


static int shld_probe(const char *shld_name)
{
	struct shld_probe_info *i;
	for (i = shld_list; i->name; i++)
		if (sysfs_streq(shld_name, i->name) && !i->probed) {
			if (!shld_register(i))
				i->probed = 1;
			return 0;
		}
	return -EINVAL;
}

static void shld_remove(const char *shld_name)
{
	struct shld_probe_info *i;
	for (i = shld_list; i->name; i++)
		if (sysfs_streq(shld_name, i->name) && i->probed) {
			shld_unregister(i);
			i->probed = 0;
		}
}

static ssize_t show_shld_list(struct device *dev, struct device_attribute *attr,
			char *buf)
{
	struct shld_probe_info *i;
	buf[0] = '\0';
	for (i = shld_list; i->name; i++) {
		strcat(buf, i->name);
		strcat(buf, "\t");
	}
	strcat(buf, "\n");
	return strlen(buf);
}

static DEVICE_ATTR(shield_list, S_IRUSR, show_shld_list, NULL);

static ssize_t store_shld_register(struct device *dev,
				     struct device_attribute *attr,
				     const char *buf, size_t count)
{
	int ret;
	ret = shld_probe(buf);
	if (ret)
		dev_err(dev, "%s probe failed ret=%d\n", buf, ret);
	return count;
}

static DEVICE_ATTR(shield_register, S_IWUSR, NULL, store_shld_register);

static ssize_t store_shld_unregister(struct device *dev,
				       struct device_attribute *attr,
				       const char *buf, size_t count)
{
	shld_remove(buf);
	return count;
}

static DEVICE_ATTR(shield_unregister, S_IWUSR, NULL, store_shld_unregister);

static int __init shld_core_init(void)
{
	device_create_file(&mcuio_bus, &dev_attr_shield_list);
	device_create_file(&mcuio_bus, &dev_attr_shield_register);
	device_create_file(&mcuio_bus, &dev_attr_shield_unregister);
	return 0;
}

static void __exit shld_core_exit(void)
{
	device_remove_file(&mcuio_bus, &dev_attr_shield_list);
	device_remove_file(&mcuio_bus, &dev_attr_shield_register);
	device_remove_file(&mcuio_bus, &dev_attr_shield_unregister);
}

late_initcall(shld_core_init);
module_exit(shld_core_exit);

MODULE_AUTHOR("Aurelio Colosimo");
MODULE_DESCRIPTION("driver for MCUIO shields manual probe");
MODULE_LICENSE("GPL v2");
