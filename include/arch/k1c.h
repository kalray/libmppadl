#ifndef K1_MPPADL_ARCH_K1C_H
#define K1_MPPADL_ARCH_K1C_H

/* FIXME: This should be derived from MDS
   See T5741 */

enum MPPA_DL_RELOCATIONS {
	R_K1_32 = 2,
	R_K1_GLOB_DAT = 18,
	R_K1_JMP_SLOT = 24,
	R_K1_RELATIVE = 25
};

#endif
