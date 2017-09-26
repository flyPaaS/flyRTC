#include <string.h> //strlen()
#include <stdio.h>
#include <time.h>

#ifdef _WIN32
#include <ws2tcpip.h>
#endif

#include "conductor.h"
#include "event_queue.h"
#include "audio_device.h"

using namespace webrtc;

#define	TraceLog(A,...) WEBRTC_TRACE(A, kTraceVoice, 0, __VA_ARGS__);    

#if !defined(_WIN32)
#define _snprintf snprintf
#endif

#if defined(_WIN32)
#ifndef strcasecmp
#define strcasecmp stricmp
#endif
#endif

const int RTT_RATIO = 3;  // 3 : 1
static ME_cb_vtable_t g_cvtablecb; /*App callback function*/
Conductor* Conductor::m_pInstance = NULL;
Conductor* g_pVoiceEngine = NULL;

#define NUMOFLOCALPORT      11  //num of local port spacing
#define NUMOFLOCALPORTUNIT 200  //num of one local port spacing

#define MAX_CAMERA_NAME_LENGTH 256

#define DEFAULT_CODEC_WIDTH			240
#define DEFAULT_CODEC_HEIGHT 		320
#define DEFAULT_FRAME_RATE 			15
#define DEFAULT_MIN_BITRATE			120
#define DEFAULT_MAX_BITRATE			400

//local port spacing
unsigned short ausLocalPort[NUMOFLOCALPORT] = 
{15004, 20004, 25004, 30004, 35004, 40004, 45004, 50004, 55004, 60004, 65004};

//cn packet pt(13)
unsigned char g_ucRtpCnPkt[] = { 
	0x80, 0x0d, 0x02, 0x38, 0xed, 0xc0, 0xb8, 0x09, 0x70, 0x3c, 
	0x46, 0x50, 0x45, 0x86, 0x67, 0xa5, 0x8a, 0x7a, 0x76, 0x7b,
	0x9c };

//keepalive packet
unsigned char g_ucRtpKeepAlivePkt[]={0xff};
/**************below's definitions for bitrate ajustability**************/
#define QU_DALTA  0.9*100 //Unloaded state threshold
#define QL_DALTA  0.7*100 //loaded state threshold
#define QC_DALTA  0.7*100 //Congested state threshold
#define RATE_DALTA  0.8*32768

enum eMe_bit_rate_status                           
{
	eME_RATE_UNLOADED,         //Unloaded state
	eME_RATE_LOADED,           //Loaded state
	eME_RATE_CONGESTED,        //Congested state
};

typedef struct tag_stPktInfo
{
    unsigned short usLen;   //RTP packet length
    unsigned short usRate;   //RTP packet rate
} stPktInfo;

typedef struct tag_stCodecRateInfo
{
    emd_codecs_bitrate_e usRateType;   //codec type
    unsigned short usRate;      //codec rate
} stCodecRateInfo;

/*codec rate table*/
#define MAX_RATE_CNT    10  //don't contain 0 index
#define AMR_RATE_CNT    4 //7   //contain 0 index
stCodecRateInfo iarAmrRate[] =
{
	{ eEMD_AMR_12200BITRATE, 12200 },
	{ eEMD_AMR_10200BITRATE, 10200 },
	{ eEMD_AMR_7950BITRATE, 7950 },
	{ eEMD_AMR_7400BITRATE, 7400 },
	{ eEMD_AMR_6700BITRATE, 6700 },
};

#define G729_RATE_CNT    4 //0   //contain 0 index
stCodecRateInfo iarG729Rate[] =
{
	{ eEMD_G729_8KBITRATE, 8000 },
	{ eEMD_G729_8KBITRATE, 8000 },
	{ eEMD_G729_8KBITRATE, 8000 },
	{ eEMD_G729_8KBITRATE, 8000 },
	{ eEMD_G729_8KBITRATE, 8000 },
};

#define AAC_RATE_CNT    4 //0   //contain 0 index
stCodecRateInfo iarAACRate[] =
{
	{ eEMD_AAC_16KBITRATE, 16000 },
	{ eEMD_AAC_16KBITRATE, 16000 },
	{ eEMD_AAC_16KBITRATE, 16000 },
	{ eEMD_AAC_16KBITRATE, 16000 },
	{ eEMD_AAC_16KBITRATE, 16000 },
};

#define SILK_RATE_CNT    4 //3   //contain 0 index
stCodecRateInfo iarSilkRate[] =
{
	{ eEMD_SILK_16KBITRATE, 16000 },
	{ eEMD_SILK_14KBITRATE, 14000 },
	{ eEMD_SILK_12KBITRATE, 12000 },
	{ eEMD_SILK_8KBITRATE, 8000 },
	{ eEMD_SILK_8KBITRATE, 8000 },
};

#define SILKWB_RATE_CNT    4 //5 //8   //contain 0 index
stCodecRateInfo iarSilkWBRate[] =
{
	{ eEMD_SILKWB_24KBITRATE, 24000 },
	{ eEMD_SILKWB_22KBITRATE, 22000 },
	{ eEMD_SILKWB_20KBITRATE, 20000 },
	{ eEMD_SILKWB_18KBITRATE, 18000 },
	{ eEMD_SILKWB_16KBITRATE, 16000 },
};

#define Opus_RATE_CNT    4 //5   //contain 0 index
stCodecRateInfo iarOpusRate[] =
{
	{ eEMD_Opus_24KBITRATE, 24000 },
	{ eEMD_Opus_22KBITRATE, 22000 },
	{ eEMD_Opus_20KBITRATE, 20000 },
	{ eEMD_Opus_18KBITRATE, 18000 },
	{ eEMD_Opus_16KBITRATE, 16000 },
};

#define Opusnb_RATE_CNT    4 //5   //contain 0 index
stCodecRateInfo iarOpusnbRate[] =
{
	{ eEMD_Opusnb_12KBITRATE, 12000 },
	{ eEMD_Opusnb_10KBITRATE, 10000 },
	{ eEMD_Opusnb_8KBITRATE, 8000 },
	{ eEMD_Opusnb_8KBITRATE, 8000 },
	{ eEMD_Opusnb_8KBITRATE, 8000 },
};

#define Opusfb_RATE_CNT    4 //0   //contain 0 index
stCodecRateInfo iarOpusfbRate[] =
{
	{ eEMD_Opusfb_48KBITRATE, 64000 },
	{ eEMD_Opusfb_48KBITRATE, 64000 },
	{ eEMD_Opusfb_48KBITRATE, 64000 },
	{ eEMD_Opusfb_48KBITRATE, 64000 },
	{ eEMD_Opusfb_48KBITRATE, 64000 },
};

#define ILBC_RATE_CNT    4 //5   //contain 0 index
stCodecRateInfo iarILBCRate[] =
{
	{ eEMD_ILBC_13KBITRATE, 13300 },
	{ eEMD_ILBC_13KBITRATE, 13300 },
	{ eEMD_ILBC_13KBITRATE, 13300 },
	{ eEMD_ILBC_13KBITRATE, 13300 },
	{ eEMD_ILBC_13KBITRATE, 13300 },
};

//packet rate that ip/udp/rtp need(Don't use other length, comment by Rambo.Fu)
#define MAX_PKT_CNT 	2	//contain 0 index.
stPktInfo astPktInfo8K[] =
{
    {160, 40*8*50},   //rtp rate every 20ms frame
    {320, 40*8*25},   //rtp rate every 40ms frame    
    {480, 40*8*17},   //rtp rate every 60ms frame    
};

stPktInfo astPktInfo16K[] =
{
    {320, 40*8*50},   //rtp rate every 20ms frame
    {640, 40*8*25},   //rtp rate every 40ms frame    
    {960, 40*8*17},   //rtp rate every 60ms frame    
};

stPktInfo astPktInfo48K[] =
{
	{ 960, 40 * 8 * 17 },   //rtp rate every 20ms frame
	{ 960, 40 * 8 * 17 },   //rtp rate every 40ms frame    
	{ 960, 40 * 8 * 17 },   //rtp rate every 60ms frame    
};

/**************end: below's definitions for bitrate ajustability**************/

static void UpdateEmodelCodecIeValue(emd_codecs_bitrate_e bitrate);
void UpdateEmodelCodecIeValue(emd_codecs_bitrate_e bitrate)
{
#ifdef EMODEL_ENABLED
    /* Modified By MNY@2015-01-19, delete kTraceReport */
	TraceLog(kTraceInfo, "UpdateEmodelCodecIeValue=%d\n", bitrate);
	emd_update_ie_value(bitrate);
#endif	
}

int rand_count(void)
{
	int rand_num = 0;

	srand((unsigned)time(NULL));
	
	rand_num = rand();

	return rand_num;
}

void Conductor::AddRef(int Val)
{
	_loadCnt += Val;
}

void Conductor::UpdateState(enum eME_state eState)
{
	_curState = eState;
}

Conductor::Conductor(void)
{
    _voeEngine = NULL;
    _voeBase        = NULL;
    _voeVqe         = NULL;
    _voeCodec       = NULL;
    _voeHardware    = NULL;
    _voeDtmf        = NULL;
    _voeVolume      = NULL;
    _voeRtp_Rtcp    = NULL;
    _voeFile        = NULL;
    _voeNetwork     = NULL;
	_voeNeteqStats = NULL;
#ifdef EXTERNAL_ENCRYPT
	_voeEncrypt = NULL;
#endif

#ifdef EXTERNAL_MEDIA_PROCESS
    _voeExtMedia        = NULL;
    _extMediaProc  = NULL;
#endif // EXTERNAL_MEDIA_PROCESS

    _intVoiceTransportPtr  = NULL;    
#ifdef EXTERNAL_TRANSPORT
	_extVoiceTransportPtr = NULL;
#endif

#ifdef EXTERNAL_AUDIO_DEVICE
    _extAudioDevice = NULL;
#endif

#ifdef EXTERNAL_PLAYFILE
	_extPlayFile = NULL;
#endif
    _voeAudioTransport = NULL;
    _ptrThread              = NULL;
    
    _ptrTimeEvent   = EventWrapper::Create();
    _critSect       = CriticalSectionWrapper::CreateCriticalSection();
	_critSectAudio  = CriticalSectionWrapper::CreateCriticalSection();
    
    memset(&_voiceNetInfo, 0x00, sizeof(ME_net_calc_info_t)); //added by luochengcong20141024
    memset(&_stEnvConfig, 0x00 ,sizeof(ME_ENV_cfg_t));//added by Rambo20141222
    memset(&_stVQEConfig, 0x00, sizeof(ME_VQE_cfg_t));
    memset(&_stCTRLConfig, 0x00, sizeof(ME_CTRL_cfg_t));
		
	_stCTRLConfig.ucRealTimeType = 0;
	_stCTRLConfig.ucVideoEnable = 0;
	_stCTRLConfig.ucEmodelEnable = 1;
	_stCTRLConfig.ucFecEnable = 0;//add by wuzhaoyang20140604
	_stCTRLConfig.ucIPv6Enable = 0;

	_stRTPConfig.uiRTPTimeout = COND_RTP_TIMEOUT_DEFAULT;
	_stRTPConfig.uiFixLowPayload = false;

    _stVQEConfig.Ec_enable   = true;		
	_stVQEConfig.Agc_enable  = true;	
	_stVQEConfig.Ns_enable   = true;
    _stVQEConfig.Agc_targetDbfs = AGC_TARGET_DBOV_DEFAULT;
    _stVQEConfig.Agc_compressionGaindB = AGC_COMPRESSION_DB_DEFAULT;
    
    _voiceChannelId       = -1;	
 	_recordDeviceNum         = 0;
 	_playDeviceNum           = 0;
	
    _audioDeviceReady      = eDEVICE_GET_FAIL;

    _recordMode             = -1;  // default shutdown
    _recordFormat           = 1; // default wav file
    _playFileDirect           = -1;

	_curState          = eME_done;
	_loadCnt           = 0;
	_threadKeepAlive   = false;
	_voeP2PRtt            = 0;
	_voeRtppRtt           = 0;
	_iceMode           = eME_ICE_RTPP_MD;

	_consultFecEn         = false;
	_consultRtppMultFec    = false; //add by wuzhaoyang20151225
	_smoothLossRate = 50;       //add by wuzhaoyang20151225

	/*reset audio stream param---rookie20140617*/
	ResetAudioStreamParam();

	//need to set RTP/Voice engine when start audio;
	_updateCfgFlag = CFG_UPDATE_VOICE | CFG_UPDATE_RTP | CFG_UPDATE_TGO | CFG_UPDATE_FEC;
	
    _usLastRate         = 0;            //last stream rate contain codec and packet
    _usLastCodeRateIdx  = 0;            //last codec rate index
    _usLastPktLenIdx    = 0;            //last packet length index
}

Conductor::~Conductor(void)
{
    delete _ptrTimeEvent;
	delete _critSect;
	delete _critSectAudio;
}

