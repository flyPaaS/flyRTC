#include "stdafx.h"
#include "Base.h"
#include "FlyBase.h"
#include "flySDK.h"

extern CFlyBase g_flyBase;

char* g_pchConnTonePath = "connecting_tone.pcm";
char* g_pchDialTonePath = "dialling_tone.pcm";

// 内网外网标记
boolean bOutNet = true;
// AMS服务器地址和端口
CString CS_URL = bOutNet ? (L"http://59.110.10.28") : (L"http://192.168.0.8");
CString CS_PORT = L"8097";
// 登陆AMS服务器地址URL(GET)
CString AMS_URL_ADDRESS = bOutNet ? (L"http://59.110.10.28:8097/ams2/login.act?") : (L"http://192.168.0.8:8097/ams2/login.act?");
// 获取CS,RTPP服务器地址URL(GET)
CString GET_URL_ADDRESS = bOutNet ? (L"http://59.110.10.28:8097/v3/geturl?") : (L"http://192.168.0.8:8097/v3/geturl?");
// 获取CS服务器地址URL(GET)
CString CS_URL_ADDRESS = bOutNet ? (L"http://59.110.10.28:8097/v3/getcslist?") : (L"http://192.168.0.8:8097/v2/getcslist?");
// 获取RTPP服务器地址URL(GET)
CString RTPP_URL_ADDRESS = bOutNet ? (L"http://59.110.10.28:8097/v3/getrtpplist?") : (L"http://192.168.0.8:8097/v3/getrtpplist?");

