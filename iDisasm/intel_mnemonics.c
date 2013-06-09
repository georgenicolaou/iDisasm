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

#include "intel.h"
#include <string.h>
#include "intel_defs.h"
#include "idisasm_include.h"

char * get_operand_mnemonic( OPERAND * lpOperand, char * lpMnemonic );

BOOL intel_get_mnemonic( PSIDISASM lpDisasm )
{
	int nInstrIndex = lpDisasm->InternalStuff.nOpcodeIndex;
	int i = 0;
	char * lpInstrMnemonic = (char *)&lpDisasm->Mnemonic;
	char lpMnemonic[64];
	INSTRUCTION_INFO * lpInstructionInfo;

	memset( lpDisasm->Mnemonic, 0, sizeof( lpDisasm->Mnemonic ) );
	memset( lpDisasm->Instruction.InstructionMnemonicName, 0, 
		sizeof( lpDisasm->Instruction.InstructionMnemonicName ) );

	lpInstructionInfo = 
		&sinstruction_info[lpDisasm->InternalStuff.nOpcodeIndex];
	
	strcpy( lpDisasm->Instruction.InstructionMnemonicName, 
		lpInstructionInfo->lpszName );
	
	lpInstrMnemonic += 
		sprintf( lpInstrMnemonic, "%s ", lpInstructionInfo->lpszName );

	if( lpDisasm->Instruction.Operands[i].Type == OPERAND_NONE ) return TRUE;

	if( ( lpDisasm->Instruction.Operands[i].Access & REFERENCE ) == NULL ) {
		lpInstrMnemonic += sprintf( lpInstrMnemonic, "%s", 
			get_operand_mnemonic( &lpDisasm->Instruction.Operands[i], 
			lpMnemonic ) );
	}

	for( i = 1; i < INTEL_MAX_NUMBER_OF_OPERANDS; i++ ) {
		if( lpDisasm->Instruction.Operands[i].Type != OPERAND_NONE ) {
			if( ( lpDisasm->Instruction.Operands[i].Access & REFERENCE ) == 
				NULL ) {
					lpInstrMnemonic += sprintf( lpInstrMnemonic, ",%s", 
						get_operand_mnemonic( &lpDisasm->Instruction.Operands[i], 
						lpMnemonic ) );
			}
		}
	}
}

