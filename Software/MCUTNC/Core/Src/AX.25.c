/*
 * AX.25.c
 *
 *  Created on: Sep 2, 2020
 *      Author: Kobe
 */
#include "AX.25.h"


//*************** variables for detecting and validating  AX.25  ******************************************************
bool AX25_flag = false; 						//indicates whether the TNC started reading for packets
int rxBit_count = 0; 							//keeps count of the temp buffer index

bool AX25TBYTE[FLAG_SIZE] = { 0, 1, 1, 1, 1, 1, 1, 0 };

//NEED TO SET LOCAL_ADDRESS
bool local_address[address_len/2];
//*********************************************************************************************************************

//**************** KISS *************************************************************************************************************
bool KISS_FLAG[FLAG_SIZE] = { 0, 0, 0, 0, 0, 0, 1, 1 };

//Conversion functions
void conv_HEX_to_BIN(uint16_t hex_byte_in, bool *bin_byte_out, bool select_8_16){
    int temp;

    sprintf(uartData, "\nSelector              = %d",select_8_16);
    debug_print_msg();

    sprintf(uartData, "\nByte value            = %d\nBinary value[LSB:MSB] =",hex_byte_in);
	debug_print_msg();

	//8 bits
    if(select_8_16){
		for(int i = 0; i < 8; i++){
			temp = hex_byte_in >> i;
			temp = temp%2;

			sprintf(uartData, " %d ",temp);
			debug_print_msg();

			*(bin_byte_out+i) = temp;
		}
		sprintf(uartData, "\n ");
		debug_print_msg();
    }

    //16 bits
    else{
	   sprintf(uartData, "\nByte value            = %x\nBinary value[LSB:MSB] =",hex_byte_in);
		debug_print_msg();
		for(int i = 0; i < 16; i++){
			temp = hex_byte_in >> i;
			temp = temp%2;

			*(bin_byte_out + i) = temp; //MSB is at lowest index
		}
		sprintf(uartData, "\n ");
		debug_print_msg();
    }

    //sprintf(uartData, "\n");
	//debug_print_msg();
}
uint16_t conv_BIN_to_HEX(bool *bin_byte_in,bool select_8_16){
	uint16_t acc = 0;
	int bits = (select_8_16) ? 8 : 16;
	for(int i = 0; i < bits; i++){
		acc += ( *(bin_byte_in+i) )? pow(2,i) : 0;
	}
	return acc;
}

//General Program
//****************************************************************************************************************
void tx_rx() {
	//Transmission Mode
	if (mode) {
		bool packet_received = false;
		bool packet_converted = false;

		//Run receiving KISS
		packet_received = receiving_KISS();

		if(packet_received){
			//Convert KISS packet to AX.25 packet
			packet_converted = KISS_TO_AX25();
			//Upon exit, have a perfectly good AX.25 packet
		}

		//Output AFSK waveform for radio
		if(packet_converted) {
			output_AX25();
//			print_AX25();
		}
		clear_AX25();

		//Packet was not received properly
		if(!packet_received){
			sprintf(uartData, "Error receiving KISS packet\n");
			debug_print_msg();
		}
		//Packet was not converted properly
		else if(!packet_converted){
			sprintf(uartData, "Error converting KISS packet\n");
			debug_print_msg();
		}
		//Successful transmission!
		else {
			sprintf(uartData, "KISS packet received, converted, and transmitted to radio\n");
			debug_print_msg();
		}

		setHardwareMode(0);
	}

	//Receiving Mode
	else {
	}
}

