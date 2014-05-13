
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
	//������Ϣ
	struct TraceInfo
	{
		BOOL		bCheckIO;
		BOOL		bCheckCPU;
		BOOL		bCheckMemory;
		BOOL		bCheckHung;
		BOOL		bIsHung;				//���ڿ���
		int			iCpuUsage;				//CPUռ����
		ULONGLONG	ullWorkingSetSize;		//ʹ�õ��ڴ�
		ULONGLONG	ullPageFileUsage;		//ʹ�õķ�ҳ
		IO_COUNTERS	ioCountersPerSecond;	//ÿ���дIO
	};

	WorkerThread();
	~WorkerThread();

public:
	// ��ʼ��
	long Init(HWND hWnd, std::wstring wstrSavePath);
	// ����ʼ��
	long UnInit();
	// ���ý������󴰿�
	long SetInteractionWnd(HWND hWnd);
	// ��ʼִ���߳�
	long Start();
	// �˳��߳�
	long Quit();
	// ��ù����߳�ID
	DWORD GetWorkerThreadId();

private:
	// �̺߳���
	static unsigned __stdcall Worker(void* pParam);
	// ������Ϣ���˳�����
	long SaveInfoAndExitTrace(std::wstring wstrExt, const TraceInfo& traceInfo, BOOL bExit = FALSE);

private:
	std::wstring	m_wstrSavePath;			//�����Ϣ����·��
	HWND			m_hWnd;					//���̴߳���
	DWORD			m_dwWorkerThreadId;		//�����߳�ID	
	HANDLE			m_hThunderProcess;		//Thunder���̾��	
	DWORD			m_dwThunderProcessId;	//Thunder����ID
	BOOL			m_bFirstGetIo;			//���Ʊ�ʶ���Ƿ�Ϊһ��������ʱ���ڵ�һ�λ��IO
	TraceInfo		m_traceInfo;			//������Ϣ
	int				m_iTriggerCount;		//��������
	IpcServer		m_IpcServer;			//���̼�ͨ��
};