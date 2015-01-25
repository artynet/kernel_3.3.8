/*!
 * @section LICENSE
 * (C) Copyright 2014 Bosch Sensortec GmbH All Rights Reserved
 *
 * This software program is licensed subject to the GNU General
 * Public License (GPL).Version 2,June 1991,
 * available at http://www.fsf.org/copyleft/gpl.html
 *
 * @filename bme280.h
 * @date     2014/09/12
 * @id       "301409e"
 *
 * @brief
 * API Header
 *
 * Revision: 2.0.1(Pressure and Temperature compensation code revision is 1.1
 * and Humidity compensation code revision is 1.0)
 */
/**************************************************************************/

#ifndef __BME280_H__
#define __BME280_H__


/*******************************************************
* These definition uses for define the data types
********************************************************
*While porting the API please consider the following
*Please check the version of C standard
*Are you using Linux platform
*******************************************************/

/*********************************************************
* This definition uses for the Linux platform support
* Please use the types.h for your data types definitions
*********************************************************/
#ifdef	__KERNEL__

#include <linux/types.h>
#include <linux/math64.h>
#define BME280_64BITSUPPORT_PRESENT
#else /* ! __KERNEL__ */
/**********************************************************
* These definition uses for define the C
* standard version data types
***********************************************************/
# if !defined(__STDC_VERSION__)

/************************************************
 * compiler is C11 C standard
************************************************/
#if (__STDC_VERSION__ == 201112L)

/************************************************/
#include <stdint.h>
/************************************************/

/*unsigned integer types*/
#define	u8	uint8_t
#define	u16	uint16_t
#define	u32	uint32_t
#define	u64	uint64_t

/*signed integer types*/
#define	s8	int8_t
#define	s16	int16_t
#define	s32	int32_t
#define	s64	int64_t
#define BME280_64BITSUPPORT_PRESENT
/************************************************
 * compiler is C99 C standard
************************************************/

#elif (__STDC_VERSION__ == 199901L)

/* stdint.h is a C99 supported c library.
which is used to fixed the integer size*/
/************************************************/
#include <stdint.h>
/************************************************/

/*unsigned integer types*/
#define	u8	uint8_t
#define	u16	uint16_t
#define	u32	uint32_t
#define	u64	uint64_t

/*signed integer types*/
#define s8	int8_t
#define	s16	int16_t
#define	s32	int32_t
#define	s64	int64_t
#define BME280_64BITSUPPORT_PRESENT
/************************************************
 * compiler is C89 or other C standard
************************************************/

#else /*  !defined(__STDC_VERSION__) */
/*	By default it is defined as 32 bit machine configuration*/
/*	define the definition based on your machine configuration*/
/*	define the data types based on your
	machine/compiler/controller configuration*/
#define  MACHINE_32_BIT

/* If your machine support 16 bit
define the MACHINE_16_BIT*/
#ifdef MACHINE_16_BIT
#include <limits.h>
/*signed integer types*/
#define	s8	signed char
#define	s16	signed short int
#define	s32	signed long int

#if defined(LONG_MAX) && LONG_MAX == 0x7fffffffffffffffL
#define s64 long int
#define u64 unsigned long int
#define BME280_64BITSUPPORT_PRESENT
#elif defined(LLONG_MAX) && (LLONG_MAX == 0x7fffffffffffffffLL)
#define s64 long long int
#define u64 unsigned long long int
#define BME280_64BITSUPPORT_PRESENT
#else
#warning Either the correct data type for signed 64 bit integer \
could not be found, or 64 bit integers are not supported in your environment.
#warning The API will only offer 32 bit pressure calculation.This will \
slightly impede accuracy(noise of ~1 pascal RMS will be added to output).
#warning If 64 bit integers are supported on your platform, \
please set s64 manually and "#define(BME280_64BITSUPPORT_PRESENT)" manually.
#endif

/*unsigned integer types*/
#define	u8	unsigned char
#define	u16	unsigned short int
#define	u32	unsigned long int


/* If your machine support 32 bit
define the MACHINE_32_BIT*/
#elif defined MACHINE_32_BIT
/*signed integer types*/
#define	s8	signed char
#define	s16	signed short int
#define	s32	signed int
#define	s64	signed long long int

/*unsigned integer types*/
#define	u8	unsigned char
#define	u16	unsigned short int
#define	u32	unsigned int
#define	u64	unsigned long long int
#define BME280_64BITSUPPORT_PRESENT

/* If your machine support 64 bit
define the MACHINE_64_BIT*/
#elif defined MACHINE_64_BIT
/*signed integer types*/
#define	s8	signed char
#define	s16	signed short int
#define	s32	signed int
#define	s64	signed long int

/*unsigned integer types*/
#define	u8	unsigned char
#define	u16	unsigned short int
#define	u32	unsigned int
#define	u64	unsigned long int
#define BME280_64BITSUPPORT_PRESENT

#else
#warning If the above provided data types not supported\
define the data types manualy
#endif
#endif

/*** This else will execute for the compilers
 *	which are not supported the C standards
 *	Like C89/C99/C11***/
#else
/*	By default it is defined as 32 bit machine configuration*/
/*	define the definition based on your machine configuration*/
/*	define the data types based on your
	machine/compiler/controller configuration*/
#define  MACHINE_32_BIT

/* If your machine support 16 bit
define the MACHINE_16_BIT*/
#ifdef MACHINE_16_BIT
#include <limits.h>
/*signed integer types*/
#define	s8	signed char
#define	s16	signed short int
#define	s32	signed long int

