
#pragma once
#include "IpcClient.h"

class InjectModule
{
public:
	static InjectModule* GetInstance();

private:
	InjectModule();

public:
	//��ʼ��
	long Init();

	static unsigned __stdcall DllWorkerThread(void* pParam);

private:
	IpcClient	m_IpcClient;	//���̼�ͨ��
};

#define injectModule InjectModule::GetInstance()