void output_AX25(){
	struct PACKET_STRUCT* local_packet = &global_packet;

	sprintf(uartData, "Beginning AFSK transmission\n");
	debug_print_msg();

	int wave_start = 0;
	freqSelect = true;

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
	HAL_GPIO_WritePin(PTT_GPIO_Port, PTT_Pin, GPIO_PIN_SET); //START PTT

	bool dumbbits[4] = { 0, 0, 0, 0 };
	//Init dac playing 4*8 zeros to help receiver sync
	for(int i = 0;i<8;i++){
		wave_start = bitToAudio(dumbbits,4,1,wave_start);
	}

	wave_start = bitToAudio(AX25TBYTE, FLAG_SIZE,1,wave_start); //start flag

	//Real information fields
	wave_start = bitToAudio(local_packet->address, address_len + local_packet->stuffed_address,1,wave_start); 		//lsb first
	wave_start = bitToAudio(local_packet->control,control_len + local_packet->stuffed_control,1,wave_start);		//lsb first
	wave_start = bitToAudio(local_packet->PID,PID_len + local_packet->stuffed_PID,1,wave_start);					//lsb first
	wave_start = bitToAudio(local_packet->Info,local_packet->Info_Len + local_packet->stuffed_Info,1,wave_start);	//lsb first
	bitToAudio(local_packet->FCS,FCS_len + local_packet->stuffed_FCS + local_packet->stuffed_FCS,1,wave_start);		//msb first

	bitToAudio(AX25TBYTE, FLAG_SIZE,1,wave_start);//stop flag

	HAL_DAC_Stop_DMA(&hdac, DAC_CHANNEL_1);

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(PTT_GPIO_Port, PTT_Pin, GPIO_PIN_RESET); //stop transmitting

	sprintf(uartData, "Ending AFSK transmission\n");
	debug_print_msg();

	//Debugging mode that will repeat send message. Must restart to stop or change message
	if(BROADCASTR){
		const int millis = 4000;
		sprintf(uartData, "BROADCASTING WILL REPEAT IN A %d MILLISSECOND",millis);
		debug_print_msg();

		int millis_div = (millis * 1.0) / 10 * 1.0;
		for(int i = 0;i<10;i++){
			sprintf(uartData, " . ");
			debug_print_msg();
			HAL_Delay(millis_div);
		}
		sprintf(uartData, "\n\n");
		debug_print_msg();

		print_AX25();
		output_AX25();
	}
}

void clear_AX25(){
	struct PACKET_STRUCT* local_packet = &global_packet;
	sprintf(uartData, "Clearing AX.25 packet info\n");
	debug_print_msg();

	//reset bitstuff members
	local_packet->stuffed_address = 0;
	local_packet->stuffed_control = 0;
	local_packet->stuffed_PID = 0;
	local_packet->stuffed_Info = 0;
	local_packet->stuffed_FCS = 0;
	local_packet->bit_stuffed_zeros = 0;

	memcpy(local_packet->AX25_PACKET,0,AX25_PACKET_MAX);
	local_packet->got_packet = false;
}

void output_HEX() {
	struct UART_INPUT* local_UART_packet = &UART_packet;

	sprintf(uartData, "\n");
	debug_print_msg();

	for(int i = 0;i<local_UART_packet->received_byte_cnt;i++){
		sprintf(uartData, "%x",local_UART_packet->HEX_KISS_PACKET[i]);
		HAL_UART_Transmit(&huart2, uartData, strlen(uartData), 10);
	}

	sprintf(uartData, "\n");
	debug_print_msg();
}

//AX.25 to KISS data flow
//****************************************************************************************************************
bool receiving_AX25(){
	sprintf(uartData, "\nreceiving_AX25() start\n");
	debug_print_msg();
	struct PACKET_STRUCT* local_packet = &global_packet;

	//Validate packet
	bool AX25_IsValid = AX25_Packet_Validate();

//		sprintf(uartData, "AX.25 frame valid check returned: %d\n",AX25_IsValid);
//		debug_print_msg();

	if(AX25_IsValid){

		sprintf(uartData, "Packet is valid af\n");
		debug_print_msg();

		//Put data into KISS format and buffer
		AX25_TO_KISS();

		//Put data into HEX buffer
		KISS_TO_HEX();

		//Transmit KISS Packet that has been generated
		output_HEX();

		//Clear AX.25 buffer
		clear_AX25();
//		clear_KISS();
//		clear_HEX();
	}
}

