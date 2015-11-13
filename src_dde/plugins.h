/*
	plugins.h

	Author: MiDi$
*/

#ifndef _PLUGINS_H
#define _PLUGINS_H

	HMODULE hModuleTrn = NULL;

	typedef DWORD (WINAPI *PFNIMPTRNPLUGIN) (void *ctx, void *pImpStruct, WORD w_ObjType, void *pRes1);
	PFNIMPTRNPLUGIN Imp_Trn_Plugin = NULL;

#endif