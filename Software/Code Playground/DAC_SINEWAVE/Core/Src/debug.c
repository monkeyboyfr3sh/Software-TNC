/*
 * debug.c
 *
 *  Created on: Nov 1, 2020
 *      Author: monke
 */
#include "debug.h"

void debug_print_msg(){
	if(debug_printing){
		HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);
	}
}

//Printing Packets
//****************************************************************************************************************
void print_AX25(){
	struct PACKET_STRUCT* local_packet = &global_packet;
	int bytecnt = local_packet->byte_cnt;
	bool *curr_mem;
	sprintf(uartData, "\nPrinting AX25_PACKET... All fields printed [LSB:MSB]\n");
	debug_print_msg();

	//Print Address Field
	curr_mem = local_packet->AX25_PACKET;

	//Output byte at a time
	for(int i = 0;i<address_len/8;i++){

		sprintf(uartData, "Address Field %d =",i+1);
		debug_print_msg();

		for(int j = 0;j<8;j++){
			sprintf(uartData, " %d ",*(curr_mem+j));
			debug_print_msg();
		}
		curr_mem += 8;
		sprintf(uartData, "\n");
		debug_print_msg();
	}

	//Check if address is stuffed
	if(local_packet->stuffed_address>0){
		sprintf(uartData, "Stuffed Address:");
		debug_print_msg();

		for(int i = 0;i<local_packet->stuffed_address;i++){
			sprintf(uartData, "%d",*(curr_mem+i));
			debug_print_msg();
		}
		sprintf(uartData, "\n");
		debug_print_msg();

		curr_mem+=local_packet->stuffed_address;
	}

	//Print Control Field
	sprintf(uartData, "Control Field   =");
	debug_print_msg();
	for(int i = 0;i<8;i++){
		sprintf(uartData, " %d ",*(curr_mem+i));
		debug_print_msg();
	}
	sprintf(uartData, "\n");
	debug_print_msg();
	curr_mem += control_len;//Subtract 8 to start at the flag start

	//Check if control is stuffed
	if(local_packet->stuffed_control>0){
		sprintf(uartData, "Stuffed Control:");
		debug_print_msg();

		for(int i = 0;i<local_packet->stuffed_control;i++){
			sprintf(uartData, "%d",*(curr_mem+i));
			debug_print_msg();
		}
		sprintf(uartData, "\n");
		debug_print_msg();

		curr_mem+=local_packet->stuffed_control;
	}

	//PID
	sprintf(uartData, "PID Field       =");
	debug_print_msg();

	for(int i = 0;i<8;i++){
		sprintf(uartData, " %d ",*(curr_mem+i));
		debug_print_msg();
	}
	sprintf(uartData, "\n");
	debug_print_msg();
	curr_mem += PID_len;

	//Check if PID is stuffed
	if(local_packet->stuffed_control>0){
		sprintf(uartData, "Stuffed PID:");
		debug_print_msg();

		for(int i = 0;i<local_packet->stuffed_PID;i++){
			sprintf(uartData, "%d",*(curr_mem+i));
			debug_print_msg();
		}
		sprintf(uartData, "\n");
		debug_print_msg();

		curr_mem+=local_packet->stuffed_PID;
	}

	//Print Info Field
	for(int i = 0;i<(local_packet->Info_Len/8);i++){
		sprintf(uartData, "Info Field %d    =",i+1)	;
		debug_print_msg();

		for(int j = 0;j<8;j++){
			sprintf(uartData, " %d ",*(curr_mem+j));
			debug_print_msg();
		}
		curr_mem += 8;
		sprintf(uartData, "\n");
		debug_print_msg();
	}
	//Check if INFO is stuffed
	if(local_packet->stuffed_Info>0){
		sprintf(uartData, "Stuffed INFO:");
		debug_print_msg();

		for(int i = 0;i<local_packet->stuffed_Info;i++){
			sprintf(uartData, "%d",*(curr_mem+i));
			debug_print_msg();
		}
		sprintf(uartData, "\n");
		debug_print_msg();

		curr_mem+=local_packet->stuffed_Info;
	}

	sprintf(uartData, "FCS Field = ")	;
	debug_print_msg();
	curr_mem = local_packet->FCS;
	for(int i = 0;i<FCS_len;i++){
		sprintf(uartData, " %d ",*(curr_mem+i));
		debug_print_msg();
	}
	sprintf(uartData, "\n");
	debug_print_msg();

	//Check if FCS is stuffed
	if(local_packet->stuffed_FCS>0){
		sprintf(uartData, "Stuffed FCS:");
		debug_print_msg();

		for (int i = 0; i < local_packet->stuffed_FCS;i++){
			sprintf(uartData, "%d",*(curr_mem+i));
			debug_print_msg();
		}
		sprintf(uartData, "\n");
		debug_print_msg();

		curr_mem+=local_packet->stuffed_FCS;
	}
}

