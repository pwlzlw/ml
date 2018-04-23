/*
 * MC3672.h
 *
 *  Created on: 14 Jun 2017
 *      Author: User
 */

 /*  Created on: 9 Jun 2017
 *      Edited by : Pawel Zalewski
 *      Ported from the manufacturer and edited to my needs (extra includes, function defines, some register
 *      defines and structs which were missing).
*/
#ifndef MC3672_H_
#define MC3672_H_

#include <stdio.h>
#include <string.h>
#include "comdef.h"
#include "threading_TI_SPI.h"
#include <ti/drivers/PIN.h>
#include <ti/drivers/pin/PINCC26XX.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <xdc/runtime/Timestamp.h>
#include <xdc/runtime/Types.h>

#define MC3672_CFG_BUS_SPI
#define MSGSIZE                               8
#define SELECT_ON                             0
#define SELECT_OFF                            1
#define MAX_SAMPLES                           192
#define MAX_S_HALF                            (MAX_SAMPLES/2)

/*******************************************************************************
 *** CONSTANT / DEFINE
 *******************************************************************************/
#define MC3672_RETCODE_SUCCESS                 (0)
#define MC3672_RETCODE_ERROR_BUS               (-1)
#define MC3672_RETCODE_ERROR_NULL_POINTER      (-2)
#define MC3672_RETCODE_ERROR_STATUS            (-3)
#define MC3672_RETCODE_ERROR_SETUP             (-4)
#define MC3672_RETCODE_ERROR_GET_DATA          (-5)
#define MC3672_RETCODE_ERROR_IDENTIFICATION    (-6)
#define MC3672_RETCODE_ERROR_NO_DATA           (-7)
#define MC3672_RETCODE_ERROR_WRONG_ARGUMENT    (-8)
#define MC3672_FIFO_DEPTH             32
#define MC3672_REG_MAP_SIZE             64



/*******************************************************************************
 *** CONSTANT / DEFINE
 *******************************************************************************/

//=============================================
#define MC3672_INTR_C_IPP_MODE_OPEN_DRAIN    (0x00)
#define MC3672_INTR_C_IPP_MODE_PUSH_PULL     (0x01)

#define MC3672_INTR_C_IAH_ACTIVE_LOW      (0x00)
#define MC3672_INTR_C_IAH_ACTIVE_HIGH     (0x02)


/*******************************************************************************
 *** Register Map
 *******************************************************************************/
//=============================================
#define MC3672_REG_EXT_STAT_1       (0x00)
#define MC3672_REG_EXT_STAT_2       (0x01)
#define MC3672_REG_XOUT_LSB         (0x02)
#define MC3672_REG_XOUT_MSB         (0x03)
#define MC3672_REG_YOUT_LSB         (0x04)
#define MC3672_REG_YOUT_MSB         (0x05)
#define MC3672_REG_ZOUT_LSB         (0x06)
#define MC3672_REG_ZOUT_MSB         (0x07)
#define MC3672_REG_STATUS_1         (0x08)
#define MC3672_REG_STATUS_2         (0x09)
#define MC3672_REG_FREG_2           (0x0E)
#define MC3672_REG_MODE_C           (0x10)
#define MC3672_REG_WAKE_C           (0x11)
#define MC3672_REG_SNIFF_C          (0x12)
#define MC3672_REG_SNIFFTH_C        (0x13)
#define MC3672_REG_SNIFF_CONF_C     (0x14)
#define MC3672_REG_RANGE_C          (0x15)
#define MC3672_REG_FIFO_C           (0x16)
#define MC3672_REG_INTR_C           (0x17)
#define MC3672_REG_PROD             (0x18)
#define MC3672_REG_POWER_MODE       (0x1C)
#define MC3672_REG_DMX              (0x20)
#define MC3672_REG_DMY              (0x21)
#define MC3672_REG_GAIN             (0x21)
#define MC3672_REG_DMZ              (0x22)
#define MC3672_REG_RESET            (0x24)
#define MC3672_REG_XOFFL            (0x2A)
#define MC3672_REG_XOFFH            (0x2B)
#define MC3672_REG_YOFFL            (0x2C)
#define MC3672_REG_YOFFH            (0x2D)
#define MC3672_REG_ZOFFL            (0x2E)
#define MC3672_REG_ZOFFH            (0x2F)
#define MC3672_REG_XGAIN            (0x30)
#define MC3672_REG_YGAIN            (0x31)
#define MC3672_REG_ZGAIN            (0x32)
#define MC3672_REG_OPT              (0x3B)
#define MC3672_REG_LOC_X            (0x3C)
#define MC3672_REG_LOC_Y            (0x3D)
#define MC3672_REG_LOT_dAOFSZ       (0x3E)
#define MC3672_REG_WAF_LOT          (0x3F)

#define MC3672_NULL_ADDR        (0)


struct MC3672_acc_t
{
    short XAxis;
    short YAxis;
    short ZAxis;
    float XAxis_g;
    float YAxis_g;
    float ZAxis_g;
} ;

typedef enum
{
    MC3672_GAIN_DEFAULT    = 0b00,
    MC3672_GAIN_4X         = 0b01,
    MC3672_GAIN_1X         = 0b10,
    MC3672_GAIN_NOT_USED   = 0b11,
}   MC3672_gain_t;

