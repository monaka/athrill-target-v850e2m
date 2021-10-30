#include "op_exec_ops.h"
#include "cpu.h"
#include "bus.h"

bool op_exec_cond(TargetCoreType *cpu, uint16 cond)
{
	uint16 is_br = FALSE;
	uint16 flg_s = CPU_ISSET_S(&cpu->reg);
	uint16 flg_ov = CPU_ISSET_OV(&cpu->reg);
	uint16 flg_z = CPU_ISSET_Z(&cpu->reg);
	uint16 flg_cy = CPU_ISSET_CY(&cpu->reg);
	uint16 flg_sat = CPU_ISSET_SAT(&cpu->reg);

	switch (cond) {
	case 0b1110:	//BGE
		if ((flg_s ^ flg_ov) == FALSE) {
			is_br = TRUE;
		}
		break;
	case 0b1111:	//BGT
		if (((flg_s ^ flg_ov) | flg_z ) == FALSE) {
			is_br = TRUE;
		}
		break;
	case 0b0111:	//BLE
		if (((flg_s ^ flg_ov) | flg_z ) == TRUE) {
			is_br = TRUE;
		}
		break;
	case 0b0110:	//BLT
		if ((flg_s ^ flg_ov) == TRUE) {
			is_br = TRUE;
		}
		break;

	case 0b1011:	//BH
		if ((flg_cy | flg_z) == FALSE) {
			is_br = TRUE;
		}
		break;
	case 0b0001:	//BL
/*	case 0b0001: */	//BC
		if ((flg_cy) == TRUE) {
			is_br = TRUE;
		}
		break;
	case 0b0011:	//BNH
		if ((flg_cy | flg_z) == TRUE) {
			is_br = TRUE;
		}
		break;
	case 0b1001:	//BNL
/*	case 0b1001: */	//BNC
		if ((flg_cy) == FALSE) {
			is_br = TRUE;
		}
		break;

	case 0b0010:	//BE
/*	case 0b0010: */	//BZ
		if ((flg_z) == TRUE) {
			is_br = TRUE;
		}
		break;
	case 0b1010:	//BNZ
		if ((flg_z) == FALSE) {
			is_br = TRUE;
		}
		break;

	case 0b0100:	//BN
		if ((flg_s) == TRUE) {
			is_br = TRUE;
		}
		break;
	case 0b1000:	//BNV
		if ((flg_ov) == FALSE) {
			is_br = TRUE;
		}
		break;
	case 0b1100:	//BP
		if ((flg_s) == FALSE) {
			is_br = TRUE;
		}
		break;
	case 0b0101:	//BR
		is_br = TRUE;
		break;
	case 0b1101:	//BSA
		if ((flg_sat) == TRUE) {
			is_br = TRUE;
		}
		break;
	case 0b0000:	//BV
		if ((flg_ov) == TRUE) {
			is_br = TRUE;
		}
		break;
	default:
		break;
	}
	return is_br;
}

static void op_exec_bcond(TargetCoreType *cpu, uint16 cond, sint32 disp, sint32 code_size)
{
	uint16 is_br = op_exec_cond(cpu, cond);
	if (is_br == TRUE) {
		sint32 pc = cpu->reg.pc;
		pc = pc + disp;
		DBG_PRINT((DBG_EXEC_OP_BUF(), DBG_EXEC_OP_BUF_LEN(), "0x%x: B cond(0x%x):0x%x\n", cpu->reg.pc, cond, pc));
		cpu->reg.pc = pc;
	}
	else {
		sint32 pc = cpu->reg.pc + code_size;
		DBG_PRINT((DBG_EXEC_OP_BUF(), DBG_EXEC_OP_BUF_LEN(), "0x%x: Bcond(0x%x):0x%x\n", cpu->reg.pc, cond, pc));
		cpu->reg.pc = pc;
	}
}
/*
 * Format1
 */
int op_exec_jmp(TargetCoreType *cpu)
{
	uint32 reg1 = cpu->decoded_code->type1.reg1;
	if (reg1 >= CPU_GREG_NUM) {
		return -1;
	}
	DBG_PRINT((DBG_EXEC_OP_BUF(), DBG_EXEC_OP_BUF_LEN(), "0x%x: JMP r%d(0x%x)\n", cpu->reg.pc, reg1, cpu->reg.r[reg1]));
	cpu->reg.pc = cpu->reg.r[reg1];
	return 0;
}


/*
 * Format3
 */
