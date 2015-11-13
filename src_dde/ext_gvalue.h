/*
	ext_gvalue.h

	Author: MiDi$
*/

#ifndef EXT_GVALUE_H
#define EXT_GVALUE_H

#ifdef  __cplusplus
extern "C" {
#endif

//Потоки
	extern DWORD g_dwThreadsCntTrnMax;	//Макс. число потоков -- TRN
	extern LONG g_lThreadCount;			//Счетчик числа потоков
	extern BOOL g_bThreadExec;			//Проверка возможности запуска потоков
	extern char g_chMultiThread;		//Режим многопоточности: Y - ВКЛ. N - ВЫКЛ.

//Синхронизация
	extern HANDLE g_hEventThreads;		//Событие ожидающее завершения потоков

//ORA параметры
	extern DWORD g_dwOraConnTimeOut;		//Время ожидания в миллисекундах (1 сек. = 1000 миллисекунд)
	extern DWORD g_dwOraCntTranCommit;		//Количество записей в транзакции
	extern char g_chDdeImpTrn[MAX_PATH];	//Имя объекта TRN
	extern char g_chDdeImpCval[MAX_PATH];	//Имя объекта CVAL
	extern DWORD g_dwOraSessMax;			//Максимальное число сессий в пуле соединений
	extern char g_chOraEnable;				//Режим взаимодействия с Oracle: Y - ДА N - НЕТ

//Параметры
	extern DWORD g_dwCtxCntRows;			//Количество записей в контексте
	extern char g_chDdeImpTrnTxt[MAX_PATH];	//Вывод в TXT - TRN
	char g_chDebugInfo;						//Режим записи отладочной информации в лог: : Y - ДА N - НЕТ

//Plugins
	//--//
	extern char g_chDdeImpTrnPlugin[MAX_PATH];	//Имя объекта TRN
	extern char g_chPluginEnableTrn;			//Режим выгрузки TRN через плагин: Y - ВКЛ. N - ВЫКЛ.
	extern char g_chLibNameTrn[MAX_PATH];		//Имя подгружаемого модуля
	extern char g_chFuncNameTrn[MAX_PATH];		//Имя функции в подгружаемом модуле
	//--//

#ifdef  __cplusplus
}
#endif

#endif