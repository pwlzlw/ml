/*
 * ICM_20948.c *
 *  Created on: 7 Oct 2017
 *      Author: Pawel Zalewski.
 */
#include "ICM_20948.h"
#include <stdio.h>
//pin handle
static PIN_Handle pinHandle = NULL;
static PIN_Config pinTable[] = {
      CC2640R2_LAUNCHXL_DIO15     | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_PUSHPULL,    //chip select
      CC2640R2_LAUNCHXL_DIO25     | PIN_INPUT_EN       | PIN_PULLUP    | PIN_IRQ_NEGEDGE | PIN_HYSTERESIS, //IRQ
      PIN_TERMINATE
  };
static PIN_State pinState;

static uint32_t t0,t1,time,freq;
static Types_FreqHz frequency;


//functions for asserting and de-asserting chip select pin
static void Select(void) {
  PIN_setOutputValue(pinHandle,CC2640R2_LAUNCHXL_DIO15,0);
}

static void Deselect(void) {
  PIN_setOutputValue(pinHandle,CC2640R2_LAUNCHXL_DIO15,1);
}


// Read 8-bit from register
static uint8_t readRegister8(uint8_t reg) {

  SPI_Transaction spiTransaction;
  uint8_t SPI_data_out_buffer_[2];
  uint8_t SPI_data_in_buffer_[2];

  spiTransaction.arg   = NULL;
  spiTransaction.count = 2;
  spiTransaction.txBuf = (Ptr)SPI_data_out_buffer_;
  spiTransaction.rxBuf = (Ptr)SPI_data_in_buffer_;

  SPI_data_out_buffer_[0] = (reg | 0x80 ); //MSB is 1 for read

  Select();

  threadsafe_SPI1_transfer(&spiTransaction);

  Deselect();

  return SPI_data_in_buffer_[1];
}
// Write 8-bit to register
static void writeRegister8(uint8_t reg, uint8_t value) {

  SPI_Transaction spiTransaction;
  uint8_t SPI_data_out_buffer_[2];
  uint8_t SPI_data_in_buffer_[2];

  spiTransaction.arg   = NULL;
  spiTransaction.count = 2;
  spiTransaction.txBuf = (Ptr)SPI_data_out_buffer_;
  spiTransaction.rxBuf = (Ptr)SPI_data_in_buffer_;

  SPI_data_out_buffer_[0] = reg & 0b01111111; //MSB = 0
  SPI_data_out_buffer_[1] = value;

  Select();

  threadsafe_SPI1_transfer(&spiTransaction);

  Deselect();
}

static void readRegisters(uint8_t reg, uint8_t *buffer, uint8_t len) {

  SPI_Transaction spiTransaction;
  uint8_t SPI_data_out_buffer[192+1] = {0};
  uint8_t SPI_data_in_buffer[192+1] = {0};
  int i;

  spiTransaction.arg   = NULL;
  spiTransaction.count = len+1;
  spiTransaction.txBuf = (Ptr)SPI_data_out_buffer;
  spiTransaction.rxBuf = (Ptr)SPI_data_in_buffer;

  //adress first
  SPI_data_out_buffer[0] = (reg| 0x80 );

  Select();

  threadsafe_SPI1_transfer(&spiTransaction);

  Deselect();

   //need to re-arrange to get rid of the blank byte
  for(i = 1 ; i < len+1 ; i++) {
    buffer[i-1] = SPI_data_in_buffer[i];
  }
}


// Read 8-bit from register
void IMC20948_process_FIFO(uint8_t *buffer) {
    uint8_t reg[320];
    short j,k;
    for(j= 0;j<320;j++) {
        reg[j] = readRegister8(REG_FIFO_R_W);
    }

    for(j=0;j<32;j++) {
        for (k=0;k<6;k++) {
            buffer[(j*6)+k] = reg[(j*10) + k];
        }
    }
}

static void ICM20948_image_load() {

    SPI_Handle image;
    SPI_Params spiParamss;
    SPI_Params_init(&spiParamss);
    spiParamss.transferMode = SPI_MODE_BLOCKING;
    spiParamss.bitRate = 7000000;
    spiParamss.dataSize = 8;
    spiParamss.transferCallbackFxn = NULL;
    spiParamss.frameFormat = SPI_POL0_PHA0;
    spiParamss.transferTimeout = SPI_WAIT_FOREVER;
    spiParamss.mode = SPI_MASTER;

    image = SPI_open(Board_SPI1, &spiParamss);


    int j = 0;
    SPI_Transaction spiTransaction;
    uint8_t SPI_data_out_buffer_[128];
    spiTransaction.arg   = NULL;
    spiTransaction.count = 128;
    spiTransaction.txBuf = (Ptr)SPI_data_out_buffer_;
    spiTransaction.rxBuf = NULL;

    //continously send 14290 bytes...
    //SectionA
    Select();
    for (j = 0;j<28;j++) {
        memcpy(SPI_data_out_buffer_,dmp3A+j*128,sizeof(SPI_data_out_buffer_));

        SPI_transfer(image, &spiTransaction);
    }
    //SectionAA
    for (j = 0;j<28;j++) {
        memcpy(SPI_data_out_buffer_,dmp3AA+j*128,sizeof(SPI_data_out_buffer_));

        SPI_transfer(image, &spiTransaction);
    }
    //SectionB
    for (j = 0;j<58;j++) {
        memcpy(SPI_data_out_buffer_,dmp3B+j*128,sizeof(SPI_data_out_buffer_));
        if(j == 57) {
            spiTransaction.count = 82;
        }

        SPI_transfer(image, &spiTransaction);
    }
    Deselect();
    SPI_close(image);
    image = NULL;
}

