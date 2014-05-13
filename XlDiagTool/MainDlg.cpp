
#include "stdafx.h"
#include "MainDlg.h"


CMainDlg::CMainDlg()
: m_iCurSelTab(0)
, m_pWorkerThread(NULL)
{

}

CMainDlg::~CMainDlg()
{

}


LRESULT CMainDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
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

	UIAddChildWindowContainer(m_hWnd);

	//�����ʾ�ı�����
	m_PromptStatic = GetDlgItem(IDC_STATIC_PROMPT);

	//���Thunder��װĿ¼
	std::wstring wstrThunderPath;
	GetThunderInstDir(wstrThunderPath);
	TSDEBUG(L"wstrThunderPath = %s", wstrThunderPath.c_str());

	//��������Ŀ¼
	std::wstring wstrXlDiagToolPath = wstrThunderPath + L"XlDiagTool\\";
	if (!::CreateDirectory(wstrXlDiagToolPath.c_str(), NULL))
	{
		TSDEBUG(L"CreateDirectory failed, error code is %u", ::GetLastError());
	}

	//���������߳�
	m_pWorkerThread = new WorkerThread();
	m_pWorkerThread->Init(m_hWnd, wstrXlDiagToolPath);
	m_pWorkerThread->Start();

	m_TypeTabCtrl = GetDlgItem(IDC_TYPETAB);
	m_TypeTabCtrl.InsertItem(0, L"���");
	m_TypeTabCtrl.InsertItem(1, L"����");

	m_CheckTabDlg.SetWorkerThreadPtr(m_pWorkerThread);
	HWND hCheckTabDlg = m_CheckTabDlg.Create(m_hWnd);
	if (hCheckTabDlg == NULL)
	{
		MessageBoxW(L"Can not create CheckTabDlg window", L"Error", MB_OK);
		return TRUE;
	}
	m_TraceTabDlg.SetWorkerThreadPtr(m_pWorkerThread);
	HWND hTraceTabDlg = m_TraceTabDlg.Create(m_hWnd);
	if (hTraceTabDlg == NULL)
	{
		MessageBoxW(L"Can not create TraceTabDlg window", L"Error", MB_OK);
		return TRUE;
	}
	m_aTabDlg[0] = &m_CheckTabDlg;
	m_aTabDlg[1] = &m_TraceTabDlg;

	CRect crTypeTabCtrl;
	m_TypeTabCtrl.GetClientRect(&crTypeTabCtrl);

	//�趨TAB����ʾ��Χ
	CRect crTabDlg;
	crTabDlg.top = crTypeTabCtrl.top + 32;
	crTabDlg.left = crTypeTabCtrl.left + 12;
	crTabDlg.right = crTypeTabCtrl.right + 8;
	//crTabDlg.bottom = crTypeTabCtrl.bottom + 8;
	crTabDlg.bottom = crTypeTabCtrl.bottom - 40;

	//����CheckTabDlg�ķ�Χ
	m_CheckTabDlg.MoveWindow(&crTabDlg);
	m_TraceTabDlg.MoveWindow(&crTabDlg);

	m_aTabDlg[0]->ShowWindow(SW_SHOW);
	m_aTabDlg[1]->ShowWindow(SW_HIDE);

	//������ʾλ��
	CRect crPrompt;
	crPrompt.top = crTypeTabCtrl.bottom - 40;
	crPrompt.left = crTypeTabCtrl.left + 12;
	crPrompt.right = crTypeTabCtrl.right + 8;
	crPrompt.bottom = crTypeTabCtrl.bottom + 8;
	m_PromptStatic.MoveWindow(&crPrompt);
/*
	//���½�Ϊ���Ƹ��ٰ汾����
	m_TypeTabCtrl.SetCurSel(1);
	m_aTabDlg[1]->ShowWindow(SW_SHOW);
	m_aTabDlg[0]->ShowWindow(SW_HIDE);
	m_TypeTabCtrl.EnableWindow(FALSE);*/

	return TRUE;
}

LRESULT CMainDlg::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CloseDialog(0);

	return 0;
}

LRESULT CMainDlg::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// unregister message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	pLoop->RemoveIdleHandler(this);

	m_pWorkerThread->Quit();
	delete m_pWorkerThread;

	return 0;
}

LRESULT CMainDlg::OnTcnSelchangeTypetab(int idCtrl, LPNMHDR pNMHDR, BOOL& bHandled)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	TSDEBUG(L"Enter");

	//�õ��µ�ҳ������
	int iCurSel = m_TypeTabCtrl.GetCurSel();
	TSDEBUG(L"m_iCurSelTab = %d, iCurSel = %d", m_iCurSelTab, iCurSel);

	if (m_iCurSelTab == iCurSel)
	{
		TSDEBUG(L"m_iCurSelTab = iCurSel");
		return 0;
	}

	m_aTabDlg[m_iCurSelTab]->ShowWindow(SW_HIDE);
	m_aTabDlg[iCurSel]->ShowWindow(SW_SHOW);
	m_iCurSelTab = iCurSel;

	return 0;
}

LRESULT CMainDlg::OnSetTypeTab(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	TSDEBUG(L"Enter");

	if (0 == wParam)
	{
		m_TypeTabCtrl.EnableWindow(FALSE);
	}
	else
	{
		m_TypeTabCtrl.EnableWindow(TRUE);
	}

	return 0;
}

LRESULT CMainDlg::OnGetProcessFailed(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	TSDEBUG(L"Enter");

	//��ʾ�û���ȡ����ʧ��
	m_PromptStatic.SetWindowText(L"    ����ʧ�ܣ�Ѹ��û����������������Ѹ�ף�");

	return 0;
}

void CMainDlg::CloseDialog(int nVal)
{
	DestroyWindow();
	::PostQuitMessage(nVal);
}
