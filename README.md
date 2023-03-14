# ZL4AA Radio Projects

Author: Tim Molteno (ZL4TCM)

Some very basic radios, developed as a way to learn about radio.

## SDR

Very basic radio project. It uses the CH32V305RBT6 RISC-V microprocessor (http://www.wch-ic.com/downloads/CH32V20x_30xDS0_PDF.html). It has the following features
made by Nanjing Qinheng Microelectronics Co. 

* 144 MHz 32-bit RISC-V core (IMAFC instructions)
* Single cycle multiply and hardware division (FPU)
* 64K SRAM
* 4 Op Amps
* 2 x 12-bit DAC !MHz (Dual Channel Mode available for Baseband)
* 2 x 12-bit ADC 1MHz

Reference manual (http://www.wch-ic.com/downloads/CH32FV2x_V3xRM_PDF.html)

### IMAFC

Set of RISC-V instruction extensions.

Integer (M) multiply and divide.
Atomic (A) instructions
Floating Point (F) single-precision
Compressed (C) instructions.


## Architecture

A local oscillator with I/Q outputs.

A switching I/Q mixer to zero IF.

40 dB LNA on Antenna Input for RX.

DAC I,Q -> MIXER -> HPF -> PA


## Gui.

SPI OLED screen, LED, Button1, Button2, Morse Key. Beep through speaker.

## Basic 6m Morse TX

Set LO to 50 MHz. 

Button 1. Increase f
Button 3. Decrease f,
Button 3 ON. Tx on (beep)




