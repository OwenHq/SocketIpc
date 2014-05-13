// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "resource.h"
#include "Common\CommonDefine.h"
#include "WorkerThread.h"

class CCheckTabDlg : public CDialogImpl<CCheckTabDlg>, public CUpdateUI<CCheckTabDlg>,
	public CMessageFilter, public CIdleHandler
{
public:
	enum { IDD = IDD_CHECKDLG };

	CCheckTabDlg();
	~CCheckTabDlg();

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		return CWindow::IsDialogMessage(pMsg);
	}

	virtual BOOL OnIdle()
	{
		return FALSE;
	}

	BEGIN_UPDATE_UI_MAP(CCheckTabDlg)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CCheckTabDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_CLOSE, OnClose);
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		COMMAND_HANDLER(IDC_BUTTONSTARTCHECK, BN_CLICKED, OnBnClickedButtonstartcheck)
		MESSAGE_HANDLER(WM_DLG_PRINTLOG, OnPrintLog)
	END_MSG_MAP()

public:
	// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonstartcheck(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	LRESULT OnPrintLog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	void CloseDialog(int nVal);

public:
	// 设置工作线程对象
	void SetWorkerThreadPtr(WorkerThread* pWorkerThread);

public:
	// “卡死检测”复选框
	CButton m_checkboxNoResponse;
	// “抓取dump”复选框
	CButton m_checkboxGetMiniDump;
	// “搜集日志”复选框
	CButton m_checkboxGetTrace;
	// “开始检测”按钮
	CButton	m_buttonStartCheck;
	// “日志”列表框
	CListBox m_listboxLog;
	//工作线程
	WorkerThread* m_pWorkerThread;
};
