/*
	struct.h

	Author: MiDi$
*/

#ifndef _STRUCT_H
#define _STRUCT_H

#include "const.h"

typedef struct
{
	char chQuikName[STRUCT_COL_SIZE + 1];
	char chRoboexName[STRUCT_COL_SIZE + 1];
	char *chValue;
} ROBOEX_QUIK_STRUCT;


typedef struct
{
	ROBOEX_QUIK_STRUCT **roboex_quik_st;
	WORD ctx_idx;		// ������� ������ �� ���������
	WORD ctx_size;		// �����������
} TRN_OUT_CTX;

typedef struct
{
	ROBOEX_QUIK_STRUCT **roboex_quik_st;
	WORD ctx_idx;		// ������� ������ �� ���������
	WORD ctx_size;		// �����������
} CVAL_OUT_CTX;


#endif