
#pragma once
#include "../XlDiagTool/Common/CommonDefine.h"
#include "lua/XLLuaRuntime.h"

typedef int (__stdcall *LUAERRORHANDLE)(lua_State* luaState, const wchar_t* pExtInfo, const wchar_t* luaErrorString, PXL_LRT_ERROR_STACK pStackInfo);

class HookFunction
{
public:
	HookFunction();
	~HookFunction();

	static long Init();
	static long UnInit();
	static long Hook();
	static long UnHook();

public:
	// Hook�ű�����ص�����
	static LUAERRORHANDLE ms_pfnOriginalLuaErrorHandle;
	static int __stdcall HookLuaErrorHandle(lua_State* luaState, const wchar_t* pExtInfo, const wchar_t* luaErrorString, PXL_LRT_ERROR_STACK pStackInfo);
};