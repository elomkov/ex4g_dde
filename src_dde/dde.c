/*
	DDE взаимодействие
	dde.c

	Author: MiDi$
*/

//#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <windows.h>
#include <process.h>
#include "const.h"
#include "struct.h"
#include "proto.h"
#include "dde_const.h"
#include "ext_gvalue.h"

CRITICAL_SECTION CrThreadXlTableParse;
CRITICAL_SECTION CrWriteLogDDE;

//
//Парсит XLTable
//
DWORD ParseXLTable(WORD w_ObjType, LPBYTE lpbBufData, DWORD dwBufLen)
{
	WORD i;
	WORD wSize = 0;
	void *out_ctx = NULL;
	DWORD dwRet = RET_OK;
	WORD *wTypeBlock = NULL;  // Тип блока
	WORD *wDataCnt;			  // Длина буфера в блоке
	WORD wRowsNum;			  // Число строк
	WORD wColNum;			  // Число полей
	char *chDataBlock = NULL; // Временный буфер для хранения значений блока tdtString
	DWORD dwIdx = 0;		  // Счетчик считанных байт в общем буфере	
	double dNum = 0;		  // Значение блока tdtFloat
	WORD ctx_num = 0;		  // Локальный текущий индекс по массиву ROBOEX_QUIK_STRUCT
	WORD ctx_num_full = 0;	  // Глобальный индекс по массиву структур 
	WORD w_commit_num = 0;    // Счетчик на фиксацию транзакции
	WORD w_ctx_num_rows = 0;  // Счетчик на число записей в контексте
	void *pImpStruct = NULL;
	WORD wRealCntRows = 0;


	if (!lpbBufData || dwBufLen == 0) return ERR_ARGS;

	//Парсинг полученного массива из блоков XLTable
	while (dwIdx < dwBufLen)
	{
		wTypeBlock = (WORD *) &lpbBufData[dwIdx];
		if (*wTypeBlock == DDE_TABLE_BLK) {	//Тип tdtTable			
			dwIdx += (DWORD) WORD_SIZE;
			wDataCnt = (WORD *) &lpbBufData[dwIdx];			
			dwIdx += (DWORD) WORD_SIZE;
			wRowsNum = (WORD) *((WORD *) &lpbBufData[dwIdx]);			
			dwIdx += (DWORD) WORD_SIZE;
			wColNum = (WORD) *((WORD *) &lpbBufData[dwIdx]);			
			dwIdx += (DWORD) WORD_SIZE;

			//Проверка корректности поставляемых блоков и определение типа объекта
			dwRet = IsObjAndSizeOk(w_ObjType, wColNum);
			if (dwRet != RET_OK){				
				return dwRet;
			}

			//Инициализация структур импорта
			dwRet = Init_Imp(&pImpStruct, w_ObjType);
			if (dwRet != RET_OK) {
				return dwRet;
			}

			Begin_Imp(pImpStruct, w_ObjType);

			//Инициализация контекста в зависимости от типа объекта
			if (g_dwCtxCntRows > wRowsNum) {
				wRealCntRows = wRowsNum;
			}
			else wRealCntRows = (WORD) g_dwCtxCntRows;
			dwRet = Init_Ctx(&out_ctx, wRealCntRows, w_ObjType);
			if (dwRet != RET_OK) {
				Release_Imp(pImpStruct, w_ObjType);
				return dwRet;
			}

		}
		else if (*wTypeBlock == DDE_STRING_BLK) {	//Тип tdtString			
			dwIdx += (DWORD) WORD_SIZE;
			wDataCnt = (WORD *) &lpbBufData[dwIdx]; //Общая длина строки			
			dwIdx += (DWORD) WORD_SIZE;
			i = 0;
			while (i < *wDataCnt) {
				wSize = (WORD) *(BYTE *) &lpbBufData[dwIdx];
				chDataBlock = (char *) malloc(wSize + 1);
				if (chDataBlock == NULL) {
					Ctx_Free(out_ctx, w_ObjType);
					return ERR_MALLOC;
				}
				memset(chDataBlock, 0, wSize + 1);
				if (wSize != 0) {
					memcpy(chDataBlock, &lpbBufData[dwIdx + 1], wSize);
				}
				Fill_Ctx(&out_ctx, chDataBlock, ctx_num, w_ObjType);
				i += wSize + 1;
				dwIdx += wSize + 1;				
				ctx_num = GetNumNext(w_ObjType, ctx_num);
				if (ctx_num == 0) {
					ctx_num_full++;
					w_commit_num++;
					w_ctx_num_rows++;
					SetNextCtxIdx(&out_ctx, w_ObjType);
				}
			}
		}
		else if (*wTypeBlock == DDE_FLOAT_BLK) {	//Тип tdtFloat
			dwIdx += (DWORD) WORD_SIZE;
			wDataCnt = (WORD *) &lpbBufData[dwIdx]; //Общая длина строки			
			dwIdx += (DWORD) WORD_SIZE;
			i = 0;
			wSize = DOUBLE_SIZE;
			while (i < *wDataCnt) {
				chDataBlock = (char *) malloc(wSize + 1);
				if (chDataBlock == NULL) {
					Ctx_Free(out_ctx, w_ObjType);
					return ERR_MALLOC;
				}
				memset(chDataBlock, 0, wSize + 1);
				memcpy(chDataBlock, &lpbBufData[dwIdx], wSize);
				dNum = *((double *) chDataBlock); //OK!!
				Mem_Free(&chDataBlock);
				chDataBlock = (char *) malloc(NUM_LEN + 1);
				if (chDataBlock == NULL) {
					Ctx_Free(out_ctx, w_ObjType);
					return ERR_MALLOC;	
				}
				memset(chDataBlock, 0, NUM_LEN + 1);
				sprintf_s(chDataBlock, NUM_LEN + 1, "%f", dNum);
				i += wSize;
				dwIdx += wSize;

				Fill_Ctx(&out_ctx, chDataBlock, ctx_num, w_ObjType);
				
				ctx_num = GetNumNext(w_ObjType, ctx_num);
				if (ctx_num == 0) {
					ctx_num_full++;
					w_commit_num++;
					w_ctx_num_rows++;
					SetNextCtxIdx(&out_ctx, w_ObjType);
				}
			}
		}
		else {			
			dwRet = ERR_XL_TYPE;
			break;
		}

		//Импорт
		if (dwRet == RET_OK) {
			if (w_ctx_num_rows >= wRealCntRows) {
				w_ctx_num_rows = 0;
				dwRet = Imp_Data(out_ctx, pImpStruct, w_ObjType);
				Ctx_Free(out_ctx, w_ObjType);
				dwRet = Init_Ctx(&out_ctx, wRealCntRows, w_ObjType);
				if (dwRet != RET_OK) {
					Commit_Imp(pImpStruct, w_ObjType);
					return dwRet;
				}
			}
			if (w_commit_num >= g_dwOraCntTranCommit) { //Фиксация транзакции
				w_commit_num = 0;
				Commit_Imp(pImpStruct, w_ObjType);
				Begin_Imp(pImpStruct, w_ObjType);
			}
		}
	}

	dwRet = Imp_Data(out_ctx, pImpStruct, w_ObjType);
	Commit_Imp(pImpStruct, w_ObjType);
	Release_Imp(pImpStruct, w_ObjType);
	Ctx_Free(out_ctx, w_ObjType);

	return dwRet;
}

