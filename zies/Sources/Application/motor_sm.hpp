#ifndef MOTOR_SM_HPP_
#define MOTOR_SM_HPP_

#include "motor_drv8825.hpp"

// =================================================================
enum MotorUserIndex
{
	X_AXIS_MOTOR = 1, Y_AXIS_MOTOR = 2, Z_AXIS_MOTOR = 3, D_AXIS_MOTOR = 4
};

enum HomedStatus
{
	NotHomed = 0, Homed = 1,
};
// =================================================================
class SMM_MotorMoveTo : public SM_Int32ParamMsg
{
	public:
		inline SMM_MotorMoveTo ()
		{
		}
};
// =================================================================
class SMM_MotorMoveRel : public SM_Int32ParamMsg
{
	public:
		inline SMM_MotorMoveRel ()
		{
		}
};
// =================================================================
class SMM_MotorStop : public SM_Int32ParamMsg
{
	public:
		inline SMM_MotorStop ()
		{
		}
};
// =================================================================
class SMM_MotorHoldEnable : public SM_Int32ParamMsg
{
	public:
		inline SMM_MotorHoldEnable ()
		{
		}
};
// =================================================================
class SMM_MotorFindHome : public SM_Msg
{
	public:
		inline SMM_MotorFindHome ()
		{
		}
};
// =================================================================
class SMM_MotorFindMaxTravel : public SM_Msg
{
	public:
		inline SMM_MotorFindMaxTravel ()
		{
		}
};
// =================================================================
class SMM_MotorInitialize : public SM_Msg
{
	public:
		inline SMM_MotorInitialize ()
		{
		}
};
// =================================================================
class SMM_MotorDumpRegs : public SM_Msg
{
	public:
		inline SMM_MotorDumpRegs ()
		{
		}
};
// =================================================================
class SMM_MotorMarkZero : public SM_Msg
{
	public:
		inline SMM_MotorMarkZero ()
		{
		}
};
// =================================================================
class MotorSM : public StateMachine
{
	public:
		Error SM_Status;
		MotorDrv8825 & Motor;
		MotorEncoder & Encoder;
		MotorRegisters & HostRegs;
		int32_t CurrentPosition;
		const Time TimeoutDuration;
		TimeoutTimer Timer;
		BooleanFunc IsHome;
		BooleanFunc IsMaxTravel;
		int32_t RequestedPosition;

		MotorSM ( const char * theName, int theErrorStatusBitNum, MotorDrv8825 & theMotor,
				MotorRegisters & theHostRegs, BooleanFunc theIsHomeFunc,
				BooleanFunc theIsMaxTravelFunc, Time theTimeout );
		virtual void PowerUp () = 0;

		void MarkPositionAsZero (void);
		int32_t GetCurrentPosition(void);
		bool StopRequested ( SM_Msg & theOrigMsg );

		SMM_MotorMoveTo SMM_MoveTo;
		SMM_MotorMoveRel SMM_MoveRel;
		SMM_MotorStop SMM_Stop;
		SMM_MotorHoldEnable SMM_HoldEnable;
		SMM_MotorFindHome SMM_FindHome;
		SMM_MotorFindMaxTravel SMM_FindMaxTravel;
		SMM_MotorInitialize SMM_Initialize;
		SMM_MotorMarkZero SMM_MarkZero;
};

#endif /* MOTORSM_HPP_ */
