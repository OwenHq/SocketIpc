
#include "stdafx.h"
#include "TraceTabDlg.h"


CTraceTabDlg::CTraceTabDlg()
: m_pWorkerThread(NULL)
{

}

CTraceTabDlg::~CTraceTabDlg()
{

}


LRESULT CTraceTabDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{/*
	// center the dialog on the screen
	CenterWindow();

	// set icons
	HICON hIcon = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
	SetIcon(hIcon, TRUE);
	HICON hIconSmall = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
	SetIcon(hIconSmall, FALSE);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	UIAddChildWindowContainer(m_hWnd);*/

	//��á�������⡱��ѡ��
	m_checkboxNoResponse = GetDlgItem(IDC_CHECKNORESPONSE);
	m_checkboxNoResponse.SetCheck(BST_CHECKED);
	//��á��ű����󡱸�ѡ��
	m_checkboxLuaError = GetDlgItem(IDC_CHECKLUAERROR);
	m_checkboxLuaError.SetCheck(BST_CHECKED);
	//��á��Ѽ���־����ѡ��
	m_checkboxGetTrace = GetDlgItem(IDC_CHECKGETTRACE);
	m_checkboxGetTrace.SetCheck(BST_CHECKED);
	//��á���дIO����ѡ��
	m_checkboxReadWriteIO = GetDlgItem(IDC_CHECKREADWRITEIO);
	m_checkboxReadWriteIO.SetCheck(BST_CHECKED);
	//��á�CPUռ�á���ѡ��
	m_checkboxCPU = GetDlgItem(IDC_CHECKCPU);
	m_checkboxCPU.SetCheck(BST_CHECKED);
	//��á��ڴ�ռ�á���ѡ��
	m_checkboxMemory = GetDlgItem(IDC_CHECKMEMORY);
	m_checkboxMemory.SetCheck(BST_CHECKED);
	//��á���ʼ��⡱��ť
	m_buttonStartTrace = GetDlgItem(IDC_BUTTONSTARTTRACE);
	//��á���־���б��
	m_listboxLog = GetDlgItem(IDC_LISTLOG);

	m_pWorkerThread->SetInteractionWnd(m_hWnd);

	return TRUE;
}

LRESULT CTraceTabDlg::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CloseDialog(0);

	return 0;
}

LRESULT CTraceTabDlg::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// unregister message filtering and idle updates
	/*CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	pLoop->RemoveIdleHandler(this);*/

	return 0;
}

LRESULT CTraceTabDlg::OnBnClickedButtonstarttrace(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	TSDEBUG(L"Enter");

	BOOL bIsStartTrace = FALSE;

	//��ð�ť�ı�
	wchar_t wszButtonText[10];
	m_buttonStartTrace.GetWindowText(wszButtonText, 10);
	TSDEBUG(L"wszButtonText = %s", wszButtonText);
	if (0 == wcsicmp(wszButtonText, L"��ʼ����"))
	{
		bIsStartTrace = TRUE;
	}
	TSDEBUG(L"bIsStartTrace = %d", bIsStartTrace);

	//��ø����ڣ�Ҳ����TAB�ؼ�
	CWindow MainWnd = GetParent();

	//���ý���״̬
	if (bIsStartTrace)
	{
		m_buttonStartTrace.SetWindowText(L"ֹͣ����");
		m_checkboxNoResponse.EnableWindow(FALSE);
		m_checkboxLuaError.EnableWindow(FALSE);
		m_checkboxGetTrace.EnableWindow(FALSE);
		m_checkboxReadWriteIO.EnableWindow(FALSE);
		m_checkboxCPU.EnableWindow(FALSE);
		m_checkboxMemory.EnableWindow(FALSE);
		MainWnd.SendMessage(WM_MAINDLG_SETTYPETAB, 0, NULL);
	}
	else
	{
		m_buttonStartTrace.SetWindowText(L"��ʼ����");
		m_checkboxNoResponse.EnableWindow(TRUE);
		m_checkboxLuaError.EnableWindow(TRUE);
		m_checkboxGetTrace.EnableWindow(TRUE);
		m_checkboxReadWriteIO.EnableWindow(TRUE);
		m_checkboxCPU.EnableWindow(TRUE);
		m_checkboxMemory.EnableWindow(TRUE);
		MainWnd.SendMessage(WM_MAINDLG_SETTYPETAB, 1, NULL);
	}

	return SetTrace(bIsStartTrace);
}

