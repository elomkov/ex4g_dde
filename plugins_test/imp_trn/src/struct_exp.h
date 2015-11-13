#ifndef _STRUCT_EXP_H
#define _STRUCT_EXP_H

	#define RET_OK		      0				//��� ��
	#define ERR_MALLOC	      1111111100	//������ ��������� ������
	#define ERR_ARGS	      1111111101	//������ ��������� �������

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
		WORD ctx_idx;		// ������� ������ �� ���������
		WORD ctx_size;		// �����������
	} TRN_OUT_CTX;

#endif