int Conductor::LoadAPIs(int iDataSource)
{
    _voeEngine = VoiceEngine::Create();
	_voeBase  = VoEBase::GetInterface(_voeEngine);
	_voeVqe   = VoEAudioProcessing::GetInterface(_voeEngine);
	_voeCodec = VoECodec::GetInterface(_voeEngine);
	_voeHardware    = VoEHardware::GetInterface(_voeEngine);
	_voeDtmf        = VoEDtmf::GetInterface(_voeEngine);
	_voeVolume      = VoEVolumeControl::GetInterface(_voeEngine);
	_voeRtp_Rtcp    = VoERTP_RTCP::GetInterface(_voeEngine);
	_voeNeteqStats = VoENetEqStats::GetInterface(_voeEngine);
	_voeFile        = VoEFile::GetInterface(_voeEngine);
	_voeNetwork     = VoENetwork::GetInterface(_voeEngine);
#ifdef EXTERNAL_ENCRYPT
	_voeEncrypt = VoEEncryption::GetInterface(_voeEngine);
#endif

#ifdef EXTERNAL_MEDIA_PROCESS
	_voeExtMedia    = VoEExternalMedia::GetInterface(_voeEngine);
#endif // EXTERNAL_MEDIA_PROCESS	

	int dbLevel = kTraceReport | kTraceError;
	VoiceEngine::SetTraceFilter(dbLevel);   
	
    //test code
#if defined(ANDROID)
    VoiceEngine::SetTraceFile("/mnt/sdcard/WebRtc_trace.txt");
#elif defined(_WIN32) //&& defined(WEBRTC_WRITE_TO_FILE)
	_voeEngine->SetTraceFile("D:\\WebRtc_trace.txt");
#endif

#ifdef EXTERNAL_AUDIO_DEVICE
    if (iDataSource == 1)
    {
        _extAudioDevice = ExlAudioDeviceModuleImpl::Create();
    }
#endif

#ifdef EXTERNAL_TRANSPORT /*add by rookie for extransport 20131106*/
	_extVoiceTransportPtr = new CExternalTransportImpl();
	if (_extVoiceTransportPtr == NULL)
	{
		TraceLog(kTraceError, "WebRTC! _extVoiceTransportPtr handle null.");
		return -1;
	}
#endif

#ifdef EXTERNAL_PLAYFILE
	_extPlayFile = new CExlPlayFileAdapter();
	if (_extPlayFile == NULL)
	{
		TraceLog(kTraceError, "WebRTC! _extPlayFile handle null.");
		return -1;
	}
#endif

#ifdef EXTERNAL_MEDIA_PROCESS
    _extMediaProc = new MediaProcessImpl();
    if ( _extMediaProc == NULL )
    {
        TraceLog(kTraceError, "WebRTC! _extMediaProc handle null.");
        return -1;
    }
#endif // EXTERNAL_MEDIA_PROCESS

    /* add by Vinton.Liu on 2016-03-29 for support external audio device push pcm to and pull pcm from engine begin */
    _voeAudioTransport = new VoEAudioTransportImpl();
    if ( _voeAudioTransport == NULL )
    {
        TraceLog(kTraceError, "WebRTC! _voeAudioTransport handle null.");
    }
    /* Vinton.Liu add end */

	if( NULL == _voeEngine|| NULL == _voeBase||NULL == _voeVqe || NULL == _voeCodec
		||NULL == _voeHardware|| NULL == _voeDtmf || NULL == _voeVolume || NULL == _voeRtp_Rtcp
		|| NULL == _voeFile || NULL == _voeNetwork || NULL == _voeNeteqStats
#ifdef EXTERNAL_ENCRYPT
        || NULL == _voeEncrypt        
#endif
#ifdef EXTERNAL_MEDIA_PROCESS
        || NULL == _voeExtMedia
#endif // EXTERNAL_MEDIA_PROCESS
    )
	{
		TraceLog(kTraceError,"WebRTC! InitEngine handle null.");
		return -1;
	}
	
#ifdef _WIN32	
	if(_voeHardware)
	{
		_voeHardware->GetNumOfRecordingDevices(_recordDeviceNum);
		_voeHardware->GetNumOfPlayoutDevices(_playDeviceNum);
	}
#endif	
    
    /**add by Rookie John20120701 register callback */
	_voeBase->RegisterVoiceEngineObserver(*this);

	return 0;
}

/*init audio device*/
int Conductor::AudioDeviceInit(void) 
{
    int iRet = 0;

	if (eDEVICE_GET_FAIL == _audioDeviceReady)
	{
#ifdef EXTERNAL_AUDIO_DEVICE
		iRet = _voeBase->Init(_extAudioDevice); 
#else
		iRet = _voeBase->Init(); 
#endif
		if ( -1 == iRet )
		{	
			TraceLog(kTraceError, "InitEngine error\n");
			_audioDeviceReady = eDEVICE_GET_FAIL;
		}
		else
		{
			_audioDeviceReady = (eDevice_status)iRet;
		}

#ifdef __APPLE__
		/*check audio device state add by rookie for ios backgroud mode20131129*/
		if (_voeHardware)
		{
			bool isAvailable = false;
			_voeHardware->GetPlayoutDeviceStatus(isAvailable);
			TraceLog(kTraceReport, "GetPlayoutDeviceStatus(%d)\n", isAvailable);

			_voeHardware->GetRecordingDeviceStatus(isAvailable);
			TraceLog(kTraceReport, "GetRecordingDeviceStatus(%d)\n", isAvailable);
		}
#endif

	}
	return(iRet);
} 

int Conductor::AuthAndInit(void)
{
    int iRet = 0;

	/*don't return if fail, beacuse mic maybe 
	is ok. modified by Rambo Fu on 2013-1-22*/
    iRet = AudioDeviceInit();

#ifdef _WIN32//WINDOWS  //jkh+
	SetMicVolume(255);
	SetMicrophoneBoost(true);
#endif	

#ifdef EMODEL_ENABLED
	emd_init();
#endif

#ifdef ME_USE_EVENT_LIST
	/* Added by gonghuojin, 2013/9/3   PN:event list init */
	evt_init();
#endif

	/* add by Vinton.Liu on 20160301 for may dead lock when initialize*/
    /* Set trace callback must behind event queue created( evt_init() ) ,
           because Print() will call thread mute lock to push event queue.    */
	VoiceEngine::SetTraceCallback(this);
	
	StartThread();//start thread to detect event
	TraceLog(kTraceReport, " AuthAndInit ok");
	return iRet;
}

int Conductor::GetCurState(void)
{
	return _curState;
}

int Conductor::GetCurLoadCnt(void)
{
	return _loadCnt;
}

/* BEGIN: Added by gonghuojin, 2013/11/27   PN:media rtt value */
int Conductor::ResetRTT(void)
{
	_voeP2PRtt = 0;
	_voeRtppRtt = 0;
	return 0;
}

int Conductor::GetRTT(void)
{		
	return (_iceMode==eME_ICE_P2P_MD)?_voeP2PRtt:_voeRtppRtt;
}

int Conductor::RatioRTT(int oRTT, int nRTT)
{
	int rtt = 0;
	
	if (oRTT == 0)
		rtt = nRTT;
	else
	{
		// Weighting of the old rtt value to new data.
		rtt = (RTT_RATIO * oRTT + nRTT) / (RTT_RATIO+1);
	}

	return rtt;
}

int Conductor::UpdateRTT(int rtt)
{
	if (_iceMode == eME_ICE_P2P_MD)
	{
		_voeP2PRtt = RatioRTT(_voeP2PRtt, rtt);
	}	
	else
	{
		_voeRtppRtt = RatioRTT(_voeRtppRtt, rtt);
	}
	
	return 0;
}

/* BEGIN: Added by gonghuojin, 2014/1/2   PN:restart audio stream*/
int Conductor::RestartAudioStream(void)
{
	int iRet;

	if (_voiceChannelId < 0)
	{
		TraceLog(kTraceError, "Conductor::RestartAudioStream _voiceChannelId Failed!");
		return -1;
	}
	
	_voeBase->StopSend(_voiceChannelId);
	_voeBase->StopPlayout(_voiceChannelId);
	_voeBase->StopReceive(_voiceChannelId);

	//need a dynamic setting, this is a temporary method;
	SetLocalReceiver(0, _voiceChannelId, 15004);

	iRet = _voeBase->StartReceive(_voiceChannelId);
	if (0 != iRet)
	{
	    _voeDnLinkSinglepassReason |= eME_REASON_START_RECEIVE_FAIL;
		TraceLog(kTraceError, "Conductor::RestartAudioStream StartReceive Failed!");
	}   

	iRet = _voeBase->StartPlayout(_voiceChannelId);
	if (0 != iRet)
	{
		TraceLog(kTraceError, "Conductor::RestartAudioStream StartPlayout Failed!");
	}

	iRet = _voeBase->StartSend(_voiceChannelId);
	if (0 != iRet)
	{
	    _voeUpLinkSinglepassReason |= eME_REASON_START_SEND_FAIL;
		TraceLog(kTraceError, "Conductor::RestartAudioStream StartSend Failed!");
	}

	iRet = _voeDtmf->SetSendTelephoneEventPayloadType(_voiceChannelId, 
                                                    DTMF_PAYLOAD_TYPE_DEFAULT);
	if (0 != iRet)
	{
		TraceLog(kTraceError, "Conductor::RestartAudioStream SetSendTelephoneEventPayloadType Failed!");
	}
 
	return 0;
}
/* END:   Added by gonghuojin, 2014/1/2 */

int Conductor::RestartAudioStreamSend(void)//add by wuzhaoyang20140709
{
	int iRet;

	if (_voiceChannelId < 0)
	{
		TraceLog(kTraceError, "Conductor::RestartAudioStreamSend _voiceChannelId Failed!");
		return -1;
	}

	_voeBase->StopSend(_voiceChannelId);
	
	iRet = _voeBase->StartSend(_voiceChannelId);
	if (0 != iRet)
	{
		_voeUpLinkSinglepassReason |= eME_REASON_START_SEND_FAIL;
		TraceLog(kTraceError, "Conductor::RestartAudioStreamSend StartSend Failed!");
	}

	iRet = _voeDtmf->SetSendTelephoneEventPayloadType(_voiceChannelId, 
                                                    DTMF_PAYLOAD_TYPE_DEFAULT);
	if (0 != iRet)
	{
		TraceLog(kTraceError, "Conductor::RestartAudioStreamSend SetSendTelephoneEventPayloadType Failed!");
	}

	return 0;
}

int Conductor::UpdateIceMode(int ice_mode)
{
	_iceMode = (eMeIceMode)ice_mode;
	return 0;
}

int Conductor::GetIceMode()
{
    return _iceMode;
}

int Conductor::GetEmodelValue(ME_emodel_info_t* eValue)
{
#ifdef EMODEL_ENABLED
	if (NULL == eValue)
	{
	    TraceLog(kTraceError, "GetEmodelValue() input param is invalid.");
		return -1;
	}
	
    //add by luochengcong20141023
    eValue->delay.min = _voiceNetInfo.delay.min;
    eValue->delay.max = _voiceNetInfo.delay.max;
    eValue->delay.average = _voiceNetInfo.delay.average;
    eValue->jitbuf.min = _voiceNetInfo.jitbuf.min;
    eValue->jitbuf.max = _voiceNetInfo.jitbuf.max;
    eValue->jitbuf.average = _voiceNetInfo.jitbuf.average;
    //add by luochengcong20141023

	eValue->pkt_stat.pktRecv = _voePktReceived;
	eValue->pkt_stat.pktSnd = _voePktSent;
	
	if (_stCTRLConfig.ucEmodelEnable)
	{
		CriticalSectionScoped lock(_critSect);
		emd_get_value(eValue);
	}
    
#endif

	return 0;
}

int Conductor::GetChannelCodecRate(int channelid, void* pCodecRateAttr, int &max_codec_cnt, void* pPktAttr, int &max_pkt_cnt)
{
	if (NULL == pCodecRateAttr || NULL == pPktAttr || channelid < 0)
	{
		TraceLog(kTraceError, "GetChannelCodecRateAttr param is error!!!");
		return -1;
	}
//#define __INTER_DEBUG__
	CodecInst inst;
	memset(&inst, 0, sizeof(CodecInst));
	//get codec type and type number
	_voeCodec->GetSendCodec(_voiceChannelId, inst);

	if(strcasecmp("G729",inst.plname) == 0)
	{
		memcpy(pCodecRateAttr, iarG729Rate, sizeof(iarG729Rate));
		memcpy(pPktAttr, astPktInfo8K, sizeof(astPktInfo8K));
        max_codec_cnt = G729_RATE_CNT;
        max_pkt_cnt = MAX_PKT_CNT;
#ifdef __INTER_DEBUG__
        printf("Current send Codec is G729\n");
#endif
	}
	else if (strcasecmp("AAC", inst.plname) == 0)
	{
		memcpy(pCodecRateAttr, iarAACRate, sizeof(iarAACRate));
		memcpy(pPktAttr, astPktInfo16K, sizeof(astPktInfo16K));
		max_codec_cnt = AAC_RATE_CNT;
		max_pkt_cnt = MAX_PKT_CNT;
#ifdef __INTER_DEBUG__
		printf("Current send Codec is AAC\n");
#endif
	}
	else if(strcasecmp("AMR",inst.plname) == 0)
    {
		memcpy(pCodecRateAttr, iarAmrRate, sizeof(iarAmrRate));
		memcpy(pPktAttr, astPktInfo8K, sizeof(astPktInfo8K));
        max_codec_cnt = AMR_RATE_CNT;
        max_pkt_cnt = MAX_PKT_CNT;
#ifdef __INTER_DEBUG__
        printf("Current send Codec is AMR\n");
#endif
    }
	else if(strcasecmp("SILK",inst.plname) == 0)
	{
		memcpy(pCodecRateAttr, iarSilkRate, sizeof(iarSilkRate));
		memcpy(pPktAttr, astPktInfo8K, sizeof(astPktInfo8K));
        max_codec_cnt = SILK_RATE_CNT;
        max_pkt_cnt = MAX_PKT_CNT;
#ifdef __INTER_DEBUG__
        printf("Current send Codec is SILK\n");
#endif
	}
	else if(strcasecmp("SILKWB",inst.plname) == 0)
	{
		memcpy(pCodecRateAttr, iarSilkWBRate, sizeof(iarSilkWBRate));
		memcpy(pPktAttr, astPktInfo16K, sizeof(astPktInfo16K));
        max_codec_cnt = SILKWB_RATE_CNT;
        max_pkt_cnt = MAX_PKT_CNT;
#ifdef __INTER_DEBUG__
        printf("Current send Codec is SILKWB\n");
#endif
	}
	else if(strcasecmp("opus",inst.plname) == 0)
  {
    /*add by wuhui*/
    memcpy(pCodecRateAttr, iarOpusRate, sizeof(iarOpusRate));
    memcpy(pPktAttr, astPktInfo16K, sizeof(astPktInfo16K));
    max_codec_cnt = Opus_RATE_CNT;
    max_pkt_cnt = MAX_PKT_CNT;
    /*wuhui add end*/
#ifdef __INTER_DEBUG__
        printf("Current send Codec is opus\n");
#endif
	}
/* (Begin) Added by WeiYouquan in 2015.12.29*/
	else if (strcasecmp("Opusnb", inst.plname) == 0)
	{
		memcpy(pCodecRateAttr, iarOpusnbRate, sizeof(iarOpusnbRate));
		memcpy(pPktAttr, astPktInfo8K, sizeof(astPktInfo8K));
		max_codec_cnt = Opusnb_RATE_CNT;
		max_pkt_cnt = MAX_PKT_CNT;
#ifdef __INTER_DEBUG__
		printf("Current send Codec is Opusnb\n");
#endif
	}
	else if (strcasecmp("opusfb", inst.plname) == 0)
	{
		/*add by wuhui*/
		memcpy(pCodecRateAttr, iarOpusfbRate, sizeof(iarOpusfbRate));
		memcpy(pPktAttr, astPktInfo48K, sizeof(astPktInfo48K));
		max_codec_cnt = Opusfb_RATE_CNT;
		max_pkt_cnt = MAX_PKT_CNT;
		/*wuhui add end*/
#ifdef __INTER_DEBUG__
		printf("Current send Codec is opusfb\n");
#endif
	}
	else if (strcasecmp("ILBC", inst.plname) == 0)
	{
		memcpy(pCodecRateAttr, iarILBCRate, sizeof(iarILBCRate));
		memcpy(pPktAttr, astPktInfo8K, sizeof(astPktInfo8K));
		max_codec_cnt = ILBC_RATE_CNT;
		max_pkt_cnt = MAX_PKT_CNT;
#ifdef __INTER_DEBUG__
		printf("Current send Codec is ILBC\n");
#endif
	}
/* (End) Added by WeiYouquan in 2015.12.29*/
    else
    {
    	TraceLog(kTraceError,"Don't exist codec in get rate: %s", inst.plname);
    	return -1;
    }
#undef __INTER_DEBUG__
	return 0;
}

