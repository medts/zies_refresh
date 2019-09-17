#include "pwm_drv.h"
#include "PE_Types.h"

#include "M1_MODE_0.h"
#include "M1_MODE_1.h"
#include "M1_MODE_2.h"

struct FTM_Params stFTM0;
struct FTM_Params stFTM3;

#define SYS_CLOCK_FREQ_HZ   CPU_BUS_CLK_HZ

volatile uint32_t FTM0_counter;
volatile uint32_t FTM3_counter;

void FTM0_StartPWM ( float freqInHz );
void FTM3_StartPWM ( float freqInHz );


//-------------------------------------------------------------------------------------
void FTM0_Init ()
{
	NVICIP58 = 0x30; //set FTM0 interrupt priority
	NVICICPR1 |= 1 << 10; //clear the pending register of interrupt source 58(FTM0)
	NVICISER1 |= 1 << 10; //Enable the interrupt source of FTM0

	SIM_SCGC6 |= 0x01000000; // Enable clock gate;
	PORTA_PCR1 = 0x0A0302; //PTA1 in FTM mode
	GPIOA_PDDR = 0x0002; //PTA1 is in output mode

	/*Initializing the status and control register to set clock source*/
	FTM0_SC = 0x68; //PRESCALE = /1

	FTM0_CONF = 0xC0; //set up BDM in 11
	FTM0_FMS = 0x00; //clear the WPEN so that WPDIS is set in FTM0_MODE register
	FTM0_C6SC = 0x28; //High_Low_High_ for center-alignment
	FTM0_SYNC |= 0x01;
	FTM0_SYNCONF |= 0x80; //enable enhanced PWM synchronization
	FTM0_CNTIN = 0x00;

	FTM0_POL |= 0x40; //the masked pin are HIGH logic
	FTM0_CONF |= 0x00; //four PWM cycle  generate one overflow interrupt
	FTM0_SYNCONF |= (FTM_SYNCONF_SWWRBUF_MASK | FTM_SYNCONF_SYNCMODE_MASK); //SET THE //SWWRBUF BIT AND SWRSTCNT BIT
	FTM0_SYNCONF |= FTM_SYNCONF_SWOM_MASK; //enable mask function
	FTM0_SYNC |= (FTM_SYNC_SWSYNC_MASK | FTM_SYNC_CNTMIN_MASK);

}

void FTM0_StartPWM ( float freqInHz )
{
	uint32_t mod = 0;
	uint32_t duty = 0;

	mod = (uint32_t) (SYS_CLOCK_FREQ_HZ / freqInHz); // 1pulse time period = 0.04us, 1MOD_Value = 1/(freq * time period of 1 pulse)
	duty = (uint32_t) (mod / 2); //duty cycle = 50%
	/*Initializing the modulo register (final value)*/
	FTM0_MOD = mod;
	/*Initializing CnV register value*/
	FTM0_C6V = duty;

	/*enable write the CnV register*/
	FTM0_MODE |= 0x05;
}

void FTM0_SendSteps ( uint32_t steps, float freqInHz )
{
	stFTM0.freq = freqInHz;
	stFTM0.pulses = steps;
	FTM0_StartPWM(stFTM0.freq);
}

void FTM0_StopPWM ()
{
	FTM0_SC &= 0xFFFFFFFE;	//disabling the PWM
}

void FTM0_Interrupt ( void )
{
	FTM0_counter++;
	if (FTM0_SC & 0x80)
	{
		FTM0_SC &= 0x7F; //clear TOF bit
		if (FTM0_counter >= stFTM0.pulses)
		{
			FTM0_SC &= 0xFFFFFFFE;
			FTM0_POL |= 0x00;
			FTM0_counter = 0;
		}
	}
}

bool FTM0_IsSteppingDone (void)
{
	return (FTM0_counter == 0);
}

void M1SetMode(DrvStepSize stepsize)
{
	switch(stepsize)
	{
		case FullStep:
			M1_MODE_0_PutVal(0, FALSE);
			M1_MODE_1_PutVal(0, FALSE);
			M1_MODE_2_PutVal(0, FALSE);
			break;

		case HalfStep :
			M1_MODE_0_PutVal(0, TRUE);
			M1_MODE_1_PutVal(0, FALSE);
			M1_MODE_2_PutVal(0, FALSE);
			break;

		case QuarterStep :
			M1_MODE_0_PutVal(0, FALSE);
			M1_MODE_1_PutVal(0, TRUE);
			M1_MODE_2_PutVal(0, FALSE);
			break;

		case MicroSteps_8 :
			M1_MODE_0_PutVal(0, FALSE);
			M1_MODE_1_PutVal(0, TRUE);
			M1_MODE_2_PutVal(0, TRUE);
			break;

		case MicroSteps_16 :
			M1_MODE_0_PutVal(0, FALSE);
			M1_MODE_1_PutVal(0, FALSE);
			M1_MODE_2_PutVal(0, TRUE);
			break;

		case MicroSteps_32 :
			M1_MODE_0_PutVal(0, TRUE);
			M1_MODE_1_PutVal(0, FALSE);
			M1_MODE_2_PutVal(0, TRUE);
			break;
	}
}
//-------------------------------------------------------------------------------------

