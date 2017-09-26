// flySDK.cpp : Defines the exported functions for the DLL application.
//
#include "stdafx.h"
#include "flySDK.h"
#include "FlyBase.h"

CFlyBase g_flyBase;

DLL_EXPORT_IMPORT void Init()
{
	g_flyBase.Init();
}

DLL_EXPORT_IMPORT void Free()
{
	g_flyBase.Free();
}

DLL_EXPORT_IMPORT void Login(InitParam param)
{
	g_flyBase.Login(param);
}

DLL_EXPORT_IMPORT void LoginOut()
{
	g_flyBase.LoginOut();
}

DLL_EXPORT_IMPORT void TcpConnect()
{
	g_flyBase.TcpConnect();
}

DLL_EXPORT_IMPORT void NextTcpConnect()
{
	g_flyBase.NextTcpConnect();
}

DLL_EXPORT_IMPORT int Dialing(char *pUid)
{
	CStringA str;
	str.Format("%s", pUid);
	return g_flyBase.Dialing(str);
}

DLL_EXPORT_IMPORT int Answer()
{
	return g_flyBase.Answer();
}

DLL_EXPORT_IMPORT int Hangup()
{
	return g_flyBase.Hangup();
}

DLL_EXPORT_IMPORT int SetCode(int nVideoCodec, int nAudioCodec)
{
	return g_flyBase.SetCode(nVideoCodec, nAudioCodec);
}


// TEST
gl_media_engine::ViGoEngine* g_pAVEngine = NULL;
static bool g_enableVideo = true;
static int g_LoadFlag = FALSE;
static void on_vogo_cb(int ev_type, int ev_reason, const char* something, void* param, int size);
static int ConvertEmodelValueToString(ME_emodel_info_t* emodel);
static CallBackTips g_callback = NULL;

DLL_EXPORT_IMPORT void Vigo_init(CallBackTips cb)
{
	if (g_LoadFlag)
	{
		return;
	}
	g_pAVEngine = new ViGoEngine();
	if (g_pAVEngine)
	{
		ME_cb_vtable_t cb_table;
		cb_table.event_cb = on_vogo_cb;
		cb_table.log_cb = NULL;
		cb_table.send_cb = NULL;
		cb_table.screenshot_cb = NULL;
		// 初始化SDK
		g_pAVEngine->callback_vtable(&cb_table);
		g_pAVEngine->init(0);
		g_callback = cb;
		// 初始化VIGO
		ME_CTRL_cfg_t stCfg;
		g_pAVEngine->set_config(ME_CTRL_CFG_MODULE_ID, &stCfg);
		stCfg.ucRealTimeType = 0;
		stCfg.ucVideoEnable = 1;
		stCfg.ucEmodelEnable = 0;
		stCfg.ucFecEnable = 0;
		stCfg.ucRtpEncEnable = 0;
		stCfg.ucExtMediaProcEn = 0;
		stCfg.ucExtAudioTransEn = 0;
		stCfg.ucIPv6Enable = 0;
		stCfg.vieFecEnabled = false;
		stCfg.vieNackEnabled = false;
		g_pAVEngine->set_config(ME_CTRL_CFG_MODULE_ID, &stCfg);
		// 初始化视频编码参数
		ME_video_enc_param_t stVideoEncCfg;
		g_pAVEngine->set_config(ME_VIDEO_ENC_CFG_MODULE_ID, &stVideoEncCfg);
		stVideoEncCfg.ucMaxFramerate = 25;
		stVideoEncCfg.usMinBitrate = 30;
		stVideoEncCfg.usMaxBitrate = 2000;
		stVideoEncCfg.usWidth = 480;
		stVideoEncCfg.usHeight = 360;
		stVideoEncCfg.usStartBitrate = 500;
		stVideoEncCfg.ucUseHwEnc = 0;
		stVideoEncCfg.ucUseHwDec = 0;
		g_pAVEngine->set_config(ME_VIDEO_ENC_CFG_MODULE_ID, &stVideoEncCfg);
		// 初始化摄像头
		ME_video_camera_param_t stVideoDevicePara;
		g_pAVEngine->video_get_camera_state(&stVideoDevicePara);
		stVideoDevicePara.usCameraIdx = -1;
		stVideoDevicePara.usRotateAngle = -1;
		stVideoDevicePara.ucMaxFps = 30;
		stVideoDevicePara.usWidth = 1280;
		stVideoDevicePara.usHeight = 720;
		g_pAVEngine->video_set_camera(&stVideoDevicePara);
		// 更新标记
		g_LoadFlag = TRUE;
	}
}

