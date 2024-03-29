#ifndef MOTOR_DRV8825_HPP_
#define MOTOR_DRV8825_HPP_

//#include "motor_encoder.hpp"
#include "state_machine.hpp"
#include "registers.hpp"
#include "time.hpp"

// =================================================================
typedef float MotorPosition;

inline MotorDirection OppositeDirection ( MotorDirection theDir )
{
	return (theDir == Reverse ? Forward : Reverse);
}

// =================================================================
class MotorDrv8825
{
	public:
		uint8_t UserNum;  // 0..n
		const char * UserName;
		uint32_t MinSpeed;
		uint32_t MaxSpeed;
		uint32_t Acceleration;
		uint32_t DeAcceleration;
		uint8_t StepSize;  // 0=full, 1=half, ... 6=1/64, 7=1/128
		uint32_t Deadband;
		uint32_t MaxMoveRetries;
		bool InvertedDirection;
		BooleanFunc DeviceIsBusy;
//		MotorEncoder & Encoder;
		MotorDirection HomeDirection;
		bool MotorIsRunning;
		TimeoutTimer DelayTimer;

		MotorDrv8825 ( const char * theName, uint8_t theUserNum, BooleanFunc theIsBusyFunc );
		void SetParameters ( const MotorRegisters & theHostRegs );
		Error Initialize ( const TimeoutTimer & theTimer );
		Error MoveTo ( int32_t thePosition, const TimeoutTimer & theTimer );
		Error Stop ( bool theSoftStop, const TimeoutTimer & theTimer );
		Error HoldEnable ( bool theDoEnable, const TimeoutTimer & theTimer );
		Error FindHome ( BooleanFunc theIsHomeFunc, const TimeoutTimer & theTimer );
		Error FindMaxTravel ( BooleanFunc theIsHomeFunc, const TimeoutTimer & theTimer );
		uint32_t UserToMotorSpeed ( uint32_t theUserSpeed );
		float MotorToUserSpeed ( uint32_t theMotorSpeed );
		uint32_t UserToMotorCurrent ( uint32_t theUserCurrent );
		inline void AbortOperation ( void )
		{
			StateNum = 0;
			SubStateNum = 0;
			OrigPosition = 0;
			Retries = 0;
		}

	protected:
		int16_t StateNum;
		int16_t SubStateNum;
		float UserToMotor;
		float MotorToUser;
		int32_t OrigPosition;
		uint32_t Retries;

		Error move (uint32_t theAmount, MotorDirection theDirection, const TimeoutTimer & theTimer);
		Error run (uint32_t theSpeed, MotorDirection theDirection, const TimeoutTimer & theTimer);
		Error CheckStatus (const TimeoutTimer & theTimer);
		Error TimeoutCheck (const TimeoutTimer & theTimer);
		Error ErrorCheck (Error theError, const TimeoutTimer & theTimer);
};
#endif /* MOTOR_DRV8825_HPP_ */
