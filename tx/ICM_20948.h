/*
 * ICM_20948.h
 *
 *  Created on: 7 Oct 2017
 *      Author: Pawel Zalewski
 */
/*  Created on: 7 Oct 2017
 *  Edited by : Pawel Zalewski
 *  The header file with the register addresses defines comes from IVSENSE library, I have edited it
 *  to include my function prototypes and changed some of the defines.
 *  Can be obtained from (requires registration and is delivered via email): *
 *  https://www.invensense.com/products/motion-tracking/9-axis/icm-20948/
 */
#ifndef ICM_20948_H_
#define ICM_20948_H_

#include <stdio.h>
#include <string.h>
#include "comdef.h"
#include "threading_TI_SPI.h"
#include <ti/drivers/PIN.h>
#include <ti/drivers/pin/PINCC26XX.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include "ICM_DMP3_image.h"
#include <xdc/runtime/Timestamp.h>
#include <xdc/runtime/Types.h>
//#include "CS_driver.h"
#define HW_ICM20948     0x02
#define MEMS_CHIP       HW_ICM20948

#define MSEC_PER_SEC 1000
#define NSEC_PER_MSEC 1000000
#define NSEC_PER_SEC NSEC_PER_MSEC * MSEC_PER_SEC

#define FIFO_DIVIDER    19

#define REG_BANK_0      0x00
#define REG_BANK_1      0x01

#define BANK_0                  (0 << 7)
#define BANK_1                  (0 << 7)
#define BANK_2                  (0 << 7)
#define BANK_3                  (0 << 7)


/* user bank register maps */

/* bank 0 register map */
#define REG_WHO_AM_I            (BANK_0 | 0x00)
#define REG_LPF                 (BANK_0 | 0x01)

#define REG_USER_CTRL           (BANK_0 | 0x03)

#define REG_LP_CONFIG           (BANK_0 | 0x05)

#define BIT_I2C_MST_CYCLE               0x40
#define BIT_ACCEL_CYCLE                 0x20
#define BIT_GYRO_CYCLE                  0x10

#define REG_PWR_MGMT_1          (BANK_0 | 0x06)
#define BIT_H_RESET                     0x80
#define BIT_SLEEP                       0x40
#define BIT_TMP_DIS                     0b00001000
#define BIT_LP_EN                       0x20
#define BIT_CLK_PLL                     0x01

#define REG_PWR_MGMT_2          (BANK_0 | 0x07)
#define BIT_PWR_PRESSURE_STBY           0x40
#define BIT_PWR_ACCEL_STBY              0x38
#define BIT_PWR_GYRO_STBY               0x07
#define BIT_PWR_ALL_OFF                 0x7f
#define BIT_GYRO_ON_ACC_OFF             0b00111000

#define REG_INT_PIN_CFG         (BANK_0 | 0x0F)
#define BIT_INT_AL                      0b10000000
#define BIT_INT_PP                      0b00000000
#define BIT_INT_NP                      0b01000000
#define BIT_INT_CLEAR                   0b00010000
#define BIT_INT_LATCH_EN                0x20
#define BIT_BYPASS_EN                   0x02

#define REG_INT_ENABLE          (BANK_0 | 0x10)
#define BIT_DMP_INT_EN                  0x02

#define REG_INT_ENABLE_1        (BANK_0 | 0x11)
#define BIT_DATA_RDY_3_EN               0x08
#define BIT_DATA_RDY_2_EN               0x04
#define BIT_DATA_RDY_1_EN               0x02
#define BIT_DATA_RDY_0_EN               0b00000001

#define REG_INT_ENABLE_2        (BANK_0 | 0x12)
#define BIT_FIFO_OVERFLOW_EN_0          0x1

#define REG_INT_ENABLE_3        (BANK_0 | 0x13)

#define REG_DMP_INT_STATUS      (BANK_0 | 0x18)
#define BIT_WAKE_ON_MOTION_INT          0x08
#define BIT_MSG_DMP_INT                 0x0002
#define BIT_MSG_DMP_INT_0               0x0100  // CI Command

#define BIT_MSG_DMP_INT_2               0x0200  // CIM Command - SMD
#define BIT_MSG_DMP_INT_3               0x0400  // CIM Command - Pedometer

#define BIT_MSG_DMP_INT_4               0x1000  // CIM Command - Pedometer binning
#define BIT_MSG_DMP_INT_5               0x2000  // CIM Command - Bring To See Gesture
#define BIT_MSG_DMP_INT_6               0x4000  // CIM Command - Look To See Gesture