#ifdef EXTERNAL_ENCRYPT
void Conductor::encrypt(
        int channel,
        unsigned char* in_data,
        unsigned char* out_data,
        int bytes_in,
        int* bytes_out)
{
    if (g_cvtablecb.encrypt != NULL)
    {
        g_cvtablecb.encrypt(in_data, out_data, bytes_in, bytes_out);
    }
}

void Conductor::decrypt(
        int channel,
        unsigned char* in_data,
        unsigned char* out_data,
        int bytes_in,
        int* bytes_out)
{
    if (g_cvtablecb.decrypt != NULL)
    {
        g_cvtablecb.decrypt(in_data, out_data, bytes_in, bytes_out);
    }
}

void Conductor::encrypt_rtcp(
        int channel,
        unsigned char* in_data,
        unsigned char* out_data,
        int bytes_in,
        int* bytes_out)
{
    if (g_cvtablecb.encrypt != NULL)
    {
        g_cvtablecb.encrypt(in_data, out_data, bytes_in, bytes_out);
    }
}

void Conductor::decrypt_rtcp(
        int channel,
        unsigned char* in_data,
        unsigned char* out_data,
        int bytes_in,
        int* bytes_out)
{
    if (g_cvtablecb.decrypt != NULL)
    {
        g_cvtablecb.decrypt(in_data, out_data, bytes_in, bytes_out);
    }
}
#endif

int Conductor::CallBackVtable(ME_cb_vtable_t* pvtable)
{
	if (pvtable != NULL)
	{
		memcpy(&g_cvtablecb, pvtable, sizeof(ME_cb_vtable_t));
		evt_callback((evt_cb_f)g_cvtablecb.event_cb);

#ifdef EXTERNAL_TRANSPORT
		if (g_cvtablecb.send_cb && _extVoiceTransportPtr)
			_extVoiceTransportPtr->RegisterSendCallback((exsend_data_cb_f)g_cvtablecb.send_cb);
#endif

#ifdef EXTERNAL_MEDIA_PROCESS
		if ( g_cvtablecb.extmedia_cb && _extMediaProc )
		{
		    _extMediaProc->RegisterExternMediaProcessingCb((ex_media_proc_cb_f)g_cvtablecb.extmedia_cb);
		}
#endif // EXTERNAL_MEDIA_PROCESS
	}

    /* add by Vinton.Liu on 2016-03-29 for support external audio device push pcm to and pull pcm from engine begin */
    if ( _voeAudioTransport )
    {
        AudioTransportApiCb_t callbacks = { NULL };
        callbacks.initplayout_cb = g_cvtablecb.initplayout_cb;
        callbacks.initrecording_cb = g_cvtablecb.initrecording_cb;
        callbacks.pull_capture_cb = g_cvtablecb.pull_capture_cb;
        callbacks.push_render_cb = g_cvtablecb.push_render_cb;
        _voeAudioTransport->RegisterCallbackTable(&callbacks);
    }
    /* Vinton.Liu add end */
        
	TraceLog(kTraceReport, "callback vtable register ok!");

	return 0;
}

void Conductor::CallbackOnError(const int channel, const int errCode)
{
	char strTmp[256] = {0};
 
	switch (errCode)
	{
        case VE_RECEIVE_PACKET_TIMEOUT:
            if (GetCurState() == eME_Connected)
            {
                _snprintf(strTmp, 256, "Channel[%d]RTP timeout!!!\n", channel);
                evt_appand(eME_RTP_TIMEOUT_EV, \
                            eME_REASON_RTP_TIMEOUT, strTmp, NULL, 0);
				TraceLog(kTraceReport, "%s", strTmp);
            }
            break;

        case VE_PACKET_RECEIPT_STARTED:
        case VE_PACKET_RECEIPT_RESTARTED:
			if (GetCurState() != eME_Connected)
		    {
		    	_snprintf(strTmp, 256, "Channel[%d]RTP recevied!!!\n", channel);
		    	evt_appand(eME_EARLY_MEDIA_EV, \
		    	            eME_REASON_EARLY_MEDIA, strTmp, NULL, 0);
				TraceLog(kTraceReport, "%s", strTmp);
			}
            break;
        case VE_FILE_PLAY_TO_ENDED:
            if ( GetCurState() == eME_Connected )
            {
                _snprintf(strTmp, 256, "Channel[%d] file play to end!!!\n", channel);
				evt_appand(eME_FILE_PLAY_TO_ENDED, \
				            eME_REASON_FILE_PLAY_TO_ENDED, strTmp, NULL, 0);
				TraceLog(kTraceReport, "%s", strTmp);
            }
            break;
            
//only for android platform. comment by Rambo.Fu 2014-2-28
#ifdef ANDROID
        /*we can consider restart udp port with supposing receiver is error here. 
        	   added by Rambo.Fu on 2013-11-06*/
        case VE_RECEIVE_PACKET_SINGLEPASS:  
            if (GetCurState() == eME_Connected)
            {
            	/* BEGIN: Added by gonghuojin, 2013/12/24   PN:appand to event list, restart in ugo */
                _snprintf(strTmp, 256, \
                    "Channel[%d]RTP singlepass, try to restart udp port!!!\n", channel);
				evt_appand(eME_RTP_DNSINGLEPASS_EV, \
				            eME_REASON_LOCAL_RECEIVE_FAIL, strTmp, NULL, 0);
				TraceLog(kTraceReport,"%s", strTmp); 
				/* END:   Added by gonghuojin, 2013/12/24 */
            }
 
            break;
#endif
        default:
			break;
	}
	
	return;
}

//add by wuzhaoyang20150310 for get dtmf data
void Conductor::OnReceivedTelephoneEventOutOfBand(int channel, int eventCode, bool endOfEvent)
{
	char strTmp[256] = { 0 };
	//CriticalSectionScoped lock(_critSect);
	if (endOfEvent == true)
	{
		_snprintf(strTmp, 256, "Have get the dtmf data, dtmfvalue = %d\n", eventCode);
		evt_appand(eME_GET_DTMF_EV, eventCode, strTmp, NULL, 0);
		TraceLog(kTraceReport, "%s", strTmp);
    }
	return;
}

void Conductor::OnReceivedTelephoneEventInband(int channel, int eventCode, bool endOfEvent)
{
	char strTmp[256] = { 0 };
	//CriticalSectionScoped lock(_critSect);
	if (endOfEvent == true)
	{
		_snprintf(strTmp, 256, "Have get the dtmf data, dtmfvalue = %d\n", eventCode);
		evt_appand(eME_GET_DTMF_EV, eventCode, strTmp, NULL, 0);
		TraceLog(kTraceReport, "%s", strTmp);
	}
	return;
}

/*add by wuhui*/
int Conductor::SetAudioCodecRate(unsigned short usCurStatus, unsigned short rate)
{
  unsigned short usCurCodeRate;        //current codec bitrate 
  unsigned short usCurPktLen;          //current packet length
  unsigned short usCurCodeRateIdx;    //current codec bitrate index
  unsigned short usCurPktLenIdx;      //current packet length index
  //    unsigned short usCurStatus;         //current network status
  unsigned short usMaxRate;            //max bitrate
  unsigned short usMinRate;            //min bitrate

  stCodecRateInfo stCodeTypeTbl[MAX_RATE_CNT];
  stPktInfo   stPktTypeTbl[MAX_PKT_CNT+1];
  int iCodeTypeNum = 0; //the number of codec		
  int iPktTypeNum = 0; //the number of packet

  if (_voiceChannelId < 0)
  {
    return -1;
  }

  if (GetChannelCodecRate(_voiceChannelId, stCodeTypeTbl, iCodeTypeNum, stPktTypeTbl, iPktTypeNum) < 0)
  {
    TraceLog(kTraceError, "SetAudioCodecRate: get codec rate info error!!");
    return -1;
  }

  //Get max and min bitrate
  usMinRate = stCodeTypeTbl[iCodeTypeNum].usRate + stPktTypeTbl[iPktTypeNum].usRate;
  usMaxRate = stCodeTypeTbl[0].usRate + stPktTypeTbl[0].usRate;

  //Get rate index that we can update.    
  usCurCodeRateIdx = _usLastCodeRateIdx;
  usCurCodeRate = stCodeTypeTbl[usCurCodeRateIdx].usRate;
  usCurPktLenIdx = _usLastPktLenIdx;
  usCurPktLen = stPktTypeTbl[usCurPktLenIdx].usLen;

  /* (Begin) Added by WeiYouquan in 2015.12.30 */
  switch (usCurStatus)
  {
  case NETSTATE_EXCELENT:
	  if (_usLastRate != usMinRate)
	  {
		  usCurCodeRateIdx = 0;
	  }
	  break;

  case NETSTATE_NICE:
	  if (_usLastRate != usMinRate)
	  {
		  usCurCodeRateIdx = 1;
	  }
	  break;

  case NETSTATE_GOOD:
	  if (_usLastRate != usMinRate)
	  {
		  usCurCodeRateIdx = 2;
	  }
	  break;

  case NETSTATE_FAIR:
	  if (_usLastRate != usMinRate)
	  {
		  usCurCodeRateIdx = 3;
	  }
	  break;

  case NETSTATE_POOR:
	  if (_usLastRate != usMinRate)
	  {
		  usCurCodeRateIdx = 4;
	  }
	  break;

  default:
	  break;
  }
  /* (End) Added by WeiYouquan in 2015.12.30 */

  CodecInst inst;
  memset(&inst, 0, sizeof(CodecInst));
  //get codec type and type number
  _voeCodec->GetSendCodec(_voiceChannelId, inst);
  //set bitrate    
  if (usCurCodeRateIdx != _usLastCodeRateIdx)
  {
    usCurCodeRate = stCodeTypeTbl[usCurCodeRateIdx].usRate;
    _voeCodec->SetBitRate(_voiceChannelId, usCurCodeRate);
    _usLastRate = usCurCodeRate;

	TraceLog(kTraceReport, "change code bitrate from %d to %d", \
        stCodeTypeTbl[_usLastCodeRateIdx].usRate, usCurCodeRate);
    _usLastCodeRateIdx = usCurCodeRateIdx;

    //update emodel codec type
    UpdateEmodelCodecIeValue(stCodeTypeTbl[usCurCodeRateIdx].usRateType);
  }

#if 0
  else if (usCurPktLenIdx != _usLastPktLenIdx)
  {
    usCurPktLen = stPktTypeTbl[usCurPktLenIdx].usLen;
    _voeCodec->SetPayloadLen(_voiceChannelId, usCurPktLen);
	TraceLog(kTraceReport, "change rtp packet lenght from %d to %d", \
        stPktTypeTbl[_usLastPktLenIdx].usLen, usCurPktLen);
    _usLastPktLenIdx = usCurPktLenIdx;    
  }
#endif
  return ((usCurPktLen * 1000) / inst.plfreq);
}
/*wuhui add end*/

//webRTC log trace
void Conductor::Print(const TraceLevel level, const char *traceString, const int length)
{
    if ( level == kTraceError)
    {
		evt_appand(eME_LOG_TRACE_EV, eME_REASON_LOG_LEVEL_ERR, \
		    traceString, NULL, 0);
    }
	else  if (level == kTraceReport)
	{
		evt_appand(eME_LOG_TRACE_EV, eME_REASON_LOG_LEVEL_INFO, \
		    traceString, NULL, 0);
	}
}

#ifdef EXTERNAL_TRANSPORT
int Conductor::RecvDataPacket(const recv_pm_t* recv_pm)
{
    if ( GetCurState() != eME_Running && GetCurState() != eME_Connected )
    {
        return 0;
    }
    
	if (_voeNetwork != NULL && recv_pm != NULL && recv_pm->len != 0)
	{
		if (recv_pm->media_type == eME_MEDIA_AUDIO)
		{
			if (recv_pm->data_type == eME_MEDIA_RTP && _voiceChannelId != -1)
			{
				_voeNetwork->ReceivedRTPPacket(_voiceChannelId, recv_pm->data, recv_pm->len);
			}
			else
			{
				_voeNetwork->ReceivedRTCPPacket(_voiceChannelId, recv_pm->data, recv_pm->len);
			}
		}

	}
	
	return 0;
}

#endif

