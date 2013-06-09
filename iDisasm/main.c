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

#include <stdlib.h>
#include <stdio.h>
#include "idisasm_include.h"

int main() {
	SIDISASM sDisasm = {0};
	int i;
	/*
	004010E6  |?  00C9          ADD CL,CL
	004010E8  |.  C2 0400       RETN 4
	*/
	char lpHex[] = {
		'\x00', '\xc9',
		'\xc2'
	};
	char * lpAddr = (char *)&lpHex;

	sDisasm.Architecture = X86;
	sDisasm.InstructionPointer = (uiptr)lpHex;
	sDisasm.VirtualAddress = 0x004010E6;
	sDisasm.InternalStuff.bPopulateMnemonics = TRUE;
	for( i = 0 ; i < 2; i++ ) {
		disasm( &sDisasm );
		printf("%#X %s\n", sDisasm.VirtualAddress, sDisasm.Mnemonic );
	}
	
	system("PAUSE");
}
