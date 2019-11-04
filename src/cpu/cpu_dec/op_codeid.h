#ifndef _OP_CODEID_H_
#define _OP_CODEID_H_

typedef enum {
	OpCodeId_ABSF_S_F = 0,
	OpCodeId_ADD_1,
	OpCodeId_ADD_2,
	OpCodeId_ADDF_D_F,
	OpCodeId_ADDF_S_F,
	OpCodeId_ADDI_6,
	OpCodeId_ADF_11,
	OpCodeId_AND_1,
	OpCodeId_ANDI_6,
	OpCodeId_BSH_12,
	OpCodeId_BSW_12,
	OpCodeId_Bcond_3,
	OpCodeId_CALLT_2,
	OpCodeId_CAXI_11,
	OpCodeId_CEILF_SL_F,
	OpCodeId_CEILF_SUL_F,
	OpCodeId_CEILF_SUW_F,
	OpCodeId_CEILF_SW_F,
	OpCodeId_CLR1_8,
	OpCodeId_CLR1_9,
	OpCodeId_CMOV_12,
	OpCodeId_CMOV_11,
	OpCodeId_CMOVF_S_F,
	OpCodeId_CMP_1,
	OpCodeId_CMP_2,
	OpCodeId_CMPF_S_F,
	OpCodeId_CTRET_10,
	OpCodeId_CVTF_DS_F,
	OpCodeId_CVTF_LS_F,
	OpCodeId_CVTF_SD_F,
	OpCodeId_CVTF_SL_F,
	OpCodeId_CVTF_SUL_F,
	OpCodeId_CVTF_SUW_F,
	OpCodeId_CVTF_SW_F,
	OpCodeId_CVTF_ULS_F,
	OpCodeId_CVTF_UWS_F,
	OpCodeId_CVTF_WS_F,
	OpCodeId_DI_10,
	OpCodeId_DISPOSE_13,
	OpCodeId_DIV_11,
	OpCodeId_DIVF_S_F,
	OpCodeId_DIVH_1,
	OpCodeId_DIVH_11,
	OpCodeId_DIVHU_11,
	OpCodeId_DIVQ_11,
	OpCodeId_DIVQU_11,
	OpCodeId_DIVU_11,
	OpCodeId_EI_10,
	OpCodeId_EIRET_10,
	OpCodeId_FERET_10,
	OpCodeId_FETRAP_1,
	OpCodeId_FLOORF_SL_F,
	OpCodeId_FLOORF_SUL_F,
	OpCodeId_FLOORF_SUW_F,
	OpCodeId_FLOORF_SW_F,
	OpCodeId_HALT_10,
	OpCodeId_HSH_12,
	OpCodeId_HSW_12,
	OpCodeId_JARL_6,
	OpCodeId_JARL_5,
	OpCodeId_JMP_1,
	OpCodeId_JMP_6,
	OpCodeId_JR_6,
	OpCodeId_JR_5,
	OpCodeId_LDSR_9,
	OpCodeId_LD_B_7,
	OpCodeId_LD_B_14,
	OpCodeId_LD_BU_7,
	OpCodeId_LD_BU_14,
	OpCodeId_LD_H_7,
	OpCodeId_LD_H_14,
	OpCodeId_LD_HU_14,
	OpCodeId_LD_HU_7,
	OpCodeId_LD_W_7,
	OpCodeId_LD_W_14,
	OpCodeId_MAC_11,
	OpCodeId_MACU_11,
	OpCodeId_MAXF_S_F,
	OpCodeId_MINF_S_F,
	OpCodeId_MOV_1,
	OpCodeId_MOV_2,
	OpCodeId_MOV_6,
	OpCodeId_MOVEA_6,
	OpCodeId_MOVHI_6,
	OpCodeId_MUL_11,
	OpCodeId_MUL_12,
	OpCodeId_MULF_D_F,
	OpCodeId_MULF_S_F,
	OpCodeId_MULH_1,
	OpCodeId_MULH_2,
	OpCodeId_MULHI_6,
	OpCodeId_MULU_11,
	OpCodeId_MULU_12,
	OpCodeId_NEGF_S_F,
	OpCodeId_NOP_1,
	OpCodeId_NOT_1,
	OpCodeId_NOT1_8,
	OpCodeId_NOT1_9,
	OpCodeId_OR_1,
	OpCodeId_ORI_6,
	OpCodeId_PREPARE_13,
	OpCodeId_RECIPF_S_F,
	OpCodeId_RETI_10,
	OpCodeId_RIE_1,
	OpCodeId_RIE_10,
	OpCodeId_RSQRTF_S_F,
	OpCodeId_SAR_2,
	OpCodeId_SAR_9,
	OpCodeId_SAR_11,
	OpCodeId_SASF_9,
	OpCodeId_SATADD_1,
	OpCodeId_SATADD_2,
	OpCodeId_SATADD_11,
	OpCodeId_SATSUB_1,
	OpCodeId_SATSUB_11,
	OpCodeId_SATSUBI_6,
	OpCodeId_SATSUBR_1,
	OpCodeId_SBF_11,
	OpCodeId_SCH0L_9,
	OpCodeId_SCH0R_9,
	OpCodeId_SCH1L_9,
	OpCodeId_SCH1R_9,
	OpCodeId_SET1_8,
	OpCodeId_SET1_9,
	OpCodeId_SETF_9,
	OpCodeId_SHL_2,
	OpCodeId_SHL_9,
	OpCodeId_SHL_11,
	OpCodeId_SHR_2,
	OpCodeId_SHR_9,
	OpCodeId_SHR_11,
	OpCodeId_SLD_B_4,
	OpCodeId_SLD_BU_4,
	OpCodeId_SLD_H_4,
	OpCodeId_SLD_HU_4,
	OpCodeId_SLD_W_4,
	OpCodeId_SQRTF_S_F,
	OpCodeId_SST_B_4,
	OpCodeId_SST_H_4,
	OpCodeId_SST_W_4,
	OpCodeId_STSR_9,
	OpCodeId_ST_B_7,
	OpCodeId_ST_B_14,
	OpCodeId_ST_H_7,
	OpCodeId_ST_H_14,
	OpCodeId_ST_W_7,
	OpCodeId_ST_W_14,
	OpCodeId_SUB_1,
	OpCodeId_SUBF_S_F,
	OpCodeId_SUBR_1,
	OpCodeId_SWITCH_1,
	OpCodeId_SXB_1,
	OpCodeId_SXH_1,
	OpCodeId_SYNCE_1,
	OpCodeId_SYNCM_1,
	OpCodeId_SYNCP_1,
	OpCodeId_SYSCALL_10,
	OpCodeId_TRAP_10,
	OpCodeId_TRFSR_F,
	OpCodeId_TRNCF_SL_F,
	OpCodeId_TRNCF_SUL_F,
	OpCodeId_TRNCF_SUW_F,
	OpCodeId_TRNCF_SW_F,
	OpCodeId_TST_1,
	OpCodeId_TST1_8,
	OpCodeId_TST1_9,
	OpCodeId_XOR_1,
	OpCodeId_XORI_6,
	OpCodeId_ZXB_1,
	OpCodeId_ZXH_1,
	OpCodeId_Num,
} OpCodeId;

#endif /* _OP_CODEID_H_ */
