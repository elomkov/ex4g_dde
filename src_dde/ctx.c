/*
    Работа с контекстами
	ctx.c

	Author: MiDi$
*/

//#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <windows.h>
#include "const.h"
#include "struct.h"
#include "proto.h"
#include "template.h"
#include "ext_gvalue.h"

//
//Инициализация контекста TRN
//
DWORD Init_TrnCtx(TRN_OUT_CTX **trn_out_ctx, WORD w_Size)
{
	WORD i;
	TRN_OUT_CTX *trnout_ctx = NULL;

	if (*trn_out_ctx) {
		return ERR_CTX_EXISTS;
	}
	
	trnout_ctx = (TRN_OUT_CTX *) malloc(sizeof(TRN_OUT_CTX));
	if (trnout_ctx == NULL) {
		return ERR_MALLOC;
	}
	memset(trnout_ctx, 0, sizeof(TRN_OUT_CTX));

	trnout_ctx->ctx_idx = 0;
	trnout_ctx->ctx_size = w_Size;

	trnout_ctx->roboex_quik_st = (ROBOEX_QUIK_STRUCT **) malloc (w_Size * sizeof(ROBOEX_QUIK_STRUCT));
	if (trnout_ctx->roboex_quik_st == NULL) {
		Mem_Free(&trnout_ctx);
		return ERR_MALLOC;
	}
	memset(trnout_ctx->roboex_quik_st, 0, w_Size * sizeof(ROBOEX_QUIK_STRUCT));

	for (i = 0; i < w_Size; i++) {
		trnout_ctx->roboex_quik_st[i] = (ROBOEX_QUIK_STRUCT *) malloc (sizeof(ROBOEX_QUIK_STRUCT) * DDE_TRN_SIZE);
		if (trnout_ctx->roboex_quik_st[i] == NULL) {
			Mem_Free(&trnout_ctx);
			Mem_Free(trnout_ctx->roboex_quik_st);
			return ERR_MALLOC;
		}
		memset(trnout_ctx->roboex_quik_st[i], 0, sizeof(ROBOEX_QUIK_STRUCT) * DDE_TRN_SIZE);
	}

	*trn_out_ctx = trnout_ctx;

	return RET_OK;
}

//
//Освобождение контекста TRN
//
void TrnCtx_Free(TRN_OUT_CTX *trn_out_ctx)
{
	WORD i, j, wSize;

	if (trn_out_ctx) {

		wSize = trn_out_ctx->ctx_size;

		for (i = 0; i < wSize; i++) {
			for (j = 0; j < DDE_TRN_SIZE; j++) {
				Mem_Free(&trn_out_ctx->roboex_quik_st[i][j].chValue);
			}
			Mem_Free(&trn_out_ctx->roboex_quik_st[i]);
		}

		Mem_Free(&trn_out_ctx);
	}
}

//
//Инициализация контекста CVAL
//
DWORD Init_CvalCtx(CVAL_OUT_CTX **cval_out_ctx, WORD w_Size)
{
	WORD i;
	CVAL_OUT_CTX *cvalout_ctx = NULL;

	if (*cval_out_ctx) {
		return ERR_CTX_EXISTS;
	}
	
	cvalout_ctx = (CVAL_OUT_CTX *) malloc(sizeof(CVAL_OUT_CTX));
	if (cvalout_ctx == NULL) {
		return ERR_MALLOC;
	}
	memset(cvalout_ctx, 0, sizeof(CVAL_OUT_CTX));

	cvalout_ctx->ctx_idx = 0;
	cvalout_ctx->ctx_size = w_Size;

	cvalout_ctx->roboex_quik_st = (ROBOEX_QUIK_STRUCT **) malloc (w_Size * sizeof(ROBOEX_QUIK_STRUCT));
	if (cvalout_ctx->roboex_quik_st == NULL) {
		Mem_Free(&cvalout_ctx);
		return ERR_MALLOC;
	}
	memset(cvalout_ctx->roboex_quik_st, 0, w_Size * sizeof(ROBOEX_QUIK_STRUCT));

	for (i = 0; i < w_Size; i++) {
		cvalout_ctx->roboex_quik_st[i] = (ROBOEX_QUIK_STRUCT *) malloc (sizeof(ROBOEX_QUIK_STRUCT) * DDE_CVAL_SIZE);
		if (cvalout_ctx->roboex_quik_st[i] == NULL) {
			Mem_Free(&cvalout_ctx);
			Mem_Free(cvalout_ctx->roboex_quik_st);
			return ERR_MALLOC;
		}
		memset(cvalout_ctx->roboex_quik_st[i], 0, sizeof(ROBOEX_QUIK_STRUCT) * DDE_CVAL_SIZE);
	}

	*cval_out_ctx = cvalout_ctx;

	return RET_OK;
}

