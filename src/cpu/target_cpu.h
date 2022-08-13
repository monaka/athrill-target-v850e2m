#ifndef _CPU_REGISTER_H_
#define _CPU_REGISTER_H_

#include "cpu_dec/op_dec.h"
#include "std_types.h"
#include "object_container.h"

#define ATHRILL_TARGET_ARCH	"V850e2m"
/*
 * version: X.Y.Z
 *  X: generation
 *  Y: function
 *  Z: bug fix, small changes
 */
#define ATHRILL_TARGET_VERSION "1.0.2"


#define CPU_GREG_NUM			(32U)
#define CPU_SYSREG_NUM			(28U)
#define CPU_COMMON_SYSREG_NUM	(4U)
#define CPU_SYSBNK_NUM			(3U)

#define CPU_SYSREG_EIWR			(0U)
#define CPU_SYSREG_FEWR			(1U)
#define CPU_SYSREG_DBWR			(2U)
#define CPU_SYSREG_BSEL			(3U)
typedef enum {
	SYS_GRP_CPU = 0,
	SYS_GRP_PROSESSOR,
	SYS_GRP_PMU,
	SYS_GRP_FPU,
	SYS_GRP_USER,
	SYS_GRP_NUM,
} SysGrpuType;

typedef enum {
	SYS_GRP_CPU_BNK_0 = 0,
	SYS_GRP_CPU_BNK_1,
	SYS_GRP_CPU_BNK_2,
	SYS_GRP_CPU_BNK_NUM,
} SysGrpCpuBnkType;

typedef enum {
	SYS_REG_EIPC = 0,
	SYS_REG_EIPSW,
	SYS_REG_FEPC,
	SYS_REG_FEPSW,
	SYS_REG_ECR,
	SYS_REG_PSW,
	/* RESERVE 6-10 */
	SYS_REG_RESERVE_6,
	SYS_REG_RESERVE_7,
	SYS_REG_RESERVE_8,
	SYS_REG_RESERVE_9,
	SYS_REG_RESERVE_10,
	SYS_REG_SCCFG,
	SYS_REG_SCBP,
	SYS_REG_EIIC,
	SYS_REG_FEIC,
	SYS_REG_DBIC,
	SYS_REG_CTPC,
	SYS_REG_CTPSW,
	SYS_REG_DBPC,
	SYS_REG_DBPSW,
	SYS_REG_CTBP,
	SYS_REG_DIR,
	/* 22-27 デバッグ機能レジスタ － － － */
} SysGrpCpuBnkBaseRegisterType;

typedef enum {
	SYS_REG_SW_CTL = 0,
	SYS_REG_SW_CFG,
	SYS_REG_SW_RESERVE_2,
	SYS_REG_SW_BASE,
	SYS_REG_SW_RESERVE_4,
	SYS_REG_SW_RESERVE_5,
	SYS_REG_SW_RESERVE_6,
	SYS_REG_SW_RESERVE_7,
	SYS_REG_SW_RESERVE_8,
	SYS_REG_SW_RESERVE_9,
	SYS_REG_SW_RESERVE_10,
	SYS_REG_SW_RESERVE_11,
	SYS_REG_SW_RESERVE_12,
	SYS_REG_SW_RESERVE_13,
	SYS_REG_SW_RESERVE_14,
	SYS_REG_SW_RESERVE_15,
	SYS_REG_SW_RESERVE_16,
	SYS_REG_SW_RESERVE_17,
	SYS_REG_SW_RESERVE_18,
	SYS_REG_SW_RESERVE_19,
	SYS_REG_SW_RESERVE_20,
	SYS_REG_SW_RESERVE_21,
	SYS_REG_SW_RESERVE_22,
	SYS_REG_SW_RESERVE_23,
	SYS_REG_SW_RESERVE_24,
	SYS_REG_SW_RESERVE_25,
	SYS_REG_SW_RESERVE_26,
	SYS_REG_SW_RESERVE_27,
} SysGrpCpuBnk0ExceptionRegisterType;

