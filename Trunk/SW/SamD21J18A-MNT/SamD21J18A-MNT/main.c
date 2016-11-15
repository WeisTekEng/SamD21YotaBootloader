/*SamD21J18A-MNT.c
 *main.c
 *Project:			SamD21J18A-MNT USART bootloader.
 *Author:			Weistek Engineering (jeremy G.)
 *website:			www.weistekengineering.com
 *date:				Created: 11/9/2016 10:13:45 PM
 *
 *Config:			USART has been configured for Pins 13/14 PA04D PA05D SERCOM0 PAD0 PAD1
 *Config USB:		PA24(USB_D-) PA25(USB_D+) 45/46 
 *Config SPI:		PA00(MOSI) PA01(SCK) PA18(MISO) 1/2/37
 *
 *
 Notes:
 Pin	Function	SERCOM		SERCOM alt
 -----------------------------------------
 PA11				SERCOM0.3	SERCOM2.3
 PA10				SERCOM0.2	SERCOM2.2
 PA14				SERCOM2.2	SERCOM4.2
 PA09				SERCOM0.1	SERCOM2.1
 PA08				SERCOM0.0	SERCOM2.0
 PA15				SERCOM2.3	SERCOM4.3
 PA20				SERCOM5.2	SERCOM3.2
 PA21				SERCOM5.3	SERCOM3.3
 PA06							SERCOM0.2
 PA07							SERCOM0.3
 PA18				SERCOM1.2	SERCOM3.2
 PA16				SERCOM1.0	SERCOM3.0
 PA19				SERCOM1.3	SERCOM3.3
 PA17				SERCOM1.1	SERCOM3.1
 PB08							SERCOM4.0
 PB09							SERCOM4.1
 PA04							SERCOM0.0
 PA05							SERCOM0.1
 PB02							SERCOM5.0
 PA22	SDA			SERCOM3.0	SERCOM5.0
 PA23	SCL			SERCOM3.1	SERCOM5.1
 PA12	MISO		SERCOM2.0	SERCOM4.0
 PB10	MOSI		SERCOM4.2
 PB11	SCK			SERCOM4.3
 */ 

#include "includes.h"

uint8_t data_8 = 1;
uint32_t i, dest_addr;
uint32_t volatile app_start_address;
uint8_t volatile data_from_flash;
uint32_t *flash_ptr;
uint8_t *flash_byte_ptr;

//Version information.
/*this takes up allot of space, if your need space commend out the define in defines.h*/
#ifdef VERSION
char aVER_Array[] = {"miniSam "BOARD_VER"\nBootloader "BL_VER"\nDev Board registered to "REG_TO"\nBoard ID "BOARD_ID"\n"};
#endif

void setup_ptrs()
{
	//set values, for flash pointers.
	dest_addr = APP_START;
	flash_ptr = APP_START;
	app_start_address = *flash_ptr;
	flash_byte_ptr = APP_START;
	sendConfirm();
}

void enable_interrupts()
{
	NVIC_EnableIRQ(SERCOM1_IRQn);
}