#define REG_INT_STATUS          (BANK_0 | 0x19)
#define BIT_DMP_INT                     0x02

#define REG_INT_STATUS_1        (BANK_0 | 0x1A)
#define REG_INT_STATUS_2        (BANK_0 | 0x1B)

#define REG_SINGLE_FIFO_PRIORITY_SEL        (BANK_0 | 0x26)

#define REG_GYRO_XOUT_H_SH      (BANK_0 | 0x33)

#define REG_TEMPERATURE         (BANK_0 | 0x39)
#define REG_TEMP_CONFIG         (BANK_0 | 0x53)

#define REG_EXT_SLV_SENS_DATA_00 (BANK_0 | 0x3B)
#define REG_EXT_SLV_SENS_DATA_08 (BANK_0 | 0x43)
#define REG_EXT_SLV_SENS_DATA_09 (BANK_0 | 0x44)
#define REG_EXT_SLV_SENS_DATA_10 (BANK_0 | 0x45)

#define REG_FIFO_EN             (BANK_0 | 0x66)
#define BIT_SLV_0_FIFO_EN               0x01

#define REG_FIFO_EN_2           (BANK_0 | 0x67)
#define BIT_PRS_FIFO_EN                 0x20
#define BIT_ACCEL_FIFO_EN               0x10
#define BITS_GYRO_FIFO_EN               0x0E
#define REG_FIFO_MODE                   0x69

#define REG_FIFO_RST            (BANK_0 | 0x68)

#define REG_FIFO_COUNT_H        (BANK_0 | 0x70)
#define REG_FIFO_COUNT_L        (BANK_0 | 0x71)
#define REG_FIFO_R_W            (BANK_0 | 0x72)

#define REG_HW_FIX_DISABLE      (BANK_0 | 0x75)

#define REG_FIFO_CFG            (BANK_0 | 0x76)
#define BIT_MULTI_FIFO_CFG              0x01
#define BIT_SINGLE_FIFO_CFG             0x00

#define REG_ACCEL_XOUT_H_SH     (BANK_0 | 0x2D)
#define REG_ACCEL_XOUT_L_SH     (BANK_0 | 0x2E)
#define REG_ACCEL_YOUT_H_SH     (BANK_0 | 0x2F)
#define REG_ACCEL_YOUT_L_SH     (BANK_0 | 0x30)
#define REG_ACCEL_ZOUT_H_SH     (BANK_0 | 0x31)
#define REG_ACCEL_ZOUT_L_SH     (BANK_0 | 0x32)

#define REG_GYRO_XOUT_H          (BANK_0 | 0x33)

#define REG_MEM_START_ADDR      (BANK_0 | 0x7C)
#define REG_MEM_R_W             (BANK_0 | 0x7D)
#define REG_MEM_BANK_SEL        (BANK_0 | 0x7E)

/* bank 1 register map */
#define REG_TIMEBASE_CORRECTION_PLL   (BANK_1 | 0x28)
#define REG_TIMEBASE_CORRECTION_RCOSC (BANK_1 | 0x29)
#define REG_SELF_TEST1                (BANK_1 | 0x02)
#define REG_SELF_TEST2                (BANK_1 | 0x03)
#define REG_SELF_TEST3                (BANK_1 | 0x04)
#define REG_SELF_TEST4                (BANK_1 | 0x0E)
#define REG_SELF_TEST5                (BANK_1 | 0x0F)
#define REG_SELF_TEST6                (BANK_1 | 0x10)

/* bank 2 register map */
#define REG_GYRO_SMPLRT_DIV     (BANK_2 | 0x00)

#define REG_GYRO_CONFIG_1       (BANK_2 | 0x01)
#define BIT_FCHOICE                     0x01
#define SHIFT_GYRO_FS_SEL               1
#define SHIFT_GYRO_DLPCFG               3

#define REG_GYRO_CONFIG_2       (BANK_2 | 0x02)
#define BIT_GYRO_CTEN                   0x38

#define REG_ACCEL_SMPLRT_DIV_1  (BANK_2 | 0x10)
#define REG_ACCEL_SMPLRT_DIV_2  (BANK_2 | 0x11)

#define REG_ACCEL_CONFIG        (BANK_2 | 0x14)
#define SHIFT_ACCEL_FS                  1

