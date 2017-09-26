
// flyUDPDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"

// CflyUDPDlg dialog
class CflyUDPDlg : public CDialog
{
// Construction
public:
	CflyUDPDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_FLYUDP_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedBtnPush();
	afx_msg void OnBnClickedBtnStop();
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedRadio3();
	afx_msg void OnBnClickedRadio4();
	afx_msg void OnBnClickedBtnRes();
	afx_msg LRESULT OnUpdate(WPARAM wParam, LPARAM lParam);

public:
	// 打开关闭线程
	void StartWorkThread();
	void StartExitThread();
	// 线程函数
	DWORD static WINAPI WorkThread(LPVOID lpParam);
	DWORD static WINAPI ExitThread(LPVOID lpParam);

	// 获取推流服务器列表
	void UpdateUdpList();
	DWORD static WINAPI GetUdpThread(LPVOID lpParam);

	// 更新拉流服务器列表
	void UpdatePullServer(CStringA strSer, CStringA strSerShow, CStringA strSerName);
	DWORD static WINAPI GetUdpPullThread(LPVOID lpParam);
	DWORD static WINAPI GetUdpPullOneThread(LPVOID lpParam);

	// 更新列表显示
	void UpdateListCtrl();

	// 刷新流ID
	void UpdateStreamId();

public:
	// 推拉流选择
	int m_nMode;
	// 控件
	CButton *m_pCheckPush;
	CButton *m_pCheckPull;

	// flyCan选择
	int m_nStreamCan;
	// 控件
	CButton *m_pCheck1;
	CButton *m_pCheck2;

	// 推拉流控件
	CButton m_btnStart;
	// 服务器列表控件
	CListCtrl m_ltServer;
	// 刷新会话列表控件
	CButton m_btnRes;
	// 本地视频显示控件
	CStatic m_sLocal;
	// 远程视频显示控件
	CStatic m_sRemote;
	// Tips显示控件
	CStatic m_sTip;

	// 工作标记
	BOOL m_bWork;

	// 生成的流ID
	CStringA m_strStreamId;
	CHAR m_szID[128];
	
	// 推流服务器IP
	std::vector<CStringA> m_vtList;
	// 推流服务器名字
	std::vector<CStringA> m_vtNameList;
	// 推流服务器显示
	std::vector<CStringA> m_vtShowList;

	// 拉流服务器IP
	std::vector<CStringA> m_vtPullList;
	// 拉流服务器Name
	std::vector<CStringA> m_vtPullNameList;
	// 拉流服务器流ID
	std::vector<CStringA> m_vtPullStreamList;
	// 拉流服务器显示
	std::vector<CStringA> m_vtPullShowList;
};