LRESULT CTraceTabDlg::OnPrintLog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	TSDEBUG(L"Enter");

	wchar_t* wszPrintLog = (wchar_t*)wParam;
	m_listboxLog.InsertString(-1, wszPrintLog);

	delete [] wszPrintLog;

	return 0;
}

LRESULT CTraceTabDlg::OnStopTrace(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	TSDEBUG(L"Enter");

	OnBnClickedButtonstarttrace(0, 0, NULL, bHandled);

	return 0;
}

void CTraceTabDlg::CloseDialog(int nVal)
{
	DestroyWindow();
	::PostQuitMessage(nVal);
}

void CTraceTabDlg::SetWorkerThreadPtr(WorkerThread* pWorkerThread)
{
	TSDEBUG(L"Enter");
	m_pWorkerThread = pWorkerThread;
}

long CTraceTabDlg::SetTrace(BOOL bTrace)
{
	TSDEBUG(L"Enter, bTrace = %d", bTrace);

	DWORD dwWorkerThreadId = m_pWorkerThread->GetWorkerThreadId();

	//���ݸ���ѡ�����ø���
	int iNoResponseCheck = m_checkboxNoResponse.GetCheck();
	int iLuaErrorCheck = m_checkboxLuaError.GetCheck();
	int iGetTraceCheck = m_checkboxGetTrace.GetCheck();
	int iReadWriteIOCheck = m_checkboxReadWriteIO.GetCheck();
	int iCPUCheck = m_checkboxCPU.GetCheck();
	int iMemoryCheck = m_checkboxMemory.GetCheck();
	TSDEBUG(L"iNoResponseCheck = %d, iLuaErrorCheck = %d, iGetTraceCheck = %d, iReadWriteIOCheck = %d, iCPUCheck = %d, iMemoryCheck = %d", iNoResponseCheck, iLuaErrorCheck, iGetTraceCheck, iReadWriteIOCheck, iCPUCheck, iMemoryCheck);

	if (BST_CHECKED == iNoResponseCheck)
	{
		if (!::PostThreadMessage(dwWorkerThreadId, WM_WORKER_CHECKHUNG, bTrace, NULL))
		{
			TSDEBUG(L"PostThreadMessage WM_WORKER_CHECKHUNG failed, error code is %u", ::GetLastError());
		}
	}
	if (BST_CHECKED == iLuaErrorCheck)
	{
		if (!::PostThreadMessage(dwWorkerThreadId, WM_WORKER_LUAERROR, bTrace, NULL))
		{
			TSDEBUG(L"PostThreadMessage WM_WORKER_LUAERROR failed, error code is %u", ::GetLastError());
		}
	}
	if (BST_CHECKED == iGetTraceCheck)
	{
		if (!::PostThreadMessage(dwWorkerThreadId, WM_WORKER_STARTTRACE, bTrace, NULL))
		{
			TSDEBUG(L"PostThreadMessage WM_WORKER_STARTTRACE failed, error code is %u", ::GetLastError());
		}
	}
	if (BST_CHECKED == iReadWriteIOCheck)
	{
		if (!::PostThreadMessage(dwWorkerThreadId, WM_WORKER_CHECKIO, bTrace, NULL))
		{
			TSDEBUG(L"PostThreadMessage WM_WORKER_CHECKIO failed, error code is %u", ::GetLastError());
		}
	}
	if (BST_CHECKED == iCPUCheck)
	{
		if (!::PostThreadMessage(dwWorkerThreadId, WM_WORKER_CHECKCPU, bTrace, NULL))
		{
			TSDEBUG(L"PostThreadMessage WM_WORKER_CHECKCPU failed, error code is %u", ::GetLastError());
		}
	}
	if (BST_CHECKED == iMemoryCheck)
	{
		if (!::PostThreadMessage(dwWorkerThreadId, WM_WORKER_CHECKMEMORY, bTrace, NULL))
		{
			TSDEBUG(L"PostThreadMessage WM_WORKER_CHECKMEMORY failed, error code is %u", ::GetLastError());
		}
	}

	return XLDIAGTOOL_SUCC;
}
