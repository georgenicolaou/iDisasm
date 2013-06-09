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
#include "intel_defs.h"

/*
** Reg Field CR Addressing Map
*/
const ADDRESSING_MAP addressing_sreg = {
	0x38,
	0x03,
	{
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_CR), 
			ADDR_GIVEN+REG_ES },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_CR), 
			ADDR_GIVEN+REG_CS },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_CR), 
			ADDR_GIVEN+REG_SS },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_CR), 
			ADDR_GIVEN+REG_DS },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_CR), 
			ADDR_GIVEN+REG_FS },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_CR), 
			ADDR_GIVEN+REG_GS },
		{ ADDR_INVALID },
		{ ADDR_INVALID },
	}
};

/*
** Reg Field Segment Register Addressing Map
*/
const ADDRESSING_MAP addressing_cr = {
	0x38,
	0x03,
	{
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_CR), 
			ADDR_GIVEN+REG_CR_0 },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_CR), 
		ADDR_GIVEN+REG_CR_1 },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_CR), 
		ADDR_GIVEN+REG_CR_2 },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_CR), 
		ADDR_GIVEN+REG_CR_3 },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_CR), 
		ADDR_GIVEN+REG_CR_4 },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_CR), 
		ADDR_GIVEN+REG_CR_5 },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_CR), 
		ADDR_GIVEN+REG_CR_6 },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_CR), 
		ADDR_GIVEN+REG_CR_7 },
	}
};


/*
** Reg Field DR Addressing Map
*/
const ADDRESSING_MAP addressing_dr = {
	0x38,
	0x03,
	{
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_DR), 
			ADDR_GIVEN+REG_DR_0 },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_DR), 
			ADDR_GIVEN+REG_DR_1 },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_DR), 
			ADDR_GIVEN+REG_DR_2 },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_DR), 
			ADDR_GIVEN+REG_DR_3 },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_DR), 
			ADDR_GIVEN+REG_DR_4 },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_DR), 
			ADDR_GIVEN+REG_DR_5 },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_DR), 
			ADDR_GIVEN+REG_DR_6 },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_DR), 
			ADDR_GIVEN+REG_DR_7 },
	}
	
};

/*
** Reg Field 32bit Addressing Map (EAX-EDI)
*/
const ADDRESSING_MAP addressing_reg_32 = {
	0x38,
	0x03,
	{
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_GENERAL), 
			ADDR_GIVEN+REG_EAX },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_GENERAL), 
		ADDR_GIVEN+REG_ECX },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_GENERAL), 
		ADDR_GIVEN+REG_EDX },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_GENERAL), 
		ADDR_GIVEN+REG_EBX },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_GENERAL), 
		ADDR_GIVEN+REG_ESP },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_GENERAL), 
		ADDR_GIVEN+REG_EBP },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_GENERAL), 
		ADDR_GIVEN+REG_ESI },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_GENERAL), 
		ADDR_GIVEN+REG_EDI },
	}

};

/*
** Reg Field 16bit Addressing Map (AX-DI)
*/
const ADDRESSING_MAP addressing_reg_16 = 
{
	0x38,
	0x03,
	{
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_GENERAL), 
			ADDR_GIVEN+REG_AX },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_GENERAL), 
			ADDR_GIVEN+REG_CX },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_GENERAL), 
			ADDR_GIVEN+REG_DX },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_GENERAL), 
			ADDR_GIVEN+REG_BX },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_GENERAL), 
			ADDR_GIVEN+REG_SP },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_GENERAL), 
			ADDR_GIVEN+REG_BP },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_GENERAL), 
			ADDR_GIVEN+REG_SI },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_GENERAL), 
			ADDR_GIVEN+REG_DI },
	}

};

