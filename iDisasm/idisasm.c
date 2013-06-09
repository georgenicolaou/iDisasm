/*
Copyright (C) 2013  George Nicolaou <george[at]preaver.[dot]com>

This file is part of Icarus Disassembly Engine (iDisasm).

Icarus Disassembly Engine (iDisasm) is free software: you can redistribute it 
and/or modify it under the terms of the GNU Lesser General Public License as 
published by the Free Software Foundation, either version 3 of the License, 
or (at your option) any later version.

Icarus Disassembly Engine (iDisasm) is distributed in the hope that it will be 
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General 
Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with Icarus Disassembly Engine (iDisasm).  If not, see 
<http://www.gnu.org/licenses/>.
*/

#include "idisasm_internal.h"
#include "idisasm_include.h"
#include "archs.h"

#include <string.h>

int _disasm( PSIDISASM lpsDisasm, ARCHITECTURE_MODULE * lpArchModule, 
	PLUGIN_MAP * lpMap )
{
	int nConsumedBytes = 0;
	uchar * lpInstrPointer = (uchar *)lpsDisasm->InstructionPointer;
	INSTRUCTIONINTERNAL * lpInstruction;
	dprintf( LVL1, "Got Byte: 0x%02X", *lpInstrPointer );
	
	switch( lpMap[*lpInstrPointer].hint_t ) {
	case MAP_INVALID:
		dprintf( LVL1, "Byte is Invalid/not implemented" );
		return INSTRUCTION_INVALID;
	case MAP_SPECIAL:
		//XXX not implemented. MAP_SPECIAL opcodes have different meaning when
		// a prefix is present.
		dprintf( LVL1, "Byte is Special Opcode" );
	case MAP_OPCODE:
		dprintf( LVL1, "Byte is Opcode" );
		lpsDisasm->Instruction.Opcode += *lpInstrPointer;
		lpInstruction = &lpMap[*lpInstrPointer].uItem.sInstruction;
		lpsDisasm->InternalStuff.nOpcodeIndex = lpInstruction->uOpcodeMap.eOpcodePtr;
		lpsDisasm->Instruction.Category = lpArchModule->get_opcode_category( 
			(ENUM_OPCODE_PTR)lpInstruction->uOpcodeMap.eOpcodePtr );
		CONSUME_BYTES( lpsDisasm, 1 )

		nConsumedBytes = lpArchModule->addressing_handler( lpsDisasm, 
			lpInstruction );
		CONSUME_BYTES( lpsDisasm, nConsumedBytes );
		break;
	case MAP_ESCAPE:
		dprintf( LVL1, "Byte is Escape" );
		lpsDisasm->Instruction.Opcode += *lpInstrPointer;
		lpsDisasm->Instruction.Opcode <<= 8;
		CONSUME_BYTES( lpsDisasm, 1 );
		return _disasm( lpsDisasm, lpArchModule, 
			lpMap[*lpInstrPointer].uItem.lpForwarderMap );

	case MAP_GROUP:
		dprintf( LVL1, "Byte is Group" );
		lpsDisasm->Instruction.Opcode += *lpInstrPointer;
		CONSUME_BYTES( lpsDisasm, 1 );
		//Dummy holder to check for params
		lpInstruction = &lpMap[*lpInstrPointer].uItem.sInstruction;
		if( lpInstruction->Parameters[0].eAddressing 
			!= ADDR_INVALID ) {
				nConsumedBytes += lpArchModule->addressing_handler( lpsDisasm, 
					lpInstruction );
		}
		
		nConsumedBytes += lpArchModule->group_handler( lpsDisasm,
			lpMap[*lpInstrPointer].uItem.lpGroupMap );

		CONSUME_BYTES( lpsDisasm, nConsumedBytes );
		break;
	case MAP_PREFIX:
		dprintf( LVL1, "Byte is Prefix" );
		nConsumedBytes = lpMap[*lpInstrPointer].uItem.prefix_handler( lpsDisasm, 
			*lpInstrPointer );
		CONSUME_BYTES( lpsDisasm, nConsumedBytes );
		break;
	}
	return INSTRUCTION_VALID;
}

IDISASM_API int disasm( PSIDISASM lpsDisasm ) 
{
	ARCHITECTURE_MODULE * lpsArchModule = 
		&ArchitectureModules[lpsDisasm->Architecture];
	uiptr ulTmpVirtualAddress = NULL;
	if( lpsArchModule == NULL ) {
		eprintf( "Architecture not implemented" );
		return ENGINE_ERROR;
	}
	//lpsDisasm->InstructionSize = 0;
	lpsDisasm->Prefixes = PREFIX_NONE;
	lpsDisasm->InternalStuff.nOpcodeIndex = 0;
	memset( &lpsDisasm->Instruction, 0, sizeof( INSTRUCTION ) );
	
	//Looks ugly when in 1 if statement
	if( lpsDisasm->InternalStuff.nVADelta == 0 && lpsDisasm->VirtualAddress != 0 ) {
			lpsDisasm->InternalStuff.nVADelta = lpsDisasm->VirtualAddress - 
				lpsDisasm->InstructionPointer;
			lpsDisasm->InternalStuff.OldVirtualAddress = lpsDisasm->VirtualAddress;
			lpsDisasm->InstructionSize = 0;
	}
	else if( lpsDisasm->InternalStuff.OldVirtualAddress != lpsDisasm->VirtualAddress ) {
		lpsDisasm->InternalStuff.nVADelta = lpsDisasm->VirtualAddress - 
			lpsDisasm->InstructionPointer;
		lpsDisasm->InternalStuff.OldVirtualAddress = lpsDisasm->VirtualAddress;
	}
	//Add previous instruction size to VA to get new address
	lpsDisasm->VirtualAddress += lpsDisasm->InstructionSize;
	lpsDisasm->InstructionSize = 0;
	if( _disasm( lpsDisasm, lpsArchModule, lpsArchModule->lpInitialPluginMap ) == 
		INSTRUCTION_INVALID ) {
			return INSTRUCTION_INVALID;
	}
	else {
		if( lpsDisasm->InternalStuff.bPopulateMnemonics ) {
			lpsArchModule->get_mnemonic( lpsDisasm );
		}
	}
	return INSTRUCTION_VALID;
}

IDISASM_API char * disasm_version(void)
{
	return IDISASM_VERSION;
}