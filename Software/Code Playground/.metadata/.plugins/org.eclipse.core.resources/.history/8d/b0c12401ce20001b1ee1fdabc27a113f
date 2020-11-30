/*
 * debug.c
 *
 *  Created on: Nov 1, 2020
 *      Author: monke
 */
#include "debug.h"
#include "AX.25.h"
#include "FreqIO.h"
//Printing Packets
//****************************************************************************************************************
void print_AX25(){
	struct PACKET_STRUCT* local_packet = &global_packet;
	int bytecnt = local_packet->byte_cnt;
	bool *curr_mem;
	sprintf(uartData, "\nPrinting AX25_PACKET... All fields printed [MSB:LSB]\n");
	HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);

	//Print Address Field
	curr_mem = (local_packet->AX25_PACKET) + address_len - 1;
	for(int i = 0;i<address_len/8;i++){
		sprintf(uartData, "Address Field %d =",i+1);
		HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);

		for(int j = 0;j<8;j++){
			sprintf(uartData, " %d ",*(curr_mem-j));
			HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);
		}
		curr_mem -= 8;
		sprintf(uartData, "\n");
		HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);
	}

	//Print Control Field
	curr_mem += address_len;//Subtract 8 to start at the flag start
	sprintf(uartData, "Control Field   =");
	HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);

	for(int i = 0;i<8;i++){
		sprintf(uartData, " %d ",*(curr_mem+8-i-1));
		HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);
	}
	sprintf(uartData, "\n");
	HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);

	//PID
	curr_mem += control_len;//Subtract 8 to start at the flag start
	sprintf(uartData, "PID Field       =");
	HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);

	for(int i = 0;i<8;i++){
		sprintf(uartData, " %d ",*(curr_mem+8-i-1));
		HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);
	}
	sprintf(uartData, "\n");
	HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);
	curr_mem += PID_len;

	//Print Info Field
	curr_mem += local_packet->Info_Len - 1;
	for(int i = 0;i<(local_packet->Info_Len/8);i++){
		sprintf(uartData, "Info Field %d    =",i+1)	;
		HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);

		for(int j = 0;j<8;j++){
			sprintf(uartData, " %d ",*(curr_mem-j));
			HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);
		}
		curr_mem -= 8;
		sprintf(uartData, "\n");
		HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);
	}
	curr_mem += local_packet->Info_Len;

	sprintf(uartData, "FCS Field = ")	;
	HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);
	for(int i = 0;i<FCS_len;i++){
		sprintf(uartData, " %d ",curr_mem[i])	;
		HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);
	}
	sprintf(uartData, "\n");
	HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);
}

void print_outAX25(){
	struct PACKET_STRUCT* local_packet = &global_packet;
	int bytecnt = local_packet->byte_cnt;
	bool *curr_mem;
	sprintf(uartData, "\nPrinting AX25_PACKET being sent to radio\n");
	HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);
	sprintf(uartData, "AX25 FLAG = ");
	HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);
	curr_mem = AX25TBYTE;
	for(int i = 0; i < 8; i++){
		sprintf(uartData, " %d ",curr_mem[i]);
		HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);
	}
	sprintf(uartData, "\n");
	HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);

	//Print Address Field
	curr_mem = local_packet->address;
	for(int i = 0;i<address_len/8;i++){
		sprintf(uartData, "Address Field %d =",i+1);
		HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);

		for(int j = 0;j<8;j++){
			sprintf(uartData, " %d ",*(curr_mem+j));
			HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);
		}
		curr_mem += 8;
		sprintf(uartData, "\n");
		HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);
	}

	//if address was bitstuffed then print rest of address field
	sprintf(uartData, "Address Field extra = ");
	HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);
	curr_mem += address_len;
	if(local_packet->stuffed_address > 0){
		for(int i = 0; i < local_packet->stuffed_address; i++){
			sprintf(uartData, " %d ",*(curr_mem-i));
			HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);
		}
	}
	sprintf(uartData, "\n");
	HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);

	//Print Control Field
	curr_mem = local_packet->control;//Subtract 8 to start at the flag start
	sprintf(uartData, "Control Field   =");
	HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);

	for(int i = 0;i<control_len + local_packet->stuffed_control;i++){
		sprintf(uartData, " %d ",*(curr_mem+i));
		HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);
	}
	sprintf(uartData, "\n");
	HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);

	//PID
	curr_mem = local_packet->PID;//Subtract 8 to start at the flag start
	sprintf(uartData, "PID Field       =");
	HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);

	for(int i = 0;i<PID_len + local_packet->stuffed_PID;i++){
		sprintf(uartData, " %d ",*(curr_mem+i));
		HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);
	}
	sprintf(uartData, "\n");
	HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);

	sprintf(uartData, "Info Field = ");
	HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);
	curr_mem = local_packet->Info;
	for(int i = 0; i < local_packet->Info_Len + local_packet->stuffed_Info;i++){
		sprintf(uartData, " %d ",*(curr_mem+i));
		HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);
	}
	sprintf(uartData, "\n");
	HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);

	curr_mem = local_packet->FCS;
	sprintf(uartData, "FCS Field     =")	;
	HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);
	for(int i = 0;i<FCS_len+local_packet->stuffed_FCS;i++){
		sprintf(uartData, " %d ",*(curr_mem+i));
		HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);
	}

	sprintf(uartData, "\n");
	HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);

	sprintf(uartData, "AX25 FLAG = ");
	HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);
	curr_mem = AX25TBYTE;
	for(int i = 0; i < 8; i++){
		sprintf(uartData, " %d ",curr_mem[i]);
		HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);
	}
	sprintf(uartData, "\n");
	HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);



	//reset bitstuff members
	local_packet->stuffed_address = 0;
	local_packet->stuffed_control = 0;
	local_packet->stuffed_PID = 0;
	local_packet->stuffed_Info = 0;
	local_packet->stuffed_FCS = 0;
	local_packet->bit_stuffed_zeros = 0;
}

