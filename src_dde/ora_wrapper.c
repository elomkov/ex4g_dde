/*
	Переходники на ORA функции
	ora_wrapper.c

	Author: MiDi$
*/

#include <windows.h>
#include "struct.h"
#include "ext_gvalue.h"
#include "struct.h"
#include "ora_wrapper.h"
#include "proto.h"

//
//Запуск Ora Lib
//
DWORD Init_Ora_Lib()
{
	if (g_chOraEnable == 'Y') {
		hModuleOra = LoadLibrary(TEXT("imp_ora.dll"));
		if (hModuleOra == NULL) {
			return GetLastError();
		}

		_InitOraConnect = (PFN_INITORACONNECT) GetProcAddress(hModuleOra, TEXT("_InitOraConnect"));
		if (_InitOraConnect == NULL) {
			return GetLastError();
		}

		_CloseOraConnect = (PFN_CLOSEORACONNECT) GetProcAddress(hModuleOra, TEXT("_CloseOraConnect"));
		if (_CloseOraConnect == NULL) {
			return GetLastError();
		}

		_GetDbSessionFromPool = (PFN_GETBBSESSIONFROMPOOL) GetProcAddress(hModuleOra, TEXT("_GetDbSessionFromPool"));
		if (_GetDbSessionFromPool == NULL) {
			return GetLastError();
		}

		_RemoveDbSessionFromPool = (PFN_REMOVEDBSESSIONFROMPOOL) GetProcAddress(hModuleOra, TEXT("_RemoveDbSessionFromPool"));
		if (_RemoveDbSessionFromPool == NULL) {
			return GetLastError();
		}

		_BeginTransaction = (PFN_BEGINTRANSACTION) GetProcAddress(hModuleOra, TEXT("_BeginTransaction"));
		if (_BeginTransaction == NULL) {
			return GetLastError();
		}

		_CommitTransaction = (PFN_COMMITTRANSACTION) GetProcAddress(hModuleOra, TEXT("_CommitTransaction"));
		if (_CommitTransaction == NULL) {
			return GetLastError();
		}

		_Imp_Trn = (PFN_IMP_TRN) GetProcAddress(hModuleOra, TEXT("_Imp_Trn"));
		if (_Imp_Trn == NULL) {
			return GetLastError();
		}

		_Imp_Cval = (PFN_IMP_CVAL) GetProcAddress(hModuleOra, TEXT("_Imp_Cval"));
		if (_Imp_Cval == NULL) {
			return GetLastError();
		}
	}

	return RET_OK;
}

//
//Выгрузка Ora Lib
//
void Free_Ora_Lib()
{
	if (g_chOraEnable == 'Y') {
		if (hModuleOra != NULL) {
			FreeLibrary(hModuleOra);
		}
	}
}

//
//Ф-ия инициализации коннекта -- wrapper
//
DWORD InitOraConnect(char *chUserName, char *chPasswd, char *chInstance, DWORD dw_OraSessMax)
{
	DWORD dwRet;

	__try {

	dwRet = _InitOraConnect(chUserName, chPasswd, chInstance, dw_OraSessMax);

	}
	__finally {
		if (AbnormalTermination()) {
			PrintCallStackAtomic();
		}
	}

	return dwRet;
}

//
//Закрытие коннекта -- wrapper
//
void CloseOraConnect()
{
	__try {

	_CloseOraConnect();	

	}
	__finally {
		if (AbnormalTermination()) {
			PrintCallStackAtomic();
		}
	}
}

//
//Получить индекс сессии из пула соединений -- wrapper
//
DWORD GetDbSessionFromPool(WORD *w_idx, DWORD dw_OraConnTimeOut)
{
	DWORD dwRet;

	__try {

	dwRet = _GetDbSessionFromPool(w_idx, dw_OraConnTimeOut);

	}
	__finally {
		if (AbnormalTermination()) {
			PrintCallStackAtomic();
		}
	}

	return dwRet;
}

//
//Очистить сессию из пула -- wrapper
//
void RemoveDbSessionFromPool(WORD w_idx)
{
	__try {

	_RemoveDbSessionFromPool(w_idx);

	}
	__finally {
		if (AbnormalTermination()) {
			PrintCallStackAtomic();
		}
	}
}

//
//Начать транзакцию -- wrapper
//
void BeginTransaction(WORD w_idx)
{
	__try {

	_BeginTransaction(w_idx);

	}
	__finally {
		if (AbnormalTermination()) {
			PrintCallStackAtomic();
		}
	}
}

//
//Зафиксировать транзакцию -- wrapper
//
void CommitTransaction(WORD w_idx)
{
	__try {

	_CommitTransaction(w_idx);

	}
	__finally {
		if (AbnormalTermination()) {
			PrintCallStackAtomic();
		}
	}
}

//
//Отправка транзакции в TRN -- wrapper
//
DWORD Imp_Trn(TRN_OUT_CTX *trn_out_ctx, WORD w_idx, char *ch_DdeImpTrn)
{
	DWORD dwRet;

	__try {

	dwRet = _Imp_Trn(trn_out_ctx, w_idx, ch_DdeImpTrn);

	}
	__finally {
		if (AbnormalTermination()) {
			PrintCallStackAtomic();
		}
	}

	return dwRet;
}

//
//Отправка транзакции в CVAL -- wrapper
//
DWORD Imp_Cval(CVAL_OUT_CTX *cval_out_ctx, WORD w_idx, char *ch_DdeImpCval)
{
	DWORD dwRet;

	__try {
	
	dwRet = _Imp_Cval(cval_out_ctx, w_idx, ch_DdeImpCval);

	}
	__finally {
		if (AbnormalTermination()) {
			PrintCallStackAtomic();
		}
	}

	return dwRet;
}


//EOF