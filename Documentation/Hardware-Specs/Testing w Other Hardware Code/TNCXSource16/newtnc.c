#include "newtnc.h"
#include "fram8k.c"     //Contains routines to read and write to/from the FRAM memory.

int minus1, minus2;		//maintain a history list of bytes received.
int rcvbyte, bitcount;  //the byte being recieved and the num of bits so far
short oldstate;			//keeps track of the state of the rcv data pin
int ones;			    //keeps track of the number of consequtive ones received
short fiveones;	 		//true if we have received 5 consequtive ones
int crclo,crchi;		//for calculating CRC
short done;			    //true if packet reception is done
short isflag = false;	//true if the current incoming byte is a flag or noise.
int count;			    //keeps track of whether it is the beginning or end of a packet on receive
short rmiddle = false;	//keeps track of whether we are in the middle of a packet when sending to serial port (RCV data)
short xmiddle = false;	//keeps track of whether we are in the middle of a packet when receiving from the serial port (XMIT data)
int receivedpackets; 	//number of received packets that have not been sent to serial port
int C0xmit;			    //number of packets to transmit = C0XMIT
signed long rcvinadd;	//address of byte from radio to memory  addresses 0 to 1999
signed long rcvoutadd;	//address of byte from memory to serial port addresses 0 to 1999
signed long begin;		//beginning address of this packet.
signed long txinadd;	//address of byte from serial to memory	 addresses 2048 to 8192
signed long txoutadd;	//address of byte from memory to radio (transmit data) addresses 2048 to 8192
int xcrchi, xcrclo;     //for transmit CRC calculation
int stuff;              //counts the number of ones in a row to see if it is necessary to stuff a bit
short flag, crcflag;    //true if we are transmitting flags (flag) or crcbytes (crcflag)

short xmitlatch = false;	//enable transmit
byte PERSIST=30;		//default 1 in 4 chance of transmitting
byte SLOTTIME=10;		//wait 100 ms to check to transmit
int slotcount = 0;


//********************FUNCTION PROTOTYPES FOLLOW***********************
int gettxdelay();		      //determine txdelay
void sendbyteserial();		  //send a byte over serial port (to terminal)
void rcvbyteserial();		  //receive a byte from the serial port (from terminal)
void setParms(int ad);
void sendParms();
void crcbit(byte tbyte);	  //ongoing CRC calculation for transmitted data
void addcrc(int crbyte);	  //ongoing CRC calculation for received data
void sendbyte(byte inbyte);   //send a byte via radio
void xmit();			      //send accumulated data via radio
void findflag();		      //look for the beginning of a received packet
void resetclock();		      //synchronizeds clock on zeros when receiving (Change on B interrupt)
void rcvbits();			      //receive bits from radio (Timer interrupt fires every 833 us)
void resetxmit();			  //if something goes horribly wrong during xmit, this allows smooth recovery...should never be called


//*********************SERIAL PORT ROUTINES FOLLOW******************


char rand()	{						  //pseudo random number generator 
	char random;
	char sum = 0;
	random = GET_RTCC();
	if(random & 0x80)	sum = 1;
	if(random & 0x20)	sum ^= 1;
	if(random & 0x10)	sum ^= 1;
	if(random & 0x04)	sum ^= 1;
	random <<= 1;
	random |= sum;
	return random;
}

void fram_dump(){
long i=0;
int stuff;


	while (i <60){
	    stuff = fram_read(i);
	    putc(stuff);
	   i++;
	}
}
	

void sendbyteserial(){

int store;
   if (bit_test(pir1,4)){                     //if the port is ready to receive data
	  if (receivedpackets > 0){               //if there is data to send
	    disable_interrupts(global);			  //don't allow interrupt of memory operation
	    store = fram_read(rcvoutadd++);		  //get a byte from memory to route out serial port
	    if (rcvoutadd == 2048) rcvoutadd = 0; //memory rollover, if necessary
	    enable_interrupts(global);			  //interrupt ok now.
	    if (store == 0xC0){				      //if we are on a packet boundary
		    if (rmiddle){				      //if middle = true we have the ending C0
			    rmiddle = false;		      //no longer in the middle
		   	 receivedpackets--;		          //decrement the number of received packets.
		    }else rmiddle = true;			  //if we weren't in the middle, we now are
	    }
	    putc(store);				          //send the data to the serial port
	  }
   }
}

