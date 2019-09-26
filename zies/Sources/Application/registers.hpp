// =================================================================
// registers.hpp
// Header defines all host communication interface registers. Command
// enumeration and structures.
// =================================================================
#ifndef REGISTERS_HPP_
#define REGISTERS_HPP_

#include "common.hpp"

//================================================================
enum CommonCommandCodes
{ // for StateMachine
	CommandCodeAbort = -1
};

//================================================================
enum SerialCommCommands
{
	SerialCommCmd_Transmit = 1u, SerialCommCmd_AckTheRcvBuf = 2u, SerialCommCmd_FlushRcvBuf = 3u
};

#define SerialComm_REGS(id, rcvBufLen, xmtBufLen) typedef volatile struct SerialComm_REGS ## id ## _type { \
	int32_t   Command; /* 1=transmit the TransmitBuffer, 2=ack the ReceiveBuffer*/ \
	uint32_t  CommandParam; \
	uint32_t  ErrorCode; \
	uint32_t  ReceiveBufferLen; \
	uint32_t  TransmitBufferLen; \
	uint8_t   TransmitBuffer [ xmtBufLen ]; \
	uint32_t  TerminationChar; \
	uint8_t   ReceiveBuffer [ rcvBufLen ]; \
} SerialComm_REGS ## id;
//================================================================
enum MotorCommands
{
	MotorCmd_Initialize = 1,
	MotorCmd_GotoHome = 2,
	MotorCmd_MarkPositionAsZero = 3,
	MotorCmd_GotoPosition = 4,
	MotorCmd_MoveRelative = 5,
	MotorCmd_EnableHolding = 6, // CommandParam: 0/1 = disable/enable, holding current= off/on
	MotorCmd_FindMaxTravel = 7,
	MotorCmd_Stop = CommandCodeAbort // CommandParam: 0/1 = soft/hard
};

enum MotorDirection
{
	Reverse = 0,
	Forward = 1
};

enum MotorEnabling
{
	DisableMotor = 0,
	EnableMotor = 1
};

typedef struct MotorRegisters_type
{
	int32_t Command; // MotorCommands
	int32_t CommandParam1;
	int32_t CommandParam2;
	uint32_t ErrorCode;
	int32_t Position; // Current position
	uint32_t HomedStatus;

	uint32_t unused0[4]; // reserved for future use

	uint32_t HomeDirection;      // 0=Reverse, 1=Forward
	uint32_t MotorFullStepsPerRev; // full step counts / rev
	uint32_t UnitsPerRev; // number of U in one motor revolution
	uint32_t GearheadRatio;
	uint32_t unused1; // Encoder counts/rev for future purpose
	uint32_t Deadband;
	uint32_t InvertedDirection;  // 0=normal, 1=motor circuit is wired in reverse
	uint32_t StepSize;           // FullStep divisor, 1=full, 2=1/2 .. 128=1/128
	uint32_t Acceleration;
	uint32_t DeAcceleration;
	uint32_t MaxSpeed;
	uint32_t MinSpeed;
	uint32_t EncoderTicksPerRev;
	uint32_t unused2[3]; //  reserved for future use

	uint32_t MaxMoveRetries;
	uint32_t MinMaxTravelPosition; // axis limit in encoder ticks

	uint32_t unused3[20]; //  reserved for future use
} MotorRegisters;

//================================================================

enum RegisterIds
{
	SwVersion = 0, // 1..n
	ResetReason = 4, // Reset reason
	ErrorStatusBM = 8, // bitmask identifying all non-zero ErrorCodes
	HostCommError = 12, // ErrorCode from HostComm
	Signals = 16, // bitmask of the states of various HW signals
	MonitorVoltageBitMap = 20, // bitmask of measurements that are out of limits
	MonitorVoltageMeasurements = 24, // System monitor voltages [0..31] 128 bytes long
	UART0Regs = 152, // Serial Comm Registers, 1048 bytes long with 512 Bytes each for TX & Rx buffers
	// Motor registers, 192 bytes long
	Motor1Regs = 1200,
	Motor2Regs = 1392,
	Motor3Regs = 1584,
	Motor4Regs = 1776,
	//
	LastRegisterAddress = 1968,
	RegistersSize = 8192
};
//================================================================
class Registers
{
	public:
		Registers ();
		uint32_t & operator[] ( RegisterIds theId ) const;
};
//================================================================
extern Registers gRegisters;

#endif /* REGISTERS_HPP_ */
