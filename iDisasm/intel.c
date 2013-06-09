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
#include "intel.h"
#include "intel_defs.h"
#include <string.h>

#include "intel_addressings.h"
#include "intel_opcode_maps.h"
#include "intel_instruction_info.h"

ADDRESSING_TOKEN * get_addressing_token( ADDRESSING_MAP * lpAddrMap, uchar uVal )
{
	return &lpAddrMap->eAddressing[( uVal & lpAddrMap->Mask ) >> 
		lpAddrMap->RightShift];
}

void fix_sign32( int nDisplacementSize, int32 * lpnValue  )
{
	int nMask;
	if( nDisplacementSize != BIT32 && ( *lpnValue >> ( nDisplacementSize - 1 ) ) ) {
		nMask = 1 << ( nDisplacementSize - 1 );
		*lpnValue = ( *lpnValue ^ nMask ) - nMask;
	}
}

/*
** Sets up a MODR/M defined operand
** returns: number of bytes consumed
*/
int intel_modrm( ADDRESSING_MAP * lpAddressingMap, 
	OPERAND * lpOperand, uchar * lpInstrPointer, OPERAND_TYPE eAllowedTypes, BOOL * bUsedSib )
{
	uint8 AddrByte = 0;
	int nDisplacementSize = 0;
	int nBytesConsumed = 0;
	ADDRESSING_TOKEN * lpIndexScaleToken;
	ADDRESSING_TOKEN * lpBaseToken;
	ADDRESSING_TOKEN * lpAddrToken;
	ADDRESSING_MAPS * lpAddrMaps;
	AddrByte = ( lpAddressingMap->Mask & *lpInstrPointer ) >> 
		lpAddressingMap->RightShift;
	lpAddrToken = &lpAddressingMap->eAddressing[AddrByte];
	

	if( lpAddrToken->eHint == ADDRESSING_GIVEN ) {
		lpOperand->Type = lpAddrToken->eType;
		switch( lpAddrToken->eType & OPERAND_ADDRESSING_TYPE_MASK ) 
		{
		case OPERAND_MEMORY:
			if( ( eAllowedTypes & OPERAND_MEMORY ) == 0 ) {
				return INSTRUCTION_INVALID;
			}
			//Set BaseRegister (if it exists)
			lpOperand->RegValMem.Memory.BaseRegister = 
				(REG)( lpAddrToken->eAddressing & OPERAND_ADDRESSING_MASK );
			lpOperand->Type = OPERAND_MEMORY;

			//Check for displacement
			if( ( nDisplacementSize = 
				lpAddrToken->eType & OPERAND_ADDRESSING_MASK ) != 0 ) {
					memcpy( &lpOperand->RegValMem.Memory.Displacement, 
						lpInstrPointer+1, nDisplacementSize / CHAR_BIT );
					nBytesConsumed += nDisplacementSize / CHAR_BIT;

					fix_sign32( nDisplacementSize, 
						&lpOperand->RegValMem.Memory.Displacement );
			}
			break;
		case OPERAND_REGISTER:
			if( ( eAllowedTypes & OPERAND_REGISTER ) == 0 ) {
				return INSTRUCTION_INVALID;
			}
			lpOperand->RegValMem.Register = (REG)lpAddrToken->eAddressing & 
				OPERAND_ADDRESSING_MASK;
			lpOperand->Type = OPERAND_REGISTER;
			break;
		}
	}
	else { //SIB byte follows (forwarder map)
		*bUsedSib = TRUE;
		lpOperand->Type = OPERAND_MEMORY;
		if( ( eAllowedTypes & OPERAND_MEMORY ) == 0 ) {
			return INSTRUCTION_INVALID;
		}
		lpAddrMaps = (ADDRESSING_MAPS *)lpAddrToken->lpIndexMap;
		//Check if SIB byte has a ModR/M given displacement
		if( ( nDisplacementSize = lpAddrToken->eType & OPERAND_ADDRESSING_MASK ) 
			!= 0 ) {
				memcpy( &lpOperand->RegValMem.Memory.Displacement, lpInstrPointer+2, 
					nDisplacementSize / CHAR_BIT );

				fix_sign32( nDisplacementSize, 
					&lpOperand->RegValMem.Memory.Displacement );

				nBytesConsumed += nDisplacementSize / CHAR_BIT;
		}
		lpIndexScaleToken = get_addressing_token( 
			lpAddrMaps->lpAddrMap1, (uchar)*(lpInstrPointer+1) );
		lpBaseToken = get_addressing_token( 
			lpAddrMaps->lpAddrMap2, *(lpInstrPointer+1) );

		//Check if scale exists and set
		if( ( lpIndexScaleToken->eType & OPERAND_ADDRESSING_TYPE_MASK ) == 
			OPERAND_IMMIDIATE ) {
				lpOperand->RegValMem.Memory.Scale = lpIndexScaleToken->eType & 
					OPERAND_ADDRESSING_MASK;
		}

		if( ( lpIndexScaleToken->eAddressing & OPERAND_ADDRESSING_TYPE_MASK ) == 
			ADDR_GIVEN ) {
				lpOperand->RegValMem.Memory.IndexRegister = 
					(REG)(lpIndexScaleToken->eAddressing & OPERAND_ADDRESSING_MASK);
		}

		switch( lpBaseToken->eType & OPERAND_ADDRESSING_TYPE_MASK )
		{
		case OPERAND_REGISTER:
			lpOperand->RegValMem.Memory.BaseRegister = 
				(REG)(lpBaseToken->eAddressing & OPERAND_ADDRESSING_MASK);
			break;
		case OPERAND_MEMORY://XXX erm..?? this is only register...
			nDisplacementSize = lpBaseToken->eType & OPERAND_ADDRESSING_MASK;
			memcpy( &lpOperand->RegValMem.Memory.Displacement, lpInstrPointer+2,
				nDisplacementSize / CHAR_BIT );

			fix_sign32( nDisplacementSize, 
				&lpOperand->RegValMem.Memory.Displacement );
			
			nBytesConsumed += nDisplacementSize / CHAR_BIT;
			break;
		}
	}

	return nBytesConsumed;
}

