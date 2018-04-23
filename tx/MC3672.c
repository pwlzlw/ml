/*
 * MC3672.c
 *
 *  Created on: 9 Jun 2017
 *      Author: Pawel Zalewski
 *
 *      The driver is ported from an Arduino driver supplied by the manufacturer, I had to port it
 *      and make it operational from TI RTOS context - which meant changing some funcitons.
 *      FIFO functionality and Interrupt enabling was coded from scratch.
 *      SOurce driver available from: https://github.com/mcubemems/Accelerometer_MC3672
 */

#include "MC3672.h"

//pin handle
static PIN_Handle pinHandle = NULL;
static PIN_Config pinTable[] = {
      CC2640R2_LAUNCHXL_SPI0_CSN | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_PUSHPULL,    //chip select is dio11
      CC2640R2_LAUNCHXL_DIO0     | PIN_INPUT_EN       | PIN_PULLUP    | PIN_IRQ_NEGEDGE | PIN_HYSTERESIS, //IRQ
      PIN_TERMINATE
  };
static PIN_State pinState;


static uint32_t t0,t1,time,freq;
static Types_FreqHz frequency;


//functions for asserting and de-asserting chip select pin
static void Select(void) {
  PIN_setOutputValue(pinHandle,CC2640R2_LAUNCHXL_SPI0_CSN,SELECT_ON);
}

static void Deselect(void) {
  PIN_setOutputValue(pinHandle,CC2640R2_LAUNCHXL_SPI0_CSN,SELECT_OFF);
}

//used to calculate the acceleration
uint8_t CfgRange, CfgResolution;

// Read 8-bit from register
static uint8_t readRegister8(uint8_t reg) {

  SPI_Transaction spiTransaction;
  uint8_t SPI_data_out_buffer[2];
  uint8_t SPI_data_in_buffer[2];

  spiTransaction.arg   = NULL;
  spiTransaction.count = 2;
  spiTransaction.txBuf = (Ptr)SPI_data_out_buffer;
  spiTransaction.rxBuf = (Ptr)SPI_data_in_buffer;


  //commands are embedded into the address bytes
  SPI_data_out_buffer[0] = (reg | 0x80 | 0x40);

  Select();

  threadsafe_SPI0_transfer(&spiTransaction);

  Deselect();

  //NB: the first entry is 0x00 (cycle of sending the address)
  return SPI_data_in_buffer[1];
}

// Write 8-bit to register
static void writeRegister8(uint8_t reg, uint8_t value) {

  SPI_Transaction spiTransaction;
  uint8_t SPI_data_out_buffer[2];
  uint8_t SPI_data_in_buffer[2];

  spiTransaction.arg   = NULL;
  spiTransaction.count = 2;
  spiTransaction.txBuf = (Ptr)SPI_data_out_buffer;
  spiTransaction.rxBuf = (Ptr)SPI_data_in_buffer;

  SPI_data_out_buffer[0] = (reg | 0x40);
  SPI_data_out_buffer[1] = value;

  Select();

  threadsafe_SPI0_transfer(&spiTransaction);

  Deselect();
}

// Repeated Read X,Y,Z data from register 0x02 - 0x07
static void readRegisters(uint8_t reg, uint8_t *buffer, uint8_t len) {

  SPI_Transaction spiTransaction;
  uint8_t SPI_data_out_buffer[MAX_SAMPLES+1] = {0};
  uint8_t SPI_data_in_buffer[MAX_SAMPLES+1] = {0};
  int i;

  spiTransaction.arg   = NULL;
  spiTransaction.count = len+1;
  spiTransaction.txBuf = (Ptr)SPI_data_out_buffer;
  spiTransaction.rxBuf = (Ptr)SPI_data_in_buffer;

  //adress first
  SPI_data_out_buffer[0] = (reg | 0x80 | 0x40);

  Select();

  threadsafe_SPI0_transfer(&spiTransaction);

  Deselect();

   //need to re-arrange to get rid of the blank byte
  for(i = 1 ; i < len+1 ; i++) {
    buffer[i-1] = SPI_data_in_buffer[i];
  }
}

