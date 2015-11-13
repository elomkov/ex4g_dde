/*
    Универсальный шлюз импорта
	imp.c

	Author: MiDi$
*/

//#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <windows.h>
#include "const.h"
#include "proto.h"
#include "struct.h"
#include "ext_gvalue.h"

CRITICAL_SECTION CrSecTxtTrn;

//
//Инициализация критических секций
//
void Init_CrSecImp()
{
	InitializeCriticalSection(&CrSecTxtTrn);
}

//
//Удаление критических секций
//
void UnInit_CrSecImp()
{
	DeleteCriticalSection(&CrSecTxtTrn);
}

//
//Инициализация контекста импорта
//
DWORD Init_ImpCtx(IMP_CTX **imp_ctx)
{
	IMP_CTX *impctx = NULL;

	if (*imp_ctx) {
		return ERR_CTX_EXISTS;
	}

	impctx = (IMP_CTX *) malloc (sizeof(IMP_CTX));
	if (impctx == NULL) {
		return ERR_MALLOC;
	}
	memset(impctx, 0, sizeof(IMP_CTX));

	*imp_ctx = impctx;

	return RET_OK;	
}

//
//Освобождение контекста импорта
//
void ImpCtx_Free(IMP_CTX *imp_ctx)
{
	if (imp_ctx) {

		switch (imp_ctx->wObjType)
		{
			case DDE_TRN_OK:
			case DDE_CVAL_OK:
			case DDE_PLUGIN_TRN_OK:
				break;

			case DDE_TXT_TRN_OK:
				CloseHandle(imp_ctx->hTrnTxtFile);
				break;

			default : break;
		}

		Mem_Free(&imp_ctx);
	}
}

