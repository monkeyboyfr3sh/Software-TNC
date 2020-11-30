# Software information

## Getting Started
Follow this guide to install the STM32Cube IDE. This is an IDE created by ST for their boards/processors.
The STM32Cube IDE is based on ECLIPSE add-ons and GNU C/C++ toolchain and GDB debugger. 

https://www.digikey.com/en/maker/projects/getting-started-with-stm32-introduction-to-stm32cubeide/6a6c60a670c447abb90fd0fd78008697

This guide will also step you through the process of creating a simple code to blink the onboard LD2.
**This project is based the NUCLEO-F446RE. _Porting will need to be done for other MCUs!_**

## Breakdown of include file contents
- **AX.25.h**
  - This file defines methods for handling data processing of AX.25. These methods **do not** directly control hardware.
- **debug.h**
  - This file defines methods for debugging. These methods mostly consist of print statements.
  - To enable debug printing, set debug_printing true
  - To force controller into repeat broadcastmode, set BROADCASTR true
- **FreqIO.h**
  - This file defines methods for directly controller hardware. Setting hardware mode, timer reload settings, DAC control, etc. are setup here.
- **interrupt_services.h**
  - This file defines methods for handling interrupt callbacks. Interrupt corresponding macro definitions are found here.
- **sine.h**
  - This file defines LUTs for the controller. A sine wave LUT is saved and a arcsin LUT are saved.