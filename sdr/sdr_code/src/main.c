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

volatile u8 current_state = STATE_ERROR;

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

	GPIO_Pins_Init();
	DAC_Initialize();

	AudioEnable();

	DAC_DMA_Init(&DAC_Value[0], N_SAMPLES);
	TIM8_Init(0x7,48000-1);
	Synthesizer_Init(100000, 0x77);


	uint8_t ledState = 0;
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
