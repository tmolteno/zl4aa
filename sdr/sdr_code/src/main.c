#include <ch32v30x.h>
#include <debug.h>

#include "hardware.h"
#include "Si5351.h"

void NMI_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void HardFault_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void Delay_Init(void);
void Delay_Ms(uint32_t n);



enum STATE {
	STATE_IDLE,
	STATE_SENDING,
	STATE_RECEIVING,
	STATE_ERROR
};


/*********************************************************************
 * @fn      TIM8_Init
 *
 * @brief   Initializes TIM3 collection.
 *
 * @param   arr - TIM_Period
 *          psc - TIM_Prescaler
 *
 * @return  none
 */
void TIM8_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure={0};

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_TIM8, ENABLE );

    TIM_TimeBaseInitStructure.TIM_Period = arr;
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Down;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter =  0x00;
    TIM_TimeBaseInit( TIM8, &TIM_TimeBaseInitStructure);

	TIM_SelectOutputTrigger(TIM8, TIM_TRGOSource_Update);
	TIM_Cmd(TIM8, ENABLE);
}

volatile u8 current_state = STATE_IDLE;

/* Global Variable */
u16 dacbuff16bit[8]={0x123,0x234,0x345,0x456,0x567,0x678,0x789,0x89a};

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	SystemCoreClockUpdate();
	Delay_Init();

	LED_Init();
	DAC_Initialize();
	DAC_DMA_Init(&dacbuff16bit[0]);
	TIM8_Init(0x7,48000-1);
	Synthesizer_Init(100000, 0x77);


	uint8_t ledState = STATE_ERROR;
	while (1)
	{
		switch (current_state) {
			case STATE_IDLE:
				break;
			case STATE_SENDING:
				break;
			case STATE_RECEIVING:
				break;
			case STATE_ERROR:
				GPIO_WriteBit(BLINKY_GPIO_PORT, BLINKY_GPIO_PIN, ledState);
				ledState ^= 1; // invert for the next run
				Delay_Ms(1000);
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
