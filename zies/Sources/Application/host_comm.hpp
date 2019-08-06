#ifndef HOSTCCOMM_HPP_
#define HOSTCCOMM_HPP_

#include "serial_comm.hpp"

// =================================================================
enum HostCommands
{
	HostCommand_Read = 1,
	HostCommand_Write = 2,
	HostCommand_ReadAndClear = 3,
};
// =================================================================
class HostCommSM : public StateMachine
{
	public:
		Error ErrorCode;
		const Time TimeoutDuration;
		TimeoutTimer Timer;
		SerialCommController & UartSM;
		int StateNum;
		bool DebugPrintPackets;
		// current HostRequest
		UnionUint16 RequestLength;
		uint8_t RequestCommand;
		UnionUint16 RequestAddress;
		CRC16 RequestCRC;

		// sizeof(RequestLength + RequestCommand + RequestAddress);
		const uint8_t HostRequestHeaderSize = sizeof(UnionUint16) + sizeof(uint8_t) + sizeof(UnionUint16);

		// sizeof(RequestLength + RequestCommand + RequestAddress + CRC16);
		const uint8_t HostRequestMinSize = sizeof(UnionUint16) + sizeof(uint8_t) + sizeof(UnionUint16) + sizeof(CRC16);

		HostCommSM ();
		Error ErrorCheck ();
		State * TimeoutCheck ( Error theCode );
		void SendReply ( uint16_t theNumBytesToReply );
};
// =================================================================
extern HostCommSM gSM_HostComm;

#endif /* HOSTCCOMM_HPP_ */
