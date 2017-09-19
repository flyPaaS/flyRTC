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
#include "voe_encryption.h"
#include "channel_transport.h"

//add by Rookie 20130408 for neteq rtp state
#include "voe_neteq_stats.h"//neteq stats

#include "vie_base.h"
#include "vie_capture.h"
#include "vie_codec.h"
#include "vie_network.h"
#include "vie_render.h"
#include "vie_rtp_rtcp.h"
#include "vie_errors.h"
#include "vie_image_process.h"
#include "video_engine/vie_image_process_impl.h"

#include "frame_callback.h"
#include "webrtc/modules/video_capture/include/video_capture_factory.h"
//#include "libyuv/convert_from.h"
#include "../../third_party/libyuv/include/libyuv.h"
#include "webrtc/system_wrappers/interface/clock.h"

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
#define AGC_TARGET_DBOV_DEFAULT  3
#define AGC_COMPRESSION_DB_MIN   1
#define AGC_COMPRESSION_DB_MAX   30
#define AGC_COMPRESSION_DB_DEFAULT   12

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
#define TIMER_PERIOD_MS			1000	//the time of thread to execute,ms;

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

#define VIDEO_NETSTATE_FRACT_NICE_VALUE     	10
#define VIDEO_NETSTATE_FRACT_WELL_VALUE       20
#define VIDEO_NETSTATE_FRACT_GENERAL_VALUE    30

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
    eDEVICE_GET_SUCCESS = 0, 			//get device sucessfully
    eDEVICE_GET_FAIL = 0x01, 		 	//get device failed
    eDEVICE_INIT_MIC_FAIL = 0x02, 		//init mic failed
    eDEVICE_INIT_SPEAKER_FAIL = 0x04, 	//init speaker sucesessfully
};


enum enumProfileLevel
{
	emProfileLevel_None = 0,
	emProfileLevel_Low=10,
	emProfileLevel_Low_Andriod_0 = 11,
	emProfileLevel_Normal=20,
	emProfileLevel_Normal_Andriod_0=21,
	emProfileLevel_High=30,
	emProfileLevel_1280720=40,
};


typedef struct tag_render_position
{
	ucpaas_float32 left; 	/* valid range: 0.0 to 1.0 */
	ucpaas_float32 top; 	/* valid range: 0.0 to 1.0 */
	ucpaas_float32 right; 	/* valid range: 0.0 to 1.0 */
	ucpaas_float32 bottom; 	/* valid range: 0.0 to 1.0 */
	ucpaas_boolean enableMirrorX;
	ucpaas_boolean enableMirrorY;
}video_render_position;

class RemoteConductor :
	public I420FrameCallback
{
public:
	virtual void FrameCallback(I420VideoFrame* video_frame) ;

	me_screenshot_cb_f		rt_cb;		/* remote screenshot callback*/
	int rotation;
	int64_t previousTime;
	int fps;
	RemoteConductor(){ rotation = 0; previousTime = 0; fps = 0; }
};

class LocalConductor :
	public I420FrameCallback
{
public:
	virtual void FrameCallback(I420VideoFrame* video_frame) ;
	me_screenshot_cb_f		lc_cb;		/* local screenshot callback*/
	int rotation;
	LocalConductor(){ rotation = 0; }

};

class Conductor: 
	public VoiceEngineObserver 
	,public TraceCallback
	,public VoETelephoneEventObserver
	, public ViERTPObserver
	, public VoERtcpReportObserver
