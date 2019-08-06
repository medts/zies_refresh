#ifdef VERSION_DEBUG
#include "common.hpp"
#include "debug.hpp"
#include "time.hpp"
#include "state_machine.hpp"
#include "main_loop.hpp"
#include "string.h"
#include "stdarg.h"
#include "stdio.h"
#include "stdlib.h"
#include <cctype>
#include <limits.h>

extern "C"
{
#include "Debug_UART1.h"
}  // end extern "C"

// global vars
const bool DebugPrintHostPackets = false;
bool DebugInterruptsAreDisabled = false;

// State Machine ===================================================
SM_STATE(Debug_PowerUp)
SM_STATE(Debug_Idle)
SM_STATE(Debug_WaitForCommandCompletion)
SM_STATE(Debug_PromptIntegerPoll)
SM_STATE(Debug_PromptIndexPoll)
SM_STATE(Debug_PromptStringPoll)
SM_STATE(Debug_WaitForReadHostIndex)
SM_STATE(Debug_WaitForWriteHostIndex)
SM_STATE(Debug_WaitForWriteHostValue)

StateMachine gSM_Debug("Debug", &Debug_PowerUp, 0, 0);

// local vars
const int DebugPrintBufferSize = 1000;
char DebugPrintBuffer[DebugPrintBufferSize + 1];
const Time DebugTimeoutTime(5000.0);  // 5 sec
TimeoutTimer DebugTimer;
SM_Msg * DebugCommandMsg;
int32_t DebugParam1;  // general purpose, SM var
int32_t DebugParam2;  // general purpose, SM var
int DebugDeviceIndex;

// DebugPromptFor:
int32_t DebugPromptInteger;  // return value
State * DebugCallback;
int32_t DebugIntegerMin;
int32_t DebugIntegerMax;
int DebugNumChars;
const int DebugRcvBufMaxChars = 100;
char DebugRcvBuf[DebugRcvBufMaxChars + 2];

