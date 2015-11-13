/*
	Отправка в Oracle -- oo4o
	oo4o.cpp

	Author: MiDi$
*/

#define _CRT_SECURE_NO_WARNINGS

#define LEN_1 8 //strlen(" => :, ")
#define LEN_2 15 //strlen("BEGIN (); END;")
#define LEN_3 32 //Ora object name len

#include <stdio.h>
#include "oracl.h"
#include "const.h"
#include "struct.h"

/*
OTYPE_VARCHAR2	Varchar2, variable length character.
OTYPE_NUMBER	Numeric field.
OTYPE_LONG	Long text (> 2000 bytes).
OTYPE_ROWID	Oracle rowid.
OTYPE_DATE	A date.
OTYPE_RAW	Raw bytes.
OTYPE_LONGRAW	Long blob of bytes (generally > 255 bytes).
OTYPE_CHAR	Fixed-length text.
OTYPE_MSLABEL	Special type for Trusted Oracle.
OTYPE_CURSOR	PL/SQL cursor type
OTYPE_OBJECT	Object
OTYPE_REF	Ref
OTYPE_COLL	Collection
OTYPE_BLOB	Blob
OTYPE_CLOB	Clob
OTYPE_BFILE	Bfile
OTYPE_VARRAY	VARRAY
OTYPE_TABLE	Table
*/

typedef struct
{
	ODatabase odb[MAX_THREADS];
	BYTE b_idx[MAX_THREADS]; //Массив состояний: 0 -- свободен; 1 -- занят
} DB_POOL_CTX;

DB_POOL_CTX db_pool_ctx;

CRITICAL_SECTION CrSecDbPool;

OSession osess;

char *UserNameOra = NULL;
char *PasswdOra = NULL;
char *InstanceOra = NULL;