ENUM_OPCODE_CATEGORY_T intel_get_opcode_category( ENUM_OPCODE_PTR eOpcode )
{
	return sinstruction_info[eOpcode].eCategory;
}

ENUM_INTEL_REGISTER_TYPE get_register_type( ENUM_INTEL_OPERAND_TYPE eType )
{
	if( eType == TYPE_INVALID )
		return INTEL_TYPE_REG_INVALID;

	switch( eType & OPERAND_ADDRESSING_TYPE_MASK ) {
	case ADDR_GIVEN:
		//Since the only way to distinguish between segment and regs is the bit
		//location
		if( ( eType & OPERAND_ADDRESSING_LOW )  != 0 ) 
			return INTEL_TYPE_REG_GENERAL;
		if( ( eType & OPERAND_ADDRESSING_HIGH ) != 0 ) 
			return INTEL_TYPE_REG_GENERAL;
		if( ( eType & OPERAND_ADDRESSING_SPECIAL ) != 0 ) 
			return INTEL_TYPE_REG_SEGMENT;
		else { //EAX/AX/AL Have 0 value
			return INTEL_TYPE_REG_GENERAL;
		}
	case ADDR_GIVEN_IMMIDIATE:
		return INTEL_TYPE_REG_INVALID;
	case ADDR_ADDRESSING:
		switch( eType ) {
		case TYPE_b:
		case TYPE_w:
		case TYPE_d:
		case TYPE_v:
		case TYPE_y:
		case TYPE_z:
		case TYPE_si:
			return INTEL_TYPE_REG_GENERAL;
		case TYPE_dq:
		case TYPE_ss:
			return INTEL_TYPE_REG_SSE;
		case TYPE_pi:
		case TYPE_q:
			return INTEL_TYPE_REG_MMX;
		case TYPE_a:
		case TYPE_p:
		default:
			return INTEL_TYPE_REG_INVALID;
		}
	}
}

BITSIZE get_addressing_size_x86( PSIDISASM lpDisasm ) 
{
	return ( lpDisasm->Prefixes & INTEL_PREFIX_GRP4_ADDRSIZEOVERRIDE ) ? 
		BIT16 : INTEL_DEFAULT_SIZE;
}

BITSIZE get_operand_size_x86(  ENUM_INTEL_OPERAND_TYPE eType, PSIDISASM lpDisasm )
{
	BOOL bOperandSizeOverwrite;
	
	bOperandSizeOverwrite = 
		( lpDisasm->Prefixes & INTEL_PREFIX_GRP3_OPSIZEOVERRIDE ) ? TRUE : FALSE;
	
	switch( eType )
	{
	case TYPE_a:
		return ( bOperandSizeOverwrite ) ? BIT32 : BIT64; //XXX
	case TYPE_s:
		return BIT_INVALID; //XXX no idea...
	case TYPE_b:
		return BIT8;
	case TYPE_w:
		return BIT16;
	case TYPE_c:
		return ( bOperandSizeOverwrite ) ? BIT8 : BIT16;
	case TYPE_d:
	case TYPE_si:
		return BIT32;
	case TYPE_q:
	case TYPE_pi:
		return BIT64;
	case TYPE_dq:
	case TYPE_ss:
	case TYPE_sd:
		return BIT128;
	case TYPE_p:
		return ( bOperandSizeOverwrite ) ? BIT32 : BIT48;
	case TYPE_pd:
		return BIT_INVALID; //only on 64bit
	case TYPE_qq:
		return BIT256;
	case TYPE_v:
		return ( bOperandSizeOverwrite ) ? BIT16 : BIT32;
	case TYPE_x:
		return ( bOperandSizeOverwrite ) ? BIT128 : BIT256;
	case TYPE_y:
		return BIT_INVALID; //ONLY ON 64BIT MODE
	case TYPE_z:
		return ( bOperandSizeOverwrite ) ? BIT16 : BIT32;
	case TYPE_INVALID:
	default:
		return BIT_INVALID;
	}
}

