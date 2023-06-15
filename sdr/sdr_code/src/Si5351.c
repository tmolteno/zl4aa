#include <ch32v30x.h>

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

void Si5351_SetFrequency(float frequency) {
	/* 
		Step 1: Disable Outputs
		Step 2: Set PLLA to desired frequency
		Step 3: Set CLK0 to use PLLA
		Step 4: Enable CLK0
		Step 5: Enable Outputs

		https://www.skyworksinc.com/-/media/Skyworks/SL/documents/public/application-notes/AN619.pdf
		https://github.com/MR-DOS/Si5351-lib/blob/master/src/si5351.c
	*/

	/* Step 1: Disable Outputs */
	Si5351_WriteRegister(3, 0xFF);
	Si5351_WriteRegister(9, 0xFF);

	/* Powerdown all output drivers: Reg. 16, 17, 18, 19, 20, 21, 22, 23 = 0x80 */
	for (int clk=0; clk<=7; clk++) {
		Si5351_WriteRegister(16 + clk, 0x80);
	}
	/* Set Interrupt Masks Reg 2:  Unused on The Si5351A */

	/* Set CLK0 to use PLLA */
	Si5351_WriteRegister(15, 0x00); // Use XTAL as PLL clock source

	/* Write configuration map of all registers */




    /* Set Crystal Load Capacitance */
	Si5351_WriteRegister(183, 0xC0); // 10pF

	/* Soft Reset PLLA and PLLB */
	
	Si5351_WriteRegister(177, 0xAC);
	
	/* Enable CLK0 and CLK1 */
	Si5351_WriteRegister(3, 0b11111100);

}