//	,public ViEDecoderObserver
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

	int StartScreenShot(int islocal, ScreenType type);

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
	int PlayFile(int32_t mode, const media_file_play_pm_t* pm);
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
#ifdef IOS_SET_FILEPATH
    int set_filepath(char *filepath);//add by wuzhaoyang20160407
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

	//add by wuzhaoyang20150309
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


	/* Init video resource all at once, return 0 if succeed, else -1 */
	int VideoInit(void);
	/* Release video resource all at once, return 0 if succeed, else -1 */
	int	VideoRelease(void);
	/* Start video, interface also for user, return 0 if succeed */
	int VideoStart(const unsigned int type);
	/* Stop video, interface also for user, return 0 if succeed */
	int VideoStop(const unsigned int type);
	/* Set video stream, return 0 if succeed, else -1 */
	int VideoSetStream(const ME_video_stream_info_t* pVideoInfo);
	/* Create video stream, return 0 if succeed, else -1 */
	int VideoCreateStream(void);
	/* Delete video stream, return 0 if succeed, else -1 */
	int VideoDeleteStream(void);
	/* Set video device, return 0 if succeed, else -1 */
	int VideoSetCamera(const ME_video_camera_param_t* param);
	/* Get camera count, return value = camera count */
	int VideoGetCameraCount(void);
	/* Get camera state which was used currently */
	int VideoGetCameraState(ME_video_camera_param_t* camera_state);
	/* Get camera info */
	int VideoGetCameraInfo(const unsigned int camera_idx, ME_video_camera_info_t* camera_info);
	/* Get camera Capability */
	int VideoGetCameraCapability(const ucpaas_uint8 camera_idx, const ucpaas_uint8 cap_idx, ME_video_camera_capability_t* camera_capability);
	/* Configure codec, return 0 if succeed, else -1 */
	int VideoConfigCodec(const unsigned char type);
	/* Configure render, return 0 if succeed, else -1 */
	int VideoConfigRender(void);
	/* Reset video parameters, return 0 if succeed, else -1 */
	int VideoResetParam(void);
	/* Video default parameters, return 0 if succeed, else -1 */
	int VideoDefaultParam(void);
	/* Check video parameter, modify illegal parameters to default value, succeed always, return 0 */
	int VideoCheckParam(void);
	/* Get video parameters, return 0 if succeed, else -1 */
	int VideoGetParam(const int type_param, void *para);
	/* Set video parameters, return 0 if succeed, else -1 */
	int VideoSetParam(const int type_param);
	/* Start recording Video, parameter:0-output, 1-input, return 0 if succeed, else -1 */
	int VideoStartRecord(const ME_video_record_param_t* record_param);
	/* Stop recording Video, parameter:0-output, 1-input, return 0 if succeed, else -1 */
	int VideoStopRecord(const unsigned int stream_select);

    /* Set Video remote send rotation */
    int VideoSetRemoteSendRotation(const int angle);
    
    /* Get Video Send Rotation and State */
    int VideoGetSendRotationAndState(int& angle, int& state);

    /* Set video rotation angle via rtp channel or not */
    int VideoSetMediaSignalStatus(int status);

    int VideoSendStatePackets( uint16_t type );
    
	int VideoPreset(char*key, char*value);


	VideoRotation VideoGetRotate(int r);

	int VideoAndriodprocess();

	int VideoSetRemoteReslution(int res);

	int VideoSetRemoteCameraReslution(int width, int height);
	int VideoSetRemoteFps(int decfps);

	int VideoSetRemoteBitRate(unsigned int start, unsigned int min, unsigned int max);

	int VideoSetSendReciveRotation(unsigned int sendRotation, unsigned int recivedRotation);

	int VideoIncomingFrame(const ucpaas_uint8* data, const ucpaas_uint32 len);
    
    int VideoSetFilter(const int filter_type,const ucpaas_float32 filter_sensity);
    
	void VideoPresetDefinition();
	
	/* This method will be called periodically if the average system CPU usage exceeds 75% */
	void PerformanceAlarm(const unsigned int cpuLoad);
    /* This method is called when the decoder needs a new key frame from encoder on the sender. */
    void RequestNewKeyFrame(const int videoChannel);
    void IncomingCodecChanged(const int videoChannel, const VideoCodec& videoCodec);
    void IncomingRate(const int videoChannel, const unsigned int framerate, const unsigned int bitrate);

	// This method is called if SSRC of the incoming stream is changed.
	virtual void IncomingSSRCChanged(const int video_channel,
		const unsigned int SSRC);

	// This method is called if a field in CSRC changes or if the number of
	// CSRCs changes.
	virtual void IncomingCSRCChanged(const int video_channel,
		const unsigned int CSRC,
		const bool added);

	virtual void OnRecivedShortMessage(const int id,
		const unsigned short m);
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
	int SetCngMode(const ME_audio_stream_info_t* paudioinfo);  //added by king on 20160120
	//int SetSendCodec(int pt);
	int SetSendCodec(int pt, const ME_audio_stream_info_t* paudioinfo);  //modified by king on 20160120
	int SendCNPacket(void);
	
	int AudioDeviceInit(void);
	int AudioDeviceTerminate(void) {_audioDeviceReady = eDEVICE_GET_FAIL; return _voeBase->Terminate(); }
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

	void UpdateVideoProtectionMethod();
	
	void StartThread();	// thread to monitor device changed
	void StopThread();

	void SetDirectcallAudioFec(unsigned short lossrate);//add by wuzhaoyang20151225
	
    static bool ThreadFunc(void*);
    bool ThreadProcess();
	void releaseWebrtcSubApi();//release sub api reasource
	int SetLocalReceiver(int type, int channel, int port);
	void RegisterScreenshotCallback(me_screenshot_cb_f cb);

	float VideoCalcScale();