// 事件回调
void TUGo_CallBack(int ev_type, int ev_reason, const char* something, const void* param)
{
	int iRet = 0;
	char temp[1024] = { 0 };
	// 分析
	switch (ev_type)
	{
	case eUGo_CALLDIALING_EV:
	{
		// 主叫事件
		if (ev_reason == eUGo_Reason_Success)
		{
			// 接听电话
			g_flyBase.StopPlayfile();
			g_flyBase.m_nCallStatus = 3;
			// 回调状态
			if (g_flyBase.m_CallBackFunc != NULL)
			{
				g_flyBase.m_CallBackFunc(CALL_ANSWER, ev_reason, 0, (char*)something, (char*)param);
			}
		}
		else if (ev_reason == eUGo_Reason_Connecting)
		{
			// 播放声音
			g_flyBase.StartPlayfile(g_pchConnTonePath, 7, 0, 1);
			g_flyBase.m_nCallStatus = 1;
		}
		else if (ev_reason == eUGo_Reason_Ringing)
		{
			// 播放声音
			g_flyBase.StopPlayfile();
			g_flyBase.StartPlayfile(g_pchDialTonePath, 7, 0, 1);
			g_flyBase.m_nCallStatus = 1;
			// 回调状态
			if (g_flyBase.m_CallBackFunc != NULL)
			{
				g_flyBase.m_CallBackFunc(CALL_ALERT, ev_reason, 0, (char*)something, (char*)param);
			}
		}
		else
		{
			// 呼叫失败
			if (g_flyBase.m_CallBackFunc != NULL)
			{
				g_flyBase.m_CallBackFunc(CALL_OUTFAIL, ev_reason, 0, (char*)something, (char*)param);
			}
		}
	}
	break;
	case eUGo_CALLINCOMING_EV:
	{
		// 来电事件
		if (ev_reason == eUGo_Reason_Success)
		{
			g_flyBase.StartPlayfile(g_pchDialTonePath, 7, 0, 1);
			g_flyBase.m_nCallStatus = 2;

			ME_video_render_param_t stVideoRenderConfig;
			UGo_get_config(ME_VIDEO_RENDER_CFG_MODULE_ID, &stVideoRenderConfig, 0);
			stVideoRenderConfig.pWindowRemote = g_flyBase.m_hRemoteWnd;
			stVideoRenderConfig.pWindowLocal = g_flyBase.m_hLocalWnd;
			stVideoRenderConfig.remoteWidth = 320;
			stVideoRenderConfig.remoteHeight = 240;
			stVideoRenderConfig.renderMode = 1;
			UGo_set_config(ME_VIDEO_RENDER_CFG_MODULE_ID, &stVideoRenderConfig, 0);
			UGo_video_start(31);
		}
		// 回调上层
		if (g_flyBase.m_CallBackFunc != NULL)
		{
			g_flyBase.m_CallBackFunc(CALL_INCOME, ev_reason, 0, (char*)something, (char*)param);
		}
	}
	break;
	case eUGo_CALLANSWER_EV:
	{
		// 接听事件
		g_flyBase.StopPlayfile();
		g_flyBase.m_nCallStatus = 3;
		if (ev_reason == eUGo_Reason_Success)
		{
			// 回调上层
			if (g_flyBase.m_CallBackFunc != NULL)
			{
				g_flyBase.m_CallBackFunc(CALL_ANSWER, ev_reason, 0, (char*)something, (char*)param);
			}
		}
	}
	break;
	case eUGo_CALLHUNGUP_EV:
	{
		// 挂断事件
		g_flyBase.StopPlayfile();
		g_flyBase.m_nCallStatus = 0;
		// 回调上层
		if (g_flyBase.m_CallBackFunc != NULL)
		{
			g_flyBase.m_CallBackFunc(CALL_HANDUP, ev_reason, 0, (char*)something, (char*)param);
		}
	}
	break;
	case eUGo_TCPTRANSPORT_EV:
	{
		/*
		if (ev_reason == eUGo_TCP_CONNECTED)
		{
			// 回调上层
			if (g_flyBase.m_CallBackFunc != NULL)
			{
				g_flyBase.m_CallBackFunc(TCP_CONNECT, ev_reason, 0, (char*)something, (char*)param);
			}
		}
		if (ev_reason == eUGo_TCP_DISCONNECTED)
		{
			// 回调上层
			if (g_flyBase.m_CallBackFunc != NULL)
			{
				g_flyBase.m_CallBackFunc(TCP_DISCONNECT, ev_reason, 0, (char*)something, (char*)param);
			}
		}
		*/
	}
	break;
	case eUGo_GETDTMF_EV:
		// 暂不处理
		break;
	case eUGo_NETWORK_EV:
	{
		char s[256] = { 0 };
		ME_network_state_t *net = (ME_network_state_t*)param;
		if (net)
		{
			char* tmpState = NULL;
			switch (net->net_state)
			{
			case eME_REASON_NETWORK_NICE:
				tmpState = "nice";
				break;
			case eME_REASON_NETWORK_WELL:
				tmpState = "well";
				break;
			case eME_REASON_NETWORK_GENERAL:
				tmpState = "general";
				break;
			case eME_REASON_NETWORK_POOR:
				tmpState = "poor";
				break;
			case eME_REASON_NETWORK_BAD:
				tmpState = "bad";
				break;
			default:
				tmpState = "unknown";
				break;
			}
			sprintf_s(s, 256, "state:%s ice: %d, rtt: %d, lost: %d(s) %d(r), jitter: %d(s) %d(r)",
				tmpState, net->ice, net->rttMs, net->uplinkLost,
				net->downlinkLost, net->upJitter, net->downJitter);
		}
		// 回调上层
		if (g_flyBase.m_CallBackFunc != NULL)
		{
			g_flyBase.m_CallBackFunc(CALL_ANETWORK, ev_reason, 0, (char*)s, (char*)param);
		}
	}
	break;
	case eUGo_VIDEO_NETWORK_EV:
	{
		char strNet[1024] = { 0 };
		ME_video_network_state_t *net = (ME_video_network_state_t*)param;
		if (ev_reason == eUGo_Reason_Success)
		{
			if (net)
			{
				char* tmpState = NULL;
				switch (net->net_state)
				{
				case eME_REASON_NETWORK_NICE:
					tmpState = "nice";
					break;
				case eME_REASON_NETWORK_WELL:
					tmpState = "well";
					break;
				case eME_REASON_NETWORK_GENERAL:
					tmpState = "general";
					break;
				case eME_REASON_NETWORK_POOR:
					tmpState = "poor";
					break;
				case eME_REASON_NETWORK_BAD:
					tmpState = "bad";
					break;
				default:
					tmpState = "unknown";
					break;
				}

				char rtPOT[1024] = { 0 };
				sprintf_s(strNet, 1024, "vie state: %s, ice: %d, rtt: %d, lost: %d(s) %d(r)," \
					" rate: %d(s) %d(r), res: %dx%d(s) %dx%d(r)," \
					" frame: %d(s) %d(r) pt: %d(s) %d(r) codec: %s(s) %s(r) RelayCnt: %d,",
					tmpState, net->ice, net->rttMs, net->uplinkLost, net->downlinkLost,
					net->bitrate_bps, net->rec_bitrate, net->width, net->height,
					net->decode_width, net->decode_height, net->target_fps, net->decode_fps,
					net->EncoderPt, net->DecoderPt, net->encCodec, net->decCodec, net->rtPOT_nCount);

				for (int i = 0; i < net->rtPOT_nCount; i++)
				{
					sprintf_s(rtPOT, 1024, " Relay:%d{IP: %d(s) %d(r) Flow_a: %dKB(s) %dKB(r) Flow_v: %dKB(s) %dKB(r)} ", i, net->rtPOT_SendIP[i], net->rtPOT_RecvIP[i], net->rtPOT_SendValue_a[i], net->rtPOT_RecvValue_a[i], net->rtPOT_SendValue_v[i], net->rtPOT_RecvValue_v[i]);
					strcat_s(strNet, rtPOT);
				}
			}
			// 回调上层
			if (g_flyBase.m_CallBackFunc != NULL)
			{
				g_flyBase.m_CallBackFunc(CALL_VNETWORK, ev_reason, 0, (char*)strNet, (char*)param);
			}
		}
	}
	break;
	case eUGo_UPSINGLEPASS_EV:
	case eUGo_DNSINGLEPASS_EV:
	case eUGo_CONFERENCE_EV:
	case eUGo_REMOTE_VIDEO_STATE_NOTIFY_EV:
	case eUGo_VIDEO_EV:
	default:
		break;
	}
	return;
}