#if defined(LONG_MAX) && LONG_MAX == 0x7fffffffffffffffL
#define s64 long int
#define u64 unsigned long int
#define BME280_64BITSUPPORT_PRESENT
#elif defined(LLONG_MAX) && (LLONG_MAX == 0x7fffffffffffffffLL)
#define s64 long long int
#define u64 unsigned long long int
#define BME280_64BITSUPPORT_PRESENT
#else
#warning Either the correct data type for signed 64 bit integer \
could not be found, or 64 bit integers are not supported in your environment.
#warning The API will only offer 32 bit pressure calculation.This will \
slightly impede accuracy(noise of ~1 pascal RMS will be added to output).
#warning If 64 bit integers are supported on your platform, \
please set s64 manually and "#define(BME280_64BITSUPPORT_PRESENT)" manually.
#endif

/*unsigned integer types*/
#define	u8	unsigned char
#define	u16	unsigned short int
#define	u32	unsigned long int


/* If your machine support 32 bit
define the MACHINE_32_BIT*/
#elif defined MACHINE_32_BIT
/*signed integer types*/
#define	s8	signed char
#define	s16	signed short int
#define	s32	signed int
#define	s64	signed long long int

/*unsigned integer types*/
#define	u8	unsigned char
#define	u16	unsigned short int
#define	u32	unsigned int
#define	u64	unsigned long long int
#define BME280_64BITSUPPORT_PRESENT


/* If your machine support 64 bit
define the MACHINE_64_BIT*/
#elif defined MACHINE_64_BIT
/*signed integer types*/
#define	s8	signed char
#define	s16	signed short int
#define	s32	signed int
#define	s64	signed long int

/*unsigned integer types*/
#define	u8	unsigned char
#define	u16	unsigned short int
#define	u32	unsigned int
#define	u64	unsigned long int
#define BME280_64BITSUPPORT_PRESENT

#else
#warning If the above provided data types not supported\
define the data types manualy
#endif
#endif
#endif


/* If the user wants to support floating point calculations, please set
	the following #define. If floating point
	calculation is not wanted or allowed
	(e.g. in Linux kernel), please do not set the define. */
#if 0
#define BME280_ENABLE_FLOAT
#endif
/* If the user wants to support 64 bit integer calculation
	(needed for optimal pressure accuracy) please set
	the following #define. If int64 calculation is not wanted
	(e.g. because it would include
	large libraries), please do not set the define. */
#define BME280_ENABLE_INT64

/** defines the return parameter type of the BME280_WR_FUNCTION */
#define BME280_BUS_WR_RETURN_TYPE s8

/**\brief links the order of parameters defined in
BME280_BUS_WR_PARAM_TYPE to function calls used inside the API*/
#define BME280_BUS_WR_PARAM_TYPES u8, u8,\
		u8 *, u8

/**\brief links the order of parameters defined in
BME280_BUS_WR_PARAM_TYPE to function calls used inside the API*/
#define BME280_BUS_WR_PARAM_ORDER(device_addr, register_addr,\
		register_data, wr_len)

/* never change this line */
#define BME280_BUS_WRITE_FUNC(device_addr, register_addr,\
register_data, wr_len) bus_write(device_addr, register_addr,\
		register_data, wr_len)

/**\brief defines the return parameter type of the BME280_RD_FUNCTION
*/
#define BME280_BUS_RD_RETURN_TYPE s8

/**\brief defines the calling parameter types of the BME280_RD_FUNCTION
*/
#define BME280_BUS_RD_PARAM_TYPES (u8, u8,\
		u8 *, u8)

/**\brief links the order of parameters defined in \
BME280_BUS_RD_PARAM_TYPE to function calls used inside the API
*/
#define BME280_BUS_RD_PARAM_ORDER (device_addr, register_addr,\
		register_data)

/* never change this line */
#define BME280_BUS_READ_FUNC(device_addr, register_addr,\
		register_data, rd_len)bus_read(device_addr, register_addr,\
		register_data, rd_len)

/**\brief defines the return parameter type of the BME280_DELAY_FUNCTION
*/
#define BME280_DELAY_RETURN_TYPE void

/**\brief defines the calling parameter types of the BME280_DELAY_FUNCTION
*/
#define BME280_DELAY_PARAM_TYPES u16

/* never change this line */
#define BME280_DELAY_FUNC(delay_in_msec)\
		delay_func(delay_in_msec)

