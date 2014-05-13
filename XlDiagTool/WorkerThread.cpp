
#include "stdafx.h"
#include "WorkerThread.h"
#include "TraceMgr.h"
#include "Common/Common.h"

WorkerThread::WorkerThread()
: m_hWnd(NULL)
, m_hThunderProcess(NULL)
, m_bFirstGetIo(TRUE)
, m_dwThunderProcessId(0)
, m_iTriggerCount(0)
{

	m_traceInfo.bCheckIO = FALSE;
	m_traceInfo.bCheckCPU = FALSE;
	m_traceInfo.bCheckMemory = FALSE;
	m_traceInfo.bCheckHung = FALSE;
}

WorkerThread::~WorkerThread()
{

}

long WorkerThread::Init(HWND hWnd, std::wstring wstrSavePath)
{
	TSDEBUG(L"Enter, wstrSavePath = %s", wstrSavePath.c_str());

	if (NULL != m_hWnd)
	{
		TSDEBUG(L"Have init");
		return XLDIAGTOOL_FAILED_HAVEINIT;
	}
	if (NULL == hWnd)
	{
		TSDEBUG(L"hWnd is NULL");
		return XLDIAGTOOL_FAILED_INVALIDPARAM;
	}
	m_hWnd = hWnd;
	m_wstrSavePath = wstrSavePath;

	//初始化IPC通信
	m_IpcServer.Init();

	return XLDIAGTOOL_SUCC;
}

long WorkerThread::UnInit()
{
	TSDEBUG(L"Enter");

	return XLDIAGTOOL_SUCC;
}

long WorkerThread::SetInteractionWnd(HWND hWnd)
{
	TSDEBUG(L"Enter");

	if (NULL == hWnd)
	{
		TSDEBUG(L"hWnd is NULL");
		return XLDIAGTOOL_FAILED_INVALIDPARAM;
	}

	m_hWnd = hWnd;

	return XLDIAGTOOL_SUCC;
}

long WorkerThread::Start()
{
	TSDEBUG(L"Enter");

	unsigned int uiWorkerThreadId;
	HANDLE hWorker = (HANDLE)_beginthreadex(NULL, 0, &Worker, this, 0, &uiWorkerThreadId);
	if (NULL == hWorker)
	{
		TSDEBUG(L"_beginthreadex failed");
		return XLDIAGTOOL_FAILED_CREATETHREAD;
	}
	::CloseHandle(hWorker);

	TSDEBUG(L"uiWorkerThreadId = %u", uiWorkerThreadId);
	m_dwWorkerThreadId = uiWorkerThreadId;

	return XLDIAGTOOL_SUCC;
}

long WorkerThread::Quit()
{
	TSDEBUG(L"Enter");

	//SaveInfoAndExitTrace(L"TraceOver", m_traceInfo);

	if (!::PostThreadMessage(m_dwWorkerThreadId, WM_QUIT, NULL, NULL))
	{
		DWORD dwErrCode = ::GetLastError();
		TSDEBUG(L"PostThreadMessage WM_QUIT failed, error code is %u", dwErrCode);
		return dwErrCode;
	}

	return XLDIAGTOOL_SUCC;
}

DWORD WorkerThread::GetWorkerThreadId()
{
	TSDEBUG(L"Enter, m_dwWorkerThreadId = %u", m_dwWorkerThreadId);
	return m_dwWorkerThreadId;
}