//
//Инициализация потокового контекста
//
DWORD Init_ThreadCtx(DDE_POKE_THREAD **dde_poke_thread)
{
	DDE_POKE_THREAD *ddepoke_thread = NULL;

	ddepoke_thread = (DDE_POKE_THREAD *) malloc (sizeof(DDE_POKE_THREAD));
	if (ddepoke_thread == NULL) return ERR_MALLOC;
	memset(ddepoke_thread, 0, sizeof(DDE_POKE_THREAD)); 

	*dde_poke_thread = ddepoke_thread;

	return RET_OK;
}

//
//Освобождение потокового контекста
//
void ThreadCtx_Free(DDE_POKE_THREAD *dde_poke_thread)
{
	if (dde_poke_thread) {
		dde_poke_thread->hSemaphor = NULL;
		Mem_Free(&dde_poke_thread->lpbBufData);
		Mem_Free(&dde_poke_thread);
	}
}

//
//Атомарные действа в начале исполнения потока
//
void Inc_ThreadCount()
{
	SYSTEMTIME lt;
	char chTime[256] = {0};

	EnterCriticalSection(&CrThreadXlTableParse);
	GetLocalTime(&lt);    
	sprintf_s(chTime, 256, "[DBG] Time Begin_%d: %02d:%02d:%02d", GetCurrentThreadId(), lt.wHour, lt.wMinute, lt.wSecond);
	WriteLog_DDE(chTime, 0);
	ResetEvent(g_hEventThreads);
	g_lThreadCount++;
	LeaveCriticalSection(&CrThreadXlTableParse);	
}

