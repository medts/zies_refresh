#include "motor3.hpp"
#include "motor_encoder.hpp"

extern "C"
{
#include "M3_HOME.h"
#include "M3_DIR.h"
#include "M3_nFAULT.h"
#include "M3_ENABLE.h"
#include "pwm_drv.h"
} // end extern "C"

// =================================================================
bool Motor3_IsHome ()
{
	return M3_HOME_GetVal(0);	// true = sensor is active
}
void Motor3_SetDirection ( bool theDirection )
{
	if (theDirection == Forward)
		M3_DIR_PutVal(NULL, 1);
	else
		M3_DIR_PutVal(NULL, 0);
}

void Motor3_Enable ( bool theEnable )
{
	if (theEnable == EnableMotor)
		M3_ENABLE_PutVal(NULL, 0);
	else
		M3_ENABLE_PutVal(NULL, 1);
}

void Motor3_Stop (void)
{
	FTM1_StopPWM();
}

bool Motor3_IsFault (void)
{
	return !M3_nFAULT_GetVal(0); //If low M1 is at fault
}

bool Motor3_IsSteppingDone(void)
{
	return FTM1_IsSteppingDone();
}

MotorDrv8825 Motor3("ZAxis", static_cast<uint8_t>(Z_AXIS_MOTOR),
		FTM1_SendSteps, Motor3_SetDirection, Motor3_Enable, Motor3_Stop, Motor3_IsFault, Motor3_IsSteppingDone, gMotor3Encoder);
MotorSM_Motor3 gSM_Motor3(Motor3, (MotorRegisters&) gRegisters[Motor3Regs], Motor3_IsHome, 0, 10000.0);

// =================================================================
// Setup default values for the motor, these can be altered by the host through registers write
void MotorSM_Motor3::PowerUp ()
{
	HostRegs.HomeDirection = static_cast<uint32_t>(Reverse);
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
	HostRegs.MinSpeed = 10; // in mm/sec
	HostRegs.MaxSpeed = 100; // in mm/sec
}
// =================================================================