#define BME280_GET_BITSLICE(regvar, bitname)\
		((regvar & bitname##__MSK) >> bitname##__POS)

#define BME280_SET_BITSLICE(regvar, bitname, val)\
((regvar & ~bitname##__MSK) | ((val<<bitname##__POS)&bitname##__MSK))


/* Constants */
#define BME280_NULL                          0
#define BME280_RETURN_FUNCTION_TYPE          s8

#define SHIFT_RIGHT_4_POSITION               4
#define SHIFT_LEFT_2_POSITION                2
#define SHIFT_LEFT_4_POSITION                4
#define SHIFT_LEFT_5_POSITION                5
#define SHIFT_LEFT_8_POSITION                8
#define SHIFT_LEFT_12_POSITION               12
#define SHIFT_LEFT_16_POSITION               16
#define BME280_Four_U8X                      4
#define BME280_Zero_U8X                      0
#define BME280_Eight_U8X                     8
#define	SUCCESS								((u8)0)

#define E_BME280_NULL_PTR                    ((s8)-127)
#define E_BME280_COMM_RES                    ((s8)-1)
#define E_BME280_OUT_OF_RANGE                ((s8)-2)

#define BME280_I2C_ADDRESS1                  0x76
#define BME280_I2C_ADDRESS2                  0x77

/* Sensor Specific constants */
#define BME280_SLEEP_MODE                    0x00
#define BME280_FORCED_MODE                   0x01
#define BME280_NORMAL_MODE                   0x03
#define BME280_SOFT_RESET_CODE               0xB6

#define BME280_STANDBYTIME_1_MS              0x00
#define BME280_STANDBYTIME_63_MS             0x01
#define BME280_STANDBYTIME_125_MS			 0x02
#define BME280_STANDBYTIME_250_MS            0x03
#define BME280_STANDBYTIME_500_MS            0x04
#define BME280_STANDBYTIME_1000_MS           0x05
#define BME280_STANDBYTIME_10_MS             0x06
#define BME280_STANDBYTIME_20_MS             0x07

#define BME280_OVERSAMPLING_SKIPPED          0x00
#define BME280_OVERSAMPLING_1X               0x01
#define BME280_OVERSAMPLING_2X               0x02
#define BME280_OVERSAMPLING_4X               0x03
#define BME280_OVERSAMPLING_8X               0x04
#define BME280_OVERSAMPLING_16X              0x05

/*#define BME280_ULTRALOWPOWER_MODE            0x00
#define BME280_LOWPOWER_MODE                 0x01
#define BME280_STANDARDRESOLUTION_MODE       0x02
#define BME280_HIGHRESOLUTION_MODE           0x03
#define BME280_ULTRAHIGHRESOLUTION_MODE      0x04

#define BME280_ULTRALOWPOWER_OSRS_P          BME280_OVERSAMPLING_1X
#define BME280_ULTRALOWPOWER_OSRS_T          BME280_OVERSAMPLING_1X

#define BME280_LOWPOWER_OSRS_P               BME280_OVERSAMPLING_2X
#define BME280_LOWPOWER_OSRS_T               BME280_OVERSAMPLING_1X

#define BME280_STANDARDRESOLUTION_OSRS_P     BME280_OVERSAMPLING_4X
#define BME280_STANDARDRESOLUTION_OSRS_T     BME280_OVERSAMPLING_1X

#define BME280_HIGHRESOLUTION_OSRS_P         BME280_OVERSAMPLING_8X
#define BME280_HIGHRESOLUTION_OSRS_T         BME280_OVERSAMPLING_1X

#define BME280_ULTRAHIGHRESOLUTION_OSRS_P    BME280_OVERSAMPLING_16X
#define BME280_ULTRAHIGHRESOLUTION_OSRS_T    BME280_OVERSAMPLING_2X */

#define BME280_STANDARD_OSRS_H			BME280_OVERSAMPLING_1X
#define BME280_FILTERCOEFF_OFF               0x00
#define BME280_FILTERCOEFF_2                 0x01
#define BME280_FILTERCOEFF_4                 0x02
#define BME280_FILTERCOEFF_8                 0x03
#define BME280_FILTERCOEFF_16                0x04

#define T_INIT_MAX                             20
		/* 20/16 = 1.25 ms */
#define T_MEASURE_PER_OSRS_MAX                          37
		/* 37/16 = 2.3125 ms*/

#define T_SETUP_PRESSURE_MAX                            10
		/* 10/16 = 0.625 ms */

#define T_SETUP_HUMIDITY_MAX                            10
		/* 10/16 = 0.625 ms */

/*calibration parameters */
#define BME280_DIG_T1_LSB_REG                0x88
#define BME280_DIG_T1_MSB_REG                0x89
#define BME280_DIG_T2_LSB_REG                0x8A
#define BME280_DIG_T2_MSB_REG                0x8B
#define BME280_DIG_T3_LSB_REG                0x8C
#define BME280_DIG_T3_MSB_REG                0x8D
#define BME280_DIG_P1_LSB_REG                0x8E
#define BME280_DIG_P1_MSB_REG                0x8F
#define BME280_DIG_P2_LSB_REG                0x90
#define BME280_DIG_P2_MSB_REG                0x91
#define BME280_DIG_P3_LSB_REG                0x92
#define BME280_DIG_P3_MSB_REG                0x93
#define BME280_DIG_P4_LSB_REG                0x94
#define BME280_DIG_P4_MSB_REG                0x95
#define BME280_DIG_P5_LSB_REG                0x96
#define BME280_DIG_P5_MSB_REG                0x97
#define BME280_DIG_P6_LSB_REG                0x98
#define BME280_DIG_P6_MSB_REG                0x99
#define BME280_DIG_P7_LSB_REG                0x9A
#define BME280_DIG_P7_MSB_REG                0x9B
#define BME280_DIG_P8_LSB_REG                0x9C
#define BME280_DIG_P8_MSB_REG                0x9D
#define BME280_DIG_P9_LSB_REG                0x9E
#define BME280_DIG_P9_MSB_REG                0x9F

#define BME280_DIG_H1_REG                    0xA1

#define BME280_DIG_H2_LSB_REG                0xE1
#define BME280_DIG_H2_MSB_REG                0xE2
#define BME280_DIG_H3_REG                    0xE3
#define BME280_DIG_H4_MSB_REG                0xE4
#define BME280_DIG_H5_LSB_H4_LSB_REG         0xE5
#define BME280_DIG_H5_MSB_REG                0xE6
#define BME280_DIG_H6_REG                    0xE7


#define BME280_CHIPID_REG                    0xD0  /*Chip ID Register */
#define BME280_RESET_REG                     0xE0  /*Softreset Register */
#define BME280_STATUS_REG                    0xF3  /*Status Register */
#define BME280_CTRLMEAS_REG                  0xF4  /*Ctrl Measure Register */
#define BME280_CTRLHUM_REG                   0xF2  /*Ctrl Humidity Register*/
#define BME280_CONFIG_REG                    0xF5  /*Configuration Register */
#define BME280_PRESSURE_MSB_REG              0xF7  /*Pressure MSB Register */
#define BME280_PRESSURE_LSB_REG              0xF8  /*Pressure LSB Register */
#define BME280_PRESSURE_XLSB_REG             0xF9  /*Pressure XLSB Register */
#define BME280_TEMPERATURE_MSB_REG           0xFA  /*Temperature MSB Reg */
#define BME280_TEMPERATURE_LSB_REG           0xFB  /*Temperature LSB Reg */
#define BME280_TEMPERATURE_XLSB_REG          0xFC  /*Temperature XLSB Reg */
#define BME280_HUMIDITY_MSB_REG              0xFD  /*Humidity MSB Reg */
#define BME280_HUMIDITY_LSB_REG              0xFE  /*Humidity LSB Reg */

/* Status Register */
#define BME280_STATUS_REG_MEASURING__POS           3
#define BME280_STATUS_REG_MEASURING__MSK           0x08
#define BME280_STATUS_REG_MEASURING__LEN           1
#define BME280_STATUS_REG_MEASURING__REG           BME280_STATUS_REG

#define BME280_STATUS_REG_IMUPDATE__POS            0
#define BME280_STATUS_REG_IMUPDATE__MSK            0x01
#define BME280_STATUS_REG_IMUPDATE__LEN            1
#define BME280_STATUS_REG_IMUPDATE__REG            BME280_STATUS_REG

/* Control Measurement Register */
#define BME280_CTRLMEAS_REG_OSRST__POS             5
#define BME280_CTRLMEAS_REG_OSRST__MSK             0xE0
#define BME280_CTRLMEAS_REG_OSRST__LEN             3
#define BME280_CTRLMEAS_REG_OSRST__REG             BME280_CTRLMEAS_REG

#define BME280_CTRLMEAS_REG_OSRSP__POS             2
#define BME280_CTRLMEAS_REG_OSRSP__MSK             0x1C
#define BME280_CTRLMEAS_REG_OSRSP__LEN             3
#define BME280_CTRLMEAS_REG_OSRSP__REG             BME280_CTRLMEAS_REG

#define BME280_CTRLMEAS_REG_MODE__POS              0
#define BME280_CTRLMEAS_REG_MODE__MSK              0x03
#define BME280_CTRLMEAS_REG_MODE__LEN              2
#define BME280_CTRLMEAS_REG_MODE__REG              BME280_CTRLMEAS_REG

#define BME280_CTRLHUM_REG_OSRSH__POS             0
#define BME280_CTRLHUM_REG_OSRSH__MSK             0x07
#define BME280_CTRLHUM_REG_OSRSH__LEN             3
#define BME280_CTRLHUM_REG_OSRSH__REG             BME280_CTRLHUM_REG

/* Configuration Register */
#define BME280_CONFIG_REG_TSB__POS                 5
#define BME280_CONFIG_REG_TSB__MSK                 0xE0
#define BME280_CONFIG_REG_TSB__LEN                 3
#define BME280_CONFIG_REG_TSB__REG                 BME280_CONFIG_REG

#define BME280_CONFIG_REG_FILTER__POS              2
#define BME280_CONFIG_REG_FILTER__MSK              0x1C
#define BME280_CONFIG_REG_FILTER__LEN              3
#define BME280_CONFIG_REG_FILTER__REG              BME280_CONFIG_REG

#define BME280_CONFIG_REG_SPI3WEN__POS             0
#define BME280_CONFIG_REG_SPI3WEN__MSK             0x01
#define BME280_CONFIG_REG_SPI3WEN__LEN             1
#define BME280_CONFIG_REG_SPI3WEN__REG             BME280_CONFIG_REG

/* Data Register */
#define BME280_PRESSURE_XLSB_REG_DATA__POS         4
#define BME280_PRESSURE_XLSB_REG_DATA__MSK         0xF0
#define BME280_PRESSURE_XLSB_REG_DATA__LEN         4
#define BME280_PRESSURE_XLSB_REG_DATA__REG         BME280_PRESSURE_XLSB_REG

#define BME280_TEMPERATURE_XLSB_REG_DATA__POS      4
#define BME280_TEMPERATURE_XLSB_REG_DATA__MSK      0xF0
#define BME280_TEMPERATURE_XLSB_REG_DATA__LEN      4
#define BME280_TEMPERATURE_XLSB_REG_DATA__REG      BME280_TEMPERATURE_XLSB_REG

#define BME280_WR_FUNC_PTR\
		s8 (*bus_write)(u8, u8,\
		u8 *, u8)

#define BME280_RD_FUNC_PTR\
		s8 (*bus_read)(u8, u8,\
		u8 *, u8)

#define BME280_MDELAY_DATA_TYPE u16

#define	BME280_3MS_DELAY	3
/** this structure holds all device specific calibration parameters */
struct bme280_calibration_param_t {
	u16 dig_T1;
	s16 dig_T2;
	s16 dig_T3;
	u16 dig_P1;
	s16 dig_P2;
	s16 dig_P3;
	s16 dig_P4;
	s16 dig_P5;
	s16 dig_P6;
	s16 dig_P7;
	s16 dig_P8;
	s16 dig_P9;

	u8  dig_H1;
	s16 dig_H2;
	u8  dig_H3;
	s16 dig_H4;
	s16 dig_H5;
	s8  dig_H6;

	s32 t_fine;
};
/** BME280 image registers data structure */
struct bme280_t {
	struct bme280_calibration_param_t cal_param;

	u8 chip_id;
	u8 dev_addr;

	u8 osrs_t;
	u8 osrs_p;
	u8 osrs_h;
	u8 ctrl_hum_reg;
	u8 ctrl_meas_reg;
	u8 config_reg;
	BME280_WR_FUNC_PTR;
	BME280_RD_FUNC_PTR;
	void(*delay_msec)(BME280_MDELAY_DATA_TYPE);
};
/* Function Declarations */
/*******************************************************************************
 *	Description: *//**\brief This function is used for initialize
 *	the bus read and bus write functions
 *  and assign the chip id and I2C address of the BME280 sensor
 *	chip id is read in the register 0xD0 bit from 0 to 7
 *
 *	 \param p_bme280 *bme280 structure pointer.
 *
 *	While changing the parameter of the p_bme280
 *	consider the following point:
 *	Changing the reference value of the parameter
 *	will changes the local copy or local reference
 *	make sure your changes will not
 *	affect the reference value of the parameter
 *	(Better case don't change the reference value of the parameter)
 *
 *
 *
 *
 * \return results of bus communication function
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BME280_RETURN_FUNCTION_TYPE bme280_init(struct bme280_t *bme280);
/*******************************************************************************
 *	Description: *//**\brief This API is used to read uncompensated temperature
 *	in the registers 0xFA, 0xFB and 0xFC
 *	0xFA -> MSB -> bit from 0 to 7
 *	0xFB -> LSB -> bit from 0 to 7
 *	0xFC -> LSB -> bit from 4 to 7
 *
 * \param s32 utemperature : Pointer holding
 *			the uncompensated temperature.
 *
 *
 *
 *  \return	results of bus communication function
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BME280_RETURN_FUNCTION_TYPE bme280_read_ut(s32 *utemperature);
/*******************************************************************************
 * Description: *//**\brief Reads actual temperature from uncompensated temperature
 *                    and returns the value in 0.01 degree Centigrade
 *                    Output value of "5123" equals 51.23 DegC.
 *
 *
 *
 *  \param s32 : value of uncompensated temperature
 *
 *  \return
 *			s32 : actual temperature
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
s32 bme280_compensate_T_int32(s32 adc_t);
/*******************************************************************************
 * Description: *//**\brief Reads actual temperature from uncompensated temperature
 *		and returns the value with 500LSB/DegC centred around 24 DegC
 *		output value of "5123" equals(5123/500)+24 = 34.246DegC
 *
 *
 *  \param s32 : value of uncompensated temperature
 *
 *
 *
 *  \return
 *			s16 : actual temperature
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
s16 bme280_compensate_T_int32_sixteen_bit_output(s32 adc_t);
/*******************************************************************************
 *	Description: *//**\brief This API is used to read uncompensated pressure.
 *	in the registers 0xF7, 0xF8 and 0xF9
 *	0xF7 -> MSB -> bit from 0 to 7
 *	0xF8 -> LSB -> bit from 0 to 7
 *	0xF9 -> LSB -> bit from 4 to 7
 *
 *
 *
 *	\param s32 upressure : Pointer holding the uncompensated pressure.
 *
 *
 *
 *	\return: results of bus communication function
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BME280_RETURN_FUNCTION_TYPE bme280_read_up(s32 *upressure);
/*******************************************************************************
 * Description: *//**\brief Reads actual pressure from uncompensated pressure
 *							and returns the value in Pascal(Pa)
 *                          Output value of "96386" equals 96386 Pa =
 *                          963.86 hPa = 963.86 millibar
 *
 *
 *
 *  \param s32 : value of uncompensated pressure
 *
 *
 *
 *  \return
 *			u32 : actual pressure
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
u32 bme280_compensate_P_int32(s32 adc_p);
/*******************************************************************************
 *	Description: *//**\brief This API is used to read uncompensated humidity.
 *	in the registers 0xF7, 0xF8 and 0xF9
 *	0xFD -> MSB -> bit from 0 to 7
 *	0xFE -> LSB -> bit from 0 to 7
 *
 *
 *
 *	\param s32 uhumidity : Pointer holding the uncompensated humidity.
 *
 *
 *
 *	\return: results of bus communication function
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BME280_RETURN_FUNCTION_TYPE bme280_read_uh(s32 *uhumidity);
/*******************************************************************************
 * Description: *//**\brief Reads actual humidity from
 *        uncompensated humidity
 *        and returns the value in %rH as unsigned 32bit integer
 *        in Q22.10 format(22 integer 10 fractional bits).
 *        An output value of 42313
 *        represents 42313 / 1024 = 41.321 %rH
 *
 *
 *
 *  \param s32 adc_h : value of uncompensated humidity
 *
 *
 *  \return
 *			u32 : actual relative humidity
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
u32 bme280_compensate_H_int32(s32 adc_h);
/*******************************************************************************
 * Description: *//**\brief Reads actual humidity from
 *        uncompensated humidity
 *        and returns the value in %rH as unsigned 16bit integer
 *        An output value of 42313
 *        represents 42313/512 = 82.643 %rH
 *
 *
 *
 *  \param s32 : value of uncompensated humidity
 *
 *
 *
 *  \return
 *			u16 : actual relative humidity
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
u16 bme280_compensate_H_int32_sixteen_bit_output(s32 adc_h);
/*******************************************************************************
 * Description: *//**\brief reads uncompensated pressure,temperature and humidity
 *
 *
 *
 *
 *  \param s32 upressure: Pointer holding the uncompensated pressure.
 *  \param s32 utemperature: Pointer holding
 *                    the uncompensated temperature.
 *  \param s32 uhumidity: Pointer holding the uncompensated humidity.
 *
 *
 *
 *  \return results of bus communication function
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BME280_RETURN_FUNCTION_TYPE bme280_read_uputuh(s32 *upressure,
s32 *utemperature, s32 *uhumidity);
/*******************************************************************************
 * Description: *//**\brief reads pressure, temperature and humidity.
 *
 *
 *
 *
 *	\param u32 pressure : Pointer holding
 *                          the compensated pressure.
 *	\param s32 temperature : Pointer holding
 *                      the compensated temperature.
 *	\param u32 humidity : Pointer holding
 *                         the compensated humidity.
 *
 *
 *  \return results of bus communication function
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BME280_RETURN_FUNCTION_TYPE bme280_read_pth(u32 *pressure,
s32 *temperature, u32 *humidity);
/*******************************************************************************
 *	Description: *//**\brief This API is used to
 *	calibration parameters used for calculation in the registers
 *	parameter	Register address	bit
 *	dig_T1			0x88/0x89		0 : 7 / 8: 15
 *	dig_T2			0x8A/0x8B		0 : 7 / 8: 15
 *	dig_T3			0x8C/0x8D		0 : 7 / 8: 15
 *	dig_P1			0x8E/0x8F		0 : 7 / 8: 15
 *	dig_P2			0x90/0x91		0 : 7 / 8: 15
 *	dig_P3			0x92/0x93		0 : 7 / 8: 15
 *	dig_P4			0x94/0x95		0 : 7 / 8: 15
 *	dig_P5			0x96/0x97		0 : 7 / 8: 15
 *	dig_P6			0x98/0x99		0 : 7 / 8: 15
 *	dig_P7			0x9A/0x9B		0 : 7 / 8: 15
 *	dig_P8			0x9C/0x9D		0 : 7 / 8: 15
 *	dig_P9			0x9E/0x9F		0 : 7 / 8: 15
 *	dig_H1				0xA1			0 : 7
 *	dig_H2			0xE1/0xE2		0 : 7 / 8: 15
 *	dig_H3				0xE3			0 : 7
 *
 *	\param:  None
 *
 *
 *
 *	\return: results of bus communication function
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BME280_RETURN_FUNCTION_TYPE bme280_get_calib_param(void);
/*******************************************************************************
 *	Description: *//**\brief This API is used to get
 *	the temperature oversampling setting in the register 0xF4
 *	bits from 5 to 7
 *
 *	bit					temperature oversampling
 *	0x00						Skipped
 *	0x01						BME280_OVERSAMPLING_1X
 *	0x02						BME280_OVERSAMPLING_2X
 *	0x03						BME280_OVERSAMPLING_4X
 *	0x04						BME280_OVERSAMPLING_8X
 *	0x05,0x06 and 0x07			BME280_OVERSAMPLING_16X
 *
 *
 *  \param u8 value : Pointer holding the osrs_t value
 *
 *
 *
 *  \return: results of bus communication function
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BME280_RETURN_FUNCTION_TYPE bme280_get_osrs_t(u8 *value);
/*******************************************************************************
 *	Description: *//**\brief This API is used to set
 *	the temperature oversampling in the register 0xF4
 *	bits from 5 to 7
 *
 *	bit					temperature oversampling
 *	0x00						Skipped
 *	0x01						BME280_OVERSAMPLING_1X
 *	0x02						BME280_OVERSAMPLING_2X
 *	0x03						BME280_OVERSAMPLING_4X
 *	0x04						BME280_OVERSAMPLING_8X
 *	0x05,0x06 and 0x07			BME280_OVERSAMPLING_16X
 *
 *
 *  \param u8 value : the osrs_t value
 *
 *
 *
 *  \return: results of bus communication function
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BME280_RETURN_FUNCTION_TYPE bme280_set_osrs_t(u8 value);
/*******************************************************************************
 *	Description: *//**\brief This API is used to get
 *	the pressure oversampling setting in the register 0xF4
 *	bits from 2 to 4
 *
 *	bit					pressure oversampling
 *	0x00						Skipped
 *	0x01						BME280_OVERSAMPLING_1X
 *	0x02						BME280_OVERSAMPLING_2X
 *	0x03						BME280_OVERSAMPLING_4X
 *	0x04						BME280_OVERSAMPLING_8X
 *	0x05,0x06 and 0x07			BME280_OVERSAMPLING_16X
 *
 *
 *  \param u8 value : Pointer holding the osrs_p value
 *
 *
 *
 *  \return: results of bus communication function
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BME280_RETURN_FUNCTION_TYPE bme280_get_osrs_p(u8 *value);
/*******************************************************************************
 *	Description: *//**\brief This API is used to set
 *	the pressure oversampling in the register 0xF4
 *	bits from 2 to 4
 *
 *	bit					pressure oversampling
 *	0x00						Skipped
 *	0x01						BME280_OVERSAMPLING_1X
 *	0x02						BME280_OVERSAMPLING_2X
 *	0x03						BME280_OVERSAMPLING_4X
 *	0x04						BME280_OVERSAMPLING_8X
 *	0x05,0x06 and 0x07			BME280_OVERSAMPLING_16X
 *
 *
 *  \param u8 value : the osrs_p value
 *
 *
 *
 *  \return: results of bus communication function
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BME280_RETURN_FUNCTION_TYPE bme280_set_osrs_p(u8 value);
/*******************************************************************************
 *	Description: *//**\brief This API is used to get
 *	the humidity oversampling setting in the register 0xF2
 *	bits from 0 to 2
 *
 *	bit					pressure oversampling
 *	0x00						Skipped
 *	0x01						BME280_OVERSAMPLING_1X
 *	0x02						BME280_OVERSAMPLING_2X
 *	0x03						BME280_OVERSAMPLING_4X
 *	0x04						BME280_OVERSAMPLING_8X
 *	0x05,0x06 and 0x07			BME280_OVERSAMPLING_16X
 *
 *
 *  \param u8 value : Pointer holding the osrs_h value
 *
 *
 *
 *  \return: results of bus communication function
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BME280_RETURN_FUNCTION_TYPE bme280_get_osrs_h(u8 *value);
/*******************************************************************************
 *	Description: *//**\brief This API is used to set
 *	the humidity oversampling setting in the register 0xF2
 *	bits from 0 to 2
 *
 *	bit					pressure oversampling
 *	0x00						Skipped
 *	0x01						BME280_OVERSAMPLING_1X
 *	0x02						BME280_OVERSAMPLING_2X
 *	0x03						BME280_OVERSAMPLING_4X
 *	0x04						BME280_OVERSAMPLING_8X
 *	0x05,0x06 and 0x07			BME280_OVERSAMPLING_16X
 *
 *
 *
 * The "BME280_CTRLHUM_REG_OSRSH" register sets the humidity
 * data acquisition options of the device.
 * changes to this registers only become effective after a write operation to
 * "BME280_CTRLMEAS_REG" register.
 * In the code automated reading and writing of "BME280_CTRLHUM_REG_OSRSH"
 * register first set the "BME280_CTRLHUM_REG_OSRSH" and then read and write
 * the "BME280_CTRLMEAS_REG" register in the function.
 *
 *
 *
 *  \param u8 value : Value of the humidity oversampling setting
 *
 *  \return: results of bus communication function
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BME280_RETURN_FUNCTION_TYPE bme280_set_osrs_h(u8 value);
/*******************************************************************************
 *	Description: *//**\brief This API used to get the
 *	Operational Mode from the sensor in the register 0xF4 bit 0 and 1
 *
 *
 *
 *	\param u8 *mode : Pointer holding the mode value.
 *	0x00			->	BME280_SLEEP_MODE
 *	0x01 and 0x02	->	BME280_FORCED_MODE
 *	0x03			->	BME280_NORMAL_MODE
 *
 *  \return : results of bus communication function
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BME280_RETURN_FUNCTION_TYPE bme280_get_mode(u8 *mode);
/*******************************************************************************
 *	Description: *//**\brief This API used to set the
 *	Operational Mode from the sensor in the register 0xF4 bit 0 and 1
 *
 *
 *
 *	\param u8 *mode : Pointer holding the mode value.
 *	0x00			->	BME280_SLEEP_MODE
 *	0x01 and 0x02	->	BME280_FORCED_MODE
 *	0x03			->	BME280_NORMAL_MODE
 *
 *
 *  \return : results of bus communication function
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BME280_RETURN_FUNCTION_TYPE bme280_set_mode(u8 mode);
/*******************************************************************************
 * Description: *//**\brief Used to reset the sensor
 * The value 0xB6 is written to the 0xE0 register the device is reset using the
 * complete power-on-reset procedure.
 * Softreset can be easily set using bme280_set_softreset().
 * Usage Hint : bme280_set_softreset()
 *
 *
 *  \param:	None
 *
 *
 *
 *  \return: result of bus communication function
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BME280_RETURN_FUNCTION_TYPE bme280_set_softreset(void);
/*******************************************************************************
 *	Description: *//**\brief This API used to set the sensor
 *	SPI mode(communication type) in the register 0xF5 bit 0
 *
 *
 *
 *	\param  u8 *enable_disable : Pointer holding the
 *	spi3 enable or disable state.
 *
 *
 *
 *  \return result of bus communication function
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BME280_RETURN_FUNCTION_TYPE bme280_get_spi3(u8 *enable_disable);
/*******************************************************************************
 *	Description: *//**\brief This API used to set the sensor
 *	SPI mode(communication type) in the register 0xF5 bit 0
 *
 *
 *
 *	\param  u8 enable_disable : the spi3 enable or disable value.
 *
 *
 *
 *  \return result of bus communication function
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BME280_RETURN_FUNCTION_TYPE bme280_set_spi3(u8 enable_disable);
/*******************************************************************************
 *	Description: *//**\brief This API is used to reads filter setting
 *	in the register 0xF5 bit 3 and 4
 *
 *
 *
 *	\param u8 *value : Pointer holding the filter value.
 *
 *	value			Filter coefficient
 *	0x00				Filter Off
 *	0x01				2
 *	0x02				4
 *	0x03				8
 *	0x04				16
 *
 *  \return result of bus communication function
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BME280_RETURN_FUNCTION_TYPE bme280_get_filter(u8 *value);
/*******************************************************************************
 *	Description: *//**\brief This API is used to set filter setting
 *	in the register 0xF5 bit 3 and 4
 *
 *
 *
 *	\param u8 value : The filter coefficient value
 *
 *	value			Filter coefficient
 *	0x00				Filter Off
 *	0x01				2
 *	0x02				4
 *	0x03				8
 *	0x04				16
 *
 *  \return result of bus communication function
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BME280_RETURN_FUNCTION_TYPE bme280_set_filter(u8 value);
/*******************************************************************************
 *	Description: *//**\brief This API used to Read the
 *	standby duration time from the sensor in the register 0xF5 bit 5 to 7
 *
 *	\param u8 *time : Pointer holding
 *                        the standby duration time value.
 *              0x00 - BME280_STANDBYTIME_1_MS
 *              0x01 - BME280_STANDBYTIME_63_MS
 *              0x02 - BME280_STANDBYTIME_125_MS
 *              0x03 - BME280_STANDBYTIME_250_MS
 *              0x04 - BME280_STANDBYTIME_500_MS
 *              0x05 - BME280_STANDBYTIME_1000_MS
 *              0x06 - BME280_STANDBYTIME_2000_MS
 *              0x07 - BME280_STANDBYTIME_4000_MS
 *
 *
 *  \return result of bus communication function
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BME280_RETURN_FUNCTION_TYPE bme280_get_standbydur(u8 *time);
/*******************************************************************************
 *	Description: *//**\brief This API used to write
 *	standby duration time from the sensor in the register 0xF5 bit 5 to 7
 *	Normal mode comprises an automated perpetual cycling between an (active)
 *	Measurement period and an (inactive) standby period.
 *	The standby time is determined by the contents of the register t_sb.
 *	Standby time can be set using BME280_STANDBYTIME_125_MS.
 *
 *	Usage Hint : bme280_set_standbydur(BME280_STANDBYTIME_125_MS)
 *
 *	\param u8 time : Value of the standby duration
 *              0x00 - BME280_STANDBYTIME_1_MS
 *              0x01 - BME280_STANDBYTIME_63_MS
 *              0x02 - BME280_STANDBYTIME_125_MS
 *              0x03 - BME280_STANDBYTIME_250_MS
 *              0x04 - BME280_STANDBYTIME_500_MS
 *              0x05 - BME280_STANDBYTIME_1000_MS
 *              0x06 - BME280_STANDBYTIME_2000_MS
 *              0x07 - BME280_STANDBYTIME_4000_MS
 *
 *
 *
 *
 *
 *  \return result of bus communication function
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BME280_RETURN_FUNCTION_TYPE bme280_set_standbydur(u8 time);
/*******************************************************************************
 * Description: *//**\brief Writes the working mode to the sensor
 *
 *
 *
 *
 *  \param u8 : Mode to be set
 *				0 -> BME280_ULTRALOWPOWER_MODE
 *				1 -> BME280_LOWPOWER_MODE
 *				2 -> BME280_STANDARDRESOLUTION_MODE
 *				3 -> BME280_HIGHRESOLUTION_MODE
 *				4 -> BME280_ULTRAHIGHRESOLUTION_MODE
 *  \return result of bus communication function
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
/*BME280_RETURN_FUNCTION_TYPE bme280_set_workmode(u8 mode);*/
/*******************************************************************************
 * Description: *//**\brief Read both uncompensated temperature,pressure and humidity
 *                                                       in forced mode
 *
 *
 *	\param s32 upressure: Pointer holding
 *                     the uncompensated pressure.
 *	\param s32 utemperature: Pointer holding
 *                    the uncompensated temperature.
 *	\param s32 uhumidity: Pointer holding
 *                     the uncompensated humidity.
 *
 *
 *  \return result of bus communication function
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BME280_RETURN_FUNCTION_TYPE bme280_get_forced_uputuh(s32 *upressure,
s32 *utemperature, s32 *uhumidity);
/*******************************************************************************
 * Description: *//**\brief This API gives data to the given register and
 *                          the data is written in the corresponding register
 *							address
 *
 *
 *
 *  \param u8 addr, u8 data, u8 len
 *          addr -> Address of the register
 *          data -> Data to be written to the register
 *          len  -> Length of the Data
 *
 *
 *
 *  \return result of bus communication function
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BME280_RETURN_FUNCTION_TYPE bme280_write_register(u8 addr,
u8 *data, u8 len);
/*******************************************************************************
 * Description: *//**\brief This API reads the data from the given register
 *							address
 *
 *
 *
 *  \param u8 addr, u8 *data, u8 len
 *         addr -> Address of the register
 *         data -> address of the variable, read value will be kept
 *         len  -> Length of the data
 *
 *
 *
 *
 *  \return result of bus communication function
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BME280_RETURN_FUNCTION_TYPE bme280_read_register(u8 addr,
u8 *data, u8 len);

#ifdef BME280_ENABLE_FLOAT
/*******************************************************************************
 * Description: *//**\brief Reads actual temperature from uncompensated temperature
 *							and returns the value in Degree centigrade
 *                          Output value of "51.23" equals 51.23 DegC.
 *
 *
 *
 *  \param signed long : value of uncompensated temperature
 *
 *
 *
 *  \return
 *			double : actual temperature in floating point
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
double bme280_compensate_T_double(s32 adc_t);
/*******************************************************************************
 * Description: *//**\brief Reads actual pressure from uncompensated pressure
 *							and returns pressure in Pa as double.
 *                          Output value of "96386.2"
 *                          equals 96386.2 Pa = 963.862 hPa.
 *
 *
 *  \param signed int : value of uncompensated pressure
 *
 *
 *
 *  \return
 *			double : actual pressure in floating point
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
double bme280_compensate_P_double(s32 adc_p);
/*******************************************************************************
 * Description: *//**\brief Reads actual humidity from uncompensated humidity
 *							and returns the value in relative humidity (%rH)
 *                          Output value of "42.12" equals 42.12 %rH
 *
 *  \param signed int : value of uncompensated humidity
 *
 *
 *
 *  \return
 *			double : actual humidity in floating point
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
double bme280_compensate_H_double(s32 adc_h);
#endif
#if defined(BME280_ENABLE_INT64) && defined(BME280_64BITSUPPORT_PRESENT)
/*******************************************************************************
 * Description: *//**\brief Reads actual pressure from uncompensated pressure
 *                          and returns the value in Pa as unsigned 32 bit
 *                          integer in Q24.8 format (24 integer bits and
 *                          8 fractional bits). Output value of "24674867"
 *                          represents 24674867 / 256 = 96386.2 Pa = 963.862 hPa
 *
 *
 *
 *  \param signed long : value of uncompensated temperature
 *
 *
 *
 *  \return
 *			unsigned long : actual pressure
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
u32 bme280_compensate_P_int64(s32 adc_p);
/*******************************************************************************
 * Description: *//**\brief Reads actual pressure from uncompensated pressure
 *		and returns the value in Pa.
 *		Output value of "12337434"
 *		represents 12337434 / 128 = 96386.2 Pa = 963.862 hPa
 *
 *
 *
 *  \param s32 : value of uncompensated temperature
 *
 *
 *  \return
 *			u32 : actual pressure
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
u32 bme280_compensate_P_int64_twentyfour_bit_output(s32 adc_p);
#endif
/*******************************************************************************
 * Description: *//**\brief Computing waiting time for sensor data read
 *
 *
 *
 *
 *  \param
 *			u8 : value of time
 *
 *
 *  \return result of bus communication function
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ****************************************************************************/
BME280_RETURN_FUNCTION_TYPE bme280_compute_wait_time(u8
*v_delaytime_u8r);
#endif
