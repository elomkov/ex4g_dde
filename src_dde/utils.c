/*
	Утилиты
	utils.c

	Author: MiDi$
*/

//#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <windows.h>
#include "const.h"
#include "proto.h"
#include "ext_gvalue.h"

//Шаблон импорта TRN
extern ROBOEX_QUIK_STRUCT roboex_quik_trn_template[DDE_TRN_SIZE];

//Шаблон импорта CVAL
extern ROBOEX_QUIK_STRUCT roboex_quik_cval_template[DDE_CVAL_SIZE];

//
//Получить имя ORA - объекта
//
DWORD GetOraObjName(char *chStr, char **ch_OraObjName)
{
    char *chOraObjName = NULL;
    char *chSt1 = NULL;
    char *chSt2 = NULL;
    size_t chStrLen;

    if (!chStr) return ERR_ARGS;

    chSt1 = strstr(chStr, TEXT("["));
    if (chSt1 == NULL) return ERR_ARGS;
    chSt2 = strstr(chStr, TEXT("]"));
    if (chSt2 == NULL || chSt2 < chSt1) return ERR_ARGS;

    chStrLen = strlen(chStr);
    chOraObjName = (char *) malloc(chStrLen + 1);
    if (chOraObjName == NULL) return ERR_MALLOC;
	memset(chOraObjName, 0, chStrLen + 1);

    strncpy_s(chOraObjName, chStrLen + 1, ++chSt1, chStrLen);
    chOraObjName[chSt2-chSt1] = '.';

    *ch_OraObjName = chOraObjName;

    return RET_OK;
}

//
//Проверка соответствия объекта
//
DWORD IsObjOk(char *chOraObj, WORD *wStatus)
{
	if (!chOraObj) return ERR_ARGS;

	if (!strcmp(chOraObj, g_chDdeImpTrn)) {
		*wStatus = DDE_TRN_OK;
		return RET_OK;
	}
	else if (!strcmp(chOraObj, g_chDdeImpCval)) {
		*wStatus = DDE_CVAL_OK;
		return RET_OK;
	}
	else if (!strcmp(chOraObj, g_chDdeImpTrnTxt)) {
		*wStatus = DDE_TXT_TRN_OK;
		return RET_OK;
	}
	else if (!strcmp(chOraObj, g_chDdeImpTrnPlugin)) {
		*wStatus = DDE_PLUGIN_TRN_OK;
		return RET_OK;
	}

	return ERR_ORA_STATUS;
}

//
//Проверка соответствия объекта и размерности таблиц
//
DWORD IsObjAndSizeOk(WORD w_ObjType, WORD wColNum)
{
	DWORD dwRet = ERR_ORA_STATUS;

	switch(w_ObjType)
	{
		case DDE_TRN_OK:
		case DDE_TXT_TRN_OK:
		case DDE_PLUGIN_TRN_OK:
			if (wColNum == DDE_TRN_SIZE) {
				dwRet = RET_OK;
			}
			break;

		case DDE_CVAL_OK:
			if (wColNum == DDE_CVAL_SIZE) {
				dwRet = RET_OK;
			}
			break;
	}

	return dwRet;
}

//
//Запись в лог
//
DWORD WriteLog(LPCTSTR lp_FileName, LPCVOID lp_Buffer, DWORD dw_Len)
{
	DWORD dwRet;
	BOOL bRet;
	HANDLE hCreateFile;
	DWORD dwNumBytesWritten;

	if (!lp_FileName || !lp_Buffer || dw_Len == 0) return ERR_ARGS;

	hCreateFile = CreateFile(lp_FileName, /*GENERIC_READ |*/ GENERIC_WRITE, FILE_SHARE_READ /*| FILE_SHARE_WRITE*/, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hCreateFile == INVALID_HANDLE_VALUE) {
		return GetLastError();
	}

	dwRet = SetFilePointer(hCreateFile, 0, NULL, FILE_END);
	if (dwRet == INVALID_SET_FILE_POINTER) {
		CloseHandle(hCreateFile);
		return GetLastError();
	}

	bRet = WriteFile(hCreateFile, lp_Buffer, dw_Len, &dwNumBytesWritten, NULL);
	if (bRet == FALSE) {
		CloseHandle(hCreateFile);
		return GetLastError();
	}

	bRet = WriteFile(hCreateFile, "\r\n", (DWORD) strlen("\r\n"), &dwNumBytesWritten, NULL);
	if (bRet == FALSE) {
		CloseHandle(hCreateFile);
		return GetLastError();
	}

	CloseHandle(hCreateFile);

	return RET_OK;
}

