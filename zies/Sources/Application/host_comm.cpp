#include "host_comm.hpp"
#include "error_status.hpp"
#include "debug.hpp"

extern "C"
{
#include <string.h>
} // end extern "C"

// =================================================================
// global vars
HostCommSM gSM_HostComm;

extern bool gIsHostTrafficOn;

// State Machine ===================================================
SM_STATE(HostCommSM_PowerUp)
SM_STATE(HostCommSM_Idle)
SM_STATE(HostCommSM_WaitForCRC)
SM_STATE(HostCommSM_WaitForTransmit)
SM_STATE(HostCommSM_WaitForRcvAck)
SM_STATE(HostCommSM_ErrorOccured)
SM_STATE(HostCommSM_ErrorWaitForTransmit)
SM_STATE(HostCommSM_ErrorWaitForFlush)
#define sm (*(HostCommSM *)theSM)

// =================================================================
typedef union
{
		uint32_t word;
		struct
		{
				bool notUsed0 : 4;  // 0..3
				bool Motor1_Home :1;  // 4
				bool Motor1_MaxTravel :1;  // 5
				bool Motor2_Home :1;  // 6
				bool Motor2_MaxTravel :1;  // 7
				bool Motor3_Home :1;  // 8
				bool Motor3_MaxTravel :1;  // 9
				bool Motor4_Home :1;  // 10
				bool Motor4_MaxTravel :1;  // 11
				bool Motor5_Home :1;  // 12
				bool Motor5_MaxTravel :1;  // 13
				bool Motor6_Home :1;  // 14
				bool Motor6_MaxTravel :1;  // 15
				bool Motor7_Home :1;  // 16
				bool Motor7_MaxTravel :1;  // 17
				bool Motor8_Home :1;  // 18
				bool Motor8_MaxTravel :1;  // 19
				bool notUsed1 :4;  // 20..23
				bool notUsed2 :8;  // 24..31
		} bit;
} SignalStatus_t;
// =================================================================
void SignalStatus_Update ()
{
	SignalStatus_t & signals = (SignalStatus_t&) gRegisters[Signals];
	signals.word = 0;
	// Update the signal status here for e.g., signals.bit.Motor1_Home = gSM_Motor1.IsHome() ? 1 : 0;
}

// =================================================================
typedef union
{
		uint16_t word;
		struct
		{
				bool ErrorBit :1; // 0
				bool HostCommErr :1; // 1
				bool notUsed0 :6; // 2..7
				bool Motor1Busy :1; // 8
				bool Motor2Busy :1; // 9
				bool Motor3Busy :1; // 10
				bool Motor4Busy :1; // 11
				bool Motor5Busy :1; // 12
				bool Motor6Busy :1; // 13
				bool Motor7Busy :1; // 14
				bool Motor8Busy :1; // 15
		} bit;
} BoardStatus_t;
// =================================================================
uint16_t BoardStatus ()
{
	ErrorStatus::UpdateBitMap();
	BoardStatus_t boardStatus;
	boardStatus.word = 0;
	boardStatus.bit.ErrorBit = (gRegisters[ErrorStatusBM] != 0);
	boardStatus.bit.HostCommErr = gRegisters[HostCommError] != static_cast<Error>(NoError);
	// Update motor status here for e.g., boardStatus.bit.Motor1Busy = (gSM_Motor1.SM_Status == static_cast<Error>(SM_Busy));
	return boardStatus.word;
}

// =================================================================
struct HostReply
{
		uint16_t Length;
		uint16_t BoardStatus;
		uint8_t Data; // for Read, CRC16 for Write
		uint8_t fillerForCRC16; // so that sizeof(HostReply) = sizeof(Length) + sizeof(BoardStatus) + sizeof(CRC16)
}__attribute__((packed));

