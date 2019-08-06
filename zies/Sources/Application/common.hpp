// =================================================================
// common.hpp
// Common header for generic definitions and error codes
// =================================================================

#ifndef COMMON_HPP_
#define COMMON_HPP_

extern "C"
{
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "Cpu.h"
}


// MAJOR VERSION 1x | MINOR VERSION 1x | BUILD VERSION 2x
const uint8_t FW_VERSION = 0x00000001u;

//-------------------
inline void DisableInterrupts ()
{
	EnterCritical();
} // = __DI () + counter; }
inline void EnableInterrupts ()
{
	ExitCritical();
}  // = __EI () + counter; }

inline void SoftReset ()
{
	// Ensure all outstanding memory accesses included buffered write are completed before reset
	__asm volatile ("dsb");
	SCB_AIRCR = (SCB_AIRCR_VECTKEY (0x05FAu) | SCB_AIRCR_SYSRESETREQ_MASK);
	__asm volatile ("dsb");
	while (1)
		;
}

typedef enum
{
	TamperReset = 0x8000u,
	StopAckErrorReset = 0x2000u,
	EzPortReset = 0x1000u,
	MDM_AP_Reset = 0x0800u,
	SoftwareReset = 0x0400u,
	CoreLockupReset = 0x0200u,
	JTAG_Reset = 0x0100u,
	PowerOnReset = 0x0080u,
	ExcternalPinReset = 0x0040u,
	WatchdogReset = 0x0020u,
	LossOfClockReset = 0x0004u,
	LowVololtageReset = 0x0002u,
	LowVololtageWakeup = 0x0001u
} ResetSource;

inline ResetSource GetResetSource ( void )
{
	return (ResetSource) (((RCM_SRS1 & 0x00FFu) << 8u) | (RCM_SRS0 & 0x00FFu));
}

#define NON_CORE_VECTOR_OFFSET     16u

inline void DisableIRQ ( IRQInterruptIndex IRQ_Index )
{
	NVIC_BASE_PTR->ICER[(IRQ_Index - NON_CORE_VECTOR_OFFSET) / 32u] = (0x00000001u << ((IRQ_Index - NON_CORE_VECTOR_OFFSET) % 32u));
}
inline void EnableIRQ ( IRQInterruptIndex IRQ_Index )
{
	NVIC_BASE_PTR->ISER[(IRQ_Index - NON_CORE_VECTOR_OFFSET) / 32u] = (0x00000001u << ((IRQ_Index - NON_CORE_VECTOR_OFFSET) % 32u));
}

inline ResetSource ReadResetReason ( void )
{
	return (ResetSource) (RCM_BASE_PTR->SRS0 + ((RCM_BASE_PTR->SRS1 & 0x00FFu) << sizeof(uint8_t)));
}

#define CR "\r"
#define CRLF "\r\n"
#define UEP_MAKESTRING2(x) #x
#define UEP_MAKESTRING(x) UEP_MAKESTRING2(x)
#define UEP_FILELINE  " " __FILE__ ":" UEP_MAKESTRING( __LINE__ )

extern "C" void CRASH ( int theBlinkCount, const char * theDescription );
#define ASSERT(expression) { if (!(expression)) CRASH (3, "ASSERT failed:" UEP_FILELINE); }
//================================================================
union UnionUint16
{
	public:
		uint16_t word;
		uint8_t bytes[2];

		inline UnionUint16 ()
		{
		}
		inline UnionUint16 ( uint16_t theValue ) :
				word(theValue)
		{
		}
};
//================================================================
union UnionUint32
{
	public:
		uint32_t dword;
		uint8_t bytes[4];
		uint16_t words[2];

		inline UnionUint32 ()
		{
		}
		inline UnionUint32 ( uint32_t theValue ) :
				dword(theValue)
		{
		}
};
//================================================================
template<typename Type>
Type max ( Type a, Type b )
{
	return a > b ? a : b;
}
//================================================================
template<typename Type>
Type min ( Type a, Type b )
{
	return a < b ? a : b;
}
//================================================================
extern uint16_t ReverseEndian16 ( uint16_t theVal );
extern uint32_t ReverseEndian32 ( uint32_t theVal );
inline uint16_t htons ( uint16_t val )
{
	return ReverseEndian16(val);
}
;
inline uint16_t ntohs ( uint16_t val )
{
	return ReverseEndian16(val);
}
;
inline uint32_t htonl ( uint32_t val )
{
	return ReverseEndian32(val);
}
;
inline uint32_t ntohl ( uint32_t val )
{
	return ReverseEndian32(val);
}
;

