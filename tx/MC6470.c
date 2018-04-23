/*
 * MC6470.c
 *
 *  Created on: 18 Jul 2017
 *      Author: Pawel Zalewski. *     
 */

#include "MC6470.h"

//pin configuration
static PIN_Handle pinHandle = NULL;
static PIN_Config pinTable[] = {
      CC2640R2_LAUNCHXL_DIO22 | PIN_INPUT_EN | PIN_PULLDOWN | PIN_IRQ_POSEDGE | PIN_HYSTERESIS, //IRQ Magnetometer
      PIN_TERMINATE
  };

static PIN_State pinState;

static PIN_Handle pinHandleB = NULL;
static PIN_Config pinTableB[] = {
      CC2640R2_LAUNCHXL_DIO21 | PIN_INPUT_EN | PIN_PULLDOWN | PIN_IRQ_POSEDGE | PIN_HYSTERESIS, //IRQ Accelerometer
      PIN_TERMINATE
  };

static PIN_State pinStateB;

/*********MAGNETOMETER******************/

// Read 8-bit from register
static uint8_t readRegister8(uint8_t reg, uint8_t addr) {

  I2C_Transaction i2cTransaction;
  uint8_t         txBuffer[1] = {0};
  uint8_t         rxBuffer[1] = {0};

  txBuffer[0] = reg;

  i2cTransaction.slaveAddress = addr;
  i2cTransaction.writeBuf = txBuffer;
  i2cTransaction.writeCount = 1;
  i2cTransaction.readBuf = rxBuffer;
  i2cTransaction.readCount = 1;

  threadsafe_I2C_transfer(&i2cTransaction);

  return rxBuffer[0];
}

// Write 8-bit to the register
static void writeRegister8(uint8_t reg, uint8_t value, uint8_t addr) {

  I2C_Transaction i2cTransaction;
  uint8_t         txBuffer[2] = {0};
  uint8_t         rxBuffer[1] = {0};

  txBuffer[0] = reg;
  txBuffer[1] = value;

  i2cTransaction.slaveAddress = addr;
  i2cTransaction.writeBuf = txBuffer;
  i2cTransaction.writeCount = 2;
  i2cTransaction.readBuf = rxBuffer;
  i2cTransaction.readCount = 0;

  threadsafe_I2C_transfer(&i2cTransaction);

}

// Sfotware reset
static void MC6470_MAG_reset(void) {
  writeRegister8(MCMAG_REG_CTRL3,MCMAG_CTRL3_ENABLE_SOFT_RESET,MCMAG_I2C_ADDR);
}

//self test
static bool MC6470_MAG_test(void) {

  uint8_t id;

  /* Check connection */

  id = readRegister8(MCMAG_REG_CTRL3,MCMAG_I2C_ADDR);
  id &= 11101111;
  id |= MCMAG_SELFTEST_DATA;
  writeRegister8(MCMAG_REG_CTRL3,MCMAG_SELFTEST_DATA,MCMAG_I2C_ADDR);

  id = readRegister8(MCMAG_REG_STB,MCMAG_I2C_ADDR);

  if (id != 0xAA) {
          /* No MC6470 detected ... return false */
          System_printf("***No MC6470 detected\n");
          return false;
  }

  id = readRegister8(MCMAG_REG_STB,MCMAG_I2C_ADDR);

  if (id != MCMAG_SELFTEST_RESPONSE) {
        /* No MC6470 detected ... return false */
        System_printf("***No MC6470 detected\n");
        return false;
  }


  id = readRegister8(MCMAG_REG_STB,MCMAG_I2C_ADDR);

  //calibrate
  id = readRegister8(MCMAG_REG_CTRL3,MCMAG_I2C_ADDR);
  id &= 11111110;
  id |= 0x01;
  writeRegister8(MCMAG_REG_CTRL3,MCMAG_SELFTEST_DATA,MCMAG_I2C_ADDR);

  return true;
}

//set between stand-by and active mode, the sensor needs to be called into standby for any major changes
void MC6470_MAG_setMode(MC6470_mag_mode_t mode) {
  uint8_t val;
  val = readRegister8(MCMAG_REG_CTRL1,MCMAG_I2C_ADDR);
  val &= 0b01111111;
  val |= mode;
  writeRegister8(MCMAG_REG_CTRL1,val,MCMAG_I2C_ADDR);
}
//data rate of the magnetometer
void MC6470_MAG_setDataRate(MC6470_mag_data_rate_t rate) {
  uint8_t val;
  val = readRegister8(MCMAG_REG_CTRL1,MCMAG_I2C_ADDR);
  val &= 0b11100111;
  val |= rate;
  writeRegister8(MCMAG_REG_CTRL1,val,MCMAG_I2C_ADDR);
}
//state: forced or normal (continous)
void MC6470_MAG_setState(MC6470_mag_state_t state) {
  uint8_t val;
  val = readRegister8(MCMAG_REG_CTRL1,MCMAG_I2C_ADDR);
  val &= 0b11111101;
  val |= state;
  writeRegister8(MCMAG_REG_CTRL1,val,MCMAG_I2C_ADDR);
}

