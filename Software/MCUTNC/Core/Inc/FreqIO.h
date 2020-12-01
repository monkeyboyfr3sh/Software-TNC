/*
 * FreqIO.h
 *
 *  Created on: Jul 21, 2020
 *      Author: monke
 *
 *      This file is dedicated to functions for creating and reading frequencies.
 */

#ifndef SRC_FREQIO_H_
#define SRC_FREQIO_H_
#include "main.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include "math.h"
#include "sine.h"
#include "AX.25.h"
#include "interrupt_services.h"
#include "FreqIO.h"

//Needed uController Objects
//****************************************************************************************************************
extern DAC_HandleTypeDef hdac;
extern DMA_HandleTypeDef hdma_dac1;
//Connectivity
//****************************************************************************************************************
extern char uartData[3000];

//General Program
//****************************************************************************************************************
/*
 * 	Bool used to determine if in TX or RX mode.
 * 	TX = 1
 * 	RX = 0
 *
 */
extern bool mode;

void initProgram(bool modeStart);
void setHardwareMode(int set_mode);

int loadBitBuffer(bool bit_val);
int readBitBuffer();
void resetBitBuffer();

//GENERATING FREQ
//****************************************************************************************************************
#define PI 					3.1415926
#define OUT_AMPL			4096
#define LOWF 				1200 		//This is the sample count for the low frequency , as configured maps to 1200Hz
#define HIGHF				2200		//This is the sample count for the high frequency, as configured maps to 2200Hz

extern bool 	midbit;
extern bool		changeMode;
extern bool		freqSelect;						//Tracks lasts state of output freq for NRZI encoding

void edit_sineval(uint32_t *sinArray, int arraySize, int waves, float shiftPercent);
int bitToAudio(bool *bitStream, int arraySize, bool direction,int wave_start);
void generateBitstream();
void initOUTData();

//READING FREQ
//****************************************************************************************************************
#define	BUFFER_SCALE		1024			//Scalar for buffer base
#define BIT_BUFF_BASE		16			//Base amount of bits to store
#define RX_BUFFERSIZE		2655

extern bool bitBuffer[RX_BUFFERSIZE];		//Stores bitstream values
extern bool		canWrite;
extern bool		canRead;
extern bool 	bufffull;
extern uint16_t bitSaveCount;				//Keep track of index to save period
extern uint16_t bitReadCount;				//Keep track of index to read period
extern uint16_t	signal_detect_decay;			//Pseudo timer to detect if value is valid
extern bool		signal_valid;					//Determines if frequency being read is a valid bit
extern uint16_t trackBit;						//Tracks bits being loaded into bit buffer
extern uint16_t bitSaveCount;					//Tracks bits being saved into bit buffer


#endif /* SRC_FREQIO_H_ */
