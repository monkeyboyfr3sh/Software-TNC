void fram_bits(int obt){  			        //outputs the bits to the FRAM
int bittc,mask;
   mask =0b10000000;   			       //start with MSB
   for(bittc=0;bittc<8;bittc++){
      output_low(data);
      if ((mask & obt) != 0) (output_high(data));
      output_high(clock);
      output_low(clock);
      rotate_right(&mask,1);
   }
}
void fram_write(int byt, long addr){  	//write a byte to FRAM
    int hiaddr, loaddr;
    hiaddr = (int) (addr >> 8);
    loaddr = (int) (addr & 0xFF);
    set_tris_a(0b100000);
    output_low(cs);
    fram_bits(0b00000110); 		  // set write enable latch
    output_high(cs);
    output_low(cs);
    fram_bits(0b00000010);		     //this is a write
    fram_bits(hiaddr);			     //write the two byte address
    fram_bits(loaddr);
    fram_bits(byt);			        //write the data
    output_high(cs);			       //all done
 }


int fram_read(long addr){   		//read a byte from FRAM
int inbyte,bitc, hiaddr, loaddr;
   hiaddr = (int) (addr >> 8);
   loaddr = (int) (addr & 0xFF);
   set_tris_a(0b100000);
   output_low(cs);
   fram_bits(0b00000011);		   //this is a read
   fram_bits(hiaddr);			   //send the two byte address
   fram_bits(loaddr);
   set_tris_a(0b100100);  		   //set I/O to read from FRAM
   inbyte = 0;
   for(bitc=0;bitc<8;bitc++){		//clock in the bits
         output_high(clock);
         if (input(data)) (inbyte++);
         output_low(clock);
         rotate_left(&inbyte,1);
    }
    rotate_right(&inbyte,1);
    output_high(cs);			      //add done
    return inbyte;			      //return the read byte
}