typedef enum
{
    MC3672_MODE_SLEEP      = 0b000,
    MC3672_MODE_STANDBY    = 0b001,
    MC3672_MODE_SNIFF      = 0b010,
    MC3672_MODE_CWAKE      = 0b101,
    MC3672_MODE_TRIG       = 0b111,
}   MC3672_mode_t;

typedef enum
{
    MC3672_RANGE_2G    = 0b000,
    MC3672_RANGE_4G    = 0b001,
    MC3672_RANGE_8G    = 0b010,
    MC3672_RANGE_16G   = 0b011,
    MC3672_RANGE_12G   = 0b100,
    MC3672_RANGE_END,
}   MC3672_range_t;

typedef enum
{
    MC3672_RESOLUTION_6BIT    = 0b000,
    MC3672_RESOLUTION_7BIT    = 0b001,
    MC3672_RESOLUTION_8BIT    = 0b010,
    MC3672_RESOLUTION_10BIT   = 0b011,
    MC3672_RESOLUTION_12BIT   = 0b100,
    MC3672_RESOLUTION_14BIT   = 0b101,  //(Do not select if FIFO enabled)
    MC3672_RESOLUTION_END,
}   MC3672_resolution_t;

typedef enum
{
    MC3672_CWAKE_SR_DEFAULT_54Hz = 0b0000,
    MC3672_CWAKE_SR_14Hz         = 0b0101,
    MC3672_CWAKE_SR_28Hz         = 0b0110,
    MC3672_CWAKE_SR_54Hz         = 0b0111,
    MC3672_CWAKE_SR_105Hz        = 0b1000,
    MC3672_CWAKE_SR_210Hz        = 0b1001,
    MC3672_CWAKE_SR_400Hz        = 0b1010,
    MC3672_CWAKE_SR_600Hz        = 0b1011,
    MC3672_CWAKE_SR_END,
}   MC3672_cwake_sr_t;

typedef enum
{
    MC3672_SNIFF_SR_DEFAULT_7Hz = 0b0000,
    MC3672_SNIFF_SR_0p4Hz       = 0b0001,
    MC3672_SNIFF_SR_0p8Hz       = 0b0010,
    MC3672_SNIFF_SR_1p5Hz       = 0b0011,
    MC3672_SNIFF_SR_7Hz         = 0b0100,
    MC3672_SNIFF_SR_14Hz        = 0b0101,
    MC3672_SNIFF_SR_28Hz        = 0b0110,
    MC3672_SNIFF_SR_54Hz        = 0b0111,
    MC3672_SNIFF_SR_105Hz       = 0b1000,
    MC3672_SNIFF_SR_210Hz       = 0b1001,
    MC3672_SNIFF_SR_400Hz       = 0b1010,
    MC3672_SNIFF_SR_600Hz       = 0b1011,
    MC3672_SNIFF_SR_END,
}   MC3672_sniff_sr_t;

typedef enum
{
    MC3672_FIFO_CONTROL_DISABLE = 0,
    MC3672_FIFO_CONTROL_ENABLE,
    MC3672_FIFO_CONTROL_END,
}   MC3672_fifo_control_t;

typedef enum
{
    MC3672_FIFO_MODE_NORMAL = 0,
    MC3672_FIFO_MODE_WATERMARK,
    MC3672_FIFO_MODE_END,
}   MC3672_fifo_mode_t;


typedef enum
{
    MC3672_LOWPOWER       = 0b000,
    MC3672_ULTRALOWPOWER  = 0b011,
    MC3672_PRECISION      = 0b100
}   MC3672_power_mode_t;


typedef struct
{
    unsigned char    bWAKE;              // Sensor wakes from sniff mode.
    unsigned char    bACQ;               // New sample is ready and acquired.
    unsigned char    bFIFO_EMPTY;        // FIFO is empty.
    unsigned char    bFIFO_FULL;         // FIFO is full.
    unsigned char    bFIFO_THRESHOLD;    // FIFO sample count is equal to or greater than the threshold count.
    unsigned char    bRESV;
    unsigned char    baPadding[2];
}   MC3672_InterruptEvent;


void MC3672_IRQ_Handler (PIN_Handle handle, PIN_Id pinId);

extern void MC3672_SetMode(MC3672_mode_t);
extern void MC3672_SetPowerMode(MC3672_power_mode_t);
extern void MC3672_SetRangeCtrl(MC3672_range_t);
extern void MC3672_SetResolutionCtrl(MC3672_resolution_t);
extern void MC3672_SetCWakeSampleRate(MC3672_cwake_sr_t);
extern void MC3672_SetSniffAGAIN(MC3672_gain_t);
extern void MC3672_SetWakeAGAIN(MC3672_gain_t);
extern MC3672_sniff_sr_t MC3672_GetSniffSampleRate(MC3672_sniff_sr_t);
extern MC3672_resolution_t MC3672_GetResolutionCtrl(void);
extern MC3672_range_t MC3672_GetRangeCtrl(void);
extern MC3672_cwake_sr_t MC3672_GetCWakeSampleRate(void);

extern bool MC3672_start(void);
extern void MC3672_reset(void);
extern void MC3672_stop(void);


extern void MC3672_enableFIFO(void);
extern void MC3672_processFIFO(uint8_t *);
extern void MC3672_clrIRQ(void);

extern void MC3672_checkSampleRate(char *);
extern void MC3672_checkRange(char *);
extern void MC3672_checkResolution(char *);
extern void MC3672_Probe(void);

#endif /* MC3672_H_ */
