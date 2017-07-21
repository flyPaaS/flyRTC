
// flyRTCDlg.cpp : implementation file
//

#include "stdafx.h"
#include "flyRTC.h"
#include "flyRTCDlg.h"
#include "afxdialogex.h"
#include "BaseData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// CflyRTCDlg dialog

CflyRTCDlg::CflyRTCDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CflyRTCDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	// 初始化
	m_nSelect = 0;
	m_nItemSelect = 0;
	m_bWork = false;
}

void CflyRTCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_ANSWER, m_btnAnswer);
	DDX_Control(pDX, IDC_BTN_DAIL, m_btnDail);
	DDX_Control(pDX, IDC_BTN_HANDUP, m_btnHandup);
	DDX_Control(pDX, IDC_BTN_LOGIN, m_btnLogin);
	DDX_Control(pDX, IDC_BTN_LOGINOUT, m_btnLoginOut);
	DDX_Control(pDX, IDC_LIST_CALL, m_ltCall);
	DDX_Control(pDX, IDC_LIST_CLIENT, m_ltClient);
	DDX_Control(pDX, IDC_STA_LOCAL, m_sLocalWnd);
	DDX_Control(pDX, IDC_STA_REMOTE, m_sRemoteWnd);
	DDX_Control(pDX, IDC_STA_TEXT, m_sText);
	DDX_Control(pDX, IDC_COMBO1, m_cVideoCode);
	DDX_Control(pDX, IDC_COMBO2, m_cAudioCode);
}

BEGIN_MESSAGE_MAP(CflyRTCDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_LOGIN, &CflyRTCDlg::OnBnClickedBtnLogin)
	ON_BN_CLICKED(IDC_BTN_LOGINOUT, &CflyRTCDlg::OnBnClickedBtnLoginout)
	ON_BN_CLICKED(IDC_BTN_DAIL, &CflyRTCDlg::OnBnClickedBtnDail)
	ON_BN_CLICKED(IDC_BTN_ANSWER, &CflyRTCDlg::OnBnClickedBtnAnswer)
	ON_BN_CLICKED(IDC_BTN_HANDUP, &CflyRTCDlg::OnBnClickedBtnHandup)
	ON_BN_CLICKED(IDC_RADIO1, &CflyRTCDlg::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &CflyRTCDlg::OnBnClickedRadio2)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CflyRTCDlg::OnCbnSelchangeCombo1)
	ON_CBN_SELCHANGE(IDC_COMBO2, &CflyRTCDlg::OnCbnSelchangeCombo2)
END_MESSAGE_MAP()

// CflyRTCDlg message handlers

BOOL CflyRTCDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	Init();
	InitCtrl();
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CflyRTCDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CflyRTCDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CflyRTCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CflyRTCDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: Add your message handler code here
	Free();
}

void CflyRTCDlg::OnBnClickedBtnLogin()
{
	// TODO: Add your control notification handler code here
	POSITION pos = m_ltClient.GetFirstSelectedItemPosition();
	if (pos != NULL)
	{
		int nItem = m_ltClient.GetNextSelectedItem(pos);
		// 区分
		if (m_nSelect == 0)
		{
			InitParam st;
			strcpy_s(st.accountSid, MAX_PATH, strsid);
			strcpy_s(st.accountToken, MAX_PATH, strtoken);
			strcpy_s(st.clientId, MAX_PATH, strClientIdSR[nItem]);
			strcpy_s(st.clientPwd, MAX_PATH, strClientPwdSR[nItem]);

			st.hLocalWnd = m_sLocalWnd.GetSafeHwnd();
			st.hRemoteWnd = m_sRemoteWnd.GetSafeHwnd();
			st.cbCallBackFunc = CallBack;
			Login(st);

			m_ltCall.DeleteAllItems();
			for (int i = 0; i < 16; i++)
			{
				if (i == nItem) {
					continue;
				}
				m_ltCall.InsertItem(m_ltCall.GetItemCount(), StringAToString(strClientIdSR[i]));
			}
			m_ltCall.SetItemState(0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
		}
		else
		{
			InitParam st;
			strcpy_s(st.accountSid, MAX_PATH, strsid);
			strcpy_s(st.accountToken, MAX_PATH, strtoken);
			strcpy_s(st.clientId, MAX_PATH, strClientIdCAN[nItem]);
			strcpy_s(st.clientPwd, MAX_PATH, strClientPwdCAN[nItem]);

			st.hLocalWnd = m_sLocalWnd.GetSafeHwnd();
			st.hRemoteWnd = m_sRemoteWnd.GetSafeHwnd();
			st.cbCallBackFunc = CallBack;
			Login(st);

			m_ltCall.DeleteAllItems();
			for (int i = 0; i < 16; i++)
			{
				if (i == nItem) {
					continue;
				}
				m_ltCall.InsertItem(m_ltCall.GetItemCount(), StringAToString(strClientIdCAN[i]));
			}
			m_ltCall.SetItemState(0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
		}
		//
		m_nItemSelect = nItem;
		m_btnLogin.EnableWindow(FALSE);
		m_btnLoginOut.EnableWindow(TRUE);
		m_ltClient.EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_RADIO1))->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_RADIO2))->EnableWindow(FALSE);

		m_btnDail.EnableWindow(TRUE);
		m_btnAnswer.EnableWindow(TRUE);
		m_btnHandup.EnableWindow(TRUE);

		m_sText.SetWindowText(L"登录中，请等待...");
		m_bWork = true;
	}
	else
	{
		m_sText.SetWindowText(L"请选择一个 Client Id 登录");
	}
}

