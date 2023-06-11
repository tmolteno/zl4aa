#if defined(CH32V00X)
#include <ch32v00x.h>
#elif defined(CH32V10X)
#include <ch32v10x.h>
#elif defined(CH32V20X)
#include <ch32v20x.h>
#elif defined(CH32V30X)
#include <ch32v30x.h>
#endif
#include <debug.h>

#define BLINKY_GPIO_PORT GPIOB
#define BLINKY_GPIO_PIN GPIO_Pin_8

#define I2C_SCL_PIN GPIO_Pin_6
#define I2C_SDA_PIN GPIO_Pin_7

void NMI_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void HardFault_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void Delay_Init(void);
void Delay_Ms(uint32_t n);

void LED_Init(void) {
	GPIO_InitTypeDef GPIO_InitStructure = {0};
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Pin = BLINKY_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(BLINKY_GPIO_PORT, &GPIO_InitStructure);
}

void SYnthesizer_Init(u32 bound, u16 address)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    I2C_InitTypeDef  I2C_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

	/* I2S interface pins */
    GPIO_InitStructure.GPIO_Pin = I2C_SCL_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = I2C_SDA_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    I2C_InitStructure.I2C_ClockSpeed = bound;
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = 0x77; // Unimportant Init
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_Init(I2C1, &I2C_InitStructure);

    I2C_Cmd(I2C1, ENABLE);
    I2C_AcknowledgeConfig(I2C1, ENABLE);

    while( I2C_GetFlagStatus( I2C1, I2C_FLAG_BUSY ) != RESET );
}


void I2C_StartTx(u8 slave_address) {
	while( I2C_GetFlagStatus( I2C1, I2C_FLAG_BUSY ) != RESET );
	I2C_GenerateSTART(I2C1, ENABLE);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

	I2C_Send7bitAddress( I2C1, slave_address << 1, I2C_Direction_Transmitter );

    while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED ) );
}

void I2C_StartRx(u8 slave_address) {
	while( I2C_GetFlagStatus( I2C1, I2C_FLAG_BUSY ) != RESET );
	I2C_GenerateSTART(I2C1, ENABLE);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

	I2C_Send7bitAddress( I2C1, slave_address << 1, I2C_Direction_Receiver );

	while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED ) );
}

void I2C_TxByte(u8 data) {
    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_TXE) == RESET) ;
    I2C_SendData( I2C1, data );
    while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED ) );
}

u8 I2C_RxByte(void) {
	while( I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE) == RESET );
    u8 ret = I2C_ReceiveData( I2C1 );
	while( !I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED ) );
	return ret;
}

const u8 SI5351_ADDRESS = 0b1100000;

u8 Si5351_ReadRegister(u8 reg) {
	/* 
		A read operation is performed in two stages. A data write is used to set the register address, then a data read is
		performed to retrieve the data from the set address. A read burst operation is also supported.
	*/
    u8 ret = 0;
	
	I2C_StartTx(SI5351_ADDRESS);
	I2C_TxByte(reg);
    I2C_GenerateSTOP( I2C1, ENABLE );

	/* Step 2. Perform a read, to retrieve the data */

	I2C_StartRx(SI5351_ADDRESS);
    ret = I2C_RxByte();
    I2C_GenerateSTOP( I2C1, ENABLE );

	return ret;
}

void Si5351_WriteRegister(u8 reg, u8 data) {
	/* 
		Data is transferred MSB first in 8-bit words as specified by the I 2C specification. A write command consists of a 7-
		bit device (slave) address + a write bit, an 8-bit register address, and 8 bits of data
	*/
    I2C_StartTx(SI5351_ADDRESS);
	I2C_TxByte(reg);
	I2C_TxByte(data);
    I2C_GenerateSTOP( I2C1, ENABLE );
}


int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	SystemCoreClockUpdate();
	Delay_Init();

	LED_Init();

#if 1
	SYnthesizer_Init(100000, 0x77);
	/* Si5351 Step 1: Disable Outputs */
	Si5351_WriteRegister(3, 0xFF);
	
	/* Powerdown all output drivers: Reg. 16, 17, 18, 19, 20, 21, 22, 23 = 0x80 */
	Si5351_WriteRegister(16, 0x80);
	Si5351_WriteRegister(17, 0x80);
	Si5351_WriteRegister(18, 0x80);
	Si5351_WriteRegister(19, 0x80);
	Si5351_WriteRegister(20, 0x80);
	Si5351_WriteRegister(21, 0x80);
	Si5351_WriteRegister(22, 0x80);
	Si5351_WriteRegister(23, 0x80);

	/* Set Interrupt Masks Reg 2:  Unused on The Si5351A */

	/* Set Crystal Load Capacitance */
	Si5351_WriteRegister(183, 0xC0); // 10pF

	/* See AN619 about setting up PLL's. 
		Quadrature requires an even multiple of the output frequency to be the PLL frequency
		The PLL frequency must be between 600 and 900 MHz 
		
		if f = 50 MHz, then PLL = 800 MHz (x16), and the output frequency is 50 MHz.
		The delay register must therefore be set to 16, to get the quadrature output on the second clock.
	*/
#endif
	uint8_t ledState = 0;
	while (1)
	{
		GPIO_WriteBit(BLINKY_GPIO_PORT, BLINKY_GPIO_PIN, ledState);
		ledState ^= 1; // invert for the next run
		Delay_Ms(1000);
	}
}

void NMI_Handler(void) {}
void HardFault_Handler(void)
{
	while (1)
	{
	}
}
