/*
 * Copyright 2014 Dog Hunter SA
 * Author: Aurelio Colosimo <aurelio@aureliocolosimo.it>
 * Originally copied from mcuio-js-shield.c,
 *      by Davide Ciminaghi <ciminaghi@gnudd.com>
 *
 * GNU GPLv2 or later
 */

#define DEBUG

/* mcuio driver for Lucky shield */

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
#include <linux/hid.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/platform_data/ssd1307.h>
#include <linux/i2c/pca953x.h>

#include <linux/mcuio.h>
#include <linux/mcuio_ids.h>
#include <linux/mcuio-proto.h>

#include "mcuio-internal.h"
#include "mcuio-shields.h"

static unsigned int pca9555_base = 200;
static unsigned short pca9555_addr = 0x20;
static unsigned short mpl3115_addr = 0x60;
static unsigned short mag3110_addr = 0x0e;
static unsigned short sht21_addr = 0x40;
static unsigned short ssd1307_addr = 0x3c;
static unsigned short mma8491_addr = 0x55;
static unsigned int mma8491_rst = 200;
static unsigned short bme280_addr = 0x77;
module_param(pca9555_base, uint, 0644);
module_param(pca9555_addr, ushort, 0644);
module_param(mpl3115_addr, ushort, 0644);
module_param(mag3110_addr, ushort, 0644);
module_param(sht21_addr, ushort, 0644);
module_param(ssd1307_addr, ushort, 0644);
module_param(mma8491_addr, ushort, 0644);
module_param(mma8491_rst, uint, 0644);
module_param(bme280_addr, uint, 0644);

static struct ssd1307_platform_data ssd1307_plat = {
	.type = SSD1307_TYPE_1306,
	.width = 128,
	.height = 64,
	.page_offset = 0,
	.pins_config = 0x12,
	.display_offset = 0,
};

static struct pca953x_platform_data pca9555_plat;

static struct mcuio_shld_i2c_info i2c_lst[] = {
	MCUIO_SHLD_I2C_DEV("pca9555", &pca9555_addr, &pca9555_plat, 122),
	MCUIO_SHLD_I2C_DEV("mpl3115", &mpl3115_addr, NULL, -1),
	MCUIO_SHLD_I2C_DEV("mma8491", &mma8491_addr, &mma8491_rst, -1),
	MCUIO_SHLD_I2C_DEV("mag3110", &mag3110_addr, NULL, -1),
	MCUIO_SHLD_I2C_DEV("sht21", &sht21_addr, NULL, -1),
	MCUIO_SHLD_I2C_DEV("ssd1307fb", &ssd1307_addr, &ssd1307_plat, -1),
	MCUIO_SHLD_I2C_DEV("bme280", &bme280_addr, NULL, -1),
};

static int mcuio_lucky_probe(struct mcuio_device *mdev)
{
	struct mcuio_shld_i2c_info *i;
	int cnt;

	struct mcuio_shld_data *data;

	dev_dbg(&mdev->dev, "%s entered\n", __func__);

	data = devm_kzalloc(&mdev->dev, sizeof(*data), GFP_KERNEL);
	if (!data)
		return -ENOMEM;

	dev_set_drvdata(&mdev->dev, data);

	/* Apply module_param values to platform_data when needed */
	pca9555_plat.gpio_base = pca9555_base;
	ssd1307_plat.reset_gpio = pca9555_base + 15;

	data->i2c_adap = mcuio_get_i2c_adapter(mdev);

	if (!data->i2c_adap) {
		dev_err(&mdev->dev, "error setting up i2c adapter\n");
		return -ENODEV;
	}

	data->i2c_info = i2c_lst;
	data->i2c_cnt = ARRAY_SIZE(i2c_lst);

	/* Register all devices in Lucky shield */
	for (cnt = 0; cnt < data->i2c_cnt; cnt++) {
		i = &data->i2c_info[cnt];
		i->info.addr = *i->paddr;
		i->info.irq = (i->gpio_irq >= 0) ?
			gpio_to_irq(i->gpio_irq) : 0;

		i->i2c_client = i2c_new_device(data->i2c_adap, &i->info);

		if (!i->i2c_client)
			dev_err(&mdev->dev,
				"i2c_new_device %s failed\n", i->info.type);
	}

	put_device(&data->i2c_adap->dev);

	dev_dbg(&mdev->dev, "%s returns ok\n", __func__);

	return 0;
}

static int mcuio_lucky_remove(struct mcuio_device *mdev)
{
	struct mcuio_shld_i2c_info *i;
	struct mcuio_shld_data *data;

	data = dev_get_drvdata(&mdev->dev);

	/* Unregister all devices in Lucky shield, in reverse order as
	 * they were registered */
	for (i = &data->i2c_info[data->i2c_cnt - 1];
	     data->i2c_cnt; i--, data->i2c_cnt--) {
		if (i->i2c_client) {
			i2c_unregister_device(i->i2c_client);
			i->i2c_client = NULL;
		}
	}

	return 0;
}

static const struct mcuio_device_id lucky_drv_ids[] = {
	{
		.vendor = MCUIO_VENDOR_DOGHUNTER,
		.device = MCUIO_DEVICE_LUCKY_SHIELD,
	},
	/* Terminator */
	{
		.device = MCUIO_NO_DEVICE,
		.class = MCUIO_CLASS_UNDEFINED,
	},
};

static struct mcuio_driver mcuio_lucky_driver = {
	.driver = {
		.name = "mcuio-lucky-shield",
	},
	.id_table = lucky_drv_ids,
	.probe = mcuio_lucky_probe,
	.remove = mcuio_lucky_remove,
};


static int i2cdev_notifier_call(struct notifier_block *nb, unsigned long action,
			 void *data)
{
	struct mcuio_shld_i2c_info *i;
	struct i2c_client *c = to_i2c_client(data);
	int cnt;

	if (action == BUS_NOTIFY_DEL_DEVICE) {
		for (cnt = 0; cnt < ARRAY_SIZE(i2c_lst); cnt++) {
			i = &i2c_lst[cnt];
			if (i->i2c_client == to_i2c_client(data))
				i->i2c_client = NULL;
		}
	}
	return 0;
}

static struct notifier_block i2cdev_notifier = {
	.notifier_call = i2cdev_notifier_call,
};

static int __init mcuio_lucky_init(void)
{
	int ret;

	/* Keep track of devices which will be added or removed later */
	ret = bus_register_notifier(&i2c_bus_type, &i2cdev_notifier);
	if (ret)
		return ret;

	return mcuio_driver_register(&mcuio_lucky_driver, THIS_MODULE);
}

static void __exit mcuio_lucky_exit(void)
{
	bus_unregister_notifier(&i2c_bus_type, &i2cdev_notifier);
	return mcuio_driver_unregister(&mcuio_lucky_driver);
}

subsys_initcall(mcuio_lucky_init);
module_exit(mcuio_lucky_exit);

MODULE_AUTHOR("Aurelio Colosimo");
MODULE_DESCRIPTION("MCUIO driver for Lucky shield");
MODULE_LICENSE("GPL v2");
