#ifndef MOTOR3_HPP_
#define MOTOR3_HPP_
#include "motor_sm.hpp"
// =================================================================
class MotorSM_Motor3 : public MotorSM
{
	public:
		MotorSM_Motor3 (MotorDrv8825 & theMotor, MotorRegisters & theRegs, BooleanFunc theIsHomeFunc,
				BooleanFunc theIsMaxTravelFunc, Time theTimeout) :
				MotorSM ("Motor3", 3, theMotor, theRegs, theIsHomeFunc, theIsMaxTravelFunc, theTimeout)
		{
		}
		virtual void PowerUp ();
};
// =================================================================
extern MotorSM_Motor3 gSM_Motor3;

#endif /* MOTOR3_HPP_ */
