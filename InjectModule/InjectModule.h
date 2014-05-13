
#pragma once
#include "IpcClient.h"

class InjectModule
{
public:
	static InjectModule* GetInstance();

private:
	InjectModule();

public:
	//初始化
	long Init();

	static unsigned __stdcall DllWorkerThread(void* pParam);

private:
	IpcClient	m_IpcClient;	//进程间通信
};

#define injectModule InjectModule::GetInstance()