void rcvbyteserial(){  					            //process incoming characters from the serial port
int inbyt;
    if (bit_test(pir1,5)){  				        //if there is a byte in the buffer
         inbyt = getc();  				            //get the incoming byte
         if (inbyt == 0xC0) {                       //do this if the byte is a C0 (begins or ends KISS frame
		       disable_interrupts(global);		    //don't allow interrupt of memory operation
                     fram_write(inbyt,txinadd++);		    //write byte to memory
		       if (txinadd == 8192) txinadd = 2048;	//rollover memory space if necessary
		       enable_interrupts(global);		    //interrupt ok now.
			if (xmiddle) c0xmit++;
		       xmiddle = !xmiddle;			        //flip middle indicator
         }else{                                     //if we are in the middle of a KISS frame or in between KISS frames (shouldn't happen)
	        if (xmiddle) {                     	    //confirm that we are in the middle
	           	 disable_interrupts(global);		//don't allow interrupt of memory operation
                 fram_write(inbyt,txinadd++);		 //write byte to memory
		         if (txinadd == 8192) txinadd = 2048; //rollover memory space
		         enable_interrupts(global);		      //interrupt ok now.
	        }//end of if
         }//end of else
     }//end of if
}

void setParms(int ad){
	   int bt;
	   
  	   bt = fram_read(txoutadd++);		            //reads the command byte
	   if (txoutadd == 8192) txoutadd = 2048;		    //rollover if necessary
	   if ((ad == 2) & (bt < 1)) bt = 1;
	   WRITE_EEPROM(ad,bt);					//write the value to EEPROM memory
	  if (ad == 2) PERSIST = bt;				//change the value currently used
	  if (ad == 3) SLOTTIME = bt;
}
void sendParms(){
	int p,s;
	p = READ_EEPROM(2);
	s = READ_EEPROM(3);
	putc(p);
	putc(s);
}


//*********************TRANSMIT ROUTINES FOLLOW***************************
void resetxmit(){
int test, rep;
				output_low(PTT);     		 //unkey PTT
  				output_low(radtx);			 //leave transmit pin low
   				output_low(M0);  			 //MX 614 to recv mode
				c0xmit = 0;					 //reset xmit values
			 	txoutadd = txinadd = 2048;
				xmiddle = false;
				for (rep = 0; rep < 10; rep++){
					delay_ms(10);
					restart_wdt();
				}
				while (bit_test(pir1,5)){    //wait until the stream of incoming data has ended
					test = getc();
					for (rep=0; rep < 25; rep++){
						delay_ms(10);
						restart_wdt();
					}
				}
}

int gettxdelay(){				                //read the TXdelay pot
int level;
        setup_comparator(A0_VR_A1_VR);		    //set up to use analog comparators on A0/A1
        for (level = 0; level < 16; level++){	//run the internal volt ref through each of its 16 settings.
                setup_vref(vref_low | level);   //set the vref for the next step
                if (bit_test(cmcon,7)) break;	//if the reference rises above the voltage on A1 we have txdelay
        }
        setup_comparator(NC_NC_NC_NC);		    //pins all digital again
        return 5 * level;			            //multiply value that was read by 5 to get txdelay (range 0 to abt 500 ms)
}

void crcbit(byte tbyte){				//accumulates the CRC for transmit
#asm
   BCF    03,0
   RRF    xcrchi,F                      //rotates the entire 16 bits to the right
   RRF    xcrclo,F
#endasm
   if (((status & 0x01)^(tbyte)) ==0x01){
         xcrchi = xcrchi^0x84;
         xcrclo = xcrclo^0x08;
   }
 }