void FTM3_Init ()
{
	NVICIP87 = 0x50; //set FTM3 interrupt priority
	NVICICPR2 |= 1 << 17; //clear the pending register of interrupt source 87(FTM3)
	NVICISER2 |= 1 << 17; //set the interrupt source of FTM3

	SIM_SCGC6 |= 0x03000040; // Enable clock gate;
	PORTC_PCR8 = 0x0A0302; //PTC8 in FTM mode
	GPIOC_PDDR = 0x0100; //PTC8 is in output mode

	/*Initializing the status and control register to set clock source*/
	FTM3_SC = 0x68; //PRESCALE = /1

	FTM3_CONF = 0xC0; //set up BDM in 11
	FTM3_FMS = 0x00; //clear the WPEN so that WPDIS is set in FTM0_MODE register
	FTM3_C4SC = 0x28; //High_Low_High_ for center-alignment
	FTM3_SYNC |= 0x01;
	FTM3_SYNCONF |= 0x80; //enable enhanced PWM synchronization
	FTM3_CNTIN = 0x00;

	FTM3_POL |= 0x10; //the masked pin are HIGH logic
	FTM3_CONF |= 0x00; //four PWM cycle  generate one overflow interrupt
	FTM3_SYNCONF |= (FTM_SYNCONF_SWWRBUF_MASK | FTM_SYNCONF_SYNCMODE_MASK); //SET THE //SWWRBUF BIT AND SWRSTCNT BIT
	FTM3_SYNCONF |= FTM_SYNCONF_SWOM_MASK; //enable mask function
	FTM3_SYNC |= (FTM_SYNC_SWSYNC_MASK | FTM_SYNC_CNTMIN_MASK);
}

void FTM3_StartPWM ( float freqInHz )
{
	uint32_t mod = 0;
	uint32_t duty = 0;

	mod = (uint32_t) (SYS_CLOCK_FREQ_HZ / freqInHz); // 1pulse time period = 0.04us, 1MOD_Value = 1/(freq*timeperiod of 1 pulse)
	duty = (uint32_t) (mod / 2); //duty cycle = 50%

	/*Initializing the modulo register (final value)*/
	FTM3_MOD = mod;
	/*Initializing CnV register value*/
	FTM3_C4V = duty;

	/*enable write the CnV register*/
	FTM3_MODE |= 0x05;

}

void FTM3_SendSteps ( uint32_t steps, float freqInHz )
{
	stFTM3.freq = freqInHz;
	stFTM3.pulses = steps;
	FTM3_StartPWM(stFTM3.freq);
}

void FTM3_StopPWM ()
{
	FTM3_SC &= 0xFFFFFFFE;
}

void FTM3_Interrupt ( void )
{
	FTM3_counter++;
	if (FTM3_SC & 0x80)
	{
		FTM3_SC &= 0x7F; //clear TOF bit
		if (FTM3_counter == stFTM3.pulses)
		{
			FTM3_SC &= 0xFFFFFFFE;
			FTM3_POL |= 0x00;
			FTM3_counter = 0;
		}
	}
}

bool FTM3_IsSteppingDone (void)
{
	return (FTM0_counter == 0);
}

void M2SetMode(DrvStepSize stepsize)
{
	switch(stepsize)
	{
		case FullStep:
			M2_MODE_0_PutVal(0, FALSE);
			M2_MODE_1_PutVal(0, FALSE);
			M2_MODE_2_PutVal(0, FALSE);
			break;

		case HalfStep :
			M2_MODE_0_PutVal(0, TRUE);
			M2_MODE_1_PutVal(0, FALSE);
			M2_MODE_2_PutVal(0, FALSE);
			break;

		case QuarterStep :
			M2_MODE_0_PutVal(0, FALSE);
			M2_MODE_1_PutVal(0, TRUE);
			M2_MODE_2_PutVal(0, FALSE);
			break;

		case MicroSteps_8 :
			M2_MODE_0_PutVal(0, FALSE);
			M2_MODE_1_PutVal(0, TRUE);
			M2_MODE_2_PutVal(0, TRUE);
			break;

		case MicroSteps_16 :
			M2_MODE_0_PutVal(0, FALSE);
			M2_MODE_1_PutVal(0, FALSE);
			M2_MODE_2_PutVal(0, TRUE);
			break;

		case MicroSteps_32 :
			M2_MODE_0_PutVal(0, TRUE);
			M2_MODE_1_PutVal(0, FALSE);
			M2_MODE_2_PutVal(0, TRUE);
			break;
	}
}
