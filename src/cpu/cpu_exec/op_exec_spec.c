#include "op_exec_ops.h"
#include "cpu.h"
#include "bus.h"
#include "device.h"
#include "std_cpu_ops.h"
#include <stdio.h>

static int set_sysreg_grp_bnk(CpuRegisterType *cpu, uint32 sysreg)
{
	uint32 bnk = (sysreg & 0x000000FF);
	uint32 grp = (sysreg & 0x0000FF00) >> 8U;
	uint32 reg_bnk;
	uint32 reg_grp;
	
	switch (grp) {
	case 0x00:
		reg_grp = SYS_GRP_CPU;
		if (bnk == 0x00) {
			reg_bnk = SYS_GRP_CPU_BNK_0;
		}
		else if (bnk == 0x10) {
			reg_bnk = SYS_GRP_CPU_BNK_1;
		}
		else if (bnk == 0x11) {
			reg_bnk = SYS_GRP_CPU_BNK_2;
		}
		else {
			goto errdone;
		}
		break;
	case 0x10:
		reg_grp = SYS_GRP_PROSESSOR;
		if (bnk == 0x00) {
			reg_bnk = SYS_GRP_CPU_BNK_0;
		}
		else if (bnk == 0x01) {
			reg_bnk = SYS_GRP_CPU_BNK_1;
		}
		else if (bnk == 0x10) {
			reg_bnk = SYS_GRP_CPU_BNK_2;
		}
		else {
			goto errdone;
		}
		break;
	case 0x11:
		reg_grp = SYS_GRP_PMU;
		if (bnk == 0x00) {
			reg_bnk = SYS_GRP_CPU_BNK_0;
		}
		else {
			goto errdone;
		}
		break;
	case 0x20:
		reg_grp = SYS_GRP_FPU;
		if (bnk == 0x00) {
			reg_bnk = SYS_GRP_CPU_BNK_0;
		}
		else {
			goto errdone;
		}
		break;
	case 0xFF:
		reg_grp = SYS_GRP_USER;
		if (bnk == 0x00) {
			reg_bnk = SYS_GRP_CPU_BNK_0;
		}
		else if (bnk == 0xFF) {
			reg_bnk = SYS_GRP_CPU_BNK_1;
		}
		else {
			goto errdone;
		}
		break;
	default:
		goto errdone;
	}
	cpu->sys.current_grp = reg_grp;
	cpu->sys.current_bnk = reg_bnk;
	return 0;

errdone:
	printf("ERROR: invalid set sysreg(0x%x)\n", sysreg);
	return -1;
}

static int get_sysreg(CpuRegisterType *cpu, uint32 regid, uint32 **regp)
{
	if (regid >= CPU_GREG_NUM) {
		return -1;
	}

	*regp = cpu_get_sysreg(&cpu->sys, regid);

	return 0;
}

/*
 * protect:31-6
 */
#define PSW_PROTECT_VALUE(psw)	((psw) & 0xFFFFFFC0)
static inline bool has_permission_psw_change(uint32 org_psw, uint32 new_psw)
{
	if (PSW_PROTECT_VALUE(org_psw) != PSW_PROTECT_VALUE(new_psw)) {
		return FALSE;
	}
	return TRUE;
}

static bool ldsr_grp_cpu_has_permission(TargetCoreType *cpu, uint32 regid, uint32 reg2_data)
{
	bool permission = FALSE;
	uint32 psw = cpu_get_psw(&cpu->reg.sys);
	/*
	 * SYS_GRP_CPU_BNK_0
	 * SYS_GRP_CPU_BNK_1
	 * SYS_GRP_CPU_BNK_2
	 */
	switch (cpu->reg.sys.current_bnk) {
	case SYS_GRP_CPU_BNK_0:
		if (regid == SYS_REG_PSW) {
			permission = has_permission_psw_change(psw, reg2_data);
		}
		break;
	case SYS_GRP_CPU_BNK_1:
	case SYS_GRP_CPU_BNK_2:
	default:
		break;
	}
	return permission;
}

