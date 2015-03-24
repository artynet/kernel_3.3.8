/*
 *  Atheros AR71XX/AR724X/AR913X GPIO API support
 *
 *  Copyright (C) 2010-2011 Jaiganesh Narayanan <jnarayanan@atheros.com>
 *  Copyright (C) 2008-2011 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  Parts of this file are based on Atheros' 2.6.15/2.6.31 BSP
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/spinlock.h>
#include <linux/io.h>
#include <linux/ioport.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/irq.h>

#include <asm/mach-ath79/ar71xx_regs.h>
#include <asm/mach-ath79/ath79.h>
#include <asm/mach-ath79/irq.h>
#include "common.h"

struct ath79_gpio_data {
	int irq_base;
};

void __iomem *ath79_gpio_base;
EXPORT_SYMBOL_GPL(ath79_gpio_base);

static unsigned long ath79_gpio_count;
static DEFINE_SPINLOCK(ath79_gpio_lock);

static inline u32 __ath79_gpio_get_int_pending(void)
{
	return __raw_readl(ath79_gpio_base + AR71XX_GPIO_REG_INT_PENDING);
}

static void __ath79_gpio_set_value(unsigned gpio, int value)
{
	void __iomem *base = ath79_gpio_base;

	if (value)
		__raw_writel(1 << gpio, base + AR71XX_GPIO_REG_SET);
	else
		__raw_writel(1 << gpio, base + AR71XX_GPIO_REG_CLEAR);
}

static int __ath79_gpio_get_value(unsigned gpio)
{
	return (__raw_readl(ath79_gpio_base + AR71XX_GPIO_REG_IN) >> gpio) & 1;
}

static int ath79_gpio_get_value(struct gpio_chip *chip, unsigned offset)
{
	return __ath79_gpio_get_value(offset);
}

static void ath79_gpio_set_value(struct gpio_chip *chip,
				  unsigned offset, int value)
{
	__ath79_gpio_set_value(offset, value);
}

static int ath79_gpio_direction_input(struct gpio_chip *chip,
				       unsigned offset)
{
	void __iomem *base = ath79_gpio_base;
	unsigned long flags;

	spin_lock_irqsave(&ath79_gpio_lock, flags);

	__raw_writel(__raw_readl(base + AR71XX_GPIO_REG_OE) & ~(1 << offset),
		     base + AR71XX_GPIO_REG_OE);

	spin_unlock_irqrestore(&ath79_gpio_lock, flags);

	return 0;
}

static int ath79_gpio_direction_output(struct gpio_chip *chip,
					unsigned offset, int value)
{
	void __iomem *base = ath79_gpio_base;
	unsigned long flags;

	spin_lock_irqsave(&ath79_gpio_lock, flags);

	if (value)
		__raw_writel(1 << offset, base + AR71XX_GPIO_REG_SET);
	else
		__raw_writel(1 << offset, base + AR71XX_GPIO_REG_CLEAR);

	__raw_writel(__raw_readl(base + AR71XX_GPIO_REG_OE) | (1 << offset),
		     base + AR71XX_GPIO_REG_OE);

	spin_unlock_irqrestore(&ath79_gpio_lock, flags);

	return 0;
}

static int ar934x_gpio_direction_input(struct gpio_chip *chip, unsigned offset)
{
	void __iomem *base = ath79_gpio_base;
	unsigned long flags;

	spin_lock_irqsave(&ath79_gpio_lock, flags);

	__raw_writel(__raw_readl(base + AR71XX_GPIO_REG_OE) | (1 << offset),
		     base + AR71XX_GPIO_REG_OE);

	spin_unlock_irqrestore(&ath79_gpio_lock, flags);

	return 0;
}

static int ar934x_gpio_direction_output(struct gpio_chip *chip, unsigned offset,
					int value)
{
	void __iomem *base = ath79_gpio_base;
	unsigned long flags;

	spin_lock_irqsave(&ath79_gpio_lock, flags);

	if (value)
		__raw_writel(1 << offset, base + AR71XX_GPIO_REG_SET);
	else
		__raw_writel(1 << offset, base + AR71XX_GPIO_REG_CLEAR);

	__raw_writel(__raw_readl(base + AR71XX_GPIO_REG_OE) & ~(1 << offset),
		     base + AR71XX_GPIO_REG_OE);

	spin_unlock_irqrestore(&ath79_gpio_lock, flags);

	return 0;
}

static struct gpio_chip ath79_gpio_chip = {
	.label			= "ath79",
	.get			= ath79_gpio_get_value,
	.set			= ath79_gpio_set_value,
	.direction_input	= ath79_gpio_direction_input,
	.direction_output	= ath79_gpio_direction_output,
	.base			= 0,
};

static void __iomem *ath79_gpio_get_function_reg(void)
{
	u32 reg = 0;

	if (soc_is_ar71xx() ||
	    soc_is_ar724x() ||
	    soc_is_ar913x() ||
	    soc_is_ar933x())
		reg = AR71XX_GPIO_REG_FUNC;
	else if (soc_is_ar934x())
		reg = AR934X_GPIO_REG_FUNC;
	else
		BUG();

	return ath79_gpio_base + reg;
}

void ath79_gpio_function_enable(u32 mask)
{
	void __iomem *reg = ath79_gpio_get_function_reg();
	unsigned long flags;

	spin_lock_irqsave(&ath79_gpio_lock, flags);

	__raw_writel(__raw_readl(reg) | mask, reg);
	/* flush write */
	__raw_readl(reg);

	spin_unlock_irqrestore(&ath79_gpio_lock, flags);
}

