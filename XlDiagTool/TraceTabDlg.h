// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "resource.h"
#include "Common\CommonDefine.h"
#include "WorkerThread.h"
#include "Common/Common.h"

class CTraceTabDlg : public CDialogImpl<CTraceTabDlg>, public CUpdateUI<CTraceTabDlg>,
	public CMessageFilter, public CIdleHandler
{
public:
	enum { IDD = IDD_TRACEDLG };

	CTraceTabDlg();
	~CTraceTabDlg();

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		return CWindow::IsDialogMessage(pMsg);
	}

	virtual BOOL OnIdle()
	{
		return FALSE;
	}

	BEGIN_UPDATE_UI_MAP(CTraceTabDlg)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CTraceTabDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_CLOSE, OnClose);
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		COMMAND_HANDLER(IDC_BUTTONSTARTTRACE, BN_CLICKED, OnBnClickedButtonstarttrace)
		MESSAGE_HANDLER(WM_DLG_PRINTLOG, OnPrintLog)
		MESSAGE_HANDLER(WM_TRACEDLG_STOPTRACE, OnStopTrace)
	END_MSG_MAP()

public:
	// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonstarttrace(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	LRESULT OnPrintLog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnStopTrace(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	void CloseDialog(int nVal);

public:
	// ���ù����̶߳���
	void SetWorkerThreadPtr(WorkerThread* pWorkerThread);

private:
	// ��ʼ����
	long SetTrace(BOOL bTrace);

private:
	// ��������⡱��ѡ��
	CButton m_checkboxNoResponse;
	// ���ű����󡱸�ѡ��
	CButton m_checkboxLuaError;
	// ���Ѽ���־����ѡ��
	CButton m_checkboxGetTrace;
	// ����дIO����ѡ��
	CButton m_checkboxReadWriteIO;
	// ��CPUռ�á���ѡ��
	CButton m_checkboxCPU;
	// ���ڴ�ռ�á���ѡ��
	CButton m_checkboxMemory;
	// ����ʼ��⡱��ť
	CButton	m_buttonStartTrace;
	// ����־���б��
	CListBox m_listboxLog;

	//�����߳�
	WorkerThread* m_pWorkerThread;
};
