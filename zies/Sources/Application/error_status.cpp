#include "error_status.hpp"
#include "registers.hpp"
#include "debug.hpp"

// =================================================================
struct ErrorStatusDef
{
		const char * Name;
		Error * ErrorCode;
};

ErrorStatusDef ErrorStatusCodes[64u] = { //{ "SystemVoltages", static_cast<Error*>(NULL) },       // 0
        { "Not Used", static_cast<Error*>(NULL) },       // 1
        { " ", static_cast<Error*>(NULL) },     // 2
        { " ", static_cast<Error*>(NULL) },     // 3
        { " ", static_cast<Error*>(NULL) },     // 4
        { " ", static_cast<Error*>(NULL) },     // 5
        { " ", static_cast<Error*>(NULL) },     // 6
        { " ", static_cast<Error*>(NULL) },     // 7
        { " ", static_cast<Error*>(NULL) },     // 8
        { " ", static_cast<Error*>(NULL) },     // 9
        { " ", static_cast<Error*>(NULL) },     // 10
        { " ", static_cast<Error*>(NULL) },     // 11
        { " ", static_cast<Error*>(NULL) },     // 12
        { " ", static_cast<Error*>(NULL) },     // 13
        { " ", static_cast<Error*>(NULL) },     // 14
        { " ", static_cast<Error*>(NULL) },     // 15
        { " ", static_cast<Error*>(NULL) },     // 16
        { " ", static_cast<Error*>(NULL) },     // 17
        { " ", static_cast<Error*>(NULL) },     // 18
        { " ", static_cast<Error*>(NULL) },     // 19
        { " ", static_cast<Error*>(NULL) },     // 20
        { " ", static_cast<Error*>(NULL) },     // 21
        { " ", static_cast<Error*>(NULL) },     // 22
        { " ", static_cast<Error*>(NULL) },     // 23
        { " ", static_cast<Error*>(NULL) },     // 24
        { " ", static_cast<Error*>(NULL) },     // 25
        { " ", static_cast<Error*>(NULL) },     // 26
        { " ", static_cast<Error*>(NULL) },     // 27
        { " ", static_cast<Error*>(NULL) },     // 28
        { " ", static_cast<Error*>(NULL) },     // 29
        { " ", static_cast<Error*>(NULL) },     // 30
        { " ", static_cast<Error*>(NULL) }      // 31
};

// =================================================================
void ErrorStatus::PowerUp ()
{

}
// =================================================================
void ErrorStatus::UpdateBitMap ()
{
	uint32_t bitmap = 0;
	// bit 0
	if (gRegisters[MonitorVoltageBitMap] != 0) // if Monitored Voltages has errors
	    bitmap |= 1;

	// bits 1..31
	for (uint32_t bitIndex = 3, mask = 8; bitIndex < 32; bitIndex++, mask <<= 1)
		if (ErrorStatusCodes[bitIndex].ErrorCode != NULL)
		    if ((*ErrorStatusCodes[bitIndex].ErrorCode & ~(static_cast<uint32_t>(SM_Busy))) != 0) bitmap |= mask;
	gRegisters[ErrorStatusBM] = bitmap;
}
// =================================================================
void ErrorStatus::PrintAll ()
{
	DebugPrintf("  ErrorCodes bitmap= 0x%08X" CRLF, gRegisters[ErrorStatusBM]);
	for (ErrorStatusDef * def = ErrorStatusCodes; def->ErrorCode; def++)
		DebugPrintf("  0x%8X  %s" CRLF, *def->ErrorCode, def->Name);
}
// =================================================================

