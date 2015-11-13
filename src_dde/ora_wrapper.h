/*
	ora_wrapper.h

	Author: MiDi$
*/

#ifndef _ORA_WRAPPER_H
#define _ORA_WRAPPER_H

	HMODULE hModuleOra = NULL;

	typedef DWORD (WINAPI *PFN_INITORACONNECT) (char *chUserName, char *chPasswd, char *chInstance, DWORD dw_OraSessMax);
	PFN_INITORACONNECT _InitOraConnect = NULL;

	typedef void (WINAPI *PFN_CLOSEORACONNECT) ();
	PFN_CLOSEORACONNECT _CloseOraConnect = NULL;

	typedef DWORD (WINAPI *PFN_GETBBSESSIONFROMPOOL) (WORD *w_idx, DWORD dw_OraConnTimeOut);
	PFN_GETBBSESSIONFROMPOOL _GetDbSessionFromPool = NULL;

	typedef void (WINAPI *PFN_REMOVEDBSESSIONFROMPOOL) (WORD w_idx);
	PFN_REMOVEDBSESSIONFROMPOOL _RemoveDbSessionFromPool = NULL;

	typedef void (WINAPI *PFN_BEGINTRANSACTION) (WORD w_idx);
	PFN_BEGINTRANSACTION _BeginTransaction = NULL;

	typedef void (WINAPI *PFN_COMMITTRANSACTION) (WORD w_idx);
	PFN_COMMITTRANSACTION _CommitTransaction = NULL;

	typedef DWORD (WINAPI *PFN_IMP_TRN) (TRN_OUT_CTX *trn_out_ctx, WORD w_idx, char *ch_DdeImpTrn);
	PFN_IMP_TRN _Imp_Trn = NULL;

	typedef DWORD (WINAPI *PFN_IMP_CVAL) (CVAL_OUT_CTX *cval_out_ctx, WORD w_idx, char *ch_DdeImpCval);
	PFN_IMP_CVAL _Imp_Cval = NULL;

#endif