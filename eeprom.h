#ifndef __EEPROM__H__
#define __EEPROM__H__
#include<avr/eeprom.h>

typedef enum
{
	ePilotOnly = 0, //pilot only, all sensors disabled
	ePilotLine,
	ePilotRange,
	ePilotRangeLine,
	eMinisumo,
	eLastMode

} E_SYSTEM_MODE;

typedef struct
{
	unsigned char PWM_Max_Value;
	E_SYSTEM_MODE SYSTEM_MODE;

} S_SYSTEM_CONFIG;

unsigned char EEPROM_read(unsigned int uiAddress);
void EEPROM_write(unsigned int uiAddress, unsigned char ucData);
char readSystemConfig( S_SYSTEM_CONFIG * a_pSystemConfig);
char writeSystemConfig( S_SYSTEM_CONFIG * a_pSystemConfig);

#endif //__EEPROM__H__
