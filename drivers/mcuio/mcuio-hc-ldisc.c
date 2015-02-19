/*
 * Copyright 2013 Dog Hunter SA
 *
 * Author Davide Ciminaghi
 * GNU GPLv2
 */

/* Line discipline based mcuio host controller */

#include <linux/module.h>
#include <linux/tty.h>
#include <linux/errno.h>
#include <linux/circ_buf.h>

#include <linux/mcuio.h>

#include <linux/mcuio-proto.h>
#include <linux/mcuio-hc.h>
#include <linux/mcuio-soft-hc.h>

#include "mcuio-internal.h"

/*
 * FIXME
 */
#define N_MCUIO 29

#define PSIZE ((int)sizeof(struct mcuio_packet))

struct ldisc_priv_data {
	struct device *dev;
	int blen;
	char buf[PSIZE];
};



static int mcuio_ldisc_shc_write(struct mcuio_soft_hc *shc,
				 const u8 *ptr, unsigned int len)
{
	int stat = 0, count;
	struct tty_struct *tty = shc->priv;

	for (count = 0; count < len; count += stat) {
		stat = tty->ops->write(tty, (char *)&ptr[count], len - count);
		if (stat <= 0)
			break;
	}
	return stat <= 0 ? stat : 0;
}

static const struct mcuio_soft_hc_ops ops = {
	.write = mcuio_ldisc_shc_write,
};

/*
 * Open ldisc: register an mcuio controller
 */
static int mcuio_ldisc_open(struct tty_struct *tty)
{
	struct ldisc_priv_data *priv;
	struct device *dev;
	dev = mcuio_add_soft_hc(NULL, &ops, tty);
	if (IS_ERR(dev))
		return (PTR_ERR(dev));
	priv = devm_kzalloc(dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;
	priv->dev = dev;
	tty->disc_data = priv;
	tty->receive_room = PSIZE;
	return 0;
}

static void mcuio_ldisc_close(struct tty_struct *tty)
{
	struct ldisc_priv_data *priv = tty->disc_data;
	if (!priv)
		return;
	if (!priv->dev)
		return;
	mcuio_del_hc_device(priv->dev);
	tty->disc_data = NULL;
}

static int mcuio_ldisc_hangup(struct tty_struct *tty)
{
	mcuio_ldisc_close(tty);
	return 0;
}

static void mcuio_ldisc_receive_buf(struct tty_struct *tty,
				    const unsigned char *cp,
				    char *fp, int count)
{
	struct ldisc_priv_data *priv = tty->disc_data;
	struct mcuio_hc_platform_data *plat;
	struct device *dev;
	int i, s, togo, done;

	if (!priv)
		return;
	dev = priv->dev;
	plat = dev_get_platdata(dev);
	if (!plat) {
		WARN_ON(1);
		return;
	}
	for (i = 0; fp && i < count; i++)
		if (fp[i]) {
			pr_err("%s: flags for char %d = 0x%02x\n",
			       __func__, i, (u8)fp[i]);
		}

	for (togo = count, done = 0; togo; priv->blen = 0) {
		s = min(togo, PSIZE - priv->blen);
		memcpy(&priv->buf[priv->blen], &cp[done], s);
		priv->blen += s;
		togo -= s;
		done += s;
		if (priv->blen < PSIZE)
			break;
		mcuio_soft_hc_push_chars(plat->data, priv->buf, PSIZE);
	}
}

static struct tty_ldisc_ops mcuio_ldisc = {
	.owner 		= THIS_MODULE,
	.magic 		= TTY_LDISC_MAGIC,
	.name 		= "mcuio",
	.open 		= mcuio_ldisc_open,
	.close	 	= mcuio_ldisc_close,
	.hangup	 	= mcuio_ldisc_hangup,
	.receive_buf	= mcuio_ldisc_receive_buf,
};

static int __init mcuio_ldisc_init(void)
{
	return tty_register_ldisc(N_MCUIO, &mcuio_ldisc);
}

static void __exit mcuio_ldisc_exit(void)
{
	tty_unregister_ldisc(N_MCUIO);
}

module_init(mcuio_ldisc_init);
module_exit(mcuio_ldisc_exit);

MODULE_LICENSE("GPL v2");
MODULE_ALIAS_LDISC(N_MCUIO);
MODULE_AUTHOR("Davide Ciminaghi, derived from slip ldisc implementation");
