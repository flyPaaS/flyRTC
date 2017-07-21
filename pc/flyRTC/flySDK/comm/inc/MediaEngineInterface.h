/***************************************************************************
** 版权所有:  Copyright (c) 2014-2019
** 文件名称:  MediaEngineInterface.h
** 文件标识: 
** 内容摘要:  interface define for media engine
** 当前版本:  v1.0
** Author  :  Rambo fu
** 完成日期: 2014年9月06日
** 修改记录: 
** 修改日期:  2014.9.23
** 版本号: 
** 修改人:    Mao Ningyuan
** 修改内容:  Optimize video interface and struct
***************************************************************************/

#ifndef  _MEDIA_ENGINE_INTERFACE_H_
#define _MEDIA_ENGINE_INTERFACE_H_

#include "typedef.h"

#ifdef WIN32
#pragma warning (disable:4996)
#endif

#define RES_OK	0
#define RES_ERR -1
#define RES_ERR_USED -2 			 //VOE is sound send

#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE  0
#endif

/*************************************************************************************
**
**								enum/state/event defintion
**
*************************************************************************************/

/*********** media engine struct define ************/
enum eME_state
{
	eME_done,			  //done
	eME_init,			  //init
	eME_idle,			  //idle
	eME_Running, 		  //active
	eME_Connected		  //connected
};

enum eME_event_type
{
	eME_RTP_TIMEOUT_EV,		// RTP timeout
	eME_RTP_UPSINGLEPASS_EV,// UP RTP single pass
	eME_RTP_DNSINGLEPASS_EV,// DN RTP single pass
	eME_NETWORK_STATE_EV,	// networt state ("general", "nice" ,"bad")
	eME_VIDEO_NETWORK_STATE_EV,	// networt state ("general", "nice" ,"bad")
	eME_EARLY_MEDIA_EV,		// early media (ringing)
	eME_DEVICE_EV,			// device status info	
    eME_LOG_TRACE_EV,		// trace log
	eME_GET_DTMF_EV,		// dtmf event
	eME_FILE_PLAY_TO_ENDED,      // file play to end
	eME_VIDEO_EV,
    eME_REMOTE_VIDEO_STATE_NOTIFY_EV
//Me_OPEN_CAMARA_STATE_NOTIFY_EV
};

/** network state reason *****/
enum eME_netstate_event_reason
{
    eME_REASON_NETWORK_NICE,    /* network state is nice */
    eME_REASON_NETWORK_WELL,    /* network state is well */
    eME_REASON_NETWORK_GENERAL, /* network state is general */
    eME_REASON_NETWORK_POOR,    /* network state is poor */
    eME_REASON_NETWORK_BAD,     /* network state is bad */
};

enum eME_event_reason
{
    /**RTP timeout****/
    eME_REASON_RTP_TIMEOUT, 	/*rtp recv timeout*/

    /**RTP singlepass****/
    eME_REASON_NETWORK_PROBLEM,			//network problem
    eME_REASON_AUDIO_DEVICE_INIT_FAIL,	//local device init failed
    eME_REASON_START_SEND_FAIL,			//start send failed
    eME_REASON_START_RECEIVE_FAIL,		//start receive failed
    eME_REASON_SET_LOCAL_RECEIVER_FAIL,	//set local receive info failed 
    eME_REASON_LOCAL_RECEIVE_FAIL,		//local receive packet failed
    eME_REASON_VIDEO_DEVICE_INIT_FAIL,	//local device init failed
    eME_REASON_OPEN_CAMERA_FAILED,      // open camara failed
    eME_REASON_RTP_RESTORE_OK,          // rtp singlepass restore

    /**early media***/
    eME_REASON_EARLY_MEDIA,	/*first time recv rtp stream, it is ringing audio*/

    /**log level**/
    eME_REASON_LOG_LEVEL_ERR,	/*log level for error*/
    eME_REASON_LOG_LEVEL_INFO,  /*log level for info*/

