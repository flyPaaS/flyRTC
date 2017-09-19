#include "stdafx.h"
#include "TcpClient.h"

CTcpClient::CTcpClient()
{
	m_bInit = false;
	m_socket = INVALID_SOCKET;
	InitializeCriticalSection(&m_csLock);
}

CTcpClient::~CTcpClient()
{
	DeleteCriticalSection(&m_csLock);
}

void CTcpClient::Init()
{
	// ÏÈÊÍ·Å
	Free();
	// ¼ÓÔØ
	EnterCriticalSection(&m_csLock);
	WSADATA wsaData = {0};
	WORD wVersionRequested = MAKEWORD(2, 2);
	int err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)
	{
		LeaveCriticalSection(&m_csLock);
		return;
	}
	if (LOBYTE(wsaData.wVersion) != 2 && HIBYTE(wsaData.wVersion) != 2) 
	{
		WSACleanup();
		LeaveCriticalSection(&m_csLock);
		return;
	}

	m_socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_socket == INVALID_SOCKET)
	{
		WSACleanup();
		LeaveCriticalSection(&m_csLock);
		return;
	}
	m_bInit = true;
	LeaveCriticalSection(&m_csLock);
}

void CTcpClient::Free()
{
	EnterCriticalSection(&m_csLock);
	if (m_bInit)
	{
		::closesocket(m_socket);
		m_socket = INVALID_SOCKET;
		WSACleanup();
	}
	LeaveCriticalSection(&m_csLock);
}

bool CTcpClient::Connect(char *server, int port)
{
	EnterCriticalSection(&m_csLock);
	if (m_bInit)
	{
		sockaddr_in servAddr;
		servAddr.sin_family = AF_INET;
		servAddr.sin_port = htons(port);
		servAddr.sin_addr.S_un.S_addr = inet_addr(server);

		if (::connect(m_socket, (sockaddr*)&servAddr, sizeof(sockaddr)) == 0)
		{
			LeaveCriticalSection(&m_csLock);
			return true;
		}
	}
	LeaveCriticalSection(&m_csLock);
	return false;
}

bool CTcpClient::Send(char *send, int nSize)
{
	EnterCriticalSection(&m_csLock);
	if (m_bInit)
	{
		int nSend = ::send(m_socket, send, nSize, 0);
		if (nSend == nSize)
		{
			LeaveCriticalSection(&m_csLock);
			return true;
		}
	}
	LeaveCriticalSection(&m_csLock);
	return false;
}

bool CTcpClient::Receive(char *read, int nSize)
{
	if (m_bInit)
	{
		int nRecv = ::recv(m_socket, read, nSize, 0);
		if (nRecv > 0)
		{
			read[nRecv] = '\0';
			return true;
		}
	}
	return false;
}