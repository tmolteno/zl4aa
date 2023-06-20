// ===================================================================================
// Tiny Joypad Drivers for CH32V305                                          * v1.0 *
// ===================================================================================
//
// MCU abstraction layer.
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <debug.h>
#include "oled_min.h"

// Pin assignments
#define PIN_ACT     GPIO_Pin_8   // pin conected to direction buttons: PTT Button PC8
#define GPIO_ACT    GPIOC

// Joypad calibration values
#define JOY_N       197   // joypad UP
#define JOY_NE      259   // joypad UP + RIGHT
#define JOY_E       90    // joypad RIGHT
#define JOY_SE      388   // joypad DOWN + RIGHT
#define JOY_S       346   // joypad DOWN
#define JOY_SW      616   // joypad DOWN + LEFT
#define JOY_W       511   // joypad LEFT
#define JOY_NW      567   // JOYPAD UP + LEFT
#define JOY_DEV     20    // deviation

// Sound enable
#define JOY_SOUND   1     // 0: no sound, 1: with sound

// Game slow-down delay
#define JOY_SLOWDOWN()    Delay_Ms(10)

// Init driver
//tatic void JOY_init(void) {
 // PIN_input_AN(PIN_PAD);
 // PIN_input_PU(PIN_ACT);
 // PIN_output(PIN_BEEP);
 // PIN_high(PIN_BEEP);
 // OLED_init();
 // ADC_init();
 // ADC_input(PIN_PAD);
//}

// OLED commands
#define JOY_OLED_end              OLED_I2C_stop
#define JOY_OLED_send(b)          OLED_I2C_write(b)
#define JOY_OLED_send_command(c)  OLED_send_command(c)
#define JOY_OLED_data_start(y)    {OLED_setpos(0,y);OLED_data_start();}

// Buttons
bool JOY_act_pressed(void) {
  return(!GPIO_ReadInputDataBit(GPIO_ACT, PIN_ACT));
}

bool JOY_act_released(void) {
  return(!JOY_act_pressed());
}

inline bool JOY_pad_pressed(void) {
  return 0; // (ADC_read() > 10);
}

inline bool JOY_pad_released(void) {
  return !JOY_pad_pressed();
}

inline bool JOY_all_released(void) {
  return(JOY_act_released() && JOY_pad_released());
}


#if 0
static inline uint8_t JOY_up_pressed(void) {
 uint16_t val = ADC_read();
 return(   ((val > JOY_N  - JOY_DEV) && (val < JOY_N  + JOY_DEV))
         | ((val > JOY_NE - JOY_DEV) && (val < JOY_NE + JOY_DEV))
         | ((val > JOY_NW - JOY_DEV) && (val < JOY_NW + JOY_DEV)) );
}

static inline uint8_t JOY_down_pressed(void) {
 uint16_t val = ADC_read();
 return(   ((val > JOY_S  - JOY_DEV) && (val < JOY_S  + JOY_DEV))
         | ((val > JOY_SE - JOY_DEV) && (val < JOY_SE + JOY_DEV))
         | ((val > JOY_SW - JOY_DEV) && (val < JOY_SW + JOY_DEV)) );
}
#endif

static inline uint8_t JOY_left_pressed(void) {
#if 0
 uint16_t val = ADC_read();
 return(   ((val > JOY_W  - JOY_DEV) && (val < JOY_W  + JOY_DEV))
         | ((val > JOY_NW - JOY_DEV) && (val < JOY_NW + JOY_DEV))
         | ((val > JOY_SW - JOY_DEV) && (val < JOY_SW + JOY_DEV)) );
#else 
  return 0;
#endif
}

static inline uint8_t JOY_right_pressed(void) {
#if 0
 uint16_t val = ADC_read();
 return(   ((val > JOY_E  - JOY_DEV) && (val < JOY_E  + JOY_DEV))
         | ((val > JOY_NE - JOY_DEV) && (val < JOY_NE + JOY_DEV))
         | ((val > JOY_SE - JOY_DEV) && (val < JOY_SE + JOY_DEV)) );
#else
  return 0;
#endif
}

// Buzzer
void JOY_sound(uint8_t freq, uint8_t dur) {
  #if 0
  while(dur--) {
    #if JOY_SOUND == 1
    if(freq) PIN_low(PIN_BEEP);
    #endif
    DLY_us(255 - freq);
    PIN_high(PIN_BEEP);
    DLY_us(255 - freq);
  }
  #endif /* 0*/
}

// Pseudo random number generator
uint16_t rnval = 0xACE1;
uint16_t JOY_random(void) {
  rnval = (rnval >> 0x01) ^ (-(rnval & 0x01) & 0xB400);
  return rnval;
}

// Delays
#define JOY_DLY_ms    Delay_Ms
#define JOY_DLY_us    Delay_Us

// Additional Defines
#define abs(n) ((n>=0)?(n):(-(n)))

long map(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

#ifdef __cplusplus
};
#endif