char * get_register_mnemonic( OPERAND_TYPE Type, BITSIZE BitSize, int Register ) {
	switch( Type & OPERAND_TYPE_REGISTER_MASK ) {
	case INTEL_TYPE_REG_GENERAL:
		switch( BitSize ) {
		case BIT32:
			switch( Register ) {
				case REG_EAX: return "EAX";
				case REG_ECX: return "ECX";
				case REG_EDX: return "EDX";
				case REG_EBX: return "EBX";
				case REG_ESP: return "ESP";
				case REG_EBP: return "EBP";
				case REG_ESI: return "ESI";
				case REG_EDI: return "EDI";
				default: return "";
			}
		case BIT16:
			switch( Register ) {
				case REG_AX: return "AX";
				case REG_CX: return "CX";
				case REG_DX: return "DX";
				case REG_BX: return "BX";
				case REG_SP: return "SP";
				case REG_BP: return "BP";
				case REG_SI: return "SI";
				case REG_DI: return "DI";
				default: return "";
			}
		case BIT8:
			switch( Register ) {
				case REG_AL: return "AL";
				case REG_CL: return "CL";
				case REG_DL: return "DL";
				case REG_BL: return "BL";
				case REG_SPL: return "SPL";
				case REG_BPL: return "BPL";
				case REG_SL: return "SL";
				case REG_DIL: return "DIL";
				case REG_AH: return "AH";
				case REG_CH: return "CH";
				case REG_DH: return "DH";
				case REG_BH: return "BH";
				case REG_SPH: return "SPH";
				default: return "";
			}
		}
	case INTEL_TYPE_REG_MMX:
		switch( Register ) {
			case REG_MM0: return "MM0";
			case REG_MM1: return "MM1";
			case REG_MM2: return "MM2";
			case REG_MM3: return "MM3";
			case REG_MM4: return "MM4";
			case REG_MM5: return "MM5";
			case REG_MM6: return "MM6";
			case REG_MM7: return "MM7";
		}
	case INTEL_TYPE_REG_FPU:
		switch( Register ) {
			case REG_ST0: return "ST0";
			case REG_ST1: return "ST1";
			case REG_ST2: return "ST2";
			case REG_ST3: return "ST3";
			case REG_ST4: return "ST4";
			case REG_ST5: return "ST5";
			case REG_ST6: return "ST6";
			case REG_ST7: return "ST7";
		}
	case INTEL_TYPE_REG_SSE:
		switch( Register ) {
			case REG_XMM0: return "XMM0";
			case REG_XMM1: return "XMM1";
			case REG_XMM2: return "XMM2";
			case REG_XMM3: return "XMM3";
			case REG_XMM4: return "XMM4";
			case REG_XMM5: return "XMM5";
			case REG_XMM6: return "XMM6";
			case REG_XMM7: return "XMM7";
		}
	case INTEL_TYPE_REG_YMM:
		switch ( Register ) {
			case REG_YMM0: return "YMM0";
			case REG_YMM1: return "YMM1";
			case REG_YMM2: return "YMM2";
			case REG_YMM3: return "YMM3";
			case REG_YMM4: return "YMM4";
			case REG_YMM5: return "YMM5";
			case REG_YMM6: return "YMM6";
			case REG_YMM7: return "YMM7";
		}
	case INTEL_TYPE_REG_CR:
		switch( Register ) {
			case REG_CR_0: return "CR0";
			case REG_CR_1: return "CR1";
			case REG_CR_2: return "CR2";
			case REG_CR_3: return "CR3";
			case REG_CR_4: return "CR4";
			case REG_CR_5: return "CR5";
			case REG_CR_6: return "CR6";
			case REG_CR_7: return "CR7";
		}
	case INTEL_TYPE_REG_DR:
		switch( Register ) {
			case REG_DR_0: return "DR0";
			case REG_DR_1: return "DR1";
			case REG_DR_2: return "DR2";
			case REG_DR_3: return "DR3";
			case REG_DR_4: return "DR4";
			case REG_DR_5: return "DR5";
			case REG_DR_6: return "DR6";
			case REG_DR_7: return "DR7";
		}
	case INTEL_TYPE_REG_SEGMENT:
		switch( Register ) {
			case REG_ES: return "ES";
			case REG_CS: return "CS";
			case REG_SS: return "SS";
			case REG_DS: return "DS";
			case REG_FS: return "FS";
			case REG_GS: return "GS";
		}
	default:
		return "[UNKNOWN/UNIMPLEMENTED]";
	}
}

char * get_size_mnemonic( BITSIZE BitSize )
{
	switch( BitSize ) {
	case BIT8: return "BYTE";
	case BIT16: return "WORD";
	case BIT32: return "DWORD";
	case BIT48: return "FWORD";
	case BIT64: return "QWORD";
	case BIT128: return "DQWORD";
	case BIT256: return "QQWORD";
	default: return "[UNKNOWN/UNIMPLEMENTED]";
	}
}

char * get_addressing_register( ADDRESSING_REGISTER AddressingRegister, 
	char * lpTmpStr )
{
	//XXX should handle AR_ADDRESS and AR_REGISTER
	if( ( AddressingRegister & AR_TYPE ) == AR_REGISTER )  {
		switch( AddressingRegister & AR_REGVAL ) {
		case REG_ES: return "ES";
		case REG_CS: return "CS";
		case REG_SS: return "SS";
		case REG_FS: return "FS";
		case REG_GS: return "GS";
		case REG_DS:
		default:
			return "DS";
		}
	}
	else {
		sprintf( lpTmpStr, "%X", AddressingRegister & AR_REGVAL );
		return lpTmpStr;
	}	
}

