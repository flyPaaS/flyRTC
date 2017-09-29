
// flyUDPDlg.cpp : implementation file
//

#include "stdafx.h"
#include "flyUDP.h"
#include "flyUDPDlg.h"
#include "afxdialogex.h"
#include "HttpTool.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_UPDATE_ITEM	(WM_USER + 100)
#define WM_VIGO_START	(WM_USER + 101)

struct MyStruct
{
	CflyUDPDlg *pBase;
	CStringA strSer;
	CStringA strSerName;
	CStringA strSerShow;
};

struct MyParam
{
	CStringA strUrl;
	int nAudio;
	int nVideo;
};

// 更新Tips
static void UpdateTipsCallBack(char *pTips)
{
	CStringA str;
	str.Format("%s\r\n", pTips);
	CflyUDPDlg *pDlg = (CflyUDPDlg *)theApp.m_pMainWnd;
	pDlg->m_sTip.SetWindowTextW(StringAToString(str));
}

// CflyUDPDlg dialog

CflyUDPDlg::CflyUDPDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CflyUDPDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	// 初始化
	m_bWork = FALSE;
	m_strStreamId = "";
	memset(m_szID, 0, sizeof(CHAR) * 128);

	m_nMode = 0;
	m_pCheckPush = NULL;
	m_pCheckPull = NULL;

	m_nStreamCan = 0;
	m_pCheck1 = NULL;
	m_pCheck2 = NULL;

	m_vtList.clear();
	m_vtNameList.clear();
	m_vtShowList.clear();

	m_vtPullList.clear();
	m_vtPullNameList.clear();
	m_vtPullStreamList.clear();
	m_vtPullShowList.clear();
}

void CflyUDPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SERVER, m_ltServer);
	DDX_Control(pDX, IDC_STA_LOCAL, m_sLocal);
	DDX_Control(pDX, IDC_STA_REMOTE, m_sRemote);
	DDX_Control(pDX, IDC_STA_TEXT, m_sTip);
	DDX_Control(pDX, IDC_BTN_RES, m_btnRes);
	DDX_Control(pDX, IDC_BTN_PUSH, m_btnStart);
}

BEGIN_MESSAGE_MAP(CflyUDPDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_PUSH, &CflyUDPDlg::OnBnClickedBtnPush)
	ON_BN_CLICKED(IDC_BTN_STOP, &CflyUDPDlg::OnBnClickedBtnStop)
	ON_BN_CLICKED(IDC_RADIO1, &CflyUDPDlg::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &CflyUDPDlg::OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_RADIO3, &CflyUDPDlg::OnBnClickedRadio3)
	ON_BN_CLICKED(IDC_RADIO4, &CflyUDPDlg::OnBnClickedRadio4)
	ON_BN_CLICKED(IDC_BTN_RES, &CflyUDPDlg::OnBnClickedBtnRes)
	ON_MESSAGE(WM_UPDATE_ITEM, &CflyUDPDlg::OnUpdate)
	ON_MESSAGE(WM_VIGO_START, &CflyUDPDlg::OnVigoStart)
END_MESSAGE_MAP()

// CflyUDPDlg message handlers

BOOL CflyUDPDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	DWORD dwStyle = m_ltServer.GetExtendedStyle();
	m_ltServer.SetExtendedStyle(dwStyle | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_ltServer.InsertColumn(0, L"选择会话中转服务器", LVCFMT_LEFT, 200);

	// 推拉流
	m_nMode = 0;
	m_pCheckPush = (CButton *)GetDlgItem(IDC_RADIO1);
	m_pCheckPull = (CButton *)GetDlgItem(IDC_RADIO2);
	m_pCheckPush->SetCheck(BST_CHECKED);
	m_pCheckPull->SetCheck(BST_UNCHECKED);
	// flyCan选择
	m_nStreamCan = 0;
	m_pCheck1 = (CButton *)GetDlgItem(IDC_RADIO3);
	m_pCheck2 = (CButton *)GetDlgItem(IDC_RADIO4);
	m_pCheck1->SetCheck(BST_CHECKED);
	m_pCheck2->SetCheck(BST_UNCHECKED);
	// 隐藏
	m_btnRes.ShowWindow(SW_HIDE);
	m_btnStart.SetWindowText(L"Push");
	// 生成流ID
	UpdateStreamId();
	// 启动
	Vigo_init(UpdateTipsCallBack);
	// 获取服务器列表
	HANDLE hHandle = ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)GetUdpThread, this, 0, NULL);
	CloseHandle(hHandle);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CflyUDPDlg::OnPaint()
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
HCURSOR CflyUDPDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CflyUDPDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: Add your message handler code here
	Vigo_destroy();
}

