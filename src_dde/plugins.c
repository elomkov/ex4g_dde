/*
	Поддержка плагинов
	plugins.c

	Author: MiDi$
*/

#include <windows.h>
#include "const.h"
#include "plugins.h"
#include "ext_gvalue.h"
#include "proto.h"

CRITICAL_SECTION CrPluginTrn;

//
//Запуск всех доступных плагинов
//
DWORD Init_Plugins()
{
	//Запуск TRN плагина
	if (g_chPluginEnableTrn == 'Y') {
		hModuleTrn = LoadLibrary(g_chLibNameTrn);
		if (hModuleTrn == NULL) {
			return GetLastError();
		}

		Imp_Trn_Plugin = (PFNIMPTRNPLUGIN) GetProcAddress(hModuleTrn, g_chFuncNameTrn);
		if (Imp_Trn_Plugin == NULL) {
			return GetLastError();
		}
	}

	return RET_OK;
}

//
//Выгрузка всех доступных плагинов
//
void Free_Plugins()
{
	if (g_chPluginEnableTrn == 'Y') {
		if (hModuleTrn != NULL) {
			FreeLibrary(hModuleTrn);
		}
	}
}

//
//Инициализация критических секций
//
void Init_CrPlugins()
{
	InitializeCriticalSection(&CrPluginTrn);
}

//
//Удаление критических секций
//
void UnInit_CrPlugins()
{
	DeleteCriticalSection(&CrPluginTrn);
}

//
//Импорт TRN через плагин -- wrapper
//
DWORD Imp_Trn_Plugin_Wrapper(void *ctx, void *pImpStruct, WORD w_ObjType, void *pRes1)
{
	DWORD dwRet;

	if (!Imp_Trn_Plugin) return ERR_FUNC_NOT_FIND;
	if (!ctx) return ERR_ARGS;

	__try {
	
	EnterCriticalSection(&CrPluginTrn);
	dwRet = Imp_Trn_Plugin(ctx, pImpStruct, w_ObjType, pRes1);
	LeaveCriticalSection(&CrPluginTrn);

	}
	__finally {
		if (AbnormalTermination()) {
			PrintCallStackAtomic();
		}
	}

	return dwRet;
}

//
//Импорт через плагины - универсальная
//
DWORD Imp_Plugin(void *ctx, void *pImpStruct, WORD w_ObjType, void *pRes1)
{
	DWORD dwRet = ERR_FUNC_NOT_FIND;

	switch (w_ObjType)
	{
		case DDE_PLUGIN_TRN_OK:
			dwRet = Imp_Trn_Plugin_Wrapper(ctx, pImpStruct, w_ObjType, pRes1);
			break;

		default: break;
	}

	return dwRet;
}

//EOF