void print_outAX25(){
	struct PACKET_STRUCT* local_packet = &global_packet;
	int bytecnt = local_packet->byte_cnt;
	bool *curr_mem;
	sprintf(uartData, "\nPrinting AX25_PACKET being sent to radio\n");
	debug_print_msg();
	sprintf(uartData, "AX25 FLAG = ");
	debug_print_msg();
	curr_mem = AX25TBYTE;
	for(int i = 0; i < 8; i++){
		sprintf(uartData, " %d ",curr_mem[i]);
		debug_print_msg();
	}
	sprintf(uartData, "\n");
	debug_print_msg();

	//Print Address Field
	curr_mem = local_packet->address;
	for(int i = 0;i<address_len/8;i++){
		sprintf(uartData, "Address Field %d =",i+1);
		debug_print_msg();

		for(int j = 0;j<8;j++){
			sprintf(uartData, " %d ",*(curr_mem+j));
			debug_print_msg();
		}
		curr_mem += 8;
		sprintf(uartData, "\n");
		debug_print_msg();
	}

	//if address was bitstuffed then print rest of address field
	sprintf(uartData, "Address Field extra = ");
	debug_print_msg();
	curr_mem += address_len;
	if(local_packet->stuffed_address > 0){
		for(int i = 0; i < local_packet->stuffed_address; i++){
			sprintf(uartData, " %d ",*(curr_mem-i));
			debug_print_msg();
		}
	}
	sprintf(uartData, "\n");
	debug_print_msg();

	//Print Control Field
	curr_mem = local_packet->control;//Subtract 8 to start at the flag start
	sprintf(uartData, "Control Field   =");
	debug_print_msg();

	for(int i = 0;i<control_len + local_packet->stuffed_control;i++){
		sprintf(uartData, " %d ",*(curr_mem+i));
		debug_print_msg();
	}
	sprintf(uartData, "\n");
	debug_print_msg();

	//PID
	curr_mem = local_packet->PID;//Subtract 8 to start at the flag start
	sprintf(uartData, "PID Field       =");
	debug_print_msg();

	for(int i = 0;i<PID_len + local_packet->stuffed_PID;i++){
		sprintf(uartData, " %d ",*(curr_mem+i));
		debug_print_msg();
	}
	sprintf(uartData, "\n");
	debug_print_msg();

	sprintf(uartData, "Info Field = ");
	debug_print_msg();
	curr_mem = local_packet->Info;
	for(int i = 0; i < local_packet->Info_Len;i++){
		if(i%8==0){
			sprintf(uartData, " \n ");
			debug_print_msg();
		}
		sprintf(uartData, " %d ",*(curr_mem+i));
		debug_print_msg();
	}
	sprintf(uartData, "\n");
	debug_print_msg();
	curr_mem+=local_packet->Info_Len;

	sprintf(uartData, "Stuffed Info Field = ");
	debug_print_msg();
	for(int i = 0;i<local_packet->stuffed_Info;i++){
		if(i%8==0){
			sprintf(uartData, " \n ");
			debug_print_msg();
		}
		sprintf(uartData, " %d ",*(curr_mem+i));
		debug_print_msg();
	}
	sprintf(uartData, "\n");
	debug_print_msg();

	curr_mem = local_packet->FCS;
	sprintf(uartData, "FCS Field     =");
	debug_print_msg();
	for(int i = 0;i<FCS_len+local_packet->stuffed_FCS;i++){
		sprintf(uartData, " %d ",*(curr_mem+i));
		debug_print_msg();
	}

	sprintf(uartData, "\n");
	debug_print_msg();

	sprintf(uartData, "AX25 FLAG = ");
	debug_print_msg();
	curr_mem = AX25TBYTE;
	for(int i = 0; i < 8; i++){
		sprintf(uartData, " %d ",curr_mem[i]);
		debug_print_msg();
	}
	sprintf(uartData, "\n");
	debug_print_msg();
}