static bool ldsr_has_permission(TargetCoreType *cpu, uint32 regid, uint32 reg2_data)
{
	uint32 psw = cpu_get_psw(&cpu->reg.sys);
	bool permission = FALSE;

	if (IS_TRUSTED_NPV(psw)) {
		return TRUE;
	}
	/*
	 * SYS_GRP_CPU
	 * SYS_GRP_PROSESSOR
	 * SYS_GRP_PMU
	 */
	switch (cpu->reg.sys.current_grp) {
	case SYS_GRP_CPU:
		permission = ldsr_grp_cpu_has_permission(cpu, regid, reg2_data);
		break;
	case SYS_GRP_PROSESSOR:
		break;
	default:
		/* not supported yet */
		break;
	}


	return permission;
}
/*
 * Format9
 */
int op_exec_ldsr(TargetCoreType *cpu)
{
	int ret;
	uint32 *sysreg;
	/*
	 * [ユーザーズマニュアルから抜粋]
	 * 注意 この命令では，ニモニック記述の都合上，ソース・レジスタを reg2としていますが，
	 * オペコード上はreg1のフィールドを使用しています。したがって，ニモニック記述と
	 * オペコードにおいて，レジスタ指定の意味付けがほかの命令と異なります。
	 * rrrrr： regID指定
	 * RRRRR： reg2指定
	 */
	uint32 regid = cpu->decoded_code->type9.reg2;
	uint32 reg2 = cpu->decoded_code->type9.gen;
	uint32 *factor_sysreg = cpu_get_mpu_illegal_factor_sysreg(&cpu->reg.sys);
	uint32 *setting_sysreg = cpu_get_mpu_settign_sysreg(&cpu->reg.sys);

	if (reg2 >= CPU_GREG_NUM) {
		printf("ERROR: ldsr reg=%d regID=%d\n", reg2, regid);
		return -1;
	}
	if (regid >= CPU_GREG_NUM) {
		printf("ERROR: ldsr reg=%d regID=%d\n", reg2, regid);
		return -1;
	}
	ret = get_sysreg(&cpu->reg, regid, &sysreg);
	if (ret < 0) {
		printf("ERROR: ldsr reg=%d regID=%d\n", reg2, regid);
		return -1;
	}
	if (!ldsr_has_permission(cpu, regid, cpu->reg.r[reg2])) {
		if (factor_sysreg[SYS_REG_MPV_VSECR] == 0U) {
			factor_sysreg[SYS_REG_MPV_VSTID] = setting_sysreg[SYS_REG_MPU_TID];
			factor_sysreg[SYS_REG_MPV_VSADR] = cpu->reg.pc;
		}
		factor_sysreg[SYS_REG_MPV_VSECR] = factor_sysreg[SYS_REG_MPV_VSECR] + 1U;
		DBG_PRINT((DBG_EXEC_OP_BUF(), DBG_EXEC_OP_BUF_LEN(), "0x%x: ERROR LDSR r%d(0x%x) regID(%d):NONE\n", cpu->reg.pc, reg2, cpu->reg.r[reg2], regid));
		goto done;
	}

	if (regid == 31U) { /* BSEL */
		ret = set_sysreg_grp_bnk(&cpu->reg, cpu->reg.r[reg2]);
		if (ret < 0) {
			printf("ERROR: ldsr reg=%d regID=%d\n", reg2, regid);
			return -1;
		}
	}

	DBG_PRINT((DBG_EXEC_OP_BUF(), DBG_EXEC_OP_BUF_LEN(), "0x%x: LDSR r%d(0x%x) regID(%d):0x%x\n", cpu->reg.pc, reg2, cpu->reg.r[reg2], regid, *sysreg));
	*sysreg = cpu->reg.r[reg2];
	//printf("pu->reg.sys.sysreg[CPU_SYSREG_BSEL]=0x%x\n", cpu->reg.sys.sysreg[CPU_SYSREG_BSEL]);
	if (cpu->reg.sys.sysreg[CPU_SYSREG_BSEL] == CPU_CONFIG_BSEL_MPU_BNK_SETTING) {
		cpu_mpu_construct_containers(cpu->core_id);
	}
	else if (cpu->reg.sys.sysreg[CPU_SYSREG_BSEL] == CPU_CONFIG_BSEL_FPU_SETTING) {
		fpu_sync_sysreg(cpu, regid, cpu->reg.sys.current_grp);
	}

done:	
	cpu->reg.pc += 4;

	return 0;
}

