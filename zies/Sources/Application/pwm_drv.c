#include "pwm_drv.h"
#include "PE_Types.h"

struct FTM_Params stFTM0;
struct FTM_Params stFTM3;
struct FTM_Params stFTM1;
struct FTM_Params stFTM2;

#define SYS_CLOCK_FREQ_HZ   CPU_CORE_CLK_HZ

volatile uint32_t FTM0_counter;
volatile uint32_t FTM3_counter;
volatile uint32_t FTM1_counter;
volatile uint32_t FTM2_counter;

void FTM0_StartPWM ( float freqInHz );
void FTM3_StartPWM ( float freqInHz );
void FTM1_StartPWM ( float freqInHz );
void FTM2_StartPWM ( float freqInHz );


//-------------------------------------------------------------------------------------
void FTM0_Init ()
{
	NVICIP58 = 0x50u; //set FTM0 interrupt priority
	NVICICPR1 |= 1u << 10u; //clear the pending register of interrupt source 58(FTM0)
	NVICISER1 |= 1u << 10u; //Enable the interrupt source of FTM0

	SIM_SCGC5  |= 1u << 11u; //Enable Port C clock
	SIM_SCGC6 |= 1u << 24u; // Enable FTM0 clock gate;
	PORTC_PCR2 = 0x0A0402; //PTC2 in FTM mode
	GPIOC_PDDR = 0x0004; //PTC2 is in output mode

	FTM0_CONF = 0xC0; //set up BDM in 11
	FTM0_FMS = 0x00; //clear the WPEN so that WPDIS is set in FTM0_MODE register
	FTM0_C1SC = 0x28; //High_Low_High_ for center-alignment
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

	mod = (uint32_t) (SYS_CLOCK_FREQ_HZ / freqInHz); // 1pulse time period = 0.04us, 1MOD_Value = 1/(freq * time period of 1 pulse, SYS_CLOCK_FREQ_HZ)
	duty = (uint32_t) (mod / 2); //duty cycle = 50%

	/*enable write the CnV register*/
	FTM0_MODE |= 0x05;

	/*Initializing the modulo register (final value)*/
	FTM0_MOD = mod;
	/*Initializing CnV register value*/
	FTM0_C1V = duty;

	/*Initializing the status and control register to set clock source*/
	FTM0_SC = 0x68; //PRESCALE = /1
}

void FTM0_SendSteps ( uint32_t steps, float freqInHz )
{
	stFTM0.freq = freqInHz;
	stFTM0.pulses = steps;
	FTM0_StartPWM(stFTM0.freq);
}

void FTM0_StopPWM ()
{
	FTM0_SC &= 0xFFFFFFA7;	//disabling the PWM
}

void FTM0_Interrupt ( void )
{
	FTM0_counter++;
	if (FTM0_SC & 0x80)
	{
		FTM0_SC &= 0x7F; //clear TOF bit
		if (FTM0_counter >= stFTM0.pulses)
		{
			FTM0_SC = FTM0_SC & 0xFFFFFFA7;
			FTM0_POL |= 0x00;
			FTM0_counter = 0;
		}
	}
}

bool FTM0_IsSteppingDone (void)
{
	return (FTM0_counter == stFTM0.pulses);
}

//-------------------------------------------------------------------------------------

void FTM3_Init ()
{
	NVICIP87 = 0x50u; //set FTM3 interrupt priority
	NVICICPR2 |= 1u << 7u; //clear the pending register of interrupt source 87(FTM3)
	NVICISER2 |= 1u << 7u; //set the interrupt source of FTM3

	SIM_SCGC5  |= 1u << 11u; //Enable Port C clock
	SIM_SCGC6 |= 1u << 6u; // Enable FTM3 clock gate;
	PORTC_PCR8 = 0x0A0302u; //PTC8 in FTM mode
	GPIOC_PDDR = 0x0100u; //PTC8 is in output mode

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

	/*enable write the CnV register*/
	FTM3_MODE |= 0x05;
	/*Initializing the modulo register (final value)*/
	FTM3_MOD = mod;
	/*Initializing CnV register value*/
	FTM3_C4V = duty;

	FTM3_SC = 0x68; //PRESCALE = /1
}

