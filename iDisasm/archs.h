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

#ifndef _ARCHS_H
#define _ARCHS_H
#include "idisasm_include.h"

#include "intel.h"
#include "intel_defs.h"
#include "intel_mnemonics.h"

typedef struct _architecture_module {
	ARCHITECTURE Architecture;
	PLUGIN_MAP * lpInitialPluginMap;
	ENUM_OPCODE_CATEGORY_T ( * get_opcode_category )( ENUM_OPCODE_PTR );
	int ( * addressing_handler )( PSIDISASM, INSTRUCTIONINTERNAL * );
	int ( * group_handler )( PSIDISASM, GROUP_MAP * );
	BOOL ( * get_mnemonic)( PSIDISASM );
} ARCHITECTURE_MODULE;


ARCHITECTURE_MODULE ArchitectureModules[] = {
	{ 
		X86, 
		(PLUGIN_MAP *)&one_byte_opcode_map, 
		&intel_get_opcode_category,
		&intel_addressing_handler,
		&intel_group_handler,
		&intel_get_mnemonic
	},
	{ ARM, NULL }
};


#endif //_ARCHS_H