// LOG信息回调
void TUGo_LogCallBack(char* summary, char* detail, enum eME_TraceLevel level)
{
	CStringA str;
	str.Format("Callback TraceLog Msg:\r\nLevel:%d\r\nSummary:%s\r\nDetails:\r\n%s\r\n", level, summary, detail);
	OutputDebugStringA(str);
}

void TUGO_Send_Msg_CB(const ucpaas_int8* msg, ucpaas_int32 slen)
{
	g_flyBase.m_cTcp.m_cTcpClient.Send((char *)msg, slen);
}

void TUGo_EncryptCb(unsigned char* in_data, unsigned char* out_data, int bytes_in, int* bytes_out)
{
	if (in_data == NULL || out_data == NULL || bytes_in == 0)
	{
		return;
	}

	int i, j;
	for (i = 0, j = (bytes_in - 1); i < bytes_in; i++, j--)
	{
		out_data[i] = in_data[j];
	}
	*bytes_out = bytes_in;
}

void TUGo_decryptCb(unsigned char* in_data, unsigned char* out_data, int bytes_in, int* bytes_out)
{
	if (in_data == NULL || out_data == NULL || bytes_in == 0)
	{
		return;
	}
	int i, j;
	for (i = 0, j = (bytes_in - 1); i < bytes_in; i++, j--)
	{
		out_data[i] = in_data[j];
	}
	*bytes_out = bytes_in;
}

void TUGO_Screen_ShotCB(const ucpaas_uint8* dst_argb, int dst_stride, int width, int height, int islocal, ScreenType screen_type)
{
	// 抓拍图像写入D盘
	BmpWrite((unsigned char*)dst_argb, width, height, islocal, screen_type, "d:\\ShotBmp");
}

void TUGO_ExlMedia_ProcCB(ucpaas_int16* audio_10ms, ucpaas_int32 samples, ucpaas_int32 samplingFreqHz, ucpaas_boolean isStereo, ucpaas_int16* audio_out, ucpaas_int32* samples_out)
{
	// 暂时不处理
}

void TUGo_InitPlayoutCb(unsigned int sample_rate, unsigned int bytes_per_sample, unsigned int num_of_channels)
{
	// 暂时不处理
}

void TUGo_InitRecordingCb(unsigned int sample_rate, unsigned int bytes_per_sample, unsigned int num_of_channels)
{
	// 暂时不处理
}

int TUGo_ReadRecordingDataCb(signed char *audio_data, unsigned int nSize)
{
	return 0;
}

int TUGo_WritePlayoutDataCb(const signed char *audio_data, unsigned int nSize)
{
	return 0;
}

int BmpWrite(unsigned char *image, int xsize, int ysize, int islocal, ScreenType screen_type, char *filename)
{
	int nbytes = 0;
	if (screen_type == ARGB)
	{
		nbytes = 4;
	}
	else if (screen_type == RGB24)
	{
		nbytes = 3;
	}

	unsigned char header[54] =
	{
		0x42, 0x4d, 0, 0, 0, 0, 0, 0, 0, 0,
		54, 0, 0, 0, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 8 * nbytes, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0
	};

	long file_size = (long)xsize * (long)ysize * nbytes + 54;
	header[2] = (unsigned char)(file_size & 0x000000ff);
	header[3] = (file_size >> 8) & 0x000000ff;
	header[4] = (file_size >> 16) & 0x000000ff;
	header[5] = (file_size >> 24) & 0x000000ff;

	long width = xsize;
	header[18] = width & 0x000000ff;
	header[19] = (width >> 8) & 0x000000ff;
	header[20] = (width >> 16) & 0x000000ff;
	header[21] = (width >> 24) & 0x000000ff;

	long height = ysize;
	header[22] = height & 0x000000ff;
	header[23] = (height >> 8) & 0x000000ff;
	header[24] = (height >> 16) & 0x000000ff;
	header[25] = (height >> 24) & 0x000000ff;

	char fname_bmp[128];
	sprintf_s(fname_bmp, "%s_%d.bmp", filename, islocal);

	FILE *fp = NULL;
	errno_t res = fopen_s(&fp, fname_bmp, "wb");
	if (res != 0)
	{
		return -1;
	}

	fwrite(header, sizeof(unsigned char), 54, fp);
	fwrite(image, sizeof(unsigned char), (size_t)(long)xsize * ysize * nbytes, fp);
	fclose(fp);
	return 0;
}