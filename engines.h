#ifndef __ENGINES__
#define __ENGINES__

#include <stdio.h>
#include <stdlib.h>

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

#include "trace.h"
#include "contract.h"

void engine_1_Forward( unsigned char a_ui8Speed);
void engine_1_Backward( unsigned char a_ui8Speed);
void engine_2_Forward( unsigned char a_ui8Speed);
void engine_2_Backward( unsigned char a_ui8Speed);
void engine_1_Brake( void);
void engine_2_Brake( void);
void enginesInit (void);

#endif /*__ENGINES__*/