void sendbyte (byte inbyte){				            //send a byte over the radio link
   int k, bt;
   restart_wdt();	
   for (k=0;k<8;k++){
     bt = inbyte & 0x01;       				            //strip off the rightmost bit
     while (get_rtcc() < 32) rcvbyteserial();		    //wait until 832 us have elapsed
     delay_us(11);					                    //timer resolution not good enough
     set_rtcc(0);					                    //start timer back at 0
     if ((!crcflag) && (!flag)) (crcbit(bt));   	    //do crc, but only if this is not a flag or crc byte
     if (!bt){						                    //if this is a zero
        stuff = 0;					                    //reset stuff and send a 0
        portb = portb^0b00001000;			            //flip the radtx pin
     }else {						                    //otherwise it is a one
        stuff++;    					                //increment 1's count
        if ((!flag) && (stuff == 5)){   		        //stuff an extra 0, if 5 1's in a row
            while(get_rtcc() < 32) rcvbyteserial();     //wait until 832 us have elapsed   
     	      delay_us(11);				                //timer resolution not good enough
	         set_rtcc(0);					            //start timer back at 0
            stuff = 0;					                //reset stuff and send a 0
            portb = portb^0b00001000;			        //flip the radtx pin
        }//end of if
     }//end of else
     rotate_right(&inbyte,1);				            //get ready to process the next bit
  }//end of for
}

void xmit(){						                    //transmits any waiting data
int c,outbyte,txdelay;
short DB = false;
short firstpack = true;					                //this is the first frame
   while (c0xmit > 0){				                    //do this if there is any data to transmit
  	   stuff = 0;					                    //keeps track of number of ones
          crcflag = FALSE;				                    //we aren't transmitting the CRC now
  	   xcrclo=xcrchi=0xFF;				                //reset CRC values
          output_high(m0);  				                //set up the MX614 to transmit
  	   outbyte = fram_read(txoutadd++);		            //reads the C0 byte
	   if (txoutadd == 8192) txoutadd = 2048;		    //rollover if necessary
   	   outbyte = fram_read(txoutadd++);		            //reads the next byte (00 if it's data, 02 or 03 for command)
	   if (txoutadd == 8192) txoutadd = 2048; 	        //rollover if necessary
	  if (outbyte == 2) setParms(2);				//set persist value
	  if (outbyte == 3)	setParms(3);					//set slottime value
	  if (outbyte ==6) sendParms();					//send Persist and Slottime
          if (outbyte == 0) output_high(PTT);		        //key the transmitter, but only if data, not KISS command
	   if (outbyte > 6){					 	        //need to reset... this is not a valid kiss frame
			output_low(PTT);
			resetxmit();
			return;
	   }
       if (firstpack){					                //got to do the txdelay if this is the first frame
		    txdelay = gettxdelay();			            //read the txdelay pot
   		 	flag = TRUE;				                //start with some flags (no bit stuffing)
		    set_rtcc(0);				                //set the timer to zero to begin timing
 		    for (c=0;c<txdelay;c++) (sendbyte(0));      //send nulls while TXDelay
   		 	sendbyte(0x7E); 			                //send flag
   		 	sendbyte(0x7E);				                //each byte takes approx 6.7 ms
   		 	flag = FALSE; 				                //done with flags
		    firstpack = false;			                //subsequent frames in this transmission don't need txdelay
	    }
        outbyte = fram_read(txoutadd++);		        //reads the first address byte.
	    if (txoutadd == 8192) txoutadd = 2048; 	        //rollover if necessary
   	    While (outbyte != 0xC0){			            //does the kiss substitutions and transmits byte
      	   switch (outbyte){							//do KISS substitutions, if necessary
      		    case 0xDB : db = TRUE; break;
          	    case 0xDC : if (db == TRUE){
                               sendbyte(0xC0);
                               db = FALSE;
                            }else sendbyte(0xDC);
                            break;
                case 0xDD : if (db == TRUE){
                                sendbyte(0xDB);
                                db = FALSE;
                            }else sendbyte(0xDD);
                            break;
                default   : sendbyte(outbyte);
            }//end of switch
            outbyte = fram_read(txoutadd++);
	         if (txoutadd == 8192) txoutadd = 2048;
       }//end of while
       crcflag = TRUE;					               //now sending CRC... don't do crc calc on these bytes
       xcrclo = xcrclo^0xff;				           //finish CRC calculation
       xcrchi = xcrchi^0xff;
       sendbyte(xcrclo); 				               //send the low byte of crc
       sendbyte(xcrchi); 				               //send the high byte of crc
       FLAG = TRUE;					                   //going to send flags now (no bit stuffing)
       crcflag = false;					               //done sending CRC
       sendbyte(0x7e);sendbyte(0x7e);			       // Send flags to end packet
       flag = false;  					               //done with flags
       c0xmit--; 				                       // decrement the number of frames waiting.
   }//end of while c0xmit
   output_low(PTT);     				               //unkey PTT
   output_low(radtx);					               //leave transmit pin low
   output_low(M0);  					               //MX 614 to recv mode
  	disable_interrupts(INT_TIMER2);						//reset slottime/persist
	slotcount = 0;
	xmitlatch = false;
}

