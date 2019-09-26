#ifndef MOTOR_ENCODER_HPP_
#define MOTOR_ENCODER_HPP_

#include "Registers.hpp"
#include <math.h>

// =================================================================
class MotorEncoder
{
	public:
		float UserToEncoder;  // Converts a real-world unit into an Encoder position.
		float EncoderToUser;  // Converts an Encoder ticks into a real-world unit.
		float EncoderTicksPerMotorStep;  // Converts an Encoder ticks into motor steps.
		float MotorStepsPerEncoderTick;  // Converts motor steps into Encoder ticks.
		bool UserDirectionReversed;
		uint32_t Modulus;

		MotorEncoder () :
				UserToEncoder(1),
				EncoderToUser(1),
				EncoderTicksPerMotorStep(1),
				MotorStepsPerEncoderTick(1),
				UserDirectionReversed(false),
				Modulus(0xFFFFFFFu)
		{
		}

		virtual void MarkPositionAsZero () = 0;  // set current physical position as home
		virtual int32_t CurrentPosition () = 0;  // relative to home
		virtual int32_t NormalizePosition () = 0;  // Wrap around
		void SetConversionFactors ( const MotorRegisters & theRegs );

		virtual void MotorMoved ( uint32_t theMotorSteps, MotorDirection theDirection ) // Motor informs encoder of all motor movements (except Run/Stop)
		{
		}

		int32_t MotorStepsTo ( int32_t thePosition ) // Return the number of motor steps required to move from the current position to thePosition
		{
			return lrintf((float) (thePosition - CurrentPosition()) * MotorStepsPerEncoderTick);
		}
};

// =================================================================
class MotorEncoder_Simulated : public MotorEncoder
{
		int32_t Counter;
	public:
		MotorEncoder_Simulated () :
				Counter(0)
		{
		}
		//----------------------------------------------------------------
		void MarkPositionAsZero ()
		{
			Counter = 0;
		}
		//----------------------------------------------------------------
		int32_t CurrentPosition ();
		//----------------------------------------------------------------
		void MotorMoved ( uint32_t theMotorSteps, MotorDirection theDirection )
		{
			Counter += theMotorSteps * (theDirection == Reverse ? -1 : +1);
		}
		int32_t NormalizePosition ();
};
// =================================================================
extern MotorEncoder_Simulated gMotor1Encoder;
extern MotorEncoder_Simulated gMotor2Encoder;
extern MotorEncoder_Simulated gMotor3Encoder;
extern MotorEncoder_Simulated gMotor4Encoder;

#endif /* MOTOR_ENCODER_HPP_ */