//================================================================
void DebugPrint ( const char * theText )
{
	word numToSend = strlen(theText);
	word numSent;
	do
	{
		Debug_UART1_SendBlock((Debug_UART1_TComData *) theText, numToSend, &numSent);
		theText += numSent;
		numToSend -= numSent;
		if (DebugInterruptsAreDisabled)
		{  // force the ISR to execute
			uint32_t & STIR = *((uint32_t*) 0xE000EF00); // Provides a mechanism for software to generate an interrupt. see ARM Arch.Ref.Man.
			STIR = INT_UART1_RX_TX - 16u;
		}
	} while (numToSend > 0);
}
//================================================================
void DebugPrintf ( const char* theFormat, ... )
{
	va_list args;
	va_start(args, theFormat);
	ASSERT(0 < vsnprintf(DebugPrintBuffer, DebugPrintBufferSize, theFormat, args));
	va_end(args);
	DebugPrint(DebugPrintBuffer);
}
//================================================================
bool KeyPressed ()
{
	char c;
	return !Debug_UART1_RecvChar((Debug_UART1_TComData *) &c);
}
//================================================================
bool DebugGetChar ( char * theChar )  // return true iff a char was recieved
{
	return !Debug_UART1_RecvChar((Debug_UART1_TComData *) theChar);
}
//================================================================
State * DebugPromptForInteger ( const char * thePrompt, int32_t theMin, int32_t theMax, State * theCallback )
{
	DebugCallback = theCallback;
	DebugIntegerMin = theMin;
	DebugIntegerMax = theMax;
	DebugNumChars = 0;
	DebugPrintf("%s (%d..%d): ", thePrompt, theMin, theMax);
	return &Debug_PromptIntegerPoll;
}
//================================================================
SM_POLL (Debug_PromptIntegerPoll)
{
	if (!DebugGetChar(&DebugRcvBuf[DebugNumChars])) return 0;  // wait for a keystroke
	DebugRcvBuf[DebugNumChars + 1] = 0;
	DebugPrint(&DebugRcvBuf[DebugNumChars]);  // echo the char

	switch (DebugRcvBuf[DebugNumChars])
	{
		case '-':
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			if (DebugNumChars < DebugRcvBufMaxChars) DebugNumChars++;
			return 0;

		case '\b':  // Backspace
			if (DebugNumChars > 0) DebugNumChars--;
			return 0;

		case '\r':  // Enter
			DebugPrint(CRLF);
			DebugRcvBuf[DebugNumChars] = 0;
			DebugPromptInteger = min(max(DebugIntegerMin, (int32_t) atoi(DebugRcvBuf)), DebugIntegerMax);
			return DebugCallback;

	}  // end switch
	DebugPrint(" ... canceled." CRLF);
	return &Debug_Idle;
}
//================================================================
State * DebugPromptForIndex ( int32_t theMin, int32_t theMax, State * theCallback )
{
	DebugCallback = theCallback;
	DebugIntegerMin = theMin;
	DebugIntegerMax = theMax;
	return &Debug_PromptIndexPoll;
}
//================================================================
SM_POLL (Debug_PromptIndexPoll)
{
	char chr[] = { ' ', 0 };
	if (!DebugGetChar(chr))  // if rcv error, wait for char
	    return 0;
	DebugPrint(chr);  // echo the keystroke
	if (chr[0] < '0' || chr[0] > '9')
	{
		DebugPrintf("  !error, must be %d..%d" CRLF, DebugIntegerMin, DebugIntegerMax);
		return &Debug_Idle;
	}
	DebugPromptInteger = min(max(DebugIntegerMin, (int32_t) atoi(chr)), DebugIntegerMax);
	return DebugCallback;
}
//================================================================
State * DebugPromptForString ( const char * thePrompt, State * theCallback )
{
	DebugCallback = theCallback;
	DebugNumChars = 0;
	DebugPrintf("%s: ", thePrompt);
	return &Debug_PromptStringPoll;
}
//================================================================
SM_POLL (Debug_PromptStringPoll)
{
	if (!DebugGetChar(&DebugRcvBuf[DebugNumChars])) return 0;  // wait for a keystroke
	DebugRcvBuf[DebugNumChars + 1] = 0;
	DebugPrint(&DebugRcvBuf[DebugNumChars]);  // echo the char

	switch (DebugRcvBuf[DebugNumChars])
	{
		default:
			if (DebugNumChars < DebugRcvBufMaxChars)
				DebugNumChars++;
			else
				DebugPrintf(" (%d chars max) ", DebugRcvBufMaxChars);
			return 0;

		case '\b':  // Backspace
			if (DebugNumChars > 0) DebugNumChars--;
			return 0;

		case '\r':  // Enter
			DebugPrint(CRLF);
			DebugRcvBuf[DebugNumChars] = 0;
			return DebugCallback;

	}  // end switch
	DebugPrint(" ... canceled." CRLF);
	return &Debug_Idle;
}
//================================================================
State * DebugSendCommand ( SM_Msg * theMsg, const char * theText, State * theCallback )
{
	DebugCommandMsg = theMsg;
	DebugCallback = theCallback;
	if (DebugCommandMsg->Pending)
	{
		DebugPrint("Command is already in progress." CRLF);
		return &Debug_Idle;
	}
	DebugPrintf(" %s ... ", theText, CRLF);
	DebugCommandMsg->Pending = true;  // send message
	return &Debug_WaitForCommandCompletion;
}
//================================================================
SM_POLL (Debug_WaitForCommandCompletion)
{
	if (DebugCommandMsg->Pending) return 0;
	DebugPrint(" Done." CRLF);
	if (DebugCallback) return DebugCallback;
	return &Debug_Idle;
}
//================================================================
SM_POLL (Debug_WaitForReadHostIndex)
{
	int32_t index = DebugPromptInteger;
	uint32_t value = gRegisters[(RegisterIds) index];
	DebugPrintf("Register[%d]= %d  (0x%X)" CRLF, index, value, value);
	return &Debug_Idle;
}
//================================================================
SM_POLL (Debug_WaitForWriteHostIndex)
{
	DebugParam1 = DebugPromptInteger;  // save the index
	return DebugPromptForInteger("Enter value", LONG_MIN, LONG_MAX, &Debug_WaitForWriteHostValue);
}
//================================================================
SM_POLL (Debug_WaitForWriteHostValue)
{
	int32_t index = DebugParam1;
	uint32_t value = DebugPromptInteger;
	gRegisters[(RegisterIds) index] = value;
	DebugPrintf("...Register[%d]= %d  (0x%X)" CRLF, index, value, value);
	return &Debug_Idle;
}
//================================================================
SM_POLL (Debug_PowerUp)
{
	return &Debug_Idle;
}
//================================================================
SM_POLL (Debug_Idle)
{
	char chr[] = { ' ', ' ', 0 };
	if (!DebugGetChar(chr))  // if rcv error, wait for char
	    return 0;
	DebugPrint(chr);  // echo the keystroke
	switch (toupper(chr[0]))
	{
		case 'E':  // print ErrorCodes --------------------------------------------------------
			StateMachine::PrintAllErrorCodes();
			break;

		case 'F':  // reset loop stats --------------------------------------------------------
			DebugPrint("Reset loop-time stats." CRLF);
			gLoopStats.Reset();
			break;

		case 'R':  // read Host register --------------------------------------------------------
			return DebugPromptForInteger("Enter reg index", 0, RegistersSize - sizeof(uint32_t),
			        &Debug_WaitForReadHostIndex);

		case 'W':  // write Host register --------------------------------------------------------
			return DebugPromptForInteger("Enter reg index", 0, RegistersSize - sizeof(uint32_t),
			        &Debug_WaitForWriteHostIndex);

		default:
			DebugPrint(CRLF
			"   R  -Read  Host register" CRLF
			"   W  -Write Host register" CRLF
			"   E  -Print SM ErrorCodes" CRLF
			"   H  -Print Host packets, toggle" CRLF
			"   F  -Reset loop stats" CRLF);
	}
}
#endif