int op_exec_stsr(TargetCoreType *cpu)
{
	int ret;
	uint32 *sysreg;
	uint32 regid = cpu->decoded_code->type9.gen;
	uint32 reg2 = cpu->decoded_code->type9.reg2;

	if (reg2 >= CPU_GREG_NUM) {
		return -1;
	}
	if (regid >= CPU_GREG_NUM) {
		return -1;
	}
	ret = get_sysreg(&cpu->reg, regid, &sysreg);
	if (ret < 0) {
		return -1;
	}
	DBG_PRINT((DBG_EXEC_OP_BUF(), DBG_EXEC_OP_BUF_LEN(), "0x%x: STSR regID(%d) r%d(0x%x):0x%x\n", cpu->reg.pc, regid, reg2, cpu->reg.r[reg2], *sysreg));
	cpu->reg.r[reg2] = *sysreg;

	cpu->reg.pc += 4;
	return 0;
}

/*
 * Format10
 */


int op_exec_diei(TargetCoreType *cpu)
{
	if (cpu->decoded_code->type10.gen1 == 0x04) {
		/* EI */
		CPU_CLR_ID(&cpu->reg);
		DBG_PRINT((DBG_EXEC_OP_BUF(), DBG_EXEC_OP_BUF_LEN(), "0x%x: EI\n", cpu->reg.pc));
	}
	else {
		/* DI */
		CPU_SET_ID(&cpu->reg);
		DBG_PRINT((DBG_EXEC_OP_BUF(), DBG_EXEC_OP_BUF_LEN(), "0x%x: DI\n", cpu->reg.pc));

	}
	cpu->reg.pc += 4;

	return 0;
}

int op_exec_nop(TargetCoreType *cpu)
{
	DBG_PRINT((DBG_EXEC_OP_BUF(), DBG_EXEC_OP_BUF_LEN(), "0x%x: NOP\n", cpu->reg.pc));

	cpu->reg.pc += 2;

	return 0;
}
int op_exec_reti(TargetCoreType *cpu)
{
	if (CPU_ISSET_EP(&cpu->reg)) {
		DBG_PRINT((DBG_EXEC_OP_BUF(), DBG_EXEC_OP_BUF_LEN(), "0x%x: RETI:0x%x\n", cpu->reg.pc, sys_get_cpu_base(&cpu->reg)->r[SYS_REG_EIPC]));
		cpu->reg.pc = sys_get_cpu_base(&cpu->reg)->r[SYS_REG_EIPC];
		sys_get_cpu_base(&cpu->reg)->r[SYS_REG_PSW] = sys_get_cpu_base(&cpu->reg)->r[SYS_REG_EIPSW];
		//CPU例外の場合は，ISPRの設定は行わないため不要
		//intc_clr_currlvl_ispr(cpu);
	}
	else if (CPU_ISSET_NP(&cpu->reg)) {
		DBG_PRINT((DBG_EXEC_OP_BUF(), DBG_EXEC_OP_BUF_LEN(), "0x%x: RETI:0x%x\n", cpu->reg.pc, sys_get_cpu_base(&cpu->reg)->r[SYS_REG_FEPC]));
		cpu->reg.pc = sys_get_cpu_base(&cpu->reg)->r[SYS_REG_FEPC];
		sys_get_cpu_base(&cpu->reg)->r[SYS_REG_PSW] = sys_get_cpu_base(&cpu->reg)->r[SYS_REG_FEPSW];
		intc_clr_nmi(cpu);
	}
	else {
		DBG_PRINT((DBG_EXEC_OP_BUF(), DBG_EXEC_OP_BUF_LEN(), "0x%x: RETI:0x%x\n", cpu->reg.pc, sys_get_cpu_base(&cpu->reg)->r[SYS_REG_EIPC]));
		//printf("0x%x: RETI:0x%x\n", cpu->cpu.pc, cpu->cpu.eipc);
		//fflush(stdout);
		cpu->reg.pc = sys_get_cpu_base(&cpu->reg)->r[SYS_REG_EIPC];
		sys_get_cpu_base(&cpu->reg)->r[SYS_REG_PSW] = sys_get_cpu_base(&cpu->reg)->r[SYS_REG_EIPSW];
		//intc_clr_currlvl_ispr(cpu);
	}
	return 0;
}

