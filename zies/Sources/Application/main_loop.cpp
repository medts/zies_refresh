#include "main_loop.hpp"
#include "registers.hpp"
#include "error_status.hpp"
#include "debug.hpp"

extern "C"
{
#include "HEARTBEAT_LED.h"
} // end extern "C"
// =================================================================
// global vars
LoopStats gLoopStats;

// =================================================================
class Heartbeat
{
		const Time Period;
		bool State;
		TimeoutTimer Timer;
	public:
		//----------------------------------------------------------------
		Heartbeat () :
				Period(1000.0), // 1 sec
				State(false)
		{
			Timer.Start(Period);
		}
		//----------------------------------------------------------------
		bool Occured ()
		{
			if (!Timer.IsTimedOut()) return false;
			State = !State;
			HEARTBEAT_LED_PutVal(0, State);
			Timer.Start(Period);
			return true;
		}
};
// =================================================================
extern "C" void MainLoop () // called from main() in PE main.c
{
	gRegisters[SwVersion] = FW_VERSION;
	gRegisters[ResetReason] = (uint32_t) ReadResetReason();

	DebugPrintf(CRLF "SwVersion= %08X, ResetReason= %04X, ", gRegisters[SwVersion], gRegisters[ResetReason]);
	ErrorStatus::PowerUp();
	gLoopStats.PowerUp();
	Heartbeat heartbeat;
	while (true)
	{
		StateMachine::PollAll(); // poll the state machines
		gLoopStats.EndLoop();
	} // end while (true)
}