//*********************RECEIVE ROUTINES FOLLOW***************************************

void addcrc(int crbyte){  				//ongoing computation of received CRC
int k,bt;
	for(k=0;k<8;k++){
		bt = crbyte & 0x01;
		#asm
  			BCF    03,0
   			RRF    crchi,F    	  	   //rotates the entire 16 bits to the right
   			RRF    crclo,F
		#endasm
   		if (((status & 0x01)^(bt)) == 0x01){
           		crchi = crchi^0x84;
           		crclo = crclo^0x08;
        	}
		rotate_right(&crbyte,1);
	}
}

#int_rb					               //interrupt routine for change on B
void resetclock(){			           //resyncs the bit timing on a zero
   if (input(radrx) != oldstate){	   //double check just to make sure the pin has really changed
	  disable_interrupts(INT_RB);	   //no more change on B interrupts until the next bit period
	  set_rtcc(240);			       //13 ticks (416 us) to next bit reading
	  bit_clear(intcon,2);		       //don't let it cause an interrupt immediately
   }
}


#INT_RTCC					               //interrupt routine for timer int
void rcvbits(){					           //fires every 832 us
int rbit;


    set_rtcc(224);				           //reset the clock
    bit_clear(intcon,2);			       //don't let it generate an immediate interrupt
    if (ones == 7){
	     done = true;				       //seven ones -- noise from the MX-614... time to start over
	     rcvinadd = begin - 2;			   //go back to before the "noise" data
	     if (rcvinadd == -2) rcvinadd = 2046;
	     if (rcvinadd == -1) rcvinadd = 2047;
     	     return;				           //we're done receiving, so leave
    }
    if (!fiveones){				           //do this unless we had 5 ones on last bit
	    rbit = 1;
  	    if (input(radrx) != oldstate){	   //if this is a zero
		      rbit = ones = 0;		       //reset ones since this was a zero
	 	      oldstate = !oldstate;		   //flip oldstate variable to reflect the change
    	 }else ones++;			           //else it must be a one so increment ones
    	 shift_right(&rcvbyte,1,rbit);	   //shift the new bit in on the right
    	 bitcount++;				       //increment number of bits received in this byte
    }
    if (fiveones) {  				        //if this is the bit after 5 consequtive ones
	     fiveones = false;			        //turn it off, since we are processing it here
    	  if (input(radrx) != oldstate){    //if bit is a zero, drop it (no shift right)
		      ones = 0;			            //reset ones since this was a zero
	 	      oldstate = !oldstate;		    //flip oldstate variable to reflect the change
	     }else{					            //otherwise this is another 1
	        ones++;				            //this is the sixth one ... must be a flag or noise
	        shift_right(&rcvbyte,1,1);	    //if it is a one, process it.
	        bitcount++;				        //increment number of bits in this byte.
	        isflag = true;
	     }//end of else
    }// end of if (fiveones)
    if (ones == 5) fiveones = true;		                       // if this is the fifth one, next bit gets specialprocessing
	if (bitcount == 8){				                   //do this if if the byte is done
        if (count < 13) count++;     	                       //stop at 13 because we don't want this rollover with long packets
	     if ((rcvbyte == 0x7E) && (count < 3)) count = 0;      //if this is a flag, don't start saving bytes yet.

	     if ((rcvbyte == 0x7E) && (isflag) && (count > 11)){   //do this when you get to the flag at the frame end
		       crchi = crchi ^ 0xFF;				           //finish the CRC calculation
		       crclo = crclo ^ 0xFF;
		       if ((crclo != minus2) || (crchi != minus1)){    //if CRC fails
			         rcvinadd = begin;  			           //revert to original address
		       }else{						                   //if crc checks
		            receivedpackets++;  			           //increment the number of received packets that haven't been sent out the serial port
		            fram_write(0xC0, rcvinadd++);		       //write C0 to finish fram
		            if (rcvinadd == 2048) rcvinadd = 0;	       //rollover if necessary
		            fram_write(0xC0, rcvinadd++);		       //write C0 to start next frame
		            if (rcvinadd == 2048) rcvinadd = 0;	       //rollover if necessary
		            fram_write(0x00, rcvinadd++);		       //write 00 to indicate data in this frame
		            if (rcvinadd == 2048) rcvinadd = 0;	       //rollover if necessary
		            begin = rcvinadd;				           //move begin up since this was a good frame
		       }//end of else
		       crchi = crclo = 0xFF;				           //prepare for next frame
		       count = 0;					                   //prepare for next frame
	     }
	     if (!((rcvbyte == 0x7E) && (isflag))){			       //do this if it is not a flag
	        	if (count > 2){					               //if we are at byte 3 or higher, send the data, calc crc
		          switch(minus2){                              //process the byte, do KISS substitutions if necessary
			           case 0xC0:  fram_write(0xDB,rcvinadd++);if (rcvinadd == 2048) rcvinadd = 0; fram_write(0xDC,rcvinadd++);if (rcvinadd == 2048) rcvinadd = 0;break;
		 	           case 0xDB:  fram_write(0xDB,rcvinadd++);if (rcvinadd == 2048) rcvinadd = 0; fram_write(0xDD,rcvinadd++);if (rcvinadd == 2048) rcvinadd = 0;break;
			           default:    fram_write(minus2,rcvinadd++);if (rcvinadd == 2048) rcvinadd = 0;
		          }
	   	       addcrc(minus2);				                   //add to ongoing crc calculation
		       }
	   	    minus2 = minus1;				                   //bump the history list
	   	    minus1 = rcvbyte;
	     }
        rcvbyte = bitcount = 0;					               //prepare for next byte
        isflag = false;
     }//end of if (bitcount==8)
     enable_interrupts(INT_RB );				               //turn the change on b interrupt back on.
}