DLL_EXPORT_IMPORT void Vigo_destroy(void)
{
	if (g_LoadFlag)
	{
		if (g_pAVEngine)
		{
			g_pAVEngine->destroy();
			delete g_pAVEngine;
			g_pAVEngine = NULL;
		}
		g_LoadFlag = FALSE;
	}
}

DLL_EXPORT_IMPORT void Vigo_start(char *ip, int audioport, int videoport, HWND hLocal, HWND hRemote)
{
	if (g_pAVEngine)
	{
		ME_audio_stream_info_t audio_param;
		memset(&audio_param, 0, sizeof(audio_param));
		audio_param.ucExTransportEnable = false;
		strcpy_s(audio_param.cRemoteAudioIp, ip);
		audio_param.iRemoteAudioPort = audioport;
		audio_param.iLocalAudioPort = audioport;
		audio_param.iPayLoadType = 111;

		unsigned short mulriple = 1;
		unsigned short offset = 1;

		g_pAVEngine->create_audio_stream();
		g_pAVEngine->set_audio_stream(&audio_param);
		g_pAVEngine->set_audio_fec(mulriple, offset);
		g_pAVEngine->enable_audio_playout(true);
		g_pAVEngine->enable_audio_receive(true);
		g_pAVEngine->enable_audio_send(true);
		g_pAVEngine->set_state(4);

		if (g_enableVideo)
		{
			ME_video_stream_info_t video_para;
			memset(&video_para, 0, sizeof(video_para));
			strcpy_s(video_para.cRemoteVideoIp, ip);
			video_para.iLocalVideoPort = videoport;
			video_para.iRemoteVideoPort = videoport;
			video_para.iPayLoadType = 121;

			ME_video_render_param_t stVideoRenderConfig;
			g_pAVEngine->get_config(ME_VIDEO_RENDER_CFG_MODULE_ID, &stVideoRenderConfig);
			stVideoRenderConfig.pWindowRemote = hRemote;
			stVideoRenderConfig.pWindowLocal = hLocal;
			g_pAVEngine->set_config(ME_VIDEO_RENDER_CFG_MODULE_ID, &stVideoRenderConfig);

			g_pAVEngine->video_create_stream();
			g_pAVEngine->video_set_stream(&video_para);
			g_pAVEngine->video_start(31);
		}
	}
}

DLL_EXPORT_IMPORT void Vigo_stop(void)
{
	if (g_pAVEngine)
	{
		g_pAVEngine->delete_audio_stream();

		/*get emodel value*/
		char emodel_str[1024] = { 0 };
		ME_emodel_info_t emodel_value;
		memset(&emodel_value, 0, sizeof(emodel_value));
		g_pAVEngine->get_emodel_calc_value(&emodel_value);
		ConvertEmodelValueToString(&emodel_value);

		if (g_enableVideo)
		{
			g_pAVEngine->video_delete_stream();
		}
	}
}

