/*
 *i2c.c
 *Project:			Generic clock library for miniSam-Zero.
 *Author:			Weistek Engineering (jeremy G.)
 *website:			www.weistekengineering.com
 *date:				06-29-2016
 *Created:			7/16/2016 2:14:26 PM
 *Summery:			header file for i2c for samd09 without asf drivers, files are still a work in progress.
 *
 *License:	        GNU GENERAL PUBLIC LICENSE Version 3 or newer. The header of this file may not change.
 *
 *					unless new features are added then the update section may be updated. The License file in
 *					the root of this repo Trunk/LICENSE should have been provided to you. If it was not you may
 *					find a copy of the GNU Open source license at https://www.gnu.org/licenses/gpl.html
 *
 */ 


#ifndef I2C_H_
#define I2C_H_

#define TWI_CLOCK 0x0C  //400khz TWI clock

#define NO_INTERRUPTS  0

#if (NO_INTERRUPTS)

#define TWCR_START  0xA4   //send start condition
#define TWCR_STOP   0x94   //send stop condition
#define TWCR_RACK   0xC4   //receive byte and return ack to slave
#define TWCR_RNACK  0x84   //receive byte and return nack to slave
#define TWCR_SEND   0x84   //pokes the TWINT flag in TWCR and TWEN

#else
#define TWCR_START  0xA5 //send START
#define TWCR_SEND   0x85 //poke TWINT flag to send another byte
#define TWCR_RACK   0xC5 //receive byte and return ACK to slave
#define TWCR_RNACK  0x85 //receive byte and return NACK to slave
#define TWCR_RST    0x04 //reset TWI
#define TWCR_STOP   0x94 //send STOP,interrupt off, signals completion

#endif

#define TWI_BUFFER_SIZE 17  //SLA+RW (1 byte) +  16 data bytes (message size)

uint8_t twi_busy(void);
void    twi_start_wr(uint8_t twi_addr, uint8_t *twi_data, uint8_t byte_cnt);
void    twi_start_rd(uint8_t twi_addr, uint8_t *twi_data, uint8_t byte_cnt);
void    init_twi();

//typedef enum
//{
	//I2C_MASTER_OPERATION = 0x5u
//}SercomI2CMode;
//
//typedef enum
//{
	//WIRE_WRITE_FLAG = 0x0ul,
	//WIRE_READ_FLAG
//} SercomWireReadWriteFlag;
//
//typedef enum
//{
	//WIRE_UNKNOWN_STATE = 0x0ul,
	//WIRE_IDLE_STATE,
	//WIRE_OWNER_STATE,
	//WIRE_BUSY_STATE
//} SercomWireBusState;
//
//// TWI clock frequency
//static const uint32_t TWI_CLOCK = 100000;
//
///*used to setup i2c*/
//void i2c_setup(Sercom *sercom);
//uint8_t i2c_read_byte(Sercom *sercom, uint8_t *data,uint8_t address,uint8_t locationH,uint8_t locationL);
//void i2c_write_byte(Sercom *sercom, uint8_t data, int size,uint8_t address, uint8_t locationH,uint8_t locationL);
//uint8_t i2c_startTransmissionWire(uint8_t address, SercomWireReadWriteFlag flag);
//void i2c_endTransmition(Sercom *sercom);
//uint8_t isBusIdleWIRE( void );
//uint8_t isBusOwnerWIRE( void );


#endif /* I2C_H_ */