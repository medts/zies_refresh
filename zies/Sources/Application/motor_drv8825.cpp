#include "motor_drv8825.hpp"
#include "debug.hpp"
#include "stdlib.h"
#include "math.h"

extern "C"
{
}

#define PI  3.142

// =================================================================
MotorDrv8825::MotorDrv8825 ( const char * theName, uint8_t theUserNum,
		Step_Func theStepFunc, Direction_Func theDirFunc, DrvEnable_Func theEnableFunc,Stop_Func theStopFunc,
		IsFault_Func theIsFaultFunc, IsSteppingDone_Func theIsSteppingDone, MotorEncoder & theEncoder) :
		UserNum(theUserNum),
		UserName(theName),
		StepFunc(theStepFunc),
		DirFunc(theDirFunc),
		DrvEnabFunc(theEnableFunc),
		StopFunc(theStopFunc),
		IsFault(theIsFaultFunc),
		IsSteppingDone(theIsSteppingDone),
		Encoder(theEncoder),
		MotorIsRunning(false),
		Retries(0),
		MinSpeed(0),
		MaxSpeed(100),
		Acceleration(100),
		DeAcceleration(100),
		Deadband(1),
		MaxMoveRetries(3),
		InvertedDirection(false),
		HomeDirection(Forward),
		StateNum(0),
		SubStateNum(0),
		UserToMotor(1),
		MotorToUser(1),
		OrigPosition(false)
{
}
// =================================================================
Error MotorDrv8825::TimeoutCheck ( const TimeoutTimer & theTimer )
{
	if (theTimer.IsTimedOut())
	{
		return UserNum | static_cast<Error>(TimeoutError_MotorDrv8825);
	}
	return static_cast<Error>(SM_Busy);
}
// =================================================================
Error MotorDrv8825::ErrorCheck ( Error theError, const TimeoutTimer & theTimer )
{
	if (theTimer.IsTimedOut())
	{
		Retries = 0;
		StateNum = 0;
		return UserNum | static_cast<Error>(TimeoutError_MotorDrv8825);
	}

	if (IsFault())
	{
		theError = HwError_MotorDriverError;
		Retries = 0;
		StateNum = 0;
		return theError;
	}
	if (theError == static_cast<Error>(SM_Busy))
		return static_cast<Error>(SM_Busy);
	if (theError)
	{
		Retries = 0;
		StateNum = 0;
		return theError;
	}
	StateNum++;
	return static_cast<Error>(SM_Busy);
}

// =================================================================
void MotorDrv8825::SetParameters ( const MotorRegisters & theHostRegs )
{
	StepSize = theHostRegs.StepSize;
	UserToMotor = (float) theHostRegs.GearheadRatio * (float) theHostRegs.MotorFullStepsPerRev * StepSize/ ((float) theHostRegs.UnitsPerRev * PI);
	MotorToUser = 1.0 / UserToMotor;
	Deadband = theHostRegs.Deadband;
	MaxMoveRetries = theHostRegs.MaxMoveRetries;
	InvertedDirection = theHostRegs.InvertedDirection != 0;
	HomeDirection = (MotorDirection) theHostRegs.HomeDirection;
	Acceleration = (uint32_t)((float) theHostRegs.Acceleration * UserToMotor);
	DeAcceleration = (uint32_t)((float) theHostRegs.DeAcceleration * UserToMotor);
	MinSpeed = (uint32_t)((float) theHostRegs.MinSpeed * UserToMotor);
	MaxSpeed = UserToMotorSpeed(theHostRegs.MaxSpeed);
}
// =================================================================
uint32_t MotorDrv8825::UserToMotorSpeed ( uint32_t theUserSpeed )
{
	return static_cast<uint32_t>(static_cast<float>(theUserSpeed) * UserToMotor);
}
// =================================================================
uint32_t MotorDrv8825::UserToMotorCurrent ( uint32_t theUserCurrent )
{
	static const float c = 1.0e-6;
	return static_cast<uint32_t>(static_cast<float>(theUserCurrent) * c);
}
// =================================================================
float MotorDrv8825::MotorToUserSpeed ( uint32_t theMotorSpeed )
{
	return (float) theMotorSpeed * MotorToUser;
}
// =================================================================
Error MotorDrv8825::Initialize ( const TimeoutTimer & theTimer )
{
	Error error = static_cast<Error>(NoError);
	switch (StateNum)
	{
		case 0:
			DelayTimer.Start(100.0);
			break;
		case 1:
			if (DelayTimer.IsTimedOut())
				error = static_cast<Error>(NoError);
			else
				error = static_cast<Error>(SM_Busy);
			break;
		default:  // all states completed
			StateNum = 0;
			return static_cast<Error>(NoError);
	}  // end switch (StateNum)
	return ErrorCheck(error, theTimer);
}