    /** file play/record **/
    eME_REASON_FILE_PLAY_TO_ENDED, /* file play to ended */
};

/*****trace level***/
enum eME_TraceLevel
{
    /* Synchronize Trace level defines with meidia engine */
    kME_TraceNone               = 0x0000,    // no trace
    kME_TraceStateInfo          = 0x0001,
    kME_TraceWarning            = 0x0002,
    kME_TraceError              = 0x0004,
    kME_TraceCritical           = 0x0008,
    kME_TraceApiCall            = 0x0010,
    kME_TraceDefault            = 0x00ff,
    
    kME_TraceModuleCall         = 0x0020,
    kME_TraceMemory             = 0x0100,   // memory info
    kME_TraceTimer              = 0x0200,   // timing info
    kME_TraceStream             = 0x0400,   // "continuous" stream of data
    
    // used for debug purposes
    kME_TraceDebug              = 0x0800,  // debug
    kME_TraceInfo               = 0x1000,  // debug info
    
    // Non-verbose level used by LS_INFO of logging.h. Do not use directly.
    kME_TraceTerseInfo          = 0x2000,
    kME_TraceReport			    = 0x4000, /* unsupported in media engine*/
    
    kME_TraceAll                = 0xffff
};

/*****file format ***/
enum eME_FileFormats
{
    kME_FileFormatWavFile        = 1, /*wav format file*/
    kME_FileFormatCompressedFile = 2, /*Compressed format file*/
    kME_FileFormatAviFile        = 3, /*avi format file*/
    kME_FileFormatPreencodedFile = 4, /*Preencoded format file*/
    kME_FileFormatPcm16kHzFile   = 7, /*Pcm16kHz format file*/
    kME_FileFormatPcm8kHzFile    = 8, /*Pcm8kHz format file*/
    kME_FileFormatPcm32kHzFile   = 9  /*Pcm32kHz format file*/
};

enum media_file_play_md_t
{
	kME_FILE_HANDLE   = 0,
	kME_FILE_STREAM   = 1,
	kME_FILE_PATHNAME = 2
};

enum tag_record_mode
{
    eME_RECORD_ALL = 0, 		//record all
    eME_RECORD_MIC = 1, 		//record :mic
    eME_RECORD_SPEAKER = 2, 		//record speaker
};

/***media type****/
enum tag_media_type
{
	eME_MEDIA_AUDIO = 0, /*audio*/
	eME_MEDIA_VIDEO = 1  /*video*/
};

/***media data type****/
enum tag_media_data_type
{
	eME_MEDIA_RTP = 0,	/*rtp*/
	eME_MEDIA_RTCP = 1  /*rtcp*/
};

/*ice mode */
typedef enum tag_ice_mode
{
	eME_ICE_P2P_MD  = 0, /*p2p mode*/
	eME_ICE_RTPP_MD = 1  /*rtpp proxy mode*/
} eMeIceMode;

/*****Check codecs supported*****/
enum eME_codecs_check_method
{
    eME_codecs_mime_method,		/*mime method*/
    eME_codesc_payload_method   /*payload method*/
};

enum ScreenType{
	ABGR,
	ARGB,
	RGB24
};

/**
  *configeration option for VoGo/ViGo, ID value is Corresponding 
  *to special VoGo/ViGo module configuration, UGo_set_config()/
  *UGo_get_config() use it to config special module parameter.
**/
enum eME_module_cfg_id
{
	ME_CTRL_CFG_MODULE_ID = 100, //Engine control module configuration corresponding media structure 
	ME_VQE_CFG_MODULE_ID  = 101, //module configuration structure corresponding vqe 
	ME_RTP_CFG_MODULE_ID  = 102, // rtp module configuration corresponding structure 
	ME_ENV_CFG_MODULE_ID  = 103, //corresponding environment variables module configuration structure
	ME_VIDEO_ENC_CFG_MODULE_ID  = 104, /* Encoder parameters included */ 
	ME_VIDEO_DEC_CFG_MODULE_ID  = 105, /* Decoder parameters included */
	ME_VIDEO_RENDER_CFG_MODULE_ID = 106, /* Render parameters included */
	ME_VIDEO_PROCES_CFG_MODULE_ID = 107, /* Image process parameters included */
	ME_CODECS_CFG_MODULE_ID   = 108,
	ME_VIDEO_EXTERN_CAPTURE_CFG_MODULE_ID = 109
};

