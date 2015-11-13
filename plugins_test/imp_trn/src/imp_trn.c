/*
    Плагин - обработка TRN
	imp_trn.c

	Author: MiDi$
*/

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <windows.h>
#include "struct_exp.h"

DWORD WINAPI IMP_TRN_PLUGIN(void *ctx, void *pImpStruct, WORD w_ObjType, void *pRes1)
{

	BOOL bRet;
	int i, j;
	DWORD dwNumBytesWritten;
	char *chBuf = NULL;
	DWORD dwLenBuf = 0;
	TRN_OUT_CTX *trn_out_ctx = NULL;
	char chFileName[MAX_PATH] = {0};
	SYSTEMTIME lt;
	HANDLE hFile = NULL;

	if (!ctx) return ERR_ARGS;

	trn_out_ctx = (TRN_OUT_CTX *) ctx;

	GetLocalTime(&lt);    
	sprintf(chFileName, "%s%04d%02d%02d%s", "trn_txt_plugin_", lt.wYear, lt.wMonth, lt.wDay, ".txt");

	hFile = CreateFile(chFileName, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		CloseHandle(hFile);
		return GetLastError();
	}			

	if (SetFilePointer(hFile, 0, NULL, FILE_END) == INVALID_SET_FILE_POINTER) {
		CloseHandle(hFile);
		return GetLastError();
	}

	for (i = 0; i < trn_out_ctx->ctx_idx; i++) {
		for (j = 0; j < DDE_TRN_SIZE; j++) {
			dwLenBuf = (DWORD) strlen(trn_out_ctx->roboex_quik_st[i][j].chQuikName) + strlen("=") + strlen(trn_out_ctx->roboex_quik_st[i][j].chValue) + strlen(";");
			chBuf = (char *) malloc(dwLenBuf + 1);
			if (chBuf == NULL) {
				CloseHandle(hFile);
				return ERR_MALLOC;
			}
			memset(chBuf, 0, dwLenBuf + 1);
			sprintf(chBuf, "%s%s%s%s", trn_out_ctx->roboex_quik_st[i][j].chQuikName, "=", trn_out_ctx->roboex_quik_st[i][j].chValue, ";");

			bRet = WriteFile(hFile, chBuf, dwLenBuf, &dwNumBytesWritten, NULL);
			if (bRet == FALSE) {
				CloseHandle(hFile);
				return GetLastError();
			}
		}

		bRet = WriteFile(hFile, "\r\n", (DWORD) strlen("\r\n"), &dwNumBytesWritten, NULL);
		if (bRet == FALSE) {
			CloseHandle(hFile);
			return GetLastError();
		}
	}

	CloseHandle(hFile);

	return 0;
}