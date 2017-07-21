
// UDPPushDlg.h : header file
//

#pragma once
#include "afxwin.h"

// CUDPPushDlg dialog
class CUDPPushDlg : public CDialog
{
// Construction
public:
	CUDPPushDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_UDPPUSH_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedBtnPush();
	afx_msg void OnBnClickedBtnStop();
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();

public:
	// 打开关闭线程
	void StartWorkThread();
	void StartExitThread();
	// 线程函数
	DWORD static WINAPI WorkThread(LPVOID lpParam);
	DWORD static WINAPI ExitThread(LPVOID lpParam);

public:
	// 线程句柄
	BOOL m_bWork;
	BOOL m_bThread;
	// 生成的流ID
	WCHAR m_szID[128];

	CEdit m_edIp;
	CEdit m_edPort;
	CEdit m_sStreamId;
	CStatic m_sLocal;
	CStatic m_sRemote;
	CStatic m_sTip;

	int m_nStreamCan;
	CButton *m_pCheck1;
	CButton *m_pCheck2;
};
