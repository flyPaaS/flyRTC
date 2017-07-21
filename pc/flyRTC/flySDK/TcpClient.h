#pragma once

class CTcpClient
{
public:
	CTcpClient();
	virtual ~CTcpClient();

public:
	// 加载
	void Init();
	// 释放
	void Free();

	bool Connect(char *server, int port);
	// 发送数据
	bool Send(char *read, int nSize);
	// 接收数据
	bool Receive(char *read, int nSize);

private:
	// 连接标记
	bool m_bInit;
	// 连接句柄
	SOCKET m_socket;
	// 操作锁
	CRITICAL_SECTION m_csLock;
};

