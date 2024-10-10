
#include "eeprom.h"

unsigned char EEPROM_read(unsigned int uiAddress)
{
	/* Wait for completion of previous write */
	while(EECR & (1<<EEWE));
	/* Set up address register */
	EEAR = uiAddress;
	/* Start eeprom read by writing EERE */
	EECR |= (1<<EERE);
	/* Return data from data register */
	return EEDR;
}

void EEPROM_write(unsigned int uiAddress, unsigned char ucData)
{
	/* Wait for completion of previous write */
	while(EECR & (1<<EEWE));
	/* Set up address and data registers */
	EEAR = uiAddress;
	EEDR = ucData;
	/* Write logical one to EEMWE */
	EECR |= (1<<EEMWE);
	/* Start eeprom write by setting EEWE */
	EECR |= (1<<EEWE);
}

char readSystemConfig( S_SYSTEM_CONFIG * a_pSystemConfig)
{
	unsigned char * pStruct = (unsigned char *)a_pSystemConfig;
	unsigned char ui8Index =0;
	
	if( 0 == pStruct) return -1;
	
	for( ui8Index=0; ui8Index < sizeof( S_SYSTEM_CONFIG); ui8Index++)
	{
		pStruct[ui8Index] = EEPROM_read( ui8Index);
	}
	
	return 0;
}

char writeSystemConfig( S_SYSTEM_CONFIG * a_pSystemConfig)
{
	unsigned char * pStruct = (unsigned char *)a_pSystemConfig;
	unsigned char ui8Index =0;
	
	if( 0 == pStruct) return -1;
	
	for( ui8Index=0; ui8Index < sizeof( S_SYSTEM_CONFIG); ui8Index++)
	{
		EEPROM_write( ui8Index, pStruct[ui8Index]);
	}
	
	return 0;	
}
