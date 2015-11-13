/*
	ex4g_dde.c

	Project: RoboEx
	Program: DDE server for communication QUIK --> External Systems

	Author:
	MiDi$, 2012.02.09
	mail: roboex@mail.ru

	v 0.1.8
*/

//#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <windows.h>
#include <process.h>
#include "const.h"
#include "proto.h"
#include "gvalue.h"

#pragma comment(lib, "dbghelp.lib")
#pragma comment(lib, "Shlwapi.lib")

char chBanner[] = "\
##################################################\r\n\
# DDE Server for communication with Quik v 0.1.8 #\r\n\
# e-mail: roboex@mail.ru                         #\r\n\
##################################################\r\n";

int main(int argc, char **argv)
{
	DWORD dwRet = RET_OK;
	HWND hWnd = NULL;
	HANDLE hThread = NULL;
	MSG msg;
	char *chUserName = NULL;
	char *chPasswd = NULL;
	char *chAlias = NULL;

	//Флаги выполнения
	BOOL bIsPlugin = FALSE;
	BOOL bIsOraLib = FALSE;
	BOOL bIsInitDDE = FALSE;

	//Установка местного фильтра необрабатываемых исключений
	SetUnhandledExceptionFilter(__UnhandledExceptionFilter);

	__try {

	Init_CrSec();

	//Баннер
	printf("%s\n", chBanner);
	WriteLog(LOG_FILE_NAME, chBanner, (DWORD) strlen(chBanner));

	dwRet = CreateStealthWin(&hWnd);
	if (dwRet != RET_OK) {
		printf("[ERR] main(..) -> CreateStealthWin(..) : %d\n", dwRet);
		WriteLog_DDE("[ERR] main(..) -> CreateStealthWin(..)", dwRet);
		dwRet = 1;
		__leave;
	}

	dwRet = InitIniValues();
	if (dwRet != RET_OK) {
		printf("[ERR] main(..) -> InitIniValues(..) : %d\n", dwRet);
		WriteLog_DDE("[ERR] main(..) -> InitIniValues(..)", dwRet);		
		dwRet = 1;
		__leave;
	}

	dwRet = Init_Plugins();
	if (dwRet != RET_OK) {
		printf("[ERR] main(..) -> Init_Plugins(..) : %d\n", dwRet);
		WriteLog_DDE("[ERR] main(..) -> Init_Plugins(..)", dwRet);
		dwRet = 1;
		__leave;
	}
	bIsPlugin = TRUE;

	if (g_chOraEnable == 'Y') {
		if (argc != 4){//Проверка ORA - аргументов
			printf("%s\n", "Usage: ex4g_dde <username> <passwd> <alias>");
			WriteLog_DDE("Usage: ex4g_dde <username> <passwd> <alias>", 0);
			WriteLog_DDE("[ERR] main(..) -> argc", argc);			
			dwRet = 1;
			__leave;
		}
		chUserName = argv[1];
		chPasswd = argv[2];
		chAlias = argv[3];

		printf("[INF] InitOraConnect(..) : Try ORA Connect - Wait...\n");

		dwRet = Init_Ora_Lib();
		if (dwRet != RET_OK) {
			WriteLog_DDE("[ERR] main(..) -> Init_Ora_Lib(..)", dwRet);
			dwRet = 1;
			__leave;
		}
		bIsOraLib = TRUE;		

		dwRet = InitOraConnect(chUserName, chPasswd, chAlias, g_dwOraSessMax);
		if (dwRet != RET_OK) {
			WriteLog_DDE("[ERR] main(..) -> InitOraConnect(..)", dwRet);
			dwRet = 1;
			__leave;
		}

		printf("[OK] InitOraConnect(..) : %d\n", dwRet);
		WriteLog_DDE("[OK] main(..) -> InitOraConnect(..)", dwRet);
	}

	dwRet = Init_Dde();
	if (dwRet != RET_OK){
		printf("[ERR] Init_Dde(..) : %u\n", dwRet);
		WriteLog_DDE("[ERR] main(..) -> Init_Dde(..)", dwRet);
		dwRet = 1;
		__leave;
	}
	bIsInitDDE = TRUE;

	hThread = (HANDLE) _beginthreadex(NULL, 0, MakeCmd, hWnd, 0, NULL);			
	if (hThread == NULL) {
		WriteLog_DDE("[ERR] main(..) -> MakeCmd", GetLastError());
		dwRet = 1;		
		__leave;
	}

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
    }
	
	}
	__finally {

		if (AbnormalTermination()) {
			PrintCallStackAtomic();
		}

		if (bIsInitDDE) {
			printf("[OK] UnInit_Dde(..) : %d\n", 0);
			WriteLog_DDE("[OK] main(..) -> UnInit_Dde(..)", 0);
			UnInit_Dde();
		}
		if (bIsOraLib) {
			printf("[OK] CloseOraConnect(..) : %d\n", 0);
			WriteLog_DDE("[OK] main(..) -> CloseOraConnect(..)", 0);
			CloseOraConnect();
			Free_Ora_Lib();			
		}
		if (bIsPlugin) {
			printf("[OK] Free_Plugins(..) : %d\n", 0);
			WriteLog_DDE("[OK] main(..) -> Free_Plugins(..)", 0);
			Free_Plugins();
		}
		CloseHandle(hThread);
		UnInit_CrSec();

	}

	return dwRet;
}

//EOF
