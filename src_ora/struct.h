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
	WORD ctx_idx;		// Текущий индекс по контексту
	WORD ctx_size;		// Размерность
} TRN_OUT_CTX;

typedef struct
{
	ROBOEX_QUIK_STRUCT **roboex_quik_st;
	WORD ctx_idx;		// Текущий индекс по контексту
	WORD ctx_size;		// Размерность
} CVAL_OUT_CTX;


#endif