ADDRESSING_REGISTER intel_get_addessing_reg( PSIDISASM lpDisasm, 
	OPERAND * lpOperand )
{
	switch( lpDisasm->Prefixes & INTEL_PREFIX_GRP2 ) {
	case INTEL_PREFIX_GRP2_CS:
		return (ADDRESSING_REGISTER)(AR_REGISTER + REG_CS);
	case INTEL_PREFIX_GRP2_SS:
		return (ADDRESSING_REGISTER)(AR_REGISTER + REG_SS);
	case INTEL_PREFIX_GRP2_ES:
		return (ADDRESSING_REGISTER)(AR_REGISTER + REG_ES);
	case INTEL_PREFIX_GRP2_FS:
		return (ADDRESSING_REGISTER)(AR_REGISTER + REG_FS);
	case INTEL_PREFIX_GRP2_GS:
		return (ADDRESSING_REGISTER)(AR_REGISTER + REG_GS);
	case INTEL_PREFIX_GRP2_DS:
	default:
		if( lpOperand->RegValMem.Memory.BaseRegister != REG_NIL ) {
			if( lpOperand->RegValMem.Memory.BaseRegister == REG_EBP || 
				lpOperand->RegValMem.Memory.BaseRegister == REG_ESP ) {
					return (ADDRESSING_REGISTER)(AR_REGISTER + REG_SS );
			}
			else {
				return (ADDRESSING_REGISTER)(AR_REGISTER + REG_DS);
			}
		}
		else {
			return (ADDRESSING_REGISTER)(AR_REGISTER + REG_DS);
		}
		
	}
}