void ICM20948_bank_select(ICM_bank_t bank) {
    uint8_t value;
    value =  readRegister8(REG_BANK_SEL);
    value &= 0b11001111;
    value |= bank;
    writeRegister8(REG_BANK_SEL, value);
}

static void ICM20948_dmp_ctrl(ICM_ctrl_t dmp){
    uint8_t value = 0;
    value =  readRegister8(REG_USER_CTRL);
    value &= 0b01111111;
    value |= dmp;
    writeRegister8(REG_USER_CTRL, value);
}

static void ICM20948_fifo_ctrl(ICM_ctrl_t fifo) {
    uint8_t value = 0;
    value =  readRegister8(REG_USER_CTRL);
    value &= 0b00111111;
    value |= fifo;
    writeRegister8(REG_USER_CTRL, value);
}

static void ICM20948_pwr_mgmt_1(uint8_t pwr) {
    uint8_t value = 0;
    value =  readRegister8(REG_PWR_MGMT_1);
    value &= 0b00001000;
    value |= pwr;
    writeRegister8(REG_PWR_MGMT_1, value);
}

static void ICM20948_pwr_mgmt_2(uint8_t pwr) {
    uint8_t value = 0;
    value =  readRegister8(REG_PWR_MGMT_2);
    value &= 0b11000000;
    value |= pwr;
    writeRegister8(REG_PWR_MGMT_2, value);
}

static void ICM20948_int_cfg(uint8_t cfg) {
    uint8_t value = 0;
    value =  readRegister8(REG_INT_PIN_CFG);
    value &= 0b00000001;
    value |= cfg;
    writeRegister8(REG_INT_PIN_CFG, value);
}

void ICM20948_int_en1(uint8_t cfg) {
    uint8_t value = 0;
    value =  readRegister8(REG_INT_ENABLE_1);
    value &= 0b11111110;
    value |= cfg;
    writeRegister8(REG_INT_ENABLE_1, value);
}

static void ICM20948_int_en2(uint8_t cfg) {
    uint8_t value = 0;
    value =  readRegister8(REG_INT_ENABLE_2);
    value &= 0b11100000;
    value |= cfg;
    writeRegister8(REG_INT_ENABLE_2, value);
}

void ICM20948_read_gyro(uint8_t *bufferDATA) {
    int j;
    for(j= 0;j<6;j++) {
      bufferDATA[j] = readRegister8(REG_GYRO_XOUT_H +j);
    }
}

static void ICM20948_lp_config(uint8_t cfg) {
    uint8_t value = 0;
    value =  readRegister8(REG_LP_CONFIG);
    value &= 0b10001111;
    value |= cfg;
    writeRegister8(REG_LP_CONFIG, value);
}

static void ICM20948_gyro_config1(uint8_t cfg) {
    uint8_t value = 0;
    value =  readRegister8(REG_GYRO_CONFIG_1);
    value &= 0b1100000;
    value |= cfg;
    writeRegister8(REG_GYRO_CONFIG_1, value);
}

static void ICM20948_gyro_config2(uint8_t cfg) {
    uint8_t value = 0;
    value =  readRegister8(REG_GYRO_CONFIG_2);
    value &= 0b1100000;
    value |= cfg;
    writeRegister8(REG_GYRO_CONFIG_2, value);
}

void ICM20948_self_test() {
    uint8_t value = 0;
    value = readRegister8(REG_SELF_TEST1);
    value = readRegister8(REG_SELF_TEST2);
    value = readRegister8(REG_SELF_TEST3);
}

static void ICM20948_fifo_en2(uint8_t cfg) {
    uint8_t value = 0;
    value =  readRegister8(REG_FIFO_EN_2);
    value &= 0b11100000;
    value |= cfg;
    writeRegister8(REG_FIFO_EN_2, value);
}