unsigned __stdcall WorkerThread::Worker(void* pParam)
{
	TSDEBUG(L"Enter");

	WorkerThread* pThis = (WorkerThread*)pParam;
	SYSTEMTIME systemTime;

	//获得Thunder进程句柄
	if (XLDIAGTOOL_SUCC != GetProcessId(/*L"Thunder.exe"*/L"Thunder.exe", pThis->m_dwThunderProcessId))
	{
		TSDEBUG(L"GetProcessId failed");
		::PostMessage(pThis->m_hWnd, WM_DLG_GETPROCESSFAILED, NULL, NULL);
		pThis->Quit();
		return 0;
	}
	TSDEBUG(L"pThis->m_dwThunderProcessId = %u", pThis->m_dwThunderProcessId);
	pThis->m_hThunderProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, pThis->m_dwThunderProcessId);
	if (NULL == pThis->m_hThunderProcess)
	{
		TSDEBUG(L"OpenProcess failed, error code is %u", ::GetLastError());
		::PostMessage(pThis->m_hWnd, WM_DLG_GETPROCESSFAILED, NULL, NULL);
		pThis->Quit();
	}

	UINT_PTR uiIOTimerId = 0, uiCPUTimerId = 0, uiMemoryTimerId = 0, uiHungTimerId = 0;
	long lRet;
	MSG msg;
	while(::GetMessage(&msg, NULL, 0, 0))
	{
		::TranslateMessage(&msg);

		switch (msg.message)
		{
		case WM_WORKER_STARTTRACE:
			{
				TSDEBUG(L"msg is WM_WORKER_STARTTRACE");

				::GetLocalTime(&systemTime);
				wchar_t* wszStartFlushTraceLog = new wchar_t[50];
				if (msg.wParam == 0)
				{
					wsprintf(wszStartFlushTraceLog, L"%02d:%02d:%02d 结束跟踪日志", systemTime.wHour, systemTime.wMinute, systemTime.wSecond);
					::PostMessage(pThis->m_hWnd, WM_DLG_PRINTLOG, (WPARAM)wszStartFlushTraceLog, NULL);

					TraceMgr::GetInstance()->DoStopTrace();
				}
				else
				{
					wsprintf(wszStartFlushTraceLog, L"%02d:%02d:%02d 开始跟踪日志", systemTime.wHour, systemTime.wMinute, systemTime.wSecond);
					::PostMessage(pThis->m_hWnd, WM_DLG_PRINTLOG, (WPARAM)wszStartFlushTraceLog, NULL);

					TraceMgr::GetInstance()->DoStartTrace();
				}				

				break;
			}
		case WM_WORKER_FLUSHTRACE:
			{
				TSDEBUG(L"msg is WM_WORKER_FLUSHTRACE");

				::GetLocalTime(&systemTime);
				wchar_t* wszStartFlushTraceLog = new wchar_t[50];
				wsprintf(wszStartFlushTraceLog, L"%02d:%02d:%02d 搜集日志开始", systemTime.wHour, systemTime.wMinute, systemTime.wSecond);
				::PostMessage(pThis->m_hWnd, WM_DLG_PRINTLOG, (WPARAM)wszStartFlushTraceLog, NULL);

				TraceMgr::GetInstance()->DoFlushTrace(L"C:\\Test.etl");

				::GetLocalTime(&systemTime);
				wchar_t* wszStopFlushTraceLog = new wchar_t[50];
				wsprintf(wszStopFlushTraceLog, L"%02d:%02d:%02d 搜集日志结束", systemTime.wHour, systemTime.wMinute, systemTime.wSecond);
				::PostMessage(pThis->m_hWnd, WM_DLG_PRINTLOG, (WPARAM)wszStopFlushTraceLog, NULL);

				break;
			}
		case WM_WORKER_ISHUNGWINDOW:
			{
				TSDEBUG(L"msg is WM_WORKER_ISHUNGWINDOW");

				::GetLocalTime(&systemTime);
				wchar_t* wszStartIsHungWindowLog = new wchar_t[50];
				wsprintf(wszStartIsHungWindowLog, L"%02d:%02d:%02d 检测窗口卡死开始", systemTime.wHour, systemTime.wMinute, systemTime.wSecond);
				::PostMessage(pThis->m_hWnd, WM_DLG_PRINTLOG, (WPARAM)wszStartIsHungWindowLog, NULL);

				BOOL bIsHung = FALSE;
				if (XLDIAGTOOL_SUCC == IsHungWindow(L"XLUEFrameHostWnd", NULL, bIsHung))//这里不应该传空，就区分普通版和极速版
				{
					TSDEBUG(L"bIsHung = %d", bIsHung);
				}

				::GetLocalTime(&systemTime);
				wchar_t* wszStopIsHungWindowLog = new wchar_t[50];
				wsprintf(wszStopIsHungWindowLog, L"%02d:%02d:%02d 检测窗口卡死结束", systemTime.wHour, systemTime.wMinute, systemTime.wSecond);
				::PostMessage(pThis->m_hWnd, WM_DLG_PRINTLOG, (WPARAM)wszStopIsHungWindowLog, NULL);

				break;
			}
		case WM_WORKER_CHECKIO:
			{
				TSDEBUG(L"msg is WM_WORKER_CHECKIO");

				::GetLocalTime(&systemTime);
				wchar_t* wszStartIsHungWindowLog = new wchar_t[50];
				if (msg.wParam == 0)
				{
					wsprintf(wszStartIsHungWindowLog, L"%02d:%02d:%02d 结束检测迅雷读写IO", systemTime.wHour, systemTime.wMinute, systemTime.wSecond);
					::PostMessage(pThis->m_hWnd, WM_DLG_PRINTLOG, (WPARAM)wszStartIsHungWindowLog, NULL);

					::KillTimer(NULL, uiIOTimerId);
					uiIOTimerId = 0;
					pThis->m_traceInfo.bCheckIO = FALSE;
				}
				else
				{
					wsprintf(wszStartIsHungWindowLog, L"%02d:%02d:%02d 开始检测迅雷读写IO", systemTime.wHour, systemTime.wMinute, systemTime.wSecond);
					::PostMessage(pThis->m_hWnd, WM_DLG_PRINTLOG, (WPARAM)wszStartIsHungWindowLog, NULL);

					pThis->m_bFirstGetIo = TRUE;
					pThis->m_traceInfo.bCheckIO = TRUE;
					uiIOTimerId = ::SetTimer(NULL, TimerId_CheckIO, 1000, NULL);
				}				

				break;
			}
		case WM_WORKER_CHECKCPU:
			{
				TSDEBUG(L"msg is WM_WORKER_CHECKCPU");

				::GetLocalTime(&systemTime);
				wchar_t* wszStartIsHungWindowLog = new wchar_t[50];
				if (msg.wParam == 0)
				{
					wsprintf(wszStartIsHungWindowLog, L"%02d:%02d:%02d 结束检测迅雷CPU占用", systemTime.wHour, systemTime.wMinute, systemTime.wSecond);
					::PostMessage(pThis->m_hWnd, WM_DLG_PRINTLOG, (WPARAM)wszStartIsHungWindowLog, NULL);

					::KillTimer(NULL, uiCPUTimerId);
					uiCPUTimerId = 0;
					pThis->m_traceInfo.bCheckCPU = FALSE;
				}
				else
				{
					wsprintf(wszStartIsHungWindowLog, L"%02d:%02d:%02d 开始检测迅雷CPU占用", systemTime.wHour, systemTime.wMinute, systemTime.wSecond);
					::PostMessage(pThis->m_hWnd, WM_DLG_PRINTLOG, (WPARAM)wszStartIsHungWindowLog, NULL);

					pThis->m_traceInfo.bCheckCPU = TRUE;
					uiCPUTimerId = ::SetTimer(NULL, TimerId_CheckCPU, 1000, NULL);
				}				

				break;
			}
		case WM_WORKER_CHECKMEMORY:
			{
				TSDEBUG(L"msg is WM_WORKER_CHECKMEMORY");

				::GetLocalTime(&systemTime);
				wchar_t* wszStartIsHungWindowLog = new wchar_t[50];
				if (msg.wParam == 0)
				{
					wsprintf(wszStartIsHungWindowLog, L"%02d:%02d:%02d 结束检测迅雷内存占用", systemTime.wHour, systemTime.wMinute, systemTime.wSecond);
					::PostMessage(pThis->m_hWnd, WM_DLG_PRINTLOG, (WPARAM)wszStartIsHungWindowLog, NULL);

					::KillTimer(NULL, uiMemoryTimerId);
					uiMemoryTimerId = 0;
					pThis->m_traceInfo.bCheckMemory = FALSE;
				}
				else
				{
					wsprintf(wszStartIsHungWindowLog, L"%02d:%02d:%02d 开始检测迅雷内存占用", systemTime.wHour, systemTime.wMinute, systemTime.wSecond);
					::PostMessage(pThis->m_hWnd, WM_DLG_PRINTLOG, (WPARAM)wszStartIsHungWindowLog, NULL);

					pThis->m_traceInfo.bCheckMemory = TRUE;
					uiMemoryTimerId = ::SetTimer(NULL, TimerId_CheckMemory, 1000, NULL);
				}

				break;
			}
		case WM_WORKER_CHECKHUNG:
			{
				TSDEBUG(L"msg is WM_WORKER_CHECKHUNG");

				::GetLocalTime(&systemTime);
				wchar_t* wszStartIsHungWindowLog = new wchar_t[50];
				if (msg.wParam == 0)
				{
					wsprintf(wszStartIsHungWindowLog, L"%02d:%02d:%02d 结束检测迅雷卡死", systemTime.wHour, systemTime.wMinute, systemTime.wSecond);
					::PostMessage(pThis->m_hWnd, WM_DLG_PRINTLOG, (WPARAM)wszStartIsHungWindowLog, NULL);

					::KillTimer(NULL, uiHungTimerId);
					uiHungTimerId = 0;
					pThis->m_traceInfo.bCheckHung = FALSE;
				}
				else
				{
					wsprintf(wszStartIsHungWindowLog, L"%02d:%02d:%02d 开始检测迅雷卡死", systemTime.wHour, systemTime.wMinute, systemTime.wSecond);
					::PostMessage(pThis->m_hWnd, WM_DLG_PRINTLOG, (WPARAM)wszStartIsHungWindowLog, NULL);

					pThis->m_traceInfo.bCheckHung = TRUE;
					uiHungTimerId = ::SetTimer(NULL, TimerId_CheckHung, 1000, NULL);
				}				

				break;
			}
		case WM_WORKER_LUAERROR:
			{
				TSDEBUG(L"msg is WM_WORKER_LUAERROR");

				if (msg.wParam == 0)
				{
					//发IPC消息，停止跟踪
				}
				else
				{
					::GetLocalTime(&systemTime);
					wchar_t* wszStartLuaErrorLog = new wchar_t[50];
					wsprintf(wszStartLuaErrorLog, L"%02d:%02d:%02d 开始检测迅雷脚本错误", systemTime.wHour, systemTime.wMinute, systemTime.wSecond);
					::PostMessage(pThis->m_hWnd, WM_DLG_PRINTLOG, (WPARAM)wszStartLuaErrorLog, NULL);

					wchar_t wszDllPath[MAX_PATH];
					::GetModuleFileName(NULL, wszDllPath, MAX_PATH);
					::PathCombine(wszDllPath, wszDllPath, L"../InjectModule.dll");
					if (0 != InjectDllByCreateRemoteThread(pThis->m_hThunderProcess, wszDllPath))
					{
						::GetLocalTime(&systemTime);
						wchar_t* wszStartLuaErrorLog = new wchar_t[50];
						wsprintf(wszStartLuaErrorLog, L"%02d:%02d:%02d 检测迅雷脚本错误失败", systemTime.wHour, systemTime.wMinute, systemTime.wSecond);
						::PostMessage(pThis->m_hWnd, WM_DLG_PRINTLOG, (WPARAM)wszStartLuaErrorLog, NULL);
					}
				}

				break;
			}
		case WM_TIMER:
			{
				TSDEBUG(L"msg is WM_TIMER");

				if (msg.wParam == uiIOTimerId)
				{
					TSDEBUG(L"msg.wParam = TimerId_CheckIO");

					//获得Thunder读写IO情况
					static IO_COUNTERS lastIoCounters;
					IO_COUNTERS curIoCounters;

					//获得IO
					if (!::GetProcessIoCounters(pThis->m_hThunderProcess, &curIoCounters))
					{
						TSDEBUG(L"GetProcessIoCounters failed, error code is %u", ::GetLastError());
						pThis->m_bFirstGetIo = TRUE;
						return 0;
					}

					if (pThis->m_bFirstGetIo)
					{
						memcpy(&lastIoCounters, &curIoCounters, sizeof(IO_COUNTERS));
						pThis->m_bFirstGetIo = FALSE;
					}
					else
					{
						//1秒内IO数据
						ULONGLONG ullReadOperationCount = curIoCounters.ReadOperationCount - lastIoCounters.ReadOperationCount;
						ULONGLONG ullWriteOperationCount = curIoCounters.WriteOperationCount - lastIoCounters.WriteOperationCount;
						ULONGLONG ullOtherOperationCount = curIoCounters.OtherOperationCount - lastIoCounters.OtherOperationCount;
						ULONGLONG ullReadTransferCount = curIoCounters.ReadTransferCount - lastIoCounters.ReadTransferCount;
						ULONGLONG ullWriteTransferCount = curIoCounters.WriteTransferCount - lastIoCounters.WriteTransferCount;
						ULONGLONG ullOtherTransferCount = curIoCounters.OtherTransferCount - lastIoCounters.OtherTransferCount;
						TSDEBUG(L"ullReadOperationCount = %I64u, ullWriteOperationCount = %I64u, ullOtherOperationCount = %I64u, ullReadTrasferCount = %I64u, ullWriteTransferCount = %I64u, ullOtherTransferCount = %I64u", ullReadOperationCount, ullWriteOperationCount, ullOtherOperationCount, ullReadTransferCount, ullWriteTransferCount, ullOtherTransferCount);
						pThis->m_traceInfo.ioCountersPerSecond.ReadOperationCount = ullReadOperationCount;
						pThis->m_traceInfo.ioCountersPerSecond.WriteOperationCount = ullWriteOperationCount;
						pThis->m_traceInfo.ioCountersPerSecond.OtherOperationCount = ullOtherOperationCount;
						pThis->m_traceInfo.ioCountersPerSecond.ReadTransferCount = ullReadTransferCount;
						pThis->m_traceInfo.ioCountersPerSecond.WriteTransferCount = ullWriteTransferCount;
						pThis->m_traceInfo.ioCountersPerSecond.OtherTransferCount = ullOtherTransferCount;

						//替换旧数据
						lastIoCounters.ReadOperationCount = curIoCounters.ReadOperationCount;
						lastIoCounters.WriteOperationCount = curIoCounters.WriteOperationCount;
						lastIoCounters.OtherOperationCount = curIoCounters.OtherOperationCount;
						lastIoCounters.ReadTransferCount = curIoCounters.ReadTransferCount;
						lastIoCounters.WriteTransferCount = curIoCounters.WriteTransferCount;
						lastIoCounters.OtherTransferCount = curIoCounters.OtherTransferCount;

						//Do some thing
					}
				}
				else if (msg.wParam == uiCPUTimerId)
				{
					TSDEBUG(L"msg.wParam = TimerId_CheckCPU");

					static int siCpuUsage = 0;
					lRet = GetProcessCpuUsage(pThis->m_hThunderProcess, siCpuUsage);
					if (XLDIAGTOOL_SUCC == lRet || XLDIAGTOOL_FAILED_HAVENOTINIT == lRet)
					{
						//Do some thing
						TSDEBUG(L"siCpuUsage = %d", siCpuUsage);
						pThis->m_traceInfo.iCpuUsage = siCpuUsage;
						if (siCpuUsage >= (WORKERTHREAD_MAX_CPU * (pThis->m_iTriggerCount + 1)))
						{
							pThis->m_iTriggerCount ++;
							//保存信息并退出
							pThis->SaveInfoAndExitTrace(L"cpu", pThis->m_traceInfo);
							//::KillTimer(NULL, uiCPUTimerId);
						}
					}
				}
				else if (msg.wParam == uiMemoryTimerId)
				{
					TSDEBUG(L"msg.wParam = TimerId_CheckMemory");

					ULONGLONG ullWorkingSetSize = 0, ullPageFileUsage = 0;
					if (XLDIAGTOOL_SUCC == GetProcessMemoryUsage(pThis->m_hThunderProcess, ullWorkingSetSize, ullPageFileUsage))
					{
						//Do some thing
						TSDEBUG(L"ullWorkingSetSize = %I64u, ullPageFileUsage = %I64u", ullWorkingSetSize, ullPageFileUsage);
						pThis->m_traceInfo.ullWorkingSetSize = ullWorkingSetSize;
						pThis->m_traceInfo.ullPageFileUsage = ullPageFileUsage;
						if (ullWorkingSetSize >= (WORKERTHREAD_MAX_WORKINGSETSIZE * (pThis->m_iTriggerCount + 1)) || ullPageFileUsage >= (WORKERTHREAD_MAX_PAGEFILEUSAGE * (pThis->m_iTriggerCount + 1)))
						{
							pThis->m_iTriggerCount ++;
							//保存信息并退出
							pThis->SaveInfoAndExitTrace(L"memory", pThis->m_traceInfo);
							//::KillTimer(NULL, uiMemoryTimerId);
						}
					}
				}
				else if (msg.wParam == uiHungTimerId)
				{
					TSDEBUG(L"msg.wParam = TimerId_CheckHung");

					static BOOL sbIsHung = FALSE;
					if (XLDIAGTOOL_SUCC == IsHungWindow(L"XLUEFrameHostWnd", NULL, sbIsHung))
					{
						//Do some thing
						TSDEBUG(L"sbIsHung = %d", sbIsHung);
						pThis->m_traceInfo.bIsHung = sbIsHung;
						if (sbIsHung)
						{
							//保存信息并退出
							pThis->SaveInfoAndExitTrace(L"hung", pThis->m_traceInfo, TRUE);
							::KillTimer(NULL, uiHungTimerId);
						}
					}
				}
			}
		}

		::DispatchMessage(&msg);
	}

	//结束获取日志
	TraceMgr::GetInstance()->DoStopTrace();

	return 0;
}

