/*
	gvalue.h

	Author: MiDi$
*/

#ifndef _GVALUE_H
#define _GVALUE_H

#include "const.h"

#ifdef  __cplusplus
extern "C" {
#endif

//������
	DWORD g_dwThreadsCntTrnMax = 0; //����. ����� ������� -- TRN
	LONG g_lThreadCount = 0;		//������� ����� �������
	BOOL g_bThreadExec = TRUE;		//�������� ����������� ������� �������
	char g_chMultiThread = 'Y';		//����� ���������������: Y - ���. N - ����.

//�������������
	HANDLE g_hEventThreads = NULL; //������� ��������� ���������� �������
	//HANDLE g_hEventPool[MAX_THREADS] = {0};

//ORA ���������
	DWORD g_dwOraConnTimeOut = 0;			//����� �������� � ������������� (1 ���. = 1000 �����������)
	DWORD g_dwOraCntTranCommit = 0;			//���������� ������� � ����������
	char g_chDdeImpTrn[MAX_PATH] = {0};		//��� ������� TRN
	char g_chDdeImpCval[MAX_PATH] = {0};	//��� ������� CVAL
	DWORD g_dwOraSessMax = 0;				//������������ ����� ������ � ���� ����������
	char g_chOraEnable = 'N';				//����� �������������� � Oracle: Y - �� N - ���

//���������
	DWORD g_dwCtxCntRows = 0;				//���������� ������� � ���������
	char g_chDdeImpTrnTxt[MAX_PATH] = {0};	//����� � TXT - TRN
	char g_chDebugInfo = 'N';				//����� ������ ���������� ���������� � ���: : Y - �� N - ���

//Plugins
	//--//
	char g_chDdeImpTrnPlugin[MAX_PATH] = {0};	//��� ������� TRN
	char g_chPluginEnableTrn = 'N';				//����� �������� TRN ����� ������: Y - ���. N - ����.
	char g_chLibNameTrn[MAX_PATH] = {0};		//��� ������������� ������
	char g_chFuncNameTrn[MAX_PATH] = {0};		//��� ������� � ������������ ������
	//--//

#ifdef  __cplusplus
}
#endif

#endif