void CflyUDPDlg::OnBnClickedBtnPush()
{
	// TODO: Add your control notification handler code here
	m_pCheckPush->EnableWindow(FALSE);
	m_pCheckPull->EnableWindow(FALSE);
	m_ltServer.EnableWindow(FALSE);
	m_btnRes.EnableWindow(FALSE);
	m_pCheck1->EnableWindow(FALSE);
	m_pCheck2->EnableWindow(FALSE);
	m_btnStart.EnableWindow(FALSE);
	// 启动
	StartWorkThread();
}

void CflyUDPDlg::OnBnClickedBtnStop()
{
	// TODO: Add your control notification handler code here
	m_pCheckPush->EnableWindow(TRUE);
	m_pCheckPull->EnableWindow(TRUE);
	m_ltServer.EnableWindow(TRUE);
	m_btnRes.EnableWindow(TRUE);
	m_pCheck1->EnableWindow(TRUE);
	m_pCheck2->EnableWindow(TRUE);
	m_btnStart.EnableWindow(TRUE);
	// 退出播放
	if (m_bWork)
	{
		Vigo_stop();
		m_bWork = FALSE;
	}
	// 退出Session
	StartExitThread();
	// 更新流ID
	UpdateStreamId();
}

void CflyUDPDlg::OnBnClickedRadio1()
{
	// TODO: Add your control notification handler code here
	if (m_nMode == 0)
	{
		return;
	}
	m_pCheckPush->SetCheck(BST_CHECKED);
	m_pCheckPull->SetCheck(BST_UNCHECKED);
	m_btnRes.ShowWindow(SW_HIDE);
	m_btnStart.SetWindowText(L"Push");
	m_ltServer.DeleteColumn(0);
	m_ltServer.InsertColumn(0, L"选择会话中转服务器", LVCFMT_LEFT, 200);
	m_nMode = 0;
	// 刷新列表
	m_ltServer.DeleteAllItems();
	UpdateListCtrl();
}

void CflyUDPDlg::OnBnClickedRadio2()
{
	// TODO: Add your control notification handler code here
	if (m_nMode == 1)
	{
		return;
	}
	m_pCheckPush->SetCheck(BST_UNCHECKED);
	m_pCheckPull->SetCheck(BST_CHECKED);
	m_btnRes.ShowWindow(SW_NORMAL);
	m_btnStart.SetWindowText(L"Pull");
	m_ltServer.DeleteColumn(0);
	m_ltServer.InsertColumn(0, L"选择会话", LVCFMT_LEFT, 200);
	m_nMode = 1;
	// 刷新列表
	OnBnClickedBtnRes();
}

void CflyUDPDlg::OnBnClickedRadio3()
{
	// TODO: Add your control notification handler code here
	m_pCheck1->SetCheck(BST_CHECKED);
	m_pCheck2->SetCheck(BST_UNCHECKED);
	m_nStreamCan = 0;
}

void CflyUDPDlg::OnBnClickedRadio4()
{
	// TODO: Add your control notification handler code here
	m_pCheck1->SetCheck(BST_UNCHECKED);
	m_pCheck2->SetCheck(BST_CHECKED);
	m_nStreamCan = 1;
}

