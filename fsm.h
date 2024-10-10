#ifndef __FSM__H__
#define __FSM__H__

typedef enum
{
	eIdleState = 0, // #Entry: After Power On; #Exit: Press Start button
	eStartupState, // #Entry: Press Start button; #Exit: 5 seconds timer expires  
	eSurviveState, // #Entry: Line has been detected #Exit: Rescue procedure finished
	eAttackState, // #Entry: Opponent has been detected; #Exit Line has been detected
	eFindState,	// #Entry 5 seconds timer expires; Rescue procedure finished; #Exit Line has been detected; Opponent has been detected
	eLastState
} E_STATE;

typedef struct
{
	E_STATE eState;
} S_FSM_CONFIG;

#endif //__FSM__H__