//
//Освобождение памяти
//
void Mem_Free(void **hMem)
{		
	if(*hMem) free(*hMem);
	*hMem = NULL;	
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
//Ф-ия инициализации коннекта
//
DWORD WINAPI _InitOraConnect(char *chUserName, char *chPasswd, char *chInstance, DWORD dw_OraSessMax)
{
	if (!chUserName || !chPasswd || !chInstance) return ERR_ARGS;

	if (!OStartup(OSTARTUP_MULTITHREADED)) {
		return ERR_OO4O_INIT;
	}

	osess.Open();
	if (!osess.IsOpen()) {	
		//printf("Database not opened: %s", osess.GetErrorText());
		osess.Close();
		OShutdown();
		return ERR_ORA_CONNECT;
	}

	//Инициализация пула коннектов	
	if (osess.CreateDatabasePool(dw_OraSessMax, dw_OraSessMax, 0, chInstance, chUserName, chPasswd, ODATABASE_DEFAULT) != OSUCCESS) {
		osess.Close();
		OShutdown();
		return ERR_OO4O_POOL;
	}

	UserNameOra = (char *) malloc(strlen(chUserName) + 1);
	if (UserNameOra == NULL){
		osess.Close();
		OShutdown();
		return ERR_MALLOC;
	}

	strcpy(UserNameOra, chUserName); 

	PasswdOra = (char *) malloc(strlen(chPasswd) + 1);
	if (PasswdOra == NULL){
		osess.Close();
		OShutdown();
		Mem_Free((void **) &UserNameOra);
		return ERR_MALLOC;
	}

	strcpy(PasswdOra, chPasswd);

	InstanceOra = (char *) malloc(strlen(chInstance) + 1);
	if (InstanceOra == NULL){
		osess.Close();
		OShutdown();
		Mem_Free((void **) &UserNameOra);
		Mem_Free((void **) &PasswdOra);
		return ERR_MALLOC;
	}

	strcpy(InstanceOra, chInstance);

	for (int i = 0; i < MAX_THREADS; i++) {
		db_pool_ctx.b_idx[i] = 0;
	}

	InitializeCriticalSection(&CrSecDbPool);

	return RET_OK;
}

//
//Закрытие коннекта
//
void WINAPI _CloseOraConnect()
{
	osess.Close();
	OShutdown();  
	Mem_Free((void **) &UserNameOra);
	Mem_Free((void **) &PasswdOra);
	Mem_Free((void **) &InstanceOra);

	DeleteCriticalSection(&CrSecDbPool);	
}

//
//Получить индекс сессии из пула соединений
//
DWORD WINAPI _GetDbSessionFromPool(WORD *w_idx, DWORD dw_OraConnTimeOut)
{
	int idx = -1;

/*
	if (!OStartup(OSTARTUP_MULTITHREADED)) {
		return ERR_OO4O_INIT;
	}
*/

	EnterCriticalSection(&CrSecDbPool);

	for (int i = 0; i < MAX_THREADS; i++) {
		if (db_pool_ctx.b_idx[i] == 0) {
			idx = i;
			break;		
		}
	}

	if (idx == -1) {
		LeaveCriticalSection(&CrSecDbPool);
		return ERR_DB_POOL_IDX;
	}

	db_pool_ctx.odb[idx] = osess.GetDatabaseFromPool(dw_OraConnTimeOut);
	if (!db_pool_ctx.odb[idx].IsOpen()) {
		LeaveCriticalSection(&CrSecDbPool);
		db_pool_ctx.odb[idx].Close();
		return ERR_ORA_CONNECT;
	}

	db_pool_ctx.b_idx[idx] = 1;

	LeaveCriticalSection(&CrSecDbPool);

	*w_idx = idx;

	return RET_OK;
}

//
//Очистить сессию из пула
//
void WINAPI _RemoveDbSessionFromPool(WORD w_idx)
{
	EnterCriticalSection(&CrSecDbPool);

	db_pool_ctx.odb[w_idx].RemoveFromPool();
	db_pool_ctx.odb[w_idx].Close();
	db_pool_ctx.b_idx[w_idx] = 0;

	LeaveCriticalSection(&CrSecDbPool);

	//OShutdown();
}

//
//Начать транзакцию
//
void WINAPI _BeginTransaction(WORD w_idx)
{
	if (db_pool_ctx.odb[w_idx].IsOpen()) {
		db_pool_ctx.odb[w_idx].BeginTrans();
	}
}

//
//Зафиксировать транзакцию
//
void WINAPI _CommitTransaction(WORD w_idx)
{
	if (db_pool_ctx.odb[w_idx].IsOpen()) {
		db_pool_ctx.odb[w_idx].CommitTrans();
	}
}

//
//Отправка транзакции в TRN
//
DWORD WINAPI _Imp_Trn(TRN_OUT_CTX *trn_out_ctx, WORD w_idx, char *ch_DdeImpTrn)
{
	WORD i, j;
	char chExecSqlBuf[(STRUCT_COL_SIZE * 2 + LEN_1) * DDE_TRN_SIZE + LEN_3 + LEN_2 + 1];

	if (!trn_out_ctx) return ERR_ARGS;

	OParameterCollection params = db_pool_ctx.odb[w_idx].GetParameters();

	for (i = 0; i < trn_out_ctx->ctx_idx; i++) {
		memset(chExecSqlBuf, 0, (STRUCT_COL_SIZE * 2 + LEN_1) * DDE_TRN_SIZE + LEN_3 + LEN_2 + 1);
		sprintf(chExecSqlBuf, "%s%s%s", "BEGIN ", ch_DdeImpTrn, "(");
		for (j = 0; j < DDE_TRN_SIZE; j++) {

			if (!trn_out_ctx->roboex_quik_st[i][j].chRoboexName || strlen(trn_out_ctx->roboex_quik_st[i][j].chRoboexName) == 0) {
				continue;
			}
			
			if (!strcmp(trn_out_ctx->roboex_quik_st[i][j].chQuikName, "TRADENUM")) {
				ReplaceSDecimal(trn_out_ctx->roboex_quik_st[i][j].chValue);
				OParameter i_TrnNum = params.Add(trn_out_ctx->roboex_quik_st[i][j].chRoboexName, trn_out_ctx->roboex_quik_st[i][j].chValue, OPARAMETER_INVAR, OTYPE_NUMBER);
			}
			else if (!strcmp(trn_out_ctx->roboex_quik_st[i][j].chQuikName, "TRADEDATE")) {
				OParameter d_TrnDate = params.Add(trn_out_ctx->roboex_quik_st[i][j].chRoboexName, trn_out_ctx->roboex_quik_st[i][j].chValue, OPARAMETER_INVAR, OTYPE_DATE);
			}
			else if (!strcmp(trn_out_ctx->roboex_quik_st[i][j].chQuikName, "TRADETIME")) {
				OParameter c_TrnTime = params.Add(trn_out_ctx->roboex_quik_st[i][j].chRoboexName, trn_out_ctx->roboex_quik_st[i][j].chValue, OPARAMETER_INVAR, OTYPE_VARCHAR2);
			}
			else if (!strcmp(trn_out_ctx->roboex_quik_st[i][j].chQuikName, "SECURITY_SHORT_NAME")) {
				OParameter c_PaperSocr = params.Add(trn_out_ctx->roboex_quik_st[i][j].chRoboexName, trn_out_ctx->roboex_quik_st[i][j].chValue, OPARAMETER_INVAR, OTYPE_VARCHAR2);
			}
			else if (!strcmp(trn_out_ctx->roboex_quik_st[i][j].chQuikName, "SECURITY_NAME")) {
				OParameter c_Paper = params.Add(trn_out_ctx->roboex_quik_st[i][j].chRoboexName, trn_out_ctx->roboex_quik_st[i][j].chValue, OPARAMETER_INVAR, OTYPE_VARCHAR2);
			}
			else if (!strcmp(trn_out_ctx->roboex_quik_st[i][j].chQuikName, "SECCODE")) {
				OParameter c_PaperCode = params.Add(trn_out_ctx->roboex_quik_st[i][j].chRoboexName, trn_out_ctx->roboex_quik_st[i][j].chValue, OPARAMETER_INVAR, OTYPE_VARCHAR2);
			}
			else if (!strcmp(trn_out_ctx->roboex_quik_st[i][j].chQuikName, "CLASS_NAME")) {
				OParameter c_Class = params.Add(trn_out_ctx->roboex_quik_st[i][j].chRoboexName, trn_out_ctx->roboex_quik_st[i][j].chValue, OPARAMETER_INVAR, OTYPE_VARCHAR2);
			}
			else if (!strcmp(trn_out_ctx->roboex_quik_st[i][j].chQuikName, "CLASSCODE")) {
				OParameter c_ClassCode = params.Add(trn_out_ctx->roboex_quik_st[i][j].chRoboexName, trn_out_ctx->roboex_quik_st[i][j].chValue, OPARAMETER_INVAR, OTYPE_VARCHAR2);
			}
			else if (!strcmp(trn_out_ctx->roboex_quik_st[i][j].chQuikName, "PRICE")) {
				ReplaceSDecimal(trn_out_ctx->roboex_quik_st[i][j].chValue);
				OParameter n_Price = params.Add(trn_out_ctx->roboex_quik_st[i][j].chRoboexName, trn_out_ctx->roboex_quik_st[i][j].chValue, OPARAMETER_INVAR, OTYPE_NUMBER);
			}
			else if (!strcmp(trn_out_ctx->roboex_quik_st[i][j].chQuikName, "QTY")) {
				ReplaceSDecimal(trn_out_ctx->roboex_quik_st[i][j].chValue);
				OParameter i_Quantity = params.Add(trn_out_ctx->roboex_quik_st[i][j].chRoboexName, trn_out_ctx->roboex_quik_st[i][j].chValue, OPARAMETER_INVAR, OTYPE_NUMBER);
			}
			else if (!strcmp(trn_out_ctx->roboex_quik_st[i][j].chQuikName, "VALUE")) {
				ReplaceSDecimal(trn_out_ctx->roboex_quik_st[i][j].chValue);
				OParameter n_Volume = params.Add(trn_out_ctx->roboex_quik_st[i][j].chRoboexName, trn_out_ctx->roboex_quik_st[i][j].chValue, OPARAMETER_INVAR, OTYPE_NUMBER);
			}

			strcat(chExecSqlBuf, trn_out_ctx->roboex_quik_st[i][j].chRoboexName);
			strcat(chExecSqlBuf, " => :");
			strcat(chExecSqlBuf, trn_out_ctx->roboex_quik_st[i][j].chRoboexName);
			strcat(chExecSqlBuf, ", ");
		}

		sprintf(&chExecSqlBuf[strlen(chExecSqlBuf) - 2], "); END;");

		oresult r = db_pool_ctx.odb[w_idx].ExecuteSQL(chExecSqlBuf);
		if (r != OSUCCESS) {				
			//WriteLog_DDE("[ERR] Imp_Trn(..)", db_pool_ctx.odb[w_idx].ServerErrorNumber());	
			;
		}

		for (j = 0; j < DDE_TRN_SIZE; j++) {
			if (!trn_out_ctx->roboex_quik_st[i][j].chRoboexName || strlen(trn_out_ctx->roboex_quik_st[i][j].chRoboexName) == 0) {
				continue;
			}

			params.Remove(trn_out_ctx->roboex_quik_st[i][j].chRoboexName);
		}
	}

	return RET_OK;
}

//
//Отправка транзакции в CVAL
//
DWORD WINAPI _Imp_Cval(CVAL_OUT_CTX *cval_out_ctx, WORD w_idx, char *ch_DdeImpCval)
{
	WORD i, j;
	char chExecSqlBuf[(STRUCT_COL_SIZE * 2 + LEN_1) * DDE_TRN_SIZE + LEN_3 + LEN_2 + 1];

	if (!cval_out_ctx) return ERR_ARGS;

	OParameterCollection params = db_pool_ctx.odb[w_idx].GetParameters();

	for (i = 0; i < cval_out_ctx->ctx_idx; i++) {
		memset(chExecSqlBuf, 0, (STRUCT_COL_SIZE * 2 + LEN_1) * DDE_TRN_SIZE + LEN_3 + LEN_2 + 1);
		sprintf(chExecSqlBuf, "%s%s%s", "BEGIN ", ch_DdeImpCval, "(");
		for (j = 0; j < DDE_CVAL_SIZE; j++) {

			if (!cval_out_ctx->roboex_quik_st[i][j].chRoboexName || strlen(cval_out_ctx->roboex_quik_st[i][j].chRoboexName) == 0) {
				continue;
			}
			
			if (!strcmp(cval_out_ctx->roboex_quik_st[i][j].chQuikName, "OPEN")) {
				ReplaceSDecimal(cval_out_ctx->roboex_quik_st[i][j].chValue);
				OParameter n_OpenPrice = params.Add(cval_out_ctx->roboex_quik_st[i][j].chRoboexName, cval_out_ctx->roboex_quik_st[i][j].chValue, OPARAMETER_INVAR, OTYPE_NUMBER);
			}
			else if (!strcmp(cval_out_ctx->roboex_quik_st[i][j].chQuikName, "LAST")) {
				ReplaceSDecimal(cval_out_ctx->roboex_quik_st[i][j].chValue);
				OParameter n_LastPriceTrn = params.Add(cval_out_ctx->roboex_quik_st[i][j].chRoboexName, cval_out_ctx->roboex_quik_st[i][j].chValue, OPARAMETER_INVAR, OTYPE_NUMBER);
			}
			else if (!strcmp(cval_out_ctx->roboex_quik_st[i][j].chQuikName, "HIGH")) {
				ReplaceSDecimal(cval_out_ctx->roboex_quik_st[i][j].chValue);
				OParameter n_MaxPriceTrn = params.Add(cval_out_ctx->roboex_quik_st[i][j].chRoboexName, cval_out_ctx->roboex_quik_st[i][j].chValue, OPARAMETER_INVAR, OTYPE_NUMBER);
			}
			else if (!strcmp(cval_out_ctx->roboex_quik_st[i][j].chQuikName, "LOW")) {
				ReplaceSDecimal(cval_out_ctx->roboex_quik_st[i][j].chValue);
				OParameter n_MinPriceTrn = params.Add(cval_out_ctx->roboex_quik_st[i][j].chRoboexName, cval_out_ctx->roboex_quik_st[i][j].chValue, OPARAMETER_INVAR, OTYPE_NUMBER);
			}
			else if (!strcmp(cval_out_ctx->roboex_quik_st[i][j].chQuikName, "LONGNAME")) {
				OParameter c_Tool = params.Add(cval_out_ctx->roboex_quik_st[i][j].chRoboexName, cval_out_ctx->roboex_quik_st[i][j].chValue, OPARAMETER_INVAR, OTYPE_VARCHAR2);
			}
			else if (!strcmp(cval_out_ctx->roboex_quik_st[i][j].chQuikName, "PREVPRICE")) {
				ReplaceSDecimal(cval_out_ctx->roboex_quik_st[i][j].chValue);
				OParameter n_ClosePrice = params.Add(cval_out_ctx->roboex_quik_st[i][j].chRoboexName, cval_out_ctx->roboex_quik_st[i][j].chValue, OPARAMETER_INVAR, OTYPE_NUMBER);
			}
			else if (!strcmp(cval_out_ctx->roboex_quik_st[i][j].chQuikName, "LOTSIZE")) {
				ReplaceSDecimal(cval_out_ctx->roboex_quik_st[i][j].chValue);
				OParameter n_SizeLot = params.Add(cval_out_ctx->roboex_quik_st[i][j].chRoboexName, cval_out_ctx->roboex_quik_st[i][j].chValue, OPARAMETER_INVAR, OTYPE_NUMBER);
			}
			else if (!strcmp(cval_out_ctx->roboex_quik_st[i][j].chQuikName, "TIME")) {
				OParameter c_LastTrnTime = params.Add(cval_out_ctx->roboex_quik_st[i][j].chRoboexName, cval_out_ctx->roboex_quik_st[i][j].chValue, OPARAMETER_INVAR, OTYPE_VARCHAR2);
			}
			else if (!strcmp(cval_out_ctx->roboex_quik_st[i][j].chQuikName, "STATUS")) {
				OParameter c_Status = params.Add(cval_out_ctx->roboex_quik_st[i][j].chRoboexName, cval_out_ctx->roboex_quik_st[i][j].chValue, OPARAMETER_INVAR, OTYPE_VARCHAR2);
			}
			else if (!strcmp(cval_out_ctx->roboex_quik_st[i][j].chQuikName, "TRADE_DATE_CODE")) {
				OParameter d_TenderDate = params.Add(cval_out_ctx->roboex_quik_st[i][j].chRoboexName, cval_out_ctx->roboex_quik_st[i][j].chValue, OPARAMETER_INVAR, OTYPE_DATE);
			}
			else if (!strcmp(cval_out_ctx->roboex_quik_st[i][j].chQuikName, "CODE")) {
				OParameter c_ToolCode = params.Add(cval_out_ctx->roboex_quik_st[i][j].chRoboexName, cval_out_ctx->roboex_quik_st[i][j].chValue, OPARAMETER_INVAR, OTYPE_VARCHAR2);
			}
			else if (!strcmp(cval_out_ctx->roboex_quik_st[i][j].chQuikName, "CLASS_CODE")) {
				OParameter c_ClassCode = params.Add(cval_out_ctx->roboex_quik_st[i][j].chRoboexName, cval_out_ctx->roboex_quik_st[i][j].chValue, OPARAMETER_INVAR, OTYPE_VARCHAR2);
			}

			strcat(chExecSqlBuf, cval_out_ctx->roboex_quik_st[i][j].chRoboexName);
			strcat(chExecSqlBuf, " => :");
			strcat(chExecSqlBuf, cval_out_ctx->roboex_quik_st[i][j].chRoboexName);
			strcat(chExecSqlBuf, ", ");
		}

		sprintf(&chExecSqlBuf[strlen(chExecSqlBuf) - 2], "); END;");

		oresult r = db_pool_ctx.odb[w_idx].ExecuteSQL(chExecSqlBuf);
		if (r != OSUCCESS) {				
			//WriteLog_DDE("[ERR] Imp_Cval(..)", db_pool_ctx.odb[w_idx].ServerErrorNumber());			
			;
		}

		for (j = 0; j < DDE_CVAL_SIZE; j++) {

			if (!cval_out_ctx->roboex_quik_st[i][j].chRoboexName || strlen(cval_out_ctx->roboex_quik_st[i][j].chRoboexName) == 0) {
				continue;
			}

			params.Remove(cval_out_ctx->roboex_quik_st[i][j].chRoboexName);

		}
	}

	return RET_OK;
}

//EOF
