/*
 * STM32_TNC.c
 *
 *  Created on: Nov 21, 2017
 *      Author: nje
 */

#include "STM32_TNC.h"
//#include <ax5043.h>
#include <main.h>
//#include <stm32f4xx_hal.h>
//#include <stm32f4xx_hal_gpio.h>
//#include <stm32f4xx_hal_uart.h>
#include <sys/_stdint.h>
//#include <Serial.h>
#include <usart.h>
//#include "defines.h"
#include "stdbool.h"
//#include "Radio.h"
//#include "CAPE-3_Status_Vars.h"
//#include "beacon.h"
//#include "stm_power_ina219.h"



uint8_t tnc_buff[1024];
uint16_t pkt_len = 0;
uint8_t tnc_message[1];

uint8_t tnc_array[1024];
uint8_t tnc_cmd_array[1024];
uint16_t last_tnc_count = 0;
uint16_t last_tnc_cmd_len = 0;


int TNC_Tx(uint8_t *message, uint16_t msg_len)
{

	uint8_t tnc_msg[400];
	uint16_t current_index = 0;


	//Header bytes

//	if (current_ax->RADIO_TYPE == 'u')
//	{
//		TNC_UHF
//		if (enable_uhf_hp)
//		{
//			RADIO_FE_NORMAL
//		}
//		RADIO_UHF_TX
//	}
//	else if (current_ax->RADIO_TYPE == 'v')
//	{
//		TNC_VHF
//		if (enable_vhf_hp)
//		{
//			RADIO_FE_HP
//		}
//		RADIO_VHF_TX
//	}

	/*
	 * SSID format: CRRSSID0
	 *
	 * TNC Header Format:
	 * 0: 0xC0: Delimiting Character
	 * 1: 0x00: remainder of frame is HDLC DATA
	 * 			DESTINATION ADDRESS: "W5UL  1"
	 * 2: 0xAE: 'W' << 1 (0x57)
	 * 3: 0x6A (j): '5' << 1 (0x35)
	 * 4: 0xAA: 'U' << 1 (0x55)
	 * 5: 0x98: 'L' << 1 (0x4C)
	 * 6: 0x40: ' ' << 1 (0x20)
	 * 7: 0x40: ' '		 (0x20)
	 * 8: 0x62 (b):  0110 0010 (response, res, res, 0001)
	 * 			SOURCE ADDRESS: "W5UL  0"
	 * 9: 0xAE: 'W' << 1
	 * 10: 0x6A: '5' << 1
	 * 11: 0xAA: 'U' << 1
	 * 12: 0x98: 'L' << 1
	 * 13: 0x40: ' ' << 1
	 * 14: 0x40: ' ' << 1
	 * 15: 0xF7:	1111 0111 (command, res, res, 1011)
	 * 16: 0x03:	Control field (Unnumbered information frame)
	 * 17: 0xF0:	PID field (no layer 3 protocol implemented)
	 *
	 *
	 *
	 *
	 */



	uint8_t tnc_header[18] = {0xC0, 0x00, 0xAE, 0x6A, 0xAA, 0x98, 0x40, 0x40, 0x62, 0xAE, 0x6A, 0xAA, 0x98, 0x40, 0x40, 0xF7, 0x03, 0xF0};
	uint8_t end_char[1] = {0xC0};

	//	TerminalWrite("Sending test to TNC:"); NL();
	//	TerminalWrite(tnc_header); NL();

//	if (current_ax->ax_fm_mode != 't')
//	{
//		AX_FM_TX();
//
//		//PulseWDT();
//
//		HAL_Delay(500);
//		//			HAL_Delay(10);
//	}
//	else
//	{
//		TerminalWrite("AX5043 already in FM TX mode!"); NL();
//	}


	//PulseWDT();

//	HAL_Delay(1);
//
//	int16_t urb_v_trans = getBusVoltage_mV(URB_INA_ADD);
//	HAL_Delay(1);
//	int16_t urb_i_trans = getCurrent_mA(URB_INA_ADD);
//	HAL_Delay(1);

	//PulseWDT();
//
//	if (message[22] == 'B')
//	{
//		//		packet_length = 201;
//
//		memcpy(message + BEACON_PKT_URB_V_TRANS, Split16Bit((uint16_t)urb_v_trans), 2);
//
//		memcpy(message + BEACON_PKT_URB_I_TRANS, Split16Bit((uint16_t)urb_i_trans), 2);
//
//		//		TerminalWrite("URB Transmit: ");
//		//		TerminalNum(urb_v_trans); TerminalWrite(" mV, ");
//		//		TerminalNum(urb_i_trans); TerminalWrite(" mA."); NL();
//
//
//	}



//	NL();
	// perform escaping
	for (uint16_t i = 0; i < msg_len; i ++)
	{
		//		TerminalHex(message[i]);
		if (message[i] == 0xC0)
		{

			//		TerminalHex(message[i]);
			//		TerminalNum(i);
			//		TerminalWrite(" Escaping FEND...   ");
			tnc_msg[current_index] = 0xDB;
			current_index ++;
			tnc_msg[current_index] = 0xDC;
		}
		else if (message[i] == 0xDB)
		{
			//		TerminalHex(message[i]);
			//		TerminalNum(i);
			//		TerminalWrite("Escaping FESC...   ");
			tnc_msg[current_index] = 0xDB;
			current_index ++;
			tnc_msg[current_index] = 0xDD;
		}
		else
		{
			tnc_msg[current_index] = message[i];

		}
		current_index ++;
	}

//	NL(); NL();
//	TerminalWrite("Length: "); TerminalNum(current_index); NL();




	//	     HAL_UART_Transmit(&huart2, startHeader, 2, 50);
	//
	//	     HAL_UART_Transmit(&huart2, destination, 7, 50);
	//
	//	     HAL_UART_Transmit(&huart2, callsign, 7, 50);
	//
	//	     HAL_UART_Transmit(&huart2, digipeat, 7, 50);
	//
	//	     HAL_UART_Transmit(&huart2, endHeader, 2, 50);

	HAL_UART_Transmit(&huart1, tnc_header, 18, 50);

	HAL_UART_Transmit(&huart1, tnc_msg, current_index, 500);

	HAL_UART_Transmit(&huart1, end_char, 1, 50);

//	do
//	{
//
//		HAL_Delay(1);
//	}
//
//	while (!(HAL_GPIO_ReadPin(TNC_PTT_GPIO_Port, TNC_PTT_Pin)));
//
//	do
//	{
//
//		//PulseWDT();
//		HAL_Delay(1);
//	}
//
//	while ((HAL_GPIO_ReadPin(TNC_PTT_GPIO_Port, TNC_PTT_Pin)));


//	HAL_Delay(500);

	//AX_FM_RX();

//	AX_FM_EXIT();

//	TerminalWrite("TNC Transmission Complete."); NL();


//	RADIO_FE_LP
//	RADIO_VHF_RX
//	RADIO_UHF_RX








	return current_index;


}
//
//uint16_t TNC_Rx()
//{
//
//#ifdef PRINT_TNC_RX
//
////	TerminalWrite("TNC message received. Total length: "); TerminalNum(last_tnc_count); NL(); NL();
////
////	//			TerminalWrite(tnc_array);
////	TerminalWrite("Source Address:      ");
////
////	for (uint8_t i = 2; i < 8; i ++)
////	{
////		TerminalChar(tnc_array[i] >> 1);
////	}
////	uint8_t source_ssid_byte = (tnc_array[8] >> 1);
////	TerminalWrite(" SSID: "); TerminalHex(source_ssid_byte); TerminalWrite(" -> ");
////
////	bool source_ssid[4] = {(source_ssid_byte & 0x08),(source_ssid_byte & 0x04),(source_ssid_byte & 0x02),(source_ssid_byte & 0x01)};
////	for (uint8_t i = 0; i < 4; i ++)
////	{
////		TerminalNum(source_ssid[i]);
////	}
////	NL();
////
////	TerminalWrite("Destination Address: ");
////	for (uint8_t i = 9; i < 15; i ++)
////	{
////		TerminalChar(tnc_array[i] >> 1);
////	}
////
////	uint8_t dest_ssid_byte = (tnc_array[15] >> 1);
////	TerminalWrite(" SSID: "); TerminalHex(dest_ssid_byte); TerminalWrite(" -> ");
////
////	bool dest_ssid[4] = {(dest_ssid_byte & 0x08),(dest_ssid_byte & 0x04),(dest_ssid_byte & 0x02),(dest_ssid_byte & 0x01)};
////	for (uint8_t i = 0; i < 4; i ++)
////	{
////		TerminalNum(dest_ssid[i]);
////	}
////	NL();
////	NL();
////	//	for (uint8_t i = 0; i < 18; i ++)
////	//		{
////	//			TerminalChar((char)tnc_array[i]);
////	//		}
////
////
////
////	//	HAL_Delay(2500);
////
////	//
////
////	TerminalWrite("Message: ");
//	for (uint16_t i = 18; i < (last_tnc_count - 1); i ++)
//	{
//		TerminalChar((char)tnc_array[i]);
//	}
//
//	NL(); NL();
//
//#endif
//
//
//
//	if ((tnc_array[18] == '*'))
//	{
//		TerminalWrite("TNC Command Header Received..."); NL();
//		for (int j = last_tnc_count; j > 0; j --)
//		{
//			if ((tnc_array[(j)] == '#'))
//			{
//				TerminalWrite("TNC Command Footer Received!"); NL();
//				last_tnc_cmd_len = (j - 2);
//				memcpy(tnc_cmd_array, (tnc_array + 19), last_tnc_cmd_len);
//				//				tnc_cmd_array[(last_tnc_cmd_len)] = 0;
//				//						last_ax_length = (j - 2);
//				radio_operations_command_received_flag = TRUE;
//				last_rx_radio = 't';
//
//				if (strncmp(tnc_cmd_array, "reset", 5) == 0)
//				{
//					//					enable_wdt = FALSE;
//				}
//				break;
//			}
//		}
//	}
//
//
//
//	//	memset(tnc_array, 0, sizeof(tnc_array));
//
//
//
//	TNC_received_flag = FALSE;
//
////	for (int i = 0; i < 1024; i ++)
////	{
////		tnc_array[i] = 0;
////	}
//
//
//	return last_tnc_count;
//
//
//}