/*
** Reg Field 8bit Addressing Map (EAX-EDI)
*/
const ADDRESSING_MAP addressing_reg_8 = 
{
	0x38,
	0x03,
	{
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_GENERAL), 
			ADDR_GIVEN+REG_AL },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_GENERAL), 
		ADDR_GIVEN+REG_CL },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_GENERAL), 
		ADDR_GIVEN+REG_DL },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_GENERAL), 
		ADDR_GIVEN+REG_BL },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_GENERAL), 
		ADDR_GIVEN+REG_AH },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_GENERAL), 
		ADDR_GIVEN+REG_CH },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_GENERAL), 
		ADDR_GIVEN+REG_DH },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_GENERAL), 
		ADDR_GIVEN+REG_BH },
	}

};

/*
** Mod R/M 32bit Field SIB Byte
*/
//XXX

const ADDRESSING_MAP addressing_sib_32_base_mod00 = {
	0x07,
	0x00,
	{
		{ ADDRESSING_GIVEN, OPERAND_REGISTER, ADDR_GIVEN+REG_EAX },
		{ ADDRESSING_GIVEN, OPERAND_REGISTER, ADDR_GIVEN+REG_ECX },
		{ ADDRESSING_GIVEN, OPERAND_REGISTER, ADDR_GIVEN+REG_EDX },
		{ ADDRESSING_GIVEN, OPERAND_REGISTER, ADDR_GIVEN+REG_EBX },
		{ ADDRESSING_GIVEN, OPERAND_REGISTER, ADDR_GIVEN+REG_ESP },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_MEMORY+BIT32), ADDR_INVALID },
		{ ADDRESSING_GIVEN, OPERAND_REGISTER, ADDR_GIVEN+REG_ESI },
		{ ADDRESSING_GIVEN, OPERAND_REGISTER, ADDR_GIVEN+REG_EDI },
	}
};


const ADDRESSING_MAP addressing_sib_32_base = {
	0x07,
	0x00,
	{
		{ ADDRESSING_GIVEN, OPERAND_REGISTER, ADDR_GIVEN+REG_EAX },
		{ ADDRESSING_GIVEN, OPERAND_REGISTER, ADDR_GIVEN+REG_ECX },
		{ ADDRESSING_GIVEN, OPERAND_REGISTER, ADDR_GIVEN+REG_EDX },
		{ ADDRESSING_GIVEN, OPERAND_REGISTER, ADDR_GIVEN+REG_EBX },
		{ ADDRESSING_GIVEN, OPERAND_REGISTER, ADDR_GIVEN+REG_ESP },
		{ ADDRESSING_GIVEN, OPERAND_REGISTER, ADDR_GIVEN+REG_EBP },
		{ ADDRESSING_GIVEN, OPERAND_REGISTER, ADDR_GIVEN+REG_ESI },
		{ ADDRESSING_GIVEN, OPERAND_REGISTER, ADDR_GIVEN+REG_EDI },
	}
};
const ADDRESSING_MAP addressing_sib_32_indexscale = {
	0xF8,
	0x03,
	{
		// SS = 00
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_IMMIDIATE+SCALE_NONE), ADDR_GIVEN+REG_EAX },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_IMMIDIATE+SCALE_NONE), ADDR_GIVEN+REG_ECX },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_IMMIDIATE+SCALE_NONE), ADDR_GIVEN+REG_EDX },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_IMMIDIATE+SCALE_NONE), ADDR_GIVEN+REG_EBX },
		{ ADDRESSING_GIVEN, OPERAND_NONE, ADDR_INVALID },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_IMMIDIATE+SCALE_NONE), ADDR_GIVEN+REG_EBP },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_IMMIDIATE+SCALE_NONE), ADDR_GIVEN+REG_ESI },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_IMMIDIATE+SCALE_NONE), ADDR_GIVEN+REG_EDI },

		//SS = 01
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_IMMIDIATE+SCALE_2), ADDR_GIVEN+REG_EAX },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_IMMIDIATE+SCALE_2), ADDR_GIVEN+REG_ECX },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_IMMIDIATE+SCALE_2), ADDR_GIVEN+REG_EDX },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_IMMIDIATE+SCALE_2), ADDR_GIVEN+REG_EBX },
		{ ADDRESSING_GIVEN, OPERAND_NONE, ADDR_INVALID },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_IMMIDIATE+SCALE_2), ADDR_GIVEN+REG_EBP },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_IMMIDIATE+SCALE_2), ADDR_GIVEN+REG_ESI },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_IMMIDIATE+SCALE_2), ADDR_GIVEN+REG_EDI },

		//SS = 01
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_IMMIDIATE+SCALE_4), ADDR_GIVEN+REG_EAX },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_IMMIDIATE+SCALE_4), ADDR_GIVEN+REG_ECX },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_IMMIDIATE+SCALE_4), ADDR_GIVEN+REG_EDX },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_IMMIDIATE+SCALE_4), ADDR_GIVEN+REG_EBX },
		{ ADDRESSING_GIVEN, OPERAND_NONE, ADDR_INVALID },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_IMMIDIATE+SCALE_4), ADDR_GIVEN+REG_EBP },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_IMMIDIATE+SCALE_4), ADDR_GIVEN+REG_ESI },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_IMMIDIATE+SCALE_4), ADDR_GIVEN+REG_EDI },

		//SS = 01
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_IMMIDIATE+SCALE_8), ADDR_GIVEN+REG_EAX },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_IMMIDIATE+SCALE_8), ADDR_GIVEN+REG_ECX },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_IMMIDIATE+SCALE_8), ADDR_GIVEN+REG_EDX },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_IMMIDIATE+SCALE_8), ADDR_GIVEN+REG_EBX },
		{ ADDRESSING_GIVEN, OPERAND_NONE, ADDR_INVALID },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_IMMIDIATE+SCALE_8), ADDR_GIVEN+REG_EBP },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_IMMIDIATE+SCALE_8), ADDR_GIVEN+REG_ESI },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_IMMIDIATE+SCALE_8), ADDR_GIVEN+REG_EDI },
	}
	
};