//
//Освобождение контекста CVAL
//
void CvalCtx_Free(CVAL_OUT_CTX *cval_out_ctx)
{
	WORD i, j, wSize;

	if (cval_out_ctx) {

		wSize = cval_out_ctx->ctx_size;

		for (i = 0; i < wSize; i++) {
			for (j = 0; j < DDE_CVAL_SIZE; j++) {
				Mem_Free(&cval_out_ctx->roboex_quik_st[i][j].chValue);
			}
			Mem_Free(&cval_out_ctx->roboex_quik_st[i]);
		}

		Mem_Free(&cval_out_ctx);
	}
}

//
//Инициализация контекста -- универсальная
//
DWORD Init_Ctx(void **out_ctx, WORD w_Size, WORD w_ObjType)
{
	DWORD dwRet = RET_OK;
	void *ctx = NULL;

	switch (w_ObjType)
	{
		case DDE_TRN_OK:
		case DDE_TXT_TRN_OK:
		case DDE_PLUGIN_TRN_OK:
			dwRet = Init_TrnCtx((TRN_OUT_CTX **) &ctx, w_Size);
			break;

		case DDE_CVAL_OK:
			dwRet = Init_CvalCtx((CVAL_OUT_CTX **) &ctx, w_Size);
			break;

		default: break;
	}

	*out_ctx = ctx;

	return dwRet;
}

//
//Освобождение контекста -- универсальная
//
void Ctx_Free(void *out_ctx, WORD w_ObjType)
{
	switch (w_ObjType)
	{
		case DDE_TRN_OK:
		case DDE_TXT_TRN_OK:
		case DDE_PLUGIN_TRN_OK:
			TrnCtx_Free((TRN_OUT_CTX *) out_ctx);
			break;

		case DDE_CVAL_OK:
			CvalCtx_Free((CVAL_OUT_CTX *) out_ctx);
			break;

		default: break;
	}
}

//
//Заполнение контекста -- TRN
//
DWORD Fill_TrnCtx(TRN_OUT_CTX **trn_out_ctx, char *ch_Data, WORD w_ctx_num)
{
	TRN_OUT_CTX *trn_outctx = NULL;
	WORD w_ctx_num_full;

	if (!trn_out_ctx || !ch_Data) return ERR_ARGS;

	trn_outctx = (*trn_out_ctx);
	w_ctx_num_full = trn_outctx->ctx_idx;

	strncpy_s(trn_outctx->roboex_quik_st[w_ctx_num_full][w_ctx_num].chQuikName, STRUCT_COL_SIZE + 1, roboex_quik_trn_template[w_ctx_num].chQuikName, STRUCT_COL_SIZE);
	strncpy_s(trn_outctx->roboex_quik_st[w_ctx_num_full][w_ctx_num].chRoboexName, STRUCT_COL_SIZE + 1, roboex_quik_trn_template[w_ctx_num].chRoboexName, STRUCT_COL_SIZE);
	trn_outctx->roboex_quik_st[w_ctx_num_full][w_ctx_num].chValue = ch_Data;

	return RET_OK;
}

//
//Заполнение контекста -- CVAL
//
DWORD Fill_CvalCtx(CVAL_OUT_CTX **cval_out_ctx, char *ch_Data, WORD w_ctx_num)
{
	CVAL_OUT_CTX *cval_outctx = NULL;
	WORD w_ctx_num_full;

	if (!cval_out_ctx || !ch_Data) return ERR_ARGS;

	cval_outctx = *cval_out_ctx;
	w_ctx_num_full = cval_outctx->ctx_idx;

	strncpy_s(cval_outctx->roboex_quik_st[w_ctx_num_full][w_ctx_num].chQuikName, STRUCT_COL_SIZE + 1, roboex_quik_cval_template[w_ctx_num].chQuikName, STRUCT_COL_SIZE);
	strncpy_s(cval_outctx->roboex_quik_st[w_ctx_num_full][w_ctx_num].chRoboexName, STRUCT_COL_SIZE + 1, roboex_quik_cval_template[w_ctx_num].chRoboexName, STRUCT_COL_SIZE);
	cval_outctx->roboex_quik_st[w_ctx_num_full][w_ctx_num].chValue = ch_Data;

	return RET_OK;
}

