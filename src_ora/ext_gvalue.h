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
	extern DWORD g_dwOraConnTimeOut;	//����� �������� � ������������� (1 ���. = 1000 �����������)
	extern DWORD g_dwOraCntTranCommit;	//���������� ������� � ����������
	extern char g_chDdeImpTrn[256];		//��� ������� TRN
	extern char g_chDdeImpCval[256];	//��� ������� CVAL
	extern DWORD g_dwOraSessMax;		//������������ ����� ������ � ���� ����������
	extern char g_chOraEnable;			//����� �������������� � Oracle: Y - �� N - ���

//���������
	extern DWORD g_dwCtxCntRows;		//���������� ������� � ���������
	extern char g_chDdeImpTrnTxt[256];

//Plugins
	//--//
	extern char g_chDdeImpTrnPlugin[256];	//��� ������� TRN
	extern char g_chPluginEnableTrn;		//����� �������� TRN ����� ������: Y - ���. N - ����.
	extern char g_chLibNameTrn[256];		//��� ������������� ������
	extern char g_chFuncNameTrn[256];		//��� ������� � ������������ ������
	//--//

#ifdef  __cplusplus
}
#endif

#endif