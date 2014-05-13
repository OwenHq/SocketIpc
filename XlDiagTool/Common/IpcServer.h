
#pragma once
#include "../../SocketIPC/SocketIPC.h"

#define IPCSERVER_TRACELUAERROR	1
#define IPCSERVER_GETLUASTACK	2

class IpcServer
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
	IpcServer();
	~IpcServer();

public:
	//初始化
	long Init();

	//跟踪LUA脚本错误
	long TraceLuaError();
	//获取LUA栈
	long GetLuaStack(const wchar_t* wszFilePath);

private:
	//监听回调
	static long __stdcall ListenClientCallback(DWORD dwErrCode, void* pUserData);
	//调用回调
	static long __stdcall SendCallback(DWORD dwErrCode, DWORD dwId, void* pUserData);

private:	
	SIPC_INSTANCE		m_sipcInstance;		//SocketIPC实例
	IpcConnectState		m_IpcConnectState;	//连接状态
};