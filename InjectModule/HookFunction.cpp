
#include "stdafx.h"
#include "HookFunction.h"

LUAERRORHANDLE HookFunction::ms_pfnOriginalLuaErrorHandle = NULL;

HookFunction::HookFunction()
{

}

HookFunction::~HookFunction()
{

}

long HookFunction::Init()
{
	TSDEBUG(L"Enter");

	return XLDIAGTOOL_SUCC;
}

long HookFunction::UnInit()
{
	TSDEBUG(L"Enter");

	return XLDIAGTOOL_SUCC;
}

long HookFunction::Hook()
{
	TSDEBUG(L"Enter");

	return XLDIAGTOOL_SUCC;
}

long HookFunction::UnHook()
{
	TSDEBUG(L"Enter");

	return XLDIAGTOOL_SUCC;
}


int __stdcall HookFunction::HookLuaErrorHandle(lua_State* luaState, const wchar_t* pExtInfo, const wchar_t* luaErrorString, PXL_LRT_ERROR_STACK pStackInfo)
{
	TSDEBUG(L"Enter");

	return XLDIAGTOOL_SUCC;
}