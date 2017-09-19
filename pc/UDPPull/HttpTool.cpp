#include "stdafx.h"
#include "HttpTool.h"

HttpTool::HttpTool()
{
	m_scheme = 0;
	m_host.Empty();
	m_port = 0;
	m_path.Empty();
}

HttpTool::~HttpTool()
{

}

BOOL HttpTool::HttpGet(CString strUrl, CStringA strData, CStringA &straResult)
{
	// 连接句柄
	HINTERNET hSession = NULL;
	HINTERNET hConnect = NULL;
	HINTERNET hRequest = NULL;
	// 分析数据
	if (parseURL(strUrl))
	{
		// Open session
		hSession = OpenSession();
		if (hSession != NULL)
		{
			// Connect
			hConnect = Connect(hSession, m_host, m_port);
			if (hConnect != NULL)
			{
				// Open request
				hRequest = OpenRequest(hConnect, L"GET", m_path + StringA2String(strData), m_scheme);
				if (hRequest != NULL)
				{
					// Add request header
					CString strHeader(L"Content-type: application/x-www-form-urlencoded\r\n");
					if (AddRequestHeaders(hRequest, strHeader))
					{
						// Send post data
						if (SendRequest(hRequest, "", 0))
						{
							// End request
							if (EndRequest(hRequest))
							{
								CStringA stra;
								if (ReadHeader(hRequest, stra))
								{
									CStringA strCode = "";
									if (ReadData(hRequest, straResult, strCode))
									{
										// 关闭连接
										CloseInternetHandle(hRequest);
										CloseInternetHandle(hConnect);
										CloseInternetHandle(hSession);
										return TRUE;
									}
									else
									{
										OutputDebugStringA("resp Error = ");
										OutputDebugStringA(strCode);
										OutputDebugStringA("\r\n");
									}
								}
							}
						}
					}
				}
			}
		}
	}
	// 关闭连接
	CloseInternetHandle(hRequest);
	CloseInternetHandle(hConnect);
	CloseInternetHandle(hSession);
	return FALSE;
}

BOOL HttpTool::HttpPost(CString strUrl, CStringA strData, CStringA &straResult)
{
	// 连接句柄
	HINTERNET hSession = NULL;
	HINTERNET hConnect = NULL;
	HINTERNET hRequest = NULL;
	// 分析数据
	if (parseURL(strUrl))
	{
		// Open session
		hSession = OpenSession();
		if (hSession != NULL)
		{
			// Connect
			hConnect = Connect(hSession, m_host, m_port);
			if (hConnect != NULL)
			{
				// Open request
				hRequest = OpenRequest(hConnect, L"POST", m_path, m_scheme);
				if (hRequest != NULL)
				{
					// Add request header
					CString strHeader(L"Content-type: application/x-www-form-urlencoded\r\n");
					if (AddRequestHeaders(hRequest, strHeader))
					{
						// Send post data
						if (SendRequest(hRequest, strData, strData.GetLength(), FALSE))
						{
							// End request
							if (EndRequest(hRequest))
							{
								CStringA stra;
								if (ReadHeader(hRequest, stra))
								{
									CStringA strCode = "";
									if (ReadData(hRequest, straResult, strCode))
									{
										// 关闭连接
										CloseInternetHandle(hRequest);
										CloseInternetHandle(hConnect);
										CloseInternetHandle(hSession);
										return TRUE;
									}
									else
									{
										OutputDebugStringA("resp Error = ");
										OutputDebugStringA(strCode);
										OutputDebugStringA("\r\n");
									}
								}
							}
						}
					}
				}
			}
		}
	}
	// 关闭连接
	CloseInternetHandle(hRequest);
	CloseInternetHandle(hConnect);
	CloseInternetHandle(hSession);
	return FALSE;
}

BOOL HttpTool::parseURL(LPCWSTR url)
{
	URL_COMPONENTS uc = { 0 };
	uc.dwStructSize = sizeof(uc);
	const DWORD BUF_LEN = 256;
	WCHAR host[BUF_LEN];
	uc.lpszHostName = host;
	uc.dwHostNameLength = BUF_LEN;
	WCHAR path[BUF_LEN];
	uc.lpszUrlPath = path;
	uc.dwUrlPathLength = BUF_LEN;
	WCHAR extra[BUF_LEN];
	uc.lpszExtraInfo = extra;
	uc.dwExtraInfoLength = BUF_LEN;
	if (!WinHttpCrackUrl(url, 0, ICU_ESCAPE, &uc))
	{
		return FALSE;
	}
	m_scheme = uc.nScheme;
	m_host = host;
	m_port = uc.nPort;
	m_path = path;
	return TRUE;
}