/*try to apply local port to fix issues that apply port failed.
*Added by Rambo.Fu on 2013-9-12.*/
int Conductor::SetLocalReceiver(int type, int channel, int port)
{
	int iRet = -1;

	if (port > 0)
	{
		iRet = _intVoiceTransportPtr->SetLocalReceiver(port);
	}

	if (0 != iRet)
	{
        int i, j;
	    i = channel%NUMOFLOCALPORT;
	    j = rand_count()%NUMOFLOCALPORTUNIT;

	    do{ //will try to apply 100 * NUMOFLOCALPORT times if failed
	    	port = ausLocalPort[i++] + j;

			iRet = _intVoiceTransportPtr->SetLocalReceiver(port);

	    	if (0 == iRet)
	    	{
	            break;
	        }
	        else if(i == NUMOFLOCALPORT)
	        {
	            i = 0;
	            j += 2;
	            if (j > NUMOFLOCALPORTUNIT)
	            {
					break;
	            }
	        }
	    }while(0 != iRet);
    }

	if (0 != iRet)
	{
		if(type == 0)
            _voeDnLinkSinglepassReason |= eME_REASON_SET_LOCAL_RECEIVER_FAIL;

		TraceLog(kTraceError, \
		    "webrtc SetLocalReceiver(%d) Failed(Error = %d)!", \
		    port, _intVoiceTransportPtr->LastError());
	}
	
	return(iRet);
}

int Conductor::SetAudioChannel(const ME_audio_stream_info_t* paudioinfo)
{
	int iRet = 0;
	bool dtmf_enable = false;
	bool dtmf_directenable = false;
	TelephoneEventDetectionMethods detectionMethod = kOutOfBand;

 
	if (NULL == paudioinfo)
	{
		TraceLog(kTraceError, "SetAudioChannel paudioinfo is NULL !!");
		return -1;
	}

	if (_voiceChannelId < 0)
	{
		TraceLog(kTraceError, "SetAudioChannel _voiceChannelId < 0 !!");
		return -1;
	}

	CriticalSectionScoped lock(_critSectAudio);
	
	/* Attention, payload type should be matched with engine */
	iRet = _voeDtmf->SetSendTelephoneEventPayloadType(_voiceChannelId, 
	                                                DTMF_PAYLOAD_TYPE_DEFAULT);
	if (0 != iRet)
	{
		TraceLog(kTraceError, "SetSendTelephoneEventPayloadType Failed!");
	}

	//add by wuzhaoyang20150309
	iRet = _voeDtmf->RegisterTelephoneEventDetection(_voiceChannelId, detectionMethod, *this);
	if (0 != iRet)
	{
		TraceLog(kTraceError, "RegisterTelephoneEventDetection Failed!");
	}

	if (0 != SetSendCodec(paudioinfo->iPayLoadType, paudioinfo))
	{
		_voeBase->DeleteChannel(_voiceChannelId);
		_voiceChannelId = -1;
		TraceLog(kTraceError, "SetSendCodec failure");
		return -1;
	}

	if (0 != SetExTransport(paudioinfo->ucExTransportEnable?true:false, 
							paudioinfo->iLocalAudioPort, 
							paudioinfo->iRemoteAudioPort, 
							paudioinfo->cRemoteAudioIp))
	{
		_voeBase->DeleteChannel(_voiceChannelId);
		_voiceChannelId = -1;
		TraceLog(kTraceError, "SetExTransport failure");
		return -1;
	}

	StoreAudioStreamParam(paudioinfo);
	SetVoEMode(paudioinfo);
	ResetEmodel(paudioinfo->iPayLoadType);
	SendCNPacket();
	ResetRTT();
	ResetSinglepass();
	
	TraceLog(kTraceReport, "SetAudioChannel:pt=%d, RemoteAddr[%s:%d]", \
        paudioinfo->iPayLoadType, paudioinfo->cRemoteAudioIp, \
        paudioinfo->iRemoteAudioPort);

	return 0;
}

int Conductor::CreateChannel(void)
{
	int iRet = 0;
	char strTmp[256] = { 0 };
	
	CriticalSectionScoped lock(_critSectAudio);
	
	if(_voiceChannelId < 0)
	{
#if defined (__APPLE__)
    	AudioDeviceInit();
#endif
   
        if((_audioDeviceReady == eDEVICE_GET_FAIL)||(_audioDeviceReady == eDEVICE_INIT_MIC_FAIL))
        {
    	    _voeUpLinkSinglepassReason |= eME_REASON_AUDIO_DEVICE_INIT_FAIL;

			_snprintf(strTmp, 256, "audio device fail:audio device initialization fail");
			evt_appand(eME_LOG_TRACE_EV, eME_REASON_LOG_LEVEL_ERR, strTmp, NULL, 0);
        }
        
        _voiceChannelId = _voeBase->CreateChannel(); //yangjj modify
    	if(_voiceChannelId == -1)
    	{
    		TraceLog(kTraceError, "CreateChannel failure(Error = %d)", _voeBase->LastError());
    		return -1;
    	}

         bool ipv6Enabled = _stCTRLConfig.ucIPv6Enable == 0 ? false : true;
        _intVoiceTransportPtr = new VoiceChannelTransport(_voeNetwork, 
                                            _voiceChannelId, ipv6Enabled);

#ifdef EXTERNAL_ENCRYPT
        if ( _stCTRLConfig.ucRtpEncEnable )
        {
		    _voeEncrypt->RegisterExternalEncryption(_voiceChannelId, *this);
		}
#endif

#ifdef EXTERNAL_MEDIA_PROCESS
        if ( _stCTRLConfig.ucExtMediaProcEn )
        {
            _extMediaProc->SetAudioChannelId(_voiceChannelId);
            _voeExtMedia->RegisterExternalMediaProcessing(_voiceChannelId, kRecordingPerChannel, *_extMediaProc);
        }
#endif // EXTERNAL_MEDIA_PROCESS

        /* add by Vinton.Liu on 2016-03-29 for support external audio device push pcm to and pull pcm from engine begin */
        if ( _stCTRLConfig.ucExtAudioTransEn && _voeAudioTransport )
        {
            _voeBase->RegisterExternalAudioTransport(*_voeAudioTransport);
        }
        /* Vinton.Liu add end */

        /* Register rtcp report observer for voice codec rate auto adapter */
        _voeRtp_Rtcp->RegisterRtcpReportObserver(_voiceChannelId, *this);

        UpdateState(eME_Running);
        TraceLog(kTraceReport, "CreateChannel(%d) Successed!!", _voiceChannelId);
	}
	else
	{
	    TraceLog(kTraceError, "CreateChannel error, pre channel haven't deleted.");
	}
	
	return 0;
}

int Conductor::DeleteChannel(void)
{
	CriticalSectionScoped lock(_critSectAudio);
	UpdateState(eME_idle);
	
	if(SoundIsStarted())
	{
		StopRecord();

        /* DeRegister rtcp report observer for voice codec rate auto adapter */
        _voeRtp_Rtcp->DeRegisterRtcpReportObserver(_voiceChannelId);

        /* add by Vinton.Liu on 2016-03-29 for support external audio device push pcm to and pull pcm from engine begin */
        _voeBase->DeRegisterExternalAudioTransport();
        /* Vinton.Liu add end */

        delete _intVoiceTransportPtr;
        _intVoiceTransportPtr = NULL;
        
		_voeBase->DeleteChannel(_voiceChannelId);
		_voiceChannelId = -1;
		ResetAudioStreamParam();
        UpdateIceMode(eME_ICE_RTPP_MD);
        ResetSinglepass();

		// Reset codec rate and payload length changed history
		_usLastRate = 0;            //last stream rate contain codec and packet
        _usLastCodeRateIdx = 0;     //last codec rate index
        _usLastPktLenIdx = 0;       //last packet length index

        //reset env variable
        memset(&_stEnvConfig, 0 ,sizeof(ME_ENV_cfg_t));

        // Avoid memory leak if app haven't stop file
        _playFileDirect = -1;     //reset play file mode	
#ifdef EXTERNAL_PLAYFILE
        if ( _extPlayFile )
        {
    	    _extPlayFile->ResetPlayFile();
        }
#endif
	}

	return 0;
}

int Conductor::StartReceive(void)
{
	CriticalSectionScoped lock(_critSectAudio);
	if (_voiceChannelId < 0)
	{
		TraceLog(kTraceError, "StartReceive _voiceChannelId < 0!");
		return -1;
	}

	if (0 != _voeBase->StartReceive(_voiceChannelId))
	{
	    _voeDnLinkSinglepassReason |= eME_REASON_START_RECEIVE_FAIL;
		TraceLog(kTraceError, "webrtc StartReceive Failed!");
		return -1;
	}   

	return 0;
}

int Conductor::StopReceive(void)
{
	CriticalSectionScoped lock(_critSectAudio);
	
	if (_voiceChannelId < 0)
	{
		TraceLog(kTraceError, "StopReceive _voiceChannelId < 0!");
		return -1;
	}

	if (0!= _voeBase->StopReceive(_voiceChannelId))
	{
		TraceLog(kTraceError, "webrtc StopReceive  Failed!");
		return -1;
	}
	
	return 0;
}

int Conductor::StartPlayOut(void)
{
	CriticalSectionScoped lock(_critSectAudio);
	
	if (_voiceChannelId < 0)
	{
		TraceLog(kTraceError, "StartPlayOut _voiceChannelId < 0!");
		return -1;
	}

	//_voeBase->SetNetEQPlayoutMode(_voiceChannelId, kNetEqDefault);//add by wuzhaoyang20140918
	
	if (0 != _voeBase->StartPlayout(_voiceChannelId))
	{
		TraceLog(kTraceError, "webrtc StartPlayout Failed!");
		return -1;
	}

	return 0;
}

int Conductor::StopPlayOut(void)
{
	CriticalSectionScoped lock(_critSectAudio);
	
	if (_voiceChannelId < 0)
	{
		TraceLog(kTraceError, "StopPlayOut _voiceChannelId < 0!");
		return -1;
	}

	if (0 != _voeBase->StopPlayout(_voiceChannelId))
	{
		TraceLog(kTraceError, "webrtc StopPlayout Failed!");
		return -1;
	}
	
	return 0;
}

//add by wuzhaoyang20140607
int Conductor::EnableFec(char enable)
{
	CriticalSectionScoped lock(_critSectAudio);

	if (_voiceChannelId < 0)
	{
		TraceLog(kTraceError, "StartSend _voiceChannelId < 0!");
		return -1;
	}

	if(enable)
	{
		_consultFecEn = true;
	}
	else
	{
		_consultFecEn = false;
	}
	
	if (_stCTRLConfig.ucFecEnable && enable == 1)
	{
		//if (0 != _voeRtp_Rtcp->SetFECStatus(_voiceChannelId, true, -1))
		if (0 != _voeRtp_Rtcp->SetREDStatus(_voiceChannelId, true, 
                                            AUDIO_RED_PAYLOAD_TYPE_DEFAULT))
		{
			TraceLog(kTraceError, "webrtc Open the FEC Failed!");
			return -1;
		}
		TraceLog(kTraceReport, "The Audio FEC Function Has Been Opened!!!!!!");
	}
	else
	{
		//if (0 != _voeRtp_Rtcp->SetFECStatus(_voiceChannelId, false, -1))
		if (0 != _voeRtp_Rtcp->SetREDStatus(_voiceChannelId, false, 
                                            AUDIO_RED_PAYLOAD_TYPE_DEFAULT))
		{
			TraceLog(kTraceError, "webrtc Close the FEC Failed!");
			return -1;
		}
		TraceLog(kTraceReport, "The Audio FEC Function Has Been Closed!!!!!!");
	}

	return 0;
}

int Conductor::GetFecStatus()
{
	CriticalSectionScoped lock(_critSectAudio);
	int ptvalue;
	bool fec_enable;

	if (_voiceChannelId < 0)
	{
		TraceLog(kTraceError, "StartSend _voiceChannelId < 0!");
		return -1;
	}
	
	//if (0 != _voeRtp_Rtcp->GetFECStatus(_voiceChannelId, fec_enable, ptvalue))
	if (0 != _voeRtp_Rtcp->GetREDStatus(_voiceChannelId, fec_enable, ptvalue))
	{
		TraceLog(kTraceError, "webrtc G the FEC Failed!");
		return -1;
	}
	
	return fec_enable;
}

int Conductor::SetAudioFec(unsigned short nMulriple, unsigned short  nOffset)//yangjj add
{
	CriticalSectionScoped lock(_critSectAudio);

	if (_voiceChannelId < 0)
	{
		TraceLog(kTraceError, "SetAudioFec _voiceChannelId < 0!");
		return -1;
	}



#ifdef _DEBUG
	_voeRtp_Rtcp->SetFECStatus(_voiceChannelId, true,
	                            AUDIO_RED_PAYLOAD_TYPE_DEFAULT);
#endif

	return _voeRtp_Rtcp->SetAudioFec(_voiceChannelId, nMulriple, nOffset);
}

int Conductor::SetNetAnalyze(bool isAnalyze)
{
	CriticalSectionScoped lock(_critSectAudio);

	if (_voiceChannelId < 0)
	{
		TraceLog(kTraceError, "SetNetAnalyze _voiceChannelId < 0!");
		return -1;
	}
	else
	{
		_voeRtp_Rtcp->SetNetAnalyze(_voiceChannelId, isAnalyze);
		return 0;
	}
	return -1;
}

int Conductor::StartSend(void)
{
	CriticalSectionScoped lock(_critSectAudio);
	
	if (_voiceChannelId < 0)
	{
		TraceLog(kTraceError, "Conductor::StartSend _voiceChannelId < 0!");
		return -1;
	}

	if (0 != _voeBase->StartSend(_voiceChannelId))
	{
	    _voeUpLinkSinglepassReason |= eME_REASON_START_SEND_FAIL;
		TraceLog(kTraceError, "Conductor::StartSend Failed!");
		return -1;
	}

	return 0;
}

int Conductor::StopSend(void)
{
	CriticalSectionScoped lock(_critSectAudio);
	
	if (_voiceChannelId < 0)
	{
		TraceLog(kTraceError, "Conductor::StopSend _voiceChannelId < 0!");
		return -1;
	}

	if (0 != _voeBase->StopSend(_voiceChannelId))
	{
		TraceLog(kTraceError, "Conductor::StopSend Failed!");
		return -1;
	}

	return 0;
}