//initialize the IC
bool MC6470_MAG_start(void)
{
  /* Create I2C for usage NB this I2C init is for the screen as well, run only once ! */

  pinHandle = PIN_open(&pinState, pinTable);
  //registes the interrupt
  PIN_registerIntCb(pinHandle, &MC6470_MAG_IRQ_Handler);

  bool I2C_config_OK;

  I2C_config_OK = I2Cthreading_init();

  if (I2C_config_OK==false)
  {
    System_printf("***Failed I2C Init\n");
    return false;
  }
  else
  {
    System_printf("I2C Init OK\n");
  }

  /* Software reset */

  MC6470_MAG_reset();

  MC6470_MAG_setMode(MC6470_MODE_ACTIVE);

  /* Do self test */

  if(! MC6470_MAG_test() ) {
    return false;
  }

  MC6470_MAG_setInterrupts(MCMAG_INTM_EN);

  MC6470_MAG_setDataRate(MC6470_DATA_RATE_20HZ);

  MC6470_MAG_setState(MC6470_STATE_NORMAL);

  return true;
}
//toggle on or off
void MC6470_MAG_setInterrupts(MC6470_mag_int_t value){
  uint8_t val;
  val = readRegister8(MCMAG_REG_CTRL2,MCMAG_I2C_ADDR);
  val &= 0b11110111;
  val |= value;
  writeRegister8(MCMAG_REG_CTRL2,val,MCMAG_I2C_ADDR);
}

uint8_t MC6470_MAG_readStatus(void) {
  uint8_t val;
  val = readRegister8(MCMAG_REG_STATUS,MCMAG_I2C_ADDR);
  return val;
}

void MC6470_MAG_startMeassure(void) {
  uint8_t val;
  val = readRegister8(MCMAG_REG_CTRL3,MCMAG_I2C_ADDR);
  val &= 0b10111111;
  val |= 0b01000000;
  writeRegister8(MCMAG_REG_CTRL3,val,MCMAG_I2C_ADDR);
}
//reads x,y,z values
void MC6470_MAG_readMgntmtr(uint8_t *bufferDATA) {
  int j;
  for(j= 0;j<6;j++) {
    bufferDATA[j] = readRegister8(MCMAG_REG_XOUT+j,MCMAG_I2C_ADDR);
  }
}

/*********ACCELEROMETER*****************/

static void MC6470_ACC_reset(void) {

  MC6470_ACC_setSampleRate(MCACC_SR_16HZ);

  MC6470_ACC_setResolution(MCACC_12bits);

  MC6470_ACC_setRange(MCACC_8g);


  MC6470_ACC_setTapDuration(0b0111); // if DUR define how long is a tap in samples

  MC6470_ACC_setTapQuiet(0b0000); //if DUR ignore taps for the next X samples

  //configure the tap detector for threshold
  MC6470_ACC_setTapDetection(MCACC_TAP_EN | MCACC_TAP_THR | MCACC_TAP_ZPEN | MCACC_TAP_YPEN  ); // | MCACC_TAP_XPEN );

}