int op_exec_feret_10(TargetCoreType *cpu)
{
	DBG_PRINT((DBG_EXEC_OP_BUF(), DBG_EXEC_OP_BUF_LEN(), "0x%x: FERET:0x%x\n", cpu->reg.pc, sys_get_cpu_base(&cpu->reg)->r[SYS_REG_FEPC]));
	cpu->reg.pc = sys_get_cpu_base(&cpu->reg)->r[SYS_REG_FEPC];
	sys_get_cpu_base(&cpu->reg)->r[SYS_REG_PSW] = sys_get_cpu_base(&cpu->reg)->r[SYS_REG_FEPSW];
	return 0;
}
int op_exec_eiret_10(TargetCoreType *cpu)
{
	DBG_PRINT((DBG_EXEC_OP_BUF(), DBG_EXEC_OP_BUF_LEN(), "0x%x: EIRET:0x%x\n", cpu->reg.pc, sys_get_cpu_base(&cpu->reg)->r[SYS_REG_EIPC]));
	cpu->reg.pc = sys_get_cpu_base(&cpu->reg)->r[SYS_REG_EIPC];
	sys_get_cpu_base(&cpu->reg)->r[SYS_REG_PSW] = sys_get_cpu_base(&cpu->reg)->r[SYS_REG_EIPSW];
	return 0;
}

int op_exec_halt(TargetCoreType *cpu)
{
	DBG_PRINT((DBG_EXEC_OP_BUF(), DBG_EXEC_OP_BUF_LEN(), "0x%x: HALT:0x%x\n", cpu->reg.pc, cpu->reg.pc + 4));
	//printf("0x%x: HALT:0x%x\n", cpu->reg.pc, cpu->reg.pc + 4);
	//fflush(stdout);
	cpu->is_halt = TRUE;
	cpu->reg.pc += 4;
	return 0;
}

int op_exec_trap(TargetCoreType *cpu)
{
	int ret = -1;
	uint32 pc;
	uint32 eicc;
	uint32 ecr;
	uint32 vector = cpu->decoded_code->type10.gen2;

	if (vector <= 0x0F) {
		ret = 0;
		pc = 0x40;
		eicc = 0x40 + vector;
	}
	else if (vector <= 0x1F) {
		ret = 0;
		pc = 0x50;
		eicc = 0x50 + (0x0F & vector);
	}

	if (ret == 0) {
		if (CPU_ISSET_MPM_AUE(&cpu->reg.sys)) {
			CPU_CLR_IMP(&cpu->reg);
			CPU_CLR_DMP(&cpu->reg);
			CPU_CLR_NPV(&cpu->reg);
			CPU_CLR_PP(&cpu->reg);
		}

		DBG_PRINT((DBG_EXEC_OP_BUF(), DBG_EXEC_OP_BUF_LEN(), "0x%x: TRAP vector(0x%x):0x%x\n", cpu->reg.pc, vector, pc));
		sys_get_cpu_base(&cpu->reg)->r[SYS_REG_EIPC] = cpu->reg.pc + 4;
		sys_get_cpu_base(&cpu->reg)->r[SYS_REG_EIPSW] = sys_get_cpu_base(&cpu->reg)->r[SYS_REG_PSW];

		ecr = sys_get_cpu_base(&cpu->reg)->r[SYS_REG_ECR];
		ecr = ecr & 0x0000FFFF;
		ecr |= (eicc << 16);
		sys_get_cpu_base(&cpu->reg)->r[SYS_REG_ECR] = ecr;
		CPU_SET_EP(&cpu->reg);
		CPU_SET_ID(&cpu->reg);
		cpu->reg.pc = pc;
	}

	return 0;
}