void slide_bits(bool* array,int bits_left){
	memmove(array,array+1,bits_left*bool_size);
}

void remove_bit_stuffing(){
	struct PACKET_STRUCT* local_packet = &global_packet;
//	sprintf(uartData, "Removing bit stuffed zeros\n");
//	debug_print_msg();

	int ones_count = 0;
	bool curr;
	for(int i = 0;i < rxBit_count;i++){
		curr = local_packet->AX25_PACKET[i]; //iterate through all data received before seperating into subfields
		if(curr){ //current bit is a 1
			ones_count++;
			if(ones_count > 5){
//				sprintf(uartData, "ERROR: SHOULD HAVE BEEN A ZERO AFTER FIFTH CONTIGIOUS ONE!\n");
//				debug_print_msg();
				return;
			}
		}
		else{
			if(ones_count >= 5){
				slide_bits(&local_packet->AX25_PACKET[i],rxBit_count-i);
				i--;
				rxBit_count--;
//				sprintf(uartData, "REMOVED BIT STUFFED ZERO!\n");
//				debug_print_msg();
			}
			ones_count = 0;
		}
	}
	//transmit kiss
}

bool AX25_Packet_Validate(){
	struct PACKET_STRUCT* local_packet = &global_packet;
	int fcs_val = 0;

	sprintf(uartData,"Received packet bit count: %d\n",local_packet->byte_cnt*8);
	debug_print_msg();

	if(rxBit_count < 120){ //invalid if packet is less than 136 bits - 2*8 bits (per flag)
		sprintf(uartData,"Trash Packet, not enough bits\n");
		debug_print_msg();
		return false;
	}
	else if((rxBit_count)%8 != 0){ //invalid if packet is not octect aligned (divisible by 8)
		sprintf(uartData,"Trash Packet, not octet aligned\n");
		debug_print_msg();
		return false;
	}

	//SHOULD BE VALID PACKET, JUST NEED TO C0MPARE CALCULATED CRC TO RECIEVED FCS
	else{
		//Set packet pointers for AX25 to KISS operation
		uint16_t local_info_len = rxBit_count-INFO_offset_woFlag;
		set_packet_pointer_AX25(local_info_len);
		print_AX25();

		return crc_check();
	}
}


void set_packet_pointer_AX25(int info_len_in){
	struct PACKET_STRUCT* local_packet = &global_packet;
	int not_info = FCS_len;
	local_packet->Info_Len = info_len_in;

	sprintf(uartData, "Setting packet pointer to AX25:\n");
	debug_print_msg();
	bool *curr_mem = &local_packet->AX25_PACKET;

	sprintf(uartData, "Setting pointer for address\n");
	debug_print_msg();
	local_packet->address = curr_mem;
	curr_mem += address_len;
	not_info += address_len;

	sprintf(uartData, "Setting pointer for control\n");
	debug_print_msg();
	local_packet->control = curr_mem;
	curr_mem += control_len;
	not_info += control_len;

	sprintf(uartData, "Setting pointer for PID\n");
	debug_print_msg();
	local_packet->PID = curr_mem;
	curr_mem += PID_len;
	not_info += PID_len;

	sprintf(uartData, "Setting pointer for Info\n");
	debug_print_msg();
	local_packet->Info = curr_mem;
	curr_mem += local_packet->Info_Len;

	sprintf(uartData, "Setting pointer for FCS\n");
	debug_print_msg();
	local_packet->FCS = curr_mem;
}

void AX25_TO_KISS(){
	struct PACKET_STRUCT* local_packet = &global_packet;
	bool *curr_mem = local_packet->KISS_PACKET;

	//Put a flag into KISS
	memcpy(curr_mem,KISS_FLAG,FLAG_SIZE);
	curr_mem += FLAG_SIZE;

	//Set port info
	memset(curr_mem,0,8);
	curr_mem += 8;

	//Put AX25 packet into KISS w/o the FCS, HAVE AN ADDED 8 due to port info
	memcpy(curr_mem,local_packet->AX25_PACKET,(local_packet->byte_cnt*8)-FCS_len);
	curr_mem += (local_packet->byte_cnt*8)-FCS_len;

	//Put a flag into KISS
	memcpy(curr_mem,KISS_FLAG,FLAG_SIZE);
}