int main(void)
{  
	
	PORT->Group[BOOT_PORT].OUTSET.reg = (1<<BOOT_PIN);  // set resistor to pull up
	PORT->Group[BOOT_PORT].PINCFG[BOOT_PIN].reg = PORT_PINCFG_INEN | PORT_PINCFG_PULLEN;
	#if 1 /*enable for testing bootloader*/
		if ((PORT->Group[BOOT_PORT].IN.reg & (1u << BOOT_PIN)))
		{
			/*Get the entry point for our new app*/
			app_start_address = *(uint32_t *)(APP_START + 4);
		
			/* Rebase the Stack Pointer */
			__set_MSP(*(uint32_t *) APP_START + 4);

			/* Rebase the vector table base address */
			SCB->VTOR = ((uint32_t) APP_START & SCB_VTOR_TBLOFF_Msk);

			/* Jump to application Reset Handler in the application */
			asm("bx %0"::"r"(app_start_address));
		}
	#endif /*end*/
	
	/* Flash page size is 64 bytes */
	uint16_t PAGE_SIZE = (8 << NVMCTRL->PARAM.bit.PSZ);	//used to read and write to flash.
	uint8_t page_buffer[PAGE_SIZE];
	
	/*get device information*/
	
	/*initilize Clocks*/
	init_clocks();

	/* Config Usart */
	UART_sercom_init();
	init_twi();

	
	/*set PA14 LED to output and turn on, now we know we are in bootloader mode.*/
	//LED pin on pin 27
	#if 0
	REG_PORT_DIR0 |= (1 << 14); //boot en led set as output.
	REG_PORT_OUT0 |= (1 << 14); //Turn boot en led on.
	#endif
	//Samd21 led pin
	#if 1
	REG_PORT_DIR0 |= (1 << 28);
	REG_PORT_OUT0 |= (1 << 28);
	#endif
	/* Make CPU to run at 8MHz by clearing prescalar bits */ 
    SYSCTRL->OSC8M.bit.PRESC = 0;
	NVMCTRL->CTRLB.bit.CACHEDIS = 1;
	
	//entering bootloader.
	send_string(aVER_Array);

    while (1) 
    {
        data_8 = uart_read_byte();

		if (data_8 == '#')
		{
			#ifdef NEW_FRONT_END
				if(!specialTalk)
				{
					sendConfirm();
				}
				else
				{
					uart_write_byte('!');
				}
			#else
				sendConfirm();
			#endif
			
			uart_write_byte((uint8_t)APP_SIZE);	
		}
		else if (data_8 == 'e')
		{
			/*erase NVM from 0x800 (starting point) to top of NVM*/
			//erase from 0x800 to the top of nvm.
			for(i = APP_START; i < FLASH_SIZE; i = i + 256)
			{
				nvm_erase_row(i,PAGE_SIZE);
			}
			#ifdef NEW_FRONT_END
				if(!specialTalk)
				{
					sendConfirm();
				}
				else
				{
					uart_write_byte('`');
				}
			#else
				sendConfirm();
			#endif
			
		}
		else if (data_8 == 'p')
		{
			#ifdef NEW_FRONT_END
				if(!specialTalk)
				{
					sendConfirm();
				}
			#else
				sendConfirm();
			#endif
			
			for (i = 0; i < PAGE_SIZE; i++)
			{
				page_buffer[i] = uart_read_byte();
			}
			nvm_write_buffer(dest_addr, page_buffer, PAGE_SIZE);
			dest_addr += PAGE_SIZE;
			
			#ifdef NEW_FRONT_END
				if(!specialTalk)
				{
					sendConfirm();
				}
				else
				{
					uart_write_byte('%');
				}
			#else
				sendConfirm();
			#endif
			
			REG_PORT_OUTTGL0 = (1 << 14); //blinks light

		}
		else if (data_8 == 'v')
		{
			#ifdef NEW_FRONT_END
				if(!specialTalk)
				{
					sendConfirm();
				}
			#else
				sendConfirm();
			#endif
			
			for (i = 0; i < (PAGE_SIZE); i++)
			{	
				//++ after pointer post increments by 1
				uart_write_byte(* flash_byte_ptr++);
				
			}
			REG_PORT_OUTTGL0 = (1 << 14); //blinks light
		}
		else if (data_8 == 'm')
		{
			setup_ptrs();
			
		}
		#ifdef VERSION
			else if (data_8 == 'i')
			{
				send_string(aVER_Array);
			}
		#endif
		#ifdef NEW_FRONT_END
			else if(data_8 == '~')
			{
				//special talk.
				specialTalk = 1;
			}
		#endif
		else if(data_8 == '^')
		{
			uint8_t next = 8;
			//Testing device ID over UART.
			for(i = 0;i < 4;i++)
			{
				uart_write_byte(_DID >> next);
				next+=8;
			}
		}
    }
}