#define REG_ACCEL_CONFIG_2      (BANK_2 | 0x15)
#define BIT_ACCEL_CTEN                  0x1C

#define REG_PRS_ODR_CONFIG      (BANK_2 | 0x20)
#define REG_PRGM_START_ADDRH    (BANK_2 | 0x50)

#define REG_MOD_CTRL_USR        (BANK_2 | 0x54)
#define BIT_ODR_SYNC                    0x7

/* bank 3 register map */
#define REG_I2C_MST_ODR_CONFIG  (BANK_3 | 0x0)

#define REG_I2C_MST_CTRL        (BANK_3 | 0x01)
#define BIT_I2C_MST_P_NSR               0x10

#define REG_I2C_MST_DELAY_CTRL  (BANK_3 | 0x02)
#define BIT_SLV0_DLY_EN                 0x01
#define BIT_SLV1_DLY_EN                 0x02
#define BIT_SLV2_DLY_EN                 0x04
#define BIT_SLV3_DLY_EN                 0x08

#define INV_MPU_BIT_SLV_EN      0x80
#define INV_MPU_BIT_BYTE_SW     0x40
#define INV_MPU_BIT_REG_DIS     0x20
#define INV_MPU_BIT_GRP         0x10
#define INV_MPU_BIT_I2C_READ    0x80

/* register for all banks */
#define REG_BANK_SEL            0x7F

/* data definitions */
#define BYTES_PER_SENSOR         6
#define FIFO_COUNT_BYTE          2
#define HARDWARE_FIFO_SIZE       1024

#define FIFO_SIZE                (HARDWARE_FIFO_SIZE * 7 / 8)
#define POWER_UP_TIME            100
#define REG_UP_TIME_USEC         100
#define DMP_RESET_TIME           20
#define GYRO_ENGINE_UP_TIME      50
#define MPU_MEM_BANK_SIZE        256
#define IIO_BUFFER_BYTES         8
#define HEADERED_NORMAL_BYTES    8
#define HEADERED_Q_BYTES         16
#define LEFT_OVER_BYTES          128
#define BASE_SAMPLE_RATE         1125

#ifdef FREQ_225
#define MPU_DEFAULT_DMP_FREQ     225
#define PEDOMETER_FREQ           (MPU_DEFAULT_DMP_FREQ >> 2)
#define DEFAULT_ACCEL_GAIN       (33554432L * 5 / 11)
#else
#define MPU_DEFAULT_DMP_FREQ     102
#define PEDOMETER_FREQ           (MPU_DEFAULT_DMP_FREQ >> 1)
#define DEFAULT_ACCEL_GAIN       33554432L
#endif
#define PED_ACCEL_GAIN           67108864L
#define ALPHA_FILL_PED           858993459
#define A_FILL_PED               214748365

#define MIN_MST_ODR_CONFIG       4
#define THREE_AXES               3
#define NINE_ELEM                (THREE_AXES * THREE_AXES)
#define MPU_TEMP_SHIFT           16
#define SOFT_IRON_MATRIX_SIZE    (4 * 9)
#define DMP_DIVIDER              (BASE_SAMPLE_RATE / MPU_DEFAULT_DMP_FREQ)
#define MAX_5_BIT_VALUE          0x1F
#define BAD_COMPASS_DATA         0x7FFF
#define DEFAULT_BATCH_RATE       400
#define DEFAULT_BATCH_TIME    (MSEC_PER_SEC / DEFAULT_BATCH_RATE)
#define MAX_COMPASS_RATE         115
#define MAX_PRESSURE_RATE        30
#define MAX_ALS_RATE             5
#define DATA_AKM_99_BYTES_DMP  10
#define DATA_AKM_89_BYTES_DMP  9
#define DATA_ALS_BYTES_DMP     8
#define APDS9900_AILTL_REG      0x04
#define BMP280_DIG_T1_LSB_REG                0x88
#define COVARIANCE_SIZE          14
#define ACCEL_COVARIANCE_SIZE  (COVARIANCE_SIZE * sizeof(int))
#define COMPASS_COVARIANCE_SIZE  (COVARIANCE_SIZE * sizeof(int))
#define TEMPERATURE_SCALE  3340827L
#define TEMPERATURE_OFFSET 1376256L
#define SECONDARY_INIT_WAIT 60
#define MPU_SOFT_UPDT_ADDR               0x86
#define MPU_SOFT_UPTD_MASK               0x0F
#define AK99XX_SHIFT                    23
#define AK89XX_SHIFT                    22
#define OPERATE_GYRO_IN_DUTY_CYCLED_MODE       (1<<4)
#define OPERATE_ACCEL_IN_DUTY_CYCLED_MODE      (1<<5)
#define OPERATE_I2C_MASTER_IN_DUTY_CYCLED_MODE (1<<6)

