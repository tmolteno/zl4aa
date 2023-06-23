// ===================================================================================
// SSD1306 128x64 Pixels OLED Minimal Functions                               * v1.0 *
// ===================================================================================
//
// Collection of the most necessary functions for controlling an SSD1306 128x64 pixels
// I2C OLED.
//
// References:
// -----------
// - TinyOLEDdemo: https://github.com/wagiminator/attiny13-tinyoleddemo
// - Neven Boyanov: https://github.com/tinusaur/ssd1306xled
// - Stephen Denne: https://github.com/datacute/Tiny4kOLED
//
// 2022 by Stefan Wagner: https://github.com/wagiminator

#include "oled_min.h"

// OLED initialisation sequence
const uint8_t OLED_INIT_CMD[] = {
  OLED_MULTIPLEX,   0x3F,                 // set multiplex ratio  
  OLED_CHARGEPUMP,  0x14,                 // set DC-DC enable  
  OLED_MEMORYMODE,  0x00,                 // set horizontal addressing mode
  OLED_COLUMNS,     0x00, 0x7F,           // set start and end column
  OLED_PAGES,       0x00, 0x3F,           // set start and end page
  OLED_COMPINS,     0x12,                 // set com pins
  OLED_XFLIP, OLED_YFLIP,                 // flip screen
  OLED_DISPLAY_ON                         // display on
};

// OLED init function
void OLED_init(void) {
  uint8_t i;
  OLED_I2C_start(OLED_ADDR);                   // start transmission to OLED
  OLED_I2C_write(OLED_CMD_MODE);               // set command mode
  for(i = 0; i < sizeof(OLED_INIT_CMD); i++)
    OLED_I2C_write(OLED_INIT_CMD[i]);          // send the command bytes
  OLED_I2C_stop();                             // stop transmission
}

// Start sending data
void OLED_data_start(void) {
  OLED_I2C_start(OLED_ADDR);                   // start transmission to OLED
  OLED_I2C_write(OLED_DAT_MODE);               // set data mode
}

// Start sending command
void OLED_command_start(void) {
  OLED_I2C_start(OLED_ADDR);                   // start transmission to OLED
  OLED_I2C_write(OLED_CMD_MODE);               // set command mode
}

// OLED send command
void OLED_send_command(uint8_t cmd) {
  OLED_I2C_start(OLED_ADDR);                   // start transmission to OLED
  OLED_I2C_write(OLED_CMD_MODE);               // set command mode
  OLED_I2C_write(cmd);                         // send command
  OLED_I2C_stop();                             // stop transmission
}

// OLED set cursor position
void OLED_setpos(uint8_t x, uint8_t y) {
  OLED_I2C_start(OLED_ADDR);                   // start transmission to OLED
  OLED_I2C_write(OLED_CMD_MODE);               // set command mode
  OLED_I2C_write(OLED_PAGE | y);	              // set page start address
  OLED_I2C_write(x & 0x0F);			              // set lower nibble of start column
  OLED_I2C_write(OLED_COLUMN_HIGH | (x >> 4)); // set higher nibble of start column
  OLED_I2C_stop();                             // stop transmission
}

// OLED fill screen
void OLED_fill(uint8_t p) {
  OLED_setpos(0, 0);                      // set cursor to display start
  OLED_I2C_start(OLED_ADDR);                   // start transmission to OLED
  OLED_I2C_write(OLED_DAT_MODE);               // set data mode
  for(uint16_t i=128*8; i; i--) OLED_I2C_write(p); // send pattern
  OLED_I2C_stop();                             // stop transmission
}

// OLED draw bitmap
void OLED_draw_bmp(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, const uint8_t* bmp) {
  for(int y = y0; y < y1; y++) {
    OLED_setpos(x0, y);
    OLED_I2C_start(OLED_ADDR);
    OLED_I2C_write(OLED_DAT_MODE);

    for(int x = x0; x < x1; x++)
      OLED_I2C_write(*bmp++);
    OLED_I2C_stop();
  }
}

u8 reverse(u8 b) {
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}


/**
 * @brief Draw an XBM image converted into 1-bit vertical mode
*/
void OLED_draw_xbm_vertical(const uint8_t * xbm) {
    for(int y=0; y<8; y++) {
      OLED_setpos(0,y);
      OLED_data_start();
      for(int x=0; x<128; x++) {
        OLED_I2C_write(xbm[x + (y * 128)]);
      }
      OLED_I2C_stop();
    }
}

/**
 * @brief Draw an XBM image as output by GIMP (in 1-bit horizontal mode)
*/
void OLED_draw_xbm(const uint8_t * xbm) {
    for(int y=0; y<64; y++) {
      OLED_setpos(0,y);
      OLED_data_start();
      for(int x=0; x<16; x++) {
        OLED_I2C_write(xbm[x + (y * 16)]);
      }
      OLED_I2C_stop();
    }
}