//****************************************************************************************************************
//END OF AX.25 to KISS data flow

//KISS to AX.25 data flow
//****************************************************************************************************************
bool receiving_KISS(){
	struct UART_INPUT* local_UART_packet = &UART_packet;
	struct PACKET_STRUCT* local_packet = &global_packet;

	//Got a packet bounded by c0 over uart
	if(local_UART_packet->got_packet){
		sprintf(uartData, "\nGot a packet via UART of size %d, printing now...\n",local_UART_packet->received_byte_cnt);
		debug_print_msg();

		int byte_cnt = local_UART_packet->received_byte_cnt;
		for(int i = 0;i < byte_cnt;i++){
			//Hex value from UART
			 //start from LS Byte = Highest index
			uint8_t hex_byte_val=local_UART_packet->HEX_KISS_PACKET[i];

			//Bool pointer for KISS array
			bool *bin_byte_ptr = &local_packet->KISS_PACKET[i*8];

			//sprintf(uartData, "Byte[%d] = %d\n",i,hex_byte_val);
			//debug_print_msg();

			//Selecting 8 bit
			conv_HEX_to_BIN(hex_byte_val, bin_byte_ptr,true);
		}

		local_UART_packet->got_packet = false;
		local_packet->got_packet = true;
		local_packet->byte_cnt = local_UART_packet->received_byte_cnt;

		print_array_octet(local_packet->KISS_PACKET,local_packet->byte_cnt*8);

		return true;
	}
	return false;
}

void set_packet_pointer_KISS(int info_len_in){
	struct PACKET_STRUCT* local_packet = &global_packet;
	sprintf(uartData, "Setting packet pointer to KISS\n");
	debug_print_msg();

	//Update info len since we received a message over UART
	local_packet->Info_Len =info_len_in;
//	sprintf(uartData, "Setting info length to %d\n",local_packet->Info_Len);
//	debug_print_msg();

	bool *curr_mem = (local_packet->KISS_PACKET+(8*2));//starting kiss packet skipping 2 bytes of flag and port info

	//Assign Address Pointer
	local_packet->address = curr_mem;
	curr_mem += address_len;

	//Assign Control Pointer
	local_packet->control = curr_mem;
	curr_mem += control_len;

	//Assign PID Pointer
	local_packet->PID = curr_mem;
	curr_mem += PID_len;

	//Assign INFO Pointer
	local_packet->Info = curr_mem;
}

bool KISS_TO_AX25(){
	struct PACKET_STRUCT* local_packet = &global_packet;

	int local_info_len = local_packet->byte_cnt*8-INFO_offset_wFlag_woFCS;

	set_packet_pointer_KISS(local_info_len);
	print_KISS();

	bool* cpy_from_ptr = local_packet->KISS_PACKET+16;//starting kiss packet skipping 2 bytes

	//Update packet pointers to AX25 members
	set_packet_pointer_AX25(local_info_len);

	//Copy address
	memcpy(local_packet->address,cpy_from_ptr,address_len);
	cpy_from_ptr += address_len;

	//Copy control
	memcpy(local_packet->control,cpy_from_ptr,control_len);
	cpy_from_ptr += control_len;

	//Copy PID
	memcpy(local_packet->PID,cpy_from_ptr,PID_len);
	cpy_from_ptr += PID_len;

	//Info
	memcpy(local_packet->Info,cpy_from_ptr,local_packet->Info_Len);

	//USE CRC HERE TO GENERATE FCS FIELD
	rxBit_count = address_len + control_len + PID_len + local_packet->Info_Len;
	crc_generate(true);
	print_AX25();

	//BIT STUFFING NEEDED
	bit_stuff_fields();

	rxBit_count = 0;
//	Print the ax25 packet
	print_outAX25();
	return true; //valid packet
}