long WorkerThread::SaveInfoAndExitTrace(std::wstring wstrExt, const TraceInfo& traceInfo, BOOL bExit)
{
	TSDEBUG(L"Enter, wstrExt = %s, bExit = %d", wstrExt.c_str(), bExit);

	SYSTEMTIME systemTime;

	//组装文件名
	::GetLocalTime(&systemTime);
	wchar_t wszCPULog[20];
	wsprintf(wszCPULog, L"%02d-%02d-%02d-%s", systemTime.wHour, systemTime.wMinute, systemTime.wSecond, wstrExt.c_str());
	std::wstring wstrFilePath = m_wstrSavePath;
	wstrFilePath = wstrFilePath + wszCPULog;
	TSDEBUG(L"m_wstrSavePath = %s, wstrFilePath = %s", m_wstrSavePath.c_str(), wstrFilePath.c_str());

	//获取LUA栈
	m_IpcServer.GetLuaStack(std::wstring(wstrFilePath + L"_luastack.txt").c_str());
	wchar_t* wszGetLuaStackLog = new wchar_t[50];
	wsprintf(wszGetLuaStackLog, L"%02d:%02d:%02d 获取LUA栈", systemTime.wHour, systemTime.wMinute, systemTime.wSecond);
	::PostMessage(m_hWnd, WM_DLG_PRINTLOG, (WPARAM)wszGetLuaStackLog, NULL);

	//获得DUMP
	WriteMiniDumpFile(m_hThunderProcess, m_dwThunderProcessId, MiniDumpNormal, std::wstring(wstrFilePath + L".dmp").c_str());
	::GetLocalTime(&systemTime);
	wchar_t* wszGetDumpLog = new wchar_t[50];
	if (0 == wstrExt.compare(L"cpu"))
	{
		wsprintf(wszGetDumpLog, L"%02d:%02d:%02d CPU占用超过%d，获取堆栈", systemTime.wHour, systemTime.wMinute, systemTime.wSecond, m_iTriggerCount * WORKERTHREAD_MAX_CPU);
	}
	else if (0 == wstrExt.compare(L"memory"))
	{
		wsprintf(wszGetDumpLog, L"%02d:%02d:%02d 内存占用超过%dM，获取堆栈", systemTime.wHour, systemTime.wMinute, systemTime.wSecond, m_iTriggerCount * 100);
	}
	else
	{
		wsprintf(wszGetDumpLog, L"%02d:%02d:%02d 迅雷卡死，获取堆栈", systemTime.wHour, systemTime.wMinute, systemTime.wSecond);
	}
	::PostMessage(m_hWnd, WM_DLG_PRINTLOG, (WPARAM)wszGetDumpLog, NULL);

	//获得日志
	::GetLocalTime(&systemTime);
	wchar_t* wszGetTraceLog = new wchar_t[50];
	wsprintf(wszGetTraceLog, L"%02d:%02d:%02d 获取跟踪日志", systemTime.wHour, systemTime.wMinute, systemTime.wSecond);
	::PostMessage(m_hWnd, WM_DLG_PRINTLOG, (WPARAM)wszGetTraceLog, NULL);
	std::wstring wstrEtl = L"\"";
	wstrEtl.append(wstrFilePath);
	wstrEtl.append(L".etl");
	TraceMgr::GetInstance()->DoFlushTrace(wstrEtl.c_str());

	//保存跟踪信息
	char szSaveData[256];
	std::string strSaveData;
	TSDEBUG(L"bCheckIO = %d, bCheckMemory = %d, bCheckCPU = %d, bCheckHung = %d", traceInfo.bCheckIO, traceInfo.bCheckMemory, traceInfo.bCheckCPU, traceInfo.bCheckHung);
	if (traceInfo.bCheckIO)
	{
		sprintf(szSaveData, 
			"[IO]\r\n \
			ReadOperationCountPerSecond=%I64u\r\n \
			WriteOperationCountPerSecond=%I64u\r\n \
			OtherOperationCountPerSecond=%I64u\r\n \
			ReadTransferCountPerSecond=%I64u\r\n \
			WriteTransferCountPerSecond=%I64u\r\n \
			OtherTransferCountPerSecond=%I64u\r\n",
			traceInfo.ioCountersPerSecond.ReadOperationCount,
			traceInfo.ioCountersPerSecond.WriteOperationCount,
			traceInfo.ioCountersPerSecond.OtherOperationCount,
			traceInfo.ioCountersPerSecond.ReadTransferCount,
			traceInfo.ioCountersPerSecond.WriteTransferCount,
			traceInfo.ioCountersPerSecond.OtherTransferCount
			);
		strSaveData.append(szSaveData);
	}
	if (traceInfo.bCheckMemory)
	{
		sprintf(szSaveData, 
			"[Memory]\r\n \
			WorkingSetSize=%I64u\r\n \
			PageFileUsage=%I64u\r\n",
			traceInfo.ullWorkingSetSize,
			traceInfo.ullPageFileUsage
			);
		strSaveData.append(szSaveData);
	}
	if (traceInfo.bCheckCPU)
	{
		sprintf(szSaveData, 
			"[CPU]\r\n \
			CpuUsage=%d\r\n",
			traceInfo.iCpuUsage
			);
		strSaveData.append(szSaveData);
	}
	if (traceInfo.bCheckHung)
	{
		sprintf(szSaveData, 
			"[Window]\r\n \
			IsHungWindow=%d\r\n",
			traceInfo.bIsHung
			);
		strSaveData.append(szSaveData);
	}
	SaveInfoToFile(std::wstring(wstrFilePath + L".txt").c_str(), strSaveData.c_str());

	if (bExit)
	{
		//停止跟踪
		::PostMessage(m_hWnd, WM_TRACEDLG_STOPTRACE, NULL, NULL);
	}

	return XLDIAGTOOL_SUCC;
}