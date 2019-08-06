#ifndef DEBUG_HPP_
#define DEBUG_HPP_

#if VERSION_DEBUG
#include "state_machine.hpp"
extern StateMachine gSM_Debug;

extern void DebugPrint ( const char * theText );
extern void DebugPrintf ( const char * theFormat, ... );
extern bool DebugInterruptsAreDisabled;
extern const bool DebugPrintHostPackets;

#else
inline void DebugPrint ( const char * theText )
{
}
inline void DebugPrintf ( const char * theFormat, ... )
{
}
#endif
#endif /* DEBUG_HPP_ */
