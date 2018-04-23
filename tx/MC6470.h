/*
 * MC6470.h
 *
 *  Created on: 28 Jun 2017
 *      Author: User
 */
/*
 *   Created on: 18 Jul 2017
 *   Edit by: Pawel Zalewski - added function prototypes, structs and defines,
 *   the header file covered only the magnetomer.
 *
 *   Header acquired from :
 *   https://github.com/rock12/ALPS.L1.MP6.V2.19_CENON6580_WE_1_L_KERNEL/blob/master/drivers/misc/mediatek/magnetometer/mc6470/mc6470.h
 *
 *
 */


#ifndef MC6470_H_
#define MC6470_H_

#include <stdio.h>
#include <string.h>
#include "comdef.h"
#include <ti/drivers/I2C.h>
#include "board.h"
#include <ti/drivers/PIN.h>
#include <ti/drivers/pin/PINCC26XX.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include "threading_I2C.h"

/**************************
 *** mCube
 **************************/
#define MCMAG_DRV_RETCODE_OK  0

#define MCMAG_DRV_DISABLE 0
#define MCMAG_DRV_ENABLE  1

#define MCMAG_DRV_AXIS_X    0
#define MCMAG_DRV_AXIS_Y    1
#define MCMAG_DRV_AXIS_Z    2
#define MCMAG_DRV_AXES_NUM  3

#define MCMAG_DRV_DATA_LEN    6
#define MCMAG_DRV_MAX_MSG_LEN 10

#define MCMAG_DRV_DEVICE_NAME   "MCMAG"
#define MCMAG_DRV_DRIVER_VERSION  "1.1.7"

#define MCMAG_DRV_DAEMON_NAME   "mc6470d"

#define MCMAG_DRV_DEFAULT_SAMPLE_DELAY  (10)

/*******************************************************************************
 *** H/W CONFIGURATION
 *******************************************************************************/
/**************************
 *** SENSOR I2C ADDR
 **************************/
#define MCMAG_I2C_ADDR    (0x0C)
#define MCACC_I2C_ADDR    (0x4C)

/*======================================================================
=========================MAGNETOMETER===================================
======================================================================*/

/**************************************************************
 *** REG MAP (refer to MC64xx Spec.)
 **************************************************************/
#define MCMAG_REG_STB             0x0C
/* ================================ */
#define MCMAG_REG_MORE_INFO       0x0D
#define MCMAG_REG_INFO_VERSION    0x0D
#define MCMAG_REG_INFO_ALPS       0x0E
#define MCMAG_REG_WHO_I_AM        0x0F
/* ================================ */
#define MCMAG_REG_XOUT            0x10
#define MCMAG_REG_XOUT_L          0x10
#define MCMAG_REG_XOUT_H          0x11
#define MCMAG_REG_YOUT            0x12
#define MCMAG_REG_YOUT_L          0x12
#define MCMAG_REG_YOUT_H          0x13
#define MCMAG_REG_ZOUT            0x14
#define MCMAG_REG_ZOUT_L          0x14
#define MCMAG_REG_ZOUT_H          0x15
/* ================================ */
#define MCMAG_REG_STATUS          0x18
/* ================================ */
#define MCMAG_REG_CTRL1           0x1B
#define MCMAG_REG_CTRL2           0x1C
#define MCMAG_REG_CTRL3           0x1D
#define MCMAG_REG_CTRL4           0x1E
/* ================================ */
#define MCMAG_REG_XOFF            0x20
#define MCMAG_REG_XOFF_L          0x20
#define MCMAG_REG_XOFF_H          0x21
#define MCMAG_REG_YOFF            0x22
#define MCMAG_REG_YOFF_L          0x22
#define MCMAG_REG_YOFF_H          0x23
#define MCMAG_REG_ZOFF            0x24
#define MCMAG_REG_ZOFF_L          0x24
#define MCMAG_REG_ZOFF_H          0x25
/* ================================ */
#define MCMAG_REG_ITHR            0x26
#define MCMAG_REG_ITHR_L          0x26
#define MCMAG_REG_ITHR_H          0x27
/* ================================ */
#define MCMAG_REG_TEMP            0x31



/**************************************************************
 *** [REG SELF TEST: 0x0C]
 **************************************************************/
  #define MCMAG_SELFTEST_DATA          0x10
  #define MCMAG_SELFTEST_RESPONSE      0x55