/*************************************************************************************
**
**								struct defintion
**
*************************************************************************************/

/*****configeration info for control*******/
typedef struct tag_CTRL_config
{	
	ucpaas_uint8 ucRealTimeType;	// Real time protocol type, 0: RTP 1: PRTP, default 0
	ucpaas_uint8 ucVideoEnable;	    // Video module, 0: disable 1: enable, default 0
	ucpaas_uint8 ucEmodelEnable;	// Emodel module, 0: disable 1: enable, default 1
	ucpaas_uint8 ucFecEnable;	    // Fec module, 0: disable 1: enable, default 0
	ucpaas_uint8 ucRtpEncEnable;    // Rtp encrypt, 0: disabled, 1: enabled, default 0
	ucpaas_uint8 ucExtMediaProcEn;  // External pcm media processing, 0: disabled, 1: enabled, default 0
	ucpaas_uint8 ucExtAudioTransEn; // support external audio device push pcm to and pull pcm from engine
	ucpaas_uint8 ucIPv6Enable;      // udp transport ipv6 enabled, 0: disabled 1: enabled, default disabled
	ucpaas_uint8 vieFecEnabled;     // video fec enabled,  false for disabled, true for enabled, default false
	ucpaas_uint8 vieNackEnabled;    // video nack enabled, false for disabled, true for enabled, default false 
	ucpaas_int8 reserved[6];       //reserved 14 byte  data,currently not used
} ME_CTRL_cfg_t;

/*****configeration info for voice quality enhancement*******/
typedef struct tag_VQE_config
{
    ucpaas_boolean Ec_enable;				//Enable EC function on send port if true, else disable    
    ucpaas_boolean Ns_enable;				//Enable Ns function on send port if true, else disable
    ucpaas_boolean Ns_Rx_enable;			//Enable Ns function on receive port if true, else disable
    ucpaas_boolean Agc_enable;			//Enable Agc function on send port if true, else disable
    ucpaas_uint16  Agc_targetDbfs;			//agc target level,value range:1---15,default value:6, typical value:3(high volume) 6(medium volume) 9(small volume)
	ucpaas_uint16  Agc_compressionGaindB;	//agc compressionGain ,value range:1---30,default value 9, typical value:12(high volume) 9(medium volume) 6(small volume)
    ucpaas_boolean Agc_Rx_enable;			//Enable Agc function on receive port if true, else disable  
	ucpaas_boolean Dual_Mic_Ns_enable;      //Enable dual microphone noise suppression
	ucpaas_int8    reserved[3];			    //reserved 1 int ,currently not used 
}ME_VQE_cfg_t;

/*****configeration info for RTP*******/
typedef struct tag_RTP_config
{
    ucpaas_uint32 	uiRTPTimeout;	//RTP time over, it is valid when set RTP as transport;
  	ucpaas_boolean 	uiFixLowPayload;//when network is bad,we used this,only used in prtp protocal,if this enable,auto payload close
  	ucpaas_int8 	reserved[7]; //reserved 3 byte data ,currently not used 
}ME_RTP_cfg_t;

typedef struct tag_ENV_config
{
	ucpaas_int32 networktype;  //network type: 0:2G; 1:3G; 2:wifi; 3:4G; the other value is invalid   
	ucpaas_int8 status;        //open or close Speaker,0:closed the speaker, 1:open the speaker; default:0
	ucpaas_int8 dialogScene;   //dialog scene: 0: general mode; else other: meeting mode
	ucpaas_int8 reserved[14];  /*reserved 8 byte data ,currently not used*/
}ME_ENV_cfg_t;

