#include "motor4.hpp"
#include "motor_encoder.hpp"

extern "C"
{
#include "M4_HOME.h"
#include "M4_DIR.h"
#include "M4_nFAULT.h"
#include "M4_ENABLE.h"
#include "pwm_drv.h"
} // end extern "C"

// =================================================================
bool Motor4_IsHome ()
{
	return M4_HOME_GetVal(0);	// true = sensor is active
}
void Motor4_SetDirection ( bool theDirection )
{
	if (theDirection == Forward)
		M4_DIR_PutVal(NULL, 1);
	else
		M4_DIR_PutVal(NULL, 0);
}

void Motor4_Enable ( bool theEnable )
{
	if (theEnable == EnableMotor)
		M4_ENABLE_PutVal(NULL, 0);
	else
		M4_ENABLE_PutVal(NULL, 1);
}

void Motor4_Stop (void)
{
	FTM2_StopPWM();
}

bool Motor4_IsFault (void)
{
	return !M4_nFAULT_GetVal(0); //If low M1 is at fault
}

bool Motor4_IsSteppingDone(void)
{
	return FTM2_IsSteppingDone();
}

MotorDrv8825 Motor4("GAxis", static_cast<uint8_t>(G_AXIS_MOTOR),
		FTM2_SendSteps, Motor4_SetDirection, Motor4_Enable, Motor4_Stop, Motor4_IsFault, Motor4_IsSteppingDone, gMotor4Encoder);
MotorSM_Motor4 gSM_Motor4(Motor4, (MotorRegisters&) gRegisters[Motor4Regs], Motor4_IsHome, 0, 10000.0);

// =================================================================
// Setup default values for the motor, these can be altered by the host through registers write
void MotorSM_Motor4::PowerUp ()
{
	HostRegs.HomeDirection = static_cast<uint32_t>(Reverse);
	HostRegs.InvertedDirection = 0;
	HostRegs.MotorFullStepsPerRev = 200;
	HostRegs.EncoderTicksPerRev = 100;
	HostRegs.UnitsPerRev = 100; // Depends on pulley diameter in mm.
	HostRegs.GearheadRatio = 1;
	HostRegs.StepSize = static_cast<uint32_t>(MicroSteps_8);
	HostRegs.Deadband = 2; // Basically pitch of pulley teethes in mm, this is the band which cannot be achieved
	HostRegs.MaxMoveRetries = 3;
	HostRegs.Acceleration = 100; // in mm/sec2
	HostRegs.DeAcceleration = 100; // in mm/sec2
	HostRegs.MinSpeed = 5; // in mm/sec
	HostRegs.MaxSpeed = 10; // in mm/sec
}
// =================================================================