const ADDRESSING_MAPS addressing_sib_32_mod00 = {
	(ADDRESSING_MAP *)&addressing_sib_32_indexscale,
	(ADDRESSING_MAP *)&addressing_sib_32_base_mod00
};

const ADDRESSING_MAPS addressing_sib_32 = {
	(ADDRESSING_MAP *)&addressing_sib_32_indexscale,
	(ADDRESSING_MAP *)&addressing_sib_32_base
};

/*
** Mod R/M Field 32bit Addressing Map
*/
const ADDRESSING_MAP addressing_modrm_32 = {
	0xC7,
	0x00,
	{
		//Mod = 00
		{ ADDRESSING_GIVEN, OPERAND_MEMORY, ADDR_GIVEN+REG_EAX },
		{ ADDRESSING_GIVEN, OPERAND_MEMORY, ADDR_GIVEN+REG_ECX },
		{ ADDRESSING_GIVEN, OPERAND_MEMORY, ADDR_GIVEN+REG_EDX },
		{ ADDRESSING_GIVEN, OPERAND_MEMORY, ADDR_GIVEN+REG_EBX },
		{ (int)&addressing_sib_32_mod00, OPERAND_MEMORY },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_MEMORY+BIT32), ADDR_INVALID },
		{ ADDRESSING_GIVEN, OPERAND_MEMORY, ADDR_GIVEN+REG_ESI },
		{ ADDRESSING_GIVEN, OPERAND_MEMORY, ADDR_GIVEN+REG_EDI },

		//Skip bytes 08-3F (56 Bytes)
		ARRAY_FILL( 0, 5, 6, { ADDRESSING_INVALID } )

		//Mod = 01
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_MEMORY+BIT8), ADDR_GIVEN+REG_EAX },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_MEMORY+BIT8), ADDR_GIVEN+REG_ECX },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_MEMORY+BIT8), ADDR_GIVEN+REG_EDX },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_MEMORY+BIT8), ADDR_GIVEN+REG_EBX },
		{ (int)&addressing_sib_32, (OPERAND_TYPE)(OPERAND_MEMORY+BIT8), ADDR_INVALID },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_MEMORY+BIT8), ADDR_GIVEN+REG_EBP },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_MEMORY+BIT8), ADDR_GIVEN+REG_ESI },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_MEMORY+BIT8), ADDR_GIVEN+REG_EDI },

		//Skip bytes 48-7F
		ARRAY_FILL( 0, 5, 6, { ADDRESSING_INVALID } )

		//Mod = 10
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_MEMORY+BIT32), ADDR_GIVEN+REG_EAX },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_MEMORY+BIT32), ADDR_GIVEN+REG_ECX },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_MEMORY+BIT32), ADDR_GIVEN+REG_EDX },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_MEMORY+BIT32), ADDR_GIVEN+REG_EBX },
		{ (int)&addressing_sib_32, (OPERAND_TYPE)(OPERAND_MEMORY+BIT32), ADDR_INVALID },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_MEMORY+BIT32), ADDR_GIVEN+REG_EBP },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_MEMORY+BIT32), ADDR_GIVEN+REG_ESI },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_MEMORY+BIT32), ADDR_GIVEN+REG_EDI },

		//Skip bytes 88-BF
		ARRAY_FILL( 0, 5, 6, { ADDRESSING_INVALID } )

		//Mod = 11
		{ ADDRESSING_GIVEN, OPERAND_REGISTER, ADDR_GIVEN+REG_EAX },
		{ ADDRESSING_GIVEN, OPERAND_REGISTER, ADDR_GIVEN+REG_ECX },
		{ ADDRESSING_GIVEN, OPERAND_REGISTER, ADDR_GIVEN+REG_EDX },
		{ ADDRESSING_GIVEN, OPERAND_REGISTER, ADDR_GIVEN+REG_EBX },
		{ ADDRESSING_GIVEN, OPERAND_REGISTER, ADDR_GIVEN+REG_ESP },
		{ ADDRESSING_GIVEN, OPERAND_REGISTER, ADDR_GIVEN+REG_EBP },
		{ ADDRESSING_GIVEN, OPERAND_REGISTER, ADDR_GIVEN+REG_ESI },
		{ ADDRESSING_GIVEN, OPERAND_REGISTER, ADDR_GIVEN+REG_EDI },
	}
};