typedef struct tag_audio_stream_info
{
	ucpaas_int32 iPayLoadType;	       //audio codec payload type
	ucpaas_int8  cRemoteAudioIp[64];   //audio remote  ip addr
	ucpaas_int32 iRemoteAudioPort;	   //audio remote port
	ucpaas_int32 iLocalAudioPort;	   //audio local port
	
	ucpaas_int8  ucExTransportEnable;  //extransport enabled
	ucpaas_int8  isDirectCall;       // whether direct call or free call
	ucpaas_int8 reserved[14];  /*reserved 8 byte data ,currently not used*/
} ME_audio_stream_info_t;
#define ME_AUDIO_STREAM_INFO_ST_SIZE sizeof(ME_audio_stream_info_t)

typedef struct tag_video_stream_info
{
	ucpaas_int32 iPayLoadType;			//video codec payload type
	ucpaas_int8  cRemoteVideoIp[64];	//video remote ip addr
	ucpaas_int32 iRemoteVideoPort;		//video remote port
	ucpaas_int32 iLocalVideoPort;		//video local port
    ucpaas_int32 iMediaSignalEnable;    // send video angle and state via rtp channel 
	ucpaas_int8  ucExTransportEnable;	//extransport enabled
	ucpaas_uint8 vieFecEnabled;         // video fec consultation result, 0 for failed, 1 for success
	ucpaas_uint8 vieNackEnabled;        // video nack consultation result, 0 for failed, 1 for success
	ucpaas_int8  reserved[9];            // reserved 8 byte data ,currently not used
} ME_video_stream_info_t;
#define ME_VIDEO_STREAM_INFO_ST_SIZE    (sizeof(ME_video_stream_info_t))

#define MAX_VIDEO_CODEC_LIST_NUM 10
typedef struct tag_video_codec
{
	ucpaas_uint8		ucPlName[32]; 	/* Payload name, that is, encoder type */
	ucpaas_uint8 		ucPlType; 		/* video codec payload type */
	ucpaas_uint8 		enable; 
}ME_video_codec_info_t;

#define MAX_VIDEO_CODEC_LIST_NUM 10
typedef struct tag_video_enc_param
{
	//以下每个二维数组是用于配置对应的值，第一维代表CPU核数（ANDROID)或机型（IOS），第二维代表了视频的分辨率
	ucpaas_uint32	presets[3][3];
	ucpaas_uint32	bitrates[3][3];
	ucpaas_uint32	fps[3][3];
}VideoProfileParam_t;

typedef struct tag_video_enc
{
	ME_video_codec_info_t	baseCodec[MAX_VIDEO_CODEC_LIST_NUM];		/* Common info */
	VideoProfileParam_t		enc_param_t;
	ucpaas_uint16			usNum;
	ucpaas_uint16			usWidth;		/* local width, including capture and encode picture */
	ucpaas_uint16			usHeight;		/* local height, including capture and encode picture */
	ucpaas_uint16			usStartBitrate; /* bit rate, kbps */
	ucpaas_uint16			usMaxBitrate;
	ucpaas_uint16			usMinBitrate;
	ucpaas_uint8			ucQpMax;
	ucpaas_uint8			ucMaxFramerate; /* fps */
	ucpaas_uint8			ucComplexity; /* 0-4*/
	ucpaas_uint8			ucFirstSelectedPt; /* 0-4*/
	ucpaas_int8				ucUseHwEnc;
	ucpaas_int8				ucUseHwDec;
	ucpaas_int8				reserved[1];
}ME_video_enc_param_t;

typedef struct tag_video_dec
{	
	ucpaas_uint32			uiMaxDefinition;		/* reselution negotiation eg 320*240  640*480*/
	ucpaas_uint8			ucMaxFramerate; /* fps, fps negotiation */
	ucpaas_uint8			reserved[3];
}ME_video_dec_info_t;

