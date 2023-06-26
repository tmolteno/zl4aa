#include <ch32v30x.h>
#include <debug.h>
#include "tiny_invaders.h"
#include "hardware.h"
#include "Si5351.h"
#include "oled_min.h"
#include "splash_screen.xbm"
void NMI_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void HardFault_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void EXTI9_5_IRQHandler(void)  __attribute__((interrupt(/*"WCH-Interrupt-fast"*/)));

void Delay_Init(void);
void Delay_Ms(uint32_t n);


void KEY_Interrupt_Init(void) {

    GPIO_InitTypeDef GPIO_InitStructure = {0};
    EXTI_InitTypeDef EXTI_InitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

	/* PB9 'MODE' Key */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = MODE_KEY_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(MODE_KEY_PORT, &GPIO_InitStructure);
	
	/* PC8 PTT Key */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_InitStructure.GPIO_Pin = PTT_KEY_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(PTT_KEY_PORT, &GPIO_InitStructure);

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource9);

    EXTI_InitStructure.EXTI_Line = EXTI_Line9;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

#include "state.h"
#include <u8g2.h>

extern u8g2_t u8g2;

void u8g2_setup(void);

volatile u8 current_state = STATE_IDLE;
volatile u8 next_state = STATE_IDLE;

/* Global define */
#define N_SAMPLES 64

/* Global Variable */
u16 DAC_Value[N_SAMPLES]={2048,2248,2447,2642,2831,3013,3185,3347,3496,3631,3750,3854,3940,4007,4056,4086,
                         4095,4086,4056,4007,3940,3854,3750,3631,3496,3347,3185,3013,2831,2642,2447,2248,
                         2048,1847,1648,1453,1264,1082,910 ,748 ,599 ,464 ,345 ,241 ,155 ,88  ,39  ,9   ,
                         0   ,9   ,39  ,88  ,155 ,241 ,345 ,464 ,599 ,748 ,910 ,1082,1264,1453,1648,1847};

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	SystemCoreClockUpdate();
	Delay_Init();

	Delay_Ms(1000);

	GPIO_Pins_Init();
	DAC_Initialize();

	AudioEnable();

	DAC_DMA_Init(&DAC_Value[0], N_SAMPLES);
	DAC_Timer_Init(0x7,24000-1);

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_RNG, ENABLE);
	RNG_Cmd(ENABLE);

	Synthesizer_Init(100000, 0x77);

    KEY_Interrupt_Init();

	OLED_I2C_init();

	u8g2_setup();

	u8g2_DrawXBM(&u8g2, 0, 0, 128, 64, splash_screen_bits);
 	u8g2_SendBuffer(&u8g2);

	uint8_t ledState = 0;
	while (1)
	{
		if (current_state != next_state) {
			u8g2_ClearBuffer(&u8g2);
			switch (next_state) {
				case STATE_IDLE:
					u8g2_DrawStr(&u8g2,2,30,"IDLE");
					break;
				case STATE_SENDING:
					u8g2_DrawStr(&u8g2,2,30,"TX");
					break;
				case STATE_RECEIVING:
					u8g2_DrawStr(&u8g2,2,30,"RX");
					break;
				case STATE_GAME:
					u8g2_DrawStr(&u8g2,2,30,"GAME");

					tiny_invaders_setup();
					break;
			}
 			u8g2_SendBuffer(&u8g2);
			current_state = next_state;
		}
		switch (current_state) {
			case STATE_IDLE:
				GPIO_WriteBit(BLINKY_GPIO_PORT, BLINKY_GPIO_PIN, ledState);
				ledState ^= 1; // invert for the next run
				Delay_Ms(1000);
				break;
			case STATE_SENDING:
				GPIO_WriteBit(BLINKY_GPIO_PORT, BLINKY_GPIO_PIN, ledState);
				ledState ^= 1; // invert for the next run
				Delay_Ms(100);
				break;
			case STATE_RECEIVING:
				break;
			case STATE_GAME:
				tiny_invaders_loop();
				Delay_Ms(2);
				break;
		}
	}
}

void NMI_Handler(void) {}
void HardFault_Handler(void)
{
	while (1)
	{
	}
}



/*********************************************************************
 * @fn      EXTI9_IRQHandler
 *
 * @brief   This function handles EXTI0 Handler.
 *
 * @return  none
 */
void EXTI9_5_IRQHandler(void)
{
  	if(EXTI_GetITStatus(EXTI_Line9) != RESET)
    {
		switch (current_state) {
			case STATE_IDLE:
				next_state = STATE_SENDING;
				break;
			case STATE_SENDING:
				next_state = STATE_RECEIVING;
				break;
			case STATE_RECEIVING:
				next_state = STATE_GAME;
				break;
			case STATE_GAME:
				next_state = STATE_IDLE;
				break;
		}
        EXTI_ClearITPendingBit(EXTI_Line9); /* Clear Flag */
    }
}