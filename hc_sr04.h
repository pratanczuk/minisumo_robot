#ifndef __hc_sr04__
#define __hc_sr04__

#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>


#define hc_sr04_Trig_port PORTA
#define hc_sr04_Trig_dir_reg DDRA
#define hc_sr04_Trig_pin PA0

#define hc_sr04_Echo_port PINA
#define hc_sr04_Echo_dir_reg  DDRA
#define hc_sr04_Echo_pin PA3

#define clockCyclesPerMicrosecond() ( F_CPU / 1000000L )
#define clockCyclesToMicroseconds(a) ( (a) / clockCyclesPerMicrosecond() )
#define microsecondsToClockCycles(a) ( (a) * clockCyclesPerMicrosecond() )


unsigned long pulseIn( unsigned long timeout);
void hc_sr04Init();
unsigned long hc_sr04Range();
#endif //#ifndef __hc_sr04__