// DTMF
#define TEV_DTMF_0			(0)
#define TEV_DTMF_1			(1)
#define TEV_DTMF_2			(2)
#define TEV_DTMF_3			(3)
#define TEV_DTMF_4			(4)
#define TEV_DTMF_5			(5)
#define TEV_DTMF_6			(6)
#define TEV_DTMF_7			(7)
#define TEV_DTMF_8			(8)
#define TEV_DTMF_9			(9)
#define TEV_DTMF_STAR		(10)
#define TEV_DTMF_POUND		(11)
#define TEV_DTMF_A			(12)
#define TEV_DTMF_B			(13)
#define TEV_DTMF_C			(14)
#define TEV_DTMF_D			(15)
#define TEV_FLASH			(16)

// DTMF
int Conductor::SendDTMF(char chDtmf)
{
	int iTevType;
	
	if(_voiceChannelId < 0)
		return -1;

	/* create the first telephony event packet */
	switch (chDtmf)
	{
	case '1':
		iTevType=TEV_DTMF_1;
		break;
	case '2':
		iTevType=TEV_DTMF_2;
		break;
	case '3':
		iTevType=TEV_DTMF_3;
		break;
	case '4':
		iTevType=TEV_DTMF_4;
		break;
	case '5':
		iTevType=TEV_DTMF_5;
		break;
	case '6':
		iTevType=TEV_DTMF_6;
		break;
	case '7':
		iTevType=TEV_DTMF_7;
		break;
	case '8':
		iTevType=TEV_DTMF_8;
		break;
	case '9':
		iTevType=TEV_DTMF_9;
		break;
	case '*':
		iTevType=TEV_DTMF_STAR;
		break;
	case '0':
		iTevType=TEV_DTMF_0;
		break;
	case '#':
		iTevType=TEV_DTMF_POUND;
		break;
	case 'A':
	case 'a':
		iTevType=TEV_DTMF_A;
		break;	
	case 'B':
	case 'b':
		iTevType=TEV_DTMF_B;
		break;
	case 'C':
	case 'c':
		iTevType=TEV_DTMF_C;
		break;
	case 'D':
	case 'd':
		iTevType=TEV_DTMF_D;
		break;
	case '!':
		iTevType=TEV_FLASH;
		break;
	default:
		return -1;
	}

	/*use out of band mode, modified by Rambo Fu 2013-01-23*/
	int iRet = _voeDtmf->SendTelephoneEvent(_voiceChannelId, iTevType);
	
	return iRet;
}

/* Add by Vinton.Liu on 20151123 for play dtmf tone to local */
int Conductor::PlayDTMF(char chDtmf)
{
    int iTevType;
	
	if(_voiceChannelId < 0)
		return -1;

	/* create the first telephony event packet */
	switch (chDtmf)
	{
	case '1':
		iTevType = TEV_DTMF_1;
		break;
	case '2':
		iTevType = TEV_DTMF_2;
		break;
	case '3':
		iTevType = TEV_DTMF_3;
		break;
	case '4':
		iTevType = TEV_DTMF_4;
		break;
	case '5':
		iTevType = TEV_DTMF_5;
		break;
	case '6':
		iTevType = TEV_DTMF_6;
		break;
	case '7':
		iTevType = TEV_DTMF_7;
		break;
	case '8':
		iTevType = TEV_DTMF_8;
		break;
	case '9':
		iTevType = TEV_DTMF_9;
		break;
	case '*':
		iTevType = TEV_DTMF_STAR;
		break;
	case '0':
		iTevType = TEV_DTMF_0;
		break;
	case '#':
		iTevType = TEV_DTMF_POUND;
		break;
	case 'A':
	case 'a':
		iTevType = TEV_DTMF_A;
		break;	
	case 'B':
	case 'b':
		iTevType = TEV_DTMF_B;
		break;
	case 'C':
	case 'c':
		iTevType = TEV_DTMF_C;
		break;
	case 'D':
	case 'd':
		iTevType = TEV_DTMF_D;
		break;
	case '!':
		iTevType = TEV_FLASH;
		break;
	default:
		return -1;
	}

	int iRet = _voeDtmf->PlayDtmfTone(iTevType, 160, 10);
	
	return iRet;
}
/* Vinton.Liu add end */

int Conductor::StartRecord(const media_file_record_pm_t* pm)
{
	int iRet = 0;
	char  tmp1[50] = "Mic.pcm";
	char  tmp2[50] = "Speaker.pcm";
	char  tmp_path[RECORDE_FILE_PATH_LEN] = {0};
	char *p = NULL;
	char *s = NULL;
	int len = 0;
    CodecInst dummyCodec = { 100, "L16", 8000, 160, 1, 8000 }; //format of recording
        
	if ( NULL == pm || !strlen(pm->filepath) 
	    || (strlen(pm->filepath) >= RECORDE_FILE_PATH_LEN) )
	{
		return -1;
	}

    //to save record mode
    _recordMode = pm->iMode;
    _recordFormat = pm->iFileFormat;
    
	s = (char *)pm->filepath;
        
	len = strlen(pm->filepath);
	p = s+len;
	for ( ; s != p && (*p != '\\' && *p != '/'); *p--)
	    ;

	len = p-s;
	if ( len )
	{
		strncpy(tmp_path, s, len);
		tmp_path[len] = '\0';
	}
	else
	{
		tmp_path[0] = '.';
		tmp_path[1] = '\0';
	}
	
    memset(&_recordMic, 0x00, sizeof(_recordMic));
    memset(&_recordSpeaker, 0x00, sizeof(_recordSpeaker));
    memset(&_recordMixFile, 0x00, sizeof(_recordMixFile));
    switch (_recordMode)
    {
    case eME_RECORD_ALL:
#ifdef _WIN32
    	_snprintf(_recordMic,  RECORDE_FILE_PATH_LEN, "%s\\%s", tmp_path, tmp1);
    	_snprintf(_recordSpeaker,  RECORDE_FILE_PATH_LEN, "%s\\%s", tmp_path, tmp2);
#elif defined(ANDROID)
        _snprintf(_recordMic,  RECORDE_FILE_PATH_LEN, "%s", "/mnt/sdcard/mic.pcm");
        _snprintf(_recordSpeaker,  RECORDE_FILE_PATH_LEN, "%s", "/mnt/sdcard/speaker.pcm");
#else //ios
        _snprintf(_recordMic,  RECORDE_FILE_PATH_LEN, "%s/%s", tmp_path, tmp1);
        _snprintf(_recordSpeaker,  RECORDE_FILE_PATH_LEN, "%s/%s", tmp_path, tmp2);
#endif
        _snprintf(_recordMixFile, RECORDE_FILE_PATH_LEN, "%s", pm->filepath);
        
        iRet = _voeFile->StartRecordingMicrophone((char *)_recordMic, NULL);
    	if (-1 == iRet)
    	{
    		TraceLog(kTraceError, "StartRecordingMicrophone false!  %s", _recordMic);
    		return iRet;
    	}

        iRet = _voeFile->StartRecordingPlayout(_voiceChannelId, (char *)_recordSpeaker, NULL);
    	if (-1 == iRet)
    	{
    		TraceLog(kTraceError, "StartRecordingPlayout false!");
    		return iRet;
    	}

    	break;
    case eME_RECORD_MIC:
    	_snprintf(_recordMic, RECORDE_FILE_PATH_LEN, "%s", pm->filepath);
       
        iRet = _voeFile->StartRecordingMicrophone((char *)_recordMic, &dummyCodec);
    	if (-1 == iRet)
    	{
    		TraceLog(kTraceError, "StartRecordingMicrophone false!  %s", _recordMic);
    		return iRet;
    	}

    	break;
    case eME_RECORD_SPEAKER:
    	_snprintf(_recordSpeaker, RECORDE_FILE_PATH_LEN, "%s", pm->filepath);
        
        iRet = _voeFile->StartRecordingPlayout(_voiceChannelId, (char *)_recordSpeaker, &dummyCodec);
    	if (-1 == iRet)
    	{
    		TraceLog(kTraceError, "StartRecordingPlayout false!");
    		return iRet;
    	}

    	break;
    default :
    	return -2;
    	break;
    }
	
	return iRet;
}

int Conductor::StopRecord(void)
{
    int iRet = 0;

    /* return if not recording */
    if ( _recordMode < 0 )
    {
        return iRet;
    }

    switch (_recordMode)
    {
    case eME_RECORD_ALL:
    	iRet = _voeFile->StopRecordingMicrophone();
    	if (-1 == iRet)
    	{
    		TraceLog(kTraceError, "Conductor::StopRecord StopRecordingMicrophone failed.");
    	}

    	iRet = _voeFile->StopRecordingPlayout(_voiceChannelId);
    	if (-1 == iRet)
    	{
    		TraceLog(kTraceError, "Conductor::StopRecord StopRecordingPlayout failed.");
    	}

    	// PCM to wav,mic add speaker to callfile
    	if ( _recordFormat == kFileFormatWavFile )
    	{
            iRet = _voeFile->MixPCMToWAV(_recordMic, _recordSpeaker, _recordMixFile);
        	if ( iRet < 0)
        	{
        		TraceLog(kTraceError, "Conductor::StopRecord MixPCMToWAV failed.");
        	}
    	}
		remove(_recordMic);
		remove(_recordSpeaker);
    	break;
    case eME_RECORD_MIC:
    	iRet = _voeFile->StopRecordingMicrophone();
    	if ( -1 == iRet )
    	{
    		TraceLog(kTraceError, "Conductor::StopRecord StopRecordingMicrophone failed.");
    	}
    	break;
    case eME_RECORD_SPEAKER:
    	iRet = _voeFile->StopRecordingPlayout(_voiceChannelId);
    	if ( -1 == iRet )
    	{
    		TraceLog(kTraceError,"Conductor::StopRecord StopRecordingPlayout failed.");
    	}
    	break;
    default :
    	break;
    }    

    _recordMode = -1;   //reset recording mode
	return iRet;
}

#ifdef EXTERNAL_PLAYFILE
int Conductor::ExternalPlayFile(int mode, const media_file_play_pm_t* pm)
{
	int iRet = 0;

	if (NULL == pm || _extPlayFile == NULL)
	{
		TraceLog(kTraceError,"External Play file false, NULL == pm || _extPlayFile == NULL!");
		return -1;
	}

	if (mode != kME_FILE_HANDLE && mode != kME_FILE_STREAM)
	{
		TraceLog(kTraceError,"External Play file false, mode != kME_FILE_HANDLE && mode != kME_FILE_STREAM!");
		return -1;
	}

	playfile_pm_t filepm;
	memset(&filepm, 0, sizeof(filepm));

	filepm.loop = (pm->iLoop?true:false);
	filepm.mode = mode;
	
	if (mode == kME_FILE_HANDLE)
	{
		filepm.fd = (FILE*)pm->filehandle.fd;
	}
	else if (mode == kME_FILE_STREAM)
	{
		filepm.fstream.size = pm->filestream.size;
		filepm.fstream.data = (char*)pm->filestream.data;
	}

	_extPlayFile->SetPlayFile(&filepm);
    
    _playFileDirect = pm->iDirect;
	
    if (0 == pm->iDirect)
    {
        iRet = _voeFile->StartPlayingFileLocally(_voiceChannelId, _extPlayFile, (FileFormats)pm->iFileFormat);
    	if (-1 == iRet)
    	{
    		TraceLog(kTraceError,"External Play file StartPlayingFileLocally false!");
    	}
    }
    else
    {
        iRet = _voeFile->StartPlayingFileAsMicrophone(_voiceChannelId, _extPlayFile, false,(FileFormats)pm->iFileFormat);
    	if (-1 == iRet)
    	{
    		TraceLog(kTraceError,"External Play file StartPlayingFileAsMicrophone false!");
    	}
    }

	TraceLog(kTraceReport, "External Play file ok!");
	
	return iRet;
}
#endif 	


int Conductor::PlayFile(const media_file_play_pm_t* pm)
{
	int iRet = 0;

	if (NULL == pm)
	{
		return -1;
	}
    
    _playFileDirect = pm->iDirect;
    if (0 == pm->iDirect)
    {
        iRet = _voeFile->StartPlayingFileLocally(_voiceChannelId, 
                                                (char *)pm->filepath.filename, 
                                                (pm->iLoop != 0),
                                                (FileFormats)pm->iFileFormat);
    	if (-1 == iRet)
    	{
    		TraceLog(kTraceError,"Play file false!");
    	}
    }
    else
    {
        iRet = _voeFile->StartPlayingFileAsMicrophone(_voiceChannelId, 
                                                (char *)pm->filepath.filename, 
                                                (pm->iLoop != 0),
                                                false,   //mixWithMicrophone
                                                (FileFormats)pm->iFileFormat);
    	if (-1 == iRet)
    	{
    		TraceLog(kTraceError,"Play file false!");
    	}
    }
	return iRet;
}

int Conductor::StopFile()
{
	int iRet = 0;

    if (_playFileDirect != -1)
    {
        if (0 == _playFileDirect)
        {
            int iRet = _voeFile->StopPlayingFileLocally(_voiceChannelId);
        	  
        	if (-1 == iRet)
        	{
        		TraceLog(kTraceError, "Stop Play file false!");
        	}

        }
        else
        {
            int iRet = _voeFile->StopPlayingFileAsMicrophone(_voiceChannelId);
        	  
        	if (-1 == iRet)
        	{
        		TraceLog(kTraceError, "Stop Play file false!");
        	}
        }
    }

    _playFileDirect = -1;     //reset play file mode
 #ifdef EXTERNAL_PLAYFILE
	_extPlayFile->ResetPlayFile();
 #endif
	return iRet;
}

#ifdef EXTERNAL_AUDIO_DEVICE
int Conductor::exl_send_pcm(ME_exl_pcm_input_itf_t* stInput)
{
	if(_extAudioDevice)
	{
        _extAudioDevice->write(stInput);
	}    
	return 0;
}

int Conductor::exl_rcv_pcm(ME_exl_pcm_output_itf_t* stOutput)
{
	if(_extAudioDevice)
	{
        _extAudioDevice->read(stOutput);
	}    
	return 0;
}
#endif

