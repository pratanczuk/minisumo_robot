
/*
History:
20.03.2011:P.Ratanczuk:First release after integration
21.03.2011:P.Ratanczuk:Servo support added, support for engine 5 and 6 added
*/

/*
CPU Ports Mapping for Atmega 32 and Atmega16

Port A
	PA0	(connected) Distance sensor Rear ( hc_sr04_Triger)
	PA1 (connected) Distance sensor Righr
	PA2 (connected) Distance sensor Left
	PA3 (connected) Distance sensor Front ( hc_sr04_Echo)
	PA4 (connected) Line sensor 4
	PA5 (connected) Line sensor 2
	PA6 (connected) Line sensor 3
	PA7 (connected) Line sensor 1

Port B
	PB0 (connected) Engine 1a 
	PB1 (connected) Engine 1b
	PB2 (not connected) INT2
	PB3 (not connected) PWM
	PB4 (not connected)
	PB5 (connected) MOSI (SPI programmer)
	PB6 (connected) MISO (SPI programmer)
	PB7 (connected) SCK (SPI programmer)

Port C
	PC0 (not connected) I2C SCL 
	PC1 (not connected) I2C SDA
	PC2 (connected) PS2 Pad (ack)
	PC3 (connected) Engine 2b
	PC4 (connected) PS2 Pad (command)
	PC5 (connected) PS2 Pad (data)
	PC6 (connected) PS2 Pad (Clock)
	PC7 (connected) PS2 Pad (attention)

Port D
	PD0 (connected) RS232 RX 
	PD1 (connected) RS232 TX
	PD2 (not connected) INT0
	PD3 (connected) INT1 ( line sensors)
	PD4 (connected) Engine 1 
	PD5 (connected) Engine 2
	PD6 (connected) Engine 2a
	PD7 (not connected) PWM
*/


#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>



#include "trace.h"
#include "contract.h"
#include "usart.h"
#include "psx_pad.h"
#include "engines.h"
#include "hc_sr04.h"
#include "eeprom.h"
#include "fsm.h"

#define ROBOT_FORWARD( Value) engine_1_Forward( Value); engine_2_Forward( Value);
#define ROBOT_BACKWARD( Value) engine_1_Backward( Value); engine_2_Backward( Value);
#define ROBOT_BREAK engine_1_Brake(); engine_2_Brake();


#define ROBOT_RIGHT( Value) engine_1_Backward( Value); engine_2_Forward( Value);
#define ROBOT_LEFT( Value) engine_2_Backward( Value); engine_1_Forward( Value);


#define RESCHEDULE 	\
	if( sFSM.eState == eSurviveState) \
	{ \
		break; \
	} \
	ui32hc_sr04Range = hc_sr04Range(); \
	if( ( 0 != ui32hc_sr04Range) && ( 50 > ui32hc_sr04Range )) \
	{\
		sFSM.eState = eAttackState;\
		/*TRACE_RAW( "eFindState->eAttackState\r\n");*/\
		break;\
	}
	
#define SLEEP_MS_AND_RESCHEDULE( Value)\
        for( i=0; i < Value; i++)\
		{\
			_delay_us( 255); \
			RESCHEDULE;\
			_delay_us( 255); \
			RESCHEDULE;\
			_delay_us( 255); \
			RESCHEDULE;\
			_delay_us( 255); \
			RESCHEDULE;\
		}

S_FSM_CONFIG sFSM;
S_SYSTEM_CONFIG sSystemConfig;

SIGNAL(SIG_INTERRUPT1) 
{
	//TRACE_RAW( "INT 1\n\r");
	ROBOT_BREAK;
	ROBOT_BACKWARD(sSystemConfig.PWM_Max_Value);
	sFSM.eState = eSurviveState;
}

