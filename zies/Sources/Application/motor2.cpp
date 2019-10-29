#include "motor2.hpp"
extern "C"
{
#include "M2_HOME.h"
#include "M2_DIR.h"
#include "M2_nFAULT.h"
#include "M1_ENABLE.h"
#include "pwm_drv.h"
} // end extern "C"
// =================================================================
bool Motor2_IsHome ()
{
	return M2_HOME_GetVal(0);	// true = sensor is active
}
void Motor2_SetDirection ( bool theDirection )
{
	if (theDirection == Forward)
		M2_DIR_PutVal(NULL, 1);
	else
		M2_DIR_PutVal(NULL, 0);
}

void Motor2_Enable ( bool theEnable )
{
	if (theEnable == EnableMotor)
		M2_ENABLE_PutVal(NULL, 0);
	else
		M2_ENABLE_PutVal(NULL, 1);
}

void Motor2_Stop (void)
{
	FTM3_StopPWM();
}

bool Motor2_IsFault ()
{
	return !M2_nFAULT_GetVal(0); //If low M2 is at fault
}

bool Motor2_IsSteppingDone()
{
	return FTM3_IsSteppingDone();
}


MotorDrv8825 Motor2("YAxis", static_cast<uint8_t>(Y_AXIS_MOTOR),
		FTM3_SendSteps, Motor2_SetDirection, Motor2_Enable, Motor2_Stop, Motor2_IsFault, Motor2_IsSteppingDone, gMotor2Encoder);
MotorSM_Motor2 gSM_Motor2(Motor2, (MotorRegisters&) gRegisters[Motor2Regs], Motor2_IsHome, 0, 10000.0);

// =================================================================
// Setup default values for the motor, these can be altered by the host through registers write
void MotorSM_Motor2::PowerUp ()
{
	HostRegs.HomeDirection = static_cast<uint32_t>(Forward);
	HostRegs.InvertedDirection = 0;
	HostRegs.MotorFullStepsPerRev = 200;
	HostRegs.EncoderTicksPerRev = 20;
	HostRegs.UnitsPerRev = 20; // Depends on pulley diameter in mm.
	HostRegs.GearheadRatio = 1;
	HostRegs.StepSize = static_cast<uint32_t>(MicroSteps_32);
	HostRegs.Deadband = 2; // Basically pitch of pulley teethes in mm, this is the band which cannot be achieved
	HostRegs.MaxMoveRetries = 3;
	HostRegs.Acceleration = 100; // in mm/sec2
	HostRegs.DeAcceleration = 100; // in mm/sec2
	HostRegs.MinSpeed = 20; // in mm/sec
	HostRegs.MaxSpeed = 100; // in mm/sec
}
// =================================================================
