/*
	proto.h

	Author: MiDi$
*/

#ifndef _PROTO_H
#define _PROTO_H

#include "struct.h"

#ifdef  __cplusplus
extern "C" {
#endif

//DDE
	UINT Init_Dde();
	void UnInit_Dde();
	DWORD WriteLog_DDE(char *chStr, int iErrCode);
	DWORD WINAPI ParseXLTable_Thread(LPVOID dde_poke_thread);

//Объекты
	DWORD GetOraObjName(char *chStr, char **ch_OraObjName);
	DWORD GetOraObjName2(char *chStr, char **ch_OraObjName);
	DWORD IsObjAndSizeOk(WORD w_ObjType, WORD wColNum);
	DWORD IsObjOk(char *chOraObj, WORD *wStatus);

//Утилиты
	DWORD WriteLog(LPCTSTR lp_FileName, LPCVOID lp_Buffer, DWORD dw_Len);
	void Mem_Free(void **hMem);
	void ReplaceSDecimal(char *chStr);
	DWORD WINAPI MakeCmd(LPVOID lpBuf);
	DWORD CreateStealthWin(HWND *h_Wnd);
	DWORD InitIniValues();
	void StopNewThreadExec();

//ORA - wrapper
	DWORD InitOraConnect(char *chUserName, char *chPasswd, char *chInstance, DWORD dw_OraSessMax);
	void CloseOraConnect();
	DWORD GetDbSessionFromPool(WORD *w_idx, DWORD dw_OraConnTimeOut);
	void RemoveDbSessionFromPool(WORD w_idx);
	DWORD Imp_Cval(CVAL_OUT_CTX *cval_out_ctx, WORD w_idx, char *ch_DdeImpCval);
	DWORD Imp_Trn(TRN_OUT_CTX *trn_out_ctx, WORD w_idx, char *ch_DdeImpTrn);
	DWORD Init_Ora_Lib();
	void Free_Ora_Lib();

//Контекст
	DWORD Init_Ctx(void **out_ctx, WORD w_Size, WORD w_ObjType);
	void Ctx_Free(void *out_ctx, WORD w_ObjType);
	DWORD Fill_Ctx(void **out_ctx, char *ch_Data, WORD w_ctx_num, WORD w_ObjType);
	WORD GetNumNext(WORD wObjType, WORD wNum);
	void SetNextCtxIdx(void **out_ctx, WORD w_ObjType);

//Импорт
	DWORD Imp_Ora(void *ctx, WORD w_ObjType);
	void BeginTransaction(WORD w_idx);
	void CommitTransaction(WORD w_idx);
	DWORD Init_Imp(void **pImpStruct, WORD w_ObjType);
	void Release_Imp(void *pImpStruct, WORD w_ObjType);
	void Begin_Imp(void *pImpStruct, WORD w_ObjType);
	void Commit_Imp(void *pImpStruct, WORD w_ObjType);
	DWORD Imp_Data(void *ctx, void *pImpStruct, WORD w_ObjType);

//Критические секции
	void Init_CrSecDDE();
	void UnInit_CrSecDDE();
	void Init_CrSecImp();
	void UnInit_CrSecImp();
	void Init_CrPlugins();
	void UnInit_CrPlugins();
	void Init_CrDbgInfo();
	void UnInit_CrDbgInfo();
	void Init_CrSec();
	void UnInit_CrSec();

//Плагины
	DWORD Init_Plugins();
	void Free_Plugins();
	DWORD Imp_Plugin(void *ctx, void *pImpStruct, WORD w_ObjType, void *pRes1);

//Отладка
	void PrintCallStackAtomic();
	LONG WINAPI __UnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo);
	int test_crash();

#ifdef  __cplusplus
}
#endif

#endif