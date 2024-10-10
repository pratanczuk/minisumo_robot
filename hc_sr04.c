
#include "hc_sr04.h"
#include "trace.h"


unsigned long pulseIn( unsigned long timeout)
{
        unsigned long width = 0; // keep initialization out of time critical area
        
        unsigned long numloops = 0;
        unsigned long maxloops = microsecondsToClockCycles(timeout) / 16;

        // wait for any previous pulse to end
        while ( (hc_sr04_Echo_port & ( 1 << hc_sr04_Echo_pin)) != 0)
                if (numloops++ == maxloops)
                        return 0;


        // wait for the pulse to start
        while ( (hc_sr04_Echo_port & ( 1 << hc_sr04_Echo_pin)) == 0)
                if (numloops++ == maxloops)
                        return 0;
        
        // wait for the pulse to stop
        while ( (hc_sr04_Echo_port & ( 1 << hc_sr04_Echo_pin)) != 0)
                width++;

        return clockCyclesToMicroseconds(width * 10 + 16); 

}


void hc_sr04Init()
{
	//setup trigger pin as an output
   hc_sr04_Trig_dir_reg |= ( 1<<hc_sr04_Trig_pin);
	//setup echo pin as an input
   hc_sr04_Echo_dir_reg &=~ ( 1<<hc_sr04_Echo_pin);
}

unsigned long hc_sr04Range()
{
    hc_sr04_Trig_port &= ~(1<<hc_sr04_Trig_pin); 
    _delay_us( 2);
    hc_sr04_Trig_port |= (1<<hc_sr04_Trig_pin); 
    _delay_us( 10);
    hc_sr04_Trig_port &= ~(1<<hc_sr04_Trig_pin);

	return  (pulseIn( 100000) / 40 / 2);
}