typedef enum {
	SYS_REG_EH_RESERVE0 = 0,
	SYS_REG_EH_CFG,
	SYS_REG_EH_RESET,
	SYS_REG_EH_BASE,
	SYS_REG_EH_RESERVE_4,
	SYS_REG_EH_RESERVE_5,
	SYS_REG_EH_RESERVE_6,
	SYS_REG_EH_RESERVE_7,
	SYS_REG_EH_RESERVE_8,
	SYS_REG_EH_RESERVE_9,
	SYS_REG_EH_RESERVE_10,
	SYS_REG_EH_RESERVE_11,
	SYS_REG_EH_RESERVE_12,
	SYS_REG_EH_RESERVE_13,
	SYS_REG_EH_RESERVE_14,
	SYS_REG_EH_RESERVE_15,
	SYS_REG_EH_RESERVE_16,
	SYS_REG_EH_RESERVE_17,
	SYS_REG_EH_RESERVE_18,
	SYS_REG_EH_RESERVE_19,
	SYS_REG_EH_RESERVE_20,
	SYS_REG_EH_RESERVE_21,
	SYS_REG_EH_RESERVE_22,
	SYS_REG_EH_RESERVE_23,
	SYS_REG_EH_RESERVE_24,
	SYS_REG_EH_RESERVE_25,
	SYS_REG_EH_RESERVE_26,
	SYS_REG_EH_RESERVE_27,
} SysGrpCpuBnk1ExceptionRegisterType;


typedef enum {
	SYS_REG_MPV_VSECR = 0,
	SYS_REG_MPV_VSTID,
	SYS_REG_MPV_VSADR,
	SYS_REG_MPV_RESERVE_3,
	SYS_REG_MPV_VMECR,
	SYS_REG_MPV_VMTID,
	SYS_REG_MPV_VMADR,
	SYS_REG_MPV_RESERVE_7,
	SYS_REG_MPV_RESERVE_8,
	SYS_REG_MPV_RESERVE_9,
	SYS_REG_MPV_RESERVE_10,
	SYS_REG_MPV_RESERVE_11,
	SYS_REG_MPV_RESERVE_12,
	SYS_REG_MPV_RESERVE_13,
	SYS_REG_MPV_RESERVE_14,
	SYS_REG_MPV_RESERVE_15,
	SYS_REG_MPV_RESERVE_16,
	SYS_REG_MPV_RESERVE_17,
	SYS_REG_MPV_RESERVE_18,
	SYS_REG_MPV_RESERVE_19,
	SYS_REG_MPV_RESERVE_20,
	SYS_REG_MPV_RESERVE_21,
	SYS_REG_MPV_RESERVE_22,
	SYS_REG_MPV_RESERVE_23,
	SYS_REG_MPV_MCA,
	SYS_REG_MPV_MCS,
	SYS_REG_MPV_MCC,
	SYS_REG_MPV_MCR,
} SysGrpProcessorProtectErrorBnkRegisterType;


typedef enum {
	SYS_REG_MPU_MPM = 0,
	SYS_REG_MPU_MPC,
	SYS_REG_MPU_TID,
	SYS_REG_MPU_RESERVE_3,
	SYS_REG_MPU_RESERVE_4,
	SYS_REG_MPU_RESERVE_5,
	SYS_REG_MPU_IPA0L,
	SYS_REG_MPU_IPA0U,
	SYS_REG_MPU_IPA1L,
	SYS_REG_MPU_IPA1U,
	SYS_REG_MPU_IPA2L,
	SYS_REG_MPU_IPA2U,
	SYS_REG_MPU_IPA3L,
	SYS_REG_MPU_IPA3U,
	SYS_REG_MPU_IPA4L,
	SYS_REG_MPU_IPA4U,
	SYS_REG_MPU_DPA0L,
	SYS_REG_MPU_DPA0U,
	SYS_REG_MPU_DPA1L,
	SYS_REG_MPU_DPA1U,
	SYS_REG_MPU_DPA2L,
	SYS_REG_MPU_DPA2U,
	SYS_REG_MPU_DPA3L,
	SYS_REG_MPU_DPA3U,
	SYS_REG_MPU_DPA4L,
	SYS_REG_MPU_DPA4U,
	SYS_REG_MPU_DPA5L,
	SYS_REG_MPU_DPA5U,
} SysGrpProcessorProtectSettingsBnkRegisterType;


