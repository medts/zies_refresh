#include "state_machine.hpp"
#include "debug.hpp"

StateMachine * StateMachine::SMlist[StateMachine::SMlistCountMax];
int StateMachine::SMlistCount = 0;

// =================================================================
void StateMachine::Poll ()
{
	State * newState;
	do
	{
		newState = CurrentState->Poll(this);
		if (newState) CurrentState = newState;
	} while (newState);
}
// =================================================================
void StateMachine::PollAll ()
{
	StateMachine ** machine = SMlist;
	for (int i = SMlistCount; i != 0; i--)
		(*(machine++))->Poll();
}
// =================================================================
bool StateMachine::SmIsBusyOrError ( RegisterIds theCommandAddress, int32_t theCommand )
{ // check the ErrorCode of the SM that has a HostCommand at theCommandAddress
// if busy, allow the Abort command
	int32_t * addr = reinterpret_cast<int32_t *>(&gRegisters[theCommandAddress]);
	StateMachine ** machine = SMlist;
	for (int i = SMlistCount; i != 0; i--, machine++)
		if (((*machine)->HostCommand != 0) && (((*machine)->HostCommand) == addr) && (*machine)->HostErrorCode != 0)
		{
			uint32_t errorCode = *(*machine)->HostErrorCode;
			if ((errorCode == SM_Busy) && (theCommand == CommandCodeAbort)) errorCode = NoError; // allow the Abort command if the SM is busy
			if (errorCode == NoError)
			{
				if (theCommand == 0x00u)
				{
					//DebugPrintf (" Wrong command %d for address %d " CRLF, theCommand, theCommandAddress);
					return false;
				}
				else
					*(*machine)->HostErrorCode = SM_Busy;  // set the SM's ErrorCode to busy
			}
			return errorCode != NoError;
		}
	return false;
}
// =================================================================
void StateMachine::PrintAllErrorCodes ()
{
	DebugPrintf("  ErrorStatus: 0x%08X" CRLF, gRegisters[ErrorStatusBM]);
	for (int i = 0; i < SMlistCount; i++)
		DebugPrintf("  0x%8X  %s" CRLF, (SMlist[i]->HostErrorCode ? *SMlist[i]->HostErrorCode : 0), SMlist[i]->Name);
}

// =================================================================
Error SM_Msg::TestAndSend ()
{
	if (Pending) return SM_Busy;
	Pending = true;
	return NoError;
}
// =================================================================
void SM_Int32ParamMsg::Send1Param ( int32_t theParam )
{
	Param1 = theParam;
	Pending = true;
}
// =================================================================
Error SM_Int32ParamMsg::TestAndSendParam ( int32_t theParam )
{
	if (Pending) return SM_Busy;
	Param1 = theParam;
	Pending = true;
	return NoError;
}
// =================================================================
void SM_2Int32ParamMsg::Send2Param ( int32_t theParam1, int32_t theParam2 )
{
	Param1 = theParam1;
	Param2 = theParam2;
	Pending = true;
}
// =================================================================
void SM_3Int32ParamMsg::Send3Param ( int32_t theParam1, int32_t theParam2, int32_t theParam3 )
{
	Param1 = theParam1;
	Param2 = theParam2;
	Param3 = theParam3;
	Pending = true;
}
// =================================================================
void SM_4Int32ParamMsg::Send4Param ( int32_t theParam1, int32_t theParam2, int32_t theParam3, int32_t theParam4 )
{
	Param1 = theParam1;
	Param2 = theParam2;
	Param3 = theParam3;
	Param4 = theParam4;
	Pending = true;
}

// =================================================================
void SM_TimeParamMsg::SendTimeParam ( const Time & theParam )
{
	Param1 = theParam;
	Pending = true;
}
// =================================================================
Error SM_TimeParamMsg::TestAndSendTimeParam ( const Time & theParam )
{
	if (Pending) return SM_Busy;
	Param1 = theParam;
	Pending = true;
	return NoError;
}
// =================================================================
void SM_StringParamMsg::SendStrParam ( char * theString, int theMaxSize )
{
	String = theString;
	MaxSize = theMaxSize;
	Pending = true;
}
// =================================================================
Error SM_Int32ParamMsg::Query ( int32_t theRequest, int32_t * theResult ) // multi-threaded callers should use unique Request Ids
{
	if (Pending) return SM_Busy;
	if (InProgress)
	{
		if (Request != theRequest) // if caller is not the originator
		    return SM_Busy;
		*theResult = Param1;
		InProgress = false;
		return NoError;
	}
	Request = theRequest; // save originator Id
	Param1 = theRequest;
	Pending = true;
	InProgress = true;
	return SM_Busy;
}

