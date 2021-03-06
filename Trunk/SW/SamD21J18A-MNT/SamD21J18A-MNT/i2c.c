/*
 *i2c.c
 *Project:			Generic clock library for miniSam-Zero.
 *Author:			Weistek Engineering (jeremy G.)
 *website:			www.weistekengineering.com
 *date:				06-29-2016
 *Created:			7/16/2016 2:14:26 PM
 *Summery:			Generic functions for i2c on SERCOM0 for samd09 not using ASF drivers.
 *					This file is still a work in progress.
 *
 *License:	        GNU GENERAL PUBLIC LICENSE Version 3 or newer. The header of this file may not change.
 *
 *					unless new features are added then the update section may be updated. The License file in
 *					the root of this repo Trunk/LICENSE should have been provided to you. If it was not you may
 *					find a copy of the GNU Open source license at https://www.gnu.org/licenses/gpl.html
 *
 *Basic Mechs:		page = 32 bytes. m = memory(i2c eeprom), MSTR(samd09), S=(start cmd), P=(stop cmd)
 *					[S addr w] ack(m) [8bitsHigh] ack(m) [8bitsLow] ack(m) {8bits,ack(m)}*n P(MSTR)
 *					[S addr r] ack(m) [8bitsHigh] ack(m) [8bitsLow] ack(m) {8bits,ack(MSTR)}*n P(MSTR) 	
 */

#include "includes.h"

#define ZERO 0x00
#define ONE  0x01

volatile uint8_t *twi_buf;
volatile uint8_t twi_msg_size;
volatile uint8_t twi_bus_addr;
volatile uint8_t twi_state;



//void init_twi(Sercom *sercom)
void init_twi()
{	
	/*setup pinmux for i2c, This is now defined in defines.h*/
	//pin_set_peripheral_function(SDA);
	//pin_set_peripheral_function(SCL);
	
	/*reset i2c module*/
	TWI_SERCOM->I2CM.CTRLA.reg |= SERCOM_I2CM_CTRLA_SWRST;
	
	/*enable i2c Master module*/
	TWI_SERCOM->I2CM.CTRLB.reg = SERCOM_I2CM_CTRLB_SMEN;
	while(TWI_SERCOM->I2CM.SYNCBUSY.reg);
	
	/*wait for soft reset to finish*/
	while(TWI_SERCOM->I2CM.SYNCBUSY.bit.SWRST);
	
    // Synchronous arithmetic baud rate
	TWI_SERCOM->I2CM.BAUD.bit.BAUD = SystemCoreClock / ( 2 * TWI_CLOCK) - 1 ;
	//sercom->I2CM.BAUD.reg = SERCOM_I2CM_BAUD_BAUD(160);
	while (TWI_SERCOM->I2CM.SYNCBUSY.reg);
	
	/*setup module*/
	TWI_SERCOM->I2CM.CTRLA.reg = SERCOM_I2CM_CTRLA_ENABLE | /* enable module */
	SERCOM_I2CM_CTRLA_MODE_I2C_MASTER |    /* i2c master mode */
	SERCOM_I2CM_CTRLA_SDAHOLD(3);        /* SDA hold time to 600ns */
	while (TWI_SERCOM->I2CM.SYNCBUSY.bit.ENABLE);
	
	TWI_SERCOM->I2CM.STATUS.reg |= SERCOM_I2CM_STATUS_BUSSTATE(1);
	while(TWI_SERCOM->I2CM.SYNCBUSY.reg);
	uint16_t timeout_counter = 0;

	//*if buss state is unknown set to idle after timeout*/
	while(!(TWI_SERCOM->I2CM.STATUS.reg & SERCOM_I2CM_STATUS_BUSSTATE(1)))
	{
		timeout_counter++;
		if(timeout_counter >= 65535)
		{
			TWI_SERCOM->I2CM.STATUS.reg |= SERCOM_I2CM_STATUS_BUSSTATE(1); /* set to idle state */
		}
	}
	
}

void i2c_endTransmition(Sercom *sercom)
{
	//Send STOP
	TWI_SERCOM->I2CM.CTRLB.reg |= SERCOM_I2CM_CTRLB_CMD(2);
}

uint8_t i2c_read_byte(Sercom *sercom, uint8_t *data,uint8_t address, uint8_t locationH,uint8_t locationL)
{
	uint8_t size = 4;
	
	//address = (address << 0x1ul) | (locationH << 0x1ul) | (locationL << 0x1ul) | 0x1;
	
	//sercom->I2CM.ADDR.reg = (address << 0x1ul) | (locationH << 0x1ul) | locationL | 1;//| 0x0ul; //i2c transfer read = 1
	
	TWI_SERCOM->I2CM.ADDR.reg = (address << 0x1ul) | 0x01;
	
	//while (0==(sercom->I2CM.INTFLAG.reg & SERCOM_I2CM_INTFLAG_SB));
	
	if(TWI_SERCOM->I2CM.STATUS.reg & SERCOM_I2CM_STATUS_RXNACK)
	{
		TWI_SERCOM->I2CM.CTRLB.reg |= SERCOM_I2CM_CTRLB_CMD(3);
		return 0;
	}
	
	TWI_SERCOM->I2CM.CTRLB.reg &= -SERCOM_I2CM_CTRLB_ACKACT;
	
	for(int i = 0;i < size-1;i++)
	{
		data[i] = TWI_SERCOM->I2CM.DATA.reg;
		while (0==(TWI_SERCOM->I2CM.INTFLAG.reg & SERCOM_I2CM_INTFLAG_SB));
	}
	
	if(size)
	{
		TWI_SERCOM->I2CM.CTRLB.reg |= SERCOM_I2CM_CTRLB_ACKACT;
		TWI_SERCOM->I2CM.CTRLB.reg |= SERCOM_I2CM_CTRLB_CMD(3);
		data[size-1] = TWI_SERCOM->I2CM.DATA.reg;
	}
	return *data;
}