#INT_TIMER2
void t2int(){
    slotcount++;					//increments the variable slotcount every 10 ms
	if (slotcount > SLOTTIME){	//if elapsed time is greater than SLOTTIME x 10 ms
		if (PERSIST > rand()) xmitlatch = true;					//if persist passes, allow transmit
		slotcount = 0;			//reset the elapsed time counter.
    }
    bit_clear(PIR1,1);
}

void findflag(){

	int time, countlegal, freetime = 0;
	oldstate = input(radrx);  				                      //remember the state of input pin
	while (input(radrx) == oldstate){			                  //wait for a change on the input pin
		 sendbyteserial(); 				                          //in the meantime, process data to/from serial port
		 rcvbyteserial();
		 restart_wdt();
	}
	set_rtcc(0);						                          //set clock to 0
	oldstate = !oldstate;					                      //since the state changed, flip oldstate
	do{							                                  //keep listening until you get 7 legal interals
	    if (C0XMIT>0)  enable_interrupts(INT_TIMER2 );	  //enable timer2 interrupt if there is data to transmit
	   while (input(radrx) == oldstate){			              //wait for portb to change again.
		 sendbyteserial();				                          //flush serial outgoing buffer
		 rcvbyteserial();				                          //grab bytes that show up on serial incoming buffer
		 restart_wdt();
 		 if (xmitlatch) xmit();					                  //if there is data to send, transmit it
 	   }
	   time = get_rtcc();					                      //time = how long it took.
	   set_rtcc(0);						                          //reset clock
	   oldstate = !oldstate;				                      //update laststate to current
	   
	   if ((time > 29) && (time < 35)) countlegal++;  	          //looking for a zero  (a legal interval)
	      else if ((time > 221) && (time < 234)) countlegal+=3;   //looking for 6 ones (a flag... legal interval) Transmission will idle on 0x00 or 0x7E
		  else countlegal = 0; 				                      //it wasn't a either of the above, start over.
	}while(countlegal < 5);					                      //this is packet, not noise, now look for a flag
	disable_interrupts(INT_TIMER2);						//don't transmit while data is being received.
	slotcount = 0;
	xmitlatch = false;
	do{
		while (input(radrx) == oldstate){                         //looking for 6 1's in a row  ... a flag
         	 	sendbyteserial();
         		rcvbyteserial();
		  	restart_wdt();
      		}      
		time = get_rtcc();                                       //how much time has elapsed?
		set_rtcc(0);                                             //start measuring time again
		oldstate = !oldstate;                                    //the pin state has flipped so flip oldstate variable
	}while (time < 210);  					                     //we are now at the end of the first flag
}

