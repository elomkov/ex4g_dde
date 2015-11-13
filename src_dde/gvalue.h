/*
	gvalue.h

	Author: MiDi$
*/

#ifndef _GVALUE_H
#define _GVALUE_H

#include "const.h"

#ifdef  __cplusplus
extern "C" {
#endif

//Потоки
	DWORD g_dwThreadsCntTrnMax = 0; //Макс. число потоков -- TRN
	LONG g_lThreadCount = 0;		//Счетчик числа потоков
	BOOL g_bThreadExec = TRUE;		//Проверка возможности запуска потоков
	char g_chMultiThread = 'Y';		//Режим многопоточности: Y - ВКЛ. N - ВЫКЛ.

//Синхронизация
	HANDLE g_hEventThreads = NULL; //Событие ожидающее завершения потоков
	//HANDLE g_hEventPool[MAX_THREADS] = {0};

//ORA параметры
	DWORD g_dwOraConnTimeOut = 0;			//Время ожидания в миллисекундах (1 сек. = 1000 миллисекунд)
	DWORD g_dwOraCntTranCommit = 0;			//Количество записей в транзакции
	char g_chDdeImpTrn[MAX_PATH] = {0};		//Имя объекта TRN
	char g_chDdeImpCval[MAX_PATH] = {0};	//Имя объекта CVAL
	DWORD g_dwOraSessMax = 0;				//Максимальное число сессий в пуле соединений
	char g_chOraEnable = 'N';				//Режим взаимодействия с Oracle: Y - ДА N - НЕТ

//Параметры
	DWORD g_dwCtxCntRows = 0;				//Количество записей в контексте
	char g_chDdeImpTrnTxt[MAX_PATH] = {0};	//Вывод в TXT - TRN
	char g_chDebugInfo = 'N';				//Режим записи отладочной информации в лог: : Y - ДА N - НЕТ

//Plugins
	//--//
	char g_chDdeImpTrnPlugin[MAX_PATH] = {0};	//Имя объекта TRN
	char g_chPluginEnableTrn = 'N';				//Режим выгрузки TRN через плагин: Y - ВКЛ. N - ВЫКЛ.
	char g_chLibNameTrn[MAX_PATH] = {0};		//Имя подгружаемого модуля
	char g_chFuncNameTrn[MAX_PATH] = {0};		//Имя функции в подгружаемом модуле
	//--//

#ifdef  __cplusplus
}
#endif

#endif