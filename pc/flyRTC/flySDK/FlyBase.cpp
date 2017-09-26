#include "stdafx.h"
#include "FlyBase.h"
#include "flySDK.h"
#include "md5.h"
#include "HttpTool.h"
#include "TcpTool.h"

CFlyBase::CFlyBase(void)
{
	m_bLoad = false;
	m_bWork = false;
	m_bGetUrl = false;

	m_nSelectCS = 0;

	m_nCallStatus = 0;
	m_strCallId = "";

	m_hLocalWnd = NULL;
	m_hRemoteWnd = NULL;

	m_hEvent = NULL;
	m_hThread = NULL;

	m_CallBackFunc = NULL;

	memset(m_accountSid, 0, MAX_PATH*sizeof(char));
	memset(m_accountToken, 0, MAX_PATH*sizeof(char));
	memset(m_clientId, 0, MAX_PATH*sizeof(char));
	memset(m_clientPwd, 0, MAX_PATH*sizeof(char));

	memset(m_phone, 0, MAX_PATH*sizeof(char));
	memset(m_ssid, 0, MAX_PATH*sizeof(char));
	memset(m_uid, 0, MAX_PATH*sizeof(char));
	memset(m_im_ssid, 0, MAX_PATH*sizeof(char));
	memset(m_name, 0, MAX_PATH*sizeof(char));
	memset(m_forwardnumber, 0, MAX_PATH*sizeof(char));

	m_stumAddress = "";
	m_csAddress.clear();
	m_rtppAddress.clear();
}

CFlyBase::~CFlyBase(void)
{

}

BOOL CFlyBase::Init()
{
	// 加载
	if (UGo_load_media_engine() != 0)
	{
		return FALSE;
	}

	if (!m_bLoad)
	{
		ugo_log_trace_pm_t stLogPm;
		ugo_cb_vtable_t cb_table;

		cb_table.event_cb = TUGo_CallBack;
		cb_table.log_cb = TUGo_LogCallBack;
		cb_table.send_cb = TUGO_Send_Msg_CB;
		cb_table.encrypt = TUGo_EncryptCb;
		cb_table.decrypt = TUGo_decryptCb;
		cb_table.screen_shot_cb = TUGO_Screen_ShotCB;
		cb_table.ext_mediaproc_cb = TUGO_ExlMedia_ProcCB;
		cb_table.initplayout_cb = TUGo_InitPlayoutCb;
		cb_table.initrecording_cb = TUGo_InitRecordingCb;
		cb_table.read_recording_cb = TUGo_ReadRecordingDataCb;
		cb_table.write_playout_cb = TUGo_WritePlayoutDataCb;

		UGo_callback_vtable(&cb_table);
		UGo_enable_stack_log("D:\\ugo_stacklog.txt");
		UGo_init();

		// engine log
		memset(&stLogPm, 0x00, sizeof(stLogPm));
		stLogPm.level = 0x40ff;
		strcpy_s(stLogPm.filepath, "D:\\enginelog.txt");
		UGo_set_log_file(&stLogPm);

		// 初始化TCP
		m_cTcp.m_pBase = this;

		// 启动线程
		StartWorkThread();
		// 修改标记
		m_bLoad = true;
	}
	return TRUE;
}

void CFlyBase::Free()
{
	if (m_bLoad)
	{
		// 注销账号
		LoginOut();
		// 退出线程
		StopWorkThread();
		// 关闭UGO
		UGo_destroy();
		UGo_unload_media_engine();
		// 更新标记
		m_bLoad = false;
		m_bGetUrl = false;
		m_bWork = false;
	}
}

void CFlyBase::Login(InitParam stParam)
{
	if (m_bLoad)
	{
		strcpy_s(m_accountSid, MAX_PATH, stParam.accountSid);
		strcpy_s(m_accountToken, MAX_PATH, stParam.accountToken);
		strcpy_s(m_clientId, MAX_PATH, stParam.clientId);
		strcpy_s(m_clientPwd, MAX_PATH, stParam.clientPwd);
		m_hLocalWnd = stParam.hLocalWnd;
		m_hRemoteWnd = stParam.hRemoteWnd;
		m_CallBackFunc = stParam.cbCallBackFunc;
		m_bWork = true;
	}
}

void CFlyBase::LoginOut()
{
	if (m_bLoad)
	{
		m_cTcp.StopTcp();
		m_bWork = false;
	}
}

void CFlyBase::TcpConnect()
{
	if (m_bGetUrl)
	{
		// 建立TCP连接
		int nSize = m_csAddress.size();
		if (nSize > 0)
		{
			if (m_nSelectCS >= nSize)
			{
				m_nSelectCS = 0;
			}
			CStringA strServer = m_csAddress[m_nSelectCS];
			int nFind = strServer.Find(":");
			if (nFind > 0)
			{
				// 获取服务器数据
				CStringA strIp = strServer.Left(nFind);
				int nPort = atoi(strServer.Mid(nFind + 1));
				// 启动连接
				m_cTcp.StartTcp(strIp.GetBuffer(), nPort);
			}
		}
	}
}

void CFlyBase::NextTcpConnect()
{
	if (m_bGetUrl)
	{
		m_nSelectCS++;
		TcpConnect();
	}
}

int CFlyBase::Dialing(CStringA strUid)
{
	// 更新渲染设置
	ME_video_render_param_t stVideoRenderConfig;
	UGo_get_config(ME_VIDEO_RENDER_CFG_MODULE_ID, &stVideoRenderConfig, 0);
	stVideoRenderConfig.pWindowRemote = m_hRemoteWnd;
	stVideoRenderConfig.pWindowLocal = m_hLocalWnd;
	UGo_set_config(ME_VIDEO_RENDER_CFG_MODULE_ID, &stVideoRenderConfig, 0);

	ugo_call_dialing_para_t callpm;
	memset(&callpm, 0x00, UGO_CALL_DIALING_PARA_SIZE);
	strcpy_s(callpm.tuid, 32, strUid);
	strcpy_s(callpm.tphone, 32, "");
	callpm.call_mode = eUGo_CM_FREE;
	callpm.video_enabled = TRUE;
	callpm.calltype = 0;
	return UGo_call_dialing(&callpm);
}

int CFlyBase::Answer()
{
	// 更新渲染设置
	ME_video_render_param_t stVideoRenderConfig;
	UGo_get_config(ME_VIDEO_RENDER_CFG_MODULE_ID, &stVideoRenderConfig, 0);
	stVideoRenderConfig.pWindowRemote = m_hRemoteWnd;
	stVideoRenderConfig.pWindowLocal = m_hLocalWnd;
	UGo_set_config(ME_VIDEO_RENDER_CFG_MODULE_ID, &stVideoRenderConfig, 0);
	return UGo_call_answered();
}

int CFlyBase::Hangup()
{
	return UGo_call_hangup(eUGo_Reason_HungupMyself);
}

int CFlyBase::SetCode(int nVideoCodec, int nAudioCodec)
{
	if (nVideoCodec == 0)
	{
		ME_video_enc_param_t stVideoEncCfg;
		UGo_get_config(ME_VIDEO_ENC_CFG_MODULE_ID, &stVideoEncCfg, 0);
		stVideoEncCfg.ucFirstSelectedPt = 121;
		stVideoEncCfg.usMinBitrate = 30;
		stVideoEncCfg.usMaxBitrate = 2000;
		stVideoEncCfg.usWidth = 480;
		stVideoEncCfg.usHeight = 360;
		stVideoEncCfg.usStartBitrate = 500;
		stVideoEncCfg.ucUseHwEnc = 0;
		stVideoEncCfg.ucUseHwDec = 0;
		UGo_set_config(ME_VIDEO_ENC_CFG_MODULE_ID, &stVideoEncCfg, 0);
	}
	if (nVideoCodec == 1)
	{
		ME_video_enc_param_t stVideoEncCfg;
		UGo_get_config(ME_VIDEO_ENC_CFG_MODULE_ID, &stVideoEncCfg, 0);
		stVideoEncCfg.ucFirstSelectedPt = 121;
		stVideoEncCfg.usMinBitrate = 30;
		stVideoEncCfg.usMaxBitrate = 2000;
		stVideoEncCfg.usWidth = 480;
		stVideoEncCfg.usHeight = 360;
		stVideoEncCfg.usStartBitrate = 500;
		stVideoEncCfg.ucUseHwEnc = 1;
		stVideoEncCfg.ucUseHwDec = 0;
		UGo_set_config(ME_VIDEO_ENC_CFG_MODULE_ID, &stVideoEncCfg, 0);
	}
	if (nVideoCodec == 2)
	{
		ME_video_enc_param_t stVideoEncCfg;
		UGo_get_config(ME_VIDEO_ENC_CFG_MODULE_ID, &stVideoEncCfg, 0);
		stVideoEncCfg.ucFirstSelectedPt = 125;
		stVideoEncCfg.usMinBitrate = 30;
		stVideoEncCfg.usMaxBitrate = 250;
		stVideoEncCfg.usWidth = 320;
		stVideoEncCfg.usHeight = 240;
		stVideoEncCfg.usStartBitrate = 200;
		stVideoEncCfg.ucUseHwEnc = 0;
		stVideoEncCfg.ucUseHwDec = 0;
		UGo_set_config(ME_VIDEO_ENC_CFG_MODULE_ID, &stVideoEncCfg, 0);
	}
	if (nVideoCodec == 3)
	{
		ME_video_enc_param_t stVideoEncCfg;
		UGo_get_config(ME_VIDEO_ENC_CFG_MODULE_ID, &stVideoEncCfg, 0);
		stVideoEncCfg.ucFirstSelectedPt = 125;
		stVideoEncCfg.usMinBitrate = 30;
		stVideoEncCfg.usMaxBitrate = 250;
		stVideoEncCfg.usWidth = 320;
		stVideoEncCfg.usHeight = 240;
		stVideoEncCfg.usStartBitrate = 200;
		stVideoEncCfg.ucUseHwEnc = 0;
		stVideoEncCfg.ucUseHwDec = 1;
		UGo_set_config(ME_VIDEO_ENC_CFG_MODULE_ID, &stVideoEncCfg, 0);
	}
	if (nAudioCodec == 0)
	{
		// OPUS
		ME_codec_list_t setcodec;
		UGo_get_config(ME_CODECS_CFG_MODULE_ID, &setcodec, 0);
		setcodec.num = 1;
		setcodec.codecs[0].enabled = TRUE;
		memcpy(setcodec.codecs[0].plname, "opus", 32);
		setcodec.codecs[0].pltype = 111;
		UGo_set_config(ME_CODECS_CFG_MODULE_ID, &setcodec, 0);
	}
	if (nAudioCodec == 1)
	{
		// AAC
		ME_codec_list_t setcodec;
		UGo_get_config(ME_CODECS_CFG_MODULE_ID, &setcodec, 0);
		setcodec.num = 1;
		setcodec.codecs[0].enabled = TRUE;
		memcpy(setcodec.codecs[0].plname, "aac", 32);
		setcodec.codecs[0].pltype = 124;
		UGo_set_config(ME_CODECS_CFG_MODULE_ID, &setcodec, 0);
	}
	return 0;
}

const int max_stream_size = 1024 * 1024;
static char g_stream[max_stream_size] = { 0 };
void CFlyBase::StartPlayfile(char* pchFilePaht, int iFormat, int iDirect, int iLoop)
{
	CString strPath;
	::GetModuleFileName(NULL, strPath.GetBufferSetLength(MAX_PATH), MAX_PATH);
	strPath.ReleaseBufferSetLength(MAX_PATH);
	strPath = strPath.Left(strPath.ReverseFind('\\'));

	CStringA str;
	str.Format("\\%s", pchFilePaht);

	FILE* fp = NULL;
	fopen_s(&fp, StringToStringA(strPath) + str, "rb");
	if (fp != NULL)
	{
		fseek(fp, 0, SEEK_END);
		int nCount = ftell(fp);

		char *pCont = new char[nCount];
		fseek(fp, 0, SEEK_SET);
		int nSize = fread(pCont, 1, nCount, fp);
		memcpy(g_stream, pCont, nSize);

		media_file_play_pm_t fileplay;
		memset(&fileplay, 0, sizeof(fileplay));
		fileplay.filestream.size = nSize;
		fileplay.filestream.data = g_stream;
		fileplay.filehandle.fd = fp;
		fileplay.iDirect = iDirect;
		fileplay.iFileFormat = iFormat;
		fileplay.iLoop = iLoop;
		strcpy_s(fileplay.filepath.filename, 256, pchFilePaht);
		UGo_play_file(kME_FILE_STREAM, &fileplay);

		fclose(fp);
		delete[] pCont;
	}
}

void CFlyBase::StopPlayfile()
{
	UGo_stop_file();
}

void CFlyBase::UpdateParam()
{
	// set ugo config
	ugo_cfg_t ucfg;
	ugo_cfg_ice_t icecfg;
	ugo_cfg_tcp_t tcpcfg;

	// set media config
	ME_VQE_cfg_t stVQEConfig;
	ME_RTP_cfg_t stRTPConfig;
	ME_CTRL_cfg_t stCtrlConfig;

	// set audio encode
	ME_codec_list_t setcodec;
	// set video encode
	ME_video_enc_param_t stVideoEncCfg;
	// set video render
	ME_video_render_param_t stVideoRenderConfig;

	/* ugo common config */
	memset(&ucfg, 0, sizeof(ucfg));
	ucfg.rc4_enabled = FALSE;
	ucfg.compress_enabled = FALSE;
	ucfg.video_enabled = TRUE;
	ucfg.platform = 1;
	ucfg.atype = 2;
	ucfg.tlv_enabled = FALSE;
	ucfg.ipv6_enabled = FALSE;
	ucfg.csrv_enabled = FALSE;
	strcpy_s(ucfg.uid, m_clientId);
	strcpy_s(ucfg.phone, m_phone);
	strcpy_s(ucfg.brand, "flyRTC");
	strcpy_s(ucfg.nickname, m_name);
	strcpy_s(ucfg.localipaddr, "192.168.0.105");
	UGo_set_config(UGO_CFG_PARAM_MODULE_ID, &ucfg, 0);

	/* ice config */
	memset(&icecfg, 0, sizeof(icecfg));
	icecfg.ice_enabled = FALSE;
	icecfg.ipv6_enabled = FALSE;
	strcpy_s(icecfg.stun_server, m_stumAddress);
	UGo_set_config(UGO_CFG_ICE_MODULE_ID, &icecfg, 0);

	/*  tcp server config */
	memset(&tcpcfg, 0, sizeof(tcpcfg));
	tcpcfg.tcp_enabled = FALSE;
	strcpy_s(tcpcfg.tcp_srvaddr, 128, m_csAddress[0]);
	UGo_set_config(UGO_CFG_TCP_MODULE_ID, &tcpcfg, 0);

	/* vqe config */
	memset(&stVQEConfig, 0x00, sizeof(stVQEConfig));
	UGo_get_config(ME_VQE_CFG_MODULE_ID, &stVQEConfig, 0);
	stVQEConfig.Agc_enable = TRUE;
	stVQEConfig.Ec_enable = TRUE;
	stVQEConfig.Ns_enable = TRUE;
	stVQEConfig.Agc_Rx_enable = FALSE;
	stVQEConfig.Ns_Rx_enable = FALSE;
	UGo_set_config(ME_VQE_CFG_MODULE_ID, &stVQEConfig, 0);

	/* rtp param config */
	UGo_get_config(ME_RTP_CFG_MODULE_ID, &stRTPConfig, 0);
	stRTPConfig.uiRTPTimeout = 45;
	UGo_set_config(ME_RTP_CFG_MODULE_ID, &stRTPConfig, 0);

	UGo_video_preset("definition", "low");
	UGo_get_config(ME_CTRL_CFG_MODULE_ID, &stCtrlConfig, 0);
	stCtrlConfig.ucVideoEnable = true;
	stCtrlConfig.ucExtAudioTransEn = false;
	UGo_set_config(ME_CTRL_CFG_MODULE_ID, &stCtrlConfig, 0);
	
	// audio encode
	memset(&setcodec, 0, sizeof(ME_codec_list_t));
	setcodec.num = 1;
	setcodec.codecs[0].enabled = TRUE;
	memcpy(setcodec.codecs[0].plname, "opus", 32);
	setcodec.codecs[0].pltype = 111;
	UGo_set_config(ME_CODECS_CFG_MODULE_ID, &setcodec, 0);

	// video encode
	UGo_get_config(ME_VIDEO_ENC_CFG_MODULE_ID, &stVideoEncCfg, 0);
	stVideoEncCfg.ucFirstSelectedPt = 121;
	stVideoEncCfg.usMinBitrate = 30;
	stVideoEncCfg.usMaxBitrate = 2000;
	stVideoEncCfg.usWidth = 480;
	stVideoEncCfg.usHeight = 360;
	stVideoEncCfg.usStartBitrate = 500;
	stVideoEncCfg.ucUseHwEnc = 0;
	stVideoEncCfg.ucUseHwDec = 0;
	UGo_set_config(ME_VIDEO_ENC_CFG_MODULE_ID, &stVideoEncCfg, 0);

	// video render
	UGo_get_config(ME_VIDEO_RENDER_CFG_MODULE_ID, &stVideoRenderConfig, 0);
	stVideoRenderConfig.pWindowRemote = m_hRemoteWnd;
	stVideoRenderConfig.pWindowLocal = m_hLocalWnd;
	stVideoRenderConfig.remoteWidth = 240;
	stVideoRenderConfig.remoteHeight = 320;
	stVideoRenderConfig.renderMode = 1;
	UGo_set_config(ME_VIDEO_RENDER_CFG_MODULE_ID, &stVideoRenderConfig, 0);
}

void CFlyBase::UpdateRtppParam()
{
	//set rtpp param
	ugo_rtpp_cfg_t rtpp_cfg;
	memset(&rtpp_cfg, 0, sizeof(rtpp_cfg));
	rtpp_cfg.rtpp_num = m_rtppAddress.size();
	for (int i = 0; i < (int)m_rtppAddress.size(); i++)
	{
		rtpp_cfg.rtpps[i].delay = 100;
		rtpp_cfg.rtpps[i].lost = 0;

		CStringA str = m_rtppAddress[i];
		str = str.Left(str.Find(":"));
		strcpy_s(rtpp_cfg.rtpps[i].ip, str);
	}
	UGo_set_config(UGO_RTPP_CFG_MODULE_ID, &rtpp_cfg, 0);
}

void CFlyBase::IMLogin()
{
	ugo_test_login(m_im_ssid, 0x01);
}

void CFlyBase::IMLoginOut()
{
	ugo_test_logout();
}

BOOL CFlyBase::LoginAMS()
{
	char *pTmp = NULL;
	char cTmp[MAX_PATH] = { 0 };
	CStringA strTmp;
	CString strUrl = AMS_URL_ADDRESS;
	CStringA str = "?";

	str += "account=";
	strTmp.Format("%s", m_clientId);
	str += strTmp;

	str += "&accountpwd=";
	strcpy_s(cTmp, MAX_PATH, m_clientPwd);
	strcat_s(cTmp, MAX_PATH, "~U!X@I#N$");
	pTmp = MD5String(cTmp);
	strTmp.Format("%s", pTmp);
	str += strTmp;

	str += "&accounttype=mobile&ip=127.0.0.1&pv=pc&securityver=0";

	str += "&mainaccount=";
	strcpy_s(cTmp, MAX_PATH, m_accountSid);
	strcat_s(cTmp, MAX_PATH, "~U!X@I#N$");
	pTmp = MD5String(cTmp);
	strTmp.Format("%s", pTmp);
	str += strTmp;

	str += "&mainaccountpwd=";
	strcpy_s(cTmp, MAX_PATH, m_accountToken);
	strcat_s(cTmp, MAX_PATH, "~U!X@I#N$");
	pTmp = MD5String(cTmp);
	strTmp.Format("%s", pTmp);
	str += strTmp;

	strTmp.Format("LoginAMS start = %s\r\n", m_clientId);
	OutputDebugStringA(strTmp);

	HttpTool mHttpTool;
	CStringA straResult = "";
	if (mHttpTool.HttpGet(strUrl, str, straResult))
	{
		std::string src;
		src.append(straResult);
		// 打印返回
		strTmp.Format("LoginAMS resp = %s\r\n", straResult);
		OutputDebugStringA(strTmp);

		try
		{
			Json json(src);
			JsonDict *pRoot = json.getRoot()->toDict();
			if (pRoot != NULL)
			{
				if (pRoot->contain("result") && (int)(pRoot->getValue("result")->toNumber()) == 0)
				{
					if (pRoot->contain("phone"))
					{
						JsonValue *pValue = pRoot->getValue("phone");
						std::string dst;
						pValue->writeToStr(dst);

						strTmp.Format("%s", dst.c_str());
						strTmp.Replace("\"", "");
						strcpy_s(m_phone, MAX_PATH, strTmp);
					}
					if (pRoot->contain("ssid"))
					{
						JsonValue *pValue = pRoot->getValue("ssid");
						std::string dst;
						pValue->writeToStr(dst);

						strTmp.Format("%s", dst.c_str());
						strTmp.Replace("\"", "");
						strcpy_s(m_ssid, MAX_PATH, strTmp);
					}
					if (pRoot->contain("uid"))
					{
						JsonValue *pValue = pRoot->getValue("uid");
						std::string dst;
						pValue->writeToStr(dst);

						strTmp.Format("%s", dst.c_str());
						strTmp.Replace("\"", "");
						strcpy_s(m_uid, MAX_PATH, strTmp);
					}
					if (pRoot->contain("im_ssid"))
					{
						JsonValue *pValue = pRoot->getValue("im_ssid");
						std::string dst;
						pValue->writeToStr(dst);

						strTmp.Format("%s", dst.c_str());
						strTmp.Replace("\"", "");
						strcpy_s(m_im_ssid, MAX_PATH, strTmp);
					}
					if (pRoot->contain("name"))
					{
						JsonValue *pValue = pRoot->getValue("name");
						std::string dst;
						pValue->writeToStr(dst);

						strTmp.Format("%s", dst.c_str());
						strTmp.Replace("\"", "");
						strcpy_s(m_name, MAX_PATH, strTmp);
					}
					if (pRoot->contain("forwardnumber"))
					{
						JsonValue *pValue = pRoot->getValue("forwardnumber");
						std::string dst;
						pValue->writeToStr(dst);

						strTmp.Format("%s", dst.c_str());
						strTmp.Replace("\"", "");
						strcpy_s(m_forwardnumber, MAX_PATH, strTmp);
					}
					return TRUE;
				}
			}
		}
		catch (Exception e)
		{
			// 打印返回
			strTmp.Format("LoginAMS fail = %s\r\n", e.getDesc().c_str());
			OutputDebugStringA(strTmp);
		}
	}
	return FALSE;
}

BOOL CFlyBase::GetUrl()
{
	char *pTmp = NULL;
	char cTmp[MAX_PATH] = { 0 };
	CStringA strTmp;
	CString strUrl = GET_URL_ADDRESS;
	CStringA str = "?";

	str += "clientnum=";
	strTmp.Format("%s", m_clientId);
	str += strTmp;

	HttpTool mHttpTool;
	CStringA straResult = "";
	OutputDebugStringA("GetUrl start\r\n");
	if (mHttpTool.HttpGet(strUrl, str, straResult))
	{
		std::string src;
		src.append(straResult);
		// 打印返回
		strTmp.Format("GetUrl resp = %s\r\n", straResult);
		OutputDebugStringA(strTmp);

		try
		{
			Json json(src);
			JsonDict *pRoot = json.getRoot()->toDict();
			if (pRoot != NULL)
			{
				if (pRoot->contain("getcsurl"))
				{
					JsonValue *pValue = pRoot->getValue("getcsurl");
					std::string dst;
					pValue->writeToStr(dst);

					strTmp.Format("%s", dst.c_str());
					strTmp.Replace("\"", "");
					CS_URL_ADDRESS = StringA2String(strTmp) + L"?";
				}
				if (pRoot->contain("getrtppurl"))
				{
					JsonValue *pValue = pRoot->getValue("getrtppurl");
					std::string dst;
					pValue->writeToStr(dst);

					strTmp.Format("%s", dst.c_str());
					strTmp.Replace("\"", "");
					RTPP_URL_ADDRESS = StringA2String(strTmp) + L"?";
				}
				m_csAddress.clear();
				m_rtppAddress.clear();
				return TRUE;
			}
		}
		catch (Exception e)
		{
			// 打印返回
			strTmp.Format("GetUrl fail = %s\r\n", e.getDesc().c_str());
			OutputDebugStringA(strTmp);
		}
	}
	return FALSE;
}

BOOL CFlyBase::GetCS()
{
	char *pTmp = NULL;
	char cTmp[MAX_PATH] = { 0 };
	CStringA strTmp;
	CString strUrl = CS_URL_ADDRESS;
	CStringA str = "?";

	str += "clientnum=";
	strTmp.Format("%s", m_clientId);
	str += strTmp;

	HttpTool mHttpTool;
	CStringA straResult = "";
	OutputDebugStringA("GetCS start\r\n");
	if (mHttpTool.HttpGet(strUrl, str, straResult))
	{
		std::string src;
		src.append(straResult);
		// 打印返回
		strTmp.Format("GetCS resp = %s\r\n", straResult);
		OutputDebugStringA(strTmp);

		try
		{
			Json json(src);
			JsonDict *pRoot = json.getRoot()->toDict();
			if (pRoot != NULL)
			{
				if (pRoot->contain("data"))
				{
					JsonValue *pValue = pRoot->getValue("data");
					if (pValue != NULL)
					{
						pValue = pValue->toDict()->getValue("csaddr");
						if (pValue != NULL)
						{
							JsonList *pList = pValue->toList();
							if (pList != NULL)
							{
								for (int i = 0; i < pList->getCount(); i++)
								{
									pValue = pList->getValue(i);
									std::string dst;
									pValue->writeToStr(dst);

									strTmp.Format("%s", dst.c_str());
									strTmp.Replace("\"", "");
									m_csAddress.push_back(strTmp);
								}
								return TRUE;
							}
						}
					}
				}
			}
		}
		catch (Exception e)
		{
			// 打印返回
			strTmp.Format("GetCS fail = %s\r\n", e.getDesc().c_str());
			OutputDebugStringA(strTmp);
		}
	}
	return FALSE;
}

BOOL CFlyBase::GetRTPP()
{
	char *pTmp = NULL;
	char cTmp[MAX_PATH] = { 0 };
	CStringA strTmp;
	CString strUrl = RTPP_URL_ADDRESS;
	CStringA str = "?";

	str += "clientnum=";
	strTmp.Format("%s", m_clientId);
	str += strTmp;

	HttpTool mHttpTool;
	CStringA straResult = "";
	OutputDebugStringA("GetRTPP start\r\n");
	if (mHttpTool.HttpGet(strUrl, str, straResult))
	{
		std::string src;
		src.append(straResult);
		// 打印返回
		strTmp.Format("GetRTPP resp = %s\r\n", straResult);
		OutputDebugStringA(strTmp);

		try
		{
			Json json(src);
			JsonDict *pRoot = json.getRoot()->toDict();
			if (pRoot != NULL)
			{
				if (pRoot->contain("rtpp"))
				{
					JsonValue *pValue = pRoot->getValue("rtpp");
					if (pValue != NULL)
					{
						JsonList *pList = pValue->toList();
						if (pList != NULL)
						{
							for (int i = 0; i < pList->getCount(); i++)
							{
								pValue = pList->getValue(i);
								std::string dst;
								pValue->writeToStr(dst);

								strTmp.Format("%s", dst.c_str());
								strTmp.Replace("\"", "");
								m_rtppAddress.push_back(strTmp);
							}
							return TRUE;
						}
					}
				}
				if (pRoot->contain("stun"))
				{
					JsonValue *pValue = pRoot->getValue("stun");
					if (pValue != NULL)
					{
						std::string dst;
						pValue->writeToStr(dst);

						strTmp.Format("%s", dst.c_str());
						strTmp.Replace("\"", "");
						m_stumAddress = strTmp;
					}
				}
			}
		}
		catch (Exception e)
		{
			// 打印返回
			strTmp.Format("GetRTPP fail = %s\r\n", e.getDesc().c_str());
			OutputDebugStringA(strTmp);
		}
	}
	return FALSE;
}

void CFlyBase::StartWorkThread()
{
	StopWorkThread();
	// 启动线程
	m_hEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hThread = ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)WorkThread, this, 0, NULL);
}

void CFlyBase::StopWorkThread()
{
	SetEvent(m_hEvent);
	DWORD dwStart = GetTickCount();
	// 关闭句柄
	if (m_hThread != NULL)
	{
		while (1)
		{
			if (WaitForSingleObject(m_hThread, 1000) == WAIT_OBJECT_0)
			{
				break;
			}

			if (GetTickCount() - dwStart > 5000)
			{
				::TerminateThread(m_hThread, 0);
				break;
			}
		}

		CloseHandle(m_hThread);
		m_hThread = NULL;
	}
	if (m_hEvent != NULL)
	{
		CloseHandle(m_hEvent);
		m_hEvent = NULL;
	}
}

DWORD WINAPI CFlyBase::WorkThread(LPVOID lpParam)
{
	CFlyBase *pBase = (CFlyBase*)lpParam;

	while (1)
	{
		if (pBase->m_bWork)
		{
			BOOL bSuc = pBase->LoginAMS();
			if (bSuc)
			{
				bSuc = pBase->GetUrl();
				if (bSuc)
				{
					bSuc = pBase->GetCS();
					if (bSuc)
					{
						bSuc = pBase->GetRTPP();
						if (bSuc)
						{
							// 更新RTPP参数
							pBase->UpdateRtppParam();
						}
						// 更新标记
						pBase->m_bGetUrl = true;
						// 更新UGO参数
						pBase->UpdateParam();
						pBase->IMLogin();
						// 启动TCP连接
						pBase->TcpConnect();
					}
					else
					{
						if (pBase->m_CallBackFunc != NULL)
						{
							pBase->m_CallBackFunc(TCP_DISCONNECT, 3, 0, NULL, NULL);
						}
					}
				}
				else
				{
					if (pBase->m_CallBackFunc != NULL)
					{
						pBase->m_CallBackFunc(TCP_DISCONNECT, 2, 0, NULL, NULL);
					}
				}
			}
			else
			{
				if (pBase->m_CallBackFunc != NULL)
				{
					pBase->m_CallBackFunc(TCP_DISCONNECT, 1, 0, NULL, NULL);
				}
			}
			pBase->m_bWork = false;
		}

		if (WaitForSingleObject(pBase->m_hEvent, 1000) == WAIT_OBJECT_0)
		{
			break;
		}
	}
	return 0;
}