public:
	int32_t                 _updateCfgFlag;		//to update the configuration of voice/rtp/video...
	ME_VQE_cfg_t            _stVQEConfig;		//config voice qulity enhancement
	ME_RTP_cfg_t            _stRTPConfig;		//config RTP
	ME_CTRL_cfg_t           _stCTRLConfig;		//config for control 
	ME_codec_list_t         _stCodecConfig;    // config for codec
	ME_ENV_cfg_t            _stEnvConfig;		//envionment for control 
	
    ME_video_enc_param_t        _stVideoEnc;
    ME_video_dec_info_t         _stVideoDec;
    ME_video_render_param_t     _stVideoRender;
    ME_video_proces_module_t    _stVideoProces;
    ME_video_camera_param_t     _stVideoCamera;
    ME_video_record_param_t     _stVideoRecordParam;
    ME_video_extern_capture_param_t     _stVideoExtCapture;

private:
    static Conductor*	        m_pInstance;
    
	//To build thread for conductor that used to config/update status and so on...
    EventWrapper*        		_ptrTimeEvent;
    ThreadWrapper* 				_ptrThread;
	CriticalSectionWrapper* 	_critSect;
	CriticalSectionWrapper* 	_critSectAudio;
	CriticalSectionWrapper* 	_critSectVideo;
	
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
    VideoChannelTransport*      _intVideoTransportPtr;
    // extrenal udp transport
#ifdef EXTERNAL_TRANSPORT
	CExternalTransportImpl*     _extVoiceTransportPtr;
	CExternalTransportImpl*     _extVideoTransportPtr;
#endif

#ifdef EXTERNAL_PLAYFILE
	CExlPlayFileAdapter*        _extPlayFile;
#endif

#ifdef EXTERNAL_AUDIO_DEVICE
	ExlAudioDeviceModuleImpl* 	_extAudioDevice;
