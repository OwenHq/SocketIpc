
#include "stdafx.h"
#include "CheckTabDlg.h"
#include "TraceMgr.h"


CCheckTabDlg::CCheckTabDlg()
: m_pWorkerThread(NULL)
{

}

CCheckTabDlg::~CCheckTabDlg()
{

}


LRESULT CCheckTabDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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
	//��á�ץȡdump����ѡ��
	m_checkboxGetMiniDump = GetDlgItem(IDC_CHECKGETMINIDUMP);
	m_checkboxGetMiniDump.SetCheck(BST_CHECKED);
	//��á��Ѽ���־����ѡ��
	m_checkboxGetTrace = GetDlgItem(IDC_CHECKGETTRACE);
	m_checkboxGetTrace.SetCheck(BST_CHECKED);
	//��á���ʼ��⡱��ť
	m_buttonStartCheck = GetDlgItem(IDC_BUTTONSTARTCHECK);
	//��á���־���б��
	m_listboxLog = GetDlgItem(IDC_LISTLOG);

	m_pWorkerThread->SetInteractionWnd(m_hWnd);

	//��ǰ��ʼ�ռ���־
	TraceMgr::GetInstance()->StartTrace();

	return TRUE;
}

LRESULT CCheckTabDlg::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CloseDialog(0);

	return 0;
}

LRESULT CCheckTabDlg::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// unregister message filtering and idle updates
	/*CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	pLoop->RemoveIdleHandler(this);*/

	return 0;
}

LRESULT CCheckTabDlg::OnBnClickedButtonstartcheck(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	TSDEBUG(L"Enter");

	DWORD dwWorkerThreadId = m_pWorkerThread->GetWorkerThreadId();
	if (!::PostThreadMessage(dwWorkerThreadId, WM_WORKER_FLUSHTRACE, NULL, NULL))
	{
		TSDEBUG(L"PostThreadMessage WM_WORKER_FLUSHTRACE failed, error code is %u", ::GetLastError());
	}
	if (!::PostThreadMessage(dwWorkerThreadId, WM_WORKER_ISHUNGWINDOW, NULL, NULL))
	{
		TSDEBUG(L"PostThreadMessage WM_WORKER_ISHUNGWINDOW failed, error code is %u", ::GetLastError());
	}

	return 0;
}

LRESULT CCheckTabDlg::OnPrintLog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	TSDEBUG(L"Enter");

	wchar_t* wszPrintLog = (wchar_t*)wParam;
	m_listboxLog.InsertString(-1, wszPrintLog);

	delete [] wszPrintLog;

	return 0;
}

void CCheckTabDlg::CloseDialog(int nVal)
{
	DestroyWindow();
	::PostQuitMessage(nVal);
}

void CCheckTabDlg::SetWorkerThreadPtr(WorkerThread* pWorkerThread)
{
	TSDEBUG(L"Enter");
	m_pWorkerThread = pWorkerThread;
}