//
//Атомарные действа по окончанию потока
//
void Dec_ThreadCount(HANDLE hSemaphor)
{
	SYSTEMTIME lt;
	char chTime[256] = {0};

	EnterCriticalSection(&CrThreadXlTableParse);
	g_lThreadCount--;
	ReleaseSemaphore(hSemaphor, 1, NULL);
	if (g_lThreadCount == 0 && g_bThreadExec == FALSE) {
		SetEvent(g_hEventThreads);
	}
	GetLocalTime(&lt);    
	sprintf_s(chTime, 256, "[DBG] Time End_%d: %02d:%02d:%02d", GetCurrentThreadId(), lt.wHour, lt.wMinute, lt.wSecond);
	WriteLog_DDE(chTime, 0);
	LeaveCriticalSection(&CrThreadXlTableParse);	
}

//
//Потоковая версия
//
DWORD WINAPI ParseXLTable_Thread(LPVOID dde_poke_thread)
{
	DWORD dwRet = RET_OK;
	DDE_POKE_THREAD *ddepoke_thread = NULL;
	HANDLE hSemaphor = NULL;

	Inc_ThreadCount();

	ddepoke_thread = (DDE_POKE_THREAD *) dde_poke_thread;
	hSemaphor = ddepoke_thread->hSemaphor; 

	//Ожидание на семафоре
	dwRet = WaitForSingleObject(ddepoke_thread->hSemaphor, INFINITE);
	if (dwRet != WAIT_OBJECT_0) {
		if (dwRet == WAIT_FAILED) {
			dwRet = GetLastError();
		}
		ThreadCtx_Free(dde_poke_thread);
		Dec_ThreadCount(hSemaphor);
		return dwRet;
	}

	if (g_bThreadExec == FALSE){
		ThreadCtx_Free(dde_poke_thread);
		Dec_ThreadCount(hSemaphor);
		return RET_OK;
	}

	dwRet = ParseXLTable(ddepoke_thread->wObjType, ddepoke_thread->lpbBufData, ddepoke_thread->dwBufLen);
	if (dwRet != RET_OK) {
		WriteLog_DDE("[ERR] ParseXLTable_Thread(..)", dwRet);
	}

	ThreadCtx_Free(dde_poke_thread);
	Dec_ThreadCount(hSemaphor);

	return dwRet;
}

//
//Получение хэндла семафора по типу объекта
//
HANDLE GetSemaphorByObjType(WORD w_ObjType)
{
	HANDLE hSemaphor = NULL;

	switch (w_ObjType)
	{
		case DDE_TRN_OK:
		case DDE_TXT_TRN_OK:
		case DDE_PLUGIN_TRN_OK:
			hSemaphor = g_hTrnSemaphore;
			break;

		case DDE_CVAL_OK:
			hSemaphor = g_hCvalSemaphore;
			break;

		default: break;
	}

	return hSemaphor;
}

