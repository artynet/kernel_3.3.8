/*!
 * @section LICENSE
 * (C) Copyright 2013 Bosch Sensortec GmbH All Rights Reserved
 *
 * This software program is licensed subject to the GNU General
 * Public License (GPL).Version 2,June 1991,
 * available at http://www.fsf.org/copyleft/gpl.html
 *
 * @filename bs_log.h
 * @date     "Fri Dec 13 14:33:28 2013 +0800"
 * @id       "ba28356"
 *
 * @brief
 * The head file of BOSCH SENSOR LOG
*/

#ifndef __BS_LOG_H
#define __BS_LOG_H

#include <linux/kernel.h>

/*! @defgroup bme280_core_src
 *  @brief The core code of BME280 device driver
 @{*/
/*! ERROR LOG LEVEL */
#define LOG_LEVEL_E 3
/*! NOTICE LOG LEVEL */
#define LOG_LEVEL_N 5
/*! INFORMATION LOG LEVEL */
#define LOG_LEVEL_I 6
/*! DEBUG LOG LEVEL */
#define LOG_LEVEL_D 7

#ifndef LOG_LEVEL
/*! LOG LEVEL DEFINITION */
#define LOG_LEVEL LOG_LEVEL_N
#endif

#ifndef MODULE_TAG
/*! MODULE TAG DEFINITION */
#define MODULE_TAG "bme280: "
#endif

#if (LOG_LEVEL >= LOG_LEVEL_E)
/*! print error message */
#define PERR(fmt, args...) \
	printk(MODULE_TAG "[E] " fmt "\n", ##args)
#else
/*! invalid message */
#define PERR(fmt, args...)
#endif

#if (LOG_LEVEL >= LOG_LEVEL_N)
/*! print notice message */
#define PNOTICE(fmt, args...) \
	printk(MODULE_TAG "[N] " fmt "\n", ##args)

#else
/*! invalid message */
#define PNOTICE(fmt, args...)
#endif

#if (LOG_LEVEL >= LOG_LEVEL_I)
/*! print information message */
#define PINFO(fmt, args...) \
	printk(MODULE_TAG "[I] " fmt "\n", ##args)
#else
/*! invalid message */
#define PINFO(fmt, args...)
#endif

#if (LOG_LEVEL >= LOG_LEVEL_D)
/*! print debug message */
#define PDEBUG(fmt, args...) \
	printk(MODULE_TAG "[D] " "<%s><%d> " fmt "\n", \
		__func__, __LINE__, ##args)
#else
/*! invalid message */
#define PDEBUG(fmt, args...)
#endif

#endif/*__BS_LOG_H*/
/*@}*/