#ifdef AUTO_TEST_SWITCH
int Conductor::SetTestMode(int iMode)
{
    //return (_voeBase->SetTestMode(_voiceChannelId, iMode));
    return 0;
}

// Store RTP and RTCP packets and dump to file (compatible with rtpplay)
int Conductor::StartRTPDump(const char fileNameUTF8[1024], int direction)
{
    return (_voeRtp_Rtcp->StartRTPDump(_voiceChannelId, fileNameUTF8, (RTPDirections)direction));
}

int Conductor::StopRTPDump(int direction)
{
    return (_voeRtp_Rtcp->StopRTPDump(_voiceChannelId, (RTPDirections)direction));
}

int Conductor::getLocalIP(int& port, char ipAddr[64])
{
    int RTCPport;
    
    //return (_voeBase->GetLocalReceiver(_voiceChannelId, port, RTCPport, ipAddr));
    return 0;
}

int Conductor::getDestIP(int& port, char ipAddr[64])
{
    int RTCPport;
    int sourcePort;
    
    //return (_voeBase->GetSendDestination(_voiceChannelId, port, ipAddr, sourcePort, RTCPport));
    return 0;
}
#endif

int Conductor::SetSpeakerVolume(unsigned int iVolumeValue)
{
    int nRet = _voeVolume->SetSpeakerVolume(iVolumeValue);
	
	if (-1 == nRet)
	{
		TraceLog(kTraceError,"webrtc SetMicVolume failed");
	}
	return nRet;
}

int Conductor::SetMicVolume(unsigned int iVolumeValue)
{
    int nRet = _voeVolume->SetMicVolume(iVolumeValue);
	
	if (-1 == nRet)
	{
		TraceLog(kTraceError, "webrtc SetMicVolume failed");
	}
	return nRet;
}

// Speaker control
int Conductor::SetLoudSpeakerStatus(bool enable)
{
    int nRet = -1;
    
#ifndef _WIN32
    nRet = _voeHardware->SetLoudspeakerStatus(enable);

    //Set speaker device status
    _stEnvConfig.status = enable;
    AecmModes enumSpeakerMode = enable ? kAecmLoudSpeakerphone : kAecmEarpiece;
	if (_voeVqe->SetAecmMode(enumSpeakerMode, true) < 0)
	{
        TraceLog(kTraceError, "set SetAecmMode fail, enumSpeakerMode=%d", \
                enumSpeakerMode);
	}	
#endif    
	return nRet;
}

int Conductor::GetLoudSpeakerStatus()
{
    int nRet = -1;
    bool bMute = false;

#ifndef _WIN32
    if ( _voeHardware ) 
    {
        nRet = _voeHardware->GetLoudspeakerStatus(bMute);
    }

    if (-1 == nRet)
	{
		TraceLog(kTraceError,"webrtc GetLoudSpeakerStatus failed");
	}
	return (int)bMute;
#else
    return nRet;
#endif
}

// Microphone mute control
int Conductor::SetMicrophoneMute(bool enable)
{
    int nRet = -1;
    
    if ( _voeVolume )
    {
        if ((GetCurState() == eME_Running) || (GetCurState() == eME_Connected))
        {
            nRet = _voeVolume->SetInputMute(_voiceChannelId, enable);
        }
    }
    
	if ( -1 == nRet )
	{
		TraceLog(kTraceError,"webrtc SetMicVolume failed");
	}

	return nRet;
}

/* add by Vinton.Liu on 20140922 for get microphone status */
int Conductor::GetMicrophoneMute ()
{
    bool mute = false;
    int nRet = -1;
    
    if ( _voeVolume )
    {
        if ((GetCurState() == eME_Running) || (GetCurState() == eME_Connected))
        {
            nRet = _voeVolume->GetInputMute (_voiceChannelId, mute);
        }
    }

    if (-1 == nRet)
	{
		TraceLog(kTraceError,"webrtc GetMicVolume failed");
	}
    return (int)mute;
}
/* Vinton.Liu add end */

/* add by Vinton.Liu on 2016-03-16 for Detect recording devices status begin */
int Conductor::GetRecordingDeviceStatus()
{
    bool isAvailable = false;
    
    if (_voeHardware)
	{
		_voeHardware->GetRecordingDeviceStatus(isAvailable);		
	}
	TraceLog(kTraceReport,"Conductor::GetRecordingDeviceStatus() isAvailable(%d)", isAvailable);
	return (int)isAvailable;
}
/* Vinton.Liu add end */

int Conductor::SetAudioDevice(const audio_device_pm_t* pm)
{
	int nRet = -1;

	if (NULL == pm)
		return -1;
	
	nRet = _voeHardware->SetRecordingDevice(pm->wav_dev_in);
	if (-1 == nRet)
		TraceLog(kTraceError,"webrtc SetRecordingDevice failed");

	nRet = _voeHardware->SetPlayoutDevice(pm->wav_dev_out);
	if (-1 == nRet)
		TraceLog(kTraceError,"webrtc SetPlayoutDevice failed");
	
	return nRet;
}

int Conductor::SetLogFilePath(const char* logfile)
{
    if (logfile == NULL && _voeEngine == NULL)
        return -1;
	
	_voeEngine->SetTraceFile(logfile, true);
    
    return 0;
}

int Conductor::SetLogLevel(const TraceLevel level)
{
	if (_voeEngine == NULL)
		return -1;

	_voeEngine->SetTraceFilter(level);

	return 0;
}

Conductor* Conductor::GetInstance(int iDataSource)
{
	if(m_pInstance == NULL)
	{
		m_pInstance = new Conductor;
		if(-1 == m_pInstance->LoadAPIs(iDataSource))
		{
			delete m_pInstance;
			m_pInstance = NULL;
			return NULL;
		}

		if(-1 == m_pInstance->AuthAndInit())
		{
			delete m_pInstance;
			m_pInstance = NULL;
			return NULL;
		}

		m_pInstance->UpdateState(eME_init);
	}

	m_pInstance->AddRef(COND_LOAD_ADD_ONE);
	
	return m_pInstance;
}

void Conductor::FreeInstance(void)
{
	if (m_pInstance != NULL)
	{
		int loadcnt = 0;
		m_pInstance->AddRef(COND_LOAD_DEL_ONE);
		loadcnt = m_pInstance->GetCurLoadCnt();
		
		if(loadcnt == 0)
		{
			m_pInstance->StopThread();//stop thread to detect event;
			m_pInstance->DeleteChannel();

			m_pInstance->AudioDeviceTerminate();
			m_pInstance->releaseWebrtcSubApi();
			m_pInstance->UpdateState(eME_done);
			delete m_pInstance;
			m_pInstance = NULL;
			g_pVoiceEngine = NULL;

#ifdef EMODEL_ENABLED
			emd_destroy();
#endif

#ifdef ME_USE_EVENT_LIST
			/* Added by gonghuojin, 2013/9/3  destroy event list*/
			evt_destroy();
#endif

#if defined(ANDROID)        
            media_jni_env_pm_t env_pm;
            env_pm.context = NULL;
            env_pm.env = NULL;
            env_pm.jvm = NULL;
			setAndroidObj(&env_pm);
#endif 
		}
	}
}

#ifdef _WIN32
int Conductor::MicrophoneBoostIsAvailable(bool* available)
{
//	return(_voeHardware->MicrophoneBoostIsAvailable(available));
	return 0;
}

int Conductor::SetMicrophoneBoost(bool enable)
{
	//return(_voeHardware->SetMicrophoneBoost(enable));
	return 0;
}

int Conductor::MicrophoneBoost(bool* enable)
{
	//return(_voeHardware->MicrophoneBoost(enable));
	return 0;
}

// Volume control _voeBased on the Windows Wave API (Windows only)
int Conductor::SetWaveOutVolume(uint16_t volumeLeft, uint16_t volumeRight)
{
	//return(_voeHardware->SetWaveOutVolume(volumeLeft, volumeRight));
	return 0;
}

int Conductor::WaveOutVolume(uint16_t* volumeLeft, uint16_t* volumeRight) 
{
	//return(_voeHardware->WaveOutVolume(volumeLeft, volumeRight));
	return 0;
}

#endif


/*
** release sub api resource called when delete voice engine
*/
void Conductor::releaseWebrtcSubApi()
{
	if(_voeBase)
	{
		_voeBase->Release();
		_voeBase = NULL;
	}
	if(_voeVqe)
	{
		_voeVqe->Release();
		_voeVqe = NULL;
	}
	if(_voeCodec)
	{
		_voeCodec->Release();
		_voeCodec = NULL;
	}
	if(_voeHardware)
	{
		_voeHardware->Release();
		_voeHardware = NULL;
	}
	if(_voeDtmf)
	{
		_voeDtmf->Release();
		_voeDtmf = NULL;
	}
	if(_voeVolume)
	{
		_voeVolume->Release();
		_voeVolume = NULL;
	}
	if(_voeRtp_Rtcp)
	{
		_voeRtp_Rtcp->Release();
		_voeRtp_Rtcp = NULL;
	}

	if (_voeFile)
	{
		_voeFile->Release();
		_voeFile = NULL;
	}

	if(_voeNetwork)
	{
		_voeNetwork->Release();
		_voeNetwork = NULL;
	}

	if (_voeNeteqStats)
	{
		_voeNeteqStats->Release();
		_voeNeteqStats = NULL;
	}

#ifdef EXTERNAL_ENCRYPT
	if(_voeEncrypt)
	{
		_voeEncrypt->Release();
		_voeEncrypt = NULL;
	}
#endif

#ifdef EXTERNAL_MEDIA_PROCESS
	if ( _voeExtMedia )
    {
        _voeExtMedia->Release();
        _voeExtMedia = NULL;
    }
	
	if ( _extMediaProc )
    {
        delete _extMediaProc;
        _extMediaProc = NULL;
    }
#endif // EXTERNAL_MEDIA_PROCESS

    /* add by Vinton.Liu on 2016-03-29 for support external audio device push pcm to and pull pcm from engine begin */
    if ( _voeAudioTransport )
    {
        delete _voeAudioTransport;
        _voeAudioTransport = NULL;
    }
    /* Vinton.Liu add end */

#ifdef EXTERNAL_AUDIO_DEVICE
	if (_extAudioDevice)
	{
		ExlAudioDeviceModuleImpl::Destroy(_extAudioDevice);
		_extAudioDevice = NULL;
	}
#endif

#ifdef EXTERNAL_TRANSPORT
	if (_extVoiceTransportPtr)
	{
		delete _extVoiceTransportPtr;
		_extVoiceTransportPtr = NULL;
	}
#endif

#ifdef EXTERNAL_PLAYFILE
	if (_extPlayFile)
	{
		delete _extPlayFile;
		_extPlayFile = NULL;
	}
#endif
	
	if(_voeEngine)
	{
		VoiceEngine::Delete(_voeEngine);
		_voeEngine = NULL;
	}
}

/*
** webrtc support the codec?
*/
bool Conductor::webrtcCodecSupport(const char* mime)
{
	if(_voeCodec)
	{
		CodecInst inst;
		int nNumCodec = _voeCodec->NumOfCodecs();
		for (int k = 0; k < nNumCodec; ++k)
		{
			_voeCodec->GetCodec(k, inst);
			if(strcasecmp(inst.plname,mime) == 0)
				return true;
		}
	}



    
	return false;
}

bool Conductor::webrtcCodecSupport(int payload)
{
	if(_voeCodec)
	{
		CodecInst inst;
		int nNumCodec = _voeCodec->NumOfCodecs();
		for (int k = 0; k < nNumCodec; ++k)
		{
			_voeCodec->GetCodec(k, inst);
			if( inst.pltype == payload)
				return true;
		}
	}
    
	return false;
}

int Conductor::getCodecs(ME_codec_list_t* codecs_list)
{
	int total = 0;
	
    if(codecs_list == NULL)
   		return -1;

	memset(codecs_list, 0, ME_CODEC_LIST_T_SIZE);
  
    if (_voeCodec)
    {
        CodecInst inst;
		
		int cnt = _voeCodec->NumOfCodecs();
		
        for (int k = 0; k < cnt && k < MAX_CODEC_LIST_NUM; ++k)
        {
            _voeCodec->GetCodec(k, inst);
			codecs_list->codecs[k].pltype = inst.pltype;
			strcpy(codecs_list->codecs[k].plname, inst.plname);
			codecs_list->codecs[k].enabled = true;
			
            total++;
        }

		codecs_list->num = total;
		
        return total;
    }
	
    return 0;
}

void Conductor::SetCurState(int state)
{
	UpdateState((enum eME_state)state);

    //tell rtpreceiver that calling has connected. start singlepass timer.
    if (eME_Connected == (enum eME_state)state)
    {
        _voeNetwork->SetConnectStatus(_voiceChannelId, true);

		/*begin:add by gonghuojin for early media support 20131202*/		
		if (_voiceChannelId >= 0)
		{
			int iRet = 0;
        	iRet = _voeBase->StartReceive(_voiceChannelId);
        	if (0 != iRet)
        	{
        	    _voeDnLinkSinglepassReason |= eME_REASON_START_RECEIVE_FAIL;
        		TraceLog(kTraceError, "webrtc StartReceive Failed!");
        	}   

        	iRet = _voeBase->StartSend(_voiceChannelId);
        	if (0 != iRet)
        	{
        	    _voeUpLinkSinglepassReason |= eME_REASON_START_SEND_FAIL;
        		TraceLog(kTraceError, "webrtc StartSend Failed!");
        	}            
		}
		/* end:   Added by gonghuojin, 2013/12/2 */
    }
}

#ifdef ANDROID
/* pass the android context to webrtc for recording*/
void  Conductor::setAndroidObj(const media_jni_env_pm_t* pm_obj)
{
	if ( NULL == pm_obj )
		return;

#if 0
    VoiceEngine::SetAndroidObjects(pm_obj->jvm, pm_obj->env, pm_obj->context);
#else
    VoiceEngine::SetAndroidObjects(pm_obj->jvm, pm_obj->context);
#endif

    /* add by Vinton.Liu on 2016-03-08 for android thread jni begin */    
	ThreadWrapper::SetAndroidObjects(pm_obj->jvm, pm_obj->context);
    /* Vinton.Liu add end */
}