void CflyUDPDlg::OnBnClickedBtnRes()
{
	// TODO: Add your control notification handler code here
	m_ltServer.DeleteAllItems();
	m_vtPullList.clear();
	m_vtPullNameList.clear();
	m_vtPullStreamList.clear();
	m_vtPullShowList.clear();
	m_btnRes.EnableWindow(FALSE);
	// 启动线程
	HANDLE hHandle = ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)GetUdpPullThread, this, 0, NULL);
	CloseHandle(hHandle);
}

LRESULT CflyUDPDlg::OnUpdate(WPARAM wParam, LPARAM lParam)
{
	// 更新UI
	m_ltServer.DeleteAllItems();
	UpdateListCtrl();
	return 0;
}

LRESULT CflyUDPDlg::OnVigoStart(WPARAM wParam, LPARAM lParam)
{
	MyParam *pParam = (MyParam *)wParam;
	Vigo_start(pParam->strUrl.GetBuffer(), pParam->nAudio, pParam->nVideo, m_sLocal.GetSafeHwnd(), m_sRemote.GetSafeHwnd());
	m_bWork = TRUE;
	return 0;
}

void CflyUDPDlg::StartWorkThread()
{
	// 启动线程
	HANDLE hHandle = ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)WorkThread, this, 0, NULL);
	CloseHandle(hHandle);
}

void CflyUDPDlg::StartExitThread()
{
	// 启动线程
	HANDLE hHandle = ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ExitThread, this, 0, NULL);
	CloseHandle(hHandle);
}

DWORD WINAPI CflyUDPDlg::WorkThread(LPVOID lpParam)
{
	CflyUDPDlg *pBase = (CflyUDPDlg*)lpParam;

	POSITION pos = pBase->m_ltServer.GetFirstSelectedItemPosition();
	if (pos != NULL)
	{
		int nIndex = pBase->m_ltServer.GetNextSelectedItem(pos);
		if (pBase->m_nMode == 0)
		{
			// 推流
			CString ip = StringA2String(pBase->m_vtList[nIndex]);
			pBase->m_strStreamId.Format("%s", pBase->m_szID);

			CString url = L"http://";
			url += ip;
			url += L":8095/udprelay/v1/streamPush?";

			CStringA data = "?streamId=" + pBase->m_strStreamId;
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
			OutputDebugStringA("Push request\r\n");
			if (mHttpTool.HttpGet(url, data, straResult))
			{
				CStringA strUrl;
				CStringA strAPort;
				CStringA strVPort;
				
				std::string src;
				src.append(straResult);
				// 打印返回
				CStringA strTmp;
				strTmp.Format("Push request resp = %s\r\n", straResult);
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
									static MyParam param;
									param.strUrl = strUrl;
									param.nAudio = atoi(strAPort);
									param.nVideo = atoi(strVPort);
									pBase->SendMessage(WM_VIGO_START, (WPARAM)&param, 0);
									// 启动音视频
									//Vigo_start(strUrl.GetBuffer(), atoi(strAPort), atoi(strVPort), pBase->m_sLocal.GetSafeHwnd(), pBase->m_sRemote.GetSafeHwnd());
									//pBase->m_bWork = TRUE;
									// 更新标题
									CStringA str = pBase->m_vtNameList[nIndex] + "(" + pBase->m_strStreamId + ")";
									pBase->SetWindowText(StringA2String(str));
								}
							}
						}
					}
				}
				catch (Exception e)
				{
					// 打印返回
					strTmp.Format("Push resp json fail = %s\r\n", e.getDesc().c_str());
					OutputDebugStringA(strTmp);
				}
			}
		}
		if (pBase->m_nMode == 1)
		{
			CString ip = StringA2String(pBase->m_vtPullList[nIndex]);
			CStringA streamId = pBase->m_vtPullStreamList[nIndex];

			CString url = L"http://";
			url += ip;
			url += L":8095/udprelay/v1/streamPull?";

			CStringA data = "?streamId=" + streamId;
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
						if (pRoot->contain("result"))
						{
							JsonValue *pValue = pRoot->getValue("result");
							if (pValue != NULL)
							{
								std::string dst;
								pValue->writeToStr(dst);
								strUrl.Format("%s", dst.c_str());
								strUrl.Replace("\"", "");

								if (strUrl == "000002")
								{
									// ID被占用
									::AfxMessageBox(L"该StreamId被占用");
									pBase->m_pCheckPush->EnableWindow(TRUE);
									pBase->m_pCheckPull->EnableWindow(TRUE);
									pBase->m_ltServer.EnableWindow(TRUE);
									pBase->m_btnRes.EnableWindow(TRUE);
									pBase->m_pCheck1->EnableWindow(TRUE);
									pBase->m_pCheck2->EnableWindow(TRUE);
									pBase->m_btnStart.EnableWindow(TRUE);
									return 0;
								}
							}
						}
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
									static MyParam param;
									param.strUrl = strUrl;
									param.nAudio = atoi(strAPort);
									param.nVideo = atoi(strVPort);
									pBase->SendMessage(WM_VIGO_START, (WPARAM)&param, 0);
									// 启动音视频
									//Vigo_start(strUrl.GetBuffer(), atoi(strAPort), atoi(strVPort), pBase->m_sLocal.GetSafeHwnd(), pBase->m_sRemote.GetSafeHwnd());
									//pBase->m_bWork = TRUE;
									// 更新标题
									CStringA str = pBase->m_vtPullNameList[nIndex] + "(" + streamId + ")";
									pBase->SetWindowText(StringA2String(str));
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
		}
	}
	return 0;
}

