#ifndef _CONDUCTOR_H_
#define _CONDUCTOR_H_

#include <assert.h>
#include <string>

#include "cfg.h"
#include "MediaEngineInterface.h"
#include "emodel_api.h"

#include <stdio.h>
#include <string.h>
#include <iostream>
#ifdef ANDROID
#include <unistd.h>
#include <android/log.h>
#include <sstream>
#include <iostream>
#elif defined(__APPLE__)  
#include <unistd.h>
#endif

/* Modified By MNY@2015-01-19, delete relative path */
#include "voe_base.h"
#include "voe_codec.h"
#include "voe_network.h"
#include "voe_file.h"
#include "voe_rtp_rtcp.h"
#include "voe_dtmf.h"
#include "voe_external_media.h"
#include "voe_hardware.h"
#include "voe_audio_processing.h"
#include "voe_volume_control.h"
#include "voe_errors.h"
#include "thread_wrapper.h"
#include "trace.h"
#include "event_wrapper.h"
#include "critical_section_wrapper.h"
#include "channel_transport.h"
#include "voe_encryption.h"

//add by Rookie 20130408 for neteq rtp state
#include "voe_neteq_stats.h"//neteq stats



#ifdef EXTERNAL_TRANSPORT
#include "exl_transport.h"
#endif

#ifdef EXTERNAL_PLAYFILE 
#include "exl_playfile.h"
#endif

#include "exl_audio_device.h"
#ifdef EXTERNAL_MEDIA_PROCESS
#include "exl_mediaprocess.h"
#endif // EXTERNAL_MEDIA_PROCESS
#include "exl_audiotransport.h"


using namespace std;
using namespace webrtc;
using namespace webrtc::test;

#define COND_LOAD_ADD_ONE  1
#define COND_LOAD_DEL_ONE  -1

#define	RTP_TIMEOUT_MIN 0
#define RTP_TIMEOUT_MAX 60
#define COND_RTP_TIMEOUT_DEFAULT 45  //time for rtp timeout

// Agc config
#define AGC_TARGET_DBOV_MIN      1
#define AGC_TARGET_DBOV_MAX      15
#define AGC_TARGET_DBOV_DEFAULT  6
#define AGC_COMPRESSION_DB_MIN   1
#define AGC_COMPRESSION_DB_MAX   30
#define AGC_COMPRESSION_DB_DEFAULT   9

#define DTMF_PAYLOAD_TYPE_DEFAULT   101
#define AUDIO_RED_PAYLOAD_TYPE_DEFAULT  127

#define CFG_UPDATE_VOICE	     0x01
#define CFG_UPDATE_VIDEO	     0x02
#define CFG_UPDATE_RTP		     0x04
#define CFG_UPDATE_TGO		     0x08
#define CFG_UPDATE_FEC  	     0x10
#define CFG_UPDATE_CODEC         0x20
#define CFG_UPDATE_ENV           0x40

#define RECORDE_FILE_PATH_LEN   260     //record file path max len
#define TIMER_PERIOD_MS			3000	//the time of thread to execute,ms;

/* Define factor of network quality */
// fraction loss of the packets(%)
#define NETSTATE_FRACT_NICE_VALUE     	5
#define NETSTATE_FRACT_WELL_VALUE       10
#define NETSTATE_FRACT_GENERAL_VALUE    30
#define NETSTATE_FRACT_POOR_VALUE       45

// Maximum Consecutive Lost Datagrams
#define NETSTATE_SEQ_NICE_VALUE         2
#define NETSTATE_SEQ_GENERAL_VALUE      4

// Round Trip Time(ms)
#define NETSTATE_RTT_NICE_VALUE         150
#define NETSTATE_RTT_WELL_VALUE         250
#define NETSTATE_RTT_GENERAL_VALUE      400
#define NETSTATE_RTT_POOR_VALUE			800

// Jitter of network(ms)
#define NETSTATE_JIFF_NICE_VALUE        120
#define NETSTATE_JIFF_WELL_VALUE        200
#define NETSTATE_JIFF_GENERAL_VALUE     300
#define NETSTATE_JIFF_POOR_VALUE        500


/*According to these network states, we select the rate of codec*/     //Added by WeiYouquan in 2015.12.30 
#define NETSTATE_EXCELENT    	5
#define NETSTATE_NICE     	    4
#define NETSTATE_GOOD     	    3
#define NETSTATE_FAIR     	    2
#define NETSTATE_POOR     	    1