void CflyRTCDlg::OnBnClickedBtnLoginout()
{
	// TODO: Add your control notification handler code here
	Hangup();
	LoginOut();
	//
	m_btnLogin.EnableWindow(TRUE);
	m_btnLoginOut.EnableWindow(FALSE);
	m_ltClient.EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_RADIO1))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_RADIO2))->EnableWindow(TRUE);

	m_btnDail.EnableWindow(FALSE);
	m_btnAnswer.EnableWindow(FALSE);
	m_btnHandup.EnableWindow(FALSE);
	m_ltCall.DeleteAllItems();

	m_sText.SetWindowText(L"Client 注销成功");
	m_bWork = false;
}

void CflyRTCDlg::OnBnClickedBtnDail()
{
	// TODO: Add your control notification handler code here
	POSITION pos = m_ltCall.GetFirstSelectedItemPosition();
	if (pos != NULL)
	{
		int nItem = m_ltCall.GetNextSelectedItem(pos);
		CString strCallId = m_ltCall.GetItemText(nItem, 0);
		if (Dialing(StringToStringA(strCallId).GetBuffer()) == 0)
		{
			m_sText.SetWindowText(L"呼叫中，请等待...");
		}
		else
		{
			m_sText.SetWindowText(L"呼叫失败...");
		}
	}
}

void CflyRTCDlg::OnBnClickedBtnAnswer()
{
	// TODO: Add your control notification handler code here
	Answer();
}

void CflyRTCDlg::OnBnClickedBtnHandup()
{
	// TODO: Add your control notification handler code here
	Hangup();
}