void print_KISS(){
	struct PACKET_STRUCT* local_packet = &global_packet;
	int bytecnt = local_packet->byte_cnt;
	bool *curr_mem;
	sprintf(uartData, "\nPrinting KISS_PACKET... All fields printed [LSB:MSB]\n");
	debug_print_msg();

	//Print Start Flag
	curr_mem = local_packet->KISS_PACKET;//start at the flag start
	sprintf(uartData, "Start flag      =");
	debug_print_msg();

	for(int i = 0;i<8;i++){
		sprintf(uartData, " %d ",*(curr_mem+i));
		debug_print_msg();
	}
	sprintf(uartData, "\n");
	debug_print_msg();

	//Print address
	curr_mem = local_packet->address;
	for(int i = 0;i<address_len/8;i++){
		sprintf(uartData, "Address Field %d =",i+1);
		debug_print_msg();

		for(int j = 0;j<8;j++){
			sprintf(uartData, " %d ",*(curr_mem+j));
			debug_print_msg();
		}
		curr_mem += 8;
		sprintf(uartData, "\n");
		debug_print_msg();
	}

	//Print Control Field
	curr_mem = (local_packet->control);
	sprintf(uartData, "Control Field   =");
	debug_print_msg();
	for(int i = 0;i<8;i++){
		sprintf(uartData, " %d ",*(curr_mem+i));
		debug_print_msg();
	}
	sprintf(uartData, "\n");
	debug_print_msg();

	//PID
	curr_mem = (local_packet->PID);
	sprintf(uartData, "PID Field       =");
	debug_print_msg();
	for(int i = 0;i<8;i++){
		sprintf(uartData, " %d ",*(curr_mem+i));
		debug_print_msg();
	}
	sprintf(uartData, "\n");
	debug_print_msg();

	//Print Info Field
	curr_mem = local_packet->Info;

	for(int i = 0;i<(local_packet->Info_Len/8);i++){
		sprintf(uartData, "Info Field %d    =",i+1)	;
		debug_print_msg();

		for(int j = 0;j<8;j++){
			sprintf(uartData, " %d ",*(curr_mem+j));
			debug_print_msg();
		}
		curr_mem += 8;
		sprintf(uartData, "\n");
		debug_print_msg();
	}

	//Print Stop Flag
	curr_mem = local_packet->KISS_PACKET;
	sprintf(uartData, "Stop flag       =");
	debug_print_msg();

	for(int i = 0;i<8;i++){
		sprintf(uartData, " %d ",*(curr_mem+i));
		debug_print_msg();
	}
	sprintf(uartData, "\n");
	debug_print_msg();

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
	debug_print_msg();

	sprintf(uartData, "\n AX.25 before bit stuff removal = \n");
	debug_print_msg();
	for(int i = 0; i < rxBit_count; i++){
		sprintf(uartData, " %d ",(local_packet->AX25_PACKET)[i]);
		debug_print_msg();
	}
	sprintf(uartData, "\n");
	debug_print_msg();

	remove_bit_stuffing();

	sprintf(uartData, "\n AX.25 after bit stuff removal = \n");
	debug_print_msg();
	for(int i = 0; i < rxBit_count; i++){
		sprintf(uartData, " %d ",(local_packet->AX25_PACKET)[i]);
		debug_print_msg();
	}
	sprintf(uartData, "\n");
	debug_print_msg();

	sprintf(uartData, "\n End of removal of bits, rxBit_count = %d\n",rxBit_count);
	debug_print_msg();
}

//****************************************************************************************************************

//Compare buffers
//****************************************************************************************************************
void compareBoolBuffers(bool *array1, bool *array2,uint16_t size){
	bool arr1_curr;
	bool arr2_curr;
	bool same;

	sprintf(uartData, "Comparing buffers:\n");
	debug_print_msg();

	//Iterate through array
	for(int i=0;i<size;i++){

		arr1_curr = *(array1+i);
		arr2_curr = *(array2+i);
		same = !(arr1_curr^arr2_curr);

		sprintf(uartData, "Comparing index %d ... Result: %d",i,same);
		debug_print_msg();

		if(!same){
			sprintf(uartData, "; bitBuffer value = %d; AX.25 value = %d",arr1_curr,arr2_curr);
			debug_print_msg();
		}
		sprintf(uartData, "\n");
		debug_print_msg();
	}

	sprintf(uartData, "Done comparing buffers!\n");
	debug_print_msg();
}
//****************************************************************************************************************
//End of compare buffers

void print_array_octet(bool* array,int array_size){
	sprintf(uartData, "Printing array:\n");
	debug_print_msg();

	//Byte at a time
	for(int i = 0;i<array_size/8;i++){
		sprintf(uartData, "Byte %d:",i);
		debug_print_msg();

		//Print each bit
		for(int j = 0;j<8;j++){
			sprintf(uartData, " %d ",*(array+(i*8)+j));
			debug_print_msg();
		}
		sprintf(uartData, "\n");
		debug_print_msg();
	}

	sprintf(uartData, "Finished Printing Array\n");
	debug_print_msg();
}
