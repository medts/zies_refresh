#ifndef SOURCES_PWM_DRV_H_
#define SOURCES_PWM_DRV_H_

#include "MK22F51212.h"
#include "Cpu.h"
#include "Init_Config.h"
#include "Vectors_Config.h"
#include "CPU_Config.h"

struct FTM_Params
{
		float freq;
		uint32_t pulses;
};

typedef enum DrvStepSize_Type
{
	FullStep = 1,
	HalfStep = 2,
	QuarterStep = 4,
	MicroSteps_8 = 8,
	MicroSteps_16 = 16,
	MicroSteps_32 = 32,
} DrvStepSize;

void FTM0_Init (void);
void FTM0_SendSteps ( uint32_t steps, float freqinHz );
void FTM0_StopPWM (void);
bool FTM0_IsSteppingDone (void);

void FTM3_Init (void);
void FTM3_SendSteps ( uint32_t steps, float freqinHz );
void FTM3_StopPWM (void);
bool FTM3_IsSteppingDone (void);

void FTM1_Init (void);
void FTM1_SendSteps ( uint32_t steps, float freqinHz );
void FTM1_StopPWM (void);
bool FTM1_IsSteppingDone (void);

void FTM2_Init (void);
void FTM2_SendSteps ( uint32_t steps, float freqinHz );
void FTM2_StopPWM (void);
bool FTM2_IsSteppingDone (void);


#endif /* SOURCES_PWM_DRV_H_ */