/*
** Mod R/M Field 32bit Addressing Map
*/
//XXX this is a copy of modrm_32, need to make it 16
const ADDRESSING_MAP addressing_modrm_16 = {
	0xC7,
	0x00,
	{
		//Mod = 00
		{ ADDRESSING_GIVEN, OPERAND_MEMORY, ADDR_GIVEN+REG_EAX },
		{ ADDRESSING_GIVEN, OPERAND_MEMORY, ADDR_GIVEN+REG_ECX },
		{ ADDRESSING_GIVEN, OPERAND_MEMORY, ADDR_GIVEN+REG_EDX },
		{ ADDRESSING_GIVEN, OPERAND_MEMORY, ADDR_GIVEN+REG_EBX },
		{ (int)&addressing_sib_32_mod00, OPERAND_MEMORY },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_MEMORY+BIT32), ADDR_INVALID },
		{ ADDRESSING_GIVEN, OPERAND_MEMORY, ADDR_GIVEN+REG_ESI },
		{ ADDRESSING_GIVEN, OPERAND_MEMORY, ADDR_GIVEN+REG_EDI },

		//Skip bytes 08-3F (56 Bytes)
		ARRAY_FILL( 0, 5, 6, { ADDRESSING_INVALID } )

		//Mod = 01
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_MEMORY+BIT16), ADDR_GIVEN+REG_EAX },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_MEMORY+BIT16), ADDR_GIVEN+REG_ECX },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_MEMORY+BIT16), ADDR_GIVEN+REG_EDX },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_MEMORY+BIT16), ADDR_GIVEN+REG_EBX },
		{ (int)&addressing_sib_32, (OPERAND_TYPE)(OPERAND_MEMORY+BIT8), ADDR_INVALID },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_MEMORY+BIT16), ADDR_GIVEN+REG_EBP },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_MEMORY+BIT16), ADDR_GIVEN+REG_ESI },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_MEMORY+BIT16), ADDR_GIVEN+REG_EDI },

		//Skip bytes 48-7F
		ARRAY_FILL( 0, 5, 6, { ADDRESSING_INVALID } )

		//Mod = 10
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_MEMORY+BIT32), ADDR_GIVEN+REG_EAX },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_MEMORY+BIT32), ADDR_GIVEN+REG_ECX },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_MEMORY+BIT32), ADDR_GIVEN+REG_EDX },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_MEMORY+BIT32), ADDR_GIVEN+REG_EBX },
		{ (int)&addressing_sib_32, (OPERAND_TYPE)(OPERAND_MEMORY+BIT32), ADDR_INVALID },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_MEMORY+BIT32), ADDR_GIVEN+REG_EBP },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_MEMORY+BIT32), ADDR_GIVEN+REG_ESI },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_MEMORY+BIT32), ADDR_GIVEN+REG_EDI },

		//Skip bytes 88-BF
		ARRAY_FILL( 0, 5, 6, { ADDRESSING_INVALID } )

		//Mod = 11
		{ ADDRESSING_GIVEN, OPERAND_REGISTER, ADDR_GIVEN+REG_EAX },
		{ ADDRESSING_GIVEN, OPERAND_REGISTER, ADDR_GIVEN+REG_ECX },
		{ ADDRESSING_GIVEN, OPERAND_REGISTER, ADDR_GIVEN+REG_EDX },
		{ ADDRESSING_GIVEN, OPERAND_REGISTER, ADDR_GIVEN+REG_EBX },
		{ ADDRESSING_GIVEN, OPERAND_REGISTER, ADDR_GIVEN+REG_ESP },
		{ ADDRESSING_GIVEN, OPERAND_REGISTER, ADDR_GIVEN+REG_EBP },
		{ ADDRESSING_GIVEN, OPERAND_REGISTER, ADDR_GIVEN+REG_ESI },
		{ ADDRESSING_GIVEN, OPERAND_REGISTER, ADDR_GIVEN+REG_EDI },
	}
};

