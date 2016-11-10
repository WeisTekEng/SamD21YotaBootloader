/*
 * main.c
 *Project:			miniSam USART bootloader.
 *Author:			Weistek Engineering (jeremy G.)
 *website:			www.weistekengineering.com
 *date:				06-29-2016
 *Summery:			Modified version of the Samd10 bootloader.
 *					If PA15 bootpin is held low, micro will enter USART bootloader mode.
 *					if PA15 is high, micro runs user program if there is one at new start
 *					memory. Look at APP_START for start location of user flash.
 *
 *License:	        GNU GENERAL PUBLIC LICENSE Version 3 or newer. The header of this file may not change.
 *
 *					unless new features are added then the update section may be updated. The License file in 
 *					the root of this repo Trunk/LICENSE should have been provided to you. If it was not you may 
 *					find a copy of the GNU Open source license at https://www.gnu.org/licenses/gpl.html
 *
 *Modified:			07/04/2016
 *
 *Important pins : 	UART pins [PA25 PAD3 -> TXd, PA24 PAD2 -> RXd]
 *					Boot En Pin PA15: enabled boot on reset when DTR pin LOW. Change to PA27?
 *					USART reset pin -> RTS -> RST PIN#. Used to reset the micro when
 *					Serial is plugged in, pulse RTS LOW. Almost arduino esqe.
 *
 *Update:			fixed write_nvm function, would fall to dummy handler.
 *
 *Todo:				clean up code, reduce bootloader size. fix a few things (noted in head).
 *					clean up and finish the python front end for loading new programs.
 *
 *Update:			7/04/2016, Thanks to Philip of Oshchip I was able to get this bootloader working.
 *					Bootloader now starts the new app at 0x800, any program built will need there starting
 *					address changed from 0x000 to 0x800. Use the included samd09d14a_flash.ld file for this.
 *					the modification is already present in that file.
 *
 *update:			07/04/2016 Made some progress with the verification loop. it now addresses the proper nvm space.
 *					I think its putting garbage on the uart line in the last bit. the python script says the device is 
 *					not responding, it is its just not sending an s like the python script wants instead its sending the 
 *					same omega symbol..
 *
 *update:			07/05/2016 fixed the verification issue, made the for loop smaller as well. learned a lot about pointers.
 *					as of now this is the final version of the boot loader V1.2, until I find a need to upgrade it
 *					to work with another IDE such as the arduino IDE. <- may be sometime soon. 
 *
 *update:			07/06/2016 cleaned up code, added special talk for new front end.
 *
 *update:			07/11/2016 code cleanup, minor revision change now 1.4.
 *					Added the ability to probe the DSU DID for chip information, could prolly use some more playing with.
 *					should allow bootloader to auto detect the device (dev board) it resides on.
 *					set pullup on pin 15 per AL1's suggestion. Seems to work the same.
 *					currently bootloader size is 1408 bytes-> ends at address 0x580 with VERSION & NEW_FRONT_END defines commented
 *					out.
 *
 *updated:			07/12/2016 Added send_string function. Versioning information can now be stored as an array of char's like it was 
 *					before but in a more readable format. working on board identification methods that don't bloat the bootloader.
 *					by default versioning is off. uncomment the defines in the defines.h file. Added minor.minor to the bootloader
 *					version number current rev is 1.4.1, additional code cleanup, forgot to remove string.h <- not needed. ifdefed out
 *					stuff used to decode the DID register. <- not used atm.
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
}

int main(void)
{  
	
	PORT->Group[BOOT_PORT].OUTSET.reg = (1<<BOOT_PIN);  // set resistor to pull up
	PORT->Group[BOOT_PORT].PINCFG[BOOT_PIN].reg = PORT_PINCFG_INEN | PORT_PINCFG_PULLEN;
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
	/* Flash page size is 64 bytes */
	uint16_t PAGE_SIZE = (8 << NVMCTRL->PARAM.bit.PSZ);	//used to read and write to flash.
	uint8_t page_buffer[PAGE_SIZE];
	
	/*get device information*/

	/* Config Usart */
	UART_sercom_init();

	
	/*set PA14 LED to output and turn on, now we know we are in bootloader mode.*/
	REG_PORT_DIR0 |= (1 << 14); //boot en led set as output.
	REG_PORT_OUT0 |= (1 << 14); //Turn boot en led on.
	
	/* Make CPU to run at 8MHz by clearing prescalar bits */ 
    SYSCTRL->OSC8M.bit.PRESC = 0;
	NVMCTRL->CTRLB.bit.CACHEDIS = 1;

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
				send_string(aVER_Array,i);
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
