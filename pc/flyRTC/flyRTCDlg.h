
// flyRTCDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"

// 回调函数
static void CallBack(int nEvent, int wParam, int lParam, char *pWParam, char *plParam);

// CflyRTCDlg dialog
class CflyRTCDlg : public CDialog
{
// Construction
public:
	CflyRTCDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_FLYRTC_DIALOG };

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
	afx_msg void OnBnClickedBtnLogin();
	afx_msg void OnBnClickedBtnLoginout();
	afx_msg void OnBnClickedBtnDail();
	afx_msg void OnBnClickedBtnAnswer();
	afx_msg void OnBnClickedBtnHandup();
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnCbnSelchangeCombo2();

public:
	// 初始化控件
	void InitCtrl();
	// 回调函数处理
	void CallBackAdapter(int nEvent, int wParam, int lParam, char *pWParam, char *plParam);

public:
	// 平台选择
	int m_nSelect;
	// 列表选择
	int m_nItemSelect;
	// 登录标记
	bool m_bWork;

	// 接听电话
	CButton m_btnAnswer;
	// 拨打电话
	CButton m_btnDail;
	// 挂断电话
	CButton m_btnHandup;
	// 登录
	CButton m_btnLogin;
	// 注销
	CButton m_btnLoginOut;
	// 可以拨打的电话号码列表
	CListCtrl m_ltCall;
	// 客户端ID列表
	CListCtrl m_ltClient;
	// 本地视频窗口
	CStatic m_sLocalWnd;
	// 远程视频窗口
	CStatic m_sRemoteWnd;
	// 文本显示窗口
	CStatic m_sText;
	// Codec
	CComboBox m_cVideoCode;
	CComboBox m_cAudioCode;
};