int main ( void)
{
	unsigned int  ui32hc_sr04Range = 0;
	int i;
	//init motors as first to secure driver
	enginesInit();

	EEPROM_read(0);

    // init HW uart
    usart_init( 57600);

    // init trace subsystem
    traceInit();

    // Activate interrupts
	// MCUCR Bits 3,2,1,0 are the ones which are of importance to us.
	// MCUCR Bits 3,2 are used for sensing an external interrupt on line INT1.
	// MCUCR Bits 1,0 are used for sensing an external interrupt on line INT0.
	// see for help http://ankiit.blogspot.com/2009/04/exernal-interrupts-on-atmega.html

    GICR  = ((1<<INT1)|(0<<INT0));                         // Activate INT1
	MCUCR = ((1<<ISC11)|(0<<ISC10)); // Set external interrupt on falling edge
	//MCUCR = ((1<<ISC11)|(1<<ISC10)|(1<<ISC01)|(1<<ISC00)); // Set external interrupt on rising edge
	PORTD &= 0b11111011; // disable pull-up on port d, pin PD2


    DDRD &= ~(1<<PD3); // int1 as input	

	// init line sensors
	DDRA  = 0b00000001; 		// 0x00 set port A as input
	PORTA = 0b00000000; //0xff pull-up port A to the VCC, 0x00 disable pull-up port A

	sei();

	// init ultrasonic range sensor
	hc_sr04Init();
	TRACE( "Ultrasonic range sensor Init done!");

    // initialize PSX PAD
    initPSX_PAD();
    TRACE( "PAD  Init done!");

    TRACE( "System Ready!");

    //init FSM
	sFSM.eState = eIdleState;

	//restore configuration
	readSystemConfig( &sSystemConfig);

	// set default values if eeprom was clean, or out of range
	if( eLastMode <= sSystemConfig.SYSTEM_MODE) 
	{
	    sSystemConfig.SYSTEM_MODE = ePilotOnly;
		sSystemConfig.PWM_Max_Value = 15;
		GICR  = ((0<<INT1)|(0<<INT0)); // disalbe line interrupts
		writeSystemConfig( &sSystemConfig);
    }
	else
	{
		if( ePilotRange == sSystemConfig.SYSTEM_MODE || ePilotOnly == sSystemConfig.SYSTEM_MODE)
		{
			GICR  = ((0<<INT1)|(0<<INT0)); // disalbe line interrupts
		}
	}

	TRACE_RAW("System config has been restored\r\n");
	TRACE_RAW( "PWM_Max_Value: %i\n\r", sSystemConfig.PWM_Max_Value);
	TRACE_RAW( "SYSTEM_MODE: %i\n\r", sSystemConfig.SYSTEM_MODE);

	
	while (1)
    {

		/*FSM begin
		eIdleState = 0, // #Entry: After Power On; #Exit: Press Start button
		eStartupState, // #Entry: Press Start button; #Exit: 5 seconds timer expires  
		eSurviveState, // #Entry: Line has been detected #Exit: Rescue procedure finished
		eAttackState, // #Entry: Opponent has been detected; #Exit Line has been detected
		eFindState	// #Entry 5 seconds timer expires; Rescue procedure finished; #Exit Line has been detected; Opponent has been detected
        */
		switch( sFSM.eState)
        {
			
			case eIdleState:
			{
				sFSM.eState = eStartupState;
				TRACE_RAW( "eIdleState->eStartupState\r\n");
				break;
			}
			case eStartupState:
			{
				// sleep 5 seconds
				for( i=0;i<20;i++) 
				{
					_delay_ms(250);
					if( eMinisumo == sSystemConfig.SYSTEM_MODE) 
					{
						if( PS2_Square == testPSX_PAD()) 
						{
							    sSystemConfig.SYSTEM_MODE = ePilotOnly;;
								TRACE_RAW( "Mode ePilotOnly\r\n");
								GICR  = ((0<<INT1)|(0<<INT0)); // disable line sensor interrupt
						}
					}
				
				}

				sFSM.eState = eFindState;
				TRACE_RAW( "eStartupState->eFindState\r\n");
				break;
			}
			case eSurviveState:
			{
				//GICR  = ((0<<INT1)|(0<<INT0)); // disable line sensor interrupt
				ROBOT_BACKWARD( sSystemConfig.PWM_Max_Value);
				_delay_ms(250);	_delay_ms(250); 
				_delay_ms(250); _delay_ms(250); 
				//_delay_ms(250); _delay_ms(250); 
				//_delay_ms(250); _delay_ms(250); 
				
				// check which line sensor reach line - turn left or right
				if( ( PINA & ( 1 << ( PA4))))
				{
				    ROBOT_RIGHT( sSystemConfig.PWM_Max_Value);
				}
				else
				{				   
				    ROBOT_LEFT( sSystemConfig.PWM_Max_Value);			
				}
				
				_delay_ms(250); _delay_ms(250); 
				_delay_ms(250); _delay_ms(250); 
				//_delay_ms(250); _delay_ms(250); 
				//_delay_ms(250); _delay_ms(250); 

				ROBOT_FORWARD( sSystemConfig.PWM_Max_Value);;

				sFSM.eState = eFindState;
				//GICR  = ((1<<INT1)|(0<<INT0)); // enable line sensor interrupt 
				TRACE_RAW( "eSurviveState->eFindState\r\n");
				break;
			}
			case eAttackState:
			{
				ROBOT_FORWARD( 200);
				break;
			}
			case eFindState:
			{
				if( ePilotOnly != sSystemConfig.SYSTEM_MODE && ePilotLine != sSystemConfig.SYSTEM_MODE)
				{
					RESCHEDULE;
					// calculate 45% of power for one wheel sSystemConfig.PWM_Max_Value * 45 /100
					ROBOT_FORWARD( sSystemConfig.PWM_Max_Value);
					engine_1_Forward( ( sSystemConfig.PWM_Max_Value * 45 /100));
					
					SLEEP_MS_AND_RESCHEDULE( 750);


					ROBOT_FORWARD( sSystemConfig.PWM_Max_Value);
					engine_2_Forward( ( sSystemConfig.PWM_Max_Value * 45 /100));
					SLEEP_MS_AND_RESCHEDULE( 900);

					ROBOT_FORWARD( sSystemConfig.PWM_Max_Value);
					SLEEP_MS_AND_RESCHEDULE( 1500);
					

				}
				break;
			}
			default:
			{	
				ILLEGAL_DEFAULT_CASE( sFSM.eState);
				break;
			}
		}
	   //FSM end

		// disable pilot in minisumo mode	
		if( eMinisumo != sSystemConfig.SYSTEM_MODE) 
		{
        	_delay_ms(10);

        	switch( testPSX_PAD())
        	{
				
				case PS2_Square:
				{
				    sSystemConfig.SYSTEM_MODE = ePilotOnly;
					TRACE_RAW( "Mode ePilotOnly\r\n");

					GICR  = ((0<<INT1)|(0<<INT0)); // disable line sensor interrupt 

					break;
				}
				case PS2_Circle:
				{
				    sSystemConfig.SYSTEM_MODE = ePilotLine;
					TRACE_RAW( "Mode ePilotLine\r\n");

					GICR  = ((1<<INT1)|(0<<INT0)); // enable line sensor interrupt 
					break;
				}
				case PS2_Triangle:
				{
				    sSystemConfig.SYSTEM_MODE = ePilotRange;
					TRACE_RAW( "Mode ePilotRange\r\n");

					GICR  = ((0<<INT1)|(0<<INT0)); // disable line sensor interrupt 
					break;
				}
				case PS2_X:
				{
				    sSystemConfig.SYSTEM_MODE = ePilotRangeLine;
					TRACE_RAW( "Mode ePilotRangeLine\r\n");

					GICR  = ((1<<INT1)|(0<<INT0)); // enable line sensor interrupt
					break;
				}
				case PS2_Select:
				{
				    sSystemConfig.SYSTEM_MODE = eMinisumo;
					// write config because pilot will be disconnected
					writeSystemConfig( &sSystemConfig);
					TRACE_RAW( "Mode eMinisumo\r\n");

					GICR  = ((1<<INT1)|(0<<INT0)); // enable line sensor interrupt
					break;
				}
				case PS2_Start:
				{
				    writeSystemConfig( &sSystemConfig);
					TRACE_RAW("System config has been stored\r\n");
					break;
				}				
				// right joystick 
            	case PS2_AXIS2U:
            	{
					ROBOT_FORWARD( sSystemConfig.PWM_Max_Value);
                	break;
            	}
            	case PS2_AXIS2D:
            	{
					ROBOT_BACKWARD( sSystemConfig.PWM_Max_Value);  
          	      	break;
            	}
            	case PS2_AXIS2L:
            	{
					ROBOT_LEFT( sSystemConfig.PWM_Max_Value);
                	break;
            	}
            	case PS2_AXIS2R:
            	{  
					ROBOT_RIGHT( sSystemConfig.PWM_Max_Value);
                	break;
            	}
				case PS2_R1:
            	{
					if( sSystemConfig.PWM_Max_Value < 200 ) sSystemConfig.PWM_Max_Value ++;
					TRACE( "Faster: %i", sSystemConfig.PWM_Max_Value);
					break;
            	}
				case PS2_R2:
            	{
					if( sSystemConfig.PWM_Max_Value > 0 ) sSystemConfig.PWM_Max_Value --;
					TRACE( "Slower:%i", sSystemConfig.PWM_Max_Value);
					break;
            	}
            	default:
            	{
              		//Stop engines
					ROBOT_BREAK;
					break;
            	}
    	    }
			
		}
    }

    }  
