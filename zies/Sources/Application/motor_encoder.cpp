#include "motor_encoder.hpp"
#include <math.h>

extern "C"
{
}  // end extern "C"

// =================================================================
// global vars
MotorEncoder_Simulated gMotor1Encoder;
MotorEncoder_Simulated gMotor2Encoder;
MotorEncoder_Simulated gMotor3Encoder;
MotorEncoder_Simulated gMotor4Encoder;

// =================================================================
void MotorEncoder::SetConversionFactors (const MotorRegisters & theRegs)
{
	EncoderTicksPerMotorStep = fabs ((float) theRegs.EncoderTicksPerRev) / (float) (theRegs.MotorFullStepsPerRev * theRegs.StepSize);
	MotorStepsPerEncoderTick = 1.0 / EncoderTicksPerMotorStep;  // precompute, * is faster than /
	UserDirectionReversed = theRegs.EncoderTicksPerRev < 0;

	UserToEncoder = fabs ((float) theRegs.EncoderTicksPerRev) / (float) theRegs.UnitsPerRev;
	EncoderToUser = 1.0 / UserToEncoder;  // pre-compute, * is faster than /
	Modulus = (int32_t) ((float) (theRegs.UnitsPerRev * theRegs.GearheadRatio) * UserToEncoder);
}
// =================================================================
int32_t MotorEncoder_Simulated::CurrentPosition ()
{
	if (UserDirectionReversed)
		return -Counter;
	return Counter;
}
// =================================================================
int32_t MotorEncoder_Simulated::NormalizePosition ()
{
	Counter %= Modulus;
	if (Counter < 0)
		Counter += (Modulus * ((static_cast<uint32_t>(Counter * -1) / Modulus) + 1u));
	return Counter;
}
