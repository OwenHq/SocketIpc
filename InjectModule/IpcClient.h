
#pragma once
#include "../SocketIPC/SocketIPC.h"
#include "lua\XLLuaRuntime.h"

#define IPCSERVER_TRACELUAERROR	1
#define IPCSERVER_GETLUASTACK	2

class IpcClient
{
public:
	typedef enum tagIpcConnectState
	{
		IpcConnectState_Succ = 0,	//连接成功
		IpcConnectState_Fail,	//连接失败
		IpcConnectState_NotConnect,	//未连接
		IpcConnectState_Connecting	//正在连接
	}IpcConnectState;

public:
	IpcClient();
	~IpcClient();

public:
	//初始化
	long Init();

private:
	//监听回调
	static long __stdcall ConnectClientCallback(DWORD dwErrCode, void* pUserData);
	//接收回调
	static long __stdcall RecvCallback(DWORD dwErrCode, DWORD dwId, SIPC_STREAM hSIpcStream, void* pUserData);
	//LUA脚本错误回调
	static int __stdcall LuaErrorHandle(lua_State* luaState,const wchar_t* pExtInfo,const wchar_t* luaErrorString, PXL_LRT_ERROR_STACK pStackInfo);

	//跟踪LUA脚本错误
	long TraceLuaError();
	//获取LUA栈
	long GetLuaStack(const wchar_t* wszFilePath);

private:	
	SIPC_INSTANCE		m_sipcInstance;		//SocketIPC实例
	IpcConnectState		m_IpcConnectState;	//连接状态
};