typedef bool (*BooleanFunc) ();
typedef bool (*PE_GetValFuncType) ( LDD_TDeviceData * ); // PE signature for its BitIO:xxx_GetVal functions
typedef void (*PE_PutValFuncType) ( LDD_TDeviceData *, bool Val ); // PE signature for its BitIO:xxx_PutVal functions

// =================================================================
class Mutex
{
	public:
		Mutex () :
				Flag(false)
		{
		}
		;
		bool Lock (); // return false when already locked; true otherwise
		inline void Unlock ()
		{
			Flag = false;
		}
	private:
		bool Flag;
};
// =================================================================
class CRC16
{ // This algorithm implements the CRC-CCITT modified to detect erroneous leading and trailing zeros.
// Its 16 bit generator polynomial (X16 + X12 + X5 + 1) is designed for a maximum of 215 - 1 bits, or 4096 bytes.
// Larger amounts of data will invalidate some of its expected error detection properties.
// A CRC is valid for data if 0x1D0F == (Append (data).Append (CRC.MSB).Append (CRC.LSB)).ToUint16()
// e.g.  msg = data + Append (data).ToUint16 ()     // created by sender (data concatenated with CRC)
//       valid = Append (msg).ToUint16 () == 0x1D0F // validated by receiver
// Do not use virtuals here, sizeof(CRC16) must be sizeof(uint16).
	public:
		uint8_t MSB;
		uint8_t LSB;

		CRC16 () :
				MSB(0xFF), LSB(0xFF)
		{
		}
		CRC16 & Append ( int theLength, uint8_t * theData );
		void Reset ()
		{
			MSB = 0xFF;
			LSB = 0xFF;
		}
		uint16_t ToUint16 ()
		{
			return htons((uint16_t) (~MSB << 8 | (~LSB & 0xFF)));
		} // big endian
		bool IsValid ()
		{
			return (MSB == 0x1D) && (LSB == 0x0F);
		}
};
//================================================================
typedef uint32_t Error;

// bitmask; reserve lower 16 bits for error specific values
enum ErrorCodes
{
	NoError = 0,
	SM_Busy = 0x00010000,
	HwError = 0x00020000,
		HwError_SerialCommController = HwError | 0x0001, // SerialComm hardware error
	TimeoutError = 0x00040000,
		TimeoutError_HostComm = TimeoutError | 0x0001, // Host comm packet didn't complete, timeout error
		TimeoutError_HostCRC = TimeoutError | 0x0002, // Host comm CRC not received completely
		TimeoutError_HostTransmit = TimeoutError | 0x0003, // Host comm transmit command timedout
		TimeoutError_HostRcvAck = TimeoutError | 0x0004, // Host comm Ack from Serial comm SM timedout
		TimeoutError_MotorDrv8825 = TimeoutError | 0x0005, // Motor driver timeout error
	CommandFailed = 0x00080000,
		InvalidCommandValue = CommandFailed  | 0x0001, // An invalid command value was received
		SerialTxBuffLenTooLarge = CommandFailed  | 0x0002, // Serial communication buffer length too large
		SerialRxBuffTooSmall = CommandFailed  | 0x0003, // Serial receive buffer received too few characters (terminated before receiving the length info)
		InvalidHostReqLength = CommandFailed  | 0x0004, //
		InvalidHostReqAddress = CommandFailed  | 0x0005, //
		InvalidHostReqCRC = CommandFailed  | 0x0006, //
		InvalidHostReqBytesToRead = CommandFailed  | 0x0007, //
		InvalidHostReqLengthnAddress = CommandFailed  | 0x0008, //
		InvalidNumOfBytesToReply = CommandFailed  | 0x0009, //
		InvalidCommandParamValue = CommandFailed  | 0x000A, // Command parameter value received from Host is invalid
		HostReqPeripheralError = CommandFailed  | 0x000B, // Peripheral SM that host is trying to communicate is busy or in error
		CommandAborted = CommandFailed  | 0x000C, // Command aborted by the host

		MotorPositionError = CommandFailed  | 0x0100, // Motor move command failed to move within position deadband limits
		MotorEncoderError = CommandFailed  | 0x0101, // Motor encoder did not change during coarse moving

};

#endif /* COMMON_HPP_ */

