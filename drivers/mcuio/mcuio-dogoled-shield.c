/*
 * Copyright 2014 Dog Hunter SA
 * Author: Aurelio Colosimo <aurelio@aureliocolosimo.it>
 *
 * GNU GPLv2 or later
 */

#define DEBUG

/* mcuio driver for Dog OLED shield */

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
#include <linux/platform_data/ssd1307.h>

#include <linux/mcuio.h>
#include <linux/mcuio_ids.h>
#include <linux/mcuio-proto.h>

#include "mcuio-internal.h"
#include "mcuio-shields.h"

static unsigned short ssd1307_addr = 0x3c;
static unsigned int ssd1307_rst = 120;
module_param(ssd1307_addr, ushort, 0644);
module_param(ssd1307_rst, uint, 0644);

static struct ssd1307_platform_data ssd1307_plat = {
	.type = SSD1307_TYPE_1306,
	.width = 128,
	.height = 64,
	.page_offset = 0,
	.pins_config = 0x12,
	.display_offset = 0,
};

static struct mcuio_shld_i2c_info i2c_lst[] = {
	MCUIO_SHLD_I2C_DEV("ssd1307fb", &ssd1307_addr, &ssd1307_plat, -1),
};

static int mcuio_dogoled_probe(struct mcuio_device *mdev)
{
	struct mcuio_shld_i2c_info *i;

	struct mcuio_shld_data *data;

	dev_dbg(&mdev->dev, "%s entered\n", __func__);

	data = devm_kzalloc(&mdev->dev, sizeof(*data), GFP_KERNEL);
	if (!data)
		return -ENOMEM;

	dev_set_drvdata(&mdev->dev, data);

	/* Apply module_param values to ssd1307 platform_data */
	ssd1307_plat.reset_gpio = ssd1307_rst;

	data->i2c_adap = mcuio_get_i2c_adapter(mdev);

	if (!data->i2c_adap) {
		dev_err(&mdev->dev, "error setting up i2c adapter\n");
		return -ENODEV;
	}

	data->i2c_info = i2c_lst;
	data->i2c_cnt = ARRAY_SIZE(i2c_lst);

	i = &data->i2c_info[0];
	i->info.addr = *i->paddr;
	i->i2c_client = i2c_new_device(data->i2c_adap, &i->info);

	dev_dbg(&mdev->dev, "%s returns ok\n", __func__);

	return 0;
}

static int mcuio_dogoled_remove(struct mcuio_device *mdev)
{
	struct mcuio_shld_i2c_info *i;
	struct mcuio_shld_data *data;

	data = dev_get_drvdata(&mdev->dev);
	i = &data->i2c_info[0];
	i2c_unregister_device(i->i2c_client);
	i->i2c_client = NULL;
	return 0;
}

static const struct mcuio_device_id dogoled_drv_ids[] = {
	{
		.vendor = MCUIO_VENDOR_DOGHUNTER,
		.device = MCUIO_DEVICE_DOGOLED_SHIELD,
		.class = MCUIO_CLASS_SHIELD,
		.class_mask = 0xffff,
	},
	/* Terminator */
	{
		.device = MCUIO_NO_DEVICE,
		.class = MCUIO_CLASS_UNDEFINED,
	},
};

static struct mcuio_driver mcuio_dogoled_driver = {
	.driver = {
		.name = "mcuio-dogoled-shield",
	},
	.id_table = dogoled_drv_ids,
	.probe = mcuio_dogoled_probe,
	.remove = mcuio_dogoled_remove,
};

static int __init mcuio_dogoled_init(void)
{
	return mcuio_driver_register(&mcuio_dogoled_driver, THIS_MODULE);
}

static void __exit mcuio_dogoled_exit(void)
{
	return mcuio_driver_unregister(&mcuio_dogoled_driver);
}

subsys_initcall(mcuio_dogoled_init);
module_exit(mcuio_dogoled_exit);

MODULE_AUTHOR("Aurelio Colosimo");
MODULE_DESCRIPTION("MCUIO driver for Dog OLED shield");
MODULE_LICENSE("GPL v2");