//void TNC_PutHeader(uint8_t *arry)
//{
//	if (arry[0] == 0xC0)
//	{
//		arry[0] = '%';
//	}
//	if (arry[1] == 0)
//	{
//		arry[1] = ' ';
//	}
//
//	TerminalWrite(arry);
//}

//void TNC_PutSign(uint8_t *arry, uint8_t ssid)
//{
//	arry[6] = '-';
//	//Print all characters except SSID
//	for(int i = 0; i<7; i++){
//		TerminalChar(arry[i]);
//		arry[i] =0;
//	}
//
//	//Possibilities
//	if(48<=ssid/2 && ssid/2<58){
//		TerminalChar(ssid/2);
//	}
//	//More Possibilities
//	else if(112<=ssid/2 && ssid/2<=122){
//		TerminalChar((ssid/2) -64);
//	}
//	//Change Double Digit SSIDs
//	else if(ssid/2 == 58 || ssid/2 == 122){
//		TerminalChar(49);
//		TerminalChar(48);
//	}
//	else if(ssid/2 == 59 || ssid/2 == 123){
//		TerminalChar(49);
//		TerminalChar(49);
//	}
//	else if(ssid/2 == 60 || ssid/2 == 124){
//		TerminalChar(49);
//		TerminalChar(50);
//	}
//	else if( ssid/2 == 61 || ssid/2 == 125){
//		TerminalChar(49);
//		TerminalChar(51);
//	}
//	else if(ssid/2 == 62 || ssid/2 == 126){
//		TerminalChar(49);
//		TerminalChar(52);
//	}
//	else if(ssid/2 == 63 || ssid/2 == 127){
//		TerminalChar(49);
//		TerminalChar(53);
//	}
//	ssid =0;
//}

