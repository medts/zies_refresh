#include "registers.hpp"
#include "string.h"

// global vars
Registers gRegisters;

//================================================================
// defined by the linker
__attribute__ ((section (".Registers"))) uint8_t RegistersData[RegistersSize] = { };

//================================================================
Registers::Registers ()
{
	memset(RegistersData, 0, RegistersSize);
}

uint32_t & Registers::operator[] ( RegisterIds theId ) const
{
	return *(uint32_t *) &RegistersData[theId];
}

