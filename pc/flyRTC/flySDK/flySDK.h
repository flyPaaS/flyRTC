#pragma once

#ifdef DLLProvider
#define DLL_EXPORT_IMPORT extern "C" __declspec(dllexport)
#else
#define DLL_EXPORT_IMPORT extern "C" __declspec(dllimport)
#endif

// Event Type
#define CALL_OUTFAIL	1
#define CALL_INCOME		2
#define CALL_ANSWER		3
#define CALL_HANDUP		4
#define CALL_ALERT		5
#define CALL_ANETWORK	6
#define CALL_VNETWORK	7
#define CALL_CAPTURE	8

#define TCP_CONNECT		10
#define TCP_DISCONNECT	11

// 事件回调
typedef void (*CallBackFunc)(int nEvent, int wParam, int lParam, char *pWParam, char *plParam);

// 初始化参数
struct InitParam
{
	// 账户参数
	char accountSid[MAX_PATH];
	char accountToken[MAX_PATH];
	char clientId[MAX_PATH];
	char clientPwd[MAX_PATH];

	HWND hLocalWnd;
	HWND hRemoteWnd;

	CallBackFunc cbCallBackFunc;
};

// 初始化SDK
DLL_EXPORT_IMPORT void Init();
// 释放SDK
DLL_EXPORT_IMPORT void Free();

// 登录，返回值看回调函数CallBackFunc
DLL_EXPORT_IMPORT void Login(InitParam param);
// 注销
DLL_EXPORT_IMPORT void LoginOut();

// TCP连接(重连)
DLL_EXPORT_IMPORT void TcpConnect();
// TCP换一个服务器连接
DLL_EXPORT_IMPORT void NextTcpConnect();

// 拨打电话
DLL_EXPORT_IMPORT int Dialing(char *pUid);
// 接听电话
DLL_EXPORT_IMPORT int Answer();
// 挂断电话
DLL_EXPORT_IMPORT int Hangup();

// 设置音视频编码
// nVideoCodec : 0 h264sw
// nVideoCodec : 1 h264hw
// nVideoCodec : 2 h265sw
// nVideoCodec : 3 h265hw
// nAudioCodec : 0 OPUS
// nAudioCodec : 1 AAC
DLL_EXPORT_IMPORT int SetCode(int nVideoCodec, int nAudioCodec);

////// TEST API //////
typedef void (*CallBackTips)(char* tips);
DLL_EXPORT_IMPORT void Vigo_init(CallBackTips cb);
DLL_EXPORT_IMPORT void Vigo_destroy(void);
DLL_EXPORT_IMPORT void Vigo_start(char *ip, int audioport, int videoport, HWND hLocal, HWND hRemote);
DLL_EXPORT_IMPORT void Vigo_stop(void);
////// Test End //////
