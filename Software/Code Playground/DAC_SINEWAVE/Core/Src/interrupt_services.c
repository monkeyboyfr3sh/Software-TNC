/*
 * interrupt_services.c
 *
 *  Created on: Nov 5, 2020
 *      Author: monke
 */

#include "interrupt_services.h"

uint8_t captured_bits_count = 0;	//How many captured bits since last clk sync
bool clk_sync = false;			//Are we synced with clock
int measure_val;

uint32_t rising_capture = 0;		// stores the timer tick value of the most recent rising edge
uint32_t falling_capture = 0;	// stores the timer tick value of the most recent falling edge
uint32_t capture_difference = 0;
bool rise_captured = false;		// these are used to ensure that we aren't trying to compute the difference
bool fall_captured = false;		// before we have captured both a rising and falling edge
bool signal_edge = RISING_EDGE;	// so we know what edge we are looking for (really, the opposite of the edge that was captured last

bool freq_pin_state_curr = false;
bool freq_pin_state_last = false;

bool hold_state;
bool NRZI;

bool process_bit_buffer = false;
int byteArray[8];
bool got_flag_start = false;
bool got_flag_end = false;

const int samp_per_bit_margin = SYMBOL_MARGIN/samp_per_bit;

//Timer 2 Output Compare Callback
void Tim2_OC_Callback(){
	static int save_cnt;
	static int flag_cnt;
	bool isFlag = false;

	freq_pin_state_last = hold_state;

	//Check if this is valid data
	if(clk_sync){
		NRZI = (freq_pin_state_curr==freq_pin_state_last) ? 1 : 0;

//		HAL_GPIO_WritePin(GPIOB,D3_Pin,NRZI);

		//Shift byte array for next comparison
//		memmove(&byteArray[1],&byteArray[0],7*sizeof(int));
		for(int i = 7;i>0;i--){
			byteArray[i] = byteArray[i-1];
		}

		byteArray[0] = NRZI;
		//01234567
		//00000000(byteArray will be filled with 0)
		//00000000
		//10000000
		//11000000
		//11100000
		//11110000
		//11111000
		//11111100
		//01111110


		//Check if this is the flag
		for (int i = 0; i < 8; i++) {
			if(byteArray[i] != AX25TBYTE[i]){
				isFlag = false;
				break;
			}
			//Got to end of byte array
			if(i==7){
				isFlag = true;
			}
		}
		//If this is not a flag, copy the values into the buffer pointer
		if(isFlag){
			flag_cnt++;

			//If we have a start flag, this is an end flag
			if(got_flag_start){
				got_flag_start = false;
				got_flag_end = true;
			}

			//Not sure how many appending flags????????
			else if(flag_cnt>=FLAG_END_COUNT){
				//If no start flag has occurred
				if(!got_flag_start){
					got_flag_start = true;
				}

				//Reset flag count
				flag_cnt = 0;
			}

			//Reset flag var
			isFlag = false;
		}

		else if(got_flag_start){
//			HAL_GPIO_TogglePin(GPIOA,D0_Pin);
			//Load the processed bit into the buffer
			save_cnt = loadBitBuffer(NRZI)+1;
		}

		//Found ending flag, now need to process bit buffer
		if(got_flag_end){
			got_flag_end = false;
			HAL_GPIO_TogglePin(GPIOA,D0_Pin);

			//Disable Interrupts for data processing
			HAL_TIM_OC_Stop_IT(&htim2, TIM_CHANNEL_1);
			HAL_TIM_OC_Stop_IT(&htim5, TIM_CHANNEL_1);

			//Buffer will be filled with ending flags, dont want this in ax.25 buffer
			save_cnt -= FLAG_SIZE;
			rxBit_count = save_cnt;//Must be before remove_bit_stuffing

//			sprintf(uartData, "byte_cnt = %d\n",global_packet.byte_cnt);
//			debug_print_msg();

			memcpy(global_packet.AX25_PACKET,bitBuffer,save_cnt);

//			compareBoolBuffers(bitBuffer,global_packet.AX25_PACKET,rxBit_count);

			sprintf(uartData, "rxBit_count (before bit destuff) = %d\n",rxBit_count);
			debug_print_msg();

			remove_bit_stuffing();

			sprintf(uartData, "rxBit_count (after bit destuff) = %d\n",rxBit_count);
			debug_print_msg();


			global_packet.byte_cnt = rxBit_count/8;

			//Receive data
			receiving_AX25();

			save_cnt = 0;

			//Enable Interrupts since data processing is complete
			HAL_TIM_OC_Start_IT(&htim2, TIM_CHANNEL_1);
			HAL_TIM_OC_Start_IT(&htim5, TIM_CHANNEL_1);
		}

		//Prepare OC for next sample
		uint32_t this_capture = __HAL_TIM_GET_COMPARE(&htim2, TIM_CHANNEL_1);
		uint32_t next_sampl = this_capture + bit_sample_period;
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1,next_sampl); // if we have not received a transition to the input capture module, we want to refresh the output compare module with the last known bit period
	}

	//Clock not syncd
	else
	{
		got_flag_start = false;
		got_flag_end = false;
		flag_cnt = 0;
	}

	//Inc number of bits since last clock sync
	captured_bits_count++;
	if(captured_bits_count >= samp_per_bit * no_clk_max_cnt){
		clk_sync = false;	//Clock is no longer sync
	}
	hold_state = freq_pin_state_curr;

	return;
}
void Tim3_IT_Callback() {
	if (mode) {
		midbit = false;
	}
}

