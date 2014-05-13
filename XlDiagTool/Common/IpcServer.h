
#pragma once
#include "../../SocketIPC/SocketIPC.h"

#define IPCSERVER_TRACELUAERROR	1
#define IPCSERVER_GETLUASTACK	2

class IpcServer
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
	IpcServer();
	~IpcServer();

public:
	//��ʼ��
	long Init();

	//����LUA�ű�����
	long TraceLuaError();
	//��ȡLUAջ
	long GetLuaStack(const wchar_t* wszFilePath);

private:
	//�����ص�
	static long __stdcall ListenClientCallback(DWORD dwErrCode, void* pUserData);
	//���ûص�
	static long __stdcall SendCallback(DWORD dwErrCode, DWORD dwId, void* pUserData);

private:	
	SIPC_INSTANCE		m_sipcInstance;		//SocketIPCʵ��
	IpcConnectState		m_IpcConnectState;	//����״̬
};