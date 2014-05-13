// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "resource.h"
#include "Common\CommonDefine.h"
#include "WorkerThread.h"
#include "CheckTabDlg.h"
#include "TraceTabDlg.h"

class CMainDlg : public CDialogImpl<CMainDlg>, public CUpdateUI<CMainDlg>,
		public CMessageFilter, public CIdleHandler
{
public:
	enum { IDD = IDD_MAINDLG };

	CMainDlg();
	~CMainDlg();

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		return CWindow::IsDialogMessage(pMsg);
	}

	virtual BOOL OnIdle()
	{
		return FALSE;
	}

	BEGIN_UPDATE_UI_MAP(CMainDlg)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CMainDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_CLOSE, OnClose);
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		NOTIFY_HANDLER(IDC_TYPETAB, TCN_SELCHANGE, OnTcnSelchangeTypetab)
		MESSAGE_HANDLER(WM_MAINDLG_SETTYPETAB, OnSetTypeTab)
		MESSAGE_HANDLER(WM_DLG_GETPROCESSFAILED, OnGetProcessFailed)
	END_MSG_MAP()

public:
// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnTcnSelchangeTypetab(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/);
	LRESULT OnSetTypeTab(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnGetProcessFailed(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	void CloseDialog(int nVal);

public:
	//TAB控件
	CTabCtrl m_TypeTabCtrl;
	//检测窗口
	CCheckTabDlg m_CheckTabDlg;
	//跟踪窗口
	CTraceTabDlg m_TraceTabDlg;
	//按TAB顺序存储窗口
	CWindow* m_aTabDlg[2];
	//静态控件
	CStatic m_PromptStatic;
	//当前选中的TAB
	int m_iCurSelTab;
	//工作线程对象
	WorkerThread* m_pWorkerThread;
};
