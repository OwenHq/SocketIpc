#include "StdAfx.h"
#include "TraceMgr.h"

CriticalSection TraceMgr::ms_CriticalSection;

TraceMgr::TraceMgr()
: m_enumTraceState(TraceState_TraceNotStart)
{
}


TraceMgr* TraceMgr::GetInstance()
{
	static TraceMgr* sInstance = NULL;
	if (NULL == sInstance)
	{
		ms_CriticalSection.Enter();
		if (NULL == sInstance)
		{
			sInstance = new TraceMgr();
		}
		ms_CriticalSection.Leave();
	}
	return sInstance;
}

long TraceMgr::StartTrace()
{
	TSDEBUG(L"Enter");

	TraceState traceState = GetTraceState();
	if (TraceState_TraceNotStart != traceState && TraceState_TraceStopped != traceState)
	{
		TSDEBUG(L"Trace has started!");
		return XLDIAGTOOL_FAILED_ALREADYSTARTED;
	}

	SetTraceState(TraceState_TraceStarting);

	unsigned int uiThreadId;
	HANDLE hStartTrace = (HANDLE)_beginthreadex(NULL, 0, &StartTraceThread, this, 0, &uiThreadId);
	if (NULL == hStartTrace)
	{
		TSDEBUG(L"_beginthreadex failed");
		return XLDIAGTOOL_FAILED_CREATETHREAD;
	}
	::CloseHandle(hStartTrace);

	return XLDIAGTOOL_SUCC;
}

long TraceMgr::FlushTrace(const wchar_t* wszSavePath)
{
	TSDEBUG(L"Enter");
	if (NULL == wszSavePath)
	{
		TSDEBUG(L"wszSavePath is NULL");
		return XLDIAGTOOL_FAILED_INVALIDPARAM;
	}
	TSDEBUG(L"wszSavePath = %s", wszSavePath);

	TraceState traceState = GetTraceState();
	if (TraceState_TraceNotStart == traceState)
	{
		return XLDIAGTOOL_FAILED_TRACENOTSTART;
	}
	else if (TraceState_TraceStarting == traceState)
	{
		return XLDIAGTOOL_FAILED_TRACESTARTING;
	}
	else if (TraceState_TraceStopping == traceState)
	{
		return XLDIAGTOOL_FAILED_TRACESTOPPING;
	}
	else if (TraceState_TraceStopped == traceState)
	{
		return XLDIAGTOOL_FAILED_TRACESTOPPED;
	}
	else if (TraceState_TraceFlushing == traceState)
	{
		return XLDIAGTOOL_FAILED_TRACEFLUSHING;
	}

	SetTraceState(TraceState_TraceFlushing);

	ThreadParam* pThreadParam = new ThreadParam;
	pThreadParam->pThis = this;
	pThreadParam->wszSavePath = new wchar_t[wcslen(wszSavePath) + 1];
	wcscpy(pThreadParam->wszSavePath, wszSavePath);

	unsigned int uiThreadId;
	HANDLE hStartTrace = (HANDLE)_beginthreadex(NULL, 0, &FlushTraceThread, pThreadParam, 0, &uiThreadId);
	if (NULL == hStartTrace)
	{
		TSDEBUG(L"_beginthreadex failed");
		return XLDIAGTOOL_FAILED_CREATETHREAD;
	}
	::CloseHandle(hStartTrace);

	return XLDIAGTOOL_SUCC;
}

long TraceMgr::StopTrace()
{
	TSDEBUG(L"Enter");

	TraceState traceState = GetTraceState();
	if (TraceState_TraceStarted != traceState)
	{
		TSDEBUG(L"Trace has not started!");
		return XLDIAGTOOL_FAILED_TRACENOTSTART;
	}

	SetTraceState(TraceState_TraceStopping);

	unsigned int uiThreadId;
	HANDLE hStartTrace = (HANDLE)_beginthreadex(NULL, 0, &StopTraceThread, this, 0, &uiThreadId);
	if (NULL == hStartTrace)
	{
		TSDEBUG(L"_beginthreadex failed");
		return XLDIAGTOOL_FAILED_CREATETHREAD;
	}
	::CloseHandle(hStartTrace);

	return XLDIAGTOOL_SUCC;
}

