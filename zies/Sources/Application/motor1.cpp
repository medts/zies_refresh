#include "motor1.hpp"
#include "motor_encoder.hpp"

extern "C"
{
#include "M1_HOME.h"
#include "M1_DIR.h"
#include "M1_nFAULT.h"
//#include "M1_ENB.h"
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
//	if (theEnable == EnableMotor)
//		M1_ENB_PutVal(NULL, 1);
//	else
//		M1_ENB_PutVal(NULL, 0);
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
		FTM0_SendSteps, Motor1_SetDirection, Motor1_Enable, Motor1_IsFault, Motor1_IsSteppingDone, gMotor1Encoder);
MotorSM_Motor1 gSM_Motor1(Motor1, (MotorRegisters&) gRegisters[Motor1Regs], Motor1_IsHome, 0, 10000.0);

// =================================================================
// Setup default values for the motor, these can be altered by the host through registers write
void MotorSM_Motor1::PowerUp ()
{
	HostRegs.HomeDirection = static_cast<uint32_t>(Reverse);
	HostRegs.InvertedDirection = 0;
	HostRegs.MotorFullStepsPerRev = 200;
	HostRegs.EncoderTicksPerRev = 200;
	HostRegs.UnitsPerRev = 100; // Depends on pulley diameter in mm.
	HostRegs.GearheadRatio = 1;
	HostRegs.StepSize = static_cast<uint32_t>(MicroSteps_8);
	HostRegs.Deadband = 2; // Basically pitch of pulley teethes in mm, this is the band which cannot be achieved
	HostRegs.MaxMoveRetries = 3;
	HostRegs.Acceleration = 1000; // in mm/sec2
	HostRegs.DeAcceleration = 1000; // in mm/sec2
	HostRegs.MinSpeed = 0; // in mm/sec
	HostRegs.MaxSpeed = 1000; // in mm/sec

	M1SetMode(static_cast<DrvStepSize>(HostRegs.StepSize));
}
// =================================================================
