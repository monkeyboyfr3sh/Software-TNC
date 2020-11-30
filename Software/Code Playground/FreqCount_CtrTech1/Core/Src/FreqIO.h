/*
 * FreqIO.h
 *
 *  Created on: Jul 21, 2020
 *      Author: monke
 */

#ifndef SRC_FREQIO_H_
#define SRC_FREQIO_H_
#include "main.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include "math.h"

#define DEBUG true

#define PI 3.1415926
#define LOWF	84 //This is the sample count for the low frequency , as configured maps to 1200Hz
#define HIGHF	46	//This is the sample count for the high frequency, as configured maps to 2200Hz

#define PCONVERT 		10000000	//f = 1/T, used for converting period to frequency
#define HIGHFREQ 		2200	//Higher freq to detect w/ afsk
#define LOWFREQ			1200	//Lower freq to detect w/ afsk
#define FREQDEV			50		//Max potential deviation in target frequency to detect

#define BUFFERPERIOD	416		//Period for timer3, this should sample input fast enough to receive @1200Hz.
#define BUFFERSIZE		50

extern DAC_HandleTypeDef hdac;
extern TIM_HandleTypeDef htim2;
extern UART_HandleTypeDef huart2;

extern char uartData[3000];
extern uint32_t periodBuffer[BUFFERSIZE];
extern uint16_t buffLoadCount;
extern uint32_t period;
extern bool first;

//Sine Arrays
extern uint32_t sine_val[100];
extern uint32_t lowFrequency[LOWF];
extern uint32_t highFrequency[HIGHF];

void get_sineval();
void edit_sineval(uint32_t *sinArray,int arraySize);
void bitToAudio(bool *bitStream, int arraySize);

/*
 *	Function to convert freq to bitstream:
 *		returns 1 if detect higher freq within threshold
 *		returns 0 if detect lower freq within threshold
 *		return if frequency is outside afsk set range
 */
int freqtobit(uint32_t inputPeriod);

/*
 * 	Function to be ran at external trigger.
 */
void FreqCounterPinEXTI();

/*
 * 	Function to be ran at Tim3 interrupt
 */
void Tim3IT();

#endif /* SRC_FREQIO_H_ */