HINTERNET HttpTool::OpenSession(LPCWSTR userAgent)
{
	return WinHttpOpen(userAgent, WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
}

HINTERNET HttpTool::Connect(HINTERNET hSession, LPCWSTR serverAddr, int portNo)
{
	return WinHttpConnect(hSession, serverAddr, (INTERNET_PORT)portNo, 0);
}

HINTERNET HttpTool::OpenRequest(HINTERNET hConnect, LPCWSTR verb, LPCWSTR objectName, int scheme)
{
	DWORD flags = 0;
	if (scheme == INTERNET_SCHEME_HTTPS)
	{
		flags |= WINHTTP_FLAG_SECURE;
	}
	return WinHttpOpenRequest(hConnect, verb, objectName, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, flags);
}

BOOL HttpTool::AddRequestHeaders(HINTERNET hRequest, LPCWSTR header)
{
	SIZE_T len = lstrlenW(header);
	return WinHttpAddRequestHeaders(hRequest, header, DWORD(len), WINHTTP_ADDREQ_FLAG_ADD);
}

BOOL HttpTool::SendRequest(HINTERNET hRequest, const void* body, DWORD size, BOOL bGet)
{
	if (bGet)
	{
		return WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);
	}
	else
	{
		return WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, const_cast<void*>(body), size, size, 0);
	}
}

BOOL HttpTool::EndRequest(HINTERNET hRequest)
{
	return WinHttpReceiveResponse(hRequest, 0);
}

BOOL HttpTool::ReadHeader(HINTERNET hRequest, CStringA &straHead)
{
	DWORD dwSize = 0;
	LPVOID lpHeaderBuffer = NULL;

	// 获取header的长度
	WinHttpQueryHeaders(hRequest,
		WINHTTP_QUERY_RAW_HEADERS_CRLF,
		WINHTTP_HEADER_NAME_BY_INDEX,
		NULL, &dwSize, WINHTTP_NO_HEADER_INDEX);

	// 根据header的长度为buffer申请内存空间
	if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
	{
		lpHeaderBuffer = new WCHAR[dwSize / sizeof(WCHAR)];

		// 使用WinHttpQueryHeaders获取header信息
		if (WinHttpQueryHeaders(hRequest,
			WINHTTP_QUERY_RAW_HEADERS_CRLF,
			WINHTTP_HEADER_NAME_BY_INDEX,
			lpHeaderBuffer, &dwSize,
			WINHTTP_NO_HEADER_INDEX))
		{
			straHead.Format("%s", lpHeaderBuffer);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL HttpTool::ReadData(HINTERNET hRequest, CStringA &straData, CStringA &strError)
{
	DWORD dwSize = 0;
	DWORD dwDownloaded = 0;
	LPSTR pszOutBuffer = NULL;

	do
	{
		// 获取返回数据的大小
		dwSize = 0;
		if (!WinHttpQueryDataAvailable(hRequest, &dwSize))
		{
			strError.Format("Error：WinHttpQueryDataAvailable failed：%d\r\n", GetLastError());
			return FALSE;
		}

		if (!dwSize)
		{
			break;
		}

		// 根据返回数据的长度为buffer申请内存空间
		pszOutBuffer = new char[dwSize + 1];
		if (!pszOutBuffer)
		{
			strError = "Out of memory.";
			return FALSE;
		}

		// 通过WinHttpReadData读取服务器的返回数据
		ZeroMemory(pszOutBuffer, dwSize + 1);
		if (!WinHttpReadData(hRequest, pszOutBuffer, dwSize, &dwDownloaded))
		{
			strError.Format("Error：WinHttpQueryDataAvailable failed：%d\r\n", GetLastError());
		}

		if (!dwDownloaded)
		{
			break;
		}

		CStringA stra;
		stra.Format("%s", pszOutBuffer);
		straData += stra;
	} while (dwSize > 0);
	return TRUE;
}

void HttpTool::CloseInternetHandle(HINTERNET hInternet)
{
	if (hInternet != NULL)
	{
		WinHttpCloseHandle(hInternet);
		hInternet = NULL;
	}
}