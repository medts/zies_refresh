#ifndef MOTOR2_HPP_
#define MOTOR2_HPP_
#include "motor_sm.hpp"
// =================================================================
class MotorSM_Motor2 : public MotorSM
{
	public:
		MotorSM_Motor2 (MotorDrv8825 & theMotor, MotorRegisters & theRegs, BooleanFunc theIsHomeFunc,
				BooleanFunc theIsMaxTravelFunc, Time theTimeout) :
				MotorSM ("Motor2", 2, theMotor, theRegs, theIsHomeFunc, theIsMaxTravelFunc, theTimeout)
		{
		}
		virtual void PowerUp ();
};
// =================================================================
extern MotorSM_Motor2 gSM_Motor2;

#endif /* MOTOR2_HPP_ */