long TraceMgr::DoStartTrace()
{
	TSDEBUG(L"Enter");

	//拉起xperf.exe来开始搜集日志
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	if (!::CreateProcess(L"xperf.exe", 
		L" -on DiagEasy+FileIO+Base+REGISTRY+ALL_FAULTS+POWER+CONTMEMGEN+DISPATCHER -BufferSize 128 -MinBuffers 817 -MaxBuffers 817 -stackwalk ImageLoad+ImageUnload+VirtualAlloc+VirtualFree+CSwitch+ReadyThread+0x0504+0x0503+Profile+DiskReadInit+DiskWriteInit+DiskFlushInit+PagefaultTransition+HardFault+PagefaultHard+PagefaultDemandZero -Buffering",
		NULL,
		NULL,
		TRUE,
		CREATE_NO_WINDOW,
		NULL,
		NULL,
		&si,
		&pi))
	{
		DWORD dwErrCode = ::GetLastError();
		TSDEBUG(L"CreateProcess failed, error code is %u", dwErrCode);
		return dwErrCode;
	}

	::WaitForSingleObject(pi.hProcess, INFINITE);

	SetTraceState(TraceState_TraceStarted);

	return 0;
}

long TraceMgr::DoFlushTrace(const wchar_t* wszSavePath)
{
	TSDEBUG(L"Enter");
	if (NULL == wszSavePath)
	{
		TSDEBUG(L"wszSavePath is NULL");
		return XLDIAGTOOL_FAILED_INVALIDPARAM;
	}

	wchar_t *wszParam = new wchar_t[wcslen(L" -flush -f ") + wcslen(wszSavePath) + 1];
	wcscpy(wszParam, L" -flush -f ");
	wcscat(wszParam, wszSavePath);
	TSDEBUG(L"wszParam = %s", wszParam);

	//拉起xperf.exe来停止搜集日志
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	if (!::CreateProcess(L"xperf.exe", 
		wszParam,
		NULL,
		NULL,
		TRUE,
		CREATE_NO_WINDOW,
		NULL,
		NULL,
		&si,
		&pi))
	{
		DWORD dwErrCode = ::GetLastError();
		TSDEBUG(L"CreateProcess failed, error code is %u", dwErrCode);
		return dwErrCode;
	}

	::WaitForSingleObject(pi.hProcess, INFINITE);

	SetTraceState(TraceState_TraceStarted);

	return 0;
}

long TraceMgr::DoStopTrace()
{
	TSDEBUG(L"Enter");

	//拉起xperf.exe来停止搜集日志
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	if (!::CreateProcess(L"xperf.exe", 
		L" -stop",
		NULL,
		NULL,
		TRUE,
		CREATE_NO_WINDOW,
		NULL,
		NULL,
		&si,
		&pi))
	{
		DWORD dwErrCode = ::GetLastError();
		TSDEBUG(L"CreateProcess failed, error code is %u", dwErrCode);
		return dwErrCode;
	}

	::WaitForSingleObject(pi.hProcess, INFINITE);

	SetTraceState(TraceState_TraceStopped);

	return 0;
}


unsigned __stdcall TraceMgr::StartTraceThread(void* pParam)
{
	TSDEBUG(L"Enter");

	TraceMgr* pThis = (TraceMgr*)pParam;
	pThis->DoStartTrace();

	return 0;
}

unsigned __stdcall TraceMgr::FlushTraceThread(void* pParam)
{
	TSDEBUG(L"Enter");

	ThreadParam* pThreadParam = (ThreadParam*)pParam;
	TSDEBUG(L"wstrSavePath = %s", pThreadParam->wszSavePath);
	pThreadParam->pThis->DoFlushTrace(pThreadParam->wszSavePath);

	delete [] pThreadParam->wszSavePath;
	delete [] pThreadParam;

	return 0;
}

unsigned __stdcall TraceMgr::StopTraceThread(void* pParam)
{
	TSDEBUG(L"Enter");

	TraceMgr* pThis = (TraceMgr*)pParam;
	pThis->DoStopTrace();

	return 0;
}

void TraceMgr::SetTraceState(TraceState traceState)
{
	CriticalSection::Lock lock(TraceMgr::ms_CriticalSection);
	TSDEBUG(L"Enter, traceState = %d", traceState);
	m_enumTraceState = traceState;
}

TraceState TraceMgr::GetTraceState()
{
	CriticalSection::Lock lock(TraceMgr::ms_CriticalSection);
	TSDEBUG(L"Enter, m_enumTraceState = %d", m_enumTraceState);
	return m_enumTraceState;
}
