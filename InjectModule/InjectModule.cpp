// InjectModule.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include "InjectModule.h"
#include <process.h>


InjectModule::InjectModule()
{

}

InjectModule* InjectModule::GetInstance()
{
	static InjectModule* sInstance = NULL;
	if (NULL == sInstance)
	{
		sInstance = new InjectModule();
	}
	return sInstance;
}


long InjectModule::Init()
{
	TSDEBUG(L"Enter");

	//����IPC��Create�Ĵ���û���Լ�����Ϣѭ������������Ҫ����һ����Ϣѭ��ʹIPC��������
	HANDLE hDllWorkerThread = (HANDLE)_beginthreadex(NULL, 0, &DllWorkerThread, this, 0, NULL);
	if (NULL == hDllWorkerThread)
	{
		TSDEBUG(L"_beginthreadex failed");
		return -1;
	}
	::CloseHandle(hDllWorkerThread);

	return 0;
}

unsigned __stdcall InjectModule::DllWorkerThread(void* pParam)
{
	InjectModule* pThis = (InjectModule*)pParam;

	pThis->m_IpcClient.Init();

	MSG msg;
	while(GetMessage(&msg,NULL,0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}