DWORD WINAPI CflyUDPDlg::ExitThread(LPVOID lpParam)
{
	CflyUDPDlg *pBase = (CflyUDPDlg*)lpParam;

	POSITION pos = pBase->m_ltServer.GetFirstSelectedItemPosition();
	if (pos != NULL)
	{
		int nIndex = pBase->m_ltServer.GetNextSelectedItem(pos);
		if (pBase->m_nMode == 0)
		{
			CString ip = StringA2String(pBase->m_vtList[nIndex]);

			CString url = L"http://";
			url += ip;
			url += L":8095/udprelay/v1/streamRelease?";

			CStringA data = "?streamId=" + pBase->m_strStreamId;
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
			OutputDebugStringA("Push Release\r\n");
			if (mHttpTool.HttpGet(url, data, straResult))
			{
				std::string src;
				src.append(straResult);
				// 打印返回
				CStringA strTmp;
				strTmp.Format("Push Release resp = %s\r\n", straResult);
				OutputDebugStringA(strTmp);
			}
			// 更新UI
			pBase->m_sTip.Invalidate();
			pBase->m_sLocal.Invalidate();
			pBase->m_sRemote.Invalidate();
		}
		if (pBase->m_nMode == 1)
		{
			CString ip = StringA2String(pBase->m_vtPullList[nIndex]);
			CStringA streamId = pBase->m_vtPullStreamList[nIndex];

			CString url = L"http://";
			url += ip;
			url += L":";
			url += L"8095";
			url += L"/udprelay/v1/pullStop?";

			CStringA data = "?streamId=" + streamId;
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
			OutputDebugStringA("Pull Release\r\n");
			if (mHttpTool.HttpGet(url, data, straResult))
			{
				std::string src;
				src.append(straResult);
				// 打印返回
				CStringA strTmp;
				strTmp.Format("Pull Release resp = %s\r\n", straResult);
				OutputDebugStringA(strTmp);
			}
			// 更新UI
			pBase->m_sTip.Invalidate();
			pBase->m_sLocal.Invalidate();
			pBase->m_sRemote.Invalidate();
		}
	}
	return 0;
}

