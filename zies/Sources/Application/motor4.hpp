#ifndef MOTOR4_HPP_
#define MOTOR4_HPP_
#include "motor_sm.hpp"
// =================================================================
class MotorSM_Motor4 : public MotorSM
{
	public:
		MotorSM_Motor4 (MotorDrv8825 & theMotor, MotorRegisters & theRegs, BooleanFunc theIsHomeFunc,
				BooleanFunc theIsMaxTravelFunc, Time theTimeout) :
				MotorSM ("Motor4", 4, theMotor, theRegs, theIsHomeFunc, theIsMaxTravelFunc, theTimeout)
		{
		}
		virtual void PowerUp ();
};
// =================================================================
extern MotorSM_Motor4 gSM_Motor4;

#endif /* MOTOR4_HPP_ */
