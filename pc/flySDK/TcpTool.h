#pragma once
#include "TcpClient.h"

class CFlyBase;
class TcpTool
{
public:
	TcpTool();
	virtual ~TcpTool();

public:
	// 启动TCP模块
	void StartTcp(char *pServer, int nPort);
	// 停止TCP模块
	void StopTcp();
	
private:
	// 发送版本号握手
	void SendVersion();
	// 发送登录包
	void SendLogin();
	// 发送心跳包
	bool SendHeat();
	// 发送退录包
	void SendLoginOut();

	// TCP心跳线程
	DWORD static WINAPI TcpWorkThread(LPVOID lpParam);
	// TCP接收线程
	DWORD static WINAPI TcpRecvThread(LPVOID lpParam);
	
public:
	// 上层对象
	CFlyBase *m_pBase;
	// TCP连接对象
	CTcpClient m_cTcpClient;

	// 连接状态
	int m_nStatus;
	// 线程标记
	HANDLE m_hWorkEvent;
	HANDLE m_hRecvEvent;
	HANDLE m_hWorkThread;
	HANDLE m_hRecvThread;

	// 记录返回随机码
	int m_nRand;
	// 心跳回应
	HANDLE m_hEvent;
};