//
//Обработка callback'ов
//
HDDEDATA CALLBACK DdeCallback(UINT uType,
							  UINT uFmt,
							  HCONV hconv,
							  HSZ hsz1,
							  HSZ hsz2,
							  HDDEDATA hData,
							  ULONG_PTR dwData1,
							  ULONG_PTR dwData2) 
{
	DWORD dwRet;
	DWORD dwDataCnt;
	LPBYTE lpbBufData = NULL;	
	char *chOraObj = NULL;
	LPTSTR lpTopic = NULL;
	HANDLE hThread = NULL; //Хэндл потока
	WORD wObjType = 0;     //Тип объекта
	DDE_POKE_THREAD *dde_poke_thread = NULL;

    switch (uType) 
    {
		case XTYP_ADVDATA:			
			WriteLog_DDE("[DBG] DdeCallback(..) -> XTYP_ADVDATA", 0);
			return (HDDEDATA) DDE_FNOTPROCESSED;
		case XTYP_ADVREQ:			
			WriteLog_DDE("[DBG] DdeCallback(..) -> XTYP_ADVREQ", 0);
			break;
		case XTYP_ADVSTART:			
			WriteLog_DDE("[DBG] DdeCallback(..) -> XTYP_ADVSTART", 0);
			return (HDDEDATA) FALSE;
		case XTYP_ADVSTOP: //This transaction is filtered if the server application specified the CBF_FAIL_ADVISES flag in the DdeInitialize function.
			WriteLog_DDE("[DBG] DdeCallback(..) -> XTYP_ADVSTOP", 0);
			break;
		case XTYP_CONNECT:
			if (DdeCmpStringHandles(g_hszServName, hsz2) != 0) {
				WriteLog_DDE("[ERR] DdeCallback(..) -> XTYP_CONNECT -> DdeCmpStringHandles(..)", -1);
				return (HDDEDATA) FALSE;
			}			
			WriteLog_DDE("[DBG] DdeCallback(..) -> XTYP_CONNECT", 0);
			return (HDDEDATA) TRUE;
		case XTYP_CONNECT_CONFIRM: //This transaction is filtered if the server application specified the CBF_SKIP_CONNECT_CONFIRMS flag in the DdeInitialize function.			
			WriteLog_DDE("[DBG] DdeCallback(..) -> XTYP_CONNECT_CONFIRM", 0);
			break;
		case XTYP_DISCONNECT: //This transaction is filtered if the application specified the CBF_SKIP_DISCONNECTS flag in the DdeInitialize function.			
			WriteLog_DDE("[DBG] DdeCallback(..) -> XTYP_DISCONNECT", 0);
			break;
		case XTYP_ERROR:			
			WriteLog_DDE("[DBG] DdeCallback(..) -> XTYP_ERROR", 0);
			break;
		case XTYP_EXECUTE:
			WriteLog_DDE("[DBG] DdeCallback(..) -> XTYP_EXECUTE", 0);
			return (HDDEDATA) DDE_FNOTPROCESSED;
		case XTYP_MONITOR:
			WriteLog_DDE("[DBG] DdeCallback(..) -> XTYP_MONITOR", 0);
			return (HDDEDATA) 0;
		case XTYP_POKE:
			if (g_bThreadExec == FALSE){
					return (HDDEDATA) DDE_FACK;
			}
			WriteLog_DDE("[DBG] DdeCallback(..) -> XTYP_POKE", 0);

			if (g_uXLTableType != uFmt) {				
				WriteLog_DDE("[ERR] DdeCallback(..) -> g_uXLTableType != uFmt -> uFmt", uFmt);
				return (HDDEDATA) DDE_FNOTPROCESSED;
			}

			dwDataCnt = DdeQueryString(g_dwIdInst, hsz1, NULL, 0, CP_WINANSI);
			if (dwDataCnt == 0) {				
				WriteLog_DDE("[ERR] DdeCallback(..) -> DdeQueryString(..)", dwDataCnt);
				return (HDDEDATA) DDE_FNOTPROCESSED;
			}

			lpTopic = (LPTSTR) malloc(dwDataCnt + 1);
			if (lpTopic == NULL) {
				WriteLog_DDE("[ERR] DdeCallback(..) -> lpTopic = malloc(..)", 0);
				return (HDDEDATA) DDE_FNOTPROCESSED;
			}

			dwRet = DdeQueryString(g_dwIdInst, hsz1, lpTopic, dwDataCnt + 1, CP_WINANSI);
			if (dwDataCnt == 0) {				
				WriteLog_DDE("[ERR] DdeCallback(..) -> DdeQueryString(..)", dwDataCnt);
				Mem_Free(&lpTopic);
				return (HDDEDATA) DDE_FNOTPROCESSED;
			}

			dwRet = GetOraObjName((char *) lpTopic, &chOraObj);
			if (dwRet != RET_OK) {				
				WriteLog_DDE("[ERR] DdeCallback(..) -> GetOraObjName(..)", dwRet);
				Mem_Free(&lpTopic);
				return (HDDEDATA) DDE_FNOTPROCESSED;
			}

			Mem_Free(&lpTopic);

			//Определение размера получаемых данных
			dwDataCnt = DdeGetData(hData, NULL, sizeof(DWORD), 0);
			if (dwDataCnt == DMLERR_DLL_NOT_INITIALIZED || dwDataCnt == DMLERR_INVALIDPARAMETER || dwDataCnt == 0) {				
				WriteLog_DDE("[ERR] DdeCallback(..) -> DdeGetData(..)", dwDataCnt);
				Mem_Free(&chOraObj);
				return (HDDEDATA) DDE_FNOTPROCESSED;
			}			

			lpbBufData = (LPBYTE) malloc(dwDataCnt);
			if (lpbBufData == NULL) {
				WriteLog_DDE("[ERR] DdeCallback(..) -> lpbBufData = malloc(..)", 0);
				Mem_Free(&chOraObj);
				return (HDDEDATA) DDE_FNOTPROCESSED;
			}
			memset(lpbBufData, 0, dwDataCnt);

			dwRet = DdeGetData(hData, lpbBufData, dwDataCnt, 0);
			if (dwRet == DMLERR_DLL_NOT_INITIALIZED || dwRet == DMLERR_INVALIDPARAMETER || dwRet == 0) {				
				WriteLog_DDE("[ERR] DdeCallback(..) -> DdeGetData(..)", dwRet);
				Mem_Free(&chOraObj);
				Mem_Free(&lpbBufData);
				return (HDDEDATA) DDE_FNOTPROCESSED;
			}

			dwRet = Init_ThreadCtx(&dde_poke_thread);
			if (dwRet != RET_OK) {
				WriteLog_DDE("[ERR] DdeCallback(..) -> Init_ThreadCtx(..)", dwRet);
				Mem_Free(&chOraObj);
				Mem_Free(&lpbBufData);
				return (HDDEDATA) DDE_FNOTPROCESSED;
			}

			//Определение типа объекта
			dwRet = IsObjOk(chOraObj, &wObjType);
			if (dwRet != RET_OK) {
				WriteLog_DDE("[ERR] DdeCallback(..) -> IsObjOk(..)", dwRet);
				Mem_Free(&chOraObj);
				Mem_Free(&lpbBufData);
				return (HDDEDATA) DDE_FNOTPROCESSED;
			}

			//Формирование контекста, далее осовобождение памяти через контекст
			dde_poke_thread->wObjType = wObjType;
			dde_poke_thread->lpbBufData = lpbBufData;
			dde_poke_thread->dwBufLen = dwDataCnt;
			dde_poke_thread->hSemaphor = GetSemaphorByObjType(wObjType);

			//Потоковый вызов рулит
			hThread = (HANDLE) _beginthreadex(NULL, 0, ParseXLTable_Thread, dde_poke_thread, 0, NULL);			
			if (hThread == NULL) {
				WriteLog_DDE("[ERR] DdeCallback(..) -> ParseXLTable_Thread", GetLastError());
				ThreadCtx_Free(dde_poke_thread);
				return (HDDEDATA) DDE_FNOTPROCESSED;
			}
			if (g_chMultiThread == 'N') {//Эмуляция однопоточной среды
				WaitForSingleObject(hThread, INFINITE); //Ожидание завершения потока
			}
			CloseHandle(hThread); // HANDLE потока нам больше не нужен

			return (HDDEDATA) DDE_FACK;
		case XTYP_REGISTER: //This transaction is filtered if the application specified the CBF_SKIP_REGISTRATIONS flag in the DdeInitialize function. 
			WriteLog_DDE("[DBG] DdeCallback(..) -> XTYP_REGISTER", 0);
			break;
		case XTYP_REQUEST:
			WriteLog_DDE("[DBG] DdeCallback(..) -> XTYP_REQUEST", 0);
			break;
		case XTYP_UNREGISTER: //This transaction is filtered if the application specified the CBF_SKIP_REGISTRATIONS flag in the DdeInitialize function.
			WriteLog_DDE("[DBG] DdeCallback(..) -> XTYP_UNREGISTER", 0);
			break;
		case XTYP_WILDCONNECT:
			WriteLog_DDE("[DBG] DdeCallback(..) -> XTYP_WILDCONNECT", 0);
			break;
		case XTYP_XACT_COMPLETE:
			WriteLog_DDE("[DBG] DdeCallback(..) -> XTYP_XACT_COMPLETE", 0);
			break;
		default:
			WriteLog_DDE("[DBG] DdeCallback(..) -> default", 0);
            break;
    }

	return (HDDEDATA) NULL;
}