void MC6470_ACC_start(void) {

  pinHandleB = PIN_open(&pinStateB, pinTableB);
  //registes the interrupt
  PIN_registerIntCb(pinHandleB, &MC6470_ACC_IRQ_Handler);

  MC6470_ACC_setMode(MC6470_ACC_MODE_STNDBY);

  Task_sleep(1);

  MC6470_ACC_reset();

  //enable x,y,z interrupts
  uint8_t  val = MCACC_INTM_EN | MCACC_INTM_TIZNEN | MCACC_INTM_TIYNEN | MCACC_INTM_TIXNEN;

  MC6470_ACC_setInterrupts(val);

  Task_sleep(5);

  MC6470_ACC_setMode(MC6470_ACC_MODE_WAKE);

}
void MC6470_ACC_setMode(MC6470_acc_mode_t mode) {
  uint8_t val;
  val = readRegister8(MCACC_REG_MODE,MCACC_I2C_ADDR);
  val &= 0b11111100;
  val |= mode;
  writeRegister8(MCACC_REG_MODE,val,MCACC_I2C_ADDR);
}
//setup the interrupts, use OR-ed values
void MC6470_ACC_setInterrupts(MC6470_acc_int_t value){

  uint8_t val;
  //set to active high

  val = readRegister8(MCACC_REG_MODE,MCACC_I2C_ADDR);
  val &= 0b01111111;
  val |= 0b10000000;
  writeRegister8(MCACC_REG_MODE,val,MCACC_I2C_ADDR);


  
  val = readRegister8(MCACC_REG_MODE,MCACC_I2C_ADDR);
  val &= 0b01111111;
  val |= 0b00000000;
  writeRegister8(MCACC_REG_MODE,val,MCACC_I2C_ADDR);
  val = readRegister8(MCACC_REG_MODE,MCACC_I2C_ADDR);
  val &= 0b01111111;
  val |= 0b10000000;
  writeRegister8(MCACC_REG_MODE,val,MCACC_I2C_ADDR);
  // remove

  val = readRegister8(MCACC_REG_INTEN,MCACC_I2C_ADDR);
  val &= 0b01000000;
  val |= value;
  writeRegister8(MCACC_REG_INTEN,val,MCACC_I2C_ADDR);
}
//interrupt are cleared by reading the status register
uint8_t MC6470_ACC_readStatus(void) {
  uint8_t val;
  val = readRegister8(MCACC_REG_SR,MCACC_I2C_ADDR);
  return val;
}
//sample rate
void MC6470_ACC_setSampleRate(MC6470_acc_sample_rate_t rate) {
  uint8_t val;
  val = readRegister8(MCACC_REG_SRTFR,MCACC_I2C_ADDR);
  val &= 0b11110000;
  val |= rate;
  writeRegister8(MCACC_REG_SRTFR,val,MCACC_I2C_ADDR);
}
//configure the tap register, use OR-ed values
void MC6470_ACC_setTapDetection(MC6470_acc_tap_t tap) {
  uint8_t val;
  val = readRegister8(MCACC_REG_TAPEN,MCACC_I2C_ADDR);
  val &= 0b00000000;
  val |= tap;
  writeRegister8(MCACC_REG_TAPEN,val,MCACC_I2C_ADDR);
}
// from 0b000 - b1111
void MC6470_ACC_setTapQuiet(uint8_t value) {
  uint8_t val;
  val = readRegister8(MCACC_REG_TTTRX,MCACC_I2C_ADDR);
  val &= 0b00001111;
  val |= value << 4 ;
  writeRegister8(MCACC_REG_TTTRX,val,MCACC_I2C_ADDR);
  writeRegister8(MCACC_REG_TTTRY,val,MCACC_I2C_ADDR);
  writeRegister8(MCACC_REG_TTTRZ,val,MCACC_I2C_ADDR);
}
//from 0b000 - b1111
void MC6470_ACC_setTapDuration(uint8_t duration){
  uint8_t val;
  val = readRegister8(MCACC_REG_TTTRX,MCACC_I2C_ADDR);
  val &= 0b11110000;
  val |= duration;
  writeRegister8(MCACC_REG_TTTRX,val,MCACC_I2C_ADDR);
  writeRegister8(MCACC_REG_TTTRY,val,MCACC_I2C_ADDR);
  writeRegister8(MCACC_REG_TTTRZ,val,MCACC_I2C_ADDR);
}
//read xyz values - reading in one go not supported
void MC6470_ACC_readAcc(uint8_t *bufferDATA) {
  int j;
  for(j= 0;j<6;j++) {
    bufferDATA[j] = readRegister8(MCACC_REG_XOUT_EX_L+j,MCACC_I2C_ADDR);
  }
}

void MC6470_ACC_setResolution(MC6470_acc_resolution_t resolution) {
  uint8_t val;
  val = readRegister8(MCACC_REG_OUTCFG ,MCACC_I2C_ADDR);
  val &= 0b01111000;
  val |= resolution;
  writeRegister8(MCACC_REG_OUTCFG ,val,MCACC_I2C_ADDR);
}

void MC6470_ACC_setRange(MC6470_acc_range_t range) {
  uint8_t val;
  val = readRegister8(MCACC_REG_OUTCFG ,MCACC_I2C_ADDR);
  val &= 0b00001111;
  val |= range << 4;
  writeRegister8(MCACC_REG_OUTCFG ,val,MCACC_I2C_ADDR);
}