/**************************************************************
 *** [REG STATUS: 0x18]
 **************************************************************/
    /**********************************************************
     *** DATA READY DETECTION
     **********************************************************/
    #define MCMAG_STATUS_DATA_READY_NOT_DETECTED    0x00
    #define MCMAG_STATUS_DATA_READY_DETECTED        0x40

    /**********************************************************
     *** DATA OVERRUN DETECTION
     **********************************************************/
    #define MCMAG_STATUS_DATA_OVERRUN_NOT_DETECTED    0x00
    #define MCMAG_STATUS_DATA_OVERRUN_DETECTED        0x20

/**************************************************************
 *** [REG CONTROL1: 0x1B]
 **************************************************************/
    /**********************************************************
     *** POWER MODE CONFIGURATION
     **********************************************************/
    #define MCMAG_CTRL1_POWER_MODE_STANDBY    0x00
    #define MCMAG_CTRL1_POWER_MODE_ACTIVE     0x80

    /**********************************************************
     *** OUTPUT DATA RATE CONFIGURATION
     **********************************************************/
    #define MCMAG_CTRL1_DAA_RATE_0p5Hz    0x00
    #define MCMAG_CTRL1_DAA_RATE_10Hz     0x08
    #define MCMAG_CTRL1_DAA_RATE_20Hz     0x10
    #define MCMAG_CTRL1_DAA_RATE_100Hz    0x18

    /**********************************************************
     *** STATE CONFIGURATION
     **********************************************************/
    #define MCMAG_CTRL1_STATE_NORMAL    0x00
    #define MCMAG_CTRL1_STATE_FORCED    0x02


/**************************************************************
 *** [REG CONTROL3: 0x1D]
 **************************************************************/
    #define MCMAG_CTRL3_ENABLE_SOFT_RESET  0x80

    #define MCMAG_CTRL3_SET_FORCE_STATE    0x40

/**************************************************************
 *** [REG CONTROL4: 0x1E]
 **************************************************************/
    #define MCMAG_CTRL4_MUST_DEFAULT_SETTING    0x80

    /**********************************************************
     *** DYNAMIC RANGE CONFIGURATION
     **********************************************************/
    #define MCMAG_CTRL4_DYNAMIC_RANGE_14bit    0x00
    #define MCMAG_CTRL4_DYNAMIC_RANGE_15bit    0x10

/*======================================================================
=========================ACCELEROMETER==================================
======================================================================*/

/**************************************************************
 *** REG MAP (refer to MC64xx Spec.)
 **************************************************************/
#define MCACC_REG_SR              0x03
#define MCACC_REG_OPSTAT          0x04
/* ================================ */
#define MCACC_REG_INTEN           0x06
#define MCACC_REG_MODE            0x07
#define MCACC_REG_SRTFR           0x08
#define MCACC_REG_TAPEN           0x09
#define MCACC_REG_TTTRX           0x0A
#define MCACC_REG_TTTRY           0x0B
#define MCACC_REG_TTTRZ           0x0C
#define MCACC_REG_XOUT_EX_L       0x0D
#define MCACC_REG_XOUT_EX_H       0x0E
#define MCACC_REG_YOUT_EX_L       0x0F
#define MCACC_REG_YOUT_EX_H       0x10
#define MCACC_REG_ZOUT_EX_L       0x11
#define MCACC_REG_ZOUT_EX_H       0x12
/* ================================ */
#define MCACC_REG_OUTCFG          0x20
#define MCACC_REG_XOFFL           0x21
#define MCACC_REG_XOFFH           0x22
#define MCACC_REG_YOFFL           0x23
#define MCACC_REG_YOFFH           0x24
#define MCACC_REG_ZOFFL           0x25
#define MCACC_REG_ZOFFH           0x26
#define MCACC_REG_XGAIN           0x27
#define MCACC_REG_YGAIN           0x28
#define MCACC_REG_ZGAIN           0x29

/*******************************************************************************
 *** S/W CONFIGURATION
 *******************************************************************************/
/**************************************************************
 *** MISC
 **************************************************************/
#define MCMAG_BUFFER_SIZE             64
#define MC6470_ACC_TAP_BITMASK 0b00111111

/*******************************************************************************
 *** DATA TYPE / ENUM
 *******************************************************************************/

/**************MAGNETOMETER**************/

typedef enum
{
    MC6470_MODE_STNDBY      = 0b00000000,
    MC6470_MODE_ACTIVE      = 0b10000000
}   MC6470_mag_mode_t;

typedef enum
{
  MC6470_DATA_RATE_05HZ   = 0b00000,
  MC6470_DATA_RATE_10HZ   = 0b01000,
  MC6470_DATA_RATE_20HZ   = 0b10000,
  MC6470_DATA_RATE_100HZ  = 0b11000
} MC6470_mag_data_rate_t;

typedef enum
{
  MC6470_STATE_NORMAL   = 0b00,
  MC6470_STATE_FORCED   = 0b10
} MC6470_mag_state_t;

