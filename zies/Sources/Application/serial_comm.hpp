#ifndef SERIAL_COMM_HPP_
#define SERIAL_COMM_HPP_
#include "state_machine.hpp"
// =================================================================
typedef union
{ // SerialComm_TError, must match the PE UARTx_TError typedef
		uint8_t err;
		struct
		{
				bool OverRun :1;  // Overrun error flag
				bool Framing :1;  // Framing error flag
				bool Parity :1;  // Parity error flag
				bool RxBufOvf :1;  // Rx buffer full error flag
				bool Noise :1;  // Noise error flag
				bool Break :1;  // Break detect
				bool LINSync :1;  // device is not connected
				bool BitError :1;  // not used
		} errName;
} SerialComm_TError;

typedef uint8_t (*PE_GetErrorFuncType) ( SerialComm_TError * ); // kludge the signature, PE uses unique _TError typdefs for each uart, but ours are all the same.
// =================================================================
typedef struct
{ // Interface routines to the PE SerialComm driver
		uint8_t (*ClearRxBuf) ( void );
		uint8_t (*ClearTxBuf) ( void );
		uint8_t (*RecvChar) ( uint8_t * Chr );
		uint8_t (*SendBlock) ( uint8_t * Ptr, uint16_t Size, uint16_t * Snd );
		uint16_t (*GetCharsInRxBuf) ( void );
		uint8_t (*GetError) ( SerialComm_TError * Err );
		uint8_t (*GetBreak) ( bool * Brk );
		void (*Init) ( void );
		uint8_t (*Enable) ( void );
		uint8_t (*Disable) ( void );
} PE_SerialComm_Interface;

// =================================================================
class SerialCommController : public StateMachine
{
	public:
		const uint32_t Id;
		const uint32_t RcvBufSize;            // size of the users .ReceiveBuffer
		const uint32_t XmtBufSize;            // size of the users .TransmitBuffer
		const uint32_t DefaultTermChar;
		const PE_SerialComm_Interface & Interface;

		volatile uint32_t NumCharsReceived;
		volatile int32_t & Command;          // SerialCommCommands
		volatile uint32_t & CommandParam;
		volatile uint32_t & ErrorCode;
		volatile uint32_t & TerminationChar;  // end-of-packet char, 0xFFFFFFFF == streaming mode
		volatile uint32_t & ReceiveBufferLen;
		volatile uint8_t & ReceiveBuffer;    // [RcvBufSize]
		volatile uint32_t & TransmitBufferLen;
		volatile uint8_t & TransmitBuffer;   // [XmtBufSize]
		Mutex RcvIsrLock;
		LDD_TDeviceData * PE_data;

		SerialCommController ( const char * theName, int theId, PE_SerialComm_Interface & theInterface,
		        int theRcvBufLen, int theXmtBufLen, int theTerminationCharDefault,
		        volatile int32_t & theCommand, volatile uint32_t & theCommandParam, volatile uint32_t & theErrorCode,
		        volatile uint32_t & theTerminationChar, volatile uint32_t & theReceiveBufferLen,
		        volatile uint8_t & theReceiveBuffer, volatile uint32_t & theTransmitBufferLen,
		        volatile uint8_t & theTransmitBuffer );

		void PowerUp ();
		uint8_t PopReceiveBuffer ( int theNumChars ); // returns the first byte in the rcv buf
		void UpdateStatus ();
		void OnTxCompleteISR ();
		void OnRxCharISR ();
		void DisableRxTxErrIRQ ();
		void EnableRxTxErrIRQ ();
};
// =================================================================
extern SerialCommController gSM_SerialComm_0;
#endif /* SERIAL_COMM_HPP_ */