int op_exec_fetrap_1(TargetCoreType *cpu)
{
	uint32 pc = 0x00000030;
	uint32 fepc;
	uint32 fecc;
	uint32 vector = cpu->decoded_code->type1.reg2;
	uint32 psw = sys_get_cpu_base(&cpu->reg)->r[SYS_REG_PSW];
	uint32 ecr = sys_get_cpu_base(&cpu->reg)->r[SYS_REG_ECR];

	fepc = cpu->reg.pc + 2U;
	fecc = 0x30 + vector;

	ecr = ecr & 0x0000FFFF;
	ecr |= (fecc << 16);

	sys_get_cpu_base(&cpu->reg)->r[SYS_REG_FEPC] = fepc;
	sys_get_cpu_base(&cpu->reg)->r[SYS_REG_FEIC] = fecc;
	sys_get_cpu_base(&cpu->reg)->r[SYS_REG_FEPSW] = psw;
	sys_get_cpu_base(&cpu->reg)->r[SYS_REG_ECR] = ecr;

	CPU_SET_EP(&cpu->reg);
	CPU_SET_ID(&cpu->reg);
	CPU_SET_NP(&cpu->reg);

	if (CPU_ISSET_MPM_AUE(&cpu->reg.sys)) {
		CPU_CLR_IMP(&cpu->reg);
		CPU_CLR_DMP(&cpu->reg);
		CPU_CLR_NPV(&cpu->reg);
		CPU_CLR_PP(&cpu->reg);
	}

	DBG_PRINT((DBG_EXEC_OP_BUF(), DBG_EXEC_OP_BUF_LEN(), "0x%x: FETRAP vector(0x%x):0x%x\n", cpu->reg.pc, vector, pc));

	cpu->reg.pc = pc;
	return 0;
}

int op_exec_syscall_10(TargetCoreType *cpu)
{
	Std_ReturnType err;
	uint32 pc;
	uint32 eipc;
	uint32 eiic;
	uint32 ecr;
	uint32 addr;
	uint32 data;
	bool backupDmp = FALSE;
	bool dmpIsTrusted = FALSE;
	uint32 psw = sys_get_cpu_base(&cpu->reg)->r[SYS_REG_PSW];
	uint32 scbp = sys_get_cpu_base(&cpu->reg)->r[SYS_REG_SCBP];
	uint32 sccfg_size = (sys_get_cpu_base(&cpu->reg)->r[SYS_REG_SCCFG] & 0x000000FF);
	uint32 vector8 = (cpu->decoded_code->type10.rfu3 << 5U)
		| (cpu->decoded_code->type10.gen2);

	eipc = cpu->reg.pc + 4U;
	eiic = 0x8000 + vector8;
	ecr = sys_get_cpu_base(&cpu->reg)->r[SYS_REG_ECR];
	ecr = ecr & 0xFFFF0000;
	ecr |= eiic;

	sys_get_cpu_base(&cpu->reg)->r[SYS_REG_EIPC] = eipc;
	sys_get_cpu_base(&cpu->reg)->r[SYS_REG_EIIC] = eiic;
	sys_get_cpu_base(&cpu->reg)->r[SYS_REG_EIPSW] = psw;
	sys_get_cpu_base(&cpu->reg)->r[SYS_REG_ECR] = ecr;

	CPU_SET_EP(&cpu->reg);
	CPU_SET_ID(&cpu->reg);

	if (CPU_ISSET_MPM_AUE(&cpu->reg.sys)) {
		CPU_CLR_IMP(&cpu->reg);
		CPU_CLR_DMP(&cpu->reg);
		CPU_CLR_NPV(&cpu->reg);
		CPU_CLR_PP(&cpu->reg);
	}
	else {
		backupDmp = TRUE;
		dmpIsTrusted = IS_TRUSTED_DMP(psw);
		CPU_CLR_DMP(&cpu->reg);
	}

	if (vector8 <= sccfg_size) {
		addr = scbp + (vector8 << 2U);
	}
	else {
		addr = scbp;
	}
	err = bus_get_data32(cpu->core_id, addr, (uint32*)&data);
	if (err != STD_E_OK) {
		printf("ERROR:SYSCALL pc=0x%x vector8=%u addr=0x%x\n", 
			cpu->reg.pc, vector8, addr);
		return -1;
	}
	if (backupDmp == TRUE) {
		if (dmpIsTrusted == TRUE) {
			CPU_CLR_DMP(&cpu->reg);
		}
		else {
			CPU_SET_DMP(&cpu->reg);
		}
	}
	pc = scbp + data;
	
	DBG_PRINT((DBG_EXEC_OP_BUF(), DBG_EXEC_OP_BUF_LEN(), "0x%x: SYSCALL vector8=%u addr=0x%x:0x%x\n", 
		cpu->reg.pc, vector8, addr, pc));

	cpu->reg.pc = pc;
	return 0;
}

