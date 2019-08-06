// =================================================================
// time.cpp
// Timer module implements generic timer routines for delay and tracking,
// based on SysTick timer.
// =================================================================

#include "time.hpp"
#include "debug.hpp"
#include "math.h"

// global vars
const Time Time::ValueZero(0, 0);
const Time Time::ValueMax(0xFFFFFFFF, 0xFFFFFFFF);

// local vars
volatile uint32_t SysTimeBaseMS = 0;
volatile uint32_t SysTimeBaseLS = 0;

const float TicksPerMSec = 100000;
const float Uint32max = 1.0 + (float) 0xFFFFFFFF;
const float TicksPerMSecMS = TicksPerMSec / Uint32max;
const float MSecPerTick = 1.0 / TicksPerMSec;
const float MSecPerTickMS = MSecPerTick * Uint32max;

const float USecPerTick = MSecPerTick * 1e3;
const float USecPerTickMS = MSecPerTickMS * 1e3;
const uint32_t USecMax = TicksPerMSec / 1e3;

const float NSecPerTick = MSecPerTick * 1e6;
const uint32_t NSecMax = TicksPerMSec / 1e6 * Uint32max;

extern "C"
{
#include "SysTick.h"
//================================================================
// Process 24 bit SysTick counter overflow, every 167.772 mSec = 0xFFFFFF / 100 MHz
	PE_ISR(SysTick_ISR)
	{
		if (SysTimeBaseLS == 0xFF000000u)
		{
			SysTimeBaseMS++;
			SysTimeBaseLS = 0;
		}
		else
			SysTimeBaseLS += 0x01000000u;
		uint32_t dummy = SYST_CSR; //  Reading the COUNTFLAG status bit clears it to 0.
	}
} // end extern "C"
//================================================================
// Construct a Time object containing the number of SysTicks elapsed since power up.
CurrentTime::CurrentTime ()
{
	uint32_t csr;
	do
	{
		DisableInterrupts();
		ValueMS = SysTimeBaseMS;
		ValueLS = SysTimeBaseLS + (0x00FFFFFFu - (SYST_CVR & 0x00FFFFFFu));
		csr = SYST_CSR; // COUNTFLAG is set to 1 by a count transition from 1 to 0. Cleared by read.
		EnableInterrupts();
	} while (csr & 0x10000u); // repeat if an overflow occurred after we read the CVR.
}
//================================================================
Time::Time ( float theMilliSecs ) :
		ValueMS(theMilliSecs * TicksPerMSecMS), ValueLS(fmodf(theMilliSecs * TicksPerMSec, Uint32max))
{
}
//================================================================
uint32_t Time::ToMilliSec () const
{
	ASSERT(ValueMS < TicksPerMSec - 1);
	return ((float) ValueMS * MSecPerTickMS) + ((float) ValueLS * MSecPerTick);
}
//================================================================
float Time::ToMilliSecF () const
{
	return ((float) ValueMS * MSecPerTickMS) + ((float) ValueLS * MSecPerTick);
}
//================================================================
uint32_t Time::ToMicroSec () const
{
	ASSERT(ValueMS < USecMax);
	return ((float) ValueMS * USecPerTickMS) + ((float) ValueLS * USecPerTick);
}
//================================================================
uint32_t Time::ToNanoSec () const
{
	ASSERT(ValueMS == 0 && ValueLS < NSecMax);
	return (float) ValueLS * NSecPerTick;
}
//================================================================
void Time::Print ( const char * theTitle ) const
{
	const char * valUnits = "mSec";
	uint32_t val = ToMilliSec();
	if (val < 1000000u)
	{
		valUnits = "uSec";
		val = ToMicroSec();
		if (val < 1000000u)
		{
			valUnits = "nSec";
			val = ToNanoSec();
		}
	}
	DebugPrintf ("%s%u %s" CRLF, theTitle, val, valUnits);
}