typedef enum {
	SYS_REG_MPM = 0,
	SYS_REG_MPC,
	SYS_REG_TID,
	SYS_REG_VMECR,
	SYS_REG_VMTID,
	SYS_REG_VMADR,
	SYS_REG_IPA0L,
	SYS_REG_IPA0U,
	SYS_REG_IPA1L,
	SYS_REG_IPA1U,
	SYS_REG_IPA2L,
	SYS_REG_IPA2U,
	SYS_REG_IPA3L,
	SYS_REG_IPA3U,
	SYS_REG_IPA4L,
	SYS_REG_IPA4U,
	SYS_REG_DPA0L,
	SYS_REG_DPA0U,
	SYS_REG_DPA1L,
	SYS_REG_DPA1U,
	SYS_REG_DPA2L,
	SYS_REG_DPA2U,
	SYS_REG_DPA3L,
	SYS_REG_DPA3U,
	SYS_REG_DPA4L,
	SYS_REG_DPA4U,
	SYS_REG_DPA5L,
	SYS_REG_DPA5U,
} SysGrpProcessorProtectPagingBnkRegisterType;

typedef enum {
	SYS_REG_FPU_RESERVE_0 = 0,
	SYS_REG_FPU_RESERVE_1,
	SYS_REG_FPU_RESERVE_2,
	SYS_REG_FPU_RESERVE_3,
	SYS_REG_FPU_RESERVE_4,
	SYS_REG_FPU_RESERVE_5,
	SYS_REG_FPSR,
	SYS_REG_FPEPC,
	SYS_REG_FPST,
	SYS_REG_FPCC,
	SYS_REG_FPCFG,
	SYS_REG_FPEC,
} SysGrpFpuBnkRegisterType;

#define CPU_REG_UINT_MAX	0xFFFFFFFFULL
#define CPU_REG_PLUS_MAX	2147483647LL
#define CPU_REG_MINUS_MAX	-2147483648LL

#define CPU_REG_SP		(3)
#define CPU_REG_EP		(30)
#define CPU_REG_LP		(31)


typedef struct {
	uint32 r[CPU_SYSREG_NUM];
} CpuSystemRegisterDataType;

typedef struct {
	uint32						current_grp;
	uint32						current_bnk;
	CpuSystemRegisterDataType	grp[SYS_GRP_NUM][CPU_SYSBNK_NUM];
	uint32						sysreg[CPU_COMMON_SYSREG_NUM];
} CpuSystemRegisterType;

typedef struct {
	uint32 pc;
	sint32 r[CPU_GREG_NUM];
	CpuSystemRegisterType	sys;
} CpuRegisterType;

static inline uint32 *cpu_get_sysreg(CpuSystemRegisterType *sys, uint32 inx) {
	if (inx < CPU_SYSREG_NUM) {
		return &sys->grp[sys->current_grp][sys->current_bnk].r[inx];
	}
	else {
		return &sys->sysreg[inx - CPU_SYSREG_NUM];
	}
}
static inline uint32 *cpu_get_sysreg_fpu(CpuSystemRegisterType *sys, uint32 regid) {
	return &sys->grp[SYS_GRP_FPU][sys->current_bnk].r[regid];
}

static inline uint32 cpu_get_psw(CpuSystemRegisterType *sys) {
	return sys->grp[SYS_GRP_CPU][SYS_GRP_CPU_BNK_0].r[SYS_REG_PSW];
}

static inline uint32 *cpu_get_mpu_illegal_factor_sysreg(CpuSystemRegisterType *sys) {
	return sys->grp[SYS_GRP_PROSESSOR][SYS_GRP_CPU_BNK_0].r;
}

static inline uint32 *cpu_get_mpu_settign_sysreg(CpuSystemRegisterType *sys) {
	return sys->grp[SYS_GRP_PROSESSOR][SYS_GRP_CPU_BNK_1].r;
}

static inline CpuSystemRegisterDataType *sys_get_cpu_base(CpuRegisterType *reg) {
	return &reg->sys.grp[SYS_GRP_CPU][SYS_GRP_CPU_BNK_0];
}

typedef enum {
	CpuExceptionError_None = 0,
	CpuExceptionError_MIP,
	CpuExceptionError_MDP,
	CpuExceptionError_PPI,
} CpuExceptionErrorCodeType;

#define TARGET_CORE_MPU_CONFIG_EXEC_MAXNUM		5U
#define TARGET_CORE_MPU_CONFIG_DATA_MAXNUM		6U

