/*
 *usart.c
 *Project:			miniSam USART bootloader.
 *Author:			Weistek Engineering (jeremy G.)
 *website:			www.weistekengineering.com
 *date:				07-11-2016
 *Summery:			setup usart functions, read and write functions.
 *
 *updated:			07/12/2016 Added send_string function.
 */ 

#include "includes.h"

/*send confirmation of command received*/
void sendConfirm()
{
	uart_write_byte('s');
}

/*send a char array and format as a string*/
void send_string(volatile char s[])
{
	uint32_t i;
	while(s[i] != 0x00)
	{
		uart_write_byte(s[i]);
		i++;
	}
}

//write to the usart register.
void uart_write_byte(uint8_t data)
{
	while(!BOOT_SERCOM->USART.INTFLAG.bit.DRE);
	
	#ifdef NEW_FRONT_END
	if(!specialTalk)
	{
		BOOT_SERCOM->USART.DATA.reg = (uint16_t)data;
	}
	else
	{
		if(data == '\n')
		{
			data = ('*');
		}
		
		BOOT_SERCOM->USART.DATA.reg = (uint16_t)data;
	}
	#endif
	BOOT_SERCOM->USART.DATA.reg = (uint16_t)data;
	
}

//read from the usart register.
uint8_t uart_read_byte(void)
{
	while(!BOOT_SERCOM->USART.INTFLAG.bit.RXC);
	return((uint8_t)(BOOT_SERCOM->USART.DATA.reg & 0x00FF));
}

/*init USART module on SERCOM1*/
void UART_sercom_init()
{
	//Pmux eve = n/1, odd = (n-1)/2
	pin_set_peripheral_function(PINMUX_PA10C_SERCOM0_PAD2); //TXD
	pin_set_peripheral_function(PINMUX_PA11C_SERCOM0_PAD3); //RXD

	//Config SERCOM1 module for UART
	BOOT_SERCOM->USART.CTRLA.reg = SERCOM_USART_CTRLA_MODE_USART_INT_CLK | SERCOM_USART_CTRLA_DORD | SERCOM_USART_CTRLA_RXPO(0x3) | SERCOM_USART_CTRLA_TXPO(0x1);
	
	BOOT_SERCOM->USART.CTRLB.reg = SERCOM_USART_CTRLB_RXEN | SERCOM_USART_CTRLB_TXEN | SERCOM_USART_CTRLB_CHSIZE(0);
	
	/*configure baud rate at 115200*/
	BOOT_SERCOM->USART.BAUD.reg = BAUD_VAL;
	
	BOOT_SERCOM->USART.CTRLA.reg |= SERCOM_USART_CTRLA_ENABLE;
	
}