int op_exec_switch(TargetCoreType *cpu)
{
	uint32 reg1 = cpu->decoded_code->type1.reg1;
	uint32 reg1_data;
	uint32 addr;
	sint32 tmp_pc;
	uint32 next_pc;
	sint16 data16;
	Std_ReturnType err;

	if (reg1 >= CPU_GREG_NUM) {
		return -1;
	}

	reg1_data = cpu->reg.r[reg1];

	addr = (cpu->reg.pc + 2U) + (reg1_data << 1U);
	/*
	 * Load-memory (adr, Half-word)
	 */
	err = bus_get_data16(cpu->core_id, addr, (uint16*)&data16);
	if (err != STD_E_OK) {
		printf("ERROR:SWITCH pc=0x%x reg1=%u(0x%x) addr=0x%x\n", cpu->reg.pc, reg1, reg1_data, addr);
		return -1;
	}
	/*
	 * (sign-extend (Load-memory (adr, Half-word) ))
	 */
	tmp_pc = (sint32)( data16 );
	/*
	 * (sign-extend (Load-memory (adr, Half-word) ) ) logically shift left by 1
	 */
	tmp_pc <<= 1U;

	/*
	 * (PC + 2) + (sign-extend (Load-memory (adr, Half-word) ) ) logically shift left by 1
	 */
	next_pc = (cpu->reg.pc + 2U) + ((uint32)tmp_pc);

	DBG_PRINT((DBG_EXEC_OP_BUF(), DBG_EXEC_OP_BUF_LEN(), "0x%x: SWITCH r%d(%d):%d\n", cpu->reg.pc, reg1, cpu->reg.r[reg1], next_pc));



	cpu->reg.pc = next_pc;
	return 0;
}

static bool has_permission_prepare(TargetCoreType *cpu, uint16 start_reg, uint16 ff)
{
	uint16 i;
	uint32 stack_write_off = (uint32)(cpu->reg.r[3]);	//sp:r3;
	uint32 stack_write_size = 0;
	uint32 program_read_off = cpu->reg.pc + 4U;;
	uint32 program_read_size = 0;
	bool permission;

	for (i = start_reg; i < 32; i++) {
		if (cpu->decoded_code->type13.list[i] == 0) {
			continue;
		}
		stack_write_size += 4U;
	}

	switch (ff) {
	case 0b00:
		program_read_size = 0U;
		break;
	case 0b01:
	case 0b10:
		program_read_size = 2U;
		break;
	case 0b11:
		program_read_size = 4U;
		break;
	default:
		program_read_size = 0U;
		break;
	}
	/*
	 * stack write permission
	 */
	permission = cpu_has_permission(cpu->core_id,
			GLOBAL_MEMORY,
			CpuMemoryAccess_WRITE,
			stack_write_off,
			stack_write_size);
	if (permission == FALSE) {
		return FALSE;
	}
	if (program_read_size == 0U) {
		return TRUE;
	}
	/*
	 * program exec permission
	 */
	permission = cpu_has_permission(cpu->core_id,
			READONLY_MEMORY,
			CpuMemoryAccess_EXEC,
			program_read_off,
			program_read_size);
	return permission;
}
/*
 * Format13
 */
