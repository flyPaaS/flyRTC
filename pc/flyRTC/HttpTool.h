#pragma once

class HttpTool
{
public:
	HttpTool();
	virtual ~HttpTool();

public:
	// HTTP Get 方式调用(UTF-8)
	BOOL HttpGet(CString strUrl, CStringA strData, CStringA &straResult);
	// HTTP Post 方式调用(UTF-8)
	BOOL HttpPost(CString strUrl, CStringA strData, CStringA &straResult);

private:
	// 分析URL正确性
	BOOL parseURL(LPCWSTR url);

	// 创建一个Session
	HINTERNET OpenSession(LPCWSTR userAgent = 0);
	// 与服务器连接
	HINTERNET Connect(HINTERNET hSession, LPCWSTR serverAddr, int portNo);
	// 发起请求
	HINTERNET OpenRequest(HINTERNET hConnect, LPCWSTR verb, LPCWSTR objectName, int scheme);
	// 增加头数据
	BOOL AddRequestHeaders(HINTERNET hRequest, LPCWSTR header);
	// 发送请求
	BOOL SendRequest(HINTERNET hRequest, const void* body, DWORD size, BOOL bGet = TRUE);
	// 结束请求，接收数据
	BOOL EndRequest(HINTERNET hRequest);
	// 读取返回头数据
	BOOL ReadHeader(HINTERNET hRequest, CStringA &straHead);
	// 读取返回数据
	BOOL ReadData(HINTERNET hRequest, CStringA &straData, CStringA &strError);
	// 关闭连接
	void CloseInternetHandle(HINTERNET hInternet);

private:
	// 连接参数
	int m_scheme;
	CString m_host;
	int m_port;
	CString m_path;
};