typedef struct {
	bool								enable_protection;
	bool								is_mask_method;
	/*
	 * is_mask_method == FALSE
	 *  => au: upper address
	 *  => al: lower address
	 *
	 * is_mask_method == TRUE
	 *  => au: mask
	 *  => al: base address
	 *
	 *  マスク値を指定する場合は,必ず下位側から 1 を連続させた値を設定してください
	 *  (000050FFHなどのように,1/0が交互に配置された場合の動作は保証しません)。
	 */
	uint32								au;
	uint32								al;
} TargetCoreMpuConfigType;

typedef struct {
	TargetCoreMpuConfigType				common;
	bool								enable_read;
	bool								enable_exec;
} TargetCoreMpuExecConfigType;

typedef struct {
	TargetCoreMpuConfigType				common;
	bool								enable_read;
	bool								enable_write;
} TargetCoreMpuDataConfigType;

typedef struct {
	ObjectContainerType					*region_permissions;
} TargetCoreMpuConfigContainerType;

typedef enum {
	CpuMemoryAccess_NONE = 0,
	CpuMemoryAccess_READ,
	CpuMemoryAccess_WRITE,
	CpuMemoryAccess_EXEC,
} CpuMemoryAccessType;

typedef struct {
	CpuExceptionErrorCodeType			exception_error_code;
	uint32								error_address;
	CpuMemoryAccessType					error_access;
	TargetCoreMpuConfigContainerType	data_configs;
	TargetCoreMpuConfigContainerType	exec_configs;
} TargetCoreMpuType;

typedef struct {
	uint32						real_elaps;
	CoreIdType					core_id;
	CpuRegisterType 			reg;
	bool						is_halt;
	uint16 						*current_code;
	OpDecodedCodeType			*decoded_code;
	TargetCoreMpuType			mpu;
} TargetCoreType;

static inline uint32 cpu_get_pc(const TargetCoreType *core)
{
	return core->reg.pc;
}
static inline uint32 cpu_get_sp(const TargetCoreType *core)
{
	return core->reg.r[3];
}


extern void fpu_sync_sysreg(TargetCoreType *cpu, uint32 regid, uint32 selid);

#define SYS_ISSET_BIT(data32p, bitpos)                  ( (*(data32p)) & (1U << (bitpos)) )
#define SYS_SET_BIT(data32p, bitpos)    \
do {    \
        *(data32p) |= (1U << (bitpos)); \
} while (0)
#define SYS_CLR_BIT(data32p, bitpos)    \
do {    \
        *(data32p) &= ~(1U << (bitpos));        \
} while (0)

#define SYS_GET_DATA2(data32p, bitpos)                  ( ( (uint8)((*(data32p)) >> (bitpos)) ) & 0x0003 )
#define SYS_SET_DATA2(data32p, bitpos, data8)   \
do { \
        uint32 _org_data = *(data32p);  \
        uint32 _new_data = _org_data & ~((uint32)(0x0003) << (bitpos)); \
        _new_data |= ( ((uint32)(data8)) << (bitpos)); \
        *(data32p) = _new_data;  \
} while(0)

#define SYS_GET_DATA5(data32p, bitpos)                  ( ( (uint8)((*(data32p)) >> (bitpos)) ) & 0x001F )
#define SYS_SET_DATA5(data32p, bitpos, data8)   \
do { \
        uint32 _org_data = *(data32p);  \
        uint32 _new_data = _org_data & ~((uint32)(0x001F) << (bitpos)); \
        _new_data |= ( ((uint32)(data8)) << (bitpos)); \
        *(data32p) = _new_data;  \
} while(0)

#define SYS_GET_DATA6(data32p, bitpos)                  ( ( (uint8)((*(data32p)) >> (bitpos)) ) & 0x003F )
#define SYS_SET_DATA6(data32p, bitpos, data8)   \
do { \
        uint32 _org_data = *(data32p);  \
        uint32 _new_data = _org_data & ~((uint32)(0x003F) << (bitpos)); \
        _new_data |= ( ((uint32)(data8)) << (bitpos)); \
        *(data32p) = _new_data;  \
} while(0)

#define SYS_GET_DATA8(data32p, bitpos)                  ( (uint8)((*(data32p)) >> (bitpos)) )
#define SYS_SET_DATA8(data32p, bitpos, data8)   \
do { \
        uint32 _org_data = *(data32p);  \
        uint32 _new_data = _org_data & ~((uint32)(0xFFFF) << (bitpos)); \
        _new_data |= ( ((uint32)(data8)) << (bitpos)); \
        *(data32p) = _new_data;  \
} while(0)



#endif /* _CPU_REGISTER_H_ */
