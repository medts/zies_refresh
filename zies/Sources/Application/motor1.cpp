#include "motor1.hpp"
extern "C"
{
#include "M1_HOME.h"
} // end extern "C"
// =================================================================
bool Motor1IsBusy ()
{
	return false;
}
bool Motor1IsHome ()
{
	return M1_HOME_GetVal(0);	// true = sensor is active
}

MotorDrv8825 Motor1("XAxis", static_cast<uint8_t>(X_AXIS_MOTOR), Motor1IsBusy);
MotorSM_Motor1 gSM_Motor1(Motor1, (MotorRegisters&) gRegisters[Motor1Regs], Motor1IsHome, 0, 10000.0);

// =================================================================
// Setup default values for the motor, these can be altered by the host through registers write
void MotorSM_Motor1::PowerUp ()
{
	HostRegs.HomeDirection = static_cast<uint32_t>(Reverse);
	HostRegs.InvertedDirection = 0;
	HostRegs.MotorFullStepsPerRev = 200;
	HostRegs.UnitsPerRev = 101600;
	HostRegs.GearheadRatio = 1;
	HostRegs.StepSize = 8;
	HostRegs.Deadband = 250;
	HostRegs.MaxMoveRetries = 3;
	HostRegs.Acceleration = 1000;
	HostRegs.DeAcceleration = 1000;
	HostRegs.MinSpeed = 0;
	HostRegs.MaxSpeed = 1000;
}
// =================================================================