char * get_operand_mnemonic( OPERAND * lpOperand, char * lpMnemonic )
{
	char lpTmpMnemonic[64] = {0};
	char lpTmpGeneralStr[32] = {0};
	char * lpTmpMnemonicPtr = NULL;
	BOOL bIsSib = FALSE; //XXX rename to bHasBase
	switch( lpOperand->Type & OPERAND_TYPE_TYPE_MASK ) {
	case OPERAND_REGISTER:
		return get_register_mnemonic( lpOperand->Type, lpOperand->BitSize, 
			lpOperand->RegValMem.Register );
	case OPERAND_MEMORY:
		// DWORD PTR <SEG>:[memory]
		lpTmpMnemonicPtr = lpTmpMnemonic;
		if( lpOperand->RegValMem.Memory.BaseRegister != REG_NIL ) {
			bIsSib = TRUE;
			lpTmpMnemonicPtr += sprintf( lpTmpMnemonic, 
				"%s", get_register_mnemonic( (OPERAND_TYPE)INTEL_TYPE_REG_GENERAL, 
				lpOperand->AddressingSize, 
				lpOperand->RegValMem.Memory.BaseRegister ) );
		}
		if( lpOperand->RegValMem.Memory.IndexRegister != REG_NIL ) {
			bIsSib = TRUE;
			lpTmpMnemonicPtr += sprintf( lpTmpMnemonicPtr, 
				"+%s", get_register_mnemonic( (OPERAND_TYPE)INTEL_TYPE_REG_GENERAL, 
				lpOperand->AddressingSize, 
				lpOperand->RegValMem.Memory.IndexRegister ) );
		}
		if( lpOperand->RegValMem.Memory.Scale != 0 && 
			lpOperand->RegValMem.Memory.Scale != 1 ) {
				lpTmpMnemonicPtr += sprintf( lpTmpMnemonicPtr, "*%d", 
					lpOperand->RegValMem.Memory.Scale );
		}
		if( lpOperand->RegValMem.Memory.Displacement != 0 && bIsSib ) {
			if( lpOperand->RegValMem.Memory.Displacement < 0 ) {
				sprintf( lpTmpMnemonicPtr, "-%X", 
					0 - lpOperand->RegValMem.Memory.Displacement );
			}
			else {
				sprintf( lpTmpMnemonicPtr, "+%X", 
					lpOperand->RegValMem.Memory.Displacement );
			}
			
		}
		else if( bIsSib == FALSE ) {
			sprintf( lpTmpMnemonicPtr, "%X", 
				lpOperand->RegValMem.Memory.Displacement );
		}

		sprintf( lpMnemonic, "%s PTR %s:[%s]", 
			get_size_mnemonic( lpOperand->BitSize ), 
			get_addressing_register( lpOperand->AddrRegister, lpTmpGeneralStr ), 
			lpTmpMnemonic );
		return lpMnemonic;
	case OPERAND_IMMIDIATE:
		sprintf( lpMnemonic, "%X", lpOperand->RegValMem.Value );
		return lpMnemonic;
	case OPERAND_IMMIDIATE_ADDR:
		if( lpOperand->AddrRegister & AR_REGISTER ) {
			sprintf( lpMnemonic, "%s:%X", get_addressing_register( 
				lpOperand->AddrRegister, lpTmpGeneralStr ), 
				lpOperand->TargetAddress );
		}
		else if( lpOperand->AddrRegister & AR_ADDRESS ) {
			sprintf( lpMnemonic, "%X:%X", lpOperand->AddrRegister & AR_REGVAL, 
				lpOperand->TargetAddress );
		}
		else {
			sprintf( lpMnemonic, "%X", lpOperand->TargetAddress );
		}
		return lpMnemonic;
	}
}