
// UDPPullDlg.h : header file
//

#pragma once
#include "afxwin.h"

// CUDPPullDlg dialog
class CUDPPullDlg : public CDialog
{
// Construction
public:
	CUDPPullDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_UDPPULL_DIALOG };

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
	afx_msg void OnBnClickedBtnPull();
	afx_msg void OnBnClickedBtnStop();
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();

public:
	// 打开关闭线程
	void StartWorkThread();
	void StopWorkThread();
	// 线程函数
	DWORD static WINAPI WorkThread(LPVOID lpParam);

public:
	// 线程句柄
	BOOL m_bWork;

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