typedef enum
{
  MCMAG_INTM_EN = 0b1000,
  MCMAG_INTM_DS = 0b0000
} MC6470_mag_int_t;

/**************ACCELEROMETER**************/

typedef enum
{
    MC6470_ACC_MODE_STNDBY      = 0b00,
    MC6470_ACC_MODE_WAKE        = 0b01
}   MC6470_acc_mode_t;

typedef enum
{
  MCACC_INTM_EN     = 0b10000000,
  MCACC_INTM_DS     = 0b00000000,
  MCACC_INTM_TIZNEN = 0b00100000,
  MCACC_INTM_TIZPEN = 0b00010000,
  MCACC_INTM_TIYNEN = 0b00001000,
  MCACC_INTM_TIYPEN = 0b00000100,
  MCACC_INTM_TIXNEN = 0b00000010,
  MCACC_INTM_TIXPEN = 0b00000001
} MC6470_acc_int_t;

typedef enum  {
  MCACC_SR_32HZ   = 0b0000,
  MCACC_SR_16HZ   = 0b0001,
  MCACC_SR_8HZ    = 0b0010,
  MCACC_SR_42HZ   = 0b0011,
  MCACC_SR_2HZ    = 0b0100,
  MCACC_SR_1HZ    = 0b0101,
  MCACC_SR_05HZ   = 0b0110,
  MCACC_SR_025HZ  = 0b0111,
  MCACC_SR_64HZ   = 0b1000,
  MCACC_SR_128HZ  = 0b1001,
  MCACC_SR_256HZ  = 0b1010
} MC6470_acc_sample_rate_t;

typedef enum {
  MCACC_TAP_EN   = 0b10000000,
  MCACC_TAP_DIS  = 0b00000000,
  MCACC_TAP_THR  = 0b01000000,
  MCACC_TAP_DUR  = 0b00000000,
  MCACC_TAP_ZNEN = 0b00100000,
  MCACC_TAP_ZPEN = 0b00010000,
  MCACC_TAP_YNEN = 0b00001000,
  MCACC_TAP_YPEN = 0b00000100,
  MCACC_TAP_XNEN = 0b00000010,
  MCACC_TAP_XPEN = 0b00000001
}MC6470_acc_tap_t;

typedef enum {
  MCACC_6bits = 0b000,
  MCACC_7bits = 0b001,
  MCACC_8bits = 0b010,
  MCACC_10bits =0b011,
  MCACC_12bits= 0b100,
  MCACC_14bits= 0b101
} MC6470_acc_resolution_t;

typedef enum {
  MCACC_2g = 0b0000,
  MCACC_4g = 0b0001,
  MCACC_8g = 0b0010,
  MCACC_16g =0b0011
} MC6470_acc_range_t;


typedef enum {
  MCTAP_MASK_X = 0b000011,
  MCTAP_MASK_Y = 0b001100,
  MCTAP_MASK_Z = 0b110000
} MC6470_tap_mask_t;

/**********FUNCITONS***************/

//magnetometer
extern bool MC6470_MAG_start(void);
extern void MC6470_MAG_readMgntmtr(uint8_t *);
extern void MC6470_MAG_setMode(MC6470_mag_mode_t);
extern void MC6470_MAG_setDataRate(MC6470_mag_data_rate_t);
extern void MC6470_MAG_setState(MC6470_mag_state_t);
extern void MC6470_MAG_setInterrupts(MC6470_mag_int_t);
extern uint8_t MC6470_MAG_readStatus(void);
extern void MC6470_MAG_startMeassure(void);

//accelerometer
extern void MC6470_ACC_start(void);
extern void MC6470_ACC_setMode(MC6470_acc_mode_t);
extern void MC6470_ACC_setInterrupts(uint8_t);
extern uint8_t MC6470_ACC_readStatus(void);
extern void MC6470_ACC_setSampleRate(MC6470_acc_sample_rate_t);
extern void MC6470_ACC_setResolution(MC6470_acc_resolution_t);
extern void MC6470_ACC_setRange(MC6470_acc_range_t);
extern void MC6470_ACC_setTapDetection(uint8_t);
extern void MC6470_ACC_setTapQuiet(uint8_t);
extern void MC6470_ACC_setTapDuration(uint8_t);
extern void MC6470_ACC_readAcc(uint8_t *);

void MC6470_ACC_IRQ_Handler (PIN_Handle handle, PIN_Id pinId);

void MC6470_MAG_IRQ_Handler (PIN_Handle handle, PIN_Id pinId);

void MC6470_findTap(uint8_t, uint8_t*);

#endif /* MC6470_H_ */