//
//Инициализация импорта в зависимости от типа обработки
//
DWORD Init_Imp(void **pImpStruct, WORD w_ObjType)
{
	DWORD dwRet = RET_OK;
	IMP_CTX *impctx = NULL;
	char chFileName[MAX_PATH] = {0};
	SYSTEMTIME lt;

	dwRet = Init_ImpCtx(&impctx);
	if (dwRet != RET_OK) {
		return dwRet;
	}

	impctx->wObjType = w_ObjType;

	switch (w_ObjType)
	{
		case DDE_TRN_OK:
		case DDE_CVAL_OK:
			dwRet = GetDbSessionFromPool(&impctx->wDbSessIdx, g_dwOraConnTimeOut);
			if (dwRet != RET_OK) {
				ImpCtx_Free(impctx);
				return dwRet;
			}
			break;

		case DDE_TXT_TRN_OK:
			//sprintf(chFileName, "%s%d%s", "trn_txt_", GetCurrentThreadId(), ".txt");
			GetLocalTime(&lt);    
			sprintf_s(chFileName, MAX_PATH, "%s%04d%02d%02d%s", "trn_txt_", lt.wYear, lt.wMonth, lt.wDay, ".txt");

			impctx->hTrnTxtFile = CreateFile(chFileName, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (impctx->hTrnTxtFile == INVALID_HANDLE_VALUE) {
				return GetLastError();
			}			
			break;

		case DDE_PLUGIN_TRN_OK:
			break;

		default:
			ImpCtx_Free(impctx);
			break;
	}

	*pImpStruct = impctx;

	return RET_OK;
}

//
//Точка начала процедуры импорта
//
void Begin_Imp(void *pImpStruct, WORD w_ObjType)
{
	IMP_CTX *impctx = NULL;

	impctx = (IMP_CTX *) pImpStruct;

	switch (w_ObjType)
	{
		case DDE_TRN_OK:
			BeginTransaction(impctx->wDbSessIdx);
			break;

		case DDE_TXT_TRN_OK:
		case DDE_CVAL_OK:
		case DDE_PLUGIN_TRN_OK:
			break;

		default:
			break;
	}
}

//
//Точка фиксации процедуры импорта
//
void Commit_Imp(void *pImpStruct, WORD w_ObjType)
{
	IMP_CTX *impctx = NULL;

	impctx = (IMP_CTX *) pImpStruct;

	switch (w_ObjType)
	{
		case DDE_TRN_OK:
			CommitTransaction(impctx->wDbSessIdx);
			break;

		case DDE_TXT_TRN_OK:
		case DDE_CVAL_OK:
		case DDE_PLUGIN_TRN_OK:
			break;

		default:
			break;
	}
}

//
//Освобождение процедуры импорта
//
void Release_Imp(void *pImpStruct, WORD w_ObjType)
{
	IMP_CTX *impctx = NULL;

	impctx = (IMP_CTX *) pImpStruct;

	switch (w_ObjType)
	{
		case DDE_TRN_OK:
		case DDE_CVAL_OK:
			RemoveDbSessionFromPool(impctx->wDbSessIdx);
			break;

		case DDE_TXT_TRN_OK:
		case DDE_PLUGIN_TRN_OK:
			break;

		default:
			break;
	}

	ImpCtx_Free(impctx);
}

//
//Импорт TRN в TXT
//
DWORD Imp_Trn2Txt(HANDLE hFile, TRN_OUT_CTX *trn_out_ctx)
{
	BOOL bRet;
	int i, j;
	DWORD dwNumBytesWritten;
	char *chBuf = NULL;
	DWORD dwLenBuf = 0;

	if (!trn_out_ctx) return ERR_ARGS;

	EnterCriticalSection(&CrSecTxtTrn);

	if (SetFilePointer(hFile, 0, NULL, FILE_END) == INVALID_SET_FILE_POINTER) {
		return GetLastError();
	}

	for (i = 0; i < trn_out_ctx->ctx_idx; i++) {
		for (j = 0; j < DDE_TRN_SIZE; j++) {
			dwLenBuf = (DWORD) strlen(trn_out_ctx->roboex_quik_st[i][j].chQuikName) + strlen(TEXT("=")) + strlen(trn_out_ctx->roboex_quik_st[i][j].chValue) + strlen(TEXT(";"));
			chBuf = (char *) malloc(dwLenBuf + 1);
			if (chBuf == NULL) {
				LeaveCriticalSection(&CrSecTxtTrn);
				return ERR_MALLOC;
			}
			memset(chBuf, 0, dwLenBuf + 1);
			sprintf_s(chBuf, dwLenBuf + 1, "%s%s%s%s", trn_out_ctx->roboex_quik_st[i][j].chQuikName, TEXT("="), trn_out_ctx->roboex_quik_st[i][j].chValue, TEXT(";"));

			bRet = WriteFile(hFile, chBuf, dwLenBuf, &dwNumBytesWritten, NULL);
			if (bRet == FALSE) {
				LeaveCriticalSection(&CrSecTxtTrn);
				return GetLastError();
			}
		}

		bRet = WriteFile(hFile, "\r\n", (DWORD) strlen("\r\n"), &dwNumBytesWritten, NULL);
		if (bRet == FALSE) {
			LeaveCriticalSection(&CrSecTxtTrn);
			return GetLastError();
		}
	}

	LeaveCriticalSection(&CrSecTxtTrn);

	return RET_OK;
}

//
//Импорт -- универсальная
//
DWORD Imp_Data(void *ctx, void *pImpStruct, WORD w_ObjType)
{
	DWORD dwRet = RET_OK;
	IMP_CTX *impctx = NULL;

	impctx = (IMP_CTX *) pImpStruct;

	switch (w_ObjType)
	{
		case DDE_TRN_OK:
			dwRet = Imp_Trn((TRN_OUT_CTX *) ctx, impctx->wDbSessIdx, g_chDdeImpTrn);
			break;

		case DDE_CVAL_OK:
			dwRet = Imp_Cval((CVAL_OUT_CTX *) ctx, impctx->wDbSessIdx, g_chDdeImpCval);
			break;

		case DDE_TXT_TRN_OK:
			dwRet = Imp_Trn2Txt(impctx->hTrnTxtFile, (TRN_OUT_CTX *) ctx);
			break;

		case DDE_PLUGIN_TRN_OK:
			dwRet = Imp_Plugin(ctx, NULL, w_ObjType, NULL);
			break;

		default: break;
	}

	return dwRet;
}

//EOF
