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

int intel_prefix( PSIDISASM lpDisasm, uchar uPrefixByte )
{
	switch( uPrefixByte )
	{
	//Group 1
	case 0xf0:
		lpDisasm->Prefixes = (PREFIXES)( lpDisasm->Prefixes | INTEL_PREFIX_GRP1_LOCK );
		break;
	case 0xf2:
		lpDisasm->Prefixes = (PREFIXES)( lpDisasm->Prefixes | INTEL_PREFIX_GRP1_REPNE );
		break;
	case 0xf3:
		lpDisasm->Prefixes = (PREFIXES)( lpDisasm->Prefixes | INTEL_PREFIX_GRP1_REP );
		break;

	//Group 2
	case 0x2e:
	case 0x36:
	case 0x3e:
	case 0x26:
	case 0x64:
	case 0x65:
		if( lpDisasm->Prefixes & INTEL_PREFIX_GRP2 ) {
			//Already a group 2 prefix present, reset the rest
			lpDisasm->Prefixes = (PREFIXES)( lpDisasm->Prefixes & 
				~INTEL_PREFIX_GRP2 );
		}
		switch( uPrefixByte ) {
		case 0x2e:
			lpDisasm->Prefixes = (PREFIXES)( lpDisasm->Prefixes | INTEL_PREFIX_GRP2_CS );
			break;
		case 0x36:
			lpDisasm->Prefixes = (PREFIXES)( lpDisasm->Prefixes | INTEL_PREFIX_GRP2_SS );
			break;
		case 0x3e:
			lpDisasm->Prefixes = (PREFIXES)( lpDisasm->Prefixes | INTEL_PREFIX_GRP2_DS );
			break;
		case 0x26:
			lpDisasm->Prefixes = (PREFIXES)( lpDisasm->Prefixes | INTEL_PREFIX_GRP2_ES );
			break;
		case 0x64:
			lpDisasm->Prefixes = (PREFIXES)( lpDisasm->Prefixes | INTEL_PREFIX_GRP2_FS );
			break;
		case 0x65:
			lpDisasm->Prefixes = (PREFIXES)( lpDisasm->Prefixes | INTEL_PREFIX_GRP2_GS );
			break;
		}
	//Group 3
	case 0x66:
		lpDisasm->Prefixes = (PREFIXES)( lpDisasm->Prefixes | INTEL_PREFIX_GRP3_OPSIZEOVERRIDE );
		break;
	
	//Group 4
	case 0x67:
		lpDisasm->Prefixes = (PREFIXES)( lpDisasm->Prefixes | INTEL_PREFIX_GRP4_ADDRSIZEOVERRIDE );
		break;
	}
	//We are consuming 1 byte
	return 1;
}


const  GROUP_MAP group_6 = {
	MODRM_REGBITS_MASK,
	MODRM_REGBITS_SHIFT,
	{
		{ MAP_OPCODE, { OP_SLDT, {{ ADDR_MR, TYPE_w, WRITE }} } },
		{ MAP_OPCODE, { OP_STR, {{ ADDR_MR, TYPE_w, WRITE }} } },
		{ MAP_OPCODE, { OP_LLDT, {{ ADDR_MR, TYPE_w, READ }} } },
		{ MAP_OPCODE, { OP_LTR, {{ ADDR_MR, TYPE_w, READ }} } },
		{ MAP_OPCODE, { OP_VERR, {{ ADDR_MR, TYPE_w, READ }} } },
		{ MAP_OPCODE, { OP_VERW, {{ ADDR_MR, TYPE_w, READ }} } },
	}
};

const GROUP_MAP group_7mod11 = {
	MODRM_REGBITS_MASK+MODRM_RMBITS_MASK, //64 possible values
	0,
	{
		//REG = 000
		{ MAP_OPCODE, { OP_VMCALL } },
		{ MAP_OPCODE, { OP_VMLAUNCH } },
		{ MAP_OPCODE, { OP_VMRESUME } },
		{ MAP_OPCODE, { OP_VMXOFF } },
		ARRAY_FILL( 0, 0, 4, { MAP_INVALID } )

		//REG = 001
		{ MAP_OPCODE, { OP_MONITOR } },
		{ MAP_OPCODE, { OP_MWAIT } },
		{ MAP_OPCODE, { OP_CLAC } },
		{ MAP_OPCODE, { OP_STAC } },
		ARRAY_FILL( 0, 0, 4, { MAP_INVALID } )

		//REG = 010
		{ MAP_OPCODE, { OP_XGETBV } },
		{ MAP_OPCODE, { OP_XSETBV } },
		{ MAP_OPCODE, { OP_VMFUNC } },
		{ MAP_OPCODE, { OP_XEND } },
		{ MAP_OPCODE, { OP_XTEST } },

		ARRAY_FILL( 0, 0, 4, { MAP_INVALID } )

		//REG = 011
		ARRAY_FILL( 0, 0, 8, { MAP_INVALID } )
		//REG = 100
		ARRAY_FILL( 0, 0, 8, { MAP_INVALID } )
		//REG = 101
		ARRAY_FILL( 0, 0, 8, { MAP_INVALID } )
		//REG = 110
		ARRAY_FILL( 0, 0, 8, { MAP_INVALID } )
		//REG = 111
		{ MAP_OPCODE, { OP_SWAPGS } }, //XXX Only 64
		{ MAP_OPCODE, { OP_RDTSCP } },

		ARRAY_FILL( 0, 0, 6, { MAP_INVALID } ) //Coz we don't need out of bounds...
	}
};

const  GROUP_MAP group_7 = {
	MODRM_MODBITS_MASK+MODRM_REGBITS_MASK,
	MODRM_REGBITS_SHIFT,
	{
		//MOD = 00
		{ MAP_OPCODE, { OP_SGDT, {{ ADDR_M, TYPE_s, WRITE }} } },
		{ MAP_OPCODE, { OP_SIDT, {{ ADDR_M, TYPE_s, WRITE }} } },
		{ MAP_OPCODE, { OP_LGDT, {{ ADDR_M, TYPE_s, READ }} } },
		{ MAP_OPCODE, { OP_LIDT, {{ ADDR_M, TYPE_s, READ }} } },
		{ MAP_OPCODE, { OP_SMSW, {{ ADDR_MR, TYPE_w, WRITE }} } },
		{ MAP_INVALID },
		{ MAP_OPCODE, { OP_LMSW, {{ ADDR_E, TYPE_w, READ }} } },
		{ MAP_OPCODE, { OP_INVLPG, {{ ADDR_M, TYPE_b, READ }} } },

		//MOD = 01
		ARRAY_FILL( 0, 0, 7, { MAP_INVALID } )

		//MOD = 10
		ARRAY_FILL( 0, 0, 7, { MAP_INVALID } )

		//MOD = 11
		{ MAP_GROUP, (int)&group_7mod11 }
	}
};


const GROUP_MAP group_prefetch = {
	MODRM_REGBITS_MASK,
	MODRM_REGBITS_SHIFT,
	{
		{ MAP_OPCODE, { OP_PREFETCHNTA } },
		{ MAP_OPCODE, { OP_PREFETCH0 } },
		{ MAP_OPCODE, { OP_PREFETCH1 } },
		{ MAP_OPCODE, { OP_PREFETCH2 } },
	}
};

//0F 1F /0 NOP INSTRUCTION
const GROUP_MAP group_nop_1f = {
	MODRM_REGBITS_MASK,
	MODRM_REGBITS_SHIFT,
	{
		{ MAP_OPCODE, { OP_NOP } }
	}
};
const PLUGIN_MAP amd_3dnow_map[] = {
	{ MAP_INVALID }
};

const PLUGIN_MAP three_byte_opcode_map_0f38[] = {
	{ MAP_INVALID }
};

const PLUGIN_MAP three_byte_opcode_map_0f3a[] = {
	{ MAP_INVALID }
};

//XXX needs to be overwritten when prefix 66
const GROUP_MAP group_12 = {
	MODRM_MODBITS_MASK+MODRM_REGBITS_MASK,
	MODRM_REGBITS_MASK,
	{
		//Skip MOD = 00, MOD == 01, MOD = 10
		{ MAP_INVALID },
		ARRAY_FILL( 0, 2, 4, { MAP_INVALID } )

		//MOD = 11
		{ MAP_INVALID },
		{ MAP_INVALID },
		{ MAP_OPCODE, { OP_PSRLW, {{ADDR_N, TYPE_q, WRITE}, { ADDR_I, TYPE_b, READ }} } },
		{ MAP_INVALID },
		{ MAP_OPCODE, { OP_PSRAW, {{ADDR_N, TYPE_q, WRITE}, { ADDR_I, TYPE_b, READ }} } },
		{ MAP_INVALID },
		{ MAP_OPCODE, { OP_PSLLW, {{ADDR_N, TYPE_q, WRITE}, { ADDR_I, TYPE_b, READ }} } },
		{ MAP_INVALID }
	}
};

const GROUP_MAP group_13 = { NULL };
const GROUP_MAP group_14 = { NULL };
const GROUP_MAP group_15 = { NULL };

const GROUP_MAP group_10 = {
	MODRM_REGBITS_MASK,
	MODRM_REGBITS_SHIFT,
	{
		{ MAP_OPCODE, { OP_UD2 } },
		{ MAP_OPCODE, { OP_UD2 } },
		{ MAP_OPCODE, { OP_UD2 } },
		{ MAP_OPCODE, { OP_UD2 } },
		{ MAP_OPCODE, { OP_UD2 } },
		{ MAP_OPCODE, { OP_UD2 } },
		{ MAP_OPCODE, { OP_UD2 } },
		{ MAP_OPCODE, { OP_UD2 } },
	}
};

const GROUP_MAP group_8 = {
	MODRM_REGBITS_MASK,
	MODRM_REGBITS_SHIFT,
	{
		{ MAP_INVALID },
		{ MAP_INVALID },
		{ MAP_INVALID },
		{ MAP_INVALID },
		{ MAP_OPCODE, { OP_BT } },
		{ MAP_OPCODE, { OP_BTS } },
		{ MAP_OPCODE, { OP_BTR } },
		{ MAP_OPCODE, { OP_BTC } },
	}
};

//XXX Need to implement MAP_GROUP for RDRAND + RDSEED and the VM instructions...
const GROUP_MAP group_9 = {
	MODRM_REGBITS_MASK,
	MODRM_REGBITS_SHIFT,
	{
		{ MAP_INVALID },
		{ MAP_OPCODE, { OP_CMPXCHG16B, ADDR_M, TYPE_dq } }, //XXX 8B ? we need it?
		{ MAP_INVALID },
		{ MAP_INVALID },
		{ MAP_INVALID },
		{ MAP_INVALID },
		{ MAP_OPCODE, { OP_VMPTRLD, { ADDR_M, TYPE_q } } },
		{ MAP_OPCODE, { OP_VMPTRST, { ADDR_M, TYPE_q } } },
	}
};

