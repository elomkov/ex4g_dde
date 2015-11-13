/*
    Работа над ошибками...
	dbg_info.c

	Author: MiDi$
*/

//#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <windows.h>
#include <Dbghelp.h>
#include <Shlwapi.h>
#include "proto.h"

CRITICAL_SECTION CrPrintCallStack;
CRITICAL_SECTION CrUnhandledException;

//
//Запись стека вызовов (требует отладочные символа)
//
void PrintCallStack()
{
	USHORT i;
	size_t tmpLen = 0;
	USHORT uFramesNum = 0;
	HANDLE hProcess = GetCurrentProcess();
	char *chSymbFindPath = NULL;
	PVOID pvBackTraceAddr[MAX_FRAMES] = {0};
	SYMBOL_INFO *symbInfo = NULL;
	size_t symbLen = 0;
	char chBuf[MAX_PATH] = {0};

	__try {

	chSymbFindPath = (char *) malloc(MAX_PATH);
	if (chSymbFindPath == NULL) {
		return;
	}
	memset(chSymbFindPath, 0, MAX_PATH);

	if (GetCurrentDirectory(MAX_PATH, chSymbFindPath) == 0) {
		if (chSymbFindPath) free(chSymbFindPath);
		return;
	}
	tmpLen = strlen(chSymbFindPath);
	sprintf_s(&chSymbFindPath[tmpLen], MAX_PATH - tmpLen, "%s%s", TEXT("\\"), SYMB_DIR);

	if (PathFileExists(chSymbFindPath) == FALSE) {
		if (chSymbFindPath) free(chSymbFindPath);
		chSymbFindPath = NULL;
	}

	if (SymInitialize(hProcess, chSymbFindPath, TRUE) == FALSE)  {
		if (chSymbFindPath) free(chSymbFindPath);
		return;
	}

	uFramesNum = CaptureStackBackTrace(0, MAX_FRAMES, pvBackTraceAddr, NULL);

	if (chSymbFindPath) free(chSymbFindPath);

	symbLen = sizeof(SYMBOL_INFO) + (MAX_PATH) * sizeof(TCHAR);
	symbInfo = (SYMBOL_INFO *) malloc(symbLen);
	if  (symbInfo == NULL) {
		return;
	}
	memset(symbInfo, 0, symbLen);

	symbInfo->MaxNameLen = MAX_PATH - 1;
	symbInfo->SizeOfStruct = sizeof(SYMBOL_INFO);

	for (i = 0; i < uFramesNum; i++) {
		memset(symbInfo->Name, 0, MAX_PATH);
		if (SymFromAddr(hProcess, (DWORD64) pvBackTraceAddr[i], 0, symbInfo) == FALSE) {
			continue;
		}
		sprintf_s(chBuf, MAX_PATH, "- %i: %s - 0x%0X", uFramesNum - i, symbInfo->Name, symbInfo->Address );
		WriteLog(LOG_FILE_NAME, (LPCVOID) chBuf, strlen(chBuf));
	}

	if (symbInfo) free(symbInfo);

	}
	__except (EXCEPTION_EXECUTE_HANDLER) {

	}
}

//
//Инициализация критических секций
//
void Init_CrDbgInfo()
{
	InitializeCriticalSection(&CrPrintCallStack);
	InitializeCriticalSection(&CrUnhandledException);
}

//
//Удаление критических секций
//
void UnInit_CrDbgInfo()
{
	DeleteCriticalSection(&CrPrintCallStack);
	DeleteCriticalSection(&CrUnhandledException);
}

//
//Запись стека вызовов -- атомарный вызов
//
void PrintCallStackAtomic()
{
	EnterCriticalSection(&CrPrintCallStack);
	PrintCallStack();
	LeaveCriticalSection(&CrPrintCallStack);
}

//
//Фильтр необработанных исключений
//
LONG WINAPI __UnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo)
{
	EnterCriticalSection(&CrUnhandledException);
	MessageBox(NULL, "Критическая ошибка работы сервера!\r\nРабота приложения будет завершена!\r\nДля изучения сложившейся проблемы просьба выслать лог файл работы программы!", "Внимание!", MB_ICONERROR);
	EnterCriticalSection(&CrUnhandledException);
	return EXCEPTION_EXECUTE_HANDLER;
}

//
//Запуск -- краш теста (исключительно для отладки SEH)
//
int test_crash()
{
	volatile int i = 1;
	volatile int j = 0;

	i = i / j;

	return 1;
}
