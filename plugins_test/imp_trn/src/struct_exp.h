#ifndef _STRUCT_EXP_H
#define _STRUCT_EXP_H

	#define RET_OK		      0				//Все ОК
	#define ERR_MALLOC	      1111111100	//Ошибка выделения памяти
	#define ERR_ARGS	      1111111101	//Ошибка аргумента функции

	#define STRUCT_COL_SIZE 24
	#define DDE_TRN_SIZE 19

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

#endif