void bit_stuff_fields(){
	struct PACKET_STRUCT* local_packet = &global_packet;
	int ax25_len = rxBit_count + FCS_len;
	int ones_count = 0;
	ones_count = bitstuffing(local_packet->address,address_len,ax25_len, ones_count, &(local_packet->stuffed_address));
	local_packet->bit_stuffed_zeros += local_packet->stuffed_address;
	ax25_len -= address_len;

	local_packet->control += local_packet->bit_stuffed_zeros;
	ones_count = bitstuffing(local_packet->control,control_len,ax25_len, ones_count, &(local_packet->stuffed_control));
	local_packet->bit_stuffed_zeros += local_packet->stuffed_control;
	ax25_len -= control_len;

	local_packet->PID += local_packet->bit_stuffed_zeros;
	ones_count = bitstuffing(local_packet->PID,PID_len,ax25_len, ones_count, &(local_packet->stuffed_PID));
	local_packet->bit_stuffed_zeros += local_packet->stuffed_PID;
	ax25_len -= PID_len;

	local_packet->Info += local_packet->bit_stuffed_zeros;
	ones_count = bitstuffing(local_packet->Info,local_packet->Info_Len, ax25_len, ones_count, &(local_packet->stuffed_Info));
	local_packet->bit_stuffed_zeros += local_packet->stuffed_Info;
	ax25_len -= local_packet->Info_Len;

	local_packet->FCS += local_packet->bit_stuffed_zeros;
	bitstuffing(local_packet->FCS,FCS_len, ax25_len, ones_count, &(local_packet->stuffed_FCS));
	local_packet->bit_stuffed_zeros += local_packet->stuffed_FCS;

	sprintf(uartData, "bit stuffed zeros = %d\n",local_packet->bit_stuffed_zeros);
	debug_print_msg();
}

void bit_stuff(bool* array,int bits_left){
	memmove(array+2,array+1,bits_left);
	*(array+1) = false;
}

int bitstuffing(bool* packet,int len,int bits_left, int ones_count,int *stuff){
	sprintf(uartData, "\nChecking if bit stuffing is needed\n");
	debug_print_msg();

	int bit_stuff_count = 0;
	bool *curr_mem = packet;
	int left;

	//Increment through array
	for(int i = 0; i < len+bit_stuff_count; i++){
		//Check if the current index in array is a one
        if(*(curr_mem+i)){
        	//Inc ones count
        	ones_count++;

        	//If 5 consecutive ones, do bit stuff
            if(ones_count == 5){
        		//Calculate the number of bits left
        		left = (bits_left+bit_stuff_count) - i - 1;
                bit_stuff(curr_mem+i,left);

                bit_stuff_count++;
                ones_count = 0;
                i++;
            }
        }
        else{
        	ones_count = 0;
        }
	}
	*stuff = bit_stuff_count;
	return ones_count;
}

void KISS_TO_HEX(){
	struct PACKET_STRUCT* local_packet = &global_packet;
	struct UART_INPUT* local_UART_packet = &UART_packet;

	local_UART_packet->received_byte_cnt = local_packet->byte_cnt+1;
	bool *curr_mem = local_packet->KISS_PACKET;
	uint8_t curr_val;

	sprintf(uartData, "Filling HEX buffer:\n");
	debug_print_msg();

    for(int i = 0; i < local_UART_packet->received_byte_cnt; i++){
    	curr_val = conv_BIN_to_HEX(curr_mem+(i*8),1);

        sprintf(uartData, "HEX[%d] = %x\n",i,curr_val);
    	debug_print_msg();

        local_UART_packet->HEX_KISS_PACKET[i] = curr_val;
    }

	sprintf(uartData, "HEX buffer filled\n");
	debug_print_msg();
}
//****************************************************************************************************************
//END OF KISS to AX.25 data flow