typedef struct tag_video_render
{
	void*			pWindowRemote; /* Display window handle for remote video */
	ucpaas_int32    remoteWidth;
	ucpaas_int32    remoteHeight;
	ucpaas_int32    renderMode;
	void*			pWindowLocal; /* Display window handle for local video */
	
}ME_video_render_param_t;

typedef struct tag_video_proces
{
	/* All cameras run the risk of getting in almost perfect sync with florescent lamps, 
	which will result in a very annoying flickering of the image. Enabling this function will remove the flicker.*/
	ucpaas_boolean	deflickingEnable;
	/* Some cameras produce very noisy captured images, especially in low-light conditions. 
	This functionality will reduce the camera noise. */
	ucpaas_boolean	denoisingEnable;
	/* This function enhances the colors on the decoded video stream */
	ucpaas_boolean	enhancementEnable;
	ucpaas_int8		reserved[3];
}ME_video_proces_module_t;

typedef struct tag_video_camera_info
{
	ucpaas_int8 	ucCameraCommonName[256];	/* Current camera's common name */
	ucpaas_int8 	ucCameraUniqueName[256];	/* Current camera's unique name */
	ucpaas_int8		capabilityCount;
	ucpaas_int8		reserved[3];
}ME_video_camera_info_t;

typedef struct tag_video_camera_capability
{
	ucpaas_uint16 	usWidth;
	ucpaas_uint16 	usHeight;
	ucpaas_int8		maxFps;
	ucpaas_int8		reserved[3];
}ME_video_camera_capability_t;

typedef struct tag_video_camera_param
{
	ucpaas_uint16   usWidth;			/* The actual capture video width */ 	
	ucpaas_uint16   usHeight;			/* The actual capture video height */
	ucpaas_int16 	usRotateAngle; 		/* Valid value:0, 90, 180, 270, default -1 */
	ucpaas_int16 	usCameraIdx;		/* Camera's index, -1 means inactive */
	ucpaas_uint8	ucMaxFps;			/* Frame rate */

	ucpaas_uint8	reserved[1];
}ME_video_camera_param_t;

typedef struct tag_video_extern_capture
{
    ucpaas_uint8 useExternCapture; 	/* 0:not use, 1:use */
    ucpaas_uint8	ucExternformate;			/* 0:i420, 1:h264 */
    ucpaas_uint8	reserved[2];
}ME_video_extern_capture_param_t;

typedef struct tag_record_video
{
	ucpaas_int8		fileName[256];
	ucpaas_uint8 	streamSelect;/* 0x01, input; 0x02, output */
	ucpaas_uint8   	encodeType;/* 0, default VP8 encode */
	ucpaas_uint8   	fileType;/* 0, default .avi pack */
	ucpaas_uint8	reserved[4];
}ME_video_record_param_t;

/*****configeration info for emodel*******/
typedef struct tag_emodel_calc_info
{
	ucpaas_int32    flag;	/*effective flag*/
	ucpaas_int32    count;   /*calc count*/
	ucpaas_float64  total;   /*calc total value*/
	ucpaas_float64  average; /*calc average value*/
	ucpaas_float64  min;		/*min value*/
	ucpaas_float64  max;     /*max value */
	ucpaas_float64  current; /*last value*/
	ucpaas_int8     reversed[16];//reversed 2 double 
}ME_emodel_calc_info_t;

typedef struct tag_net_calc_info
{
    ME_emodel_calc_info_t jitbuf;/*added by luochengcong 20141021*/
    ME_emodel_calc_info_t delay;/*added by luochengcong 20141021*/
}ME_net_calc_info_t;

/* rtp packet receive and send statistics */
typedef struct tag_packet_statistics
{
    ucpaas_int32 pktSnd;        /* count for rtp packets send */
	ucpaas_int32 pktRecv;       /* count for rtp packets receive */
} ME_packet_stat_t;

