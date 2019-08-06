#include "serial_comm.hpp"
#include "registers.hpp"
#include "debug.hpp"
#include "string.h"
extern "C"
{
#include "UART0.h"
}  // end extern "C"

// =================================================================
#define SerialCommSM(id, rcvLen, xmtLen, termChar) \
  inline uint8_t UART ## id ## _GetErrorSM (SerialComm_TError * Err) { return UART ## id ## _GetError ((UART ## id ## _TError *)Err); } \
  PE_SerialComm_Interface UART_Interface ## id = { /* PE functions */ \
    UART ## id ## _ClearRxBuf, \
    UART ## id ## _ClearTxBuf, \
    UART ## id ## _RecvChar, \
    UART ## id ## _SendBlock, \
    UART ## id ## _GetCharsInRxBuf, \
    UART ## id ## _GetErrorSM, \
    UART ## id ## _GetBreak, \
    UART ## id ## _Init, \
    UART ## id ## _Enable, \
    UART ## id ## _Disable, \
  }; \
  SerialComm_REGS (id, rcvLen , xmtLen ) /* creates the map of the user regs */ \
  \
  SerialCommController  gSM_SerialComm_ ## id ("UART" #id , id , UART_Interface ## id , rcvLen, xmtLen, termChar, \
      ((SerialComm_REGS ## id *)&( gRegisters [UART ## id ## Regs]))->Command, /* constructor selects the peices of that map that it needs */ \
      ((SerialComm_REGS ## id *)&( gRegisters [UART ## id ## Regs]))->CommandParam, \
      ((SerialComm_REGS ## id *)&( gRegisters [UART ## id ## Regs]))->ErrorCode, \
      ((SerialComm_REGS ## id *)&( gRegisters [UART ## id ## Regs]))->TerminationChar, \
      ((SerialComm_REGS ## id *)&( gRegisters [UART ## id ## Regs]))->ReceiveBufferLen, \
      ((SerialComm_REGS ## id *)&( gRegisters [UART ## id ## Regs]))->ReceiveBuffer [0], \
      ((SerialComm_REGS ## id *)&( gRegisters [UART ## id ## Regs]))->TransmitBufferLen, \
      ((SerialComm_REGS ## id *)&( gRegisters [UART ## id ## Regs]))->TransmitBuffer [0] \
    ); \
  extern "C" { /* hook the PE ISRs to our ISRs */ \
    void UART ## id ## _OnTxCompleteISR ()      { gSM_SerialComm_ ## id .OnTxCompleteISR (); } \
    void UART ## id ## _OnRxCharISR ()          { gSM_SerialComm_ ## id .OnRxCharISR (); } \
    void UART ## id ## _OnBreakISR ()           { gSM_SerialComm_ ## id .UpdateStatus (); } \
    void UART ## id ## _OnErrorISR ()           { gSM_SerialComm_ ## id .UpdateStatus (); } \
    void UART ## id ## _OnFullRxBufISR ()       { gSM_SerialComm_ ## id .UpdateStatus (); } \
  } // end extern "C"

//          Buf size
//      id, rcv, xmt, EOL char (or streaming-mode (0xFFFFFFFF))
SerialCommSM(0, 512, 512, 0xFFFFFFFF);// creates gSM_SerialComm_0 (host)

// local vars
// State Machine ===================================================
SM_STATE(SerialCommSM_PowerUp)
SM_STATE(SerialCommSM_Idle)
#define sm (*(SerialCommController *)theSM)

// =================================================================
SerialCommController::SerialCommController ( const char * theName, int theId, PE_SerialComm_Interface & theInterface,
        int theRcvBufLen, int theXmtBufLen, int theTerminationCharDefault,
        volatile int32_t & theCommand, volatile uint32_t & theCommandParam, volatile uint32_t & theErrorCode,
        volatile uint32_t & theTerminationChar, volatile uint32_t & theReceiveBufferLen,
        volatile uint8_t & theReceiveBuffer, volatile uint32_t & theTransmitBufferLen,
        volatile uint8_t & theTransmitBuffer ) :
		StateMachine(theName, &SerialCommSM_PowerUp, (int32_t*) &theCommand, (Error*) &theErrorCode), Id(theId), Interface(
		        theInterface), DefaultTermChar(theTerminationCharDefault), NumCharsReceived(0), RcvBufSize(
		        theRcvBufLen), XmtBufSize(theXmtBufLen), Command(theCommand), CommandParam(
		        theCommandParam), ErrorCode(theErrorCode), TerminationChar(theTerminationChar), ReceiveBufferLen(
		        theReceiveBufferLen), ReceiveBuffer(theReceiveBuffer), TransmitBufferLen(theTransmitBufferLen), TransmitBuffer(
		        theTransmitBuffer)
{
}
// =================================================================
void SerialCommController::PowerUp ()
{
	TerminationChar = DefaultTermChar;
	ErrorCode = NoError;
	ReceiveBufferLen = 0;
	TransmitBufferLen = 0;
	NumCharsReceived = 0;
	Interface.Enable();
	Interface.ClearRxBuf();
	Interface.ClearTxBuf();
	UpdateStatus();
}
// =================================================================
void SerialCommController::UpdateStatus ()
{  // Must be reentrant
	SerialComm_TError err;
	Interface.GetError(&err);  // get PE UART error
	//----------------------------------------------------------------
	bool breakErr;
	Interface.GetBreak(&breakErr);
	if (breakErr) err.errName.Break = 1;
	//----------------------------------------------------------------
	if (err.err == 0) return;
	ErrorCode |= HwError_SerialCommController | (Id << 8) | (err.err & 0xFF); // accumulate err bits, reset by new Command
}
// =================================================================
void SerialCommController::OnTxCompleteISR ()
{
	TransmitBufferLen = 0;
	UpdateStatus();
}
// =================================================================
void SerialCommController::DisableRxTxErrIRQ ()
{
	DisableIRQ((IRQInterruptIndex) ((uint32_t) INT_UART0_RX_TX + (Id * 2u)));
	DisableIRQ((IRQInterruptIndex) ((uint32_t) INT_UART0_ERR + (Id * 2u)));
}
// =================================================================
void SerialCommController::EnableRxTxErrIRQ ()
{
	EnableIRQ((IRQInterruptIndex) ((uint32_t) INT_UART0_RX_TX + (Id * 2u)));
	EnableIRQ((IRQInterruptIndex) ((uint32_t) INT_UART0_ERR + (Id * 2u)));
}
// =================================================================
void SerialCommController::OnRxCharISR ()
{
	UpdateStatus();  // get errors before RecvChar throws them away
	if ((ReceiveBufferLen != 0 && TerminationChar != 0xFFFFFFFF))  // if  buffer is busy
	    return;
	do  // until TerminationChar rcvd or PE rcv buf is empty
	{
		if (NumCharsReceived >= RcvBufSize)
		{
			ErrorCode = CommandFailed | 5;  // SerialComm .ReceiveBuffer too small
			ReceiveBufferLen = RcvBufSize;  // inform the user a partial packet is available
			break;
		}
		uint8_t chr;
		if (Interface.RecvChar(&chr) == ERR_RXEMPTY)  // if no char available
		    break;
		((uint8_t *) &ReceiveBuffer)[NumCharsReceived++] = chr;
		if ((uint32_t) chr == TerminationChar)
		{
			ReceiveBufferLen = NumCharsReceived;  // inform the user a packet is available
			break;
		}
		if (TerminationChar == 0xFFFFFFFF)  // keep streaming buf len updated
		    ReceiveBufferLen = NumCharsReceived;

	} while (true);
}
// =================================================================
uint8_t SerialCommController::PopReceiveBuffer ( int theNumChars )  // returns the first byte in the rcv buf
{
	uint8_t firstChr = ReceiveBuffer;
	DisableRxTxErrIRQ();
	NumCharsReceived -= theNumChars;
	ReceiveBufferLen -= theNumChars;
	memmove((void*) &ReceiveBuffer, (void*) (&ReceiveBuffer + theNumChars), NumCharsReceived); // shift remaining chars to start of buf
	EnableRxTxErrIRQ();
	return firstChr;
}
// =================================================================
SM_POLL (SerialCommSM_PowerUp)
{
	sm.PowerUp();
	return &SerialCommSM_Idle;
}
// =================================================================
SM_POLL (SerialCommSM_Idle)
{  // Check for a request from the user
	if (sm.Command == 0)
	{
		if (sm.Interface.GetCharsInRxBuf() > 0)  // if char waiting in PE rcv buf
		    sm.OnRxCharISR();  // try to process it
		return 0;
	}
	sm.ErrorCode = NoError;

	switch (sm.Command)
	{  //----------------------------------------------------------------
		case SerialCommCmd_Transmit:
			uint16_t numSent;
			if (sm.TransmitBufferLen > sm.XmtBufSize ||
			sm.Interface.SendBlock((uint8_t *) &sm.TransmitBuffer, sm.TransmitBufferLen, &numSent) != NoError)
			{
				sm.ErrorCode = CommandFailed | 4;  // SerialComm TransmitBufferLen too large
				sm.TransmitBufferLen = 0;  // ready for next xmit data
				sm.Interface.ClearTxBuf();
				break;
			}
			ASSERT(sm.TransmitBufferLen == numSent)
			;
			sm.UpdateStatus();
			break;
			//----------------------------------------------------------------
		case SerialCommCmd_AckTheRcvBuf: // user is acking the data in the ReceiveBuffer. Now ok to report the next packet.
		{
			int numBytesAcked = sm.ReceiveBufferLen;
			if (sm.TerminationChar == 0xFFFFFFFF)  // if streaming mode
			    numBytesAcked = (&sm.Command)[1u];  // CommandParam
			ASSERT((numBytesAcked >= 0) && (static_cast<uint32_t>(numBytesAcked) <= sm.NumCharsReceived));
			sm.PopReceiveBuffer(numBytesAcked);
			break;
		}
			//----------------------------------------------------------------
		case SerialCommCmd_FlushRcvBuf:
			sm.DisableRxTxErrIRQ();
			uint8_t chr;
			while (sm.Interface.RecvChar(&chr) != ERR_RXEMPTY)
				;
			sm.NumCharsReceived = 0;
			sm.ReceiveBufferLen = 0;
			sm.EnableRxTxErrIRQ();
			break;
			//----------------------------------------------------------------
		default:
			sm.ErrorCode = CommandFailed;  // an invalid Command value was received from the user
			break;
	}
	sm.Command = 0;  // Ack the request back to the user
	return 0;
}