void on_vogo_cb(int ev_type, int ev_reason, const char* something, void* param, int size)
{
	char temp[1024] = { 0 };
	char *log = (char*)something;
	sprintf_s(temp, "ev_type=%d ev_reason=%d something = %s\r\n", ev_type, ev_reason, log);
	OutputDebugStringA(temp);

	if (ev_type == eME_VIDEO_NETWORK_STATE_EV)
	{
		ME_video_network_state_t *net = (ME_video_network_state_t *)param;
		char strNet[8096] = { 0 };
		char rtPOT[8096] = { 0 };
		sprintf_s(strNet, "vie state: %d, ice: %d, rtt: %d, lost: %d(s) %d(r)," \
			" rate: %d(s) %d(r), res: %dx%d(s) %dx%d(r)," \
			" frame: %d(s) %d(r) pt: %d(s) %d(r) codec: %s(s) %s(r) RelayCnt: %d",
			ev_reason, net->ice, net->rttMs, net->uplinkLost, net->downlinkLost,
			net->bitrate_bps, net->rec_bitrate, net->width, net->height,
			net->decode_width, net->decode_height, net->target_fps, net->decode_fps,
			net->EncoderPt, net->DecoderPt, net->encCodec, net->decCodec, net->rtPOT_nCount);

		for (int i = 0; i < net->rtPOT_nCount; i++) {
			sprintf_s(rtPOT, " Relay:%d{IP: %d(s) %d(r) Flow_a: %dKB(s) %dKB(r) Flow_v: %dKB(s) %dKB(r)} ", i, net->rtPOT_SendIP[i], net->rtPOT_RecvIP[i], net->rtPOT_SendValue_a[i], net->rtPOT_RecvValue_a[i], net->rtPOT_SendValue_v[i], net->rtPOT_RecvValue_v[i]);
			strcat_s(strNet, rtPOT);
		}
		// 
		if (g_callback != NULL)
		{
			g_callback(strNet);
		}
	}
}

int ConvertEmodelValueToString(ME_emodel_info_t* emodel)
{
	char tmp_buff[128] = { 0 };

	if (emodel == NULL)
		return -1;

	sprintf_s(tmp_buff, "EMODEL: M=\"%.2lf,%.2lf,%.2lf\"\r\n", \
		emodel->emodel_mos.min, emodel->emodel_mos.max, \
		emodel->emodel_mos.average);
	OutputDebugStringA(tmp_buff);

	memset(&tmp_buff, 0x00, sizeof(tmp_buff));
	sprintf_s(tmp_buff, "T=\"%.2lf,%.2lf,%.2lf\"\r\n", \
		emodel->emodel_tr.min, emodel->emodel_tr.max, \
		emodel->emodel_tr.average);
	OutputDebugStringA(tmp_buff);

	memset(&tmp_buff, 0x00, sizeof(tmp_buff));
	sprintf_s(tmp_buff, "P=\"%.2lf,%.2lf,%.2lf\"\r\n", \
		emodel->emodel_ppl.min, emodel->emodel_ppl.max, \
		emodel->emodel_ppl.average);
	OutputDebugStringA(tmp_buff);

	memset(&tmp_buff, 0x00, sizeof(tmp_buff));
	sprintf_s(tmp_buff, "B=\"%.2lf,%.2lf,%.2lf\"\r\n", \
		emodel->emodel_burstr.min, emodel->emodel_burstr.max, \
		emodel->emodel_burstr.average);
	OutputDebugStringA(tmp_buff);

	memset(&tmp_buff, 0x00, sizeof(tmp_buff));
	sprintf_s(tmp_buff, "I=\"%.2lf,%.2lf,%.2lf\"\r\n", \
		emodel->emodel_ie.min, emodel->emodel_ie.max, \
		emodel->emodel_ie.average);
	OutputDebugStringA(tmp_buff);

	memset(&tmp_buff, 0x00, sizeof(tmp_buff));
	sprintf_s(tmp_buff, "D=\"%.2lf, %.2lf, %.2lf\"\r\n", \
		emodel->delay.min, emodel->delay.max,
		emodel->delay.average);
	OutputDebugStringA(tmp_buff);

	memset(&tmp_buff, 0x00, sizeof(tmp_buff));
	sprintf_s(tmp_buff, "J=\"%.2lf, %.2lf, %.2lf\"\r\n", \
		emodel->jitbuf.min, emodel->jitbuf.max, \
		emodel->jitbuf.average);
	OutputDebugStringA(tmp_buff);
	return 0;
}
