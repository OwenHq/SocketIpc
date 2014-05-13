
#include "stdafx.h"
#include "IpcServer.h"


IpcServer::IpcServer()
: m_sipcInstance(NULL)
, m_IpcConnectState(IpcConnectState_NotConnect)
{

}

IpcServer::~IpcServer()
{

}


long IpcServer::Init()
{
	TSDEBUG(L"Enter");

	if (NULL != m_sipcInstance)
	{
		TSDEBUG(L"Have init ipcserver");
		return -1;
	}

	m_sipcInstance = SIPC_CreateIpcServer();
	if (NULL == m_sipcInstance)
	{
		TSDEBUG(L"SIPC_CreateIpcServer failed!");
		return -1;
	}

	long lRet = SIPC_InitIpcServer(m_sipcInstance);
	if (0 != lRet)
	{
		SIPC_DeleteIpcServer(m_sipcInstance);
		return lRet;
	}

	m_IpcConnectState = IpcConnectState_Connecting;
	return SIPC_ListenClient(m_sipcInstance, ListenClientCallback, this);
}

long IpcServer::TraceLuaError()
{
	TSDEBUG(L"Enter");

	if (IpcConnectState_Succ != m_IpcConnectState)
	{
		TSDEBUG(L"Have not connected client");
		return -1;
	}

	SIPC_STREAM hSIpcStream = SIPC_CreateSocketIpcStream();

	return SIPC_Send(m_sipcInstance, IPCSERVER_TRACELUAERROR, hSIpcStream, SendCallback, this);
}

long IpcServer::GetLuaStack(const wchar_t* wszFilePath)
{
	TSDEBUG(L"Enter");

	if (NULL == wszFilePath)
	{
		TSDEBUG(L"Invalid param, wszFilePath = NULL");
		return -1;
	}

	SIPC_STREAM hSIpcStream = SIPC_CreateSocketIpcStream();
	SIPC_StreamWriteWString(hSIpcStream, wszFilePath);

	return SIPC_Send(m_sipcInstance, IPCSERVER_GETLUASTACK, hSIpcStream, SendCallback, this);
}


long IpcServer::ListenClientCallback(DWORD dwErrCode, void* pUserData)
{
	TSDEBUG(L"Enter, dwErrCode = %u", dwErrCode);

	IpcServer* pThis = (IpcServer*)pUserData;

	if (0 == dwErrCode)
	{
		pThis->m_IpcConnectState = IpcConnectState_Succ;

		//¸ú×Ù½Å±¾´íÎó
		pThis->TraceLuaError();
	}
	else
	{
		pThis->m_IpcConnectState = IpcConnectState_Fail;
	}

	return 0;
}

long IpcServer::SendCallback(DWORD dwErrCode, DWORD dwId, void* pUserData)
{
	TSDEBUG(L"Enter, dwErrCode = %u, dwId = %u", dwErrCode, dwId);

	return 0;
}