/*
 * FreqIO.c
 *
 *  Created on: Jul 21, 2020
 *      Author: monke
 */
#include "FreqIO.h"

//Needed uController Objects
//****************************************************************************************************************
DAC_HandleTypeDef hdac;
DMA_HandleTypeDef hdma_dac1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim5;
UART_HandleTypeDef huart2;
//Connectivity
//****************************************************************************************************************
char uartData[3000];

//General Program
//****************************************************************************************************************
bool mode;
bool midbit = false;
bool changeMode = false;

void initProgram(bool modeStart) {

	//Set hardware properly
	mode = modeStart;
	setHardwareMode(modeStart);
	gen_asin();
	init_UART();
}

void setHardwareMode(int set_mode) {

	//Set mode
	mode = set_mode;

	//Stop DAC
	HAL_DAC_Stop_DMA(&hdac, DAC_CHANNEL_1);
	midbit = false;

	//Stop Timers the Correct Way
	HAL_TIM_OC_Stop_IT(&htim2, TIM_CHANNEL_1);
	HAL_TIM_Base_Stop(&htim3);
	HAL_TIM_Base_Stop(&htim4);
	HAL_TIM_OC_Stop_IT(&htim5, TIM_CHANNEL_1);

	//Zero Timers
	htim2.Instance->CNT = 0;
	htim3.Instance->CNT = 0;
	htim5.Instance->CNT = 0;

	//Transmission Mode
	if (mode) {

		//Set Timer Auto Reload Settings
		htim2.Instance->ARR = TIM2_AUTORELOAD_TX;
		htim3.Instance->ARR = TIM3_AUTORELOAD_TX;
		htim4.Instance->ARR = TIM4_AUTORELOAD_TX_LOW;
		htim5.Instance->ARR = TIM5_AUTORELOAD_TX;

		//Start Timers the Correct Way
		HAL_TIM_Base_Start(&htim4);
	}

	//Receiving Mode
	else {

		//Set Timer Auto Reload Settings
		htim2.Instance->ARR = TIM2_AUTORELOAD_RX;
		htim3.Instance->ARR = TIM3_AUTORELOAD_RX;
		htim4.Instance->ARR = TIM4_AUTORELOAD_RX;
		htim5.Instance->ARR = TIM5_AUTORELOAD_RX;

//		//Start Timers the Correct Way
		HAL_TIM_OC_Start_IT(&htim2, TIM_CHANNEL_1);
		HAL_TIM_OC_Start_IT(&htim5, TIM_CHANNEL_1);
		__HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_1,1000);
	}
}

/*
 * Transmitting	: 				0x73 = 0111 0011 -> sent in this order [firstbit:lastbit] -> 11001110
 * Receiving[firstrx:lastrx] :	11001110 -> reverse it -> [MSB:LSB] 0111 0011
 *
 * Shifting method
 * 01234567
 * --------
 * 00000000
 * 10000000
 * 11000000
 * 01100000
 * 00110000
 * 10011000
 * 11001100
 * 11100110
 * 01110011
 *
 * Incrementing method
 * 01234567
 * --------
 * 00000000
 * 10000000
 * 11000000
 * 11000000
 * 11000000
 * 11001000
 * 11001100
 * 11001110
 * 11001110
 */

bool bufffull = false;
int loadBitBuffer(bool bit_val) {
	if(canWrite){
		bitBuffer[bitSaveCount] = bit_val;
		bitSaveCount++;
		if (bitSaveCount >= RX_BUFFERSIZE) {
			canWrite = false;
		}

		//Buffer is full
		if(bitSaveCount == bitReadCount){
			canWrite = false;
		}
	} else {
		bufffull = true;
	}
	canRead = true;
	return bitSaveCount;
}

int readBitBuffer(){
	int returnVal = -1;

	if(canRead){
		//Extract buffer value
		returnVal = bitBuffer[bitReadCount];

		if(returnVal == -1){
			sprintf(uartData, "End of incoming stream\n");
			debug_print_msg();
		}

		//Update period read
		bitReadCount++;
		if (bitReadCount >= RX_BUFFERSIZE) {
			canRead = false;
		}

		//Buffer is empty
		if(bitReadCount == bitSaveCount){
			canRead = false;
		}

	} else {
//		sprintf(uartData, "periodBuffer is empty\n");
//		debug_print_msg();
	}
	if(bufffull){
		sprintf(uartData, "bitBuffer is full; bitSaveCount = %d\n",bitSaveCount);
		debug_print_msg();
	}
	bufffull = false;
	canWrite = true;
	return returnVal;
}
void resetBitBuffer(){
	bitReadCount = 0;
	bitSaveCount = 0;

	canRead  = false;
	canWrite = true;

	bufffull = false;
}

//GENERATING FREQ
//****************************************************************************************************************
bool bitStream[10];
bool freqSelect = false;

void edit_sineval(uint32_t *sinArray, int arraySize, int waves, float shiftPercent) {
	double ampl 		= OUT_AMPL / 2;						//Amplitude of wave
	double phaseShift 	= shiftPercent * 2 * PI;	//Desired phase shift
	double w 			= 2 * PI  * waves / arraySize;

	for (int i = 0; i < arraySize; i++) {
		//formula in DAC Document
		sinArray[i] = (sin((i * w) + phaseShift) + 1) * ampl;
		sprintf(uartData, "sinArray[%d] = %d\n",i,sinArray[i]);
		debug_print_msg();
	}
}

int bitToAudio(bool *bitStream, int arraySize, bool direction,int wave_start) {
	bool changeFreq;
	int waveoffset = wave_start;
	for (int i = 0; i < arraySize; i++) {
		//Check if freq needs to be changed for NRZI
		if(direction){
			changeFreq = bitStream[i];
		} else {
			changeFreq = bitStream[arraySize - i - 1];
		}

		//NRZ
		//freqSelect = !changeFreq;

		//NRZI
		freqSelect = (changeFreq) ? freqSelect : !freqSelect;

//		HAL_GPIO_WritePin(GPIOB, D4_Pin, changeFreq);
//		HAL_GPIO_WritePin(GPIOB, D4_Pin, freqSelect);

		if (freqSelect) {
			htim4.Instance->ARR = TIM4_AUTORELOAD_TX_HIGH;
			waveoffset = (1.0 * FREQ_SAMP) * (1.0 * HIGHF) / (1.0 * LOWF);
		}
		else {
			htim4.Instance->ARR = TIM4_AUTORELOAD_TX_LOW;
			waveoffset = (1.0 * FREQ_SAMP) * (1.0 * LOWF) / (1.0 * LOWF);
		}

		HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_1, (wave+wave_start), FREQ_SAMP, DAC_ALIGN_12B_R);
		htim3.Instance->CNT = 0;
		HAL_TIM_Base_Start_IT(&htim3);

		//Calculate ending point for wave
		wave_start = (wave_start+waveoffset+1)%FREQ_SAMP;

		midbit = true;
		while (midbit){
			//In the future this leaves the CPU free for scheduling or something
			__NOP();
		}

	}

	HAL_TIM_Base_Stop(&htim3);
	return wave_start;
}

//READING FREQ
//****************************************************************************************************************
bool bitBuffer[RX_BUFFERSIZE];
bool		canWrite = true;
bool		canRead  = false;
uint16_t bitSaveCount = 0;
uint16_t bitReadCount = 0;
uint16_t	signal_detect_decay = 0;			//Pseudo timer to detect if value is valid
bool		signal_valid = false;					//Determines if frequency being read is a valid bit
uint16_t trackBit = 0;