//*******************************MAIN PROGRAM LOOP FOLLOWS******************************

void main(){
  int temp;
   short always=1; 
    setup_comparator(7);		  //all digital I/O ... no analog
   set_tris_a(0b100000);	  //set up I/O on port a
   set_tris_b(0b11000010);	  //set up I/O on port b
   output_high(LED);		  //make sure LED is off
   output_low(PTT);			  //make sure PTT is off
   output_high(m0);
   output_high(m1); 		  //bring up in  614 in zero power mode
   output_high(cs);		      //get fram ready
   delay_ms(10);			  //wait for everything to settle
   output_low(m1);            //614 in rcv mode
   output_low(m0);
   port_b_pullups(true);	//save one resistor on B&
				   //following determines serial port speed from jumpers.
   if ((input(term1)) & (input(term2)) == 0) set_uart_speed(1200);		   	// both low = 1200 baud
   if ((input(term1)) && (!input(term2))) set_uart_speed(4800);				//A5 high and B7 low 4800 baud
   if (!(input(term1)) && (input(term2))) set_uart_speed(9600);				//B7 high and A5 low 9600 baud
   if ((input(term1)) & (input(term2)) == 1) set_uart_speed(19200);			// both high = 19200 baud

   setup_timer_2(T2_DIV_BY_16, 0xC3, 16);				//when turned on, will interrupt every 10 ms
   receivedpackets = c0xmit = 0;			          	//no waiting received or transmit packets at powerup
    setup_counters(RTCC_INTERNAL,RTCC_DIV_128);   		//now 32 ticks = 819 us
    rcvinadd = rcvoutadd = 0;			    	//start receive at the beginning of the fram space
   begin = 2;								//begin is the start of data after C0 00
  txinadd = txoutadd = 2048;				          	//2048 is start of transmit buffer
   resetxmit();
   PERSIST = READ_EEPROM(2);			//read Persist from memory
   SLOTTIME = READ_EEPROM(3);			//read SLOTTIME from memory
  enable_interrupts(GLOBAL);							
 											//just in case the wdt reset while data was still coming in on serial line
   while (always == 1){
	     crclo = crchi = 0xFF;				          	//set up next packet to be received
	     done = fiveones = false;
	     ones= rcvbyte = bitcount= count = 0;
	     fram_write(0xC0,rcvinadd++);if (rcvinadd == 2048) rcvinadd = 0;  //start KISS frame
	     fram_write(0x00,rcvinadd++);if (rcvinadd == 2048) rcvinadd = 0;
  	     findflag();					                        //go find a flag
	     output_low(LED);				                      	//packet is here, turn on DCD LED
  	     set_rtcc(207);					                      	//delay 1.5 bit intervals to get to the middle of the next bit.
	     bit_clear(intcon,2);				                   	//don't get an interrupt right away
	     enable_interrupts(INT_RTCC | INT_RB );	 		//enable interrutps to begin to collect data
	     while (!done) {					                    //Do this while data is being received.. stop when there is noise
	           sendbyteserial();   			                	//send received data out serial port
	           rcvbyteserial();				                	//take in data to transmit from serial port
			   restart_wdt();
	     }
	     output_high(LED);				                      	//packet is over, turn off DCD LED
		 if (SLOTTIME > 0){										//reset transmit holdoff counter unless SLOTTIME == 0)
			xmitlatch = false; 
			set_timer2(0); 
			slotcount = 0;	
         }
	     disable_interrupts(INT_RTCC |INT_RB );   				//disable interrupts to prepare for next findflag
	   
   }
}