/*
** Handles addressing of instruction. Instruction pointer within the PSIDISASM
** struct points to the next byte after determining the instruction
** eg: the MOD R/M byte, or an immediate value
*/
int intel_addressing_handler( PSIDISASM lpDisasm,
	INSTRUCTIONINTERNAL * lpInstruction )
{
	int i;
	uint32 uOperandType;
	uint32 uBitSize;
	int nConsumedBytes = 0;
	uint32 uTempByteReader;
	uiptr uTmpIP = NULL;
	BOOL bUsedMod = FALSE; //Set when MODR/M byte is used
	BOOL bUsedSIB = FALSE; //Set when SIB byte is used
	ADDRESSING_MAP * lpTmpAddressingMap = NULL;

	for( i = 0; i < INTEL_MAX_NUMBER_OF_OPERANDS; i++ ) {
		dprintf( 2, "Operand %d: Scanning...", i );
		if( lpInstruction->Parameters[i].eAddressing == ADDR_INVALID ) {
			dprintf( 2, "Operand %d: ADDR_INVALID (Inexisten)", i );
			continue;
		}

		lpDisasm->Instruction.Operands[i].Access = 
			lpInstruction->Parameters[i].eReadWrite;

		uTmpIP = lpDisasm->InstructionPointer;
		//Set operand register + type, read/write, size and which register
		switch( lpInstruction->Parameters[i].eAddressing & 
			OPERAND_ADDRESSING_TYPE_MASK ) 
		{

		case ADDR_GIVEN:
			dprintf( 2, "Operand %d: ADDR_GIVEN", i );
			uOperandType = OPERAND_REGISTER + get_register_type( 
				(ENUM_INTEL_OPERAND_TYPE)lpInstruction->Parameters[i].eAddressing );

			lpDisasm->Instruction.Operands[i].Type = (OPERAND_TYPE)uOperandType;

			lpDisasm->Instruction.Operands[i].BitSize = get_operand_size_x86( 
				(ENUM_INTEL_OPERAND_TYPE)lpInstruction->Parameters[i].eSize, 
				lpDisasm );

			lpDisasm->Instruction.Operands[i].RegValMem.Register = 
				(REG)(lpInstruction->Parameters[i].eAddressing & 
				OPERAND_ADDRESSING_MASK );
			break;

		case ADDR_GIVEN_IMMIDIATE:
			dprintf( 2, "Operand %d: ADDR_GIVEN_IMMIDIATE (Special)", i );

			lpDisasm->Instruction.Operands[i].Type = OPERAND_IMMIDIATE;

			lpDisasm->Instruction.Operands[i].RegValMem.Value = 
				lpInstruction->Parameters[i].Immidiate;
			break;

		case ADDR_ADDRESSING:
			dprintf( 2, "Operand %d: ADDR_ADDRESSING", i );
			switch( lpInstruction->Parameters[i].eAddressing & 
				OPERAND_ADDRESSING_MASK ) 
			{

			case ADDR_A: // Direct Segmented Address.
				dprintf( 2, "Operand %d: ADDR_A (Direct through segment)", i );
				lpDisasm->Instruction.Operands[i].Type = OPERAND_IMMIDIATE_ADDR;
				uBitSize = get_operand_size_x86( 
					(ENUM_INTEL_OPERAND_TYPE)lpInstruction->Parameters[i].eSize, 
					lpDisasm );
				if( ( uBitSize - INTEL_SEGMENT_SIZE_BITS < 0 ) && 
					( uBitSize - INTEL_SEGMENT_SIZE_BITS % 16 != 0 ) ) {
						//XXX this shouldn't happen, addressing size % 16 and > 0
						eprintf( "Bad ADDR_A addressing size %d", uBitSize );
						break;
				}

				uTempByteReader = 0;

				//Retrieve the branch address
				memcpy( &uTempByteReader, (void *)uTmpIP, 
					( uBitSize - INTEL_SEGMENT_SIZE_BITS ) / CHAR_BIT );

				lpDisasm->Instruction.Operands[i].RegValMem.RelAddress = 
					uTempByteReader;

				lpDisasm->Instruction.Operands[i].TargetAddress = 
					uTempByteReader;

				uTmpIP += ( uBitSize - 
					INTEL_SEGMENT_SIZE_BITS ) / CHAR_BIT;

				uTempByteReader = 0;

				//Retrieve the branch segment
				memcpy( &uTempByteReader, (void *)uTmpIP, 
					INTEL_SEGMENT_SIZE_BITS / CHAR_BIT );

				uTmpIP += INTEL_SEGMENT_SIZE_BITS / CHAR_BIT;


				lpDisasm->Instruction.Operands[i].AddrRegister = 
					(ADDRESSING_REGISTER)(AR_ADDRESS + uTempByteReader);

				//Calculate how many bytes were consumed
				nConsumedBytes += uTmpIP - lpDisasm->InstructionPointer;

				break;

			case ADDR_B://VEX.vvvvGPRfield not in x86
				dprintf( 2, "Operand %d: ADDR_B (VEX GPR)", i );
				break;

			case ADDR_C://reg selects CR
				dprintf( 2, "Operand %d: ADDR_C (CR REG)", i );
				lpDisasm->Instruction.Operands[i].BitSize = INTEL_DEFAULT_SIZE;
				intel_modrm( (ADDRESSING_MAP *)&addressing_cr, 
					&lpDisasm->Instruction.Operands[i], 
					(uchar *)lpDisasm->InstructionPointer, OPERAND_REGISTER, 
					&bUsedSIB );

				lpDisasm->Instruction.Operands[i].Type = 
					(OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_CR);

				bUsedMod = TRUE;
				break;

			case ADDR_D://reg selects DR
				dprintf( 2, "Operand %d: ADDR_D (DR REG)", i );
				lpDisasm->Instruction.Operands[i].BitSize = INTEL_DEFAULT_SIZE;
				intel_modrm( (ADDRESSING_MAP *)&addressing_dr, 
					&lpDisasm->Instruction.Operands[i], 
					(uchar *)lpDisasm->InstructionPointer, OPERAND_REGISTER, 
					&bUsedSIB );

				lpDisasm->Instruction.Operands[i].Type = 
					(OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_DR);

				bUsedMod = TRUE;
				break;

			case ADDR_E://modR/M specifies operand
				dprintf( 2, "Operand %d: ADDR_E (ModRM Reg)", i );

				lpDisasm->Instruction.Operands[i].BitSize = get_operand_size_x86( 
					(ENUM_INTEL_OPERAND_TYPE)lpInstruction->Parameters[i].eSize, 
					lpDisasm );
				uBitSize = get_addressing_size_x86( lpDisasm );
				lpTmpAddressingMap = (ADDRESSING_MAP *)(( uBitSize == BIT32 ) ?
					&addressing_modrm_32 : &addressing_modrm_16);
				lpDisasm->Instruction.Operands[i].AddressingSize = uBitSize;
				nConsumedBytes += intel_modrm( lpTmpAddressingMap,
					&lpDisasm->Instruction.Operands[i], 
					(uchar *)lpDisasm->InstructionPointer, OPERAND_ALL, &bUsedSIB );

				if( lpDisasm->Instruction.Operands[i].Type == OPERAND_REGISTER ) {
					lpDisasm->Instruction.Operands[i].Type = 
						(OPERAND_TYPE)(lpDisasm->Instruction.Operands[i].Type + 
						INTEL_TYPE_REG_GENERAL);
				}
				else {
					lpDisasm->Instruction.Operands[i].AddrRegister = 
						intel_get_addessing_reg( lpDisasm, 
						&lpDisasm->Instruction.Operands[i] );
				}

				bUsedMod = TRUE;
				break;

			case ADDR_F://FLAGS register
				dprintf( 2, "Operand %d: ADDR_F (FLAGS Reg)", i );
				lpDisasm->Instruction.Operands[i].Type = 
					(OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_FLAGS);
				lpDisasm->Instruction.Operands[i].BitSize = get_operand_size_x86(
					(ENUM_INTEL_OPERAND_TYPE)lpInstruction->Parameters[i].eSize, 
					lpDisasm );
				break;

			case ADDR_G://reg selects GPR
				dprintf( 2, "Operand %d: ADDR_G (ModRM Reg GPR)", i );

				 uBitSize = get_operand_size_x86( 
					(ENUM_INTEL_OPERAND_TYPE)lpInstruction->Parameters[i].eSize, 
					lpDisasm );
				lpDisasm->Instruction.Operands[i].BitSize = uBitSize;
				switch( uBitSize ) {
				case BIT8:
					lpTmpAddressingMap = (ADDRESSING_MAP *)&addressing_reg_8;
					break;
				case BIT16:
					lpTmpAddressingMap = (ADDRESSING_MAP *)&addressing_reg_16;
					break;
				case BIT32:
					lpTmpAddressingMap = (ADDRESSING_MAP *)&addressing_reg_32;
					break;
				default:
					lpTmpAddressingMap = (ADDRESSING_MAP *)&addressing_reg_32;
				}
				//uBitSize = get_addressing_size_x86( lpDisasm );
				//lpDisasm->Instruction.Operands[i].AddressingSize = uBitSize;
				nConsumedBytes += intel_modrm( lpTmpAddressingMap, 
					&lpDisasm->Instruction.Operands[i], 
					(uchar *)lpDisasm->InstructionPointer, OPERAND_REGISTER, 
					&bUsedSIB );

				lpDisasm->Instruction.Operands[i].Type += INTEL_TYPE_REG_GENERAL;
				bUsedMod = TRUE;
				break;

			case ADDR_H://VEX.vvvvselectsXMMorYMMregister
				dprintf( 2, "Operand %d: ADDR_H (VEX Selects XMM/YMM)", i );
				break;

			case ADDR_I://Immediate data
				dprintf( 2, "Operand %d: ADDR_I (Immidiate)", i );
				uBitSize = get_operand_size_x86( 
					(ENUM_INTEL_OPERAND_TYPE)lpInstruction->Parameters[i].eSize, 
					lpDisasm );

				lpDisasm->Instruction.Operands[i].Type = OPERAND_IMMIDIATE;
				lpDisasm->Instruction.Operands[i].BitSize = (BITSIZE)uBitSize;
				uTmpIP = lpDisasm->InstructionPointer;
				if( bUsedMod ) uTmpIP++;
				if( bUsedSIB ) uTmpIP++;
				memcpy( &lpDisasm->Instruction.Operands[i].RegValMem.Value, 
					(void *)uTmpIP, uBitSize / CHAR_BIT );
				nConsumedBytes += uBitSize / CHAR_BIT;
				break;

			case ADDR_J://Relative offset
				dprintf( 2, "Operand %d: ADDR_J (Relative Offset)", i );
				uBitSize = get_operand_size_x86( 
					(ENUM_INTEL_OPERAND_TYPE)lpInstruction->Parameters[i].eSize, 
					lpDisasm );
				lpDisasm->Instruction.Operands[i].BitSize = (BITSIZE)uBitSize;
				lpDisasm->Instruction.Operands[i].Type = OPERAND_IMMIDIATE_ADDR;
				memcpy( &lpDisasm->Instruction.Operands[i].RegValMem.RelAddress, 
					(void *)lpDisasm->InstructionPointer, uBitSize / CHAR_BIT );

				fix_sign32( uBitSize, 
					(int32 *)&lpDisasm->Instruction.Operands[i].RegValMem.RelAddress );
				
				nConsumedBytes += uBitSize / CHAR_BIT;
				if( lpDisasm->VirtualAddress == 0 ) {
					lpDisasm->Instruction.Operands[i].TargetAddress = 
						lpDisasm->Instruction.Operands[i].RegValMem.RelAddress + 
						lpDisasm->InstructionPointer;
				}
				else {
					lpDisasm->Instruction.Operands[i].TargetAddress = 
						lpDisasm->Instruction.Operands[i].RegValMem.RelAddress + 
						lpDisasm->InstructionPointer + 
						lpDisasm->InternalStuff.nVADelta + nConsumedBytes;
				}
				
				break;

			case ADDR_L://Immidiate with upper 4bits selecting XMM or YMM register
				dprintf( 2, "Operand %d: ADDR_L Not Implemented yet", i );
				break;

			case ADDR_M://ModR/M only memory
				dprintf( 2, "Operand %d: ADDR_M (ModR/M Selects Memory)", i );
				//Special memory 
				lpDisasm->Instruction.Operands[i].BitSize = get_operand_size_x86( 
					(ENUM_INTEL_OPERAND_TYPE)lpInstruction->Parameters[i].eSize, 
					lpDisasm );

				uBitSize = get_addressing_size_x86( lpDisasm );
				lpTmpAddressingMap = (ADDRESSING_MAP *)(( uBitSize == BIT32 ) ?
					&addressing_modrm_32 : &addressing_modrm_16);
				
				lpDisasm->Instruction.Operands[i].AddressingSize = uBitSize;
				nConsumedBytes += intel_modrm( lpTmpAddressingMap, 
					&lpDisasm->Instruction.Operands[i], 
					(uchar *)lpDisasm->InstructionPointer, OPERAND_ALL, //XXX OPERAND_MEMORY
					&bUsedSIB );

				lpDisasm->Instruction.Operands[i].AddrRegister = 
					intel_get_addessing_reg( lpDisasm, 
					&lpDisasm->Instruction.Operands[i] );

				bUsedMod = TRUE;
				break;
			case ADDR_N://R/M selects packedquadward MMX
				dprintf( 2, "Operand %d: ADDR_N (MMX)", i );
				lpDisasm->Instruction.Operands[i].BitSize = get_operand_size_x86( 
					(ENUM_INTEL_OPERAND_TYPE)lpInstruction->Parameters[i].eSize, 
					lpDisasm );
				
				uBitSize = get_addressing_size_x86( lpDisasm );
				lpTmpAddressingMap = (ADDRESSING_MAP *)(( uBitSize == BIT32 ) ?
					&addressing_modrm_32 : &addressing_modrm_16);

				if( intel_modrm( 
					lpTmpAddressingMap,	&lpDisasm->Instruction.Operands[i], 
					(uchar *)lpDisasm->InstructionPointer,
					OPERAND_REGISTER, &bUsedSIB ) == INSTRUCTION_INVALID ) {
						eprintf( "ADDR_N ModR/M gives a memory location when only"
							"OPERAND_REGISTER is allowed.");
						lpDisasm->Instruction.bIsValid = FALSE;
				}
				//Overwrite what modrm_32 gives
				lpDisasm->Instruction.Operands[i].Type = 
					(OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_MMX);
				bUsedMod = TRUE;
				break;
			case ADDR_O://No ModR/M operand in instruction
				dprintf( 2, "Operand %d: ADDR_O (Immidiate Memory)", i );
				uBitSize = get_operand_size_x86( 
					(ENUM_INTEL_OPERAND_TYPE)lpInstruction->Parameters[i].eSize, 
					lpDisasm );

				lpDisasm->Instruction.Operands[i].Type = OPERAND_MEMORY;
				lpDisasm->Instruction.Operands[i].BitSize = (BITSIZE)uBitSize;
				uBitSize = get_addressing_size_x86( lpDisasm );
				memcpy( &lpDisasm->Instruction.Operands[i].RegValMem.Memory.Displacement, 
					(void *)lpDisasm->InstructionPointer, uBitSize / CHAR_BIT );
				nConsumedBytes += uBitSize / CHAR_BIT;
				lpDisasm->Instruction.Operands[i].AddrRegister = 
					intel_get_addessing_reg( lpDisasm, 
					&lpDisasm->Instruction.Operands[i] );
				break;
			case ADDR_P://regs Selects MMX reg
				dprintf( 2, "Operand %d: ADDR_P (reg selects MMX)", i );
				intel_modrm( (ADDRESSING_MAP *)&addressing_reg_mmx,
					&lpDisasm->Instruction.Operands[i],
					(uchar *)lpDisasm->InstructionPointer,
					OPERAND_REGISTER, &bUsedSIB );
				lpDisasm->Instruction.Operands[i].BitSize = get_operand_size_x86( 
					(ENUM_INTEL_OPERAND_TYPE)lpInstruction->Parameters[i].eSize, 
					lpDisasm );
				//XXX this type of code should be removed in latter versions and
				//moved in intel_modrm
				lpDisasm->Instruction.Operands[i].Type = OPERAND_REGISTER + 
					INTEL_TYPE_REG_MMX;
				bUsedMod = TRUE;
				break;
			case ADDR_Q://ModR/M selects MMX or memory
				dprintf( 2, "Operand %d: ADDR_Q (ModR/M Selects MMX or Memory)", i );
				lpDisasm->Instruction.Operands[i].BitSize = get_operand_size_x86( 
					(ENUM_INTEL_OPERAND_TYPE)lpInstruction->Parameters[i].eSize, 
					lpDisasm );
				
				uBitSize = get_addressing_size_x86( lpDisasm );
				lpTmpAddressingMap = (ADDRESSING_MAP *)(( uBitSize == BIT32 ) ?
					&addressing_modrm_32 : &addressing_modrm_16);

				if( intel_modrm( 
					lpTmpAddressingMap, &lpDisasm->Instruction.Operands[i], 
					(uchar *)lpDisasm->InstructionPointer,
					OPERAND_ALL, &bUsedSIB ) == INSTRUCTION_INVALID ) {
						eprintf( "ADDR_N ModR/M gives a memory location when only"
							"OPERAND_REGISTER is allowed.");
						lpDisasm->Instruction.bIsValid = FALSE;
				}

				if( lpDisasm->Instruction.Operands[i].Type & OPERAND_REGISTER ) {
					lpDisasm->Instruction.Operands[i].Type += INTEL_TYPE_REG_MMX;
				}
				else {
					lpDisasm->Instruction.Operands[i].AddrRegister = 
						intel_get_addessing_reg( lpDisasm, 
						&lpDisasm->Instruction.Operands[i] );
				}
				bUsedMod = TRUE;
				break;
			case ADDR_R://R/M refers only to GPO
				dprintf( 2, "Operand %d: ADDR_R (ModR/M Selects GPO)", i );
				lpDisasm->Instruction.Operands[i].BitSize = get_operand_size_x86( 
					(ENUM_INTEL_OPERAND_TYPE)lpInstruction->Parameters[i].eSize, 
					lpDisasm );

				uBitSize = get_addressing_size_x86( lpDisasm );
				lpTmpAddressingMap = (ADDRESSING_MAP *)(( uBitSize == BIT32 ) ?
					&addressing_modrm_32 : &addressing_modrm_16);

				if( intel_modrm( 
					lpTmpAddressingMap, &lpDisasm->Instruction.Operands[i], 
					(uchar *)lpDisasm->InstructionPointer,
					OPERAND_REGISTER, &bUsedSIB ) == INSTRUCTION_INVALID ) {
						eprintf( "ADDR_R ModR/M gives a memory location when only"
							"OPERAND_REGISTER is allowed.");
				}

				lpDisasm->Instruction.Operands[i].Type = (OPERAND_TYPE)(
					OPERAND_REGISTER+INTEL_TYPE_REG_GENERAL);

				bUsedMod = TRUE;
				break;
			case ADDR_S://reg selects Sreg
				dprintf( 2, "Operand %d: ADDR_S (Segment Register)", i );
				lpDisasm->Instruction.Operands[i].BitSize = INTEL_SEGMENT_SIZE_BITS;
				intel_modrm( (ADDRESSING_MAP *)&addressing_sreg, 
					&lpDisasm->Instruction.Operands[i], 
					(uchar *)lpDisasm->InstructionPointer, OPERAND_REGISTER, 
					&bUsedSIB );
				lpDisasm->Instruction.Operands[i].Type = 
					(OPERAND_TYPE)(OPERAND_REGISTER+INTEL_TYPE_REG_SEGMENT);

				bUsedMod = TRUE;
				break;
			case ADDR_U://R/M selects XMM or YMM reg
				dprintf( 2, "Operand %d: ADDR_U (ModR/M Selects XMM (or YMM)", i );
				uBitSize = get_addressing_size_x86( lpDisasm );
				lpTmpAddressingMap = (ADDRESSING_MAP *)(( uBitSize == BIT32 ) ?
					&addressing_modrm_32 : &addressing_modrm_16);

				nConsumedBytes += intel_modrm(  lpTmpAddressingMap, 
					&lpDisasm->Instruction.Operands[i], 
					(uchar *)lpDisasm->InstructionPointer, OPERAND_ALL, &bUsedSIB );

				lpDisasm->Instruction.Operands[i].BitSize = get_operand_size_x86( 
					(ENUM_INTEL_OPERAND_TYPE)lpInstruction->Parameters[i].eSize, 
					lpDisasm );

				lpDisasm->Instruction.Operands[i].BitSize = (BITSIZE) uBitSize;
				if( lpDisasm->Instruction.Operands[i].Type == OPERAND_REGISTER ) {
					if( uBitSize == BIT128 ) {
						lpDisasm->Instruction.Operands[i].Type = 
							(OPERAND_TYPE)(OPERAND_REGISTER + INTEL_TYPE_REG_SSE);
					}
					else if( uBitSize = BIT256 ){
						lpDisasm->Instruction.Operands[i].Type = 
							(OPERAND_TYPE)(OPERAND_REGISTER + INTEL_TYPE_REG_YMM);
					}
					else {
						eprintf( "ADDR_U Selects XMM or YMM but different size reg"
							" was given" );
					}
				}
				bUsedMod = TRUE;
				break;

			case ADDR_V://reg selects XMM or YMM
				dprintf( 2, "Operand %d: ADDR_V (reg selects XMM or YMM)", i );
				intel_modrm( (ADDRESSING_MAP *)&addressing_reg_sse, 
					&lpDisasm->Instruction.Operands[i], 
					(uchar *)lpDisasm->InstructionPointer, OPERAND_REGISTER, 
					&bUsedSIB );
				
				uBitSize = get_operand_size_x86( 
					(ENUM_INTEL_OPERAND_TYPE)lpInstruction->Parameters[i].eSize, 
					lpDisasm );
				lpDisasm->Instruction.Operands[i].BitSize = (BITSIZE)uBitSize;

				if( uBitSize = BIT256 ){
					lpDisasm->Instruction.Operands[i].Type = 
						(OPERAND_TYPE)(OPERAND_REGISTER + INTEL_TYPE_REG_YMM);
				}
				else {
					lpDisasm->Instruction.Operands[i].Type = 
						(OPERAND_TYPE)(OPERAND_REGISTER + INTEL_TYPE_REG_SSE);
				}
				bUsedMod = TRUE;
				break;

			case ADDR_W://ModR/M selects XMM or YMM or memory
				dprintf( 2, "Operand %d: ADDR_W (ModR/M Selects XMM/YMM or Memory)", i );
				
				uBitSize = get_addressing_size_x86( lpDisasm );
				lpTmpAddressingMap = (ADDRESSING_MAP *)(( uBitSize == BIT32 ) ?
					&addressing_modrm_32 : &addressing_modrm_16);

				lpDisasm->Instruction.Operands[i].BitSize = get_operand_size_x86( 
					(ENUM_INTEL_OPERAND_TYPE)lpInstruction->Parameters[i].eSize, 
					lpDisasm );

				nConsumedBytes += intel_modrm( lpTmpAddressingMap, 
					&lpDisasm->Instruction.Operands[i], 
					(uchar *)lpDisasm->InstructionPointer,
					OPERAND_ALL, &bUsedSIB );

				if( ( lpDisasm->Instruction.Operands[i].Type & 
					OPERAND_ADDRESSING_TYPE_MASK ) == OPERAND_REGISTER ) {
						if( uBitSize == BIT256 ) {
							lpDisasm->Instruction.Operands[i].Type = 
								(OPERAND_TYPE)(OPERAND_REGISTER + INTEL_TYPE_REG_YMM);
						}
						else {
							lpDisasm->Instruction.Operands[i].Type = 
								(OPERAND_TYPE)(OPERAND_REGISTER + INTEL_TYPE_REG_SSE);
						}
				}
				else {
					lpDisasm->Instruction.Operands[i].AddrRegister = 
						intel_get_addessing_reg( lpDisasm, 
						&lpDisasm->Instruction.Operands[i] );
				}
				bUsedMod = TRUE;
				break;

			case ADDR_X://Memory addressed by DS:[rSI]
				dprintf( 2, "Operand %d: ADDR_X (DS:[rSI])", i );
				lpDisasm->Instruction.Operands[i].Type = OPERAND_MEMORY;
				lpDisasm->Instruction.Operands[i].AddrRegister = 
					(ADDRESSING_REGISTER)(AR_REGISTER + REG_DS);

				lpDisasm->Instruction.Operands[i].BitSize = 
					get_operand_size_x86( TYPE_v, lpDisasm );

				lpDisasm->Instruction.Operands[i].AddressingSize = 
					get_addressing_size_x86( lpDisasm );

				lpDisasm->Instruction.Operands[i].RegValMem.Register = 
					(REG)REG_ESI;

				break;

			case ADDR_Y://Memory addressed by ES:[rDI]
				dprintf( 2, "Operand %d: ADDR_Y (ES:[rDI])", i );
				lpDisasm->Instruction.Operands[i].Type = OPERAND_MEMORY;
				lpDisasm->Instruction.Operands[i].AddrRegister = 
					(ADDRESSING_REGISTER)(AR_REGISTER + REG_ES);

				lpDisasm->Instruction.Operands[i].BitSize = 
					get_operand_size_x86( TYPE_v, lpDisasm );

				lpDisasm->Instruction.Operands[i].AddressingSize = 
					get_addressing_size_x86( lpDisasm );

				lpDisasm->Instruction.Operands[i].RegValMem.Register = 
					(REG)REG_EDI;
				break;
			}
		}
	}
	if( bUsedMod ) nConsumedBytes++;
	if( bUsedSIB ) nConsumedBytes++;
	return nConsumedBytes;
}

