Version 1.6 This file created 1/9/06

Copyright (c) 2003-2006 John A. Hansen

All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer. 

Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.
 
The name John A. Hansen may not be used to endorse or promote products 
derived from this software without specific prior written permission.
 
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER  "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER BE 
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
THE POSSIBILITY OF SUCH DAMAGE.


This archive contains the current final release of the firmware for TNC-X.  
It was written in CCS C code (for the PCM compiler ver 3.163) and developed 
using Microchip's MPLAB ver. 7.3.  Included in this archive are the following
source files:

newtnc.c	program code for TNC-X
newtnc.h	definitions for TNC-X
fram8K.c	routines for reading and writing to/from the FRAM 25640.

In addition, the newtnc.hex contains the code that must be loaded into the 
16F628A to create a TNC-X.

NOTE:  IN ORDER TO HAVE THIS .HEX FILE WORK PROPERLY, IN ADDITION TO BURNING THE .HEX FILE
INTO YOUR CHIP YOU MUST ALSO BURN THE FOLLOWING VALUES INTO THE CHIP'S EEPROM MEMORY:

LOCATION 	VALUE
   02             0X40
   03             0X0A

IF YOU DON'T DO THIS, THE TNC-X WILL NOT TRANSMIT.

Changes since version 1.2:

1. Bug fixed that caused an error when a packet was received with a bad CRC followed 
immediately (only one flag in between) a a good packet.

2. Errors fixed to the implementation of the SLOTTIME/PERSIST function.

3. SLOTTIME and PERSIST can now be set by the user (using the TXTest program also 
available on the documentation page of the www.tnc-x.com web site).

Changes since version 1.5

A Persist value of 0 is no longer allowed because it caused transmit to fail.

The author can be reached at:  hansen@fredonia.edu

More information on TNC-X can be found at: www.tnc-x.com