void CflyUDPDlg::UpdateUdpList()
{
	CString url = L"http://59.110.10.28:8097/v3/getudprelay?";
	CStringA data = "";
	CStringA straResult = "";
	OutputDebugStringA("UpdateUdpList start \r\n");

	HttpTool mHttpTool;
	if (mHttpTool.HttpGet(url, data, straResult))
	{
		std::string src;
		src.append(straResult);
		// 打印返回
		CStringA strTmp;
		strTmp.Format("UpdateUdpList resp = %s\r\n", straResult);
		OutputDebugStringA(strTmp);

		try
		{
			Json json(src);
			JsonDict *pRoot = json.getRoot()->toDict();
			if (pRoot != NULL)
			{
				if (pRoot->contain("result"))
				{
					JsonValue *pValue = pRoot->getValue("result");
					if (pValue != NULL)
					{
						std::string dst;
						pValue->writeToStr(dst);
						strTmp.Format("%s", dst.c_str());
						strTmp.Replace("\"", "");
						if (strTmp == "200")
						{
							if (pRoot->contain("ss"))
							{
								pValue = pRoot->getValue("ss");
								JsonList *pItem = pValue->toList();
								if (pItem != NULL)
								{
									int nCount = pItem->getCount();
									for (int i = 0; i < nCount; i++)
									{
										JsonDict *pIndex = pItem->getValue(i)->toDict();
										CStringA strName = "";
										CStringA strShow = "";
										if (pIndex->contain("ssip"))
										{
											pValue = pIndex->getValue("ssip");
											std::string dst;
											pValue->writeToStr(dst);
											strName.Format("%s", dst.c_str());
											strName.Replace("\"", "");
											// 增加到列表
											m_vtList.push_back(strName);
										}
										if (pIndex->contain("ssname"))
										{
											pValue = pIndex->getValue("ssname");
											std::string dst;
											pValue->writeToStr(dst);
											strShow.Format("%s", dst.c_str());
											strShow.Replace("\"", "");
											// 增加到列表
											m_vtNameList.push_back(strShow);

											// 增加到列表
											strShow = strName + " (" + strShow + ")";
											m_vtShowList.push_back(strShow);
										}
									}
								}
							}
						}
					}
				}
			}
		}
		catch (Exception e)
		{
			// 打印返回
			strTmp.Format("UpdateUdpList resp json fail = %s\r\n", e.getDesc().c_str());
			OutputDebugStringA(strTmp);
		}
	}
}

DWORD WINAPI CflyUDPDlg::GetUdpThread(LPVOID lpParam)
{
	CflyUDPDlg *pBase = (CflyUDPDlg*)lpParam;
	// 获取推拉流服务器地址
	pBase->UpdateUdpList();
	// 直接更新UI
	pBase->UpdateListCtrl();
	return 0;
}

void CflyUDPDlg::UpdatePullServer(CStringA strSer, CStringA strSerShow, CStringA strSerName)
{
	MyStruct *pNew = new MyStruct();
	pNew->pBase = this;
	pNew->strSer = strSer;
	pNew->strSerName = strSerName;
	pNew->strSerShow = strSerShow;
	HANDLE hHandle = ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)GetUdpPullOneThread, pNew, 0, NULL);
	CloseHandle(hHandle);
}

DWORD WINAPI CflyUDPDlg::GetUdpPullThread(LPVOID lpParam)
{
	CflyUDPDlg *pBase = (CflyUDPDlg*)lpParam;
	// 获取拉流服务器地址
	for (size_t i = 0; i < pBase->m_vtList.size(); i++)
	{
		pBase->UpdatePullServer(pBase->m_vtList[i], pBase->m_vtShowList[i], pBase->m_vtNameList[i]);
	}
	pBase->UpdateListCtrl();
	pBase->m_btnRes.EnableWindow(TRUE);
	return 0;
}