//
//Освобождение памяти
//
void Mem_Free(void **hMem)
{
	__try {
		if(*hMem) free(*hMem);
		*hMem = NULL;	
	}
	__finally {
		if (AbnormalTermination()) {
			PrintCallStackAtomic();
		}
	}
}

//
//Замена разделителя целой и дробной части на системный
//
void ReplaceSDecimal(char *chStr)
{
#define LEN 3
	int i = 0;
	char chRealDecimal[LEN + 1] = {0};
	char chReplaceDecimal[LEN + 1] = {0};

	if (chStr) {
		if (GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, chRealDecimal, LEN)) {
			if (chRealDecimal[0] == '.') {
				chReplaceDecimal[0] = ',';
			}
			else chReplaceDecimal[0] = '.'; 
		}
	}

	if (chReplaceDecimal) {
		while (chStr[i]) {
			if (chStr[i] == chReplaceDecimal[0]) {
				chStr[i] = chRealDecimal[0];
			}
			i++;
		}
	}
}

//
//Обработка оконных сообщений
//
LRESULT CALLBACK WinStealthProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRet = RET_OK;

	switch (uMsg) {
		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		case WM_SETTEXT:
			if (!strcmp((char *)lParam, CMD_EXIT)) {
				printf("[INF] Wait for all threads exit...\n");
				WriteLog_DDE("[INF] Wait for all threads exit", 0);
				StopNewThreadExec(); //Отстанов запуска новых потоков
				if (g_lThreadCount != 0) {
					WaitForSingleObject(g_hEventThreads, INFINITE); //Ожидание завершения работающих потоков
				}
				PostQuitMessage(0);
			}
			break;

		default:
			lRet = DefWindowProc(hWnd, uMsg, wParam, lParam);
			break;
	}

	return lRet;
}

//
//Создание скрытого окна со свойством обработки только сообщений (поддержка DDE в консоли)
//
DWORD CreateStealthWin(HWND *h_Wnd)
{
	HINSTANCE hInst;
	WNDCLASS wc = {0};
	HWND hWnd;

	hInst = GetModuleHandle(NULL);
	if (hInst == NULL) {
		return GetLastError();
	}

	wc.style = 0;
	wc.lpfnWndProc = WinStealthProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInst;
	wc.hIcon = NULL;
	wc.hCursor = NULL;
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "StealthWinClass";

	if (RegisterClass(&wc) == 0) {
		return GetLastError();
	}

	hWnd =  CreateWindowEx(0, "StealthWinClass", "StealthWin", 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, hInst, NULL);
	if (hWnd == NULL) {
		UnregisterClass("StealthWinClass", hInst);
		return GetLastError();
	}

	*h_Wnd = hWnd;

	return RET_OK;
}

//
//Обработчик командной строки
//
DWORD WINAPI MakeCmd(LPVOID lpBuf)
{
	char chCmd[CMD_LEN + 1];

	do {
		memset(chCmd, 0, CMD_LEN + 1);
		printf("%s", "cmd>");
		fgets(chCmd, CMD_LEN, stdin);
		fseek(stdin, SEEK_END, SEEK_SET);

		if (!strcmp(chCmd, CMD_EXIT)) {
			SendMessage((HWND) lpBuf, WM_SETTEXT, 0, (LPARAM) CMD_EXIT);
			break;
		}
		else if (!strcmp(chCmd, CMD_HELP)) {
			printf("%s\n", "exit - program exit");
		}
		else {
			printf("%s%s\n", chCmd, " - unknown command");
			printf("%s\n", "help - for more information");
		}
	} while(TRUE);

	return RET_OK;
}

//
//Функция чтения данных из ini файла
//
DWORD ReadIniFileStr(LPCTSTR lpFileName, LPCTSTR lpAppName, LPCTSTR lpKeyName, char **lpRet_Str)
{

	DWORD dwRet = 0;	
	DWORD nSize = INI_LEN;
	char *lpRetStr = NULL;	

	if (!lpAppName || !lpKeyName || !lpFileName) return ERR_ARGS;

	lpRetStr = (char *) malloc(nSize + 1);
	if (lpRetStr == NULL) return ERR_MALLOC;
	memset(lpRetStr, 0, nSize + 1);

    dwRet = GetPrivateProfileString(lpAppName, lpKeyName, NULL, lpRetStr, nSize, lpFileName);
	if (dwRet == nSize - 1 || dwRet == nSize - 2) {		
		Mem_Free((void **) &lpRetStr);
		return GetLastError();
	}
//Несмотря на заявление MSDN к GetPrivateProfileString(...): 
//In the event the initialization file specified by lpFileName is not found, or contains invalid values,
//this function will set errorno with a value of '0x2' (File Not Found). To retrieve extended error information,
//call GetLastError.
//Как минимум для XP -- высказывание не справедливо, потому отдельная обработка, WIN7 -- Ok.
	else if (strcmp(lpRetStr, TEXT("")) == 0) {
		Mem_Free((void **) &lpRetStr);
		return ERR_INI_VALUE;
	}

	*lpRet_Str = lpRetStr;

	return RET_OK;
}

