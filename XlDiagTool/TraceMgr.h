#pragma once
#include "Common/CriticalSection.h"
#include "Common/CommonDefine.h"

//�����Ѽ���־��״̬
typedef enum tagTraceState
{
	TraceState_TraceNotStart = 0,
	TraceState_TraceStarting,
	TraceState_TraceStarted,
	TraceState_TraceFlushing,
	TraceState_TraceStopping,
	TraceState_TraceStopped
}TraceState;

class TraceMgr
{
private:
	TraceMgr();

public:
	//�����̴߳��νṹ��
	struct ThreadParam
	{
		wchar_t*		wszSavePath;//����·��
		TraceMgr*		pThis;//thisָ��
	};
	static TraceMgr* GetInstance();

	//��ʼ�Ѽ���־
	long StartTrace();
	//�����־
	long FlushTrace(const wchar_t* wszSavePath);
	//ֹͣ�Ѽ���־
	long StopTrace();

	//��ʼ�Ѽ���־
	long DoStartTrace();
	//�����־
	long DoFlushTrace(const wchar_t* wszSavePath);
	//ֹͣ�Ѽ���־
	long DoStopTrace();

private:
	//�Ѽ���־�߳�
	static unsigned __stdcall StartTraceThread(void* pParam);
	//�����־�߳�
	static unsigned __stdcall FlushTraceThread(void* pParam);
	//ֹͣ�Ѽ���־�߳�
	static unsigned __stdcall StopTraceThread(void* pParam);

	//�����Ѽ�״̬
	void SetTraceState(TraceState traceState);
	TraceState GetTraceState();

private:
	//�ٽ�������
	static CriticalSection ms_CriticalSection;
	//��־�Ƿ������Ѽ���־
	TraceState m_enumTraceState;
};