#define PI 3.14159265
const int SAMP_COUNT = 775;
const int freq_deviation = 2382;

double phase_curr,phase_prev;
int freq_rad;
double x1,x2;

int curr_time,prev_time,diff_time;
int freq;
float buffer[1024];
int count = 0;
int adc_val;

const int max_invalid = 3;
const int min_valid = 3;
int valid_freq_low_count = 0;
int valid_freq_high_count = 0;
int invalid_freq_count = 0;
bool prev_freq_state = false;
bool curr_freq_state = false;		//Interpreted freq
bool invalid_freq = false;

//Timer 5 Output Capture Callback
void Tim5_OC_Callback(){
	prev_freq_state = curr_freq_state;
//	HAL_GPIO_WritePin(GPIOA,D0_Pin,curr_freq_state);
	//Log Values
	prev_time = curr_time;
	phase_prev = phase_curr;

	//Get ADC Val
	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1,5);
	int adcval = HAL_ADC_GetValue(&hadc1);

	//Capture time
	curr_time = htim5.Instance->CNT;

	//Calculate freq
	phase_curr = asin_lut[adcval];
	freq_rad = (phase_curr-phase_prev)/(curr_time-prev_time);

	//+ Low frequency
	if(7539-freq_deviation <freq_rad && freq_rad < 7539+freq_deviation ){

		//Reset invalid
		invalid_freq_count = 0;

		//Inc number of low frequencies
		valid_freq_low_count++;
		//Got enough valid frequencies to probably call this a low
		if(valid_freq_low_count>=min_valid){
			//Reset high count
			valid_freq_high_count = 0;

			//Set frequency status
			curr_freq_state  = false;
			invalid_freq = false;
		}
	}
	//- Low frequency
	else if(-7539-freq_deviation <freq_rad && freq_rad < -7539+freq_deviation ){

		//Reset invalid
		invalid_freq_count = 0;

		//Inc number of low frequencies
		valid_freq_low_count++;
		//Got enough valid frequencies to probably call this a low
		if(valid_freq_low_count>=min_valid){
			//Reset high count
			valid_freq_high_count = 0;

			//Set frequency status
			curr_freq_state  = false;
			invalid_freq = false;
		}
	}
	//+ High frequency
	else if(13823-freq_deviation <freq_rad && freq_rad < 13823+freq_deviation ){

		//Reset invalid
		invalid_freq_count = 0;

		//Inc number of low frequencies
		valid_freq_high_count++;
		//Got enough valid frequencies to probably call this a low
		if(valid_freq_high_count>=min_valid){
			//Reset low count
			valid_freq_low_count = 0;

			//Set frequency status
			curr_freq_state  = true;
			invalid_freq = false;
		}
	}
	//- High frequency
	else if(-13823-freq_deviation <freq_rad && freq_rad < -13823+freq_deviation ){

		//Reset invalid
		invalid_freq_count = 0;

		//Inc number of low frequencies
		valid_freq_high_count++;
		//Got enough valid frequencies to probably call this a low
		if(valid_freq_high_count>=min_valid){
			//Reset low count
			valid_freq_low_count = 0;

			//Set frequency status
			curr_freq_state  = true;
			invalid_freq = false;
		}
	}
	//Invalid frequencies
	else {
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,0);
		invalid_freq_count++;
		if(invalid_freq_count>=max_invalid){
			invalid_freq = true;
			valid_freq_high_count = 0;
			valid_freq_low_count = 0;
		}
	}
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,!invalid_freq);

	//Should look like binary
	if(curr_freq_state!=prev_freq_state){
		FreqEdgeDetection(curr_time);
	}

	uint32_t next_sampl = curr_time + SAMP_COUNT;
	__HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_1,next_sampl);
}
void FreqEdgeDetection(int edgeTime){
	uint32_t this_capture = 0;

	//Grap pin state for OC timer
	freq_pin_state_curr = signal_edge;

	//Rising Edge
	if (signal_edge)
	{
		rising_capture = edgeTime; //Time-stamp interrupt
		signal_edge = FALLING_EDGE;		// look for falling edge on next capture
		rise_captured = true;

		if (rise_captured && fall_captured)
		{
			capture_difference = rising_capture - falling_capture;		// calculate difference
			this_capture = rising_capture;		// set current sample to rising edge
		}
	}

	//Falling edge
	else
	{
		falling_capture = edgeTime;		//Time-stamp interrupt
		fall_captured = true;
		signal_edge = RISING_EDGE;		// look for rising edge on next capture

		if (rise_captured && fall_captured)
		{
			capture_difference = falling_capture - rising_capture;		// calculate difference
			this_capture = falling_capture;
		}
	}

	//Have now captured the transition period
	//Can use this to align sampling clock
	if (rise_captured && fall_captured)
	{
		//Check if the transition was a valid transition period to use
		if(SYMBOL_PERIOD-SYMBOL_MARGIN < capture_difference && capture_difference < SYMBOL_PERIOD+SYMBOL_MARGIN){

			//Predict clock
			uint32_t next_sampl;

			//If clk was not sync, start sample one period later
			if(!clk_sync){
				resetBitBuffer();
				next_sampl = this_capture + SYMBOL_PERIOD/2;
			}
			//If clk was sync, sample at normal interval
			else {
				next_sampl = this_capture + bit_sample_period/2;
			}
			//Prepare OC timer int
			__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, next_sampl);
			//Reset roll-over value
			captured_bits_count = 0;

			//Have now synced with clock
			clk_sync = true;
		}
	}
}

void init_UART(){
	HAL_UART_Receive_IT(&huart2, &(UART_packet.input), UART_RX_IT_CNT);
	UART_packet.flags = 0;
	UART_packet.got_packet = false;
	UART_packet.rx_cnt = 0;
	UART_packet.received_byte_cnt = 0;
}
void UART2_Exception_Callback(){
	HAL_UART_Receive_IT(&huart2, &(UART_packet.input), UART_RX_IT_CNT);//Reset
	UART_packet.got_packet = false;

	  if(UART_packet.input==0xc0){
		  UART_packet.flags++;
	  }

	  *(UART_packet.HEX_KISS_PACKET+UART_packet.rx_cnt) = UART_packet.input;
	  UART_packet.rx_cnt++;

	  if(UART_packet.flags>=2){
		  setHardwareMode(1);
		  UART_packet.got_packet = true;
		  UART_packet.received_byte_cnt = UART_packet.rx_cnt;



		  UART_packet.flags = 0;
		  UART_packet.rx_cnt=0;

	  }
}
