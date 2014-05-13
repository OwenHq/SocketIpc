#pragma once
#include "Common/CriticalSection.h"
#include "Common/CommonDefine.h"

//定义搜集日志的状态
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
	//用于线程传参结构体
	struct ThreadParam
	{
		wchar_t*		wszSavePath;//保存路径
		TraceMgr*		pThis;//this指针
	};
	static TraceMgr* GetInstance();

	//开始搜集日志
	long StartTrace();
	//获得日志
	long FlushTrace(const wchar_t* wszSavePath);
	//停止搜集日志
	long StopTrace();

	//开始搜集日志
	long DoStartTrace();
	//获得日志
	long DoFlushTrace(const wchar_t* wszSavePath);
	//停止搜集日志
	long DoStopTrace();

private:
	//搜集日志线程
	static unsigned __stdcall StartTraceThread(void* pParam);
	//获得日志线程
	static unsigned __stdcall FlushTraceThread(void* pParam);
	//停止搜集日志线程
	static unsigned __stdcall StopTraceThread(void* pParam);

	//设置搜集状态
	void SetTraceState(TraceState traceState);
	TraceState GetTraceState();

private:
	//临界区变量
	static CriticalSection ms_CriticalSection;
	//标志是否正在搜集日志
	TraceState m_enumTraceState;
};