typedef struct tag_emodel_info
{
	ME_emodel_calc_info_t emodel_mos;   /*mos value, 0~4.5*/
	ME_emodel_calc_info_t emodel_tr;    /*rtt value, 0~1000ms*/
	ME_emodel_calc_info_t emodel_ppl;   /*loss rate, 0~30%*/
	ME_emodel_calc_info_t emodel_burstr;/*burst ratio, 1~2*/
	ME_emodel_calc_info_t emodel_ie;    /*ie rate, 0~40*/
    ME_emodel_calc_info_t jitbuf;
    ME_emodel_calc_info_t delay;
    ME_packet_stat_t pkt_stat;          /* rtp packet receive and send statistics */
} ME_emodel_info_t;

/*****configeration info for codec*******/
#define ME_RTP_PAYLOAD_NAME_SIZE    32
typedef struct tag_codec_info
{
    ucpaas_int32 pltype;			//playload type value
    ucpaas_int8  plname[ME_RTP_PAYLOAD_NAME_SIZE];	//codec name 
    ucpaas_int32 enabled; 		//use enabled
} ME_codec_info_t;

#define MAX_CODEC_LIST_NUM		20
typedef struct tag_codec_list
{
 	ucpaas_int32 num;
	ME_codec_info_t codecs[MAX_CODEC_LIST_NUM];
} ME_codec_list_t;
#define ME_CODEC_LIST_T_SIZE	(sizeof(ME_codec_list_t))

typedef struct tag_media_event_node
{
	ucpaas_int32 ev_type;		 //event type 
	ucpaas_int32 ev_reason;		 //event reason	
	ucpaas_int8  something[256]; //event description
	ucpaas_int32 size;           // event param size
	ucpaas_int8  param[0];       // event param
} ME_event_node_t;
#define ME_EVENT_NODE_T_SIZE (sizeof(ME_event_node_t))

/* audio network state */
typedef struct tag_me_network_state
{
	ucpaas_int32 net_state;         // network state
	ucpaas_int32 uplinkLost;	    // up link lost
	ucpaas_int32 downlinkLost;      // down link lost
	ucpaas_int32 sequenceLost;      // sequence lost
	ucpaas_int32 rttMs;             // round-trip-time
	ucpaas_int32 upJitter;          // up link jitter
    ucpaas_int32 downJitter;        // down link jitter
	ucpaas_int8  ice;               // p2p or rtpp
	
} ME_network_state_t;
#define ME_NETWORK_STATE_T_SIZE (sizeof(ME_network_state_t))

/* video network state */
typedef struct tag_me_video_network_state
{
	ucpaas_int32 net_state;         // network state
	ucpaas_int32 uplinkLost;        // up link lost
	ucpaas_int32 downlinkLost;      // down link lost	
	ucpaas_int32 rttMs;             // round-trip-time
    ucpaas_int32 upJitter;          // up link jitter
    ucpaas_int32 downJitter;        // down link jitter
    ucpaas_int32 bitrate_bps;       // encoder bitrate
	ucpaas_int32 target_fps;        // encoder fps
	ucpaas_int32 rec_bitrate;       // decoder bitrate
	ucpaas_int32 width;             // encoder width
	ucpaas_int32 height;            // encoder height
	ucpaas_int32 decode_width;      // decoder width
	ucpaas_int32 decode_height;     // decoder height
	ucpaas_int32 decode_fps;        // decoder fps
	ucpaas_int32 EncoderPt;         // encoder payload-type
	ucpaas_int32 DecoderPt;         // decoder payload-type
	ucpaas_int8  encCodec[ME_RTP_PAYLOAD_NAME_SIZE];	//encoder videocodec name 
	ucpaas_int8  decCodec[ME_RTP_PAYLOAD_NAME_SIZE];	//encoder videocodec name   
	/*add by yangxiaoxiang*/
	ucpaas_uint8  rtPOT_nCount;       //Router Port Information
	ucpaas_uint32 rtPOT_SendIP[32];
	ucpaas_uint32 rtPOT_SendValue_a[32];
	ucpaas_uint32 rtPOT_SendValue_v[32];
	ucpaas_uint32 rtPOT_RecvIP[32];
	ucpaas_uint32 rtPOT_RecvValue_a[32];
	ucpaas_uint32 rtPOT_RecvValue_v[32];
	/*add by yangxiaoxiang*/
	ucpaas_int8    ice;             // p2p or rtpp
} ME_video_network_state_t;
#define ME_VIDEO_NETWORK_STATE_T_SIZE (sizeof(ME_video_network_state_t))

