# Software-TNC
 ## Project Description

  An open source, software based TNC that supports AX.25. Many features of common TNC's are not built-in but we hope to have shed light on not only how AX.25 actually works butto also enable a future developer to implement desired functionality such as bulletin board, APRS, etc.

#### **Software TNC Demo**
 ![Software TNC Demo](Documentation/Group-Information/Final-Presentation-Test.gif)
 
 This is a demo of the project receiving a KISS packet, sending an AFSK signal and then another TNC is receiving the message! The waveform in the top left is a live viewing of this audio signal. It is a short message so not many data points are captured here.

#### **Software TNC Level 1 Diagram**
 ![Software TNC Level 1 Diagram](Documentation/Diagrams/TNCMCU-Level-1-Diagram-Scaled.png)

 This is a simple diagram breaking down some of the goals we had for the end TNC. Due to the restraints of a shortened semester and COVID-19 some features such as RS-232 and 3.5mm Jack were never fully implemented. I want to take a moment to specifically list somethings that stops this device from being a fully functioning proper TNC.

Much of the software has been formatted to allow for queue structures to be implemented as data throughput requirements may need to grow with time. This was done by hosting packet information inside one of two custom datatype we defined as **UART_INPUT** and **PACKET_STRUCT** in AX.25.h. As you'll find, most functions simply operate on these global data packets but it wouldn't be very difficult to change the operation to allow input to data pointers.

This is a look at some of the info you'll find in these data types.
```
struct UART_INPUT { // For uart info
	int rx_cnt;
	int received_byte_cnt;
	int flags;
	uint8_t input;
	bool got_packet;

	//HEX Members, includes frame end flags
	uint8_t HEX_KISS_PACKET[KISS_SIZE_BYTES];//This is the buffer used to hold hex bits from UART
}UART_packet;

struct PACKET_STRUCT { // For the inbetween stage
	//AX.25 Members, does not include frame end flags
	bool AX25_PACKET[AX25_PACKET_MAX];//temporary stores bits received from radio, before formatting into AX.25 format

	//KISS Members, includes frame end flags
	bool KISS_PACKET[KISS_SIZE];

	/*
	 * 	Packet Pointers:
	 * 	Can reference data in either AX.25 packet or KISS packet
	 */
	bool *address;			//Pointer to address field in global buffer
	bool *control;			//Pointer to control field in global buffer
	bool *PID; 				//Pointer to PID field in global buffer, only present for I frames
	bool *Info;				//Pointer to info field in global buffer
	int  Info_Len;			//Length of info field, in bits
	bool *FCS;				//Pointer to fcs field in global buffer
	bool i_frame_packet;	//Flag to signal if packet is of type i-frames

	bool got_packet;
	int byte_cnt;


	//count for bit stuffed zeros in each field
	int stuffed_address;
	int stuffed_control;
	int stuffed_PID;
	int stuffed_Info;
	int stuffed_FCS;

	int bit_stuffed_zeros;

	//CRC
	uint16_t crc; 				//crc value after calculating data from PC
	int crc_count;
	bool check_crc;			//indicates weather validating fcs field or creating fcs field
}global_packet;
```

----------------------------------
 ## Project Shortcomings
 1. **Limited voltage range input**
     - This is probably the biggest issue with out software that prevents this from being a fully functional TNC. As of now the device will only properly read a signal with a ptp of 3.3V, 1.65V centered.
     - The fix is relatively straightforward, the software should become a little smarter and add a couple more modes of receiving. There are likely other methods but this is just a recomendation from the original developer.
         - **(Stage 1) Low Polling Mode** : This will be the default receiving mode for the TNC when no signals are detected and establishes the baseline noise, etc. This should be set to conserve power and only exit this mode when some voltages are coming in.
         - **(Stage 2) Very High Polling Mode** : This will be stage 2 of the analog receiving logic. The callback function should do minimal to achieve as high of sample rate as possible. During this stage, the controller will be establishing the characteristics of the wave, specifically the amplitude of incoming wave, and DC offset. AX.25 packets are preceeded by a string of 0 bits so this would be a great time to lock this info in, but make sure to leave time for the clock to sync in stage 3. After these are done, ensure to update the asin LUT. The gen_asin() function will be a good place to do this.
         - **(Stage 3) RX Logic, Medium Polling Mode** : This is essentially what we have already implemented and will actually decode the message now that the characteristics of the incoming signal have been established. Its worth mentioning we focused on optimizing the callback function to be as minimal as possible to keep poll rates as high as possible. Higher poll rates tends to lend to a more accurate frequency representation of the incoming signal.
 2. **RS-232 Port**
     - It is normal for HAM radio users to plug a TNC via an RS-232 port into a computer. This wouldn't take too much effort to implement as this is mostly a hardware issue.
 3. **3.5mm Jack**
     - Again, another creature comfort implementation. Since our project never left the bread board is wasn't exactly neccessary for us to put this in. It is worth mentioning that the output from the controller should be as high of resolution as possible. So this means you should keep the output signal at 3.3V ptp and simply use extrernal hardware to lower the voltage before inputing into the radio.
 4. **Push-To-Talk Circuit (PTT)**
     - Luckily, this is actually already designed and tested for you but due to our project never connecting to a radio, we don't have much documentation of physical testing. Take a look at the [PTT Circuit Information](https://github.com/monkeyboyfr3sh/Software-TNC/tree/main/Schematic/Ltspice/PTT-circuit) for the details on this.

----------------------
## Final Notes

  1. This repo is a slimmed version of our original GitHub repo. If anything seems to be missing or is not clear, visit:  https://github.com/MrLordLeon/TNCMCU
     - Keep in mind the other repo was our raw data base when working on the project for a senior design capstone project and has a lot of unecessary or intermediate information
  2. Hopefully this is a good tool for learning about AX.25, KISS, and how to use this protocol. Be sure to checkout some of the appendices in the out design paper as this has detailed information on subsystems as well.

------------------
## Any further questions can be directed to the following:
|Developer|Email|GitHub|
|----------|--------------------|---------------------------------|
|David Cain|d.cain2740@gmail.com|https://github.com/monkeyboyfr3sh|
|Kobe Keopraseuth|kobekeo@gmail.com|https://github.com/kobekeo24|
|Kaleb Leon|kalebleon@aol.com|https://github.com/MrLordLeon|