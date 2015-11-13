/*
	ext_gvalue.h

	Author: MiDi$
*/

#ifndef EXT_GVALUE_H
#define EXT_GVALUE_H

#ifdef  __cplusplus
extern "C" {
#endif

//������
	extern DWORD g_dwThreadsCntTrnMax;	//����. ����� ������� -- TRN
	extern LONG g_lThreadCount;			//������� ����� �������
	extern BOOL g_bThreadExec;			//�������� ����������� ������� �������
	extern char g_chMultiThread;		//����� ���������������: Y - ���. N - ����.

//�������������
	extern HANDLE g_hEventThreads;		//������� ��������� ���������� �������

//ORA ���������
	extern DWORD g_dwOraConnTimeOut;		//����� �������� � ������������� (1 ���. = 1000 �����������)
	extern DWORD g_dwOraCntTranCommit;		//���������� ������� � ����������
	extern char g_chDdeImpTrn[MAX_PATH];	//��� ������� TRN
	extern char g_chDdeImpCval[MAX_PATH];	//��� ������� CVAL
	extern DWORD g_dwOraSessMax;			//������������ ����� ������ � ���� ����������
	extern char g_chOraEnable;				//����� �������������� � Oracle: Y - �� N - ���

//���������
	extern DWORD g_dwCtxCntRows;			//���������� ������� � ���������
	extern char g_chDdeImpTrnTxt[MAX_PATH];	//����� � TXT - TRN
	char g_chDebugInfo;						//����� ������ ���������� ���������� � ���: : Y - �� N - ���

//Plugins
	//--//
	extern char g_chDdeImpTrnPlugin[MAX_PATH];	//��� ������� TRN
	extern char g_chPluginEnableTrn;			//����� �������� TRN ����� ������: Y - ���. N - ����.
	extern char g_chLibNameTrn[MAX_PATH];		//��� ������������� ������
	extern char g_chFuncNameTrn[MAX_PATH];		//��� ������� � ������������ ������
	//--//

#ifdef  __cplusplus
}
#endif

#endif