int intel_group_handler( PSIDISASM lpDisasm, GROUP_MAP * lpGroupMap )
{
	PLUGIN_MAP * lpPluginMap = NULL;
	INSTRUCTIONINTERNAL * lpInstruction;
	int nConsumedBytes = 0;

	uchar * lpInstrPointer = (uchar *)lpDisasm->InstructionPointer;
	uint8 AddrByte = 0;
	AddrByte = ( lpGroupMap->uMask & *lpInstrPointer ) >> 
		lpGroupMap->nRightShift;


	lpPluginMap = &lpGroupMap->sPluginMap[AddrByte];
	if( lpPluginMap->hint_t == MAP_INVALID ) {
		return INSTRUCTION_INVALID;
	}

	lpInstruction = &lpPluginMap->uItem.sInstruction;

	lpDisasm->InternalStuff.nOpcodeIndex = lpInstruction->uOpcodeMap.eOpcodePtr;
	lpDisasm->Instruction.Category = intel_get_opcode_category( 
		(ENUM_OPCODE_PTR)lpInstruction->uOpcodeMap.eOpcodePtr);

	if( lpInstruction->Parameters[0].eAddressing != ADDR_INVALID ) {
		nConsumedBytes += intel_addressing_handler( lpDisasm, lpInstruction );
		//If addressing doesn't consume any bytes (unlikely i think) then consume
		//this byte
		if( nConsumedBytes == 0 ) 
			nConsumedBytes++;
	}

	return nConsumedBytes;
}