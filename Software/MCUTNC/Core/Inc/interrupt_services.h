/*
 * interrupt_services.h
 *
 *  Created on: Nov 5, 2020
 *      Author: monke
 */

#ifndef SRC_INTERRUPT_SERVICES_H_
#define SRC_INTERRUPT_SERVICES_H_

#include "main.h"
#include "stdbool.h"
#include "interrupt_services.h"
#include "FreqIO.h"
#include "AX.25.h"
#include "debug.h"

extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim5;
extern UART_HandleTypeDef huart2;

#define RISING_EDGE		1
#define FALLING_EDGE	0
// this assumes that the timer is ticking at 40 MHz (80 MHz input with prescaler set to 1)
// these values to be used with prescaler = 1
#define SYMBOL_PERIOD 33333
#define SYMBOL_MARGIN 3333

//Timer TX reload definitions
#define TIM2_AUTORELOAD_TX		0			//Timer2 period, NOT USED IN THIS MODE
#define TIM3_AUTORELOAD_TX		828 		//Timer3 period, used to controller baudrate for TX
#define TIM4_AUTORELOAD_TX_LOW	108			//Timer4 period, used to control DAC and generate 1200Hz, assuming 40Mhz clk
#define TIM4_AUTORELOAD_TX_HIGH	59			//Timer4 period, used to control DAC and generate 2200Hz, assuming 40Mhz clk
#define TIM5_AUTORELOAD_TX		0			//Timer5 period, NOT USED IN THIS MODE

//Timer RX reload definitions
#define TIM2_AUTORELOAD_RX	4294967295		//Timer2 period, max value for output compare to count up to
#define TIM3_AUTORELOAD_RX	0				//Timer3 period, NOT USED IN THIS MODE
#define TIM4_AUTORELOAD_RX	0				//Timer4 period, NOT USED IN THIS MODE
#define TIM5_AUTORELOAD_RX	4294967295		//Timer5 period, input capture needs to have the same max count as tim2 output compare
#define samp_per_bit 		1	//Will take the digital reading multiple times per bit length
#define no_clk_max_cnt		64	//How many bit lengths can occur without disabling clock sync
#define bit_sample_period 	SYMBOL_PERIOD / samp_per_bit

extern uint8_t captured_bits;	//How many captured bits since last clk sync
extern bool clk_sync;			//Are we synced with clock

extern bool freq_pin_state_curr;
extern bool freq_pin_state_last;

extern bool hold_state;
extern bool NRZI;
/*
 * 	Function to be ran at Tim3 interrupt
 */
void Tim3_IT_Callback();

void FreqEdgeDetection(int edgeTime);

void init_UART();
void UART2_Exception_Callback();


#endif /* SRC_INTERRUPT_SERVICES_H_ */
