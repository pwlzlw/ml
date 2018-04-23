/*
 * machine_learning.h
 * Author: Pawel Zalewski
 * Date:   29/11/2017
 *
 * Header file for the encoder and function prototypes.
 */

#ifndef APPLICATION_MACHINE_LEARNING_H_
#define APPLICATION_MACHINE_LEARNING_H_
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "comdef.h"
#include "math.h"
#define WINDOW_SIZE 32
#define SAMPLES_COUNT 96
#define FEATURE_NUMBER 13
#define NUM_Trees 10
#include <xdc/runtime/Timestamp.h>
#include <xdc/runtime/Types.h>
//bit fields for classification byte

typedef enum {
    Sed     = 0b00<<6,
    Mod     = 0b01<<6,
    Rig     = 0b11<<6,
    Sit     = 0b00<<4,
    Liy     = 0b01<<4,
    Sta     = 0b11<<4,
    Wal     = 0b00<<2,
    TuL     = 0b01<<2,
    TuR     = 0b11<<2,
    Jum     = 0b00,
    Run     = 0b01,
    Exe     = 0b11,
    Xn      = 0b10
} activity_t;


//typedef struct activity_s activity_s;

void computeFV(uint8_t*,float*,uint8_t);
//uint8_t computeFeatures(uint8_t*);
void intoInteger16(uint8_t *input, int16_t *output, uint8_t);
float getIMA(int16_t *);
float getMU(int16_t *, uint8_t);
float getMED(int16_t *, uint8_t);
float getMIN(int16_t *, uint8_t);
float getMAX(int16_t *, uint8_t);
float getVAR(int16_t*, uint8_t, float);

static void assignProb(uint16_t a, uint16_t b ,uint16_t c, uint16_t* hist);


void stage1_RF(float*,uint16_t*);
activity_t evalS1(uint16_t s, uint16_t m, uint16_t r);
uint16_t* stage1_t1(float*);
uint16_t* stage1_t2(float*);
uint16_t* stage1_t3(float*);
uint16_t* stage1_t4(float*);
uint16_t* stage1_t5(float*);
uint16_t* stage1_t6(float*);
uint16_t* stage1_t7(float*);
uint16_t* stage1_t8(float*);
uint16_t* stage1_t9(float*);
uint16_t* stage1_t10(float*);

void stage2_S(float*, uint16_t*);
activity_t evalS2(uint16_t s, uint16_t m, uint16_t r);
uint16_t* stage2_S_t1(float*);
uint16_t* stage2_S_t2(float*);
uint16_t* stage2_S_t3(float*);
uint16_t* stage2_S_t4(float*);
uint16_t* stage2_S_t5(float*);
uint16_t* stage2_S_t6(float*);
uint16_t* stage2_S_t7(float*);
uint16_t* stage2_S_t8(float*);
uint16_t* stage2_S_t9(float*);
uint16_t* stage2_S_t10(float*);

void stage2_MA(float*,uint16_t*);
activity_t evalMA(uint16_t s, uint16_t m, uint16_t r);
uint16_t* stage2_MA_t1(float*);
uint16_t* stage2_MA_t2(float*);
uint16_t* stage2_MA_t3(float*);
uint16_t* stage2_MA_t4(float*);
uint16_t* stage2_MA_t5(float*);
uint16_t* stage2_MA_t6(float*);
uint16_t* stage2_MA_t7(float*);
uint16_t* stage2_MA_t8(float*);
uint16_t* stage2_MA_t9(float*);
uint16_t* stage2_MA_t10(float*);

void stage2_MG(float*,uint16_t*);
activity_t evalMG(uint16_t s, uint16_t m, uint16_t r);
uint16_t* stage2_MG_t1(float*);
uint16_t* stage2_MG_t2(float*);
uint16_t* stage2_MG_t3(float*);
uint16_t* stage2_MG_t4(float*);
uint16_t* stage2_MG_t5(float*);
uint16_t* stage2_MG_t6(float*);
uint16_t* stage2_MG_t7(float*);
uint16_t* stage2_MG_t8(float*);
uint16_t* stage2_MG_t9(float*);
uint16_t* stage2_MG_t10(float*);


void stage2_R(float*,uint16_t*);
activity_t evalR(uint16_t s, uint16_t m, uint16_t r);
uint16_t* stage2_R_t1(float*);
uint16_t* stage2_R_t2(float*);
uint16_t* stage2_R_t3(float*);
uint16_t* stage2_R_t4(float*);
uint16_t* stage2_R_t5(float*);
uint16_t* stage2_R_t6(float*);
uint16_t* stage2_R_t7(float*);
uint16_t*  stage2_R_t8(float*);
uint16_t* stage2_R_t9(float*);
uint16_t* stage2_R_t10(float*);

#endif /* APPLICATION_MACHINE_LEARNING_H_ */