void ath79_gpio_function_disable(u32 mask)
{
	void __iomem *reg = ath79_gpio_get_function_reg();
	unsigned long flags;

	spin_lock_irqsave(&ath79_gpio_lock, flags);

	__raw_writel(__raw_readl(reg) & ~mask, reg);
	/* flush write */
	__raw_readl(reg);

	spin_unlock_irqrestore(&ath79_gpio_lock, flags);
}

static void __iomem *ath79_gpio_get_function2_reg(void)
{
	u32 reg = 0;

	if (soc_is_ar71xx() ||
	    soc_is_ar724x() ||
	    soc_is_ar913x() ||
	    soc_is_ar933x())
		reg = AR71XX_GPIO_REG_FUNC_2;
	else
		BUG();

	return ath79_gpio_base + reg;
}


void ath79_gpio_function2_setup(u32 set, u32 clear)
{
	void __iomem *reg = ath79_gpio_get_function2_reg();
	unsigned long flags;

	spin_lock_irqsave(&ath79_gpio_lock, flags);

	__raw_writel((__raw_readl(reg) & ~clear) | set, reg);
	/* flush write */
	__raw_readl(reg);

	spin_unlock_irqrestore(&ath79_gpio_lock, flags);
}

void ath79_gpio_function_setup(u32 set, u32 clear)
{
	void __iomem *reg = ath79_gpio_get_function_reg();
	unsigned long flags;

	spin_lock_irqsave(&ath79_gpio_lock, flags);

	__raw_writel((__raw_readl(reg) & ~clear) | set, reg);
	/* flush write */
	__raw_readl(reg);

	spin_unlock_irqrestore(&ath79_gpio_lock, flags);
}

void __init ath79_gpio_output_select(unsigned gpio, u8 val)
{
	void __iomem *base = ath79_gpio_base;
	unsigned long flags;
	unsigned int reg;
	u32 t, s;

	BUG_ON(!soc_is_ar934x());

	if (gpio >= AR934X_GPIO_COUNT)
		return;

	reg = AR934X_GPIO_REG_OUT_FUNC0 + 4 * (gpio / 4);
	s = 8 * (gpio % 4);

	spin_lock_irqsave(&ath79_gpio_lock, flags);

	t = __raw_readl(base + reg);
	t &= ~(0xff << s);
	t |= val << s;
	__raw_writel(t, base + reg);

	/* flush write */
	(void) __raw_readl(base + reg);

	spin_unlock_irqrestore(&ath79_gpio_lock, flags);
}

static void __ath79_gpio_set_int_reg(unsigned long offset,
				     unsigned gpio, int value)
{
	void __iomem *base = ath79_gpio_base;
	unsigned long flags;
	int reg_val;

	spin_lock_irqsave(&ath79_gpio_lock, flags);

	reg_val = __raw_readl(base + offset);
	if (value)
		reg_val |= (1 << gpio);
	else
		reg_val &= (~(1 << gpio));


	__raw_writel(reg_val, base + offset);

	spin_unlock_irqrestore(&ath79_gpio_lock, flags);
}

static void __ath79_gpio_set_int_type(unsigned gpio, int value)
{
	__ath79_gpio_set_int_reg(AR71XX_GPIO_REG_INT_TYPE, gpio, value);
}

static void __ath79_gpio_set_int_polarity(unsigned gpio, int value)
{
	__ath79_gpio_set_int_reg(AR71XX_GPIO_REG_INT_POLARITY, gpio, value);
}

static int ar933x_irq_set_type(struct irq_data *d, unsigned int flow_type)
{
	int edge, polarity;
	struct irq_chip_generic *gc = irq_data_get_irq_chip_data(d);
	struct ath79_gpio_data *data = gc->private;
	unsigned gpio = d->irq - data->irq_base;

	switch (flow_type & IRQF_TRIGGER_MASK) {
	case IRQF_TRIGGER_HIGH:
		edge = 0;
		polarity = 1;
		break;
	case IRQF_TRIGGER_LOW:
		edge = 0;
		polarity = 0;
		break;
	case IRQF_TRIGGER_RISING:
		edge = 1;
		polarity = 1;
		break;
	case IRQF_TRIGGER_FALLING:
		edge = 1;
		polarity = 0;
		break;
	default:
		__ath79_gpio_set_int_reg(AR71XX_GPIO_REG_INT_MODE, gpio, 0);
		return -EINVAL;
	}

	/* The manual seems wrong !!! (edge and level modes seem inverted) */
	__ath79_gpio_set_int_type(gpio, !edge);
	__ath79_gpio_set_int_polarity(gpio, polarity);
	__ath79_gpio_set_int_reg(AR71XX_GPIO_REG_INT_MODE, gpio, 1);

	return 0;
}