void FTM3_SendSteps ( uint32_t steps, float freqInHz )
{
	stFTM3.freq = freqInHz;
	stFTM3.pulses = steps;
	FTM3_StartPWM(stFTM3.freq);
}

void FTM3_StopPWM ()
{
	FTM3_SC &= 0xFFFFFFA7;
}

void FTM3_Interrupt ( void )
{
	FTM3_counter++;
	if (FTM3_SC & 0x80)
	{
		FTM3_SC &= 0x7F; //clear TOF bit
		if (FTM3_counter >= stFTM3.pulses)
		{
			FTM3_SC &= 0xFFFFFFA7;
			FTM3_POL |= 0x00;
			FTM3_counter = 0;
		}
	}
}

bool FTM3_IsSteppingDone (void)
{
	return (FTM3_counter == stFTM3.pulses);
}

//-------------------------------------------------------------------------------------

void FTM1_Init ()
{
	NVICIP59 = 0x50u; //set FTM3 interrupt priority
	NVICICPR1 |= 1u << 11u; //clear the pending register of interrupt source 59(FTM1)
	NVICISER1 |= 1u << 11u; //set the interrupt source of FTM1

	SIM_SCGC5  |= 1u << 9u; //Enable Port A clock
	SIM_SCGC6 |= 1u << 25u; // Enable FTM1 clock gate;
	PORTA_PCR12 = 0x000A0302; //PTA12 in FTM mode
	GPIOA_PDDR = 0x00001000; //PTA12 is in output mode

	FTM1_CONF = 0xC0; //set up BDM in 11
	FTM1_FMS = 0x00; //clear the WPEN so that WPDIS is set in FTM0_MODE register
	FTM1_C0SC = 0x28; //High_Low_High_ for center-alignment
	FTM1_SYNC |= 0x01;
	FTM1_SYNCONF |= 0x80; //enable enhanced PWM synchronization
	FTM1_CNTIN = 0x00;

	FTM1_POL |= 0x10; //the masked pin are HIGH logic
	FTM1_CONF |= 0x00; //four PWM cycle  generate one overflow interrupt
	FTM1_SYNCONF |= (FTM_SYNCONF_SWWRBUF_MASK | FTM_SYNCONF_SYNCMODE_MASK); //SET THE //SWWRBUF BIT AND SWRSTCNT BIT
	FTM1_SYNCONF |= FTM_SYNCONF_SWOM_MASK; //enable mask function
	FTM1_SYNC |= (FTM_SYNC_SWSYNC_MASK | FTM_SYNC_CNTMIN_MASK);
}

void FTM1_StartPWM ( float freqInHz )
{
	uint32_t mod = 0;
	uint32_t duty = 0;

	mod = (uint32_t) (SYS_CLOCK_FREQ_HZ / freqInHz); // 1pulse time period = 0.04us, 1MOD_Value = 1/(freq*timeperiod of 1 pulse,SYS_CLOCK_FREQ_HZ)
	duty = (uint32_t) (mod / 2); //duty cycle = 50%

	/*enable write the CnV register*/
	FTM1_MODE |= 1u << 2u;
	/*Initializing the modulo register (final value)*/
	FTM1_MOD = mod;
	/*Initializing CnV register value*/
	FTM1_C0V = duty;
	/*enable FTM*/
	FTM1_MODE |= 3u;

	FTM1_SC = 0x68; //PRESCALE = /1
}

void FTM1_SendSteps ( uint32_t steps, float freqInHz )
{
	stFTM1.freq = freqInHz;
	stFTM1.pulses = steps;
	FTM1_StartPWM(stFTM1.freq);
}

