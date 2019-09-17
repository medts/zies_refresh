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

#include "motor1.hpp"
#include "motor2.hpp"

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
SM_STATE(Debug_PromptMotorCommand)
SM_STATE(Debug_PromptMotorMove)
SM_STATE(Debug_PromptMotorMoveRel)

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

MotorSM * DebugMotorSMs[] = { &gSM_Motor1, &gSM_Motor2 };


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
// =================================================================
SM_POLL (Debug_PromptMotorCommand)
{
	if (!DebugGetChar (DebugRcvBuf))
		return 0;
	DebugDeviceIndex = DebugPromptInteger - 1;
	MotorSM * sm = DebugMotorSMs[DebugDeviceIndex];
	DebugPrintf ("%c %s motor: ", DebugRcvBuf[0], sm->Motor.UserName);
	const char * text;
	SM_Msg * msg;

	switch (toupper (DebugRcvBuf[0]))
	{
		case 'I':  // Initialize motor driver
			msg = &sm->SMM_Initialize;
			text = "Initializing motor driver";
			break;
		case 'H':  // home motor
			msg = &sm->SMM_FindHome;
			text = "Homing motor";
			break;
		case 'M':  // move motor to requested position
			return DebugPromptForInteger ("Enter desired position", LONG_MIN, LONG_MAX, &Debug_PromptMotorMove);
		case 'Z':  // mark current position as zero
			msg = &sm->SMM_MarkZero;
			text = "Marking current position as zero";
			break;
		case 'A':  // Move relative
			return DebugPromptForInteger ("Enter desired distance", LONG_MIN, LONG_MAX, &Debug_PromptMotorMoveRel);
		case 'B':  // Stop motor
			msg = &sm->SMM_Stop;
			sm->SMM_Stop.Param1 = 1;
			text = "Stopping motor";
			break;
		case 'E':  // Enable
			msg = &sm->SMM_HoldEnable;
			sm->SMM_HoldEnable.Param1 = 1;
			text = "Enabling motor";
			break;
		case 'D':  // Disable
			msg = &sm->SMM_HoldEnable;
			sm->SMM_HoldEnable.Param1 = 0;
			text = "Disabling motor";
			break;
		default:
			DebugPrint ("INVALID command" CRLF);
			return &Debug_Idle;
	}  // end switch
	return DebugSendCommand (msg, text, 0);
}
// =================================================================
SM_POLL (Debug_PromptMotorMove)
{
	DebugMotorSMs[DebugDeviceIndex]->SMM_MoveTo.Param1 = DebugPromptInteger;
	return DebugSendCommand (&DebugMotorSMs[DebugDeviceIndex]->SMM_MoveTo, "Moving motor", 0);
}
// =================================================================
SM_POLL (Debug_PromptMotorMoveRel)
{
	DebugMotorSMs[DebugDeviceIndex]->SMM_MoveRel.Param1 = DebugPromptInteger;
	return DebugSendCommand (&DebugMotorSMs[DebugDeviceIndex]->SMM_MoveRel, "Moving motor relative", 0);
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
			return DebugPromptForInteger("Enter register index", 0, RegistersSize - sizeof(uint32_t),
			        &Debug_WaitForReadHostIndex);

		case 'W':  // write Host register --------------------------------------------------------
			return DebugPromptForInteger("Enter register index", 0, RegistersSize - sizeof(uint32_t),
			        &Debug_WaitForWriteHostIndex);

		case 'M':  // Motor control
			return DebugPromptForIndex (1, 8, &Debug_PromptMotorCommand);

		default:
			DebugPrint(CRLF
			"   R  - Read  Host register " CRLF
			"   W  - Write Host register " CRLF
			"   E  - Print SM ErrorCodes " CRLF
			"   H  - Print Host packets, toggle " CRLF
			"   F  - Reset loop statistics " CRLF
			"   M  - Motor " CRLF
			"        n = 1 - X axis " CRLF
			"            2 - Y axis " CRLF
			"            x = I - Initialize motor " CRLF
			"                H - Find and mark home " CRLF
			"                M - Move to position " CRLF
			"                Z - Mark current position as zero " CRLF
			"                A - Move relative " CRLF
			"                S - Stop motor " CRLF
			"                E - Enable motor " CRLF
			"                D - Disable motor " CRLF
			);
	}
}
#endif