// =================================================================
Error MotorDrv8825::FindHome ( BooleanFunc theIsHomeFunc, const TimeoutTimer & theTimer )
{
	static const Time pollingPeriod(10.0);  // 10 mSec
	Error error = static_cast<Error>(NoError);  // a faulty home sensor will cause a TimeoutError
	switch (StateNum)
	{
		case 0:
			if (theIsHomeFunc())// if Not Homed
			{
				StateNum = 4;
				error = static_cast<Error>(SM_Busy);
				break;
			}
			break;
		case 1:
			error = move(10, OppositeDirection(HomeDirection), theTimer);
			break;
		case 2:
			if(!(theIsHomeFunc()))
			{
				StateNum = 1;
				error = static_cast<Error>(SM_Busy);
				break;
			}
			else
			{
				error = move(1, HomeDirection, theTimer);
			}
			break;
		case 3:
			if(!(theIsHomeFunc()))
			{
				StateNum = 999;
				break;
			}
			else
			{
				StateNum = 2;
				error = static_cast<Error>(SM_Busy);
				break;
			}
			break;
		case 4:
			error = move(450, HomeDirection, theTimer); //num_of_pulses = 450(mm) * 16;
			break;
		case 5:
			if(!(theIsHomeFunc()))
			{
				StopFunc();
				StateNum = 1;							//if Homed
				error = static_cast<Error>(SM_Busy);
				break;
			}
			else
			{
				StateNum = 4;
				error = static_cast<Error>(SM_Busy);	//Not homed
				break;
			}
			break;
		default:  // all states completed
			StateNum = 0;
			return static_cast<Error>(NoError);
	}  // end switch (StateNum)

	return ErrorCheck(error, theTimer);
}
// =================================================================
Error MotorDrv8825::FindMaxTravel ( BooleanFunc theIsMaxTravelFunc, const TimeoutTimer & theTimer )
{
	Error error = static_cast<Error>(NoError);  // a faulty home sensor will cause a TimeoutError
	return ErrorCheck(error, theTimer);
}
// =================================================================
Error MotorDrv8825::MoveTo ( int32_t thePosition, const TimeoutTimer & theTimer )  // encoder position in ticks
{
	static int32_t distance;
	static MotorDirection dir;
	Error error = static_cast<Error>(NoError);  // a faulty home sensor will cause a TimeoutError

	switch(StateNum)
	{
	  case 0:
		distance = thePosition - Encoder.CurrentPosition();
	    dir = (distance < 0) ? Reverse : Forward;
	    move(abs(distance), dir, theTimer);
	    break;

	  case 1:
	    if(!IsSteppingDone())
	    	error = static_cast<Error>(SM_Busy);
	    else
	    	error = static_cast<Error>(NoError);
	    break;

	  case 2:
	    Encoder.MotorMoved(abs(distance), dir);
	    break;

	  default:
		StateNum = 0;
		return static_cast<Error>(NoError);
	}
	return ErrorCheck(error, theTimer);
}
// =================================================================
Error MotorDrv8825::Stop ( bool theSoftStop, const TimeoutTimer & theTimer )
{
	Error error = static_cast<Error>(NoError);  // a faulty home sensor will cause a TimeoutError
	return ErrorCheck(error, theTimer);
}
// =================================================================
Error MotorDrv8825::HoldEnable ( bool theDoEnable, const TimeoutTimer & theTimer )
{
	Error error = static_cast<Error>(NoError);  // a faulty home sensor will cause a TimeoutError
	DrvEnabFunc(theDoEnable);
	return ErrorCheck(error, theTimer);
}
// =================================================================
Error MotorDrv8825::move ( int32_t theDistance, MotorDirection theDirection, const TimeoutTimer & theTimer ) // num motor steps
{
	uint32_t num_of_pulses;
	uint32_t frequency;

	num_of_pulses = static_cast<uint32_t>(static_cast<float>(theDistance) * UserToMotor); //converting mm to pulses
	frequency = MaxSpeed; //converting speed in mm/sec to frequency in pulses / sec

	DrvEnabFunc(true);
	if (InvertedDirection)
		DirFunc(theDirection == Forward ? false : true);
	else
		DirFunc(theDirection == Forward ? true : false);
	StepFunc(num_of_pulses, frequency);
	return NoError;
}
// =================================================================
Error MotorDrv8825::run ( uint32_t theSpeed, MotorDirection theDirection, const TimeoutTimer & theTimer ) // num motor steps
{
	return NoError;
}
// =================================================================
Error MotorDrv8825::CheckStatus ( const TimeoutTimer & theTimer )
{
	return NoError;
}
