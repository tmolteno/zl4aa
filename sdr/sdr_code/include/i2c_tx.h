// ===================================================================================
// Basic I2C Master Functions for CH32V003                                    * v1.0 *
// ===================================================================================
//
// External pull-up resistors (4k7 - 10k) are mandatory!
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <ch32v30x.h>

// I2C Definitions
#define OLED_I2C_CLKRATE   400000    // I2C bus clock rate (Hz)

#define OLED_I2C_PORT I2C2
#define OLED_I2C_SCL_PIN GPIO_Pin_10
#define OLED_I2C_SDA_PIN GPIO_Pin_11

// I2C Functions
void OLED_I2C_init(void);            // I2C init function
void OLED_I2C_start(uint8_t addr);   // I2C start transmission, addr must contain R/W bit
void OLED_I2C_write(uint8_t data);   // I2C transmit one data byte via I2C
void OLED_I2C_stop(void);            // I2C stop transmission


#ifdef __cplusplus
};
#endif
