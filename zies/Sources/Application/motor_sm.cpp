#include "motor_sm.hpp"
#include "debug.hpp"
#include <math.h>
extern "C"
{
}  // end extern "C"

SM_STATE(MotorSM_PowerUp)
SM_STATE(MotorSM_PowerUpDelay)
SM_STATE(MotorSM_Initialize)
SM_STATE(MotorSM_Idle)
SM_STATE(MotorSM_Move)
SM_STATE(MotorSM_FindHome)
SM_STATE(MotorSM_FindMaxTravel)
SM_STATE(MotorSM_MoveRelative)
SM_STATE(MotorSM_Stop)
SM_STATE(MotorSM_HoldEnable)
SM_STATE(MotorSM_ErrorOccured)

#define sm (*(MotorSM *)theSM)

// =================================================================
MotorSM::MotorSM ( const char * theName, int theId, MotorDrv8825 & theMotor, MotorRegisters & theHostRegs,
        BooleanFunc theIsHomeFunc, BooleanFunc theIsMaxTravelFunc, Time theTimeout ) :
        StateMachine(theName, &MotorSM_PowerUp, 0, 0),
		SM_Status(0),
		TimeoutDuration(theTimeout),
		Motor(theMotor),
		Encoder(theMotor.Encoder),
		HostRegs(theHostRegs),
		IsHome(theIsHomeFunc),
		IsMaxTravel(theIsMaxTravelFunc),
		CurrentPosition(0),
		RequestedPosition(0)
{
	StateMachine::HostCommand = &HostRegs.Command;
	StateMachine::HostErrorCode = &SM_Status;
}
// =================================================================
void MotorSM::MarkPositionAsZero ()
{
	CurrentPosition = 0;
	HostRegs.Position = CurrentPosition;
	Encoder.MarkPositionAsZero();
}
// =================================================================
int32_t MotorSM::GetCurrentPosition()
{
	return Encoder.CurrentPosition();
}
// =================================================================
bool MotorSM::StopRequested ( SM_Msg & theOrigMsg )
{
	if (HostRegs.Command == static_cast<int32_t>(MotorCmd_Stop))
	{
		SMM_Stop.Param1 = (int32_t) HostRegs.CommandParam1;
		SMM_Stop.Pending = true;
		HostRegs.Command = 0;  // Ack the request back to the Host
	}
	if (!SMM_Stop.Pending) return false;
	theOrigMsg.Pending = false;
	SM_Status = static_cast<Error>(SM_Busy);
	return true;
}
// =================================================================
// =================================================================
SM_POLL (MotorSM_PowerUp)
{
	sm.PowerUp();
	sm.HostRegs.HomedStatus = static_cast<uint32_t>(NotHomed);
	sm.Timer.Start(Time(1000.0));  // give 1 sec delay beforte starting any motor operations
	return &MotorSM_PowerUpDelay;
}
// =================================================================
SM_POLL (MotorSM_PowerUpDelay)  // wait for power to motor controllers. Prevent UVLO errors.
{
	if (!sm.Timer.IsTimedOut()) return 0;
	sm.Timer.Start(Time(1000.0));  // 1 sec
	return &MotorSM_Initialize;
}
// =================================================================
SM_POLL (MotorSM_Initialize)
{
	sm.Encoder.SetConversionFactors(sm.HostRegs);
	sm.Motor.SetParameters (sm.HostRegs);
	sm.SM_Status = sm.Motor.Initialize(sm.Timer);
	if (sm.SM_Status == static_cast<Error>(SM_Busy)) return 0;
	sm.SMM_Initialize.Pending = false;
	if (sm.SM_Status) return &MotorSM_ErrorOccured;
	sm.MarkPositionAsZero();
	sm.HostRegs.ErrorCode = static_cast<Error>(NoError);
	return &MotorSM_Idle;
}
// =================================================================
SM_POLL (MotorSM_Idle)
{  // Check for a request from the Host
	sm.SM_Status = static_cast<Error>(NoError);
	if (sm.HostRegs.Command != 0)
	{
		sm.SM_Status = static_cast<Error>(SM_Busy);
		switch (sm.HostRegs.Command)
		{
			case MotorCmd_Initialize:
				DebugPrintf("Motor%d initialize request" CRLF, sm.Motor.UserNum);
				sm.SMM_Initialize.Send();
				break;

			case MotorCmd_GotoHome:
				DebugPrintf("Motor%d Home request" CRLF, sm.Motor.UserNum);
				sm.SMM_FindHome.Send();
				break;

			case MotorCmd_MarkPositionAsZero:
				DebugPrintf("Motor%d MarkZero request" CRLF, sm.Motor.UserNum);
				sm.SMM_MarkZero.Send();
				break;

			case MotorCmd_GotoPosition:
				DebugPrintf("Motor%d MoveTo %d request" CRLF, sm.Motor.UserNum, sm.HostRegs.CommandParam1);
				sm.SMM_MoveTo.Send1Param(sm.HostRegs.CommandParam1);
				break;

			case MotorCmd_MoveRelative:
				DebugPrintf("Motor%d MoveTo %d request" CRLF, sm.Motor.UserNum, sm.HostRegs.CommandParam1);
				sm.SMM_MoveRel.Send1Param(sm.HostRegs.CommandParam1);
				break;

			case MotorCmd_Stop:
				DebugPrintf("Motor%d Stop request" CRLF, sm.Motor.UserNum);
				sm.SMM_Stop.Send1Param(sm.HostRegs.CommandParam1);
				break;

			case MotorCmd_EnableHolding:
				DebugPrintf("Motor%d Enable request" CRLF, sm.Motor.UserNum);
				sm.SMM_HoldEnable.Send1Param(sm.HostRegs.CommandParam1);
				break;

			case MotorCmd_FindMaxTravel:
				DebugPrintf("Motor%d FindMaxTravel request" CRLF, sm.Motor.UserNum);
				sm.SMM_FindMaxTravel.Send();
				break;

			default:
				sm.HostRegs.ErrorCode = static_cast<Error>(CommandFailed);
				break;
		}
		sm.HostRegs.Command = 0;  // Ack the request back to the Host
	}

	sm.HostRegs.Position = sm.GetCurrentPosition();
	//----------------------------------------------------------------
	// Check for SM messages
	//----------------------------------------------------------------
	if (sm.SMM_Initialize.Pending)
	{
		sm.SM_Status = static_cast<Error>(SM_Busy);
		sm.HostRegs.ErrorCode = static_cast<Error>(SM_Busy);
		sm.Timer.Start(sm.TimeoutDuration);
		return &MotorSM_Initialize;
	}
	//----------------------------------------------------------------
	if (sm.SMM_MoveTo.Pending)
	{
		if (sm.CurrentPosition != sm.SMM_MoveTo.Param1)
		{
			sm.SM_Status = static_cast<Error>(SM_Busy);
			sm.HostRegs.ErrorCode = static_cast<Error>(SM_Busy);
			sm.Timer.Start(sm.TimeoutDuration);
			return &MotorSM_Move;
		}
		else
		{
			sm.SM_Status = static_cast<Error>(NoError);
			sm.HostRegs.ErrorCode = static_cast<Error>(NoError);
			sm.SMM_MoveTo.Pending = false;  // already in position
		}
	}
	//----------------------------------------------------------------
	if (sm.SMM_FindHome.Pending)
	{
		sm.SM_Status = static_cast<Error>(SM_Busy);
		sm.HostRegs.ErrorCode = static_cast<Error>(SM_Busy);
		sm.Timer.Start(sm.TimeoutDuration);
		sm.HostRegs.HomedStatus = static_cast<uint32_t>(NotHomed);
		return &MotorSM_FindHome;
	}
	//----------------------------------------------------------------
	if (sm.SMM_FindMaxTravel.Pending)
	{
		sm.SM_Status = static_cast<Error>(SM_Busy);
		sm.HostRegs.ErrorCode = static_cast<Error>(SM_Busy);
		sm.Timer.Start(sm.TimeoutDuration);
		return &MotorSM_FindMaxTravel;
	}
	//----------------------------------------------------------------
	if (sm.SMM_MarkZero.Pending)
	{
		sm.MarkPositionAsZero();
		sm.SMM_MarkZero.Pending = false;
		sm.SM_Status = static_cast<Error>(NoError);
		sm.HostRegs.ErrorCode = static_cast<Error>(NoError);
	}
	//----------------------------------------------------------------
	if (sm.SMM_MoveRel.Pending)
	{
			sm.SM_Status = static_cast<Error>(SM_Busy);
			sm.HostRegs.ErrorCode = static_cast<Error>(SM_Busy);
			sm.RequestedPosition = (sm.CurrentPosition + sm.SMM_MoveRel.Param1);
			sm.Timer.Start(sm.TimeoutDuration);
			return &MotorSM_MoveRelative;
	}
	//----------------------------------------------------------------
	if (sm.SMM_Stop.Pending)
	{
		sm.SM_Status = static_cast<Error>(SM_Busy);
		if (sm.HostRegs.ErrorCode == static_cast<Error>(NoError))
		sm.HostRegs.ErrorCode = static_cast<Error>(SM_Busy);
		sm.Timer.Start(Time(3000.0));  // 3 seconds
		return &MotorSM_Stop;
	}
	//----------------------------------------------------------------
	if (sm.SMM_HoldEnable.Pending)
	{
		sm.SM_Status = static_cast<Error>(SM_Busy);
		sm.HostRegs.ErrorCode = static_cast<Error>(SM_Busy);
		sm.Timer.Start(sm.TimeoutDuration);
		return &MotorSM_HoldEnable;
	}
	return 0;
}
// =================================================================
// =================================================================
SM_POLL (MotorSM_FindHome)
{
	if (sm.StopRequested(sm.SMM_FindHome))
	{
		sm.SMM_FindHome.Pending = false;
		sm.Motor.AbortOperation();
		return &MotorSM_Stop;
	}
	sm.SM_Status = sm.Motor.FindHome(sm.IsHome, sm.Timer);
	if (sm.SM_Status == static_cast<Error>(SM_Busy))
		return 0;
	sm.SMM_FindHome.Pending = false;
	if (sm.SM_Status)
		return &MotorSM_ErrorOccured;
	sm.HostRegs.HomedStatus = static_cast<uint32_t>(Homed);
	sm.HostRegs.ErrorCode = static_cast<Error>(NoError);
	sm.MarkPositionAsZero();
	return &MotorSM_Idle;
}
// =================================================================
SM_POLL (MotorSM_FindMaxTravel)
{
	if (sm.StopRequested(sm.SMM_FindMaxTravel))
	{
		sm.SMM_FindMaxTravel.Pending = false;
		sm.Motor.AbortOperation();
		return &MotorSM_Stop;
	}
	sm.SM_Status = sm.Motor.FindMaxTravel(sm.IsMaxTravel, sm.Timer);
	if (sm.SM_Status == static_cast<Error>(SM_Busy)) return 0;
	sm.SMM_FindMaxTravel.Pending = false;
	if (sm.SM_Status) return &MotorSM_ErrorOccured;
	sm.HostRegs.ErrorCode = static_cast<Error>(NoError);
	return &MotorSM_Idle;
}
// =================================================================
SM_POLL (MotorSM_Move)
{
	if (sm.StopRequested(sm.SMM_MoveTo))
	{
		sm.SMM_MoveTo.Pending = false;
		sm.Motor.AbortOperation();
		return &MotorSM_Stop;
	}
	sm.SM_Status = sm.Motor.MoveTo(lrintf (sm.Encoder.UserToEncoder * (float) sm.SMM_MoveTo.Param1), sm.Timer);
	if (sm.SM_Status == static_cast<Error>(SM_Busy))
		return 0;
	sm.HostRegs.Position = sm.GetCurrentPosition();
	if (sm.SM_Status)
	{
		sm.SMM_MoveTo.Pending = false;
		return &MotorSM_ErrorOccured;
	}
	return &MotorSM_Idle;
}
// =================================================================
SM_POLL (MotorSM_MoveRelative)
{
	if (sm.StopRequested(sm.SMM_MoveRel))
	{
		sm.SMM_MoveRel.Pending = false;
		sm.Motor.AbortOperation();
		return &MotorSM_Stop;
	}
	sm.SM_Status = sm.Motor.MoveTo(sm.RequestedPosition, sm.Timer);
	if (sm.SM_Status == static_cast<Error>(SM_Busy)) return 0;
	sm.HostRegs.Position = sm.GetCurrentPosition();
	sm.SMM_MoveRel.Pending = false;
	if (sm.SM_Status) return &MotorSM_ErrorOccured;
	sm.HostRegs.ErrorCode = static_cast<Error>(NoError);
	return &MotorSM_Idle;
}
// =================================================================
SM_POLL (MotorSM_Stop)
{
	sm.SM_Status = sm.Motor.Stop(sm.SMM_Stop.Param1 == 0, sm.Timer);  // stop the motor
	if (sm.SM_Status == static_cast<Error>(SM_Busy)) return 0;
	sm.HostRegs.Position = sm.GetCurrentPosition();
	sm.SMM_Stop.Pending = false;
	if (sm.SM_Status)
	{
		if (sm.HostRegs.ErrorCode == static_cast<Error>(NoError))
		sm.HostRegs.ErrorCode = sm.SM_Status;
		sm.SM_Status = static_cast<Error>(NoError);
		return &MotorSM_Idle;
	}
	return &MotorSM_Idle;
}
// =================================================================
SM_POLL (MotorSM_HoldEnable)
{
	sm.SM_Status = sm.Motor.HoldEnable(sm.SMM_HoldEnable.Param1 != 0, sm.Timer);
	if (sm.SM_Status == static_cast<Error>(SM_Busy)) return 0;

	sm.SMM_HoldEnable.Pending = false;
	if (sm.SM_Status) return &MotorSM_ErrorOccured;
	sm.HostRegs.ErrorCode = static_cast<Error>(NoError);
	return &MotorSM_Idle;
}
// =================================================================
SM_POLL (MotorSM_ErrorOccured)
{
	DebugPrintf("Motor%d error: 0x%X" CRLF, sm.Motor.UserNum, sm.SM_Status);
	sm.HostRegs.ErrorCode = sm.SM_Status;
	if (sm.Motor.MotorIsRunning)
	{
		sm.SM_Status = static_cast<Error>(SM_Busy);
		sm.SMM_Stop.Send1Param(1u);  // Hard stop
	}
	else
		sm.SM_Status = static_cast<Error>(NoError);
	return &MotorSM_Idle;
}
