/*
 * STM32_TNC.h
 *
 *  Created on: Nov 21, 2017
 *      Author: nje
 */

#ifndef STM32_TNC_H_
#define STM32_TNC_H_

#include <sys/_stdint.h>
#include "stdbool.h"

#define PRINT_TNC_RX

extern uint8_t tnc_buff[1024];
extern uint16_t pkt_len;
extern uint8_t tnc_message[1];

extern uint8_t tnc_array[1024];
extern uint8_t tnc_cmd_array[1024];
extern bool TNC_received_flag;
extern uint16_t last_tnc_count;
extern uint16_t last_tnc_cmd_len;

int TNC_Tx(uint8_t *message, uint16_t msg_len);
uint16_t TNC_Rx();
void TNC_PutHeader(uint8_t *arry);
void TNC_PutSign(uint8_t *arry, uint8_t ssid);
void TNC_PutDigi(int x, uint8_t *arry);
void TNC_PutMessage(int x);

#endif /* STM32_TNC_H_ */