static struct ath79_gpio_data gpio_data = {
	.irq_base = -1,
};

static int ar933x_gpio_to_irq(struct gpio_chip *chip, unsigned offset)
{
	return gpio_data.irq_base > 0 ?
		gpio_data.irq_base + offset : -ENXIO;
}

static irqreturn_t ath79_gpio_irq_handler(int irq, void *dev_id)
{
	struct ath79_gpio_data *data = dev_id;
	u32 pending;
	int i, __irq;

	pending = __ath79_gpio_get_int_pending();
	if (!pending)
		return IRQ_NONE;
	do {
		i = __ffs(pending);
		__irq = i + data->irq_base;
		generic_handle_irq(__irq);
		pending &= ~(1 << i);
	} while(pending);

	return IRQ_HANDLED;
}

static void ath79_setup_irqs(void)
{
	struct irq_chip_generic *gc;
	struct irq_chip_type *ct;

	gpio_data.irq_base = irq_alloc_descs(-1, 0, ath79_gpio_chip.ngpio, 0);
	if (gpio_data.irq_base < 0) {
		pr_err("cannot alloc descriptors for AR933x gpio irq "
		       "chip (%d)\n", gpio_data.irq_base);
		return;
	}

	gc = irq_alloc_generic_chip("GPIO-ATH79", 1, gpio_data.irq_base,
				    ath79_gpio_base, handle_level_irq);
	gc->private = &gpio_data;

	ct = gc->chip_types;
	ct->chip.irq_mask = irq_gc_mask_clr_bit;
	ct->chip.irq_unmask = irq_gc_mask_set_bit;
	ct->chip.irq_set_type = ar933x_irq_set_type;
	ct->regs.mask = AR71XX_GPIO_REG_INT_ENABLE;

	irq_setup_generic_chip(gc, IRQ_MSK(ath79_gpio_count), 0,
			       IRQ_NOREQUEST, 0);

	if (request_irq(ATH79_MISC_IRQ(2),
			ath79_gpio_irq_handler,
			0,
			"ath79-gpio",
			&gpio_data) < 0) {
		pr_err("error requesting gpio irq\n");
		return;
	}
	ath79_gpio_chip.to_irq = ar933x_gpio_to_irq;
}

void __init ath79_gpio_init(void)
{
	int err;

	if (soc_is_ar71xx())
		ath79_gpio_count = AR71XX_GPIO_COUNT;
	else if (soc_is_ar724x())
		ath79_gpio_count = AR724X_GPIO_COUNT;
	else if (soc_is_ar913x())
		ath79_gpio_count = AR913X_GPIO_COUNT;
	else if (soc_is_ar933x())
		ath79_gpio_count = AR933X_GPIO_COUNT;
	else if (soc_is_ar934x())
		ath79_gpio_count = AR934X_GPIO_COUNT;
	else if (soc_is_qca955x())
		ath79_gpio_count = QCA955X_GPIO_COUNT;
	else
		BUG();

	ath79_gpio_base = ioremap_nocache(AR71XX_GPIO_BASE, AR71XX_GPIO_SIZE);
	ath79_gpio_chip.ngpio = ath79_gpio_count;
	if (soc_is_ar934x() || soc_is_qca955x()) {
		ath79_gpio_chip.direction_input = ar934x_gpio_direction_input;
		ath79_gpio_chip.direction_output = ar934x_gpio_direction_output;
	}

	err = gpiochip_add(&ath79_gpio_chip);
	if (err) {
		panic("cannot add AR71xx GPIO chip, error=%d", err);
		return;
	}
	/* Irqs supported for ar933x and ar934x only at the moment */
	if (!soc_is_ar933x() && !soc_is_ar934x())
		return;

	ath79_setup_irqs();
}

int gpio_get_value(unsigned gpio)
{
	if (gpio < ath79_gpio_count)
		return __ath79_gpio_get_value(gpio);

	return __gpio_get_value(gpio);
}
EXPORT_SYMBOL(gpio_get_value);

void gpio_set_value(unsigned gpio, int value)
{
	if (gpio < ath79_gpio_count)
		__ath79_gpio_set_value(gpio, value);
	else
		__gpio_set_value(gpio, value);
}
EXPORT_SYMBOL(gpio_set_value);

int gpio_to_irq(unsigned gpio)
{
	return __gpio_to_irq(gpio);
}
EXPORT_SYMBOL(gpio_to_irq);

int irq_to_gpio(unsigned irq)
{
	if (irq < gpio_data.irq_base ||
	    irq > gpio_data.irq_base + ath79_gpio_chip.ngpio - 1)
		return -EINVAL;
	return irq - gpio_data.irq_base;
}
EXPORT_SYMBOL(irq_to_gpio);
