#pragma once
#include "TcpTool.h"

class CFlyBase
{
public:
	CFlyBase();
	virtual ~CFlyBase();

public:
	// 加载SDK
	BOOL Init();
	// 释放SDK
	void Free();

	// 登录
	void Login(InitParam stParam);
	// 注销
	void LoginOut();

	// TCP重连
	void TcpConnect();
	// TCP换一个CS连接
	void NextTcpConnect();

	// 拨打电话
	int Dialing(CStringA strUid);
	// 接听电话
	int Answer();
	// 挂断电话
	int Hangup();

	// 设置编解码参数
	int SetCode(int nVideoCodec, int nAudioCodec);

	// 播放音频文件
	void StartPlayfile(char* pchFilePaht, int iFormat, int iDirect, int iLoop);
	// 停止播放
	void StopPlayfile();

private:
	// 和服务器握手成功后，更新组件配置参数
	void UpdateParam();
	// 计算出RTT后，更新RTPP配置参数
	void UpdateRtppParam();

	// IM账号登录
	void IMLogin();
	void IMLoginOut();

	// AMS认证
	BOOL LoginAMS();
	// 获取CS,RTPP的获取地址
	BOOL GetUrl();
	// 登录CS
	BOOL GetCS();
	// 登录RTPP
	BOOL GetRTPP();

	// 打开关闭线程
	void StartWorkThread();
	void StopWorkThread();
	// 线程函数
	DWORD static WINAPI WorkThread(LPVOID lpParam);

public:
	// 加载成功标记
	bool m_bLoad;
	// 登录标记
	bool m_bWork;
	bool m_bGetUrl;

	// 选择的CS服务器Index
	int m_nSelectCS;

	// 通话状态
	int m_nCallStatus;
	// 呼叫id
	CStringA m_strCallId;
	
	// 视频窗口
	HWND m_hLocalWnd;
	HWND m_hRemoteWnd;

	// 线程句柄
	HANDLE m_hEvent;
	HANDLE m_hThread;

	// 回调函数
	CallBackFunc m_CallBackFunc;

	// 保存登录账号信息
	char m_accountSid[MAX_PATH];
	char m_accountToken[MAX_PATH];
	char m_clientId[MAX_PATH];
	char m_clientPwd[MAX_PATH];

	// 保存AMS认证返回的数据
	char m_phone[MAX_PATH];
	char m_ssid[MAX_PATH];
	char m_uid[MAX_PATH];
	char m_im_ssid[MAX_PATH];
	char m_name[MAX_PATH];
	char m_forwardnumber[MAX_PATH];

	// 服务器地址
	std::vector<CStringA> m_csAddress;
	std::vector<CStringA> m_rtppAddress;
	CStringA m_stumAddress;

	// TCP连接对象
	TcpTool m_cTcp;
};

