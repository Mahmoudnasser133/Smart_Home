/*
 * EEPROM_driver.c
 *
 * Created: 3/2/2018 3:14:46 PM
 *  Author: Mohamed Zaghlol
 */ 

#include <avr/io.h>
#include "std_macros.h"
void EEPROM_write(unsigned short address, unsigned char data )
{
	/*set up address register*/
	EEARL=(char)address;
	EEARH=(char)(address>>8);
	/*set up data register*/
	EEDR=data ;
	/*write logical one to EEMWE*/
	SET_BIT(EECR,EEMWE);
	/*start EEPROM write by setting EEWE*/
	SET_BIT(EECR,EEWE);
	/* wait for completion of write operation*/
	while(READ_BIT(EECR,EEWE)==1);
	
}
unsigned char EEPROM_read( unsigned short address )
{
	/*set up address register*/
	EEARL=(char)address;
	EEARH=(char)(address>>8);
	/*start EEPROM read by setting EERE*/
	SET_BIT(EECR,EERE);
	/*return data from data register*/
	return EEDR ;
}