DWORD WINAPI CflyUDPDlg::GetUdpPullOneThread(LPVOID lpParam)
{
	MyStruct *pNew = (MyStruct *)lpParam;
	if (pNew != NULL)
	{
		CflyUDPDlg *pBase = pNew->pBase;
		CStringA strSer = pNew->strSer;
		CStringA strSerName = pNew->strSerName;
		CStringA strSerShow = pNew->strSerShow;

		CString url = L"http://" + StringA2String(strSer) + ":8095/udprelay/v1/getStreamList?";
		CStringA data = "";
		CStringA straResult = "";

		CStringA strTmp;
		strTmp.Format("UpdatePullServer %s start \r\n", strSer);
		OutputDebugStringA(strTmp);

		HttpTool mHttpTool;
		if (mHttpTool.HttpGet(url, data, straResult))
		{
			std::string src;
			src.append(straResult);
			// 打印返回
			CStringA strTmp;
			strTmp.Format("UpdatePullServer %s resp = %s\r\n", strSer, straResult);
			OutputDebugStringA(strTmp);

			try
			{
				Json json(src);
				JsonDict *pRoot = json.getRoot()->toDict();
				if (pRoot != NULL)
				{
					if (pRoot->contain("cnt"))
					{
						JsonValue *pValue = pRoot->getValue("cnt");
						if (pValue != NULL)
						{
							std::string dst;
							pValue->writeToStr(dst);
							strTmp.Format("%s", dst.c_str());
							strTmp.Replace("\"", "");

							int nCount = atoi(strTmp);
							if (nCount > 0)
							{
								if (pRoot->contain("list"))
								{
									pValue = pRoot->getValue("list");
									JsonList *pItem = pValue->toList();
									if (pItem != NULL)
									{
										int nCount = pItem->getCount();
										for (int i = 0; i < nCount; i++)
										{
											JsonValue *pIndex = pItem->getValue(i);
											CStringA strName = "";
											CStringA strShow = "";
											std::string dst;
											pIndex->writeToStr(dst);
											strName.Format("%s", dst.c_str());
											strName.Replace("\"", "");

											// 增加到列表
											pBase->m_vtPullList.push_back(strSer);
											pBase->m_vtPullNameList.push_back(strSerName);
											pBase->m_vtPullStreamList.push_back(strName);
											strShow = strSerShow + " (" + strName + ")";
											pBase->m_vtPullShowList.push_back(strShow);
										}
										// 发送消息
										pBase->SendMessage(WM_UPDATE_ITEM, 0, 0);
									}
								}
							}
						}
					}
				}
			}
			catch (Exception e)
			{
				// 打印返回
				strTmp.Format("UpdatePullServer %s resp json fail = %s\r\n", strSer, e.getDesc().c_str());
				OutputDebugStringA(strTmp);
			}
		}
		delete pNew;
	}
	return 0;
}

void CflyUDPDlg::UpdateListCtrl()
{
	if (m_nMode == 0)
	{
		 // 推流
		for (size_t i = 0; i < m_vtShowList.size(); i++)
		{
			CString str = StringA2String(m_vtShowList[i]);
			m_ltServer.InsertItem(i, str);
		}
		if (m_ltServer.GetItemCount() > 0)
		{
			m_ltServer.SetItemState(0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
		}
	}
	if (m_nMode == 1)
	{
		// 拉流
		for (size_t i = 0; i < m_vtPullShowList.size(); i++)
		{
			CString str = StringA2String(m_vtPullShowList[i]);
			m_ltServer.InsertItem(i, str);
		}
		if (m_ltServer.GetItemCount() > 0)
		{
			m_ltServer.SetItemState(0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
		}
	}
}

void CflyUDPDlg::UpdateStreamId()
{
	// 生成流ID
	DWORD dwID = ::GetTickCount();
	DWORD nId = (dwID & 0x0000FFFF);
	_itoa_s(nId, m_szID, 10);

	CString str;
	str.Format(L"UpdateStreamId %d, %d, %s\r\n", dwID, nId, m_szID);
	OutputDebugString(str);
}