//
//Заполнение контекста -- универсальная
//
DWORD Fill_Ctx(void **out_ctx, char *ch_Data, WORD w_ctx_num, WORD w_ObjType)
{
	DWORD dwRet = RET_OK;
	void *ctx = NULL;

	ctx = *out_ctx;

	switch (w_ObjType)
	{
		case DDE_TRN_OK:
		case DDE_TXT_TRN_OK:
		case DDE_PLUGIN_TRN_OK:
			dwRet = Fill_TrnCtx((TRN_OUT_CTX **) &ctx, ch_Data, w_ctx_num);
			break;

		case DDE_CVAL_OK:
			dwRet = Fill_CvalCtx((CVAL_OUT_CTX **) &ctx, ch_Data, w_ctx_num);
			break;

		default: break;
	}

	return dwRet;
}

//
// Вычисление индекса
//
WORD GetNumNext(WORD wObjType, WORD wNum)
{
	if (wNum > WORD_DIM) return 0;

	wNum++;
	if (wObjType == DDE_TRN_OK && wNum == DDE_TRN_SIZE) {
		return 0;
	}
	else if (wObjType == DDE_TXT_TRN_OK && wNum == DDE_TRN_SIZE) {
		return 0;
	}
	else if (wObjType == DDE_PLUGIN_TRN_OK && wNum == DDE_TRN_SIZE) {
		return 0;
	}
	else if (wObjType == DDE_CVAL_OK && wNum == DDE_CVAL_SIZE) {
		return 0;
	}

	return wNum;	
}

void SetNextTrnIdx(TRN_OUT_CTX **trn_out_ctx)
{
	TRN_OUT_CTX *trn_outctx = NULL;

	if (trn_out_ctx) {
		trn_outctx = (*trn_out_ctx);
		trn_outctx->ctx_idx++;
	}	
}

void SetNextCvalIdx(CVAL_OUT_CTX **cval_out_ctx)
{
	CVAL_OUT_CTX *cval_outctx = NULL;

	if (cval_out_ctx) {
		cval_outctx = (*cval_out_ctx);
		cval_outctx->ctx_idx++;
	}	
}

void SetNextCtxIdx(void **out_ctx, WORD w_ObjType)
{
	switch (w_ObjType)
	{
		case DDE_TRN_OK:
		case DDE_TXT_TRN_OK:
		case DDE_PLUGIN_TRN_OK:
			SetNextTrnIdx((TRN_OUT_CTX **) out_ctx);
			break;

		case DDE_CVAL_OK:
			SetNextCvalIdx((CVAL_OUT_CTX **) out_ctx);
			break;

		default: break;
	}
}

//
//log контекста - больше для отладки
//
DWORD WriteTrnInf(LPCTSTR lp_FileName, TRN_OUT_CTX *trn_out_ctx)
{
#define BUF_LEN 2048
	WORD i, j;
	char chBuf[BUF_LEN];
	DWORD dwRet;
	BOOL bRet;
	HANDLE hCreateFile;
	DWORD dwNumBytesWritten;

	if (!lp_FileName || !trn_out_ctx) return ERR_ARGS;

	hCreateFile = CreateFile(lp_FileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hCreateFile == INVALID_HANDLE_VALUE) {
		return GetLastError();
	}

	dwRet = SetFilePointer(hCreateFile, 0, NULL, FILE_END);
	if (dwRet == INVALID_SET_FILE_POINTER) {
		CloseHandle(hCreateFile);
		return GetLastError();
	}

	for (i = 0; i < trn_out_ctx->ctx_idx; i++) {
		memset(chBuf, 0, BUF_LEN);
		sprintf_s(chBuf, BUF_LEN, "%s%s", g_chDdeImpTrn, " : \r\n");
		bRet = WriteFile(hCreateFile, chBuf, (DWORD) strlen(chBuf), &dwNumBytesWritten, NULL);
		if (bRet == FALSE) {
			CloseHandle(hCreateFile);
			return GetLastError();
		}
		for (j = 0; j < DDE_TRN_SIZE; j++) {
			memset(chBuf, 0, BUF_LEN);
			sprintf_s(chBuf, BUF_LEN, "  %s = %s\r\n", trn_out_ctx->roboex_quik_st[i][j].chQuikName, trn_out_ctx->roboex_quik_st[i][j].chValue);
			bRet = WriteFile(hCreateFile, chBuf, (DWORD) strlen(chBuf), &dwNumBytesWritten, NULL);
			if (bRet == FALSE) {
				CloseHandle(hCreateFile);
				return GetLastError();
			}
		}
	}

	CloseHandle(hCreateFile);

	return RET_OK;
}

//EOF