void print_KISS(){
	struct PACKET_STRUCT* local_packet = &global_packet;
	int bytecnt = local_packet->byte_cnt;
	bool *curr_mem;
	sprintf(uartData, "\nPrinting KISS_PACKET... All fields printed [MSB:LSB]\n");
	HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);

	//Print Start Flag
	curr_mem = (local_packet->address + address_len + 16 - 1);//start at the flag start
	sprintf(uartData, "Start flag      =");
	HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);

	for(int i = 0;i<8;i++){
		sprintf(uartData, " %d ",*(curr_mem-i));
		HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);
	}
	sprintf(uartData, "\n");
	HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);

	curr_mem = (local_packet->address) + address_len - 1;
	for(int i = 0;i<address_len/8;i++){
		sprintf(uartData, "Address Field %d =",i+1);
		HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);

		for(int j = 0;j<8;j++){
			sprintf(uartData, " %d ",*(curr_mem-j));
			HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);
		}
		curr_mem -= 8;
		sprintf(uartData, "\n");
		HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);
	}

	//Print Control Field
	curr_mem = (local_packet->control);//Subtract 8 to start at the flag start
	sprintf(uartData, "Control Field   =");
	HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);

	for(int i = 0;i<8;i++){
		sprintf(uartData, " %d ",*(curr_mem+8-i-1));
		HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);
	}
	sprintf(uartData, "\n");
	HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);

	//PID
	curr_mem = (local_packet->PID);//Subtract 8 to start at the flag start
	sprintf(uartData, "PID Field       =");
	HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);

	for(int i = 0;i<8;i++){
		sprintf(uartData, " %d ",*(curr_mem+8-i-1));
		HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);
	}
	sprintf(uartData, "\n");
	HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);

	//Print Info Field
	curr_mem = (local_packet->Info) + local_packet->Info_Len - 1;
	for(int i = 0;i<(local_packet->Info_Len/8);i++){
		sprintf(uartData, "Info Field %d    =",i+1)	;
		HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);

		for(int j = 0;j<8;j++){
			sprintf(uartData, " %d ",*(curr_mem-j));
			HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);
		}
		curr_mem -= 8;
		sprintf(uartData, "\n");
		HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);
	}

	//Print Stop Flag
	curr_mem = local_packet->KISS_PACKET;
	sprintf(uartData, "Stop flag       =");
	HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);

	for(int i = 0;i<8;i++){
		sprintf(uartData, " %d ",*(curr_mem+8-i-1));
		HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);
	}
	sprintf(uartData, "\n");
	HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);

}
//****************************************************************************************************************

//Direct testing on subsystems
//****************************************************************************************************************
void test_remove_bitstuffing(bool *test_array,int size){
	struct PACKET_STRUCT* local_packet = &global_packet;
	bool *curr_mem = local_packet->AX25_PACKET;
	memcpy(curr_mem,test_array,size);
	rxBit_count = size;

	sprintf(uartData, "\n Testing removal of bits, rxBit_count = %d\n",rxBit_count);
	HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);

	sprintf(uartData, "\n AX.25 before bit stuff removal = \n");
	HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);
	for(int i = 0; i < rxBit_count; i++){
		sprintf(uartData, " %d ",(local_packet->AX25_PACKET)[i]);
		HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);
	}
	sprintf(uartData, "\n");
	HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);

	remove_bit_stuffing();

	sprintf(uartData, "\n AX.25 after bit stuff removal = \n");
	HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);
	for(int i = 0; i < rxBit_count; i++){
		sprintf(uartData, " %d ",(local_packet->AX25_PACKET)[i]);
		HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);
	}
	sprintf(uartData, "\n");
	HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);

	sprintf(uartData, "\n End of removal of bits, rxBit_count = %d\n",rxBit_count);
	HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);
}

//****************************************************************************************************************
