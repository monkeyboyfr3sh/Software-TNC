# Software-TNC
 
 ## Project Description

 Open source, purely software TNC that supports AX.25.

 ![Software TNC Demo](Documentation/Group-Information/Final-Presentation-Test.gif)
 
 This is a demo of the project receiving a KISS packet, sending an AFSK signal and then another TNC is receiving the message! The waveform in the top left is a live viewing of this audio signal. It is a short message so not many data points are captured here.

 ![Software TNC Level 1 Diagram](Documentation/Diagrams/TNCMCU-Level-1-Diagram-Scaled.png)

 This is a simple diagram breaking down some of the goals we had for the end TNC. Due to the restraints of a shortened semester and COVID-19 some features such as RS-232 and 3.5mm Jack were never fully implemented. I want to take a moment to specifically list somethings that stops this device from being a fully functioning proper TNC.

 ## Project Shortcomings
 1. **Limited voltage range input**
     - This is probably the biggest issue with out software that prevents this from being a fully functional TNC. As of now the device will only properly read a signal with a ptp of 3.3V, 1.65V centered.
     - The fix is relatively straightforward, the software should become a little smarter and add a couple more modes of receiving. There are likely other methods but this is just a recomendation from the original developer.
         - **Low Polling Mode (1)** : This will be the default receiving mode for the TNC when no signals are detected and establishes the baseline noise, etc. This should be set to conserve power and only exit this mode when some voltages are coming in.
         - **Very High Polling Mode (2)** : This will be stage 2 of the analog receiving logic. The callback function should do minimal to achieve as high of sample rate as possible. During this stage, the controller will be establishing the characteristics of the wave, specifically the amplitude of incoming wave, and DC offset. AX.25 packets are preceeded by a string of 0 bits so this would be a great time to lock this info in, but make sure to leave time for the clock to sync in stage 3. After these are done, ensure to update the asin LUT. The gen_asin() function will be a good place to do this.
         - **RX Logic, Medium Polling Mode (3)** : This is essentially what we have already implemented and will actually decode the message now that the characteristics of the incoming signal have been established. Its worth mentioning we focused on optimizing the callback function to be as minimal as possible to keep poll rates as high as possible. Higher poll rates tends to lend to a more accurate frequency representation of the incoming signal.
 
 2. **RS-232 Port**
     - It is normal for HAM radio users to plug a TNC via an RS-232 port into a computer. This wouldn't take too much effort to implement as this is mostly a hardware issue.

 3. **3.5mm Jack**
     - Again, another creature comfort implementation. Since our project never left the bread board is wasn't exactly neccessary for us to put this in. It is worth mentioning that the output from the controller should be as high of resolution as possible. So this means you should keep the output signal at 3.3V ptp and simply use extrernal hardware to lower the voltage before inputing into the radio.

 4. **Push-To-Talk Circui (PTT)**
     - Luckily, this is actually already designed and tested for you but due to our project never connecting to a radio, we don't have much documentation of physical testing. Take a look at the [PTT Circuit Information](https://github.com/monkeyboyfr3sh/Software-TNC/tree/main/Schematic/Ltspice/PTT-circuit) for the details on this.

## Quick Notes

 This is a slimmed version of our original GitHub repo. If anything seems to be missing or is not clear, visit:  https://github.com/MrLordLeon/TNCMCU
  
  - Keep in mind this other version was our raw data base when working on the project for a class and has a lot of unecessary or intermediate information
