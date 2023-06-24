/********************************** (C) COPYRIGHT *******************************

* File Name          : main.c
* Author             : Muhammad_Minhaj
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : Main program body.
* https://github.com/M-Minhaj/u8g2-with-RISC-V-MCU---CH32V305-7-MCU

*******************************************************************************/

#include "debug.h"
#include <u8g2.h>
#include "ch32v30x.h"

#include "i2c_tx.h"
#include "oled_min.h"



uint8_t u8g2_gpio_and_delay_stm32(U8X8_UNUSED u8x8_t *u8x8, U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int, U8X8_UNUSED void *arg_ptr)
{

  switch(msg)
  {
    case U8X8_MSG_GPIO_AND_DELAY_INIT:	// called once during init phase of u8g2/u8x8

    	Delay_Init();
    	break;							// can be used to setup pins
    case U8X8_MSG_DELAY_100NANO:		// delay arg_int * 100 nano seconds
      Delay_Us(arg_int/10);
      break;
    case U8X8_MSG_DELAY_10MICRO:		// delay arg_int * 10 micro seconds
      Delay_Us(arg_int*10);


      break;
    case U8X8_MSG_DELAY_MILLI:			// delay arg_int * 1 milli second
    	Delay_Ms(arg_int);

      break;

    default:
      return 0;
    }

  return 1;
}

void I2C_Master_Transmit(uint8_t *buff, uint8_t idx );

 


uint8_t u8x8_byte_wch32_hw_i2c(u8x8_t *u8g2, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
	// u8g2/u8x8 will never send more than 32 bytes between START_TRANSFER and END_TRANSFER
//	 add extra byte for the i2c address
	static uint8_t buffer[34];
	static uint8_t buf_idx;
	uint8_t *data;
	switch(msg)  {
		case U8X8_MSG_BYTE_SEND:
		    data = (uint8_t *)arg_ptr;
		    while( arg_int > 0 )
		    {
		    	buffer[buf_idx++] = *data;
		    	data++;
		    	arg_int--;
		    }
			break;
		case U8X8_MSG_BYTE_INIT:
			// add your custom code to init i2c subsystem
      OLED_I2C_init();
			break;
		case U8X8_MSG_BYTE_SET_DC:
		//	 ignored for i2c
			break;
		case U8X8_MSG_BYTE_START_TRANSFER:
			buf_idx = 0;
			break;
		case U8X8_MSG_BYTE_END_TRANSFER:
			I2C_Master_Transmit((uint8_t *)buffer, buf_idx);
			break;
		default:
			return 0;
	}


	return 1;
}




u8g2_t u8g2;







void u8g2_setup(void)
{
 u8g2_Setup_ssd1306_i2c_128x64_noname_f(&u8g2, U8G2_R0,u8x8_byte_wch32_hw_i2c,u8g2_gpio_and_delay_stm32);

 u8g2_InitDisplay(&u8g2); // send init sequence to the display, display is in sleep mode after this,
 u8g2_SetPowerSave(&u8g2,0);
 u8g2_ClearBuffer(&u8g2);
 u8g2_SetFont(&u8g2, u8g2_font_fub14_tf);

 u8g2_DrawStr(&u8g2,2,30,"ZL4AA");

 u8g2_SendBuffer(&u8g2);
}




void I2C_Master_Transmit(uint8_t *buff, uint8_t idx )
{
 u8 i = 0;

  OLED_I2C_start(OLED_ADDR);

  while(idx > 0)
  {
     OLED_I2C_write(buff[i]);
     idx--;
     i++;
  }
  OLED_I2C_stop();
}