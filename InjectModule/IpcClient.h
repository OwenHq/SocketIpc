
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
		IpcConnectState_Succ = 0,	//���ӳɹ�
		IpcConnectState_Fail,	//����ʧ��
		IpcConnectState_NotConnect,	//δ����
		IpcConnectState_Connecting	//��������
	}IpcConnectState;

public:
	IpcClient();
	~IpcClient();

public:
	//��ʼ��
	long Init();

private:
	//�����ص�
	static long __stdcall ConnectClientCallback(DWORD dwErrCode, void* pUserData);
	//���ջص�
	static long __stdcall RecvCallback(DWORD dwErrCode, DWORD dwId, SIPC_STREAM hSIpcStream, void* pUserData);
	//LUA�ű�����ص�
	static int __stdcall LuaErrorHandle(lua_State* luaState,const wchar_t* pExtInfo,const wchar_t* luaErrorString, PXL_LRT_ERROR_STACK pStackInfo);

	//����LUA�ű�����
	long TraceLuaError();
	//��ȡLUAջ
	long GetLuaStack(const wchar_t* wszFilePath);

private:	
	SIPC_INSTANCE		m_sipcInstance;		//SocketIPCʵ��
	IpcConnectState		m_IpcConnectState;	//����״̬
};