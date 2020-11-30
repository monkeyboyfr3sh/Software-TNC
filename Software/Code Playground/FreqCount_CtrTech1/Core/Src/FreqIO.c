/*
 * FreqIO.c
 *
 *  Created on: Jul 21, 2020
 *      Author: monke
 */
#include "FreqIO.h"

DAC_HandleTypeDef hdac;
TIM_HandleTypeDef htim2;
UART_HandleTypeDef huart2;

char uartData[3000];
uint32_t periodBuffer[BUFFERSIZE];
uint16_t buffLoadCount = 0;
bool first = false;
int edge_stamp = 0;
uint32_t period1;
uint32_t period2;

//Sine Arrays
uint32_t sine_val[100];
uint32_t lowFrequency[LOWF];
uint32_t highFrequency[HIGHF];

void get_sineval(){
	for (int i=0;i<100;i++){
		//formula in DAC Document
		sine_val[i] = ((sin(i*2*PI/100)+1)*(4096/2));
	}
}
void edit_sineval(uint32_t *sinArray,int arraySize){
	for (int i=0;i<arraySize;i++){
		//formula in DAC Document
		sinArray[i] = ((sin((i-45)*2*PI/arraySize)+1.1)*(4096/4));
	}
}
void bitToAudio(bool *bitStream, int arraySize){
	for (int i=0;i<arraySize;i++){
		//if(bitStream[i])HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5, GPIO_PIN_SET);
		//else 			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5, GPIO_PIN_RESET);
		if(bitStream[i]) HAL_DAC_Start_DMA(&hdac,DAC_CHANNEL_1,lowFrequency,LOWF,DAC_ALIGN_12B_R);
		else HAL_DAC_Start_DMA(&hdac,DAC_CHANNEL_1,highFrequency,HIGHF,DAC_ALIGN_12B_R);
		HAL_Delay(0.5);
		HAL_DAC_Stop_DMA(&hdac, DAC_CHANNEL_1);
	}
}

int freqtobit(uint32_t inputPeriod){
	int freq = PCONVERT / (period1+period2);
	return freq;
//	if( (HIGHFREQ-FREQDEV < freq) && (freq < HIGHFREQ+FREQDEV) )
//		return 1;
//	if( (LOWFREQ-FREQDEV < freq) && (freq < LOWFREQ+FREQDEV) )
//		return 0;
//	else
//		return -1;
//	return freq;
}

void FreqCounterPinEXTI(){
	//HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_5);
	if(edge_stamp == 0){
		period1 = 0;
		period2 = 0;
		htim2.Instance->CNT = 0;
		edge_stamp++;
//		first = true;
	}
	else if(edge_stamp == 1) {
		period1 = htim2.Instance->CNT;
		htim2.Instance->CNT = 0;
		edge_stamp++;
//		first = false;
	}
	else{
		period2 = htim2.Instance->CNT;
		edge_stamp = 0;
	}
}

void Tim3IT(){
	//HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_5);
	periodBuffer[buffLoadCount] = period1+period2;
	buffLoadCount++;
	if(buffLoadCount>=BUFFERSIZE){
		buffLoadCount = 0;
	}
}
