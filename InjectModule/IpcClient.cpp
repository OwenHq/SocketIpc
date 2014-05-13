
#include "stdafx.h"
#include "IpcClient.h"


IpcClient::IpcClient()
: m_sipcInstance(NULL)
, m_IpcConnectState(IpcConnectState_NotConnect)
{

}

IpcClient::~IpcClient()
{

}


long IpcClient::Init()
{
	TSDEBUG(L"Enter");

	if (NULL != m_sipcInstance)
	{
		TSDEBUG(L"Have init ipcclient");
		return -1;
	}

	m_sipcInstance = SIPC_CreateIpcClient();
	if (NULL == m_sipcInstance)
	{
		TSDEBUG(L"SIPC_CreateIpcClient failed!");
		return -1;
	}

	long lRet = SIPC_InitIpcClient(m_sipcInstance);
	if (0 != lRet)
	{
		SIPC_DeleteIpcClient(m_sipcInstance);
		return lRet;
	}

	m_IpcConnectState = IpcConnectState_Connecting;
	return SIPC_ConnectServer(m_sipcInstance, ConnectClientCallback, this);
}


long IpcClient::ConnectClientCallback(DWORD dwErrCode, void* pUserData)
{
	TSDEBUG(L"Enter, dwErrCode = %u", dwErrCode);

	IpcClient* pThis = (IpcClient*)pUserData;

	if (0 == dwErrCode)
	{
		pThis->m_IpcConnectState = IpcConnectState_Succ;

		//¼àÌý½ÓÊÕ
		return SIPC_RegisterRecvCallback(pThis->m_sipcInstance, RecvCallback, pThis);
	}
	else
	{
		pThis->m_IpcConnectState = IpcConnectState_Fail;
	}

	return 0;
}

long IpcClient::RecvCallback(DWORD dwErrCode, DWORD dwId, SIPC_STREAM hSIpcStream, void* pUserData)
{
	TSDEBUG(L"Enter, dwErrCode = %u, dwId = %u", dwErrCode, dwId);

	IpcClient* pThis = (IpcClient*)pUserData;

	switch(dwId)
	{
	case IPCSERVER_TRACELUAERROR:
		{
			TSDEBUG(L"dwId = IPCSERVER_TRACELUAERROR");

			pThis->TraceLuaError();

			break;
		}
	case IPCSERVER_GETLUASTACK:
		{
			TSDEBUG(L"dwId = IPCSERVER_GETLUASTACK");

			long lLen = SIPC_StreamReadWString(hSIpcStream, NULL, 0);
			wchar_t* wszFilePath = new wchar_t[lLen];
			long lRet = SIPC_StreamReadWString(hSIpcStream, wszFilePath, lLen);
			if (0 != lRet)
			{
				TSDEBUG(L"SIPC_StreamReadWString failed, lRet = %d", lRet);
				return -1;
			}
			TSDEBUG(L"wszFilePath = %s", wszFilePath);

			pThis->GetLuaStack(wszFilePath);

			delete [] wszFilePath;
			wszFilePath = NULL;

			break;
		}
	}

	return 0;
}

int __stdcall IpcClient::LuaErrorHandle(lua_State* luaState,const wchar_t* pExtInfo,const wchar_t* luaErrorString, PXL_LRT_ERROR_STACK pStackInfo)
{
	TSDEBUG(L"Enter");

	if (NULL != pExtInfo)
	{
		TSDEBUG(L"pExtInfo = %s", pExtInfo);
	}
	if (NULL != luaErrorString)
	{
		TSDEBUG(L"luaErrorString = %s", luaErrorString);
	}

	return 0;
}


long IpcClient::TraceLuaError()
{
	TSDEBUG(L"Enter");

	XLLRT_ErrorHandle(IpcClient::LuaErrorHandle);

	return 0;
}

long IpcClient::GetLuaStack(const wchar_t* wszFilePath)
{
	TSDEBUG(L"Enter");

	XL_LRT_ENV_HANDLE hEnv = XLLRT_GetEnv(NULL);
	if(hEnv != NULL)
	{
		XL_LRT_RUNTIME_HANDLE hRuntime = XLLRT_GetRuntime(hEnv, NULL);
		if(hRuntime != NULL)
		{
			lua_State* pLuaState = XLLRT_GetLuaState(hRuntime);

			if(pLuaState != NULL)
			{
				XL_LRT_DEBUG_MEMPOOL memPool;

				long result = XLLRT_DebugCreateStackMemPool(&memPool);

				if(result == XLLRT_RESULT_SUCCESS)
				{
					XL_LRT_DEBUG_LOGS debugLog;
					result = XLLRT_DebugCreateLogs(&debugLog);

					if(result == XLLRT_RESULT_SUCCESS)
					{
						result = XLLRT_DebugOutputLuaStack(pLuaState, memPool);

						if(result == XLLRT_RESULT_SUCCESS)
						{
							result = XLLRT_DebugMemPoolGetLogs(memPool, debugLog);

							if(result == XLLRT_RESULT_SUCCESS)
							{
								std::string strLuaStack;
								const char* szLuaStack = NULL;
								while(XLLRT_RESULT_SUCCESS == XLLRT_DebugLogsPopNextLog(&szLuaStack, debugLog) && szLuaStack != NULL)
								{
									strLuaStack.append(szLuaStack);
								}								

								//WriteToFile(pszDir, _T("lua"), callstack.c_str() );
								HANDLE hFile = ::CreateFile(wszFilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
								if(hFile == INVALID_HANDLE_VALUE)
								{
									DWORD dwErrCode = ::GetLastError();
									TSDEBUG(L"CreateFile failed, error code is %u", dwErrCode);
									return dwErrCode;
								}
								else
								{
									DWORD dwNumberOfBytesWritten = 0;
									if(!::WriteFile(hFile, strLuaStack.c_str(), strLuaStack.size(), &dwNumberOfBytesWritten, NULL))
									{
										DWORD dwErrCode = ::GetLastError();
										TSDEBUG(L"CreateFile failed, error code is %u", dwErrCode);
										return dwErrCode;
									}

									::CloseHandle(hFile);
								}
							}
						}
						XLLRT_DebugDestroyLogs(debugLog);
					}
					XLLRT_DebugDesroyStackMemPool(memPool);		
				}										
			}
			XLLRT_ReleaseRunTime(hRuntime);
		}
		XLLRT_ReleaseEnv(hEnv);
	}

	return 0;
}