enum eRTP_port_mode
{
	RTP_LOCAL_AUDIO_PORT = 15004,   //local audio fix port
	RTP_LOCAL_VIDEO_PORT = 16004,	//local video fix port
	RTP_LOCAL_AUTO_PORT  = 0		//local auto to allocation port
};

/*******device init status*****/
enum eDevice_status
{
    eDEVICE_GET_SUCCESS       = 0, 			    //get device sucessfully
    eDEVICE_GET_FAIL          = 0x01, 		 	//get device failed
    eDEVICE_INIT_MIC_FAIL     = 0x02, 		    //init mic failed
    eDEVICE_INIT_SPEAKER_FAIL = 0x04, 	        //init speaker sucesessfully
};

class Conductor: 
	public VoiceEngineObserver 
	,public TraceCallback
	,public VoETelephoneEventObserver
	, public VoERtcpReportObserver //yangjj add
#ifdef EXTERNAL_ENCRYPT
	,public Encryption
#endif	
{
protected:
	Conductor(void);
	virtual ~Conductor(void);
	int LoadAPIs(int iDataSource);
	int AuthAndInit(void);
public:
	static Conductor* GetInstance(int iDataSource);//to get instance of engine conductor
	static void FreeInstance(void);

	/*Interface to user*/
	int SetAudioChannel(const ME_audio_stream_info_t* paudioinfo);
	int CreateChannel(void);
	int DeleteChannel(void);
	int StartReceive(void);
	int StopReceive(void);
	int StartPlayOut(void);
	int StopPlayOut(void);
	int StartSend(void);
	int StopSend(void);
	int StartRecord(const media_file_record_pm_t* pm);
	int StopRecord(void);
#ifdef EXTERNAL_PLAYFILE
	int ExternalPlayFile(int mode, const media_file_play_pm_t* pm);
#endif 	
	int PlayFile(const media_file_play_pm_t* pm);
#ifdef IOS_SET_FILEPATH
	int set_filepath(char *filepath);//add by wuzhaoyang20160407
#endif
	int StopFile();
	int SendDTMF(char chDtmf);
	int PlayDTMF(char chDtmf);
#ifdef AUTO_TEST_SWITCH
	int SetTestMode(int iMode);
	int StartRTPDump(const char fileNameUTF8[1024], int direction);
	int StopRTPDump(int direction);
	int getLocalIP(int& port, char ipAddr[64]);
	int getDestIP(int& port, char ipAddr[64]);	
#endif
	int SetSpeakerVolume(unsigned int volume);
	int SetMicVolume(unsigned int volume);
	int SetMicrophoneMute(bool enable);
    int GetMicrophoneMute();
	int SetLoudSpeakerStatus(bool enable);
    int GetLoudSpeakerStatus();
    /* add by Vinton.Liu on 2016-03-16 for Detect recording devices status begin */
    int GetRecordingDeviceStatus();
    /* Vinton.Liu add end */
	int SetEnv(ME_ENV_cfg_t *pstEnvcfg);//add by Rambo 20141222
	int EnableFec(char enable);//add by wuzhaoyang 20140607
	int SetDirectCallFecStatus(char enable);//add by wuzhaoyang20151225
	int GetFecStatus(void);
	int SetAudioFec(unsigned short nMulriple, unsigned short  nOffset);//yangjj add
	int SetNetAnalyze(bool isAnalyze);
	/*config interface of user*/
	int GetCurState(void);
	void SetCurState(int state);

	int GetEmodelValue(ME_emodel_info_t* eValue);
	int ResetRTT(void);
	int GetRTT(void);
	int RatioRTT(int oRTT, int nRTT);
	int UpdateRTT(int rtt);
	int UpdateIceMode(int ice_mode);
	int GetIceMode();
	
	int GetChannelCodecRate(int channelid, void* pCodecRateAttr, int &max_codec_cnt, void* pPktAttr, int &max_pkt_cnt);
	
	int RestartAudioStream(void);
	int RestartAudioStreamSend(void);
    int SetLogFilePath(const char* logfile); //set file path of log
    int SetLogLevel(const webrtc::TraceLevel level);//set the leveo of log
	int SetAudioDevice(const audio_device_pm_t* pm);
	bool webrtcCodecSupport(const char * mime);
	bool webrtcCodecSupport(int payload);
	int  getCodecs(ME_codec_list_t* codecs_list);
    
    //register callback vtable function
	int CallBackVtable(ME_cb_vtable_t* pvtable);

    // TraceCallback
    virtual void Print(TraceLevel level, const char *traceString, int length);

	// VoiceEngineObserver
	virtual void CallbackOnError(int channel, int errCode);

	//add by wuzhaoyang20150423
	virtual void OnReceivedTelephoneEventOutOfBand(int channel,	int eventCode, bool endOfEvent);
	virtual void OnReceivedTelephoneEventInband(int channel,int eventCode,	bool endOfEvent);

	// VoERtcpReportObserver
	virtual int SetAudioCodecRate(unsigned short usCurStatus, unsigned short rate);//yangjj add
#ifdef EXTERNAL_AUDIO_DEVICE
 	virtual int exl_send_pcm(ME_exl_pcm_input_itf_t* stInput);
	virtual int exl_rcv_pcm(ME_exl_pcm_output_itf_t* stOutput);
#endif

#ifdef EXTERNAL_ENCRYPT
    virtual void encrypt(int channel, unsigned char* in_data, unsigned char* out_data, int bytes_in, int* bytes_out);
    virtual void decrypt(int channel, unsigned char* in_data, unsigned char* out_data, int bytes_in, int* bytes_out);
    virtual void encrypt_rtcp(int channel, unsigned char* in_data, unsigned char* out_data, int bytes_in, int* bytes_out);
    virtual void decrypt_rtcp(int channel, unsigned char* in_data, unsigned char* out_data, int bytes_in, int* bytes_out);
#endif
	/*To get voice engine state*/
	bool SoundIsStarted(void) { return _voiceChannelId >= 0; }
	void setLowestBitrate(bool enable);

	/*Platform correlative function*/
#ifdef ANDROID
	static void setAndroidObj(const media_jni_env_pm_t* pm_obj);
	static void SetAndroidApi(int level);
#endif

#ifdef EXTERNAL_TRANSPORT
	int RecvDataPacket(const recv_pm_t* recv_pm);
#endif

private:
	int StoreAudioStreamParam(const ME_audio_stream_info_t* paudioinfo);
	int ResetAudioStreamParam(void);
	int ResetSinglepass(void);
	int ResetEmodel(int pt);
	int SetVoEMode(const ME_audio_stream_info_t* pAudioinfo);
	int SetExTransport(bool enabled, int lport, int rport, const char* raddr);
	int SetCngMode(const ME_audio_stream_info_t* paudioinfo);  //modified by king on 20160120
	//int SetSendCodec(int pt);
	int SetSendCodec(int pt, const ME_audio_stream_info_t* paudioinfo);  //modified by king on 20160120
	int SendCNPacket(void);
	
	int AudioDeviceInit(void);
	int AudioDeviceTerminate(void) { _audioDeviceReady = eDEVICE_GET_FAIL; return _voeBase->Terminate(); }
	int GetCurLoadCnt(void);
	void AddRef(int Val);
	void UpdateState(enum eME_state eState);

	int MicrophoneBoostIsAvailable(bool* available);
	int SetMicrophoneBoost(bool enable);
	int MicrophoneBoost(bool* enabled);

	// Volume control based on the Windows Wave API (Windows only)
	int32_t SetWaveOutVolume(uint16_t volumeLeft,uint16_t volumeRight);
	int32_t WaveOutVolume(uint16_t* volumeLeft,uint16_t* volumeRight) ;
	
	void updateCfg();				//interface to cfg device
	void updateSoundDevice();	//interface to change device
	void updateRtpStatus(void);
	void StartThread();	// thread to monitor device changed
	void StopThread();

	void SetDirectcallAudioFec(unsigned short lossrate);//add by wuzhaoyang20151225
	
    static bool ThreadFunc(void*);
    bool ThreadProcess();
	void releaseWebrtcSubApi();//release sub api reasource
	int SetLocalReceiver(int type, int channel, int port);

public:
	int32_t					_updateCfgFlag;         //to update the configuration of voice/rtp/video...
	ME_VQE_cfg_t		    _stVQEConfig;           //config voice qulity enhancement
	ME_RTP_cfg_t		    _stRTPConfig;           //config RTP
	ME_CTRL_cfg_t		    _stCTRLConfig;          //config for control 
	ME_codec_list_t         _stCodecConfig;
	ME_ENV_cfg_t		    _stEnvConfig;           //envionment for control 

private:
    static Conductor*	        m_pInstance;
    
    //To build thread for conductor that used to config/update status and so on...
    EventWrapper*        		_ptrTimeEvent;
    ThreadWrapper* 				_ptrThread;
	CriticalSectionWrapper* 	_critSect;
	CriticalSectionWrapper* 	_critSectAudio;
	
	//Interface of audio engine
	VoiceEngine * 		        _voeEngine;
	VoEBase* 			        _voeBase;
	VoEAudioProcessing*         _voeVqe;
	VoECodec* 			        _voeCodec;
	VoEHardware*		        _voeHardware;
	VoEDtmf*			        _voeDtmf;
	VoEVolumeControl* 	        _voeVolume;
	VoERTP_RTCP*		        _voeRtp_Rtcp;
	VoEFile* 			        _voeFile;
	VoENetwork *		        _voeNetwork;
	VoENetEqStats*		        _voeNeteqStats; //add by Rookie 20130408 for emodel
#ifdef EXTERNAL_ENCRYPT
	VoEEncryption*              _voeEncrypt;
#endif
#ifdef EXTERNAL_MEDIA_PROCESS
	VoEExternalMedia*           _voeExtMedia;
	MediaProcessImpl*           _extMediaProc;
#endif // EXTERNAL_MEDIA_PROCESS
    /* add by Vinton.Liu on 2016-03-29 for support external audio device push pcm to and pull pcm from engine begin */
    VoEAudioTransportImpl*      _voeAudioTransport;
    /* Vinton.Liu add end */

	// Udp Transport
    //internal udp transport
    VoiceChannelTransport*      _intVoiceTransportPtr;
    // extrenal udp transport
#ifdef EXTERNAL_TRANSPORT
	CExternalTransportImpl*     _extVoiceTransportPtr;
#endif

#ifdef EXTERNAL_AUDIO_DEVICE
	ExlAudioDeviceModuleImpl* 	_extAudioDevice;
#endif

#ifdef EXTERNAL_PLAYFILE
	CExlPlayFileAdapter*        _extPlayFile;
#endif

private:
	int32_t  				    _loadCnt;           //module load count
	eME_state				    _curState;          //module state, see  eCOND defined
	bool 				        _threadKeepAlive;   //thread exit flag
	int32_t					    _voeP2PRtt;         //p2p rtt value
	int32_t 				    _voeRtppRtt;        //rtpp rtt value
	eMeIceMode 				    _iceMode;           //ice mode,0:p2p 1:rtpp
    
	int32_t				        _voiceChannelId;
	int32_t				        _audioDeviceReady;

	ME_audio_stream_info_t      _audioStreamInfo;
    ME_net_calc_info_t          _voiceNetInfo;//added by luochengcong 20141021

	//for device monitor
	int32_t                     _recordDeviceNum;
	int32_t                     _playDeviceNum;

	// for file record && play
	int32_t 				    _recordMode;			//Record mode, pls refer macro define
	int32_t                     _recordFormat;          // Record file format
	char                        _recordMixFile[RECORDE_FILE_PATH_LEN]; //record file path 
	char                        _recordMic[RECORDE_FILE_PATH_LEN];
	char                        _recordSpeaker[RECORDE_FILE_PATH_LEN];
	int32_t 				    _playFileDirect;			//Play file direct, 0 play file on local, others to opposition

    // for fec
    bool                        _consultFecEn;        // consult fec result
    bool                        _consultRtppMultFec;  // is rtpp support multiple fec
    uint16_t                    _smoothLossRate;//add by wuzhaoyang20151225

    // for tmp rtp status. 
    uint16_t                    _voeUpLinkSinglepassReason;
    uint16_t                    _voeDnLinkSinglepassReason;
    uint32_t                    _voePktSent;
    uint32_t                    _voePktReceived;
    bool                        _voeUplinkSinglepassFlag;
	bool                        _voeDownlinkSinglepassFlag;
    uint16_t                    _voeFractionLost;  

	/******below's definitions for bitrate ajustability********/
	uint16_t                    _usLastRate;            //last stream rate contain codec and packet
	uint16_t                    _usLastCodeRateIdx;     //last codec rate index
	uint16_t                    _usLastPktLenIdx;       //last packet length index
};

extern Conductor* g_pVoiceEngine;


#endif