typedef struct tag_exl_pcm_input_itf_t
{
	void* 		  audioSamples;    	    //input buffer
	ucpaas_uint32 samples;       		//input count
	ucpaas_int8   bytesPerSample;      //byte count one sample
	ucpaas_uint32 samplesPerSec;  		//sampling rate 
} ME_exl_pcm_input_itf_t;

typedef struct tag_exl_pcm_output_itf_t
{
	void* 			audioSamples;        // output buffer
	ucpaas_uint32	samples;            //request count
	ucpaas_uint8 	bytesPerSample;     //byte count one sample
	ucpaas_uint32 	samplesPerSec;   	 //sampling rate 
	ucpaas_uint32& 	SamplesOut;         // output count
} ME_exl_pcm_output_itf_t;

/**me event callback prototype***/
typedef void(*me_event_cb_f)(ucpaas_int32 ev_type, 
                                ucpaas_int32 ev_reason, 
                                const char* something , 
                                void* param, 
                                ucpaas_int32 size);

/**me send message  callback prototype*/
typedef int (*me_send_cb_f)(ucpaas_int32 media_type, 
                                ucpaas_int32 data_type, 
                                const void* data, 
                                ucpaas_int32 slen);

/**me trace log notification callback prototype*/
typedef void (*me_trace_cb_f)(ucpaas_int8* summary, 
                                ucpaas_int8* detail, 
                                enum eME_TraceLevel level);

typedef void (*me_encrypt_cb_f)(ucpaas_uint8* in_data,
							        ucpaas_uint8* out_data,
							        ucpaas_int32 bytes_in,
							        ucpaas_int32* bytes_out);

typedef void (*me_decrypt_cb_f)(ucpaas_uint8* in_data,
							        ucpaas_uint8* out_data,
							        ucpaas_int32 bytes_in,
							        ucpaas_int32* bytes_out);

typedef void(*me_screenshot_cb_f)(const ucpaas_uint8* dst_argb, 
                                    ucpaas_int32 dst_stride, ucpaas_int32 width,
                                    ucpaas_int32 height, ucpaas_int32 islocal, 
                                    ScreenType screen_type);

typedef void (*me_media_proc_cb_f)( ucpaas_int16* audio_10ms,
                                    ucpaas_int32 samples,
                                    ucpaas_int32 samplingFreqHz,
                                    ucpaas_boolean isStereo,
                                    ucpaas_int16* audio_out,
                                    ucpaas_int32* samples_out);

/* add by Vinton.Liu on 2016-03-18 for  support external audio device push pcm to and pull pcm from engine begin */
typedef void (*me_init_playout_cb_f)(ucpaas_uint32 sample_rate, 
                                     ucpaas_uint32 bytes_per_sample,
                                     ucpaas_uint32 num_of_channels);

typedef void (*me_init_recording_cb_f)(ucpaas_uint32 sample_rate, 
                                     ucpaas_uint32 bytes_per_sample,
                                     ucpaas_uint32 num_of_channels);

typedef int (*me_pull_capture_data_cb_f)(ucpaas_sint8* audio_data, 
                                            ucpaas_uint32 size);

typedef int (*me_push_render_data_cb_f)(const ucpaas_sint8* audio_data, 
                                            ucpaas_uint32 size);
/* Vinton.Liu add end */