/* this is derived from 1000 divided by 55, which is the pedometer
   running frequency */
#define MS_PER_PED_TICKS         18

/* data limit definitions */
#define MIN_FIFO_RATE            4
#define MAX_FIFO_RATE            MPU_DEFAULT_DMP_FREQ
#define MAX_DMP_OUTPUT_RATE      MPU_DEFAULT_DMP_FREQ
#define MAX_READ_SIZE            128
#define MAX_MPU_MEM              8192
#define MAX_PRS_RATE             281

/* data header defines */
#define PRESSURE_HDR             0x8000
#define ACCEL_HDR                0x4000
#define ACCEL_ACCURACY_HDR       0x4080
#define GYRO_HDR                 0x2000
#define GYRO_ACCURACY_HDR        0x2080
#define COMPASS_HDR              0x1000
#define COMPASS_HDR_2            0x1800
#define CPASS_ACCURACY_HDR       0x1080
#define ALS_HDR                  0x0800
#define SIXQUAT_HDR              0x0400
#define PEDQUAT_HDR              0x0200
#define STEP_DETECTOR_HDR        0x0100

#define COMPASS_CALIB_HDR        0x0080
#define GYRO_CALIB_HDR           0x0040
#define EMPTY_MARKER             0x0020
#define END_MARKER               0x0010
#define NINEQUAT_HDR             0x0008
#define LPQ_HDR                  0x0004

#define STEP_INDICATOR_MASK      0x000f

/* init parameters */
#define MPU_INIT_SMD_THLD        1500
#define MPU_INIT_SENSOR_RATE     5
#define MPU_INIT_GYRO_SCALE      3
#define MPU_INIT_ACCEL_SCALE     0
#define MPU_INIT_PED_INT_THRESH  2
#define MPU_INIT_PED_STEP_THRESH 6
#define COMPASS_SLAVEADDR_AKM_BASE      0x0C
#define COMPASS_SLAVEADDR_AKM           0x0E

// interrupt configurations related to HW register
#define FSYNC_INT   BIT(7)
#define MOTION_INT  BIT(3)
#define PLL_INT     BIT(2)
#define DMP_INT     BIT(1)
#define I2C_INT     BIT(0)

#define CHIP_AWAKE          (0x01)
#define CHIP_LP_ENABLE      (0x02)

typedef enum {
    BANK_00        =        0b00000000,
    BANK_01         =       0b00010000,
    BANK_02         =       0b00100000,
    BANK_03         =       0b00110000
} ICM_bank_t;

typedef enum {
    DMP_EN          =       0b10000000,
    DMP_DIS         =       0b00000000,
    FIFO_EN         =       0b01000000,
    FIFO_DIS        =       0b00000000
} ICM_ctrl_t;



extern bool ICM20948_start(void);
extern void ICM20948_bank_select(ICM_bank_t);
extern void ICM20948_ping(void);
extern void ICM20948_read_gyro(uint8_t *);
extern void ICM20948_clr_irq(void);
extern void ICM20948_enable_irq(void);
extern void ICM20948_enable_FIFO(void);
extern uint8_t ICM20948_fifo_countH();

static void ICM20948_dmp_ctrl(ICM_ctrl_t);
static void ICM20948_fifo_ctrl(ICM_ctrl_t);
static void ICM20948_pwr_mgmt_1(uint8_t);
static void ICM20948_pwr_mgmt_2(uint8_t);
static void ICM20948_int_cfg(uint8_t);
extern void ICM20948_int_en1(uint8_t);
static void ICM20948_int_en2(uint8_t);
static void ICM20948_lp_config(uint8_t);
static void ICM20948_gyro_config1(uint8_t);
static void ICM20948_gyro_config2(uint8_t);
extern void IMC20948_process_FIFO(uint8_t *buffer);
static void ICM20948_fifo_en2(uint8_t);
static void ICM20948_fifo_mode(uint8_t);
extern void ICM20948_sleep(uint8_t sleep);



void  ICM20948_IRQ_Handler(PIN_Handle handle, PIN_Id pinId);

#endif /* ICM_20948_H_ */