int op_exec_prepare(TargetCoreType *cpu)
{
	uint16 subop = cpu->decoded_code->type13.gen & 0x0007;
	uint16 ff = cpu->decoded_code->type13.gen >> 3U;
	uint16 start_reg = 20U;
	uint16 i;
	uint32 addr;
	uint32 *addrp;
	uint32 *sp = (uint32*)&(cpu->reg.r[3]);	//sp:r3
	uint32 imm = ( cpu->decoded_code->type13.imm << 2U );
	Std_ReturnType err;

	if (has_permission_prepare(cpu, start_reg, ff) == FALSE) {
		return -1;
	}

	DBG_PRINT((DBG_EXEC_OP_BUF(), DBG_EXEC_OP_BUF_LEN(), "0x%x: PREPARE sp=0x%x ", cpu->reg.pc, *sp));
	for (i = start_reg; i < 32; i++) {
		if (cpu->decoded_code->type13.list[i] == 0) {
			continue;
		}
		DBG_PRINT((DBG_EXEC_OP_BUF(), DBG_EXEC_OP_BUF_LEN(), "r%u(0x%x) ", i, cpu->reg.r[i]));

		addr = (*sp) - 4U;
		err = bus_get_pointer(cpu->core_id, addr, (uint8**)&addrp);
		if (err != STD_E_OK) {
			printf("ERROR:PREPARE pc=0x%x sp=0x%x\n", cpu->reg.pc, *sp);
			return -1;
		}
		*addrp = cpu->reg.r[i];
		*sp = addr;
		cpu->real_elaps += 1U;
	}
	*sp = (*sp) - imm;

	DBG_PRINT((DBG_EXEC_OP_BUF(), DBG_EXEC_OP_BUF_LEN(), "imm5(%u) ", imm));

	if (subop == 1U) {
		cpu->reg.pc += 4;
		DBG_PRINT((DBG_EXEC_OP_BUF(), DBG_EXEC_OP_BUF_LEN(), ":sp=0x%x\n", *sp));
		return 0;
	}

	addr = cpu->reg.pc + 4U;

	err = bus_get_pointer(cpu->core_id, addr, (uint8**)&addrp);
	if (err != STD_E_OK) {
		printf("ERROR:PREPARE pc=0x%x sp=0x%x\n", cpu->reg.pc, *sp);
		return -1;
	}

	switch (ff) {
	case 0b00:
		cpu->reg.r[30] = *sp;
		cpu->reg.pc += 4;
		DBG_PRINT((DBG_EXEC_OP_BUF(), DBG_EXEC_OP_BUF_LEN(), "ep=0x%x\n", cpu->reg.r[30]));
		break;
	case 0b01:
		cpu->reg.r[30] = (sint32)(*((sint16*)addrp));
		cpu->reg.pc += 6;
		DBG_PRINT((DBG_EXEC_OP_BUF(), DBG_EXEC_OP_BUF_LEN(), "ep=0x%x\n", cpu->reg.r[30]));
		break;
	case 0b10:
		cpu->reg.r[30] = ((uint32)(*((uint16*)addrp))) << 16U;
		cpu->reg.pc += 6;
		DBG_PRINT((DBG_EXEC_OP_BUF(), DBG_EXEC_OP_BUF_LEN(), "ep=0x%x\n", cpu->reg.r[30]));
		break;
	case 0b11:
		cpu->reg.r[30] = (*((uint32*)addrp));
		cpu->reg.pc += 8;
		DBG_PRINT((DBG_EXEC_OP_BUF(), DBG_EXEC_OP_BUF_LEN(), "ep=0x%x\n", cpu->reg.r[30]));
		break;
	default:
		printf("ERROR:PREPARE pc=0x%x sp=0x%x\n", cpu->reg.pc, *sp);
		return -1;
	}
	return 0;
}


static bool has_permission_dispose(TargetCoreType *cpu, uint16 start_reg)
{
	uint16 i;
	uint32 stack_read_off = (uint32)(cpu->reg.r[3]);	//sp:r3;
	uint32 stack_read_size = 0;
	bool permission;

	for (i = start_reg; i < 32; i++) {
		if (cpu->decoded_code->type13.list[i] == 0) {
			continue;
		}
		stack_read_size += 4U;
	}

	/*
	 * stack read permission
	 */
	permission = cpu_has_permission(cpu->core_id,
			GLOBAL_MEMORY,
			CpuMemoryAccess_READ,
			stack_read_off,
			stack_read_size);

	return permission;
}