//
//Инициализация параметров из INI файла
//
DWORD InitIniValues()
{
	DWORD dwRet;
	char *chIniBuf = NULL;
	char chIniPath[MAX_PATH] = {0};
	int i;

	dwRet = GetCurrentDirectory(MAX_PATH, chIniPath);
	if (dwRet == 0){
	  return GetLastError();
	}
	strcat_s(chIniPath, MAX_PATH, TEXT("\\"));
	strcat_s(chIniPath, MAX_PATH, INI_FILE_NAME);

//Инициализация глобальных переменных

	dwRet = ReadIniFileStr(chIniPath, "Ora_Pref", "ORA_ENABLE", &chIniBuf);
	if (dwRet != RET_OK) {
		Mem_Free((void **) &chIniBuf);
		return dwRet;
	}
	g_chOraEnable = chIniBuf[0];
	Mem_Free((void **) &chIniBuf);

	if (g_chOraEnable == 'Y') {

	dwRet = ReadIniFileStr(chIniPath, "Ora_Pref", "ORA_CONN_TIMEOUT", &chIniBuf);
	if (dwRet != RET_OK) {
		Mem_Free((void **) &chIniBuf);
		return dwRet;
	}
	g_dwOraConnTimeOut = atoi(chIniBuf);
	Mem_Free((void **) &chIniBuf);

	dwRet = ReadIniFileStr(chIniPath, "Ora_Pref", "ORA_CNT_TRAN_COMMIT", &chIniBuf);
	if (dwRet != RET_OK) {
		Mem_Free((void **) &chIniBuf);
		return dwRet;
	}
	g_dwOraCntTranCommit = atoi(chIniBuf);
	Mem_Free((void **) &chIniBuf);

	}

	dwRet = ReadIniFileStr(chIniPath, "Pref", "CTX_CNT_ROWS", &chIniBuf);
	if (dwRet != RET_OK) {
		Mem_Free((void **) &chIniBuf);
		return dwRet;
	}
	g_dwCtxCntRows = atoi(chIniBuf);
	Mem_Free((void **) &chIniBuf);
	if (g_dwCtxCntRows > WORD_DIM) {
		return ERR_MAX_DIM;
	}

	dwRet = ReadIniFileStr(chIniPath, "Pref", "DEBUG_INFO", &chIniBuf);
	if (dwRet != RET_OK) {
		Mem_Free((void **) &chIniBuf);
		return dwRet;
	}
	g_chDebugInfo = chIniBuf[0];
	Mem_Free((void **) &chIniBuf);

	dwRet = ReadIniFileStr(chIniPath, "Threads", "THREADS_CNT_TRN_MAX", &chIniBuf);
	if (dwRet != RET_OK) {
		Mem_Free((void **) &chIniBuf);
		return dwRet;
	}
	g_dwThreadsCntTrnMax = atoi(chIniBuf);
	Mem_Free((void **) &chIniBuf);

	dwRet = ReadIniFileStr(chIniPath, "Threads", "MULTI_THREADS", &chIniBuf);
	if (dwRet != RET_OK) {
		Mem_Free((void **) &chIniBuf);
		return dwRet;
	}
	g_chMultiThread = chIniBuf[0];
	Mem_Free((void **) &chIniBuf);

	g_dwOraSessMax = g_dwThreadsCntTrnMax + CVAL_MAX_THREADS; //Количество сессий в пуле ORA соединений
	if (g_dwOraSessMax > MAX_THREADS) {
		return ERR_MAX_THREADS;
	}

	dwRet = ReadIniFileStr(chIniPath, "Plugin_Trn", "PLUGIN_TRN_ENABLE", &chIniBuf);
	if (dwRet != RET_OK) {
		Mem_Free((void **) &chIniBuf);
		return dwRet;
	}
	g_chPluginEnableTrn = chIniBuf[0];
	Mem_Free((void **) &chIniBuf);

	if (g_chPluginEnableTrn == 'Y') {
		dwRet = ReadIniFileStr(chIniPath, "Plugin_Trn", "PLUGIN_LIB_NAME_TRN", &chIniBuf);
		if (dwRet != RET_OK) {
			Mem_Free((void **) &chIniBuf);
			return dwRet;
		}
		memset(g_chLibNameTrn, 0, MAX_PATH);
		strncpy_s(g_chLibNameTrn, MAX_PATH, chIniBuf, MAX_PATH);
		Mem_Free((void **) &chIniBuf);

		dwRet = ReadIniFileStr(chIniPath, "Plugin_Trn", "PLUGIN_FUNC_NAME_TRN", &chIniBuf);
		if (dwRet != RET_OK) {
			Mem_Free((void **) &chIniBuf);
			return dwRet;
		}
		memset(g_chFuncNameTrn, 0, MAX_PATH);
		strncpy_s(g_chFuncNameTrn, MAX_PATH, chIniBuf, MAX_PATH);
		Mem_Free((void **) &chIniBuf);
	}

//Глобальные имена объектов - инициализация

	dwRet = ReadIniFileStr(chIniPath, "Ora_Obj_Trn", "TRN_OBJ_NAME", &chIniBuf);
	if (dwRet != RET_OK) {
		Mem_Free((void **) &chIniBuf);
		return dwRet;
	}
	memset(g_chDdeImpTrn, 0, MAX_PATH);
	strncpy_s(g_chDdeImpTrn, MAX_PATH, chIniBuf, MAX_PATH);
	Mem_Free((void **) &chIniBuf);

	dwRet = ReadIniFileStr(chIniPath, "Ora_Obj_Cval", "CVAL_OBJ_NAME", &chIniBuf);
	if (dwRet != RET_OK) {
		Mem_Free((void **) &chIniBuf);
		return dwRet;
	}
	memset(g_chDdeImpCval, 0, MAX_PATH);
	strncpy_s(g_chDdeImpCval, MAX_PATH, chIniBuf, MAX_PATH);
	Mem_Free((void **) &chIniBuf);

	dwRet = ReadIniFileStr(chIniPath, "Pref", "TXT_TRN_OBJ_NAME", &chIniBuf);
	if (dwRet != RET_OK) {
		Mem_Free((void **) &chIniBuf);
		return dwRet;
	}
	memset(g_chDdeImpTrnTxt, 0, MAX_PATH);
	strncpy_s(g_chDdeImpTrnTxt, MAX_PATH, chIniBuf, MAX_PATH);
	Mem_Free((void **) &chIniBuf);

	dwRet = ReadIniFileStr(chIniPath, "Plugin_Trn", "PLUGIN_TRN_OBJ_NAME", &chIniBuf);
	if (dwRet != RET_OK) {
		Mem_Free((void **) &chIniBuf);
		return dwRet;
	}
	memset(g_chDdeImpTrnPlugin, 0, MAX_PATH);
	strncpy_s(g_chDdeImpTrnPlugin, MAX_PATH, chIniBuf, MAX_PATH);
	Mem_Free((void **) &chIniBuf);

//Инициализация шаблона импорта TRN

	for (i = 0; i < DDE_TRN_SIZE; i++) {
		dwRet = ReadIniFileStr(chIniPath, "Ora_Obj_Trn", roboex_quik_trn_template[i].chQuikName, &chIniBuf);
		if (dwRet != RET_OK) {
			Mem_Free((void **) &chIniBuf);
		}
		else {			
			strncpy_s(roboex_quik_trn_template[i].chRoboexName, STRUCT_COL_SIZE, chIniBuf, STRUCT_COL_SIZE);			
		}
	}

//Инициализация шаблона импорта CVAL

	for (i = 0; i < DDE_CVAL_SIZE; i++) {
		dwRet = ReadIniFileStr(chIniPath, "Ora_Obj_Cval", roboex_quik_cval_template[i].chQuikName, &chIniBuf);
		if (dwRet != RET_OK) {
			Mem_Free((void **) &chIniBuf);
		}
		else {			
			strncpy_s(roboex_quik_cval_template[i].chRoboexName, STRUCT_COL_SIZE, chIniBuf, STRUCT_COL_SIZE);			
			Mem_Free((void **) &chIniBuf);
		}
	}

	return RET_OK;
}

//
//Инициализация всех доступных критических секций
//
void Init_CrSec()
{
	Init_CrSecDDE();
	Init_CrSecImp();
	Init_CrPlugins();
	Init_CrDbgInfo();	
}

//
//Удаление всех доступных критических секций
//
void UnInit_CrSec()
{
	UnInit_CrSecDDE();
	UnInit_CrSecImp();
	UnInit_CrPlugins();
	UnInit_CrDbgInfo();
}

//
//Запрет на выполнение "новых" потоков
//
void StopNewThreadExec()
{
	g_bThreadExec = FALSE;
}

//EOF