void i2c_write_byte(Sercom *sercom, uint8_t data, int size, uint8_t address, uint8_t locationH,uint8_t locationL)
{
	//address = (address << 0x1ul) | (locationH << 0x1ul) | (locationL << 0x1ul) | 0x0;
	
	//sercom->I2CM.ADDR.reg = (address << 0x1ul) | 0; //tell the slave we wish to write
	//sercom->I2CM.ADDR.reg = address;
	
	TWI_SERCOM->I2CM.ADDR.reg = 0x50 << 0x1ul | 0x0ul;
	
	while (0==(TWI_SERCOM->I2CM.INTFLAG.reg & SERCOM_I2CM_INTFLAG_MB))
	
	if(TWI_SERCOM->I2CM.STATUS.reg & SERCOM_I2CM_STATUS_RXNACK)
	{
		TWI_SERCOM->I2CM.CTRLB.reg |= SERCOM_I2CM_CTRLB_CMD(3);
		//return False;
	}
	
	TWI_SERCOM->I2CM.CTRLB.reg &= -SERCOM_I2CM_CTRLB_ACKACT;
	
	
	for(int i = 0;i< size-3;i++)
	{
		TWI_SERCOM->I2CM.DATA.reg = data;
		while(0==(TWI_SERCOM->I2CM.INTFLAG.reg & SERCOM_I2CM_INTFLAG_MB));
	}
	
	if(size)
	{
		TWI_SERCOM->I2CM.CTRLB.reg |= SERCOM_I2CM_CTRLB_ACKACT;
		TWI_SERCOM->I2CM.CTRLB.reg |= SERCOM_I2CM_CTRLB_CMD(3);
		TWI_SERCOM->I2CM.DATA.reg = sizeof(data)-1;
	}
	//return *data; //just send data for now.
}

void enableWire()
{
	// I2C Master and Slave modes share the ENABLE bit function.

	// Enable the I�C master mode
	TWI_SERCOM->I2CM.CTRLA.bit.ENABLE = 1 ;

	while ( TWI_SERCOM->I2CM.SYNCBUSY.bit.ENABLE != 0 )
	{
		// Waiting the enable bit from SYNCBUSY is equal to 0;
	}

	// Setting bus idle mode
	TWI_SERCOM->I2CM.STATUS.bit.BUSSTATE = 1 ;

	while ( TWI_SERCOM->I2CM.SYNCBUSY.bit.SYSOP != 0 )
	{
		// Wait the SYSOP bit from SYNCBUSY coming back to 0
	}
}									

//uint8_t i2c_startTransmissionWire(uint8_t address, SercomWireReadWriteFlag flag)
//{
	//// 7-bits address + 1-bits R/W
	//address = (0x50 << 0x1ul) | flag;
//
	//// Wait idle or owner bus mode
	//while ( !isBusIdleWIRE() && !isBusOwnerWIRE() );
//
	//// Send start and address
	////SERCOM0->I2CM.ADDR.bit.ADDR = address;
	//
	//TWI_SERCOM->I2CM.ADDR.reg = address;//| 0x0ul; //i2c transfer read = 1
//
//
	//// Address Transmitted
	//if ( flag == WIRE_WRITE_FLAG ) // Write mode
	//{
		//while( !TWI_SERCOM->I2CM.INTFLAG.bit.MB )
		//{
			//// Wait transmission complete
		//}
	//}
	//else  // Read mode
	//{
		//while( !TWI_SERCOM->I2CM.INTFLAG.bit.SB )
		//{
			//// If the slave NACKS the address, the MB bit will be set.
			//// In that case, send a stop condition and return False.
			//if (TWI_SERCOM->I2CM.INTFLAG.bit.MB) {
				//TWI_SERCOM->I2CM.CTRLB.bit.CMD = 3; // Stop condition
				//return False;
			//}
			//// Wait transmission complete
		//}
//
		//// Clean the 'Slave on Bus' flag, for further usage.
		////sercom->I2CM.INTFLAG.bit.SB = 0x1ul;
	//}
//
	////ACK received (0: ACK, 1: NACK)
	//if(TWI_SERCOM->I2CM.STATUS.bit.RXNACK)
	//{
		//return False;
	//}
	//else
	//{
		//return True;
	//}
//}
//
//uint8_t isBusIdleWIRE( void )
//{
	//return TWI_SERCOM->I2CM.STATUS.bit.BUSSTATE == WIRE_IDLE_STATE;
//}
//
//uint8_t isBusOwnerWIRE( void )
//{
	//return TWI_SERCOM->I2CM.STATUS.bit.BUSSTATE == WIRE_OWNER_STATE;
//}
//
//void resetWIRE()
//{
  ////I2CM OR I2CS, no matter SWRST is the same bit.
//
  ////Setting the Software bit to 1
  //TWI_SERCOM->I2CM.CTRLA.bit.SWRST = 1;
//
  ////Wait both bits Software Reset from CTRLA and SYNCBUSY are equal to 0
  //while(TWI_SERCOM->I2CM.CTRLA.bit.SWRST || SERCOM0->I2CM.SYNCBUSY.bit.SWRST);
//}