int op_exec_dispose(TargetCoreType *cpu)
{
	uint16 reg1 = cpu->decoded_code->type13.gen;
	uint16 start_reg = 20U;
	uint16 i;
	uint32 addr;
	uint32 *addrp;
	uint32 *sp = (uint32*)&(cpu->reg.r[3]);	//sp:r3
	uint32 imm = ( cpu->decoded_code->type13.imm << 2U );
	Std_ReturnType err;
	cpu->real_elaps = 0U;

	if (has_permission_dispose(cpu, start_reg) == FALSE) {
		return -1;
	}

	DBG_PRINT((DBG_EXEC_OP_BUF(), DBG_EXEC_OP_BUF_LEN(), "0x%x: DISPOSE imm=0x%x sp=0x%x ", cpu->reg.pc, imm, *sp));

	*sp = (*sp) + imm;
	for (i = 31; i >= start_reg; i--) {
		if (cpu->decoded_code->type13.list[i] == 0) {
			continue;
		}

		addr = (*sp);
		err = bus_get_pointer(cpu->core_id, addr, (uint8**)&addrp);
		if (err != STD_E_OK) {
			printf("ERROR:DISPOSE pc=0x%x sp=0x%x\n", cpu->reg.pc, *sp);
			return -1;
		}
		cpu->reg.r[i] = *addrp;
		DBG_PRINT((DBG_EXEC_OP_BUF(), DBG_EXEC_OP_BUF_LEN(), "r%u(0x%x) ", i, cpu->reg.r[i]));
		*sp = addr + 4;
		cpu->real_elaps += 1U;
	}

	if (reg1 != 0U) {
		cpu->real_elaps += 6U;
		DBG_PRINT((DBG_EXEC_OP_BUF(), DBG_EXEC_OP_BUF_LEN(), ":pc=r%u(0x%x) sp=0x%x\n", reg1, cpu->reg.r[reg1], cpu->reg.r[3]));
		cpu->reg.pc = cpu->reg.r[reg1];
	}
	else {
		cpu->real_elaps += 2U;
		DBG_PRINT((DBG_EXEC_OP_BUF(), DBG_EXEC_OP_BUF_LEN(), ":pc=r%u(0x%x) sp=0x%x\n", reg1, cpu->reg.pc, cpu->reg.r[3]));
		cpu->reg.pc += 4;
	}

	return 0;
}

/*
 * ［命令形式］ CAXI [reg1], reg2, reg3
 *
 * ［オペレーション］ adr ← GR[reg1]注
 * token ← Load-memory(adr, Word)
 * result ← GR[reg2] – token
 * If result == 0
 * then Store-memory(adr, GR[reg3],Word)
 * GR[reg3] ← token
 * else Store-memory(adr, token,Word)
 * GR[reg3] ← token
 * 注 GR[reg1]の下位 2 ビットは， 0 にマスクしadr とします。
 */
int op_exec_caxi(TargetCoreType *cpu)
{
	Std_ReturnType err;
	uint16 reg1 = cpu->decoded_code->type11.reg1;;
	uint16 reg2 = cpu->decoded_code->type11.reg2;
	uint16 reg3 = cpu->decoded_code->type11.reg3;
	sint16 token;
	sint16 result;
	uint16 put_data;

	uint32 reg1_addr = (cpu->reg.r[reg1] & 0xFFFFFFFC);
	uint32 reg2_data = cpu->reg.r[reg2];
	uint32 reg3_data = cpu->reg.r[reg3];

	/*
	 * Load-memory (adr, Half-word)
	 */
	err = bus_get_data16(cpu->core_id, reg1_addr, (uint16*)&token);
	if (err != STD_E_OK) {
		printf("ERROR:CAXI pc=0x%x reg1=%u reg1_addr=%d\n", cpu->reg.pc, reg1, reg1_addr);
		return -1;
	}

	result = reg2_data - token;
	if (result == 0) {
		put_data = (uint16)reg3_data;
	}
	else {
		put_data = (uint16)token;
	}
	err = bus_put_data16(cpu->core_id, reg1_addr, put_data);
	if (err != STD_E_OK) {
		return -1;
	}
	DBG_PRINT((DBG_EXEC_OP_BUF(), DBG_EXEC_OP_BUF_LEN(), "0x%x: CAXI r%d(%d),r%d(0x%x), r%d(0x%x):token=0x%x store=0x%x\n",
			cpu->reg.pc, reg1, cpu->reg.r[reg1], reg2, cpu->reg.r[reg2], reg3, cpu->reg.r[reg3], token, put_data));

	cpu->reg.r[reg3] = (sint32)((uint32)((uint16)token));

	op_chk_and_set_borrow(&cpu->reg, reg2_data, token);
	op_chk_and_set_overflow(&cpu->reg, reg2_data, -((sint64)token));
	op_chk_and_set_zero(&cpu->reg, result);
	op_chk_and_set_sign(&cpu->reg, result);


	cpu->reg.pc += 4;

	return 0;
}