#endif

    int32_t                     _voiceChannelId;
	int32_t                     _audioDeviceReady;

	//for device monitor
	int32_t                     _recordDeviceNum;
	int32_t                     _playDeviceNum;

    // audio file play
    int32_t 				    _playFileDirect;			//Play file direct, 0 play file on local, others to opposition
	// for audio record 
	int32_t 				    _recordMode;			//Record mode, pls refer macro define
	int32_t                     _recordFormat;          // Record file format
	char                        _recordMixFile[RECORDE_FILE_PATH_LEN]; //record file path 
	char                        _recordMic[RECORDE_FILE_PATH_LEN];
	char                        _recordSpeaker[RECORDE_FILE_PATH_LEN];

    ME_audio_stream_info_t      _audioStreamInfo;
    ME_net_calc_info_t          _voiceNetInfo;//added by luochengcong 20141021
    
	// for tmp rtp status. 
    uint16_t                    _voeUpLinkSinglepassReason;
    uint16_t                    _voeDnLinkSinglepassReason;
    uint32_t                    _voePktSent;
    uint32_t                    _voePktReceived;
    bool                        _voeUplinkSinglepassFlag;
	bool                        _voeDownlinkSinglepassFlag;
    uint16_t                    _voeFractionLost;    

private:	
	VideoEngine*                _videoEngine;
    ViEBase*                    _vieBase;
    ViECapture*                 _vieCapture;
    ViERTP_RTCP*                _vieRTPRTCP;
    ViERender*                  _vieRender;
    ViECodec*                   _vieCodec;
    ViENetwork*                 _vieNetwork;
    ViEImageProcess*            _vieImageProc;	
	VideoCaptureModule*         _vieCPM;
	ViEExternalCapture*         _vieExtCapture;

    RemoteConductor*		    _remoteConductor;
	LocalConductor*		        _localConductor;
	
    int32_t                     _videoChannelID;
	int32_t                     _videoCaptureID;
	
	ME_video_stream_info_t      _vieStreamInfo;
	video_render_position 	    _viePositionRemote; /* The relative render position of remote video to the defined window */
	video_render_position 	    _viePositionLocal; /* The relative render position of local video to the defined window */
	
	uint32_t                    _vieSendRotation;
	uint32_t                    _vieRecivedRotaton;
	uint32_t                    _vieRemoteSendRotation;
	uint32_t                    _vieRemoteVideoWidth;
	uint32_t                    _vieRemoteVideoHeight;
	enumProfileLevel            _vieDefinition;  
    int32_t                     _vieRemoteRes;    
    int32_t                     _vieRemoteFps;
    int32_t                     _vieDecoderPt;
    int32_t                     _vieEncoderPt;
    uint32_t                    _vieRemoteStartBitrate;
    uint32_t                    _vieRemoteMinBitrate;
    uint32_t                    _vieRemoteMaxBitrate;

    bool                        _vieFecEnable;
    uint32_t                    _vieType;
    uint32_t                    _vieTypeSupposed;

    // for tmp video rtp status
	uint32_t                    _vieUpLinkSinglepassReason;
	uint32_t                    _vieDnLinkSinglepassReason;
	int32_t                     _viePktSent;
	int32_t                     _viePktReceived;
	bool                        _vieUplinkSinglepassFlag;
	bool                        _vieDownlinkSinglepassFlag;
	
private:
    int32_t                     _loadCnt;			//module load count
	eME_state				    _curState; 		    //module state, see  eCOND defined
	bool 				        _threadKeepAlive;	//thread exit flag
	int32_t					    _voeP2PRtt;			//p2p rtt value
	int32_t 				    _voeRtppRtt;			//rtpp rtt value
	eMeIceMode 				    _iceMode;			//ice mode,0:p2p 1:rtpp

	bool                        _consultFecEn;        // consult fec result
	bool					    _consultRtppMultFec;  // is rtpp support multiple fec
	uint16_t                    _smoothLossRate;//add by wuzhaoyang20151225
	
	/******below's definitions for bitrate ajustability********/
	uint16_t                    _usLastRate;            //last stream rate contain codec and packet
	uint16_t                    _usLastCodeRateIdx;     //last codec rate index
	uint16_t                    _usLastPktLenIdx;       //last packet length index
};

//extern Conductor* g_pVoiceEngine;

extern Conductor* g_pVideoEngine;


#endif

