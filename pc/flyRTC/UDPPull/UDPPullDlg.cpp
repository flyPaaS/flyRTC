
// UDPPullDlg.cpp : implementation file
//

#include "stdafx.h"
#include "UDPPull.h"
#include "UDPPullDlg.h"
#include "afxdialogex.h"
#include "HttpTool.h"

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


static void UpdateTipsCallBack(char *pTips)
{
	CStringA str;
	str.Format("%s\r\n", pTips);
	CUDPPullDlg *pDlg = (CUDPPullDlg *)theApp.m_pMainWnd;
	pDlg->m_sTip.SetWindowTextW(StringAToString(str));
}

// CUDPPullDlg dialog

CUDPPullDlg::CUDPPullDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUDPPullDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	//
	m_nStreamCan = 0;
	m_bWork = FALSE;
}

void CUDPPullDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_IP, m_edIp);
	DDX_Control(pDX, IDC_EDIT_PORT, m_edPort);
	DDX_Control(pDX, IDC_STA_LOCAL, m_sLocal);
	DDX_Control(pDX, IDC_STA_REMOTE, m_sRemote);
	DDX_Control(pDX, IDC_EDIT_SID, m_sStreamId);
	DDX_Control(pDX, IDC_STA_TIPS, m_sTip);
}

BEGIN_MESSAGE_MAP(CUDPPullDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_PULL, &CUDPPullDlg::OnBnClickedBtnPull)
	ON_BN_CLICKED(IDC_BTN_STOP, &CUDPPullDlg::OnBnClickedBtnStop)
	ON_BN_CLICKED(IDC_RADIO1, &CUDPPullDlg::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &CUDPPullDlg::OnBnClickedRadio2)
END_MESSAGE_MAP()

// CUDPPullDlg message handlers

BOOL CUDPPullDlg::OnInitDialog()
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
	m_edIp.SetWindowTextW(L"183.60.189.169");
	m_edPort.SetWindowTextW(L"8095");
	m_sStreamId.SetWindowTextW(L"");

	m_pCheck1 = (CButton *)GetDlgItem(IDC_RADIO1);
	m_pCheck2 = (CButton *)GetDlgItem(IDC_RADIO2);
	m_pCheck1->SetCheck(BST_CHECKED);
	m_pCheck2->SetCheck(BST_UNCHECKED);
	// 启动
	Vigo_init(UpdateTipsCallBack);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CUDPPullDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CUDPPullDlg::OnPaint()
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
HCURSOR CUDPPullDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CUDPPullDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: Add your message handler code here
	//Vigo_destroy();
}

void CUDPPullDlg::OnBnClickedBtnPull()
{
	// TODO: Add your control notification handler code here
	StartWorkThread();
}

void CUDPPullDlg::OnBnClickedBtnStop()
{
	// TODO: Add your control notification handler code here
	if (m_bWork)
	{
		Vigo_stop();
		m_bWork = FALSE;
	}
}

void CUDPPullDlg::OnBnClickedRadio1()
{
	// TODO: Add your control notification handler code here
	m_pCheck1->SetCheck(BST_CHECKED);
	m_pCheck2->SetCheck(BST_UNCHECKED);
	m_nStreamCan = 0;
}

void CUDPPullDlg::OnBnClickedRadio2()
{
	// TODO: Add your control notification handler code here
	m_pCheck1->SetCheck(BST_UNCHECKED);
	m_pCheck2->SetCheck(BST_CHECKED);
	m_nStreamCan = 1;
}

void CUDPPullDlg::StartWorkThread()
{
	// 启动线程
	HANDLE hHandle = ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)WorkThread, this, 0, NULL);
	CloseHandle(hHandle);
}

DWORD WINAPI CUDPPullDlg::WorkThread(LPVOID lpParam)
{
	CUDPPullDlg *pBase = (CUDPPullDlg*)lpParam;

	CString ip, port, streamId;
	pBase->m_edIp.GetWindowTextW(ip);
	pBase->m_edPort.GetWindowTextW(port);
	pBase->m_sStreamId.GetWindowTextW(streamId);

	CString url = L"http://";
	url += ip;
	url += L":";
	url += port;
	url += L"/udprelay/v1/streamPull?";

	CStringA data = "?streamId=" + StringToStringA(streamId);
	if (pBase->m_nStreamCan == 0)
	{
		data += "&streamCan=0";
	}
	else
	{
		data += "&streamCan=1";
	}

	HttpTool mHttpTool;
	CStringA straResult = "";
	OutputDebugStringA("Pull request\r\n");
	if (mHttpTool.HttpGet(url, data, straResult))
	{
		CStringA strUrl;
		CStringA strAPort;
		CStringA strVPort;

		std::string src;
		src.append(straResult);
		// 打印返回
		CStringA strTmp;
		strTmp.Format("Pull request resp = %s\r\n", straResult);
		OutputDebugStringA(strTmp);

		try
		{
			Json json(src);
			JsonDict *pRoot = json.getRoot()->toDict();
			if (pRoot != NULL)
			{
				if (pRoot->contain("apinfo"))
				{
					JsonValue *pValue = pRoot->getValue("apinfo");
					if (pValue != NULL)
					{
						JsonDict *pItem = pValue->toDict();
						if (pItem != NULL)
						{
							if (pItem->contain("ip"))
							{
								pValue = pItem->getValue("ip");
								std::string dst;
								pValue->writeToStr(dst);
								strUrl.Format("%s", dst.c_str());
								strUrl.Replace("\"", "");
							}
							if (pItem->contain("aport"))
							{
								pValue = pItem->getValue("aport");
								std::string dst;
								pValue->writeToStr(dst);
								strAPort.Format("%s", dst.c_str());
								strAPort.Replace("\"", "");
							}
							if (pItem->contain("vport"))
							{
								pValue = pItem->getValue("vport");
								std::string dst;
								pValue->writeToStr(dst);
								strVPort.Format("%s", dst.c_str());
								strVPort.Replace("\"", "");
							}
							// 启动音视频
							pBase->OnBnClickedBtnStop();
							Vigo_start(strUrl.GetBuffer(), atoi(strAPort), atoi(strVPort), pBase->m_sLocal.GetSafeHwnd(), pBase->m_sRemote.GetSafeHwnd());
							pBase->m_bWork = TRUE;
						}
					}
				}
			}
		}
		catch (Exception e)
		{
			// 打印返回
			strTmp.Format("Pull request json fail = %s\r\n", e.getDesc().c_str());
			OutputDebugStringA(strTmp);
		}
	}
	return 0;
}