//Set the operation mode
void MC3672_SetMode(MC3672_mode_t mode) {
    uint8_t value;
    value = readRegister8(MC3672_REG_MODE_C);
    value &= 0b11110000;
    value |= mode;
    writeRegister8(MC3672_REG_MODE_C, value);
}

//Set the range control
void MC3672_SetRangeCtrl(MC3672_range_t range) {
    uint8_t value;
    CfgRange = range;
    MC3672_SetMode(MC3672_MODE_STANDBY);
    value = readRegister8(MC3672_REG_RANGE_C);
    value &= 0b00000111;
    value |= (range << 4)&0x70 ;
    writeRegister8(MC3672_REG_RANGE_C, value);
}

//Initial reset
void MC3672_reset() {

  writeRegister8(0x10, 0x01);

  Task_sleep(1);

  writeRegister8(0x24, 0x40);
  //need to wait here

  Task_sleep(1);

  uint8_t _bRegIO_C = 0b1000000;

  writeRegister8(0x0D, _bRegIO_C);


  writeRegister8(0x09, 0x00);

  Task_sleep(1);

  writeRegister8(0x0F, 0x42);

  Task_sleep(1);

  writeRegister8(0x20, 0x01);

  Task_sleep(1);

  writeRegister8(0x21, 0x80);

  Task_sleep(1);

  writeRegister8(0x28, 0x00);

  Task_sleep(1);

  writeRegister8(0x1A, 0x00);

  Task_sleep(1);

 
  writeRegister8(0x10, 0x01);

  Task_sleep(1);

}

//Set Sniff Analog Gain
void MC3672_SetSniffAGAIN(MC3672_gain_t gain)
{
    writeRegister8(0x20, 0x00);
    uint8_t value;
    value = readRegister8(MC3672_REG_GAIN);
    value &= 0b00111111;
    value |= (gain << 6);
    writeRegister8(MC3672_REG_GAIN, value);
}

//Set CWake Analog Gain
void MC3672_SetWakeAGAIN(MC3672_gain_t gain)
{
    writeRegister8(0x20, 0x01);
    uint8_t value;
    value = readRegister8(MC3672_REG_GAIN);
    value &= 0b00111111;
    value |= (gain << 6);
    writeRegister8(MC3672_REG_GAIN, value);
}


//Set the resolution control
void MC3672_SetResolutionCtrl(MC3672_resolution_t resolution)
{
  uint8_t value;
  CfgResolution = resolution;
  MC3672_SetMode(MC3672_MODE_STANDBY);
  value = readRegister8(MC3672_REG_RANGE_C);
  value &= 0b01110000;
  value |= resolution;
  writeRegister8(MC3672_REG_RANGE_C, value);
}

//Set the sampling rate
void MC3672_SetCWakeSampleRate(MC3672_cwake_sr_t sample_rate)
{
  uint8_t value;
  MC3672_SetMode(MC3672_MODE_STANDBY);
  value = readRegister8(MC3672_REG_WAKE_C);
  value &= 0b00000000;
  value |= sample_rate;
  writeRegister8(MC3672_REG_WAKE_C, value);
}

//Get the output sampling rate
MC3672_cwake_sr_t MC3672_GetCWakeSampleRate(void)
{
  /* Read the data format register to preserve bits */
  uint8_t value;
  value = readRegister8(MC3672_REG_WAKE_C);
  value &= 0b00001111;
  return (MC3672_cwake_sr_t) (value);
}

//Get the range control
MC3672_range_t MC3672_GetRangeCtrl(void)
{
  /* Read the data format register to preserve bits */
  uint8_t value;
  value = readRegister8(MC3672_REG_RANGE_C);
  value &= 0x70;
  return (MC3672_range_t) (value >> 4);
}

//Get the range control
MC3672_resolution_t MC3672_GetResolutionCtrl(void)
{
  /* Read the data format register to preserve bits */
  uint8_t value;
  value = readRegister8(MC3672_REG_RANGE_C);
  value &= 0x07;
  return (MC3672_resolution_t) (value);
}

bool MC3672_start(void)
{
  pinHandle = PIN_open(&pinState, pinTable);

  //set CS to a logic 1 first
  PIN_setOutputValue(pinHandle,CC2640R2_LAUNCHXL_SPI0_CSN,SELECT_OFF);

  //registes the interrupt
  PIN_registerIntCb(pinHandle, &MC3672_IRQ_Handler);

  if(!pinHandle) {
    /* Error initializing pins */
    System_printf("***Failed to obtain the handle\n");
  }

  bool SPI0_config_OK;
  SPI0_config_OK = SPI0_init(4000000);

  //Init Reset
  MC3672_reset();


  MC3672_SetWakeAGAIN(MC3672_GAIN_1X);

  MC3672_SetSniffAGAIN(MC3672_GAIN_1X);

  uint8_t id = readRegister8(MC3672_REG_PROD);
  //t1 = Timestamp_get32();
  //time = t1 - t0;
  if (id != 0x71)
  {
    /* No MC3672 detected ... return false */
    System_printf("***No MC3672 detected\n");
    return false;
  }

  MC3672_SetRangeCtrl(MC3672_RANGE_8G);        //Range: 8g

  MC3672_SetResolutionCtrl(MC3672_RESOLUTION_12BIT);     //Resolution: 12bit => no other possible for FIFO

  MC3672_SetCWakeSampleRate(MC3672_CWAKE_SR_14Hz);   //Sampling Rate: 14Hz

  Task_sleep(1);

  MC3672_SetMode(MC3672_MODE_CWAKE);         //Mode: Active

  Task_sleep(1);

  return true;
}

void MC3672_stop()
{
  MC3672_SetMode(MC3672_MODE_SLEEP); //Set mode as Sleep
}

void MC3672_enableFIFO(void) {

  uint8_t value;

  MC3672_SetMode(MC3672_MODE_STANDBY);

  //enable FIFO  rst| ena | mode ||||| #samples
  value = readRegister8(MC3672_REG_FIFO_C);
  //value &= 0b10000000;
  //value |= 0b01110010; //0b01111111;
  value &= 0b10111111;
  value |= 0b01000000;
  writeRegister8(MC3672_REG_FIFO_C, value);

  //read any no of bytes
  value = readRegister8(MC3672_REG_FREG_2);
  value &= 0b11111101;
  value |= 0x02;          //0b00000010;
  writeRegister8(MC3672_REG_FREG_2, value);

  //enable the interrupt - active low, pullup
  value = readRegister8(MC3672_REG_INTR_C);
  //value &= 0b10111110;
  //value |= 0b01000001;
  value &= 0b11011110;
  value |= 0x21;             //0b00100001;
  writeRegister8(MC3672_REG_INTR_C, value);

  Task_sleep(1);

  MC3672_SetMode(MC3672_MODE_CWAKE);         //Mode: Active

  Task_sleep(1);

}

//clear IRQ register
void MC3672_clrIRQ(void) {

  uint8_t value,x;

  value = readRegister8(MC3672_REG_STATUS_2);
  x = value & 0b00100000;
  if (x == 32) { //32) {
    //value &= 0b10111111;
      value &= 0b10001111;
      writeRegister8(MC3672_REG_STATUS_2, value);
  }
}

//process the fifo
void MC3672_processFIFO(uint8_t *bufferDATA) {
  
  readRegisters(MC3672_REG_XOUT_LSB, bufferDATA, MAX_SAMPLES);
}

//used for debugging only
void MC3672_Probe(void) {

  uint8_t values[5];

  values[0] = readRegister8(MC3672_REG_FREG_2); //fifo burst is ON

  values[1] = readRegister8(MC3672_REG_FIFO_C);

  values[2] = readRegister8(MC3672_REG_INTR_C);

  values[3] = readRegister8(MC3672_REG_STATUS_2); //interrupt.

  values[4] = readRegister8(MC3672_REG_STATUS_1);
}


void MC3672_SetPowerMode(MC3672_power_mode_t power) {
  uint8_t value;
  value = readRegister8(MC3672_REG_POWER_MODE);
  value &= 0b11111000;
  value |= power;
  writeRegister8(MC3672_REG_POWER_MODE, value);
}