//
//Инициализация DDE и не только
//
UINT Init_Dde()
{
	UINT uRet;
	HDDEDATA hDdeData;

	//Инициализация DDE
	uRet = DdeInitialize(&g_dwIdInst, (PFNCALLBACK) DdeCallback, APPCLASS_STANDARD | MF_CALLBACKS | MF_CONV | MF_ERRORS | MF_HSZ_INFO | MF_LINKS | MF_POSTMSGS | MF_SENDMSGS, 0);
	if (uRet != DMLERR_NO_ERROR) {
		return uRet;
	}

	//Получение хэндла от имени DDE сервиса
	g_hszServName = DdeCreateStringHandle(g_dwIdInst, g_lpDdeServName, CP_WINANSI);
	if (g_hszServName == 0) {
		DdeUninitialize(g_dwIdInst);
		return DdeGetLastError(g_dwIdInst);
	}

	//Регистрация имени сервиса DDE
	hDdeData = DdeNameService(g_dwIdInst, g_hszServName, 0, DNS_REGISTER);
	if (hDdeData == 0) {
		DdeFreeStringHandle(g_dwIdInst, g_hszServName);
		DdeUninitialize(g_dwIdInst);
		return DdeGetLastError(g_dwIdInst);
	}

	g_uXLTableType = RegisterClipboardFormat((LPSTR) g_lpXlTable);
	if (g_uXLTableType == 0) {
		DdeNameService(g_dwIdInst, 0, 0, DNS_UNREGISTER);
		DdeFreeStringHandle(g_dwIdInst, g_hszServName);
		DdeUninitialize(g_dwIdInst);
		return GetLastError();
	}

	g_hTrnSemaphore = CreateSemaphore(NULL, g_dwThreadsCntTrnMax, g_dwThreadsCntTrnMax, NULL);
	if (g_hTrnSemaphore == NULL) {
		DdeNameService(g_dwIdInst, 0, 0, DNS_UNREGISTER);
		DdeFreeStringHandle(g_dwIdInst, g_hszServName);
		DdeUninitialize(g_dwIdInst);
		return GetLastError();
	}

	g_hCvalSemaphore = CreateSemaphore(NULL, CVAL_MAX_THREADS, CVAL_MAX_THREADS, NULL);
	if (g_hCvalSemaphore == NULL) {
		DdeNameService(g_dwIdInst, 0, 0, DNS_UNREGISTER);
		DdeFreeStringHandle(g_dwIdInst, g_hszServName);
		DdeUninitialize(g_dwIdInst);
		CloseHandle(g_hTrnSemaphore);
		return GetLastError();
	}

	g_hEventThreads = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (g_hEventThreads == NULL) {
		DdeNameService(g_dwIdInst, 0, 0, DNS_UNREGISTER);
		DdeFreeStringHandle(g_dwIdInst, g_hszServName);
		DdeUninitialize(g_dwIdInst);
		CloseHandle(g_hTrnSemaphore);
		CloseHandle(g_hCvalSemaphore);
		return GetLastError();
	}

	return RET_OK;
}