//---------------------- FCS Generation -----------------------------------------------------------------------------------------------

//CRC Calculations
void crc_calc(int in_bit, int * crc_ptr_in, int * crc_count_ptr_in, bool tx_rx){
	struct PACKET_STRUCT* local_packet = &global_packet;
	int max_bits = (tx_rx) ? rxBit_count : rxBit_count-FCS_len;
	int out_bit;
	int roll_bit = *crc_ptr_in & 0x0001;
    int poly = 0x8408;             			//reverse order of 0x1021

    out_bit = in_bit ^ roll_bit; 		//xor lsb of current crc with input bit
	*crc_ptr_in >>= 1;               	//right shift by 1
	poly = (out_bit == 1) ? 0x8408 : 0x0000;
	*crc_ptr_in ^= poly;
	*crc_count_ptr_in+=1;//Increment count

    //End condition
	if(*crc_count_ptr_in >= max_bits){
    	*crc_ptr_in ^= 0xFFFF;//Complete CRC by XOR with all ones (one's complement)
  	    sprintf(uartData, "Convert CRC to FCS (hex) = %x\n",local_packet->crc);
    	debug_print_msg();
    	if(tx_rx){
    		//REMEBER TO CHECK THIS CRC conversion FOR ACCURACY LATER
			conv_HEX_to_BIN(*crc_ptr_in,local_packet->FCS,false);
    	}
//    	local_packet->crc = 0xFFFF;
    }
}

void crc_generate(bool tx_rx){
	struct PACKET_STRUCT* local_packet = &global_packet;
	uint16_t * crc_ptr = &(local_packet->crc);
	int * crc_count_ptr = &(local_packet->crc_count);
	bool *curr_mem;

	*crc_ptr = 0xFFFF;
	*crc_count_ptr = 0;

	//Generate CRC from packet pointers of current packet type

	//have to be inserted in reverse order
	sprintf(uartData, "Performing CRC generation\n");
	debug_print_msg();

	//Calculate CRC for address
	curr_mem = (local_packet->address);//start at MS Byte(LSB)
	for(int i = 0;i<address_len;i++){
		crc_calc((int)local_packet->address[i],crc_ptr,crc_count_ptr,tx_rx);
	}

	//Calculate CRC for control
	curr_mem = local_packet->control;
	for(int i = 0; i < control_len; i++){
		//Call crc_calc per bit
		crc_calc((int)local_packet->control[i],crc_ptr,crc_count_ptr,tx_rx);
	}

//	//Calculate CRC for PID (if packet is of type i-frame)
	curr_mem = local_packet->PID;
	for(int i = 0; i < PID_len; i++){
		//Call crc_calc per bit
		crc_calc((int)local_packet->PID[i],crc_ptr,crc_count_ptr,tx_rx);
	}

	//Calculate CRC for Info field
	curr_mem = (local_packet->Info);
	for(int i = 0;i<local_packet->Info_Len;i++){
		crc_calc((int)local_packet->Info[i],crc_ptr,crc_count_ptr,tx_rx);
	}

	sprintf(uartData, "rx_bitcnt = %d\n", rxBit_count);
	debug_print_msg();

	sprintf(uartData, "bitcnt_ptr = %d\n", *crc_count_ptr);
	debug_print_msg();
	*crc_count_ptr = 0;
}

bool crc_check(){
	struct PACKET_STRUCT* local_packet = &global_packet;
	local_packet->check_crc = true;
	uint16_t fcs_val = 0;
	bool valid_crc = false;

	fcs_val = conv_BIN_to_HEX(local_packet->FCS,0);

	//generate crc
	crc_generate(false);

	//compare crc
	valid_crc = (local_packet->crc==fcs_val) ? true : false;

	sprintf(uartData, "CRC is %s\n",(valid_crc)?"valid, boi!":"not valid, boi :'(");
	debug_print_msg();
	return valid_crc;
}