const  PLUGIN_MAP two_byte_opcode_map[] = {

	/* 0F 00 - 0F 07 */
	{ MAP_GROUP, (int)&group_6 },
	{ MAP_GROUP, (int)&group_7 },
	{ MAP_OPCODE, { OP_LAR, {{ ADDR_G, TYPE_v, WRITE }, { ADDR_E, TYPE_w, READ }} } },
	{ MAP_OPCODE, { OP_LSL, {{ ADDR_G, TYPE_v, WRITE }, { ADDR_E, TYPE_w, READ }} } },
	{ MAP_OPCODE, { OP_LOADALL } },
	{ MAP_OPCODE, { OP_SYSCALL } },
	{ MAP_OPCODE, { OP_CLTS } },
	{ MAP_OPCODE, { OP_SYSRET, {{ ADDR_GIVEN+REG_RCX, TYPE_v }, { ADDR_GIVEN+REG_FS, TYPE_v }} } },

	/* 0F 08 - 0F 0F */
	{ MAP_OPCODE, { OP_INVD } },
	{ MAP_OPCODE, { OP_WBINVD } },
	{ MAP_INVALID }, //ILLEGAL
	{ MAP_OPCODE, { OP_UD2 } },
	{ MAP_INVALID }, //ILLEGAL
	{ MAP_INVALID }, //XXX THIS IS A PREFETCH INSTRUCTION, NO INFO THO 
	{ MAP_OPCODE, { OP_FMMS } }, //AMD-3D
	{ MAP_ESCAPE, (int)&amd_3dnow_map }, //TODO AMD 3dnow instructions

	/* 0F 10 - 0F 17 */
	{ MAP_SPECIAL, { OP_MOVUPS, {{ ADDR_V, TYPE_ps, WRITE }, { ADDR_W, TYPE_ps, READ }} } },
	{ MAP_SPECIAL, { OP_MOVUPS, {{ ADDR_W, TYPE_ps }, { ADDR_V, TYPE_ps }} } },
	{ MAP_SPECIAL, { OP_MOVHLPS, {{ ADDR_V, TYPE_q }, { ADDR_H, TYPE_q }, { ADDR_U, TYPE_q } } } }, //XXX SO WTF WITH MOVLPS?
	{ MAP_SPECIAL, { OP_MOVLPS, {{ ADDR_M, TYPE_q }, { ADDR_V, TYPE_q }} } },
	{ MAP_SPECIAL, { OP_UNPCKLPS, {{ ADDR_V, TYPE_x }, { ADDR_H, TYPE_x }, { ADDR_W, TYPE_x }} } },
	{ MAP_SPECIAL, { OP_UNPCKHPS, {{ ADDR_V, TYPE_x }, { ADDR_H, TYPE_x }, { ADDR_W, TYPE_x }} } },
	{ MAP_SPECIAL, { OP_MOVLHPS, {{ ADDR_V, TYPE_dq }, { ADDR_H, TYPE_q }, { ADDR_U, TYPE_q }} } },
	{ MAP_SPECIAL, { OP_MOVHPS, {{ ADDR_M, TYPE_q }, { ADDR_V, TYPE_q }} } },

	/* 0F 18 - 0F 1F */
	{ MAP_GROUP, (int)&group_prefetch },
	{ MAP_INVALID }, 
	{ MAP_INVALID },
	{ MAP_INVALID },
	{ MAP_INVALID },
	{ MAP_INVALID },
	{ MAP_INVALID },
	{ MAP_GROUP, (int)&group_nop_1f, {{ ADDR_E, TYPE_v }} },

	/* 0F 20 - 0F 27 */
	{ MAP_OPCODE, { OP_MOV, {{ ADDR_R, TYPE_d }, { ADDR_C, TYPE_d }} } },
	{ MAP_OPCODE, { OP_MOV, {{ ADDR_R, TYPE_d }, { ADDR_D, TYPE_d }} } },
	{ MAP_OPCODE, { OP_MOV, {{ ADDR_C, TYPE_d }, { ADDR_R, TYPE_d }} } },
	{ MAP_OPCODE, { OP_MOV, {{ ADDR_D, TYPE_d }, { ADDR_R, TYPE_d }} } },
	{ MAP_INVALID },
	{ MAP_INVALID },
	{ MAP_INVALID },
	{ MAP_INVALID },

	/* 0F 28 - 0F 2F */
	{ MAP_SPECIAL, { OP_MOVAPS, {{ ADDR_V, TYPE_ps }, { ADDR_W, TYPE_ps }} } },
	{ MAP_SPECIAL, { OP_MOVAPS, {{ ADDR_W, TYPE_ps }, { ADDR_V, TYPE_ps }} } },
	{ MAP_SPECIAL, { OP_CVTPI2PS, {{ ADDR_V, TYPE_ps }, { ADDR_Q, TYPE_pi }} } },
	{ MAP_SPECIAL, { OP_MOVNTPS, {{ ADDR_M, TYPE_ps }, { ADDR_V, TYPE_ps }} } },
	{ MAP_SPECIAL, { OP_CVTTPS2PI, {{ ADDR_P, TYPE_pi }, { ADDR_W, TYPE_ps }} } },
	{ MAP_SPECIAL, { OP_CVTPS2PI, {{ ADDR_P, TYPE_pi }, { ADDR_W, TYPE_ps }} } },
	{ MAP_SPECIAL, { OP_UCOMISS, {{ ADDR_V, TYPE_ss }, { ADDR_W, TYPE_ss }} } },
	{ MAP_SPECIAL, { OP_COMISS, {{ ADDR_V, TYPE_ss }, { ADDR_W, TYPE_ss }} } },

	/* 0F 30 - 0F 37 */
	{ MAP_OPCODE, { OP_WRMSR, {{ ADDR_GIVEN+REG_EDX, TYPE_d, READ }, { ADDR_GIVEN+REG_EAX, TYPE_d, READ }, { ADDR_GIVEN+REG_ECX, TYPE_d, READ }} } },
	{ MAP_OPCODE, { OP_RDTSC, {{ ADDR_GIVEN+REG_EDX, TYPE_d, WRITE }, { ADDR_GIVEN+REG_EAX, TYPE_d, WRITE }, { ADDR_GIVEN+REG_ECX, TYPE_d, READ }} } },
	{ MAP_OPCODE, { OP_RDMSR, {{ ADDR_GIVEN+REG_EDX, TYPE_d, WRITE }, { ADDR_GIVEN+REG_EAX, TYPE_d, WRITE }, { ADDR_GIVEN+REG_ECX, TYPE_d, READ }} } },
	{ MAP_OPCODE, { OP_RDPMC, {{ ADDR_GIVEN+REG_EDX, TYPE_d, WRITE }, { ADDR_GIVEN+REG_EAX, TYPE_d, WRITE }, { ADDR_GIVEN+REG_ECX, TYPE_d, READ }} } },
	{ MAP_OPCODE, { OP_SYSENTER, {{ ADDR_GIVEN+REG_ESP, TYPE_d, (OPERAND_ACCESS)(REFERENCE+WRITE) }, { ADDR_GIVEN+REG_EIP, TYPE_d, (OPERAND_ACCESS)(REFERENCE+WRITE) }} } },
	{ MAP_OPCODE, { OP_SYSEXIT } }, //Would need some extra salt
	{ MAP_INVALID },
	{ MAP_OPCODE, { OP_GETSEC } },

	/* 0F 38 - 0F 3F */
	{ MAP_ESCAPE, (int)&three_byte_opcode_map_0f38 },
	{ MAP_INVALID },
	{ MAP_ESCAPE, (int)three_byte_opcode_map_0f3a },
	{ MAP_INVALID },
	{ MAP_INVALID },
	{ MAP_INVALID },
	{ MAP_INVALID },
	{ MAP_INVALID },

	/* 0F 40 - 0F 47 */
	{ MAP_OPCODE, { OP_CMOVO, {{ ADDR_G, TYPE_v, WRITE }, { ADDR_E, TYPE_v, READ }} } },
	{ MAP_OPCODE, { OP_CMOVNO, {{ ADDR_G, TYPE_v, WRITE }, { ADDR_E, TYPE_v, READ }} } },
	{ MAP_OPCODE, { OP_CMOVC, {{ ADDR_G, TYPE_v, WRITE }, { ADDR_E, TYPE_v, READ }} } },
	{ MAP_OPCODE, { OP_CMOVNC, {{ ADDR_G, TYPE_v, WRITE }, { ADDR_E, TYPE_v, READ }} } },
	{ MAP_OPCODE, { OP_CMOVZ, {{ ADDR_G, TYPE_v, WRITE }, { ADDR_E, TYPE_v, READ }} } },
	{ MAP_OPCODE, { OP_CMOVNZ, {{ ADDR_G, TYPE_v, WRITE }, { ADDR_E, TYPE_v, READ }} } },
	{ MAP_OPCODE, { OP_CMOVBE, {{ ADDR_G, TYPE_v, WRITE }, { ADDR_E, TYPE_v, READ }} } },
	{ MAP_OPCODE, { OP_CMOVA, {{ ADDR_G, TYPE_v, WRITE }, { ADDR_E, TYPE_v, READ }} } },

	/* 0F 48 - 0F 4F */
	{ MAP_OPCODE, { OP_CMOVS, {{ ADDR_G, TYPE_v }, { ADDR_E, TYPE_v }} } },
	{ MAP_OPCODE, { OP_CMOVNS, {{ ADDR_G, TYPE_v }, { ADDR_E, TYPE_v }} } },
	{ MAP_OPCODE, { OP_CMOVP, {{ ADDR_G, TYPE_v }, { ADDR_E, TYPE_v }} } },
	{ MAP_OPCODE, { OP_CMOVNP, {{ ADDR_G, TYPE_v }, { ADDR_E, TYPE_v }} } },
	{ MAP_OPCODE, { OP_CMOVL, {{ ADDR_G, TYPE_v }, { ADDR_E, TYPE_v }} } },
	{ MAP_OPCODE, { OP_CMOVNL, {{ ADDR_G, TYPE_v }, { ADDR_E, TYPE_v }} } },
	{ MAP_OPCODE, { OP_CMOVLE, {{ ADDR_G, TYPE_v }, { ADDR_E, TYPE_v }} } },
	{ MAP_OPCODE, { OP_CMOVNLE, {{ ADDR_G, TYPE_v }, { ADDR_E, TYPE_v }} } },

	/* 0F 50 - 0F 57 */
	{ MAP_SPECIAL, { OP_MOVMSKPS, {{ ADDR_G, TYPE_y }, { ADDR_U, TYPE_ps }} } },
	{ MAP_SPECIAL, { OP_SQRTPS, {{ ADDR_V, TYPE_ps }, { ADDR_W, TYPE_ps }} } },
	{ MAP_SPECIAL, { OP_RSQRTPS, {{ ADDR_V, TYPE_ps }, { ADDR_W, TYPE_ps }} } },
	{ MAP_SPECIAL, { OP_RCPPS, {{ ADDR_V, TYPE_ps }, { ADDR_W, TYPE_ps }} } },
	{ MAP_SPECIAL, { OP_ANDPS, {{ ADDR_V, TYPE_ps }, { ADDR_H, TYPE_ps }, { ADDR_W, TYPE_ps }} } },
	{ MAP_SPECIAL, { OP_ANDNPS, {{ ADDR_V, TYPE_ps }, { ADDR_H, TYPE_ps }, { ADDR_W, TYPE_ps }} } },
	{ MAP_SPECIAL, { OP_ORPS, {{ ADDR_V, TYPE_ps }, { ADDR_H, TYPE_ps }, { ADDR_W, TYPE_ps }} } },
	{ MAP_SPECIAL, { OP_XORPS, {{ ADDR_V, TYPE_ps }, { ADDR_H, TYPE_ps }, { ADDR_W, TYPE_ps }} } },

	/* 0F 58 - 0F 5F */
	{ MAP_SPECIAL, { OP_ADDPS, {{ ADDR_V, TYPE_ps }, { ADDR_H, TYPE_ps }, { ADDR_W, TYPE_ps }} } },
	{ MAP_SPECIAL, { OP_MULPS, {{ ADDR_V, TYPE_ps }, { ADDR_H, TYPE_ps }, { ADDR_W, TYPE_ps }} } },
	{ MAP_SPECIAL, { OP_CVTPS2PD, {{ ADDR_V, TYPE_pd }, { ADDR_W, TYPE_ps }} } },
	{ MAP_SPECIAL, { OP_CVTDQ2PS, {{ ADDR_V, TYPE_ps }, { ADDR_W, TYPE_dq }} } },
	{ MAP_SPECIAL, { OP_SUBPS, {{ ADDR_V, TYPE_ps }, { ADDR_H, TYPE_ps }, { ADDR_W, TYPE_ps }} } },
	{ MAP_SPECIAL, { OP_MINPS, {{ ADDR_V, TYPE_ps }, { ADDR_H, TYPE_ps }, { ADDR_W, TYPE_ps }} } },
	{ MAP_SPECIAL, { OP_DIVPS, {{ ADDR_V, TYPE_ps }, { ADDR_H, TYPE_ps }, { ADDR_W, TYPE_ps }} } },
	{ MAP_SPECIAL, { OP_MAXPS, {{ ADDR_V, TYPE_ps }, { ADDR_H, TYPE_ps }, { ADDR_W, TYPE_ps }} } },

	/* 0F 60 - 0F 67 */
	{ MAP_SPECIAL, { OP_PUNPCKLBW, {{ ADDR_P, TYPE_q }, { ADDR_Q, TYPE_d }} } },
	{ MAP_SPECIAL, { OP_PUNPCKLWD, {{ ADDR_P, TYPE_q }, { ADDR_Q, TYPE_d }} } },
	{ MAP_SPECIAL, { OP_PUNPCKLDQ, {{ ADDR_P, TYPE_q }, { ADDR_Q, TYPE_d }} } },
	{ MAP_SPECIAL, { OP_PACKSSWB, {{ ADDR_P, TYPE_q }, { ADDR_Q, TYPE_q }} } },
	{ MAP_SPECIAL, { OP_PCMPGTB, {{ ADDR_P, TYPE_q }, { ADDR_Q, TYPE_q }} } },
	{ MAP_SPECIAL, { OP_PCMPGTW, {{ ADDR_P, TYPE_q }, { ADDR_Q, TYPE_q }} } },
	{ MAP_SPECIAL, { OP_PCMPGTD, {{ ADDR_P, TYPE_q }, { ADDR_Q, TYPE_q }} } },
	{ MAP_SPECIAL, { OP_PACKUSWB, {{ ADDR_P, TYPE_q }, { ADDR_Q, TYPE_q }} } },

	/* 0F 68 - 0F 6F */
	{ MAP_SPECIAL, { OP_PUNPCKHBW, {{ ADDR_P, TYPE_q }, { ADDR_Q, TYPE_d }} } },
	{ MAP_SPECIAL, { OP_PUNPCKHWD, {{ ADDR_P, TYPE_q }, { ADDR_Q, TYPE_d }} } },
	{ MAP_SPECIAL, { OP_PUNPCKHDQ, {{ ADDR_P, TYPE_q }, { ADDR_Q, TYPE_d }} } },
	{ MAP_SPECIAL, { OP_PACKSSDW, {{ ADDR_P, TYPE_q }, { ADDR_Q, TYPE_d }} } },
	{ MAP_INVALID },
	{ MAP_INVALID },
	{ MAP_SPECIAL, { OP_MOVD, {{ ADDR_P, TYPE_d, WRITE }, { ADDR_E, TYPE_y, READ }} } },
	{ MAP_SPECIAL, { OP_MOVQ, {{ ADDR_P, TYPE_d, WRITE }, { ADDR_Q, TYPE_q, READ }} } },

	/* 0F 70 - 0F 77 */
	{ MAP_SPECIAL, { OP_PSHUFW, {{ ADDR_P, TYPE_q }, { ADDR_Q, TYPE_q }, { ADDR_I, TYPE_b }} } },
	{ MAP_GROUP, (int)&group_12 },
	{ MAP_GROUP, (int)&group_13 },
	{ MAP_GROUP, (int)&group_14 },
	{ MAP_SPECIAL, { OP_PCMPEQB, {{ ADDR_P, TYPE_q }, { ADDR_Q, TYPE_q }} } },
	{ MAP_SPECIAL, { OP_PCMPEQW, {{ ADDR_P, TYPE_q }, { ADDR_Q, TYPE_q }} } },
	{ MAP_SPECIAL, { OP_PCMPEQD, {{ ADDR_P, TYPE_q }, { ADDR_Q, TYPE_q }} } },
	{ MAP_SPECIAL, { OP_EMMS } },

	/* 0F 78 - 0F 7F */
	{ MAP_OPCODE, { OP_VMREAD, {{ ADDR_E, TYPE_y }, { ADDR_G, TYPE_y }} } },
	{ MAP_OPCODE, { OP_VMWRITE, {{ ADDR_G, TYPE_y }, { ADDR_E, TYPE_y }} } },
	{ MAP_INVALID },
	{ MAP_INVALID },
	{ MAP_SPECIAL },
	{ MAP_SPECIAL },
	{ MAP_SPECIAL, { OP_MOVD, {{ ADDR_E, TYPE_y, WRITE }, { ADDR_P, TYPE_d, READ }} } },
	{ MAP_SPECIAL, { OP_MOVQ, {{ ADDR_Q, TYPE_q, WRITE }, { ADDR_P, TYPE_q, READ }} } },

	/* 0F 80 - 0F 87 */
	{ MAP_OPCODE, { OP_JO, {{ ADDR_J, TYPE_z }} } },
	{ MAP_OPCODE, { OP_JNO, {{ ADDR_J, TYPE_z }} } },
	{ MAP_OPCODE, { OP_JB, {{ ADDR_J, TYPE_z }} } },
	{ MAP_OPCODE, { OP_JNB, {{ ADDR_J, TYPE_z }} } },
	{ MAP_OPCODE, { OP_JZ, {{ ADDR_J, TYPE_z }} } },
	{ MAP_OPCODE, { OP_JNZ, {{ ADDR_J, TYPE_z }} } },
	{ MAP_OPCODE, { OP_JBE, {{ ADDR_J, TYPE_z }} } },
	{ MAP_OPCODE, { OP_JNBE, {{ ADDR_J, TYPE_z }} } },

	/* 0F 87 - 0F 8F */
	{ MAP_OPCODE, { OP_JS, {{ ADDR_J, TYPE_z }} } },
	{ MAP_OPCODE, { OP_JNS, {{ ADDR_J, TYPE_z }} } },
	{ MAP_OPCODE, { OP_JP, {{ ADDR_J, TYPE_z }} } },
	{ MAP_OPCODE, { OP_JNP, {{ ADDR_J, TYPE_z }} } },
	{ MAP_OPCODE, { OP_JL, {{ ADDR_J, TYPE_z }} } },
	{ MAP_OPCODE, { OP_JNL, {{ ADDR_J, TYPE_z }} } },
	{ MAP_OPCODE, { OP_JLE, {{ ADDR_J, TYPE_z }} } },
	{ MAP_OPCODE, { OP_JNLE, {{ ADDR_J, TYPE_z }} } },

	/* 0F 90 - 0F 97 */
	{ MAP_OPCODE, { OP_SETO, {{ ADDR_E, TYPE_b }} } },
	{ MAP_OPCODE, { OP_SETNO, {{ ADDR_E, TYPE_b }} } },
	{ MAP_OPCODE, { OP_SETB, {{ ADDR_E, TYPE_b }} } },
	{ MAP_OPCODE, { OP_SETNB, {{ ADDR_E, TYPE_b }} } },
	{ MAP_OPCODE, { OP_SETZ, {{ ADDR_E, TYPE_b }} } },
	{ MAP_OPCODE, { OP_SETNZ, {{ ADDR_E, TYPE_b }} } },
	{ MAP_OPCODE, { OP_SETBE, {{ ADDR_E, TYPE_b }} } },
	{ MAP_OPCODE, { OP_SETA, {{ ADDR_E, TYPE_b }} } },

	/* 0F 98 - 0F 9F */
	{ MAP_OPCODE, { OP_SETS, {{ ADDR_E, TYPE_b }} } },
	{ MAP_OPCODE, { OP_SETNS, {{ ADDR_E, TYPE_b }} } },
	{ MAP_OPCODE, { OP_SETP, {{ ADDR_E, TYPE_b }} } },
	{ MAP_OPCODE, { OP_SETNP, {{ ADDR_E, TYPE_b }} } },
	{ MAP_OPCODE, { OP_SETL, {{ ADDR_E, TYPE_b }} } },
	{ MAP_OPCODE, { OP_SETNL, {{ ADDR_E, TYPE_b }} } },
	{ MAP_OPCODE, { OP_SETLE, {{ ADDR_E, TYPE_b }} } },
	{ MAP_OPCODE, { OP_SETNLE, {{ ADDR_E, TYPE_b }} } },

	/* 0F A0 - 0F A7 */
	{ MAP_OPCODE, { OP_PUSH, {{ ADDR_GIVEN+REG_FS, TYPE_w, READ }, { ADDR_GIVEN+REG_ESP, TYPE_v, (OPERAND_ACCESS)(REFERENCE+WRITE) }} } },
	{ MAP_OPCODE, { OP_POP, {{ ADDR_GIVEN+REG_FS, TYPE_w, WRITE }, { ADDR_GIVEN+REG_ESP, TYPE_v, (OPERAND_ACCESS)(REFERENCE+READWRITE) }} } },
	{ MAP_OPCODE, { OP_CPUID, {{ ADDR_GIVEN+REG_EAX+REG_EBX+REG_ECX+REG_EDX, TYPE_v, WRITE }, { ADDR_GIVEN+REG_EAX, TYPE_v, READ }} } },
	{ MAP_OPCODE, { OP_BT, {{ ADDR_E, TYPE_v, WRITE }, { ADDR_G, TYPE_v, READ }} } },
	{ MAP_OPCODE, { OP_SHLD, {{ ADDR_E, TYPE_v, WRITE }, { ADDR_G, TYPE_v, READ }, { ADDR_I, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_SHLD, {{ ADDR_E, TYPE_v, WRITE }, { ADDR_G, TYPE_v, READ }, { ADDR_GIVEN+REG_CL, TYPE_b, READ }} } },
	{ MAP_INVALID }, //WELL ITS FOR ANOTHER PROC
	{ MAP_INVALID }, //ANOTHER PROC

	/* 0F A8 - 0F AF */
	{ MAP_OPCODE, { OP_PUSH, {{ ADDR_GIVEN+REG_GS, TYPE_w, READ }, { ADDR_GIVEN+REG_ESP, TYPE_v, (OPERAND_ACCESS)(REFERENCE+WRITE) }} } },
	{ MAP_OPCODE, { OP_POP, {{ ADDR_GIVEN+REG_GS, TYPE_w, WRITE }, { ADDR_GIVEN+REG_ESP, TYPE_v, (OPERAND_ACCESS)(REFERENCE+READWRITE) }} } },
	{ MAP_OPCODE, { OP_RSM } },
	{ MAP_OPCODE, { OP_BTS, {{ ADDR_E, TYPE_v, WRITE }, { ADDR_G, TYPE_v, READ }} } }, //XXX
	{ MAP_OPCODE, { OP_SHRD, {{ ADDR_E, TYPE_v, WRITE }, { ADDR_G, TYPE_v, READ }, { ADDR_I, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_SHRD, {{ ADDR_E, TYPE_v, WRITE }, { ADDR_G, TYPE_v, READ }, { ADDR_GIVEN+REG_CL, TYPE_b, READ }} } },
	{ MAP_GROUP, (int)&group_15 },
	{ MAP_OPCODE, { OP_IMUL, {{ ADDR_G, TYPE_v, WRITE }, { ADDR_E, TYPE_v }} } },

	/* 0F B0 - 0F B7 */
	{ MAP_OPCODE, { OP_CMPXCHG, {{ ADDR_E, TYPE_b, WRITE }, { ADDR_G, TYPE_b, WRITE }} } },
	{ MAP_OPCODE, { OP_CMPXCHG, {{ ADDR_E, TYPE_v, WRITE }, { ADDR_G, TYPE_v, WRITE }} } },
	{ MAP_OPCODE, { OP_LSS, {{ ADDR_G, TYPE_v, WRITE }, { ADDR_M, TYPE_p, READ }, { ADDR_GIVEN+REG_SS, TYPE_w, WRITE }} } },
	{ MAP_OPCODE, { OP_BTR, {{ ADDR_E, TYPE_v, READ }, { ADDR_G, TYPE_v, READ }} } },
	{ MAP_OPCODE, { OP_LFS, {{ ADDR_G, TYPE_v, WRITE }, { ADDR_M, TYPE_p, READ }, { ADDR_GIVEN+REG_FS, TYPE_w, WRITE }} } },
	{ MAP_OPCODE, { OP_LGS, {{ ADDR_G, TYPE_v, WRITE }, { ADDR_M, TYPE_p, READ }, { ADDR_GIVEN+REG_GS, TYPE_w, WRITE }} } },
	{ MAP_OPCODE, { OP_MOVZX, {{ ADDR_G, TYPE_v, WRITE }, { ADDR_E, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_MOVZX, {{ ADDR_G, TYPE_v, WRITE }, { ADDR_E, TYPE_w, READ }} } },

	/* 0F B8 - 0F BF */
	{ MAP_SPECIAL }, //JMPE (INTERESTING INSTRUCTION)
	{ MAP_GROUP, (int)&group_10 },
	{ MAP_GROUP, (int)&group_8, {{ ADDR_E, TYPE_v, WRITE }, { ADDR_I, TYPE_b }} },
	{ MAP_OPCODE, { OP_BTC, {{ ADDR_E, TYPE_v, READ }, { ADDR_G, TYPE_v, READ }} } },
	{ MAP_SPECIAL, { OP_BSF, {{ ADDR_G, TYPE_v, WRITE }, { ADDR_E, TYPE_v, READ }} } },
	{ MAP_SPECIAL, { OP_BSR, {{ ADDR_G, TYPE_v, WRITE }, { ADDR_E, TYPE_v, READ }} } },
	{ MAP_OPCODE, { OP_MOVSX, {{ ADDR_G, TYPE_v, WRITE }, { ADDR_E, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_MOVSX, {{ ADDR_G, TYPE_v, WRITE }, { ADDR_E, TYPE_w, READ }} } },

	/* 0F C0 - 0F C7 */
	{ MAP_OPCODE, { OP_XADD, {{ ADDR_E, TYPE_b, WRITE }, { ADDR_G, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_XADD, {{ ADDR_E, TYPE_v, WRITE }, { ADDR_G, TYPE_v, READ }} } },
	{ MAP_SPECIAL, { OP_CMPPS, {{ ADDR_V, TYPE_ps, WRITE }, { ADDR_W, TYPE_ps, READ }, { ADDR_I, TYPE_b }} } }, //XXX HAS VEX SPECIAL
	{ MAP_OPCODE, { OP_MOVNTI, {{ ADDR_M, TYPE_y, WRITE }, { ADDR_G, TYPE_y, READ }} } },
	{ MAP_SPECIAL, { OP_PINSRW, {{ ADDR_P, TYPE_q, WRITE }, { ADDR_M, TYPE_w, READ }, { ADDR_I, TYPE_b, READ }} } }, //XXX SPECIAL
	{ MAP_SPECIAL, { OP_PEXTRW, {{ ADDR_G, TYPE_d, WRITE }, { ADDR_N, TYPE_q, READ }, { ADDR_I, TYPE_b, READ }} } },
	{ MAP_SPECIAL, { OP_SHUFPS, {{ ADDR_H, TYPE_ps, WRITE }, { ADDR_W, TYPE_ps, READ }, { ADDR_I, TYPE_b, READ }} } },
	{ MAP_GROUP, (int)&group_9 },

	/* 0F C8 - 0F CF */
	{ MAP_OPCODE, { OP_BSWAP, {{ADDR_GIVEN+REG_EAX, READWRITE}} } },
	{ MAP_OPCODE, { OP_BSWAP, {{ADDR_GIVEN+REG_ECX, READWRITE}} } },
	{ MAP_OPCODE, { OP_BSWAP, {{ADDR_GIVEN+REG_EDX, READWRITE}} } },
	{ MAP_OPCODE, { OP_BSWAP, {{ADDR_GIVEN+REG_EBX, READWRITE}} } },
	{ MAP_OPCODE, { OP_BSWAP, {{ADDR_GIVEN+REG_ESP, READWRITE}} } },
	{ MAP_OPCODE, { OP_BSWAP, {{ADDR_GIVEN+REG_EBP, READWRITE}} } },
	{ MAP_OPCODE, { OP_BSWAP, {{ADDR_GIVEN+REG_ESI, READWRITE}} } },
	{ MAP_OPCODE, { OP_BSWAP, {{ADDR_GIVEN+REG_EDI, READWRITE}} } },

	/* 0F D0 - 0F D7 */
	{ MAP_SPECIAL, { NULL } }, //XXX
	{ MAP_SPECIAL, { OP_PSRLW, {{ ADDR_P, TYPE_q, WRITE }, { ADDR_Q, TYPE_p, READ }} } },
	{ MAP_SPECIAL, { OP_PSRLD, {{ ADDR_P, TYPE_q, WRITE }, { ADDR_Q, TYPE_p, READ }} } },
	{ MAP_SPECIAL, { OP_PSRLQ, {{ ADDR_P, TYPE_q, WRITE }, { ADDR_Q, TYPE_p, READ }} } },
	{ MAP_SPECIAL, { OP_PADDQ, {{ ADDR_P, TYPE_q, WRITE }, { ADDR_Q, TYPE_p, READ }} } },
	{ MAP_SPECIAL, { OP_PMULLW, {{ ADDR_P, TYPE_q, WRITE }, { ADDR_Q, TYPE_p, READ }} } },
	{ MAP_SPECIAL, { NULL } }, //XXX
	{ MAP_SPECIAL, { OP_PMOVMSKB, {{ ADDR_G, TYPE_d, WRITE }, { ADDR_N, TYPE_q, READ }} } },

	/* 0F D8 - 0F DF */
	{ MAP_SPECIAL, { OP_PSUBUSB, {{ ADDR_P, TYPE_q, WRITE }, { ADDR_Q, TYPE_q }} } },
	{ MAP_SPECIAL, { OP_PSUBUSW, {{ ADDR_P, TYPE_q, WRITE }, { ADDR_Q, TYPE_q }} } },
	{ MAP_SPECIAL, { OP_PMINUB, {{ ADDR_P, TYPE_q, WRITE }, { ADDR_Q, TYPE_q }} } },
	{ MAP_SPECIAL, { OP_PAND, {{ ADDR_P, TYPE_q, WRITE }, { ADDR_Q, TYPE_q }} } },
	{ MAP_SPECIAL, { OP_PADDUSB, {{ ADDR_P, TYPE_q, WRITE }, { ADDR_Q, TYPE_q }} } },
	{ MAP_SPECIAL, { OP_PADDUSW, {{ ADDR_P, TYPE_q, WRITE }, { ADDR_Q, TYPE_q }} } },
	{ MAP_SPECIAL, { OP_PMAXUB, {{ ADDR_P, TYPE_q, WRITE }, { ADDR_Q, TYPE_q }} } },
	{ MAP_SPECIAL, { OP_PANDN, {{ ADDR_P, TYPE_q, WRITE }, { ADDR_Q, TYPE_q }} } },

	/* 0F E0 - 0F E7 */
	{ MAP_SPECIAL, { OP_PAVGB, {{ ADDR_P, TYPE_q, WRITE }, { ADDR_Q, TYPE_q, READ }} } },
	{ MAP_SPECIAL, { OP_PSRAW, {{ ADDR_P, TYPE_q, WRITE }, { ADDR_Q, TYPE_q, READ }} } },
	{ MAP_SPECIAL, { OP_PSRAD, {{ ADDR_P, TYPE_q, WRITE }, { ADDR_Q, TYPE_q, READ }} } },
	{ MAP_SPECIAL, { OP_PAVGW, {{ ADDR_P, TYPE_q, WRITE }, { ADDR_Q, TYPE_q, READ }} } },
	{ MAP_SPECIAL, { OP_PMULHUW, {{ ADDR_P, TYPE_q, WRITE }, { ADDR_Q, TYPE_q, READ }} } },
	{ MAP_SPECIAL, { OP_PMULHW, {{ ADDR_P, TYPE_q, WRITE }, { ADDR_Q, TYPE_q, READ }} } },
	{ MAP_SPECIAL, { NULL } },
	{ MAP_SPECIAL, { OP_MOVNTQ, {{ ADDR_P, TYPE_q, WRITE }, { ADDR_Q, TYPE_q, READ }} } },

	/* 0F E8 - 0F EF */
	{ MAP_SPECIAL, { OP_PSUBSB, {{ ADDR_P, TYPE_q, WRITE }, { ADDR_Q, TYPE_q, READ }} } },
	{ MAP_SPECIAL, { OP_PSUBSW, {{ ADDR_P, TYPE_q, WRITE }, { ADDR_Q, TYPE_q, READ }} } },
	{ MAP_SPECIAL, { OP_PMINSW, {{ ADDR_P, TYPE_q, WRITE }, { ADDR_Q, TYPE_q, READ }} } },
	{ MAP_SPECIAL, { OP_POR, {{ ADDR_P, TYPE_q, WRITE }, { ADDR_Q, TYPE_q, READ }} } },
	{ MAP_SPECIAL, { OP_PADDSB, {{ ADDR_P, TYPE_q, WRITE }, { ADDR_Q, TYPE_q, READ }} } },
	{ MAP_SPECIAL, { OP_PADDSW, {{ ADDR_P, TYPE_q, WRITE }, { ADDR_Q, TYPE_q, READ }} } },
	{ MAP_SPECIAL, { OP_PMAXSW, {{ ADDR_P, TYPE_q, WRITE }, { ADDR_Q, TYPE_q, READ }} } },
	{ MAP_SPECIAL, { OP_PXOR, {{ ADDR_P, TYPE_q, WRITE }, { ADDR_Q, TYPE_q, READ }} } },

	/* 0F F0 - 0F F7 */
	{ MAP_SPECIAL, { NULL } },
	{ MAP_SPECIAL, { OP_PSLLW, {{ ADDR_P, TYPE_q, WRITE }, { ADDR_Q, TYPE_q, READ }} } },
	{ MAP_SPECIAL, { OP_PSLLD, {{ ADDR_P, TYPE_q, WRITE }, { ADDR_Q, TYPE_q, READ }} } },
	{ MAP_SPECIAL, { OP_PSLLQ, {{ ADDR_P, TYPE_q, WRITE }, { ADDR_Q, TYPE_q, READ }} } },
	{ MAP_SPECIAL, { OP_PMULUDQ, {{ ADDR_P, TYPE_q, WRITE }, { ADDR_Q, TYPE_q, READ }} } },
	{ MAP_SPECIAL, { OP_PMADDWD, {{ ADDR_P, TYPE_q, WRITE }, { ADDR_Q, TYPE_q, READ }} } },
	{ MAP_SPECIAL, { OP_PSADBW, {{ ADDR_P, TYPE_q, WRITE }, { ADDR_Q, TYPE_q, READ }} } },
	{ MAP_SPECIAL, { OP_MASKMOVQ, {{ ADDR_P, TYPE_q, WRITE }, { ADDR_N, TYPE_q, READ }} } },

	/* 0F F8 - 0F FF */
	{ MAP_SPECIAL, { OP_PSUBB, {{ ADDR_P, TYPE_q, WRITE }, { ADDR_Q, TYPE_q, READ }} } },
	{ MAP_SPECIAL, { OP_PSUBW, {{ ADDR_P, TYPE_q, WRITE }, { ADDR_Q, TYPE_q, READ }} } },
	{ MAP_SPECIAL, { OP_PSUBD, {{ ADDR_P, TYPE_q, WRITE }, { ADDR_Q, TYPE_q, READ }} } },
	{ MAP_SPECIAL, { OP_PSUBQ, {{ ADDR_P, TYPE_q, WRITE }, { ADDR_Q, TYPE_q, READ }} } },
	{ MAP_SPECIAL, { OP_PADDB, {{ ADDR_P, TYPE_q, WRITE }, { ADDR_Q, TYPE_q, READ }} } },
	{ MAP_SPECIAL, { OP_PADDW, {{ ADDR_P, TYPE_q, WRITE }, { ADDR_Q, TYPE_q, READ }} } },
	{ MAP_SPECIAL, { OP_PADDD, {{ ADDR_P, TYPE_q, WRITE }, { ADDR_Q, TYPE_q, READ }} } },
	{ MAP_INVALID },
};

const GROUP_MAP group_1_map = { 
	MODRM_REGBITS_MASK,
	MODRM_REGBITS_SHIFT,
	{
		{ MAP_OPCODE, { OP_ADD } },
		{ MAP_OPCODE, { OP_OR } },
		{ MAP_OPCODE, { OP_ADC } },
		{ MAP_OPCODE, { OP_SBB } },
		{ MAP_OPCODE, { OP_AND } },
		{ MAP_OPCODE, { OP_SUB } },
		{ MAP_OPCODE, { OP_XOR } },
		{ MAP_OPCODE, { OP_CMP } },
	}
};

const GROUP_MAP group_1_1A_map = {
	MODRM_REGBITS_MASK,
	MODRM_REGBITS_SHIFT,
	{
		{ MAP_OPCODE, { OP_POP } },
		ARRAY_FILL( 0, 0, 7, { MAP_INVALID } )
	}
};


const GROUP_MAP group_2 = {
	MODRM_REGBITS_MASK,
	MODRM_REGBITS_SHIFT,
	{
		{ MAP_OPCODE, { OP_ROL } },
		{ MAP_OPCODE, { OP_ROR } },
		{ MAP_OPCODE, { OP_RCL } },
		{ MAP_OPCODE, { OP_RCR } },
		{ MAP_OPCODE, { OP_SHL } },
		{ MAP_OPCODE, { OP_SHR } },
		{ MAP_OPCODE, { OP_SAL } },
		{ MAP_OPCODE, { OP_SAR } }
	}
};

//XXX need to implement XABORT when MOD = 11
const  GROUP_MAP group_11 = {
	MODRM_REGBITS_MASK,
	MODRM_REGBITS_SHIFT,
	{
		{ MAP_OPCODE, OP_MOV, {{ ADDR_E, TYPE_b, WRITE }, { ADDR_I, TYPE_b, READ }} },
		{ MAP_INVALID },
		{ MAP_INVALID },
		{ MAP_INVALID },
		{ MAP_INVALID },
		{ MAP_INVALID },
		{ MAP_INVALID },
		{ MAP_INVALID }
	}
};

//XXX need to implement these, for now MAP_INVALID
const  PLUGIN_MAP fp_esc_0[] = {
	ARRAY_FILL( 2, 5, 6, {MAP_INVALID} )
};
const  PLUGIN_MAP fp_esc_1[] = {
	ARRAY_FILL( 2, 5, 6, {MAP_INVALID} )
};
const  PLUGIN_MAP fp_esc_2[] = {
	ARRAY_FILL( 2, 5, 6, {MAP_INVALID} )
};
const  PLUGIN_MAP fp_esc_3[] = {
	ARRAY_FILL( 2, 5, 6, {MAP_INVALID} )
};
const  PLUGIN_MAP fp_esc_4[] = {
	ARRAY_FILL( 2, 5, 6, {MAP_INVALID} )
};
const  PLUGIN_MAP fp_esc_5[] = {
	ARRAY_FILL( 2, 5, 6, {MAP_INVALID} )
};
const  PLUGIN_MAP fp_esc_6[] = {
	ARRAY_FILL( 2, 5, 6, {MAP_INVALID} )
};
const  PLUGIN_MAP fp_esc_7[] = {
	ARRAY_FILL( 2, 5, 6, {MAP_INVALID} )
};

const  GROUP_MAP unary_group_3_1 = {
	MODRM_REGBITS_MASK,
	MODRM_REGBITS_SHIFT,
	{
		{ MAP_OPCODE, { OP_TEST, {{ ADDR_E, TYPE_b, READ }, { ADDR_I, TYPE_b, READ }} } },
		{ MAP_OPCODE, { OP_TEST, {{ ADDR_E, TYPE_b, READ }, { ADDR_I, TYPE_b, READ }} } },
		{ MAP_OPCODE, { OP_NOT, {{ ADDR_E, TYPE_b, READWRITE }} } },
		{ MAP_OPCODE, { OP_NEG, {{ ADDR_E, TYPE_b, READWRITE }} } },
		{ MAP_OPCODE, { OP_MUL, {{ ADDR_E, TYPE_b, READ }} } }, //XXX affects EDX, EAX
		{ MAP_OPCODE, { OP_IMUL, {{ ADDR_E, TYPE_b, READ }} } },
		{ MAP_OPCODE, { OP_DIV, {{ ADDR_E, TYPE_b, READ }} } },
		{ MAP_OPCODE, { OP_IDIV, {{ ADDR_E, TYPE_b, READ }} } },
	}
};

const  GROUP_MAP unary_group_3_2 = {
	MODRM_REGBITS_MASK,
	MODRM_REGBITS_SHIFT,
	{
		{ MAP_OPCODE, { OP_TEST, {{ ADDR_E, TYPE_v, READ }, { ADDR_I, TYPE_z, READ }} } },
		{ MAP_OPCODE, { OP_TEST, {{ ADDR_E, TYPE_v, READ }, { ADDR_I, TYPE_z, READ }} } },
		{ MAP_OPCODE, { OP_NOT, {{ ADDR_E, TYPE_v, READWRITE }} } },
		{ MAP_OPCODE, { OP_NEG, {{ ADDR_E, TYPE_v, READWRITE }} } },
		{ MAP_OPCODE, { OP_MUL, {{ ADDR_E, TYPE_v, READ }} } }, //XXX affects EDX, EAX
		{ MAP_OPCODE, { OP_IMUL, {{ ADDR_E, TYPE_v, READ }} } },
		{ MAP_OPCODE, { OP_DIV, {{ ADDR_E, TYPE_v, READ }} } },
		{ MAP_OPCODE, { OP_IDIV, {{ ADDR_E, TYPE_v, READ }} } },
	}
};


const GROUP_MAP group_4 = {
	MODRM_REGBITS_MASK,
	MODRM_REGBITS_SHIFT,
	{
		{ MAP_OPCODE, { OP_INC } },
		{ MAP_OPCODE, { OP_DEC } },
		ARRAY_FILL( 0, 0, 6, { MAP_INVALID } )
	}
};

const GROUP_MAP group_5 = {
	MODRM_REGBITS_MASK,
	MODRM_REGBITS_SHIFT,
	{
		{ MAP_OPCODE, { OP_INC, {{ ADDR_E, TYPE_v, READWRITE }} } },
		{ MAP_OPCODE, { OP_DEC, {{ ADDR_E, TYPE_v, READWRITE }} } },
		{ MAP_OPCODE, { OP_CALL, {{ ADDR_E, TYPE_v, READ }} } },
		{ MAP_OPCODE, { OP_CALL, {{ ADDR_M, TYPE_p, READ }} } },
		{ MAP_OPCODE, { OP_JMP, {{ ADDR_E, TYPE_v, READ }} } },
		{ MAP_OPCODE, { OP_JMP, {{ ADDR_M, TYPE_p, READ }} } },
		{ MAP_OPCODE, { OP_PUSH, {{ ADDR_E, TYPE_v, READ }, { ADDR_GIVEN+REG_ESP, TYPE_v, (OPERAND_ACCESS)(REFERENCE+WRITE) }} } },
		{ MAP_INVALID }
	}
};


const  PLUGIN_MAP one_byte_opcode_map[] = {
	/* 00 - 07 */
	{ MAP_OPCODE, { OP_ADD,		{{ ADDR_E, TYPE_b, WRITE }, { ADDR_G, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_ADD,		{{ ADDR_E, TYPE_v, WRITE }, { ADDR_G, TYPE_v, READ }} } },
	{ MAP_OPCODE, { OP_ADD,		{{ ADDR_G, TYPE_b, WRITE }, { ADDR_E, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_ADD,		{{ ADDR_G, TYPE_v, WRITE }, { ADDR_E, TYPE_v, READ }} } },
	{ MAP_OPCODE, { OP_ADD,		{{ ADDR_GIVEN+REG_AL, TYPE_b, WRITE }, { ADDR_I, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_ADD,		{{ ADDR_GIVEN+REG_EAX, TYPE_z, WRITE }, { ADDR_I, TYPE_z, READ }} } },
	{ MAP_OPCODE, { OP_PUSH,	{{ ADDR_GIVEN+REG_ES, TYPE_w, READ }, { ADDR_GIVEN+REG_ESP, TYPE_v, (OPERAND_ACCESS)(REFERENCE+WRITE) }} } },
	{ MAP_OPCODE, { OP_POP,		{{ ADDR_GIVEN+REG_ES, TYPE_w, WRITE }, { ADDR_GIVEN+REG_ESP, TYPE_v, (OPERAND_ACCESS)(REFERENCE+READWRITE) }} } },

	// 08 - 0F
	{ MAP_OPCODE, { OP_OR,		{{ ADDR_E, TYPE_b, WRITE }, { ADDR_G, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_OR,		{{ ADDR_E, TYPE_v, WRITE }, { ADDR_G, TYPE_v, READ }} } },
	{ MAP_OPCODE, { OP_OR,		{{ ADDR_G, TYPE_b, WRITE }, { ADDR_E, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_OR,		{{ ADDR_G, TYPE_v, WRITE }, { ADDR_E, TYPE_v, READ }} } },
	{ MAP_OPCODE, { OP_OR,		{{ ADDR_GIVEN+REG_AL, TYPE_b, WRITE }, { ADDR_I, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_OR,		{{ ADDR_GIVEN+REG_EAX, TYPE_z, WRITE }, { ADDR_I, TYPE_z, READ }} } },
	{ MAP_OPCODE, { OP_PUSH,	{{ ADDR_GIVEN+REG_CS, TYPE_w, READ }, { ADDR_GIVEN+REG_ESP, TYPE_v, (OPERAND_ACCESS)(REFERENCE+WRITE) }} } },
	{ MAP_ESCAPE, (int)&two_byte_opcode_map },

	/* 10 - 17 */
	{ MAP_OPCODE, { OP_ADC,		{{ ADDR_E, TYPE_b, WRITE }, { ADDR_G, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_ADC,		{{ ADDR_E, TYPE_v, WRITE }, { ADDR_G, TYPE_v, READ }} } },
	{ MAP_OPCODE, { OP_ADC,		{{ ADDR_G, TYPE_b, WRITE }, { ADDR_E, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_ADC,		{{ ADDR_G, TYPE_v, WRITE }, { ADDR_E, TYPE_v, READ }} } },
	{ MAP_OPCODE, { OP_ADC,		{{ ADDR_GIVEN+REG_AL, TYPE_b, WRITE }, { ADDR_I, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_ADC,		{{ ADDR_GIVEN+REG_EAX, TYPE_z, WRITE }, { ADDR_I, TYPE_z, READ }} } },
	{ MAP_OPCODE, { OP_PUSH,	{{ ADDR_GIVEN+REG_SS, TYPE_w, READ }, { ADDR_GIVEN+REG_ESP, TYPE_v, (OPERAND_ACCESS)(REFERENCE+WRITE) }} } },
	{ MAP_OPCODE, { OP_POP,		{{ ADDR_GIVEN+REG_SS, TYPE_w, WRITE }, { ADDR_GIVEN+REG_ESP, TYPE_v, (OPERAND_ACCESS)(REFERENCE+READWRITE) }} } },

	// 18 - 1F
	{ MAP_OPCODE, { OP_SBB,		{{ ADDR_E, TYPE_b, WRITE }, { ADDR_G, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_SBB,		{{ ADDR_E, TYPE_v, WRITE }, { ADDR_G, TYPE_v, READ }} } },
	{ MAP_OPCODE, { OP_SBB,		{{ ADDR_G, TYPE_b, WRITE }, { ADDR_E, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_SBB,		{{ ADDR_G, TYPE_v, WRITE }, { ADDR_E, TYPE_v, READ }} } },
	{ MAP_OPCODE, { OP_SBB,		{{ ADDR_GIVEN+REG_AL, TYPE_b, WRITE }, { ADDR_I, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_SBB,		{{ ADDR_GIVEN+REG_EAX, TYPE_z, WRITE }, { ADDR_I, TYPE_z, READ }} } },
	{ MAP_OPCODE, { OP_PUSH,	{{ ADDR_GIVEN+REG_DS, TYPE_w, READ }, { ADDR_GIVEN+REG_ESP, TYPE_v, (OPERAND_ACCESS)(REFERENCE+WRITE) }} } },
	{ MAP_OPCODE, { OP_POP,		{{ ADDR_GIVEN+REG_DS, TYPE_w, WRITE }, { ADDR_GIVEN+REG_ESP, TYPE_v, (OPERAND_ACCESS)(REFERENCE+READWRITE) }} } },

	/* 20 - 27 */
	{ MAP_OPCODE, { OP_AND,		{{ ADDR_E, TYPE_b, WRITE }, { ADDR_G, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_AND,		{{ ADDR_E, TYPE_v, WRITE }, { ADDR_G, TYPE_v, READ }} } },
	{ MAP_OPCODE, { OP_AND,		{{ ADDR_G, TYPE_b, WRITE }, { ADDR_E, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_AND,		{{ ADDR_G, TYPE_v, WRITE }, { ADDR_E, TYPE_v, READ }} } },
	{ MAP_OPCODE, { OP_AND,		{{ ADDR_GIVEN+REG_AL, TYPE_b, WRITE }, { ADDR_I, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_AND,		{{ ADDR_GIVEN+REG_EAX, TYPE_z, WRITE }, { ADDR_I, TYPE_z, READ }} } },
	{ MAP_PREFIX, (int)&intel_prefix },
	{ MAP_OPCODE, { OP_DAA,		{{ ADDR_GIVEN+REG_AL, TYPE_b, WRITE }} } },

	// 28 - 2F
	{ MAP_OPCODE, { OP_SUB,		{{ ADDR_E, TYPE_b, WRITE }, { ADDR_G, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_SUB,		{{ ADDR_E, TYPE_v, WRITE }, { ADDR_G, TYPE_v, READ }} } },
	{ MAP_OPCODE, { OP_SUB,		{{ ADDR_G, TYPE_b, WRITE }, { ADDR_E, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_SUB,		{{ ADDR_G, TYPE_v, WRITE }, { ADDR_E, TYPE_v, READ }} } },
	{ MAP_OPCODE, { OP_SUB,		{{ ADDR_GIVEN+REG_AL, TYPE_b, WRITE }, { ADDR_I, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_SUB,		{{ ADDR_GIVEN+REG_EAX, TYPE_z, WRITE }, { ADDR_I, TYPE_z, READ }} } },
	{ MAP_PREFIX, (int)&intel_prefix },
	{ MAP_OPCODE, { OP_DAS,		{{ ADDR_GIVEN+REG_AL, TYPE_b, WRITE }} } },

	/* 30 - 37 */
	{ MAP_OPCODE, { OP_XOR,		{{ ADDR_E, TYPE_b, WRITE }, { ADDR_G, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_XOR,		{{ ADDR_E, TYPE_v, WRITE }, { ADDR_G, TYPE_v, READ }} } },
	{ MAP_OPCODE, { OP_XOR,		{{ ADDR_G, TYPE_b, WRITE }, { ADDR_E, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_XOR,		{{ ADDR_G, TYPE_v, WRITE }, { ADDR_E, TYPE_v, READ }} } },
	{ MAP_OPCODE, { OP_XOR,		{{ ADDR_GIVEN+REG_AL, TYPE_b, WRITE }, { ADDR_I, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_XOR,		{{ ADDR_GIVEN+REG_EAX, TYPE_z, WRITE }, { ADDR_I, TYPE_z, READ }} } },
	{ MAP_PREFIX, (int)&intel_prefix },
	{ MAP_OPCODE, { OP_AAA,		{{ ADDR_GIVEN+REG_AX, TYPE_w, WRITE }, {ADDR_GIVEN+REG_AL, TYPE_b, READ }} } },

	// 38 - 3F
	{ MAP_OPCODE, { OP_CMP,		{{ ADDR_E, TYPE_b, WRITE }, { ADDR_G, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_CMP,		{{ ADDR_E, TYPE_v, WRITE }, { ADDR_G, TYPE_v, READ }} } },
	{ MAP_OPCODE, { OP_CMP,		{{ ADDR_G, TYPE_b, WRITE }, { ADDR_E, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_CMP,		{{ ADDR_G, TYPE_v, WRITE }, { ADDR_E, TYPE_v, READ }} } },
	{ MAP_OPCODE, { OP_CMP,		{{ ADDR_GIVEN+REG_AL, TYPE_b, WRITE }, { ADDR_I, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_CMP,		{{ ADDR_GIVEN+REG_EAX, TYPE_z, WRITE }, { ADDR_I, TYPE_z, READ }} } },
	{ MAP_PREFIX, (int)&intel_prefix },
	{ MAP_OPCODE, { OP_AAS,		{{ ADDR_GIVEN+REG_AX, TYPE_w, WRITE }, { ADDR_GIVEN+REG_AL, TYPE_b, READ }} } },

	/* 40 - 47 */
	{ MAP_OPCODE, { OP_INC,		{{ ADDR_GIVEN+REG_EAX, TYPE_z, READWRITE }} } },
	{ MAP_OPCODE, { OP_INC,		{{ ADDR_GIVEN+REG_ECX, TYPE_z, READWRITE }} } },
	{ MAP_OPCODE, { OP_INC,		{{ ADDR_GIVEN+REG_EDX, TYPE_z, READWRITE }} } },
	{ MAP_OPCODE, { OP_INC,		{{ ADDR_GIVEN+REG_EBX, TYPE_z, READWRITE }} } },
	{ MAP_OPCODE, { OP_INC,		{{ ADDR_GIVEN+REG_ESP, TYPE_z, READWRITE }} } },
	{ MAP_OPCODE, { OP_INC,		{{ ADDR_GIVEN+REG_EBP, TYPE_z, READWRITE }} } },
	{ MAP_OPCODE, { OP_INC,		{{ ADDR_GIVEN+REG_ESI, TYPE_z, READWRITE }} } },
	{ MAP_OPCODE, { OP_INC,		{{ ADDR_GIVEN+REG_EDI, TYPE_z, READWRITE }} } },

	/* 48 - 4F */
	{ MAP_OPCODE, { OP_DEC,		{{ ADDR_GIVEN+REG_EAX, TYPE_z, READWRITE }} } },
	{ MAP_OPCODE, { OP_DEC,		{{ ADDR_GIVEN+REG_ECX, TYPE_z, READWRITE }} } },
	{ MAP_OPCODE, { OP_DEC,		{{ ADDR_GIVEN+REG_EDX, TYPE_z, READWRITE }} } },
	{ MAP_OPCODE, { OP_DEC,		{{ ADDR_GIVEN+REG_EBX, TYPE_z, READWRITE }} } },
	{ MAP_OPCODE, { OP_DEC,		{{ ADDR_GIVEN+REG_ESP, TYPE_z, READWRITE }} } },
	{ MAP_OPCODE, { OP_DEC,		{{ ADDR_GIVEN+REG_EBP, TYPE_z, READWRITE }} } },
	{ MAP_OPCODE, { OP_DEC,		{{ ADDR_GIVEN+REG_ESI, TYPE_z, READWRITE }} } },
	{ MAP_OPCODE, { OP_DEC,		{{ ADDR_GIVEN+REG_EDI, TYPE_z, READWRITE }} } },

	/* 50 - 57 */
	{ MAP_OPCODE, { OP_PUSH,	{{ ADDR_GIVEN+REG_EAX, TYPE_z, READ }, { ADDR_GIVEN+REG_ESP, TYPE_z, (OPERAND_ACCESS)(REFERENCE+WRITE) }} } },
	{ MAP_OPCODE, { OP_PUSH,	{{ ADDR_GIVEN+REG_ECX, TYPE_z, READ }, { ADDR_GIVEN+REG_ESP, TYPE_z, (OPERAND_ACCESS)(REFERENCE+WRITE) }} } },
	{ MAP_OPCODE, { OP_PUSH,	{{ ADDR_GIVEN+REG_EDX, TYPE_z, READ }, { ADDR_GIVEN+REG_ESP, TYPE_z, (OPERAND_ACCESS)(REFERENCE+WRITE) }} } },
	{ MAP_OPCODE, { OP_PUSH,	{{ ADDR_GIVEN+REG_EBX, TYPE_z, READ }, { ADDR_GIVEN+REG_ESP, TYPE_z, (OPERAND_ACCESS)(REFERENCE+WRITE) }} } },
	{ MAP_OPCODE, { OP_PUSH,	{{ ADDR_GIVEN+REG_ESP, TYPE_z, READ }, { ADDR_GIVEN+REG_ESP, TYPE_z, (OPERAND_ACCESS)(REFERENCE+WRITE) }} } },
	{ MAP_OPCODE, { OP_PUSH,	{{ ADDR_GIVEN+REG_EBP, TYPE_z, READ }, { ADDR_GIVEN+REG_ESP, TYPE_z, (OPERAND_ACCESS)(REFERENCE+WRITE) }} } },
	{ MAP_OPCODE, { OP_PUSH,	{{ ADDR_GIVEN+REG_ESI, TYPE_z, READ }, { ADDR_GIVEN+REG_ESP, TYPE_z, (OPERAND_ACCESS)(REFERENCE+WRITE) }} } },
	{ MAP_OPCODE, { OP_PUSH,	{{ ADDR_GIVEN+REG_EDI, TYPE_z, READ }, { ADDR_GIVEN+REG_ESP, TYPE_z, (OPERAND_ACCESS)(REFERENCE+WRITE) }} } },

	/* 58 - 5F */
	{ MAP_OPCODE, { OP_POP,	{{ ADDR_GIVEN+REG_EAX, TYPE_z, WRITE }, { ADDR_GIVEN+REG_ESP, TYPE_z, (OPERAND_ACCESS)(REFERENCE+READWRITE) }} } },
	{ MAP_OPCODE, { OP_POP,	{{ ADDR_GIVEN+REG_ECX, TYPE_z, WRITE }, { ADDR_GIVEN+REG_ESP, TYPE_z, (OPERAND_ACCESS)(REFERENCE+READWRITE) }} } },
	{ MAP_OPCODE, { OP_POP,	{{ ADDR_GIVEN+REG_EDX, TYPE_z, WRITE }, { ADDR_GIVEN+REG_ESP, TYPE_z, (OPERAND_ACCESS)(REFERENCE+READWRITE) }} } },
	{ MAP_OPCODE, { OP_POP,	{{ ADDR_GIVEN+REG_EBX, TYPE_z, WRITE }, { ADDR_GIVEN+REG_ESP, TYPE_z, (OPERAND_ACCESS)(REFERENCE+READWRITE) }} } },
	{ MAP_OPCODE, { OP_POP,	{{ ADDR_GIVEN+REG_ESP, TYPE_z, WRITE }, { ADDR_GIVEN+REG_ESP, TYPE_z, (OPERAND_ACCESS)(REFERENCE+READWRITE) }} } },
	{ MAP_OPCODE, { OP_POP,	{{ ADDR_GIVEN+REG_EBP, TYPE_z, WRITE }, { ADDR_GIVEN+REG_ESP, TYPE_z, (OPERAND_ACCESS)(REFERENCE+READWRITE) }} } },
	{ MAP_OPCODE, { OP_POP,	{{ ADDR_GIVEN+REG_ESI, TYPE_z, WRITE }, { ADDR_GIVEN+REG_ESP, TYPE_z, (OPERAND_ACCESS)(REFERENCE+READWRITE) }} } },
	{ MAP_OPCODE, { OP_POP,	{{ ADDR_GIVEN+REG_EDI, TYPE_z, WRITE }, { ADDR_GIVEN+REG_ESP, TYPE_z, (OPERAND_ACCESS)(REFERENCE+READWRITE) }} } },

	/* 60 - 67 */
	{ MAP_OPCODE, { OP_PUSHA,	{{ ADDR_GIVEN+REG_ESP, TYPE_z, (OPERAND_ACCESS)(REFERENCE+WRITE)}, { ADDR_GIVEN+REG_EAX+REG_ECX+REG_EDX+REG_EBX+REG_ESP+REG_EBP+REG_ESI+REG_EDI, TYPE_z, (OPERAND_ACCESS)(REFERENCE+WRITE) }} } },
	{ MAP_OPCODE, { OP_POPA,	{{ ADDR_GIVEN+REG_EAX+REG_ECX+REG_EDX+REG_EBX+REG_ESP+REG_EBP+REG_ESI+REG_EDI, TYPE_z, (OPERAND_ACCESS)(REFERENCE+WRITE) }, {ADDR_GIVEN+REG_ESP, TYPE_z, (OPERAND_ACCESS)(REFERENCE+READWRITE) }} } },
	{ MAP_OPCODE, { OP_BOUND,	{{ ADDR_G, TYPE_v, WRITE }, { ADDR_M, TYPE_a, READ }} } },
	{ MAP_OPCODE, { OP_ARPL,	{{ ADDR_E, TYPE_w, WRITE }, { ADDR_G, TYPE_w, READ }} } }, //XXX DIFFERENT IN 64BIT
	{ MAP_PREFIX, (int)&intel_prefix },
	{ MAP_PREFIX, (int)&intel_prefix },
	{ MAP_PREFIX, (int)&intel_prefix },
	{ MAP_PREFIX, (int)&intel_prefix },

	/* 68 - 6F */
	{ MAP_OPCODE, { OP_PUSH,	{{ ADDR_I, TYPE_z, READ }, { ADDR_GIVEN+REG_ESP, TYPE_v, (OPERAND_ACCESS)(REFERENCE+WRITE) }} } },
	{ MAP_OPCODE, { OP_IMUL,	{{ ADDR_G, TYPE_v, WRITE }, { ADDR_E, TYPE_v, READ }, { ADDR_I, TYPE_z, READ }} } },
	{ MAP_OPCODE, { OP_PUSH,	{{ ADDR_I, TYPE_b, READ }, { ADDR_GIVEN+REG_ESP, TYPE_v, (OPERAND_ACCESS)(REFERENCE+WRITE) }} } },
	{ MAP_OPCODE, { OP_IMUL,	{{ ADDR_G, TYPE_v, WRITE }, { ADDR_E, TYPE_v }, { ADDR_I, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_INS,		{{ ADDR_Y, TYPE_b, WRITE }, { ADDR_GIVEN+REG_DX, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_INSD,	{{ ADDR_Y, TYPE_z, WRITE }, { ADDR_GIVEN+REG_DX, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_OUTS,	{{ ADDR_GIVEN+REG_DX, TYPE_b, WRITE }, { ADDR_X, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_OUTSD,	{{ ADDR_GIVEN+REG_DX, TYPE_b, WRITE }, { ADDR_X, TYPE_z, READ }} } },

	/* 70 - 77 */
	{ MAP_OPCODE, { OP_JO,	{{ ADDR_J, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_JNO,	{{ ADDR_J, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_JB,	{{ ADDR_J, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_JNB,	{{ ADDR_J, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_JZ,	{{ ADDR_J, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_JNZ,	{{ ADDR_J, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_JBE,	{{ ADDR_J, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_JNBE,	{{ ADDR_J, TYPE_b, READ }} } },

	/* 78 - 7F */
	{ MAP_OPCODE, { OP_JS,	{{ ADDR_J, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_JNS,	{{ ADDR_J, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_JP,	{{ ADDR_J, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_JNP,	{{ ADDR_J, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_JL,	{{ ADDR_J, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_JNL,	{{ ADDR_J, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_JLE,	{{ ADDR_J, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_JNLE,	{{ ADDR_J, TYPE_b, READ }} } },

	/* 80 - 87 */
	{ MAP_GROUP, { (int)&group_1_map, {{ ADDR_E, TYPE_b, WRITE }, { ADDR_I, TYPE_b, READ }} } },
	{ MAP_GROUP, { (int)&group_1_map, {{ ADDR_E, TYPE_v, WRITE }, { ADDR_I, TYPE_z, READ }} } },
	{ MAP_GROUP, { (int)&group_1_map, {{ ADDR_E, TYPE_b, WRITE }, { ADDR_I, TYPE_b, READ }} } },
	{ MAP_GROUP, { (int)&group_1_map, {{ ADDR_E, TYPE_v, WRITE }, { ADDR_I, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_TEST,	{{ ADDR_E, TYPE_b, WRITE }, { ADDR_G, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_TEST,	{{ ADDR_E, TYPE_v, WRITE }, { ADDR_G, TYPE_v, READ }} } },
	{ MAP_OPCODE, { OP_XCHG,	{{ ADDR_E, TYPE_b, READWRITE }, { ADDR_G, TYPE_b, READWRITE }} } },
	{ MAP_OPCODE, { OP_XCHG,	{{ ADDR_E, TYPE_v, READWRITE }, { ADDR_G, TYPE_v, READWRITE }} } },

	/* 88 - 8F */
	{ MAP_OPCODE, { OP_MOV,	{{ ADDR_E, TYPE_b, WRITE }, { ADDR_G, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_MOV,	{{ ADDR_E, TYPE_v, WRITE }, { ADDR_G, TYPE_v, READ }} } },
	{ MAP_OPCODE, { OP_MOV,	{{ ADDR_G, TYPE_b, WRITE }, { ADDR_E, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_MOV,	{{ ADDR_G, TYPE_v, WRITE }, { ADDR_E, TYPE_v, READ }} } },
	{ MAP_OPCODE, { OP_MOV,	{{ ADDR_E, TYPE_v, WRITE }, { ADDR_S, TYPE_w, READ }} } },
	{ MAP_OPCODE, { OP_LEA,	{{ ADDR_G, TYPE_v, WRITE }, { ADDR_M, TYPE_v, READ }} } },
	{ MAP_OPCODE, { OP_MOV,	{{ ADDR_S, TYPE_w, WRITE }, { ADDR_E, TYPE_w, READ }} } },
	{ MAP_GROUP, { (int)&group_1_1A_map,	{{ ADDR_E, TYPE_v, WRITE }, { ADDR_GIVEN+REG_ESP, TYPE_z, READ }} } },

	/* 90 - 97 */
	{ MAP_OPCODE, { OP_NOP } }, //XCHG EAX, EAX
	{ MAP_OPCODE, { OP_XCHG, {{ ADDR_GIVEN+REG_EAX, TYPE_v, READWRITE }, { ADDR_GIVEN+REG_ECX, TYPE_v, READWRITE }} } },
	{ MAP_OPCODE, { OP_XCHG, {{ ADDR_GIVEN+REG_EAX, TYPE_v, READWRITE }, { ADDR_GIVEN+REG_EDX, TYPE_v, READWRITE }} } },
	{ MAP_OPCODE, { OP_XCHG, {{ ADDR_GIVEN+REG_EAX, TYPE_v, READWRITE }, { ADDR_GIVEN+REG_EBX, TYPE_v, READWRITE }} } },
	{ MAP_OPCODE, { OP_XCHG, {{ ADDR_GIVEN+REG_EAX, TYPE_v, READWRITE }, { ADDR_GIVEN+REG_ESP, TYPE_v, READWRITE }} } },
	{ MAP_OPCODE, { OP_XCHG, {{ ADDR_GIVEN+REG_EAX, TYPE_v, READWRITE }, { ADDR_GIVEN+REG_EBP, TYPE_v, READWRITE }} } },
	{ MAP_OPCODE, { OP_XCHG, {{ ADDR_GIVEN+REG_EAX, TYPE_v, READWRITE }, { ADDR_GIVEN+REG_ESI, TYPE_v, READWRITE }} } },
	{ MAP_OPCODE, { OP_XCHG, {{ ADDR_GIVEN+REG_EAX, TYPE_v, READWRITE }, { ADDR_GIVEN+REG_EDI, TYPE_v, READWRITE }} } },

	/* 98 - 9F */
	{ MAP_OPCODE, { OP_CBW,	{{ ADDR_GIVEN+REG_EAX, TYPE_v, WRITE }, { ADDR_GIVEN+REG_AX, TYPE_w, READ }} } }, //XXX
	{ MAP_OPCODE, { OP_CWD,	{{ ADDR_GIVEN+REG_EDX+REG_EAX, TYPE_v, WRITE }, { ADDR_GIVEN+REG_EAX, TYPE_v, READ }} } }, //XXX
	{ MAP_OPCODE, { OP_CALL,	{{ ADDR_A, TYPE_p }} } },
	{ MAP_OPCODE, { OP_WAIT } },
	{ MAP_OPCODE, { OP_PUSHFD,	{{ ADDR_F, TYPE_v,(OPERAND_ACCESS)(REFERENCE+READ)}, { ADDR_GIVEN+REG_ESP, TYPE_v, (OPERAND_ACCESS)(REFERENCE+WRITE) }} } },
	{ MAP_OPCODE, { OP_POPFD,	{{ ADDR_F, TYPE_v,(OPERAND_ACCESS)(REFERENCE+WRITE)}, { ADDR_GIVEN+REG_ESP, TYPE_v, (OPERAND_ACCESS)(REFERENCE+READWRITE) }} } },
	{ MAP_OPCODE, { OP_SAHF,	{{ ADDR_F, TYPE_v,(OPERAND_ACCESS)(REFERENCE+WRITE)}, { ADDR_GIVEN+REG_AH, TYPE_w,(OPERAND_ACCESS)(REFERENCE+READ) }} } },
	{ MAP_OPCODE, { OP_LAHF,	{{ ADDR_GIVEN+REG_AH, TYPE_w, (OPERAND_ACCESS)(REFERENCE+WRITE) }, { ADDR_F, TYPE_v,(OPERAND_ACCESS)(REFERENCE+READ)}} } },

	/* A0 - A7 */
	{ MAP_OPCODE, { OP_MOV, {{ ADDR_GIVEN+REG_AL, TYPE_b, WRITE }, { ADDR_O, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_MOV, {{ ADDR_GIVEN+REG_EAX, TYPE_v, WRITE }, { ADDR_O, TYPE_v, READ }} } },
	{ MAP_OPCODE, { OP_MOV, {{ ADDR_O, TYPE_b, WRITE }, { ADDR_GIVEN+REG_AL, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_MOV, {{ ADDR_O, TYPE_v, WRITE }, { ADDR_GIVEN+REG_EAX, TYPE_v, READ }} } },
	{ MAP_OPCODE, { OP_MOVS, {{ ADDR_Y, TYPE_b, WRITE }, { ADDR_X, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_MOVS, {{ ADDR_Y, TYPE_v, WRITE }, { ADDR_X, TYPE_v, READ }} } },
	{ MAP_OPCODE, { OP_CMPS, {{ ADDR_X, TYPE_b, WRITE }, { ADDR_Y, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_CMPS, {{ ADDR_X, TYPE_v, WRITE }, { ADDR_Y, TYPE_v, READ }} } },

	/* A8 - AF */
	{ MAP_OPCODE, { OP_TEST, {{ ADDR_GIVEN+REG_AL, TYPE_b, READ }, { ADDR_I, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_TEST, {{ ADDR_GIVEN+REG_EAX, TYPE_v, READ }, { ADDR_I, TYPE_z, READ }} } },
	{ MAP_OPCODE, { OP_STOS, {{ ADDR_Y, TYPE_b, WRITE }, { ADDR_GIVEN+REG_AL, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_STOS, {{ ADDR_Y, TYPE_v, WRITE }, { ADDR_GIVEN+REG_EAX, TYPE_v, READ }} } },
	{ MAP_OPCODE, { OP_LODS, {{ ADDR_GIVEN+REG_AL, TYPE_b, WRITE }, { ADDR_X, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_LODS, {{ ADDR_GIVEN+REG_EAX, TYPE_v, WRITE }, { ADDR_X, TYPE_v, READ }} } },
	{ MAP_OPCODE, { OP_SCAS, {{ ADDR_GIVEN+REG_AL, TYPE_b, WRITE }, { ADDR_Y, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_SCAS, {{ ADDR_GIVEN+REG_EAX, TYPE_v, WRITE }, { ADDR_X, TYPE_v, READ }} } },

	/* B0 - B7 */
	{ MAP_OPCODE, { OP_MOV, {{ ADDR_GIVEN+REG_AL, TYPE_b, WRITE }, { ADDR_I, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_MOV, {{ ADDR_GIVEN+REG_CL, TYPE_b, WRITE }, { ADDR_I, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_MOV, {{ ADDR_GIVEN+REG_DL, TYPE_b, WRITE }, { ADDR_I, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_MOV, {{ ADDR_GIVEN+REG_BL, TYPE_b, WRITE }, { ADDR_I, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_MOV, {{ ADDR_GIVEN+REG_AH, TYPE_b, WRITE }, { ADDR_I, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_MOV, {{ ADDR_GIVEN+REG_CH, TYPE_b, WRITE }, { ADDR_I, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_MOV, {{ ADDR_GIVEN+REG_DH, TYPE_b, WRITE }, { ADDR_I, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_MOV, {{ ADDR_GIVEN+REG_BH, TYPE_b, WRITE }, { ADDR_I, TYPE_b, READ }} } },

	/* B8 - BF */
	{ MAP_OPCODE, { OP_MOV, {{ ADDR_GIVEN+REG_EAX, TYPE_v, WRITE }, { ADDR_I, TYPE_v, READ }} } },
	{ MAP_OPCODE, { OP_MOV, {{ ADDR_GIVEN+REG_ECX, TYPE_v, WRITE }, { ADDR_I, TYPE_v, READ }} } },
	{ MAP_OPCODE, { OP_MOV, {{ ADDR_GIVEN+REG_EDX, TYPE_v, WRITE }, { ADDR_I, TYPE_v, READ }} } },
	{ MAP_OPCODE, { OP_MOV, {{ ADDR_GIVEN+REG_EBX, TYPE_v, WRITE }, { ADDR_I, TYPE_v, READ }} } },
	{ MAP_OPCODE, { OP_MOV, {{ ADDR_GIVEN+REG_ESP, TYPE_v, WRITE }, { ADDR_I, TYPE_v, READ }} } },
	{ MAP_OPCODE, { OP_MOV, {{ ADDR_GIVEN+REG_EBP, TYPE_v, WRITE }, { ADDR_I, TYPE_v, READ }} } },
	{ MAP_OPCODE, { OP_MOV, {{ ADDR_GIVEN+REG_ESI, TYPE_v, WRITE }, { ADDR_I, TYPE_v, READ }} } },
	{ MAP_OPCODE, { OP_MOV, {{ ADDR_GIVEN+REG_EDI, TYPE_v, WRITE }, { ADDR_I, TYPE_v, READ }} } },

	/* C0 - C7 */
	{ MAP_GROUP, (int)&group_2, {{ ADDR_E, TYPE_b, WRITE }, { ADDR_I, TYPE_b, READ }} },
	{ MAP_GROUP, (int)&group_2, {{ ADDR_E, TYPE_v, WRITE }, { ADDR_I, TYPE_b, READ }} },
	{ MAP_OPCODE, { OP_RET, {{ ADDR_I, TYPE_w, READ }, { ADDR_GIVEN+REG_ESP, TYPE_v, (OPERAND_ACCESS)(REFERENCE+READWRITE) }} } },
	{ MAP_OPCODE, { OP_RET, {{ ADDR_GIVEN+REG_ESP, TYPE_v, (OPERAND_ACCESS)(REFERENCE+READWRITE) }} } }, //XXX
	{ MAP_OPCODE, { OP_LES, {{ ADDR_G, TYPE_z, WRITE }, { ADDR_M, TYPE_p, READ }} } },
	{ MAP_OPCODE, { OP_LDS, {{ ADDR_G, TYPE_z, WRITE }, { ADDR_M, TYPE_p, READ }} } },
	{ MAP_GROUP, (int)&group_11, {{ ADDR_E, TYPE_b, WRITE }, { ADDR_I, TYPE_b, READ }} },
	{ MAP_GROUP, (int)&group_11, {{ ADDR_E, TYPE_v, WRITE }, { ADDR_I, TYPE_z, READ }} },

	/* C8 - CF */
	{ MAP_OPCODE, { OP_ENTER, {{ ADDR_I, TYPE_w, READ }, { ADDR_I, TYPE_b, READ }, { ADDR_GIVEN+REG_ESP, TYPE_v, (OPERAND_ACCESS)(REFERENCE+WRITE) }} } },
	{ MAP_OPCODE, { OP_LEAVE, {{ ADDR_GIVEN+REG_ESP, TYPE_v, (OPERAND_ACCESS)(REFERENCE+WRITE) }} } },
	{ MAP_OPCODE, { OP_RET, {{ ADDR_I, TYPE_w, READ }, { ADDR_GIVEN+REG_ESP, TYPE_v, (OPERAND_ACCESS)(REFERENCE+READWRITE) }} } },
	{ MAP_OPCODE, { OP_RET, {{ ADDR_GIVEN+REG_ESP, TYPE_v, (OPERAND_ACCESS)(REFERENCE+READWRITE) }} } },
	{ MAP_OPCODE, { OP_INT3 } },
	{ MAP_OPCODE, { OP_INT, {{ ADDR_I, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_INTO } },
	{ MAP_OPCODE, { OP_IRET } }, //XXX THIS ACTUALLY READS FROM STACK IN SOME CASES

	/* D0 - D7 */
	{ MAP_GROUP, (int)&group_2, {{ ADDR_E, TYPE_b, WRITE }, { ADDR_GIVEN_IMMIDIATE, 1, READ }} },
	{ MAP_GROUP, (int)&group_2, {{ ADDR_E, TYPE_v, WRITE }, { ADDR_GIVEN_IMMIDIATE, 1, READ }} },
	{ MAP_GROUP, (int)&group_2, {{ ADDR_E, TYPE_b, WRITE }, { ADDR_GIVEN+REG_CL, TYPE_w, READ }} },
	{ MAP_GROUP, (int)&group_2, {{ ADDR_E, TYPE_v, WRITE }, { ADDR_GIVEN+REG_CL, TYPE_w, READ }} },
	{ MAP_OPCODE, { OP_AAM, {{ ADDR_GIVEN+REG_AX, TYPE_w, WRITE }, { ADDR_I, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_AAD, {{ ADDR_GIVEN+REG_AX, TYPE_w, WRITE }, { ADDR_I, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_SALC, {{ ADDR_GIVEN+REG_AL, TYPE_b, WRITE }, { ADDR_F, TYPE_v, READ }} } },
	{ MAP_OPCODE, { OP_XLAT, {{ ADDR_GIVEN+REG_AL, TYPE_b, WRITE }, { ADDR_GIVEN+REG_EBX, TYPE_v, READ }, { ADDR_GIVEN+REG_AL, TYPE_b, READ }} } }, //XXX NEEDS SPECIAL TREATMENT

	/* D8 - DF */
	{ MAP_ESCAPE, (int)&fp_esc_0 },
	{ MAP_ESCAPE, (int)&fp_esc_1 },
	{ MAP_ESCAPE, (int)&fp_esc_2 },
	{ MAP_ESCAPE, (int)&fp_esc_3 },
	{ MAP_ESCAPE, (int)&fp_esc_4 },
	{ MAP_ESCAPE, (int)&fp_esc_5 },
	{ MAP_ESCAPE, (int)&fp_esc_6 },
	{ MAP_ESCAPE, (int)&fp_esc_7 },

	/* E0 - E7 */
	{ MAP_OPCODE, { OP_LOOPNZ, {{ ADDR_J, TYPE_b, READ }, { ADDR_GIVEN+REG_ECX, TYPE_v, READWRITE }} } },
	{ MAP_OPCODE, { OP_LOOPZ, {{ ADDR_J, TYPE_b, READ }, { ADDR_GIVEN+REG_ECX, TYPE_v, READWRITE }} } },
	{ MAP_OPCODE, { OP_LOOP, {{ ADDR_J, TYPE_b, READ }, { ADDR_GIVEN+REG_ECX, TYPE_v, READWRITE }} } },
	{ MAP_OPCODE, { OP_JECXZ, {{ ADDR_J, TYPE_b, READ }, { ADDR_GIVEN+REG_ECX, TYPE_v, READWRITE }} } },
	{ MAP_OPCODE, { OP_IN, {{ ADDR_GIVEN+REG_AL, TYPE_b, WRITE }, { ADDR_I, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_IN, {{ ADDR_GIVEN+REG_EAX, TYPE_v, WRITE }, { ADDR_I, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_OUT, {{ ADDR_I, TYPE_b, WRITE }, { ADDR_GIVEN+REG_AL, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_OUT, {{ ADDR_I, TYPE_b, WRITE }, { ADDR_GIVEN+REG_EAX, TYPE_v, READ }} } },

	/* E8 - EF */
	{ MAP_OPCODE, { OP_CALL, {{ ADDR_J, TYPE_z, READ }} } },
	{ MAP_OPCODE, { OP_JMP, {{ ADDR_J, TYPE_z, READ }} } }, //XXX PUT EIP AS WRITE OPCODE?
	{ MAP_OPCODE, { OP_JMP, {{ ADDR_A, TYPE_p, READ }} } },
	{ MAP_OPCODE, { OP_JMP, {{ ADDR_J, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_IN, {{ ADDR_GIVEN+REG_AL, TYPE_b, WRITE }, { ADDR_GIVEN+REG_DX, TYPE_w, READ }} } },
	{ MAP_OPCODE, { OP_IN, {{ ADDR_GIVEN+REG_EAX, TYPE_v, WRITE }, { ADDR_GIVEN+REG_DX, TYPE_w, READ }} } },
	{ MAP_OPCODE, { OP_OUT, {{ ADDR_GIVEN+REG_DX, TYPE_w, WRITE }, { ADDR_GIVEN+REG_AL, TYPE_b, READ }} } },
	{ MAP_OPCODE, { OP_OUT, {{ ADDR_GIVEN+REG_DX, TYPE_w, WRITE }, { ADDR_GIVEN+REG_EAX, TYPE_v, READ }} } },

	/* F0 - F7 */
	{ MAP_PREFIX, (int)&intel_prefix },
	{ MAP_OPCODE, { OP_INT1 } },
	{ MAP_PREFIX, (int)&intel_prefix },
	{ MAP_PREFIX, (int)&intel_prefix },
	{ MAP_OPCODE, { OP_HLT } },
	{ MAP_OPCODE, { OP_CMC, {{ ADDR_F, TYPE_v, WRITE }} } },
	{ MAP_GROUP, { (int)&unary_group_3_1 } },
	{ MAP_GROUP, { (int)&unary_group_3_2 } },

	/* F8 - FF */
	{ MAP_OPCODE, { OP_CLC, {{ ADDR_F, TYPE_v,(OPERAND_ACCESS)(REFERENCE+WRITE)}} } },
	{ MAP_OPCODE, { OP_STC, {{ ADDR_F, TYPE_v,(OPERAND_ACCESS)(REFERENCE+WRITE)}} } },
	{ MAP_OPCODE, { OP_CLI, {{ ADDR_F, TYPE_v,(OPERAND_ACCESS)(REFERENCE+WRITE)}} } },
	{ MAP_OPCODE, { OP_STI, {{ ADDR_F, TYPE_v,(OPERAND_ACCESS)(REFERENCE+WRITE)}} } },
	{ MAP_OPCODE, { OP_CLD, {{ ADDR_F, TYPE_v,(OPERAND_ACCESS)(REFERENCE+WRITE)}} } },
	{ MAP_OPCODE, { OP_STD, {{ ADDR_F, TYPE_v, WRITE}} } },
	{ MAP_GROUP, (int)&group_4, { ADDR_E, TYPE_b, WRITE } },
	{ MAP_GROUP, (int)&group_5 },
};