//
//Снятие регистрации сервиса DDE и не только
//
void UnInit_Dde()
{
	DdeFreeStringHandle(g_dwIdInst, g_hszServName);
	DdeNameService(g_dwIdInst, 0, 0, DNS_UNREGISTER);
	DdeUninitialize(g_dwIdInst);
	CloseHandle(g_hTrnSemaphore);
	CloseHandle(g_hCvalSemaphore);
	CloseHandle(g_hEventThreads);
}

//
//Инициализация критических секций
//
void Init_CrSecDDE()
{
	InitializeCriticalSection(&CrThreadXlTableParse);
	InitializeCriticalSection(&CrWriteLogDDE);
}

//
//Удаление критических секций
//
void UnInit_CrSecDDE()
{
	DeleteCriticalSection(&CrThreadXlTableParse);
	DeleteCriticalSection(&CrWriteLogDDE);
}

//
//Упрощенная запись в лог
//
DWORD WriteLog_DDE(char *chStr, int iErrCode)
{
	DWORD dwRet;
	char *chBuf = NULL;
	UINT uLen;

	if (!strncmp(chStr, "[DBG]", 5) && g_chDebugInfo != 'Y') {
		return RET_OK;
	}

	uLen = (UINT) strlen(chStr) + NUM_LEN;
	
	chBuf = (char *) malloc(uLen + 1);
	if (chBuf == NULL) return ERR_MALLOC;	
	memset(chBuf, 0, uLen + 1);

	sprintf_s(chBuf, uLen + 1, "%s%s%d", chStr, " : ", iErrCode);
	uLen = (UINT) strlen(chBuf);

	EnterCriticalSection(&CrWriteLogDDE);
	dwRet = WriteLog(LOG_FILE_NAME, (LPCVOID) chBuf, uLen);	
	LeaveCriticalSection(&CrWriteLogDDE);

	Mem_Free(&chBuf);

	return dwRet;
}

//EOF