void FTM1_StopPWM ()
{
	FTM1_SC &= 0xFFFFFFA7;
}

void FTM1_Interrupt ( void )
{
	FTM1_counter++;
	if (FTM1_SC & 0x80)
	{
		FTM1_SC &= 0x7F; //clear TOF bit
		if (FTM1_counter >= stFTM1.pulses)
		{
			FTM1_SC &= 0xFFFFFFA7;
			FTM1_POL |= 0x00;
			FTM1_counter = 0;
		}
	}
}

bool FTM1_IsSteppingDone (void)
{
	return (FTM1_counter == stFTM1.pulses);
}

//-------------------------------------------------------------------------------------

void FTM2_Init ()
{
	NVICIP60 = 0x50; //set FTM3 interrupt priority
	NVICICPR1 |= 1 << 12; //clear the pending register of interrupt source 60(FTM2)
	NVICISER1 |= 1 << 12; //set the interrupt source of FTM2

	SIM_SCGC5  |= 1u << 10u; //Enable Port B clock
	SIM_SCGC6 |= 1u << 26u; // Enable FTM2 clock gate;
	PORTB_PCR18 = 0x0A0302; //PTC8 in FTM mode
	GPIOB_PDDR = 0x00040000; //PTCB is in output mode

	FTM2_CONF = 0xC0; //set up BDM in 11
	FTM2_FMS = 0x00; //clear the WPEN so that WPDIS is set in FTM0_MODE register
	FTM2_C0SC = 0x28; //High_Low_High_ for center-alignment
	FTM2_SYNC |= 0x01;
	FTM2_SYNCONF |= 0x80; //enable enhanced PWM synchronization
	FTM2_CNTIN = 0x00;

	FTM2_POL |= 0x10; //the masked pin are HIGH logic
	FTM2_CONF |= 0x00; //four PWM cycle  generate one overflow interrupt
	FTM2_SYNCONF |= (FTM_SYNCONF_SWWRBUF_MASK | FTM_SYNCONF_SYNCMODE_MASK); //SET THE //SWWRBUF BIT AND SWRSTCNT BIT
	FTM2_SYNCONF |= FTM_SYNCONF_SWOM_MASK; //enable mask function
	FTM2_SYNC |= (FTM_SYNC_SWSYNC_MASK | FTM_SYNC_CNTMIN_MASK);
}

void FTM2_StartPWM ( float freqInHz )
{
	uint32_t mod = 0;
	uint32_t duty = 0;

	mod = (uint32_t) (SYS_CLOCK_FREQ_HZ / freqInHz); // 1pulse time period = 0.04us, 1MOD_Value = 1/(freq*timeperiod of 1 pulse)
	duty = (uint32_t) (mod / 2); //duty cycle = 50%

	/*enable write the CnV register*/
	FTM2_MODE |= 0x05;
	/*Initializing the modulo register (final value)*/
	FTM2_MOD = mod;
	/*Initializing CnV register value*/
	FTM2_C0V = duty;

	FTM2_SC = 0x68; //PRESCALE = /1
}

void FTM2_SendSteps ( uint32_t steps, float freqInHz )
{
	stFTM2.freq = freqInHz;
	stFTM2.pulses = steps;
	FTM2_StartPWM(stFTM2.freq);
}

void FTM2_StopPWM ()
{
	FTM2_SC &= 0xFFFFFFA7;
}

void FTM2_Interrupt ( void )
{
	FTM2_counter++;
	if (FTM2_SC & 0x80)
	{
		FTM2_SC &= 0x7F; //clear TOF bit
		if (FTM2_counter >= stFTM2.pulses)
		{
			FTM2_SC &= 0xFFFFFFA7;
			FTM2_POL |= 0x00;
			FTM2_counter = 0;
		}
	}
}

bool FTM2_IsSteppingDone (void)
{
	return (FTM2_counter == stFTM2.pulses);
}