/*
** Reg Field MMX Addressing Map (ADDR_P)
*/
const ADDRESSING_MAP addressing_reg_mmx = {
	0x38,
	0x03,
	{
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_MMX), 
			ADDR_GIVEN+REG_MM0 },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_MMX), 
		ADDR_GIVEN+REG_MM1 },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_MMX), 
		ADDR_GIVEN+REG_MM2 },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_MMX), 
		ADDR_GIVEN+REG_MM3 },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_MMX), 
		ADDR_GIVEN+REG_MM4 },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_MMX), 
		ADDR_GIVEN+REG_MM5 },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_MMX), 
		ADDR_GIVEN+REG_MM6 },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_MMX), 
		ADDR_GIVEN+REG_MM7 },
	}

};

/*
** Reg Field SSE Addressing Map (ADDR_V XMM/YMM)
*/
const ADDRESSING_MAP addressing_reg_sse = {
	0x38,
	0x03,
	{
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_SSE), 
			ADDR_GIVEN+REG_XMM0 },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_SSE), 
		ADDR_GIVEN+REG_MM1 },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_SSE), 
		ADDR_GIVEN+REG_MM2 },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_SSE), 
		ADDR_GIVEN+REG_MM3 },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_SSE), 
		ADDR_GIVEN+REG_MM4 },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_SSE), 
		ADDR_GIVEN+REG_MM5 },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_SSE), 
		ADDR_GIVEN+REG_MM6 },
		{ ADDRESSING_GIVEN, (OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_SSE), 
		ADDR_GIVEN+REG_MM7 },
	}

};