int op_exec_bcond_3(TargetCoreType *cpu)
{
	uint16 cond = cpu->decoded_code->type3.cond;
	uint32 disp_u;
	sint32 disp;

	disp_u = cpu->decoded_code->type3.disp << 1;
	disp = op_sign_extend(8, disp_u);

	op_exec_bcond(cpu, cond, disp, 2);
	return 0;
}

/*
 * Format5
 */
int op_exec_jr(TargetCoreType *cpu)
{
	uint32 reg2 = cpu->decoded_code->type5.reg2;
	sint32 pc = (sint32)cpu->reg.pc;
	sint32 disp;

	if (reg2 > 0) {
		cpu->reg.r[reg2] = cpu->reg.pc + 4;
	}
	disp = op_sign_extend(21, cpu->decoded_code->type5.disp);
	pc += disp;

	if (reg2 == 0) {
		DBG_PRINT((DBG_EXEC_OP_BUF(), DBG_EXEC_OP_BUF_LEN(), "0x%x: JR disp22(%d):0x%x\n", cpu->reg.pc, disp, pc));
	}
	else {
		DBG_PRINT((DBG_EXEC_OP_BUF(), DBG_EXEC_OP_BUF_LEN(), "0x%x: JARL disp22(%d):0x%x r%u(0x%x)\n", cpu->reg.pc, disp, pc, reg2, cpu->reg.r[reg2]));
	}

	cpu->reg.pc = pc;
	return 0;
}


/*
 * Format6
 */
static int op_exec_parse_jmp_addr_fmt6(const char* opcode, TargetCoreType* cpu, sint32 *disp32)
{
	uint16 imm_high16;
	uint32 imm_high;
	uint32 imm_high_addr;
	uint32 disp = cpu->decoded_code->type6.imm;

	imm_high_addr = cpu->reg.pc + 4U;
	int err = bus_get_data16(cpu->core_id, imm_high_addr, &imm_high16);
	if (err != STD_E_OK) {
		printf("ERROR:%s pc=0x%x type6.imm=%u(0x%x) high_addr=0x%x\n", opcode, cpu->reg.pc, disp, disp, imm_high_addr);
		return -1;
	}
	imm_high = (uint32)(imm_high16);
	*disp32 = (sint32)( (uint32)((imm_high << 16U) | disp) );
	return 0;
}

int op_exec_jmp_6(TargetCoreType *cpu)
{
	uint32 reg1 = cpu->decoded_code->type6.reg1;
	sint32 disp;

	if (reg1 >= CPU_GREG_NUM) {
		return -1;
	}
	int ret = op_exec_parse_jmp_addr_fmt6("JMP", cpu, &disp);
	if (ret < 0) {
		return -1;
	}

	DBG_PRINT((DBG_EXEC_OP_BUF(), DBG_EXEC_OP_BUF_LEN(), "0x%x: JMP disp32(%u) r%d(0x%x)\n", cpu->reg.pc, disp, reg1, cpu->reg.r[reg1] + disp));
	cpu->reg.pc = (uint32)(cpu->reg.r[reg1] + disp);

	return 0;
}

int op_exec_jarl_6(TargetCoreType *cpu)
{
	sint32 reg1 = cpu->decoded_code->type6.reg1;
	sint32 disp;
	sint32 pc = (sint32)cpu->reg.pc;

	int ret = op_exec_parse_jmp_addr_fmt6("JARL", cpu, &disp);
	if (ret < 0) {
		return -1;
	}
	pc += disp;

	DBG_PRINT((DBG_EXEC_OP_BUF(), DBG_EXEC_OP_BUF_LEN(), "0x%x: JARL disp32(%u) r%u(0x%x):0x%x r%u(0x%x)\n",
			cpu->reg.pc,
			disp,
			reg1, cpu->reg.r[reg1],
			pc,
			reg1, cpu->reg.pc + 6));

	cpu->reg.r[reg1] = cpu->reg.pc + 6;

	cpu->reg.pc = pc;
	return 0;
}

int op_exec_jr_6(TargetCoreType *cpu)
{
	sint32 disp;
	sint32 pc = (uint32)cpu->reg.pc;

	int ret = op_exec_parse_jmp_addr_fmt6("JR", cpu, &disp);
	if (ret < 0) {
		return -1;
	}

	pc += disp;

	DBG_PRINT((DBG_EXEC_OP_BUF(), DBG_EXEC_OP_BUF_LEN(), "0x%x: JR disp32(%u):0x%x\n",
			cpu->reg.pc,
			disp,
			pc));

	cpu->reg.pc = pc;
	return 0;
}