static void ICM20948_fifo_mode(uint8_t cfg){
    uint8_t value = 0;
    value =  readRegister8(REG_FIFO_MODE);
    value &= 0b11100000;
    value |= cfg;
    writeRegister8(REG_FIFO_MODE, value);
}

uint8_t ICM20948_fifo_countH(){
    uint8_t value = 0;
    value =  readRegister8(REG_FIFO_COUNT_H);
    return value;
}

void ICM20948_clr_irq() {
    readRegister8(REG_INT_STATUS_1);
    //readRegister8(REG_INT_STATUS);
}

void ICM20948_enable_irq() {


   ICM20948_int_cfg(BIT_INT_AL|BIT_INT_PP|BIT_INT_LATCH_EN|BIT_INT_CLEAR);
   
   ICM20948_int_en1(BIT_DATA_RDY_0_EN); //manual is wrong cannot write to this guy in LP mode
   
   ICM20948_pwr_mgmt_1(BIT_TMP_DIS|BIT_CLK_PLL|BIT_LP_EN);
}


void ICM20948_enable_FIFO() {
   ICM20948_fifo_ctrl(FIFO_EN);
   ICM20948_fifo_en2(0b11101110);
   ICM20948_fifo_mode(0b11100000);
   ICM20948_int_cfg(BIT_INT_AL|BIT_INT_PP|BIT_INT_LATCH_EN|BIT_INT_CLEAR);
   ICM20948_int_en2(BIT_FIFO_OVERFLOW_EN_0);
   ICM20948_pwr_mgmt_1(BIT_TMP_DIS|BIT_CLK_PLL|BIT_LP_EN);
}

void ICM20948_sleep(uint8_t sleep) {
    uint8_t value = 0;
    value =  readRegister8(REG_PWR_MGMT_1);
    value &= 0b10111111;
    value |= sleep<<6;
    writeRegister8(REG_PWR_MGMT_1, value);
}

bool ICM20948_start() {

    uint8_t value = 0;

    pinHandle = PIN_open(&pinState, pinTable);

    //set CS to a logic 1 first
    PIN_setOutputValue(pinHandle,CC2640R2_LAUNCHXL_DIO15,1);

    bool SPI1_config_OK;

    if(!pinHandle) {
        /* Error initializing pins */
        System_printf("***Failed to obtain the handle\n");
        return false;
    }

    //SPI init
    SPI1_config_OK = SPI1_init(7000000);
    if (SPI1_config_OK==false)
    {
        System_printf("***Failed SPI1 Init\n");
        return false;
    }
    else
    {
        System_printf("SPI1 Init OK\n");
    }
    //t0 = Timestamp_get32();
    value = readRegister8(REG_WHO_AM_I);
    //t1 = Timestamp_get32();
    //time = t1 - t0;
    if (value != 0xEA) {
        return false;
    }

    //reset the device
    ICM20948_pwr_mgmt_1(BIT_H_RESET);
    Task_sleep(100);
    SPI1_close();
    //SPI0_close();
    //flash the DMP image
    ICM20948_image_load();
    SPI1_config_OK = SPI1_init(7000000);
    //SPI0_config_OK = SPI0_init(4000000);
    //wake up from sleep, set CLK for the gyro, disbale temp sensor
    ICM20948_pwr_mgmt_1(BIT_CLK_PLL|BIT_TMP_DIS);
    //wait
    PIN_registerIntCb(pinHandle, &ICM20948_IRQ_Handler);
    //switch on the gyro, disable the accelerometer
    ICM20948_pwr_mgmt_2(BIT_GYRO_ON_ACC_OFF);
    //duty cycle the gyro
    ICM20948_lp_config(BIT_GYRO_CYCLE);
    ICM20948_bank_select(BANK_02);
    //configure the gyro LPF at 200Hz, 250 degrees per second
    ICM20948_gyro_config1(BIT_FCHOICE);
    //set gyro sample rate: about 17Hz : 1.1kHz/ (1 + [7:0])
    writeRegister8(REG_GYRO_SMPLRT_DIV,0x20);
    //pause required here was it?
    ICM20948_bank_select(BANK_00);
    return true;
}

void ICM20948_ping() {
    uint8_t values[9];
    values[0] = readRegister8(REG_PWR_MGMT_1);
    values[1] = readRegister8(REG_PWR_MGMT_2);
    values[2] =  readRegister8(REG_LP_CONFIG);
    ICM20948_bank_select(BANK_02);
    values[3] = readRegister8(REG_GYRO_SMPLRT_DIV);
    values[4] = readRegister8(REG_GYRO_CONFIG_1);
    values[5] = readRegister8(REG_GYRO_CONFIG_2);
    ICM20948_bank_select(BANK_00);
    values[6] = readRegister8(REG_INT_PIN_CFG);
    values[7] = readRegister8(REG_INT_ENABLE_1);
    values[8] = readRegister8(REG_INT_STATUS_1);
}
