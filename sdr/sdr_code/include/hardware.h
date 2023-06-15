#ifndef __hardware_h__
#define __haredware_h__

#define BLINKY_GPIO_PIN GPIO_Pin_8
#define BLINKY_GPIO_PORT GPIOB


#define I2C_SCL_PIN GPIO_Pin_6
#define I2C_SDA_PIN GPIO_Pin_7

#define DAC_I_PIN GPIO_Pin_4
#define DAC_Q_PIN GPIO_Pin_5
#define AUDIO_ENABLE_PIN GPIO_Pin_6

#define DAC_I_PORT GPIOA
#define DAC_Q_PORT GPIOA
#define AUDIO_ENABLE_PORT GPIOA

void LED_Init(void);
void DAC_Initialize(void);
void DAC_DMA_Init(u16* dacbuff16bit_ptr);
void Synthesizer_Init(u32 bound, u16 address);
#endif // __hardware_h__