void Conductor::SetAndroidApi(int level)
{
    /* not used */
#if 0
	VoiceEngine::SetAndroidApiLevel(level);
#endif
}
#endif

//add by wuzhaoyang20151225 start
int Conductor::SetDirectCallFecStatus(char enable)
{
	CriticalSectionScoped lock(_critSectAudio);

	if (_voiceChannelId < 0)
	{
		TraceLog(kTraceError, "StartSend _voiceChannelId < 0!");
		return -1;
	}

	if (enable)
	{
		_consultRtppMultFec = true;
		TraceLog(kTraceReport, "The RTPP Support Two Redundancy!!!!!!");
	}
	else
	{
		_consultRtppMultFec = false;
		TraceLog(kTraceReport, "The RTPP Only Support One Redundancy!!!!!!");
	}	

	return 0;
}

void Conductor::SetDirectcallAudioFec(unsigned short lossrate)
{
	signed short diff = 0;                      //modified by king 20160120
	unsigned short temp_lossrate = lossrate * 10;
	unsigned short temp_max = 0;
	unsigned char nMulriple = 0;
	unsigned char nOffset = 0;

	if (_voeRtp_Rtcp == NULL || _voiceChannelId < 0)
	{
		return;
	}

	diff = temp_lossrate - _smoothLossRate;

	if (diff < 0)
	{
		diff = -((-diff) >> 5);
	}
	else
	{
		diff = (diff >> 5);
	}
	_smoothLossRate += diff;

	if (_smoothLossRate >= temp_lossrate)
	{
		temp_max = _smoothLossRate;
	}
	else
	{
		temp_max = temp_lossrate;
	}

	if (_voeRtp_Rtcp->GetAudioFec(_voiceChannelId, nMulriple, nOffset) != 0)
	{
		return;
	}

    if ( _consultRtppMultFec )
    {
    	if ((temp_max >= 100) && (nMulriple != 2))
    	{
    		_voeRtp_Rtcp->SetAudioFec(_voiceChannelId, 2, 1);
    	}
    	else if ((temp_max < 100) && (nMulriple != 1))
    	{
    		_voeRtp_Rtcp->SetAudioFec(_voiceChannelId, 1, 1);
    	}
    }
}
//add by wuzhaoyang20151225 end

void Conductor::updateRtpStatus(void)
{ 
	char strLog[256] = {0}; 
	unsigned short upLost = 0;
	unsigned short downLost = 0;
    unsigned int jitter = 0;
    bool NewUplinkSinglepassFlag = false;
    bool NewDnlinkSinglepassFlag = false;
	bool fecflag = false;
    int redPtype = 0;
    CallStatisticsReport statReport = { 0 };

	if (_voeRtp_Rtcp == NULL || _voiceChannelId < 0)
        return ;

    //Get Q value and net delay and jitter
    _voeRtp_Rtcp->GetQValue(_voiceChannelId, &_voiceNetInfo);
    
    if (_voeRtp_Rtcp->GetCallStatisticsReport(_voiceChannelId, statReport) < 0)
    {
        return;
    }
    upLost = (statReport.up_lost * 100) >> 8;
    downLost = (statReport.down_lost * 100) >> 8;

	//add by wuzhaoyang20151225 start
    if (_voeRtp_Rtcp->GetREDStatus(_voiceChannelId, fecflag, redPtype) < 0)
	{
		fecflag = false;
        redPtype = 0;
	}

	if ((_audioStreamInfo.isDirectCall == TRUE) && (fecflag == true))
	{ 
        SetDirectcallAudioFec(upLost);//temporary close         //modified by king 20160120
	}
	//add by wuzhaoyang20151225 end

	/*  Added by gonghuojin, 2013/12/5   PN:ice rtt  */
    UpdateRTT((int)statReport.rttMs);

	/*Upate emodel value*/
#ifdef EMODEL_ENABLED
	if (_stCTRLConfig.ucEmodelEnable)
	{
		static int cnt = 0;
        if (emd_is_started() || statReport.rttMs != 0 || cnt++ > 3)
		{
			int  gq = 0;
			int  ppl = 0;
			NetworkStatistics neteqstats;

			cnt = 0;
			if (_voeNeteqStats->GetNetworkStatistics(_voiceChannelId, neteqstats) == 0)
			{
				gq = (int)neteqstats.currentGilbertQ;//Q14
			}

			ppl = 100 * statReport.down_lost << 6;//Q14

			{
				CriticalSectionScoped lock(_critSect);
                emd_calculate_value(gq, statReport.rttMs, ppl);
				emd_update_state(eEMD_START_S);
			}
		}
	}
#endif

    /* check if have uplink single pass,
           should exclude the packets is 0 at the beginning time to send */
    if ( statReport.packets_sent  == _voePktSent )
    {
        if (_voeUpLinkSinglepassReason & eME_REASON_AUDIO_DEVICE_INIT_FAIL)
        {
            _voeUpLinkSinglepassReason ^= eME_REASON_AUDIO_DEVICE_INIT_FAIL;
        	NewUplinkSinglepassFlag = true;
        	
            evt_appand(eME_RTP_UPSINGLEPASS_EV, \
        	    eME_REASON_AUDIO_DEVICE_INIT_FAIL, \
        	    "uplink singlepass:maybe voice mic initialization failed.", \
        	    NULL, 0);    	
        }
        else if (_voeUpLinkSinglepassReason & eME_REASON_START_SEND_FAIL)
        {
            // This error couldn't not be restore except redial
            //_voeUpLinkSinglepassReason ^= eME_REASON_START_SEND_FAIL;
        	NewUplinkSinglepassFlag = true;
        	
        	evt_appand(eME_RTP_UPSINGLEPASS_EV, eME_REASON_START_SEND_FAIL, \
        	    "uplink singlepass:maybe voice start send failed.", NULL, 0);
        }
        else if (statReport.packets_sent > 0)
        {
            NewUplinkSinglepassFlag = true;
            
        	evt_appand(eME_RTP_UPSINGLEPASS_EV, eME_REASON_NETWORK_PROBLEM, \
        	    "uplink singlepass:maybe network problem.", NULL, 0);
        }
    
    }
    
    if ( _voeUplinkSinglepassFlag && !NewUplinkSinglepassFlag )
    {
        evt_appand(eME_RTP_UPSINGLEPASS_EV, eME_REASON_RTP_RESTORE_OK, \
            "uplink singlepass: restore ok.", NULL, 0);
    }
	_voePktSent = statReport.packets_sent;
    _voeUplinkSinglepassFlag = NewUplinkSinglepassFlag;    

    /* check if have downlink single pass,
            should exclude the packets is 0 at the beginning time to receive */
    if ( statReport.packets_received == _voePktReceived )
    {
        if (_voeDnLinkSinglepassReason & eME_REASON_SET_LOCAL_RECEIVER_FAIL)
        {
            _voeDnLinkSinglepassReason ^= eME_REASON_SET_LOCAL_RECEIVER_FAIL;
            NewDnlinkSinglepassFlag = true;
            
            evt_appand(eME_RTP_DNSINGLEPASS_EV, \
                eME_REASON_SET_LOCAL_RECEIVER_FAIL, \
                "dnlink singlepass:maybe voice set local receiver failed.", \
                NULL, 0);
        }
        else if (_voeDnLinkSinglepassReason & eME_REASON_START_RECEIVE_FAIL)
        {
            _voeDnLinkSinglepassReason ^= eME_REASON_START_RECEIVE_FAIL;
            NewDnlinkSinglepassFlag = true;
            
            evt_appand(eME_RTP_DNSINGLEPASS_EV, \
                eME_REASON_START_RECEIVE_FAIL, \
                "dnlink singlepass:maybe voice start receive failed.", \
                NULL, 0);        
        }
        else if (statReport.packets_received > 0)
        {
            NewDnlinkSinglepassFlag = true;
            
            evt_appand(eME_RTP_DNSINGLEPASS_EV, eME_REASON_NETWORK_PROBLEM, \
                "dnlink singlepass:maybe network problem.", NULL, 0);
        }
    }
        
    if ( _voeDownlinkSinglepassFlag && !NewDnlinkSinglepassFlag )
    {
        evt_appand(eME_RTP_DNSINGLEPASS_EV, eME_REASON_RTP_RESTORE_OK, \
            "downlink singlepass: restore ok.", NULL, 0);
    }
	_voePktReceived = statReport.packets_received;
    _voeDownlinkSinglepassFlag = NewDnlinkSinglepassFlag;
   
    //updata net status
	if ( !_voeDownlinkSinglepassFlag )
	{
		ME_network_state_t net;
		memset(&net, 0, sizeof(ME_network_state_t));

		net.ice = _iceMode;
		net.uplinkLost = upLost;
		net.downlinkLost = downLost;
		// net.sequenceLost = sequenceLost;
		net.rttMs = statReport.rttMs;
		net.downJitter = statReport.down_jitter;
        net.upJitter = statReport.up_jitter;

		if ((downLost <= NETSTATE_FRACT_NICE_VALUE)
			&& ((short)statReport.rttMs <= NETSTATE_RTT_NICE_VALUE)
			&& (statReport.down_jitter <= NETSTATE_JIFF_NICE_VALUE))
		{
			net.net_state = eME_REASON_NETWORK_NICE;					
		}
        else if ((downLost <= NETSTATE_FRACT_WELL_VALUE)
            && ((short)statReport.rttMs <= NETSTATE_RTT_WELL_VALUE)
            && (statReport.down_jitter <= NETSTATE_JIFF_WELL_VALUE))
		{
			net.net_state = eME_REASON_NETWORK_WELL;
		}
        else if ((downLost <= NETSTATE_FRACT_GENERAL_VALUE)
            && ((short)statReport.rttMs <= NETSTATE_RTT_GENERAL_VALUE)
            && (statReport.down_jitter <= NETSTATE_JIFF_GENERAL_VALUE))
		{
			net.net_state = eME_REASON_NETWORK_GENERAL;
		}
        else if ((downLost <= NETSTATE_FRACT_POOR_VALUE)
            && ((short)statReport.rttMs <= NETSTATE_RTT_POOR_VALUE)
            && (statReport.down_jitter <= NETSTATE_JIFF_POOR_VALUE))
		{
			net.net_state = eME_REASON_NETWORK_POOR;			
		}
		else
		{
			net.net_state = eME_REASON_NETWORK_BAD;
		}

		evt_appand(eME_NETWORK_STATE_EV, net.net_state, \
		    "network state changed.", &net, ME_NETWORK_STATE_T_SIZE);

		_voeFractionLost = upLost;
	}
}

void Conductor::updateSoundDevice()
{
#ifdef _WIN32	
	if(_voeHardware)
	{
		int recordNum = 0;
		int playNum = 0;

		_voeHardware->GetNumOfRecordingDevices(recordNum);
		_voeHardware->GetNumOfPlayoutDevices(playNum);
		
		if(recordNum != _recordDeviceNum || playNum != _playDeviceNum)
		{
			_recordDeviceNum = recordNum;	//update record num
			_playDeviceNum = playNum;		// update play num
			_voeHardware->SetRecordingDevice(-1);
			_voeHardware->SetPlayoutDevice(-1);
			RestartAudioStreamSend();
		}
	}
#endif
}

void Conductor::updateCfg()
{
    if (_voiceChannelId < 0)
        return;

	//config voice engine
	if(CFG_UPDATE_VOICE & _updateCfgFlag)
	{
		AgcModes AgcModeCfg;
		AgcModes RX_AgcModeCfg = kAgcFixedDigital;
		EcModes EcModeCfg;

#ifdef _WIN32
		AgcModeCfg = kAgcAdaptiveAnalog;
		EcModeCfg = kEcAec;
#else
		AgcModeCfg = kAgcAdaptiveDigital;
		EcModeCfg = kEcAecm;
#endif

#ifdef __APPLE__
		//_stVQEConfig.Ec_enable = false;//chgx enable aecm module for solveing noise on iphone 20160330
#endif
		_voeVqe->SetEcStatus(_stVQEConfig.Ec_enable, EcModeCfg);
		if( kEcAecm == EcModeCfg )
		{
		    AecmModes enumSpeakerMode = _stEnvConfig.status ? kAecmLoudSpeakerphone : kAecmEarpiece;
			_voeVqe->SetAecmMode(enumSpeakerMode, true);
		}

        //Set voice cfg if meeting mode;
        if (_stEnvConfig.dialogScene)
        {
            // const AgcConfig meetConfigDefault = {6, 10, 1};	
			AgcConfig meetConfig = { _stVQEConfig.Agc_targetDbfs, _stVQEConfig.Agc_compressionGaindB, true};
#ifdef _WIN32
            _voeVqe->SetAgcConfig(meetConfig);
    		_voeVqe->SetAgcStatus(_stVQEConfig.Agc_enable, kAgcFixedDigital);
#else
    		_voeVqe->SetAgcStatus(false, kAgcFixedDigital);
#endif
    		_voeVqe->SetNsStatus(_stVQEConfig.Ns_enable, kNsVeryHighSuppression); 
        }
        else
        {
			// const AgcConfig configDefault = { 6, 9, 1 }; //default {3, 9, 1}  		
			AgcConfig config = { _stVQEConfig.Agc_targetDbfs, _stVQEConfig.Agc_compressionGaindB, true};
            _voeVqe->SetAgcConfig(config);
    		_voeVqe->SetAgcStatus(_stVQEConfig.Agc_enable, AgcModeCfg);
    		_voeVqe->SetNsStatus(_stVQEConfig.Ns_enable, kNsHighSuppression); 
        }
		//const AgcConfig rxconfig = {6, 9, 1}; //default {3, 9, 1}   modied by king for sloving small voice on 2016.02.26
        //_voeVqe->SetRxAgcConfig(_voiceChannelId, rxconfig);
		_voeVqe->SetRxNsStatus(_voiceChannelId, _stVQEConfig.Ns_Rx_enable, kNsHighSuppression);	
		_voeVqe->SetRxAgcStatus(_voiceChannelId, _stVQEConfig.Agc_Rx_enable, RX_AgcModeCfg);
		_voeVqe->SetDualMicNsStatus(_stVQEConfig.Dual_Mic_Ns_enable);//added by chgx 20160504

		_updateCfgFlag &= ~CFG_UPDATE_VOICE;
	}

	//config rtp
	if (CFG_UPDATE_RTP & _updateCfgFlag)
	{
        _voeNetwork->SetPacketTimeout(_voiceChannelId, true, _stRTPConfig.uiRTPTimeout);
		TraceLog(kTraceReport, "updateCfg uiRTPTimeout=%d uiFixLowPayload=%d", \
            _stRTPConfig.uiRTPTimeout, _stRTPConfig.uiFixLowPayload);
		_updateCfgFlag &= ~CFG_UPDATE_RTP;
	}

	//config rtp
	if (CFG_UPDATE_TGO & _updateCfgFlag)
	{
		_voeNetwork->SetPacketMode(_voiceChannelId, _stCTRLConfig.ucRealTimeType);
		_updateCfgFlag &= ~CFG_UPDATE_TGO;
	}

	//config FEC
	if (CFG_UPDATE_FEC & _updateCfgFlag)
	{
		if ( _voeRtp_Rtcp->SetREDStatus(_voiceChannelId, 
                                    _stCTRLConfig.ucFecEnable && _consultFecEn,
                                    AUDIO_RED_PAYLOAD_TYPE_DEFAULT) < 0 )
		{
            TraceLog(kTraceError, "updateCfg %s FEC failed.",
                _stCTRLConfig.ucFecEnable ? "Enabled" : "Disabled");
		}
        else
        {
		    TraceLog(kTraceReport, "updateCfg FEC %s successed", 
                    _stCTRLConfig.ucFecEnable ? "Enabled" : "Disabled");
        }
		_updateCfgFlag &= ~CFG_UPDATE_FEC;
	}

	//config codec
	if (CFG_UPDATE_CODEC & _updateCfgFlag)
	{
		if (SetSendCodec(_stCodecConfig.codecs[0].pltype, NULL) < 0)
		{
			TraceLog(kTraceError, "updateCfg set audio codec failed, pt=%d", 
                        _stCodecConfig.codecs[0].pltype);
		}
		_updateCfgFlag &= ~CFG_UPDATE_CODEC;
	}

	//envionment update
	if (CFG_UPDATE_ENV & _updateCfgFlag)
	{
        //Set voice cfg if meeting mode;
        if (_stEnvConfig.dialogScene)
        {
			//const AgcConfig configDefault = { 6, 10, 1 };//default {3, 9, 1}			 		
			AgcConfig config = { _stVQEConfig.Agc_targetDbfs, _stVQEConfig.Agc_compressionGaindB, true};
#ifdef _WIN32
            _voeVqe->SetAgcConfig(config);
    		_voeVqe->SetAgcStatus(_stVQEConfig.Agc_enable, kAgcFixedDigital);
#else
    		_voeVqe->SetAgcStatus(false, kAgcFixedDigital);
#endif
    		_voeVqe->SetNsStatus(_stVQEConfig.Ns_enable, kNsVeryHighSuppression); 
        }
        
		_updateCfgFlag &= ~CFG_UPDATE_ENV;
	}	
}

int Conductor::SendCNPacket(void)
{
    //send cn packet when audio device initiation is failed.
	if ( (_voiceChannelId >= 0)
	    && ((_audioDeviceReady == eDEVICE_GET_FAIL) 
	      || (_audioDeviceReady == eDEVICE_INIT_MIC_FAIL)))
	{
		int iLoopCnt = 10;
		while(iLoopCnt--)
		{
			int iTransferedByte;// for sent udp packet;
			_voeNetwork->SendUDPPacket(_voiceChannelId, &g_ucRtpCnPkt, 21, iTransferedByte);
		}
	}

	return 0;
}

int Conductor::StoreAudioStreamParam(const ME_audio_stream_info_t* paudioinfo)
{
	if (paudioinfo != NULL)
	{
		memcpy(&_audioStreamInfo, paudioinfo, ME_AUDIO_STREAM_INFO_ST_SIZE);
	}
	
	return 0;
}

int Conductor::ResetAudioStreamParam(void)
{
	memset(&_audioStreamInfo, 0, sizeof(_audioStreamInfo));

	_audioStreamInfo.iPayLoadType = -1;
	_audioStreamInfo.iLocalAudioPort = -1;
	_audioStreamInfo.iRemoteAudioPort = -1;
	_audioStreamInfo.ucExTransportEnable = false;
	
	return 0;
}

int Conductor::ResetSinglepass(void)
{
    _voeUpLinkSinglepassReason = 0;
    _voeDnLinkSinglepassReason = 0;
    _voePktSent        = 0;       //-1 is invalid that means sending isn't begin, Rambo on 20137-29
    _voePktReceived    = 0;   //-1 is invalid that means receiveing isn't begin, Rambo on 20137-29
    _voeUplinkSinglepassFlag   = false;
	_voeDownlinkSinglepassFlag = false;
    _voeFractionLost        = 0;

	return 0;
}

int Conductor::ResetEmodel(int pt)
{
#ifdef EMODEL_ENABLED
	CriticalSectionScoped lock(_critSect);

	stCodecRateInfo codecRate[MAX_RATE_CNT]; 
	int max_cnt = 0;
    stPktInfo   stPktTypeTbl[MAX_PKT_CNT+1];
    int max_pkt_cnt = 0; //the number of packet

    emd_calculate_clean();
	//get codec type and type number
	
	if (GetChannelCodecRate(_voiceChannelId, codecRate, max_cnt, stPktTypeTbl, max_pkt_cnt) == 0)
	{
		emd_update_ie_value(codecRate[0].usRateType);
	}

	emd_update_state(eEMD_STOP_S);
#endif

	return 0;
}

int Conductor::SetVoEMode(const ME_audio_stream_info_t* pAudioinfo)
{
    if ( pAudioinfo->isDirectCall == FALSE )
    {
        //Close Rx agc/ns when free call
        _stVQEConfig.Agc_Rx_enable = false;   //modify by wuzhaoyang20160413 
        _stVQEConfig.Ns_Rx_enable = false;
    }
    else
    {
        //Open Rx agc/ns when direct call
        _stVQEConfig.Agc_Rx_enable = true; 
        _stVQEConfig.Ns_Rx_enable = true;
    }
    
	_updateCfgFlag = CFG_UPDATE_VOICE | CFG_UPDATE_VIDEO | CFG_UPDATE_RTP | CFG_UPDATE_TGO | CFG_UPDATE_FEC;
	updateCfg();

	return 0;
}

int Conductor::SetExTransport(bool enabled, int lport, int rport, const char* raddr)
{
	int iRet = 0;
#ifdef EXTERNAL_TRANSPORT
	if (enabled)
	{
		_extVoiceTransportPtr->SetAudioChannel(_voiceChannelId);
        iRet = _voeNetwork->DeRegisterExternalTransport(_voiceChannelId);
        if ( 0 != iRet )
        {
            TraceLog(kTraceError,"webrtc DeRegisterExternalTransport Failed!, error = %d", iRet);
			return -1;
        }
		iRet = _voeNetwork->RegisterExternalTransport(_voiceChannelId, *_extVoiceTransportPtr);
		if (0 != iRet)
		{
			TraceLog(kTraceError,"webrtc RegisterExternalTransport Failed!, error = %d", iRet);
			return -1;
		}
	}
	else	
#endif
	{
		if (_audioStreamInfo.iLocalAudioPort != lport)
		{
   			iRet = SetLocalReceiver(0, _voiceChannelId, lport);
			if (0 != iRet)
			{
				TraceLog(kTraceError, "webrtc SetLocalReceiver Failed!");
				return -1;
			} 
		}

		if (_audioStreamInfo.iRemoteAudioPort != rport 
			|| strcmp(_audioStreamInfo.cRemoteAudioIp, raddr) != 0)
		{
			iRet = _intVoiceTransportPtr->SetSendDestination(raddr, rport);
			if (0 != iRet)
			{
				TraceLog(kTraceError, \
				    "webrtc SetSendDestination(%s:%d) Failed(Error = %d)!", \
				    raddr, rport, _intVoiceTransportPtr->LastError());
				return -1;
			}
		}
	}
	
	return 0;
}

/*Open CNG when free call ,Open dtx for cng, it will reduce the packet count and size obviously, start by king 20160125*/
int Conductor::SetCngMode(const ME_audio_stream_info_t* paudioinfo)
{
	/*close/open CNG when direcr/free call */
	if (paudioinfo->isDirectCall == FALSE)
	{

#ifdef _WIN32	
		_voeCodec->SetVADStatus(_voiceChannelId, true, kVadConventional, false);
#else
		/*For ios and android platform, the count of packets is very important beacuse of bandwidth,so kVadAggressiveHigh is more fit*/
		_voeCodec->SetVADStatus(_voiceChannelId, true, kVadAggressiveHigh, false);
#endif

	}
	else
	{
		/*Close CNG when direct call ,Open dtx for cng, it will reduce the packet count and size obviously; add by Rambo*/
#ifdef _WIN32	
		_voeCodec->SetVADStatus(_voiceChannelId, false, kVadConventional, false);
#else
		/*For ios and android platform, the count of packets is very important beacuse of bandwidth,so kVadAggressiveHigh is more fit*/
		_voeCodec->SetVADStatus(_voiceChannelId, false, kVadAggressiveHigh, false);
#endif
	}

	return 0;

}
/*end by king 20160125*/

int Conductor::SetSendCodec(int pt, const ME_audio_stream_info_t* paudioinfo)
{
	//Get current codec
	CodecInst inst = { 0 };
	if (_voeCodec->GetSendCodec(_voiceChannelId, inst) < 0)
	{
		return -1;
	}

	if (inst.pltype != pt)
	{
		int idx = -1;
		int nNumCodec = 0;
		CodecInst codecInst = { 0 };

		nNumCodec = _voeCodec->NumOfCodecs();
		for (int k = 0; k < nNumCodec; ++k)
		{
			_voeCodec->GetCodec(k, codecInst);
			if (codecInst.pltype == pt)
			{
				idx = k;
				break;
			}
		}

		if (idx == -1)
		{
			TraceLog(kTraceError, "codec(%d) not support", pt);
			return -1;
		}

		if (0 != _voeCodec->SetSendCodec(_voiceChannelId, codecInst))
		{
			TraceLog(kTraceError, \
				"webrtc SetSendCodec(pt = %d, plname = %s) Failed!", \
				codecInst.pltype, codecInst.plname);
			return -1;
		}
		if (NULL != paudioinfo){

			SetCngMode(paudioinfo);

			TraceLog(kTraceReport, "webrtc SetCngMode ok!");
		}
#if 0
		/*Open dtx for cng, it will reduce the packet count and size obviously; add by Rambo*/
#ifdef _WIN32   
		_voeCodec->SetVADStatus(_voiceChannelId, false, kVadConventional, false);//close cng
#else
		/*For ios and android platform, the count of packets is very important beacuse of bandwidth,
		so kVadAggressiveHigh is more fit*/
		_voeCodec->SetVADStatus(_voiceChannelId, false, kVadAggressiveHigh, false);
#endif
#endif
	}

    return 0;
}

bool Conductor::ThreadFunc(void* pThis)
{
    return (static_cast<Conductor*>(pThis)->ThreadProcess());
}

bool Conductor::ThreadProcess()
{ 
	int state = 0;
	/*excute once every 3s, comment by Rookie*/
    switch (_ptrTimeEvent->Wait(5000))
    {
	    case kEventSignaled:
	        break;
	    case kEventError:
			TraceLog(kTraceReport, "EventWrapper::Wait() failed => restarting timer");
	        _ptrTimeEvent->StopTimer();
	        _ptrTimeEvent->StartTimer(true, TIMER_PERIOD_MS);
	        return true;
	    case kEventTimeout:
	        return true;
    }

	if (!_threadKeepAlive)
	{
		TraceLog(kTraceReport, "_threadKeepAlive is true, thread exit!!");
		return true;
	}

	state = GetCurState();
	if(state == eME_Running || state == eME_Connected)
    {
		//send cn packet when audio device initiation is failed. 
		if((_audioDeviceReady == eDEVICE_GET_FAIL)||(_audioDeviceReady == eDEVICE_INIT_MIC_FAIL))
		{
			int iTransferedByte;// for sent udp packet;
			if (_voiceChannelId >= 0)
				_voeNetwork->SendUDPPacket(_voiceChannelId, &g_ucRtpCnPkt, 21, iTransferedByte);
		}
    }

    if (state == eME_Connected)	
    {
        updateRtpStatus();
		updateCfg();
        updateSoundDevice();
    }

	return  true;
}

void Conductor::StartThread()
{
    unsigned int threadID(0);
	bool res = false;

	if(_ptrThread == NULL)
	{
		_threadKeepAlive = true;
		_ptrThread = ThreadWrapper::CreateThread(ThreadFunc, this, kNormalPriority, "Conductor_Thread");
		_ptrThread->Start(threadID);
	}

	if (!_ptrTimeEvent->StartTimer(true, TIMER_PERIOD_MS))
    {
		TraceLog(kTraceError,"failed to start the timer event in conductor");
        res = _ptrThread->Stop();
		if (!res)
        {
			TraceLog(kTraceError, "unable to stop the activated thread");
        }
		
		delete _ptrThread;
        _ptrThread = NULL;
    }
}

void Conductor::StopThread()
{
	if(_ptrThread)
	{
		TraceLog(kTraceReport, "_ptrThread->Stop()---in");
		_threadKeepAlive = false;
  		_ptrTimeEvent->Set();
		_ptrThread->Stop();//this will wait 3s until thread stop
		delete _ptrThread;
		_ptrThread = NULL;
		TraceLog(kTraceReport,"_ptrThread->Stop()---out");
	}
    _ptrTimeEvent->StopTimer();
}

#ifdef IOS_SET_FILEPATH
int Conductor::set_filepath(char *filepath)//add by wuzhaoyang20160407
{
	int iRet = -1;
	iRet = _voeVqe->Set_Filepath(filepath);
	iRet = _voeBase->Base_Set_Filepath(filepath);
	return iRet;
}
#endif
