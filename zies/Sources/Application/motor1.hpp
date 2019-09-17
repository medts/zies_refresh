#ifndef MOTOR1_HPP_
#define MOTOR1_HPP_
#include "motor_sm.hpp"
// =================================================================
class MotorSM_Motor1 : public MotorSM
{
	public:
		MotorSM_Motor1 (MotorDrv8825 & theMotor, MotorRegisters & theRegs, BooleanFunc theIsHomeFunc,
				BooleanFunc theIsMaxTravelFunc, Time theTimeout) :
				MotorSM ("Motor1", 1, theMotor, theRegs, theIsHomeFunc, theIsMaxTravelFunc, theTimeout)
		{
		}
		virtual void PowerUp ();
};
// =================================================================
extern MotorSM_Motor1 gSM_Motor1;

#endif /* MOTOR1_HPP_ */
