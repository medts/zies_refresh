#include "motor1.hpp"
#include "motor_encoder.hpp"

extern "C"
{
#include "M1_HOME.h"
#include "M1_DIR.h"
#include "M1_nFAULT.h"
#include "M1_ENABLE.h"
#include "pwm_drv.h"
} // end extern "C"

// =================================================================
bool Motor1_IsHome ()
{
	return M1_HOME_GetVal(0);	// true = sensor is active
}
void Motor1_SetDirection ( bool theDirection )
{
	if (theDirection == Forward)
		M1_DIR_PutVal(NULL, 1);
	else
		M1_DIR_PutVal(NULL, 0);
}

void Motor1_Enable ( bool theEnable )
{
	if (theEnable == EnableMotor)
		M1_ENABLE_PutVal(NULL, 0);
	else
		M1_ENABLE_PutVal(NULL, 1);
}

void Motor1_Stop (void)
{
	FTM0_StopPWM();
}

bool Motor1_IsFault (void)
{
	return !M1_nFAULT_GetVal(0); //If low M1 is at fault
}

bool Motor1_IsSteppingDone(void)
{
	return FTM0_IsSteppingDone();
}

MotorDrv8825 Motor1("XAxis", static_cast<uint8_t>(X_AXIS_MOTOR),
		FTM0_SendSteps, Motor1_SetDirection, Motor1_Enable, Motor1_Stop, Motor1_IsFault, Motor1_IsSteppingDone, gMotor1Encoder);
MotorSM_Motor1 gSM_Motor1(Motor1, (MotorRegisters&) gRegisters[Motor1Regs], Motor1_IsHome, 0, 10000.0);

// =================================================================
// Setup default values for the motor, these can be altered by the host through registers write
void MotorSM_Motor1::PowerUp ()
{
	HostRegs.HomeDirection = static_cast<uint32_t>(Reverse);
	HostRegs.InvertedDirection = 0;
	HostRegs.MotorFullStepsPerRev = 200;
	HostRegs.EncoderTicksPerRev = 20;
	HostRegs.UnitsPerRev = 20; // pulley diameter in mm.
	HostRegs.GearheadRatio = 1;
	HostRegs.StepSize = static_cast<uint32_t>(MicroSteps_32);
	HostRegs.Deadband = 2; // Basically pitch of pulley teethes in mm, this is the band which cannot be achieved
	HostRegs.MaxMoveRetries = 3;
	HostRegs.Acceleration = 100; // in mm/sec2
	HostRegs.DeAcceleration = 100; // in mm/sec2
	HostRegs.MinSpeed = 20; // in mm/sec
	HostRegs.MaxSpeed = 1000; // in mm/sec
}
// =================================================================