void CflyRTCDlg::OnBnClickedRadio1()
{
	// TODO: Add your control notification handler code here
	if (!m_bWork)
	{
		if (m_nSelect != 0)
		{
			m_ltClient.DeleteAllItems();
			for (int i = 0; i < 16; i++)
			{
				m_ltClient.InsertItem(m_ltClient.GetItemCount(), StringAToString(strClientIdSR[i]));
			}
			m_ltClient.SetItemState(m_nItemSelect, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
			m_nSelect = 0;
		}
	}
}

void CflyRTCDlg::OnBnClickedRadio2()
{
	// TODO: Add your control notification handler code here
	if (!m_bWork)
	{
		if (m_nSelect != 1)
		{
			m_ltClient.DeleteAllItems();
			for (int i = 0; i < 16; i++)
			{
				m_ltClient.InsertItem(m_ltClient.GetItemCount(), StringAToString(strClientIdCAN[i]));
			}
			m_ltClient.SetItemState(m_nItemSelect, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
			m_nSelect = 1;
		}
	}
}

void CflyRTCDlg::OnCbnSelchangeCombo1()
{
	// TODO: Add your control notification handler code here
	int nVideo = m_cVideoCode.GetCurSel();
	int nAudio = m_cAudioCode.GetCurSel();
	SetCode(nVideo, nAudio);
}

void CflyRTCDlg::OnCbnSelchangeCombo2()
{
	// TODO: Add your control notification handler code here
	int nVideo = m_cVideoCode.GetCurSel();
	int nAudio = m_cAudioCode.GetCurSel();
	SetCode(nVideo, nAudio);
}

void CflyRTCDlg::InitCtrl()
{
	((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(BST_CHECKED);
	((CButton*)GetDlgItem(IDC_RADIO2))->SetCheck(BST_UNCHECKED);
	m_nSelect = 0;

	DWORD dwStyle = m_ltClient.GetExtendedStyle();
	m_ltClient.SetExtendedStyle(dwStyle | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	//m_ltClient.InsertColumn(0, L"请选择一个CilentId", LVCFMT_CENTER);
	for (int i = 0; i < 16; i++)
	{
		m_ltClient.InsertItem(m_ltClient.GetItemCount(), StringAToString(strClientIdSR[i]));
	}
	m_ltClient.SetItemState(m_nItemSelect, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
	m_nItemSelect = 0;

	m_btnLoginOut.EnableWindow(FALSE);
	m_ltCall.DeleteAllItems();
	m_sText.SetWindowText(L"");
	m_btnDail.EnableWindow(FALSE);
	m_btnAnswer.EnableWindow(FALSE);
	m_btnHandup.EnableWindow(FALSE);

	m_cVideoCode.AddString(L"h264-sw");
	m_cVideoCode.AddString(L"h264-hw");
	m_cVideoCode.AddString(L"h265-sw");
	m_cVideoCode.AddString(L"h265-hw");
	m_cVideoCode.SetCurSel(0);
	m_cAudioCode.AddString(L"OPUS");
	m_cAudioCode.AddString(L"AAC");
	m_cAudioCode.SetCurSel(0);
	m_bWork = false;
}

void CallBack(int nEvent, int wParam, int lParam, char *pWParam, char *plParam)
{
	if (theApp.m_pMainWnd != NULL)
	{
		CflyRTCDlg *pWnd = (CflyRTCDlg *)theApp.m_pMainWnd;
		pWnd->CallBackAdapter(nEvent, wParam, lParam, pWParam, plParam);
	}
}

void CflyRTCDlg::CallBackAdapter(int nEvent, int wParam, int lParam, char *pWParam, char *plParam)
{
	if (nEvent == TCP_CONNECT)
	{
		OutputDebugString(L"TCP_CONNECT\r\n");
		//
		m_sText.SetWindowText(L"登录成功...");
	}
	if (nEvent == TCP_DISCONNECT)
	{
		OutputDebugString(L"TCP_DISCONNECT\r\n");
		//
		
		if (wParam == 0)
		{
			if (lParam == 0)
			{
				// 正常退出
			}
			if (lParam == 1)
			{
				// TCP连接失败,换一个服务器登录
				NextTcpConnect();
			}
			if (lParam == 2)
			{
				// TCP心跳失败,重连
				TcpConnect();
			}
		}
		else if (wParam == 1)
		{
			m_sText.SetWindowText(L"账号认证失败...");
		}
		else if (wParam == 2)
		{
			m_sText.SetWindowText(L"获取服务器地址失败...");
		}
		else if (wParam == 3)
		{
			m_sText.SetWindowText(L"获取信令服务器地址失败...");
		}
	}

	if (nEvent == CALL_OUTFAIL)
	{
		CString str;
		str.Format(L"呼叫失败...错误码%d", wParam);
		m_sText.SetWindowText(str);
	}
	if (nEvent == CALL_INCOME)
	{
		m_sText.SetWindowText(L"有新的来电...");
	}
	if (nEvent == CALL_ANSWER)
	{
		m_sText.SetWindowText(L"通话中...");
	}
	if (nEvent == CALL_HANDUP)
	{
		CString str;
		str.Format(L"呼叫挂断...结束码%d", wParam);
		m_sText.SetWindowText(str);
	}
	if (nEvent == CALL_ALERT)
	{
		m_sText.SetWindowText(L"对方振铃中...");
	}
	if (nEvent == CALL_VNETWORK)
	{
		CStringA str;
		str.Format("%s", pWParam);
		m_sText.SetWindowText(StringAToString(str));
	}
}