#ifndef SOFTPHONE_CONFIG_H
#define SOFTPHONE_CONFIG_H

#ifdef __APPLE__
#define HAVE_FCNTL_H
#endif

#ifndef ANDROID
#define	OSIP_MT
#define	ENABLE_TRACE	//trace facilities
#endif

/*******************************************************************
* comm setting
*******************************************************************/
#define	DISABLE_TLS
#define	ORTP_STATIC	

/*******************************************************************
* test and debug setting
*******************************************************************/
//#define WEBRTC_WRITE_TO_FILE		//debug log write to file
//#define VOGO_TEST_ON				//the switch of test
#if defined(__APPLE__) || defined(_WIN32)
#define VIGO_ENABLED    		      //test video 
#endif

/*self-check test switch, test option: fec/p2p/emodel/log...
 *don't compile test code if don't define.*/
//#define AUTO_TEST_SWITCH
//#define IOS_SET_FILEPATH

/*******************************************************************
* TGO setting
*******************************************************************/
//#define ENABLE_IPV6				//IPv6 enable
//#define DOUBLE_REG				//double register enable
//#define RC4_ENCRYPT				    //RC4 encrypt enable
//#define RSA_MD5_SIGNATURE 			//to enable rsa-md5 signature
#define ENABLED_BYE_CSEQ_MISMATCH //enabled bye cseq mismatch (support VPS reinvite model)


/*******************************************************************
* UGO setting
*******************************************************************/
#if !defined(ANDROID)
#define ICE_ENABLED		      //ice enabled
#define EXTERNAL_TRANSPORT    //vogo external transport, this is used while ICE_ENABLED is open
#endif
#define EARLY_MEDIA_PLAY_BY_RTPRECEIVE //early media play by rtp receive
//#define DIRECT_SEND_RECEIVE


/*******************************************************************
* VOGO setting
*******************************************************************/
#define EMODEL_ENABLED
#define EXTERNAL_PLAYFILE
#define EXTERNAL_ENCRYPT		   //external encryption for enc/dec data of rtp
//#define EXTERNAL_AUDIO_DEVICE	   //external audio device to replace internal recording/playing. TODO
#define  EXTERNAL_MEDIA_PROCESS    //external the sound change for another voice   (yangjj add)
//#define ME_USE_EVENT_LIST

/*softphone version*/
#ifdef _WIN32
#define VOGO_VERSION	"k_v2.18.0.0"
#elif defined(ANDROID)
#define VOGO_VERSION	"k_v2.18.0.0"
#elif defined(__APPLE__)
#define VOGO_VERSION	"k_v2.18.0.0"
#elif defined(WEBRTC_LINUX)
#define VOGO_VERSION	"k_v2.18.0.0"
#endif

/*******************************************************************
* VIGO setting
*******************************************************************/
//#define	VIGO_TEST_ON			//the switch of test

/*softphone version*/
#ifdef _WIN32
#define VIGO_VERSION	"k_v2.18.0.0"
#elif defined(ANDROID)
#define VIGO_VERSION	"k_v2.18.0.0"
#elif defined(__APPLE__)
#define VIGO_VERSION	"k_v2.18.0.0"
#elif defined(WEBRTC_LINUX)
#define VIGO_VERSION	"k_v2.18.0.0"
#endif


#endif