//void TNC_PutDigi(int x, uint8_t *arry)
//{
//	TerminalChar(',');
//	TerminalChar(' ');
//	int cnt = 0;
//
//	for (int i = x; i < (x + 6); i ++)
//	{
//		arry[cnt] = (tnc_buff[i] / 2);
//		cnt ++;
//	}
//
//	int ssid_check = tnc_buff[x + 6];
//
//	TNC_PutSign(arry, ssid_check);
//}

//void TNC_PutMessage(int x)
//{
//	uint8_t ctrl_bytes[1];
//
//	TerminalChar(':');
//	TerminalChar(' ');
//
//	if (tnc_buff[(pkt_len - 1)] == 13)
//	{
//		tnc_buff[(pkt_len - 1)] = ' ';
//	}
//
//	int j = 0;
//	for (int i = x; i < (x + 2); i ++)
//	{
//		ctrl_bytes[j] = tnc_buff[i];
//	}
//
//	int k = 0;
//	for (int i = (x + 2); i < pkt_len; i ++)
//	{
//		tnc_message[k] = tnc_buff[i];
//		TerminalChar(tnc_message[k]);
//	}
//
//	TerminalWrite("TNC message complete!"); NL();
//}





void TNC_Tx_APRS(uint8_t *message, uint16_t msg_len)
{

	uint8_t tnc_msg[400];
	uint16_t current_index = 0;


	//Header bytes

//	if (current_ax->RADIO_TYPE == 'u')
//	{
//		TNC_UHF
//		if (enable_uhf_hp)
//		{
//			RADIO_FE_NORMAL
//		}
//		RADIO_UHF_TX
//	}
//	else if (current_ax->RADIO_TYPE == 'v')
//	{
//		TNC_VHF
//		if (enable_vhf_hp)
//		{
//			RADIO_FE_HP
//		}
//		RADIO_VHF_TX
//	}

	/*
	 * SSID format: CRRSSID0
	 *
	 * TNC Header Format:
	 * 0: 0xC0: Delimiting Character
	 * 1: 0x00: remainder of frame is HDLC DATA
	 * 			DESTINATION ADDRESS: "W5UL  1"
	 * 2: 0xAE: 'W' << 1 (0x57)
	 * 3: 0x6A (j): '5' << 1 (0x35)
	 * 4: 0xAA: 'U' << 1 (0x55)
	 * 5: 0x98: 'L' << 1 (0x4C)
	 * 6: 0x40: ' ' << 1 (0x20)
	 * 7: 0x40: ' '		 (0x20)
	 * 8: 0x62 (b):  0110 0010 (response, res, res, 0001)
	 * 			SOURCE ADDRESS: "W5UL  0"
	 * 9: 0xAE: 'W' << 1
	 * 10: 0x6A: '5' << 1
	 * 11: 0xAA: 'U' << 1
	 * 12: 0x98: 'L' << 1
	 * 13: 0x40: ' ' << 1
	 * 14: 0x40: ' ' << 1
	 * 15: 0xF7:	1111 0111 (command, res, res, 1011)
	 * 16: 0x03:	Control field (Unnumbered information frame)
	 * 17: 0xF0:	PID field (no layer 3 protocol implemented)
	 *
	 *
	 *
	 *
	 */



	uint8_t tnc_header[25] = {0xC0, 0x00, 0x96, 0x8C, 0x6A, 0x84, 0x9C, 0x98, 0xEE, 0xAE, 0x6A, 0xAA, 0x98, 0x40, 0x40, 0x76, 0xAE, 0x92, 0x88, 0x8A, 0x64, 0x40, 0x65, 0x03, 0xF0};
	uint8_t end_char[1] = {0xC0};

	//	TerminalWrite("Sending test to TNC:"); NL();
	//	TerminalWrite(tnc_header); NL();

//	if (current_ax->ax_fm_mode != 't')
//	{
//		AX_FM_TX();
//
//		//PulseWDT();
//
//		HAL_Delay(500);
//		//			HAL_Delay(10);
//	}
//	else
//	{
//		TerminalWrite("AX5043 already in FM TX mode!"); NL();
//	}


	//PulseWDT();

	HAL_Delay(1);

//	int16_t urb_v_trans = getBusVoltage_mV(URB_INA_ADD);
//	HAL_Delay(1);
//	int16_t urb_i_trans = getCurrent_mA(URB_INA_ADD);
//	HAL_Delay(1);

	//PulseWDT();

//	if (message[22] == 'B')
//	{
//		//		packet_length = 201;
//
//		memcpy(message + BEACON_PKT_URB_V_TRANS, Split16Bit((uint16_t)urb_v_trans), 2);
//
//		memcpy(message + BEACON_PKT_URB_I_TRANS, Split16Bit((uint16_t)urb_i_trans), 2);
//
//		//		TerminalWrite("URB Transmit: ");
//		//		TerminalNum(urb_v_trans); TerminalWrite(" mV, ");
//		//		TerminalNum(urb_i_trans); TerminalWrite(" mA."); NL();
//
//
//	}



//	NL();
	// perform escaping
	for (uint16_t i = 0; i < msg_len; i ++)
	{
		//		TerminalHex(message[i]);
		if (message[i] == 0xC0)
		{

			//		TerminalHex(message[i]);
			//		TerminalNum(i);
			//		TerminalWrite(" Escaping FEND...   ");
			tnc_msg[current_index] = 0xDB;
			current_index ++;
			tnc_msg[current_index] = 0xDC;
		}
		else if (message[i] == 0xDB)
		{
			//		TerminalHex(message[i]);
			//		TerminalNum(i);
			//		TerminalWrite("Escaping FESC...   ");
			tnc_msg[current_index] = 0xDB;
			current_index ++;
			tnc_msg[current_index] = 0xDD;
		}
		else
		{
			tnc_msg[current_index] = message[i];

		}
		current_index ++;
	}

//	NL(); NL();
//	TerminalWrite("Length: "); TerminalNum(current_index); NL();




	//	     HAL_UART_Transmit(&huart2, startHeader, 2, 50);
	//
	//	     HAL_UART_Transmit(&huart2, destination, 7, 50);
	//
	//	     HAL_UART_Transmit(&huart2, callsign, 7, 50);
	//
	//	     HAL_UART_Transmit(&huart2, digipeat, 7, 50);
	//
	//	     HAL_UART_Transmit(&huart2, endHeader, 2, 50);

	HAL_UART_Transmit(&huart1, tnc_header, 25, 50);

	HAL_UART_Transmit(&huart1, tnc_msg, current_index, 500);

	HAL_UART_Transmit(&huart1, end_char, 1, 50);

//	do
//	{
//
//		HAL_Delay(1);
//	}
//
//	while (!(HAL_GPIO_ReadPin(TNC_PTT_GPIO_Port, TNC_PTT_Pin)));
//
//	do
//	{
//
//		//PulseWDT();
//		HAL_Delay(1);
//	}
//
//	while ((HAL_GPIO_ReadPin(TNC_PTT_GPIO_Port, TNC_PTT_Pin)));


//	HAL_Delay(500);

	//AX_FM_RX();

//	AX_FM_EXIT();

//	TerminalWrite("TNC Transmission Complete."); NL();


//	RADIO_FE_LP
//	RADIO_VHF_RX
//	RADIO_UHF_RX








	return;


}
