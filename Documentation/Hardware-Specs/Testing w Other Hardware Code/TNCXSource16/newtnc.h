#include <16F628A.h>
#use delay(clock=20000000)
#fuses HS,NOWDT,PUT,NOLVP, NOBROWNOUT, NOPROTECT, NOMCLR
//#fuses HS,WDT,PUT,NOLVP, NOBROWNOUT, PROTECT, NOMCLR

#define clock   40	//clock on fram
#define A1   41		//TXDelay pot
#define data   42	//data on fram
#define cs  43		//chip select on fram
#define LED 44		//DCD LED
#define term1 45	//serial baudrate (with term2)
#define PTT   48	//radio PTT
#define rxd   49	//serial receive data
#define txd   50	//serial transmit data
#define radtx 51	//radio transmit data
#define m1   52		//modem control line
#define m0   53		//modem control line
#define radrx 54	//radio receive data
#define term2 55	//serial baudrate (with term1)
#use rs232(baud=1200,parity=N,xmit=txd,rcv=rxd,ERRORS)
#use fast_io(A)
#use fast_io(B)
#byte pir1 = 0x0C
#byte status = 0x03
#byte portb = 0x06
#byte porta = 0x05
#byte intcon = 0x0B
#byte pir1 = 0x0C
#byte cmcon = 0x1F