// =================================================================
HostCommSM::HostCommSM () :
		StateMachine("HostComm", &HostCommSM_PowerUp, 0, &gRegisters[HostCommError]),
		TimeoutDuration(1000.0), // 1 sec
		UartSM(gSM_SerialComm_0),
		StateNum(0),
		DebugPrintPackets(false),
		ErrorCode(static_cast<Error>(NoError)),
		RequestCommand(HostCommand_Read)
{
}
// =================================================================
Error HostCommSM::ErrorCheck ()
{
	ErrorCode = UartSM.ErrorCode;
	return ErrorCode;
}
// =================================================================
State * HostCommSM::TimeoutCheck ( Error theCode )
{
	if (Timer.IsTimedOut())
	{
		ErrorCode = theCode;
		return &HostCommSM_ErrorOccured;
	}
	return 0;
}
// =================================================================
void HostCommSM::SendReply ( uint16_t theNumBytesToReply ) // UartSM.TransmitBuffer.Data is filled in by the caller
{
	// create the HostReply packet
	HostReply & reply = *(HostReply *) &UartSM.TransmitBuffer;
	reply.Length = theNumBytesToReply + sizeof(HostReply);
	reply.BoardStatus = BoardStatus();
	CRC16 txCRC;
	(void) txCRC.Append(reply.Length - sizeof(uint16_t), (uint8_t *) &reply);
	*(uint16_t*) (&reply.Data + theNumBytesToReply) = txCRC.ToUint16(); // set the CRC, big endian

	if (DebugPrintPackets)
	{
		DebugPrint("reP");
		for (int i = 0; i < reply.Length; i++)
			DebugPrintf(" %02X", (&UartSM.TransmitBuffer)[i]);
		DebugPrint(CRLF);
	}
	// transmit the HostReply
	ASSERT(UartSM.Command == 0);
	UartSM.TransmitBufferLen = reply.Length;
	UartSM.Command = static_cast<int32_t>(SerialCommCmd_Transmit);
}
// =================================================================
SM_POLL (HostCommSM_PowerUp)
{
	return &HostCommSM_Idle;
}
// =================================================================
SM_POLL (HostCommSM_Idle)
{
	if (sm.UartSM.ReceiveBufferLen == 0) // if nothing from the Host
	{
		if (sm.StateNum > 0) // if a packet has started
		    return sm.TimeoutCheck(static_cast<Error>(TimeoutError_HostComm));
		return 0;
	}
	if (sm.ErrorCheck()) return &HostCommSM_ErrorOccured;

	uint8_t chr = sm.UartSM.PopReceiveBuffer(1u);
	if (sm.ErrorCheck()) return &HostCommSM_ErrorOccured;
	(void) sm.RequestCRC.Append(1, &chr);

	switch (sm.StateNum)
	{
		case 0:
			sm.RequestLength.bytes[0] = chr;  // little endian
			sm.Timer.Start(sm.TimeoutDuration);
			break;
		case 1:
			sm.RequestLength.bytes[1] = chr;
			if (sm.RequestLength.word < sm.HostRequestMinSize ||
			sm.RequestLength.word > sm.UartSM.RcvBufSize - 7u)
			{
				sm.ErrorCode = static_cast<Error>(InvalidHostReqLength);
				return &HostCommSM_ErrorOccured;
			}
			break;
		case 2:
			sm.RequestCommand = chr;
			if ((chr != static_cast<uint8_t>(HostCommand_Read)) &&
					(chr != static_cast<uint8_t>(HostCommand_Write)) &&
					(chr != static_cast<uint8_t>(HostCommand_ReadAndClear)))
			{
				sm.ErrorCode = static_cast<Error>(InvalidCommandValue);
				return &HostCommSM_ErrorOccured;
			}
			break;
		case 3:
			sm.RequestAddress.bytes[0] = chr;
			break; // little endian
		case 4:
			sm.RequestAddress.bytes[1] = chr;
			if (sm.RequestAddress.word > static_cast<uint16_t>(LastRegisterAddress))
			{
				sm.ErrorCode = static_cast<Error>(InvalidHostReqAddress);
				return &HostCommSM_ErrorOccured;
			}
			sm.StateNum = 0;
			return &HostCommSM_WaitForCRC;
	}
	sm.StateNum++;
	return &HostCommSM_Idle;
}
// =================================================================
SM_POLL (HostCommSM_WaitForCRC)
{
	if (sm.ErrorCheck()) return &HostCommSM_ErrorOccured;
	if (sm.UartSM.ReceiveBufferLen <
			(static_cast<uint32_t>(sm.RequestLength.word) - static_cast<uint32_t>(sm.HostRequestHeaderSize))) // wait for the rest of the packet
	    return sm.TimeoutCheck(static_cast<Error>(TimeoutError_HostCRC));
	//----------------------------------------------------------------
	// packet received
	int len = sm.RequestLength.word - sm.HostRequestHeaderSize; // num bytes of Data
	uint8_t * data = (uint8_t*) &sm.UartSM.ReceiveBuffer;               // Data
	//----------------------------------------------------------------
	if (sm.DebugPrintPackets)
	{
		DebugPrintf("reQ %02X %02X %02X %02X %02X", sm.RequestLength.bytes[0], sm.RequestLength.bytes[1],
		        sm.RequestCommand, sm.RequestAddress.bytes[0], sm.RequestAddress.bytes[1]);
		for (int i = 0; i < len; i++)
			DebugPrintf(" %02X", data[i]);
		DebugPrint(CRLF);
	}
	//----------------------------------------------------------------
	// Update our CRC with the HostRequest.Data and CRC16
	(void) sm.RequestCRC.Append(len, data);
	if (!sm.RequestCRC.IsValid()) // if CRC mismatch
	{
		sm.ErrorCode = static_cast<Error>(InvalidHostReqCRC);
		return &HostCommSM_ErrorOccured;
	}
	//----------------------------------------------------------------
	// a valid HostRequest was received, UartSM.ReceiveBuffer starts with first byte of Data
	HostReply & reply = *(HostReply *) &sm.UartSM.TransmitBuffer;
	uint16_t numBytesToReply = 0;
	uint8_t * requestedData = (uint8_t *) &gRegisters[(RegisterIds) sm.RequestAddress.word];
	reply.BoardStatus = BoardStatus();
	switch (sm.RequestCommand)
	{
		//----------------------------------------------------------------
		case HostCommand_ReadAndClear:  // fall through
		case HostCommand_Read:  // copy K70 Registers to Host.TransmitBuffer
			SignalStatus_Update(); // TBD only needed if the ErrorStatus Registers are being read
			numBytesToReply = *(uint16_t *) &sm.UartSM.ReceiveBuffer; // little endian
			if ((numBytesToReply + sizeof(HostReply)) > sm.UartSM.XmtBufSize) // if reply doesn't fit in the TransmitBuffer
			{
				sm.ErrorCode = static_cast<Error>(InvalidNumOfBytesToReply);
				return &HostCommSM_ErrorOccured;
			}
			if (sm.RequestAddress.word + numBytesToReply - 1u > static_cast<uint16_t>(LastRegisterAddress)) // if requested memory is outside of gRegisters
			{
				sm.ErrorCode = static_cast<Error>(InvalidHostReqLengthnAddress);
				return &HostCommSM_ErrorOccured;
			}
			memcpy(&reply.Data, requestedData, numBytesToReply);
			if (sm.RequestCommand == static_cast<uint8_t>(HostCommand_ReadAndClear))
			    memset(requestedData, 0, numBytesToReply);
			break;
			//----------------------------------------------------------------
		case HostCommand_Write: // copy HostRequest.Data to K70 Registers
			int dataLen = sm.RequestLength.word - sm.HostRequestMinSize;
			if ((sm.RequestAddress.word + dataLen - 1u) > static_cast<uint16_t>(LastRegisterAddress)) // if requested memory is outside of gRegisters
			{
				sm.ErrorCode = static_cast<Error>(InvalidHostReqLengthnAddress);
				return &HostCommSM_ErrorOccured;
			}
			// if there is an SM with a Host Command at gRegisters[RequestAddress], check it for busy or error
			// if busy, allow the Abort command
			if (StateMachine::SmIsBusyOrError((RegisterIds) sm.RequestAddress.word,
			        *(int32_t *) &sm.UartSM.ReceiveBuffer))
			{
				sm.ErrorCode = static_cast<Error>(HostReqPeripheralError);
				return &HostCommSM_ErrorOccured;
			}
			memcpy(requestedData, (void*) &sm.UartSM.ReceiveBuffer, dataLen);
			break;
	} // end switch (sm.RequestCommand)
	  //----------------------------------------------------------------
	sm.SendReply(numBytesToReply);
	return &HostCommSM_WaitForTransmit;
}
// =================================================================
SM_POLL (HostCommSM_WaitForTransmit)
{ // wait for transmit to start
	if (sm.UartSM.Command != 0) // if UartSM did not ack the cmd
	    return sm.TimeoutCheck(static_cast<Error>(TimeoutError_HostTransmit));
	if (sm.ErrorCheck()) return &HostCommSM_ErrorOccured;

	// Ack the RecieverBuffer
	sm.UartSM.Command = static_cast<int32_t>(SerialCommCmd_AckTheRcvBuf);
	sm.UartSM.CommandParam = sm.RequestLength.word - sm.HostRequestHeaderSize;
	return &HostCommSM_WaitForRcvAck;
}
// =================================================================
SM_POLL (HostCommSM_WaitForRcvAck)
{ // wait for Ack to complete
	if (sm.UartSM.Command != 0) // if UartSM did not ack the cmd
	    return sm.TimeoutCheck(static_cast<Error>(TimeoutError_HostRcvAck));
	if (sm.ErrorCheck()) return &HostCommSM_ErrorOccured;

	sm.RequestCRC.Reset();
	gRegisters[HostCommError] = static_cast<Error>(NoError);
	sm.ErrorCode = static_cast<Error>(NoError);
	return &HostCommSM_Idle;
}
// =================================================================
SM_POLL (HostCommSM_ErrorOccured)
{
	if (sm.UartSM.Command != 0) // wait for prev cmd to finish
	    return 0;

	sm.StateNum = 0;
	sm.RequestCRC.Reset();
	DebugPrintf("HostCommSM error: 0x%X" CRLF, sm.ErrorCode);
	gRegisters[HostCommError] = sm.ErrorCode; // used by BoardStatus()
	sm.SendReply(0);
	return &HostCommSM_ErrorWaitForTransmit;
}
// =================================================================
SM_POLL (HostCommSM_ErrorWaitForTransmit)
{ // wait for transmit to start
	if (sm.UartSM.Command != 0) // if UartSM ack'ed the cmd
	    return 0;

	// Flush the rcv buf
	sm.UartSM.Command = static_cast<int32_t>(SerialCommCmd_FlushRcvBuf);
	return &HostCommSM_ErrorWaitForFlush;
}
// =================================================================
SM_POLL (HostCommSM_ErrorWaitForFlush)
{ // wait for Flush to complete
	if (sm.UartSM.Command != 0) // if UartSM did not ack the cmd
	    return 0;
	return &HostCommSM_Idle;
}