// add zhouwq
typedef int(*me_tips_data_cb_f)(const ucpaas_sint8* data, ucpaas_uint32 size);
// add end

/**
 * This structure holds all callbacks that the application should implement.
 *	None is mandatory.
**/
typedef struct tag_cb_vtable_tag
{
	me_event_cb_f		event_cb;	                /* me event callback */
	me_trace_cb_f 		log_cb; 	                /* notifies that log trace */
	me_send_cb_f   		send_cb; 	                /* send callback */
	me_encrypt_cb_f     encrypt; 	                /* encrypt encode data of rtp */
	me_decrypt_cb_f		decrypt; 	                /* decrypt encode data of rtp */
	me_screenshot_cb_f	screenshot_cb;		        /* remote screenshot callback */
    me_media_proc_cb_f  extmedia_cb;                /* external media processing  */
    me_init_playout_cb_f initplayout_cb;            /* init playout param */
    me_init_recording_cb_f initrecording_cb;        /* init reocrding param */
    me_pull_capture_data_cb_f pull_capture_cb;      /* pull audio capture data */
    me_push_render_data_cb_f push_render_cb;        /* push audio playout data */
} ME_cb_vtable_t;

typedef struct media_file_record_pm_tag
{
	ucpaas_int32 iMode;		   /*record mode, see enum tag_record_mode*/
	ucpaas_int32 iFileFormat;  /*file format, pls refer enum eUGo_FileFormats define.*/
	ucpaas_int8  filepath[256];/*file save path, max len = 256*/
	ucpaas_int8  reserved[8];  /*reserved data ,currently not used*/
} media_file_record_pm_t;

typedef struct media_play_handle_tag
{
	void* fd;
	ucpaas_int8  reserved[8];  /*reserved data ,currently not used*/

} media_play_handle_t;

typedef struct media_play_stream_tag
{
	ucpaas_int32 size;
	void* data;
	ucpaas_int8  reserved[8];  /*reserved data ,currently not used*/
} media_play_stream_t;

typedef struct media_play_pathname_tag
{
	ucpaas_int8  filename[256];/*file save path, max len = 256, if use file play mode*/
	ucpaas_int8  reserved[8];  /*reserved data ,currently not used*/
} media_play_pathname_t;

typedef struct media_file_play_pm_tag
{
	media_play_handle_t filehandle;	/*file handle,if use external file play mode*/
	media_play_stream_t filestream;
	media_play_pathname_t filepath;
	ucpaas_int32 iFileFormat;  /*file format, pls refer enum eUGo_FileFormats define.*/
	ucpaas_int32 iDirect;      /*play location,0 play file on local others play file to opposition*/
	ucpaas_int32 iLoop;        /*play mode, 0 play file one time others play file loop*/
	ucpaas_int8  reserved[8];  /*reserved data ,currently not used*/
} media_file_play_pm_t;

typedef struct media_jni_env_pm_tag
{
	void* jvm;	/*jni jvm*/
	void* env;  /*jni env*/
	void* context;
	ucpaas_int8  reserved[16];  /*reserved data ,currently not used*/
} media_jni_env_pm_t;

typedef struct recv_pm_tag
{
	ucpaas_int32 media_type;
	ucpaas_int32 data_type;
	ucpaas_int32 len;
	void* data;
	ucpaas_int8  reserved[16];  /*reserved data ,currently not used*/
}recv_pm_t;

typedef struct audio_device_pm_tag
{
	ucpaas_uint32 wav_dev_in;
	ucpaas_uint32 wav_dev_out;
	ucpaas_int8   reserved[16];  /*reserved data ,currently not used*/
} audio_device_pm_t;

typedef struct codec_support_pm_tag
{
	ucpaas_int8* codecs_str;
	ucpaas_int32 e_method;
	ucpaas_int8  reserved[16];  /*reserved data ,currently not used*/
} codec_support_pm_t;

/******************************************/

#endif  //_MEDIASTREAMINTERFACE_H_

