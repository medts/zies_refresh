#include "common.hpp"
#include "debug.hpp"

extern bool DebugInterruptsAreDisabled;

extern "C"
{
#include "HEARTBEAT_LED.h"

	//================================================================
	void Cpu_OnUnhandledVector ()
	{
		CRASH(10, "Unexpected Interrupt, undefined vector");
	}
}  // end extern "C"

//================================================================
uint16_t ReverseEndian16 ( uint16_t theVal )
{
	UnionUint16 word_union(theVal);
	UnionUint16 result;
	result.bytes[0] = word_union.bytes[1];
	result.bytes[1] = word_union.bytes[0];
	return result.word;
}
//================================================================
uint32_t ReverseEndian32 ( uint32_t theVal )
{
	UnionUint32 dword_union(theVal);
	UnionUint32 result;
	result.bytes[0] = dword_union.bytes[3];
	result.bytes[1] = dword_union.bytes[2];
	result.bytes[2] = dword_union.bytes[1];
	result.bytes[3] = dword_union.bytes[0];
	return result.dword;
}
// =================================================================
bool Mutex::Lock ()  // return false when already locked; true otherwise
{
	bool success = false;
	DisableInterrupts();
	if (!Flag)
	{
		Flag = true;
		success = true;
	}
	EnableInterrupts();
	return success;
}
//================================================================
CRC16 & CRC16::Append ( int theLength, uint8_t * theData )
{
	register uint8_t X;
	register uint8_t * ptr = theData;
	for (; theLength > 0; ptr++, theLength--)
	{
		X = (uint8_t) (*ptr ^ MSB);
		X = (uint8_t) (X ^ (X >> 4));
		MSB = (uint8_t) (LSB ^ (X >> 3) ^ (X << 4));
		LSB = (uint8_t) (X ^ (X << 5));
	}
	return *this;
}
//================================================================
#define DELAYx(theDelay) \
{ \
	asm volatile /* volatile insures same code compiled regardless of optimization level */ \
  ( \
  "       push   {r3,r4}            \n\t" \
  "       movs   r4,%[delay]        \n\t" \
  "loopOuter%=:                     \n\t" \
  "       movs   r3,4               \n\t" \
  "loopInner%=:                     \n\t" \
  "       subs    r3,r3,#1          \n\t" \
  "       bne    loopInner%=        \n\t" \
  "       subs    r4,r4,#1          \n\t" \
  "       bne    loopOuter%=        \n\t" \
  "       pop    {r3,r4}" \
  :: [delay] "r" (theDelay) \
  ); \
}
//================================================================
void CRASH ( int theBlinkCount, const char * theDescription )
{
#ifdef VERSION_DEBUG
	DebugInterruptsAreDisabled = true;  // run DebugPrintf without interrupts
	while (true)
	{
		DebugPrintf("CRASH %d, %s" CRLF, theBlinkCount, theDescription);
		for (uint8_t i = 1; i <= theBlinkCount; i++)
		{
			HEARTBEAT_LED_PutVal(0, 0);
			DELAYx(666666u);  // 100 mSec
			HEARTBEAT_LED_PutVal(0, 1);
			DELAYx(1333333u);  // 200 mSec
		}
		DELAYx(5333333u);   // 800 mSec
	}
#endif
}
//================================================================
