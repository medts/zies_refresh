#include "motor2.hpp"
extern "C"
{
#include "M2_HOME.h"
#include "M2_DIR.h"
#include "M2_nFAULT.h"
//#include "M2_ENB.h"
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
//	if (theEnable == EnableMotor)
//		M2_ENB_PutVal(NULL, 1);
//	else
//		M2_ENB_PutVal(NULL, 0);
}

bool Motor2_IsFault ()
{
	return !M2_nFAULT_GetVal(0); //If low M2 is at fault
}

bool Motor2_IsSteppingDone()
{
	return FTM3_IsSteppingDone();
}


MotorDrv8825 Motor2("YAxis", static_cast<uint8_t>(X_AXIS_MOTOR),
		FTM3_SendSteps, Motor2_SetDirection, Motor2_Enable, Motor2_IsFault, Motor2_IsSteppingDone, gMotor2Encoder);
MotorSM_Motor2 gSM_Motor2(Motor2, (MotorRegisters&) gRegisters[Motor2Regs], Motor2_IsHome, 0, 10000.0);

// =================================================================
// Setup default values for the motor, these can be altered by the host through registers write
void MotorSM_Motor2::PowerUp ()
{
	HostRegs.HomeDirection = static_cast<uint32_t>(Forward);
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

	M2SetMode(static_cast<DrvStepSize>(HostRegs.StepSize));
}
// =================================================================
