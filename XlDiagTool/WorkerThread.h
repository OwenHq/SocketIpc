
#pragma once
#include "Common\CommonDefine.h"
#include "Common\IpcServer.h"

#define WORKERTHREAD_MAX_CPU	5
#define WORKERTHREAD_MAX_WORKINGSETSIZE	50*1024*1024
#define WORKERTHREAD_MAX_PAGEFILEUSAGE	50*1024*1024

class WorkerThread
{
public:
	typedef enum tagTimerId
	{
		TimerId_CheckIO = 0,
		TimerId_CheckCPU,
		TimerId_CheckMemory,
		TimerId_CheckHung
	};
	//跟踪信息
	struct TraceInfo
	{
		BOOL		bCheckIO;
		BOOL		bCheckCPU;
		BOOL		bCheckMemory;
		BOOL		bCheckHung;
		BOOL		bIsHung;				//窗口卡死
		int			iCpuUsage;				//CPU占用率
		ULONGLONG	ullWorkingSetSize;		//使用的内存
		ULONGLONG	ullPageFileUsage;		//使用的分页
		IO_COUNTERS	ioCountersPerSecond;	//每秒读写IO
	};

	WorkerThread();
	~WorkerThread();

public:
	// 初始化
	long Init(HWND hWnd, std::wstring wstrSavePath);
	// 反初始化
	long UnInit();
	// 设置交互对象窗口
	long SetInteractionWnd(HWND hWnd);
	// 开始执行线程
	long Start();
	// 退出线程
	long Quit();
	// 获得工作线程ID
	DWORD GetWorkerThreadId();

private:
	// 线程函数
	static unsigned __stdcall Worker(void* pParam);
	// 保存信息并退出跟踪
	long SaveInfoAndExitTrace(std::wstring wstrExt, const TraceInfo& traceInfo, BOOL bExit = FALSE);

private:
	std::wstring	m_wstrSavePath;			//获得信息保存路径
	HWND			m_hWnd;					//主线程窗口
	DWORD			m_dwWorkerThreadId;		//工作线程ID	
	HANDLE			m_hThunderProcess;		//Thunder进程句柄	
	DWORD			m_dwThunderProcessId;	//Thunder进程ID
	BOOL			m_bFirstGetIo;			//控制标识：是否为一个连续定时器内第一次获得IO
	TraceInfo		m_traceInfo;			//跟踪信息
	int				m_iTriggerCount;		//触发次数
	IpcServer		m_IpcServer;			//进程间通信
};