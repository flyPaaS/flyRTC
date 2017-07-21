#include <string.h> //strlen()
#include <stdio.h>
#include <time.h>

#ifdef _WIN32
#include <ws2tcpip.h>
#endif

#include "conductor.h"
#include "event_queue.h"

extern void resetVideoRenderView(void *view);

using namespace webrtc;

#define TraceLog(A,...) WEBRTC_TRACE(A, kTraceVideo, 0, __VA_ARGS__);

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
static bool g_localshotflag = false;
static bool g_remoteshotflag = false;
static int decode_width = 0;
static int decode_height = 0;
static int decode_fps = 0;
static ucpaas_int8* enc_codecname = "H.264";
static ucpaas_int8* dec_codecname = "H.264";
static int32_t enc_pltype = 121;
static int32_t dec_pltype = 121;
static VideoCodecType enc_codectype = kVideoCodecH264;
static VideoCodecType dec_codectype = kVideoCodecH264;

static ScreenType g_screentype = RGB24;

Conductor* Conductor::m_pInstance = NULL;

Conductor* g_pVideoEngine = NULL;


#define NUMOFLOCALPORT      11  //num of local port spacing
#define NUMOFLOCALPORTUNIT 200  //num of one local port spacing

#define MAX_CAMERA_NAME_LENGTH 256

#define DEFAULT_CODEC_WIDTH         320
#define DEFAULT_CODEC_HEIGHT        240
#define DEFAULT_FRAME_RATE          15
#define DEFAULT_MIN_BITRATE         120
#define DEFAULT_MAX_BITRATE         400

//local port spacing
unsigned short ausLocalPort[NUMOFLOCALPORT] =
{ 15004, 20004, 25004, 30004, 35004, 40004, 45004, 50004, 55004, 60004, 65004 };

//cn packet
unsigned char g_ucRtpCnPkt[] = { 0x80, 0x0d, 0x02, 0x38, 0xed, 0xc0, 0xb8, 0x09, 0x70, 0x3c, 0x46, 0x50, 0x45, 0x86, 0x67, 0xa5, 0x8a, 0x7a, 0x76, 0x7b, 0x9c };

//keepalive packet
unsigned char g_ucRtpKeepAlivePkt[] = { 0xff };
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
#define MAX_PKT_CNT     2   //contain 0 index.
stPktInfo astPktInfo8K[] =
{
	{ 160, 40 * 8 * 50 },   //rtp rate every 20ms frame
	{ 320, 40 * 8 * 25 },   //rtp rate every 40ms frame    
	{ 480, 40 * 8 * 17 },   //rtp rate every 60ms frame    
};

stPktInfo astPktInfo16K[] =
{
	{ 320, 40 * 8 * 50 },   //rtp rate every 20ms frame
	{ 640, 40 * 8 * 25 },   //rtp rate every 40ms frame    
	{ 960, 40 * 8 * 17 },   //rtp rate every 60ms frame    
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

int Conductor::StartScreenShot(int islocal, ScreenType type)
{
	if (islocal)
		g_localshotflag = true;
	else
		g_remoteshotflag = true;

	g_screentype = type;
	return 0;
}
//add by zhanganlong
float Conductor::VideoCalcScale()
{
	float temp, tempwidth, tempheight;
	if (this->_vieRemoteVideoWidth == 0 || g_pVideoEngine->_stVideoRender.remoteHeight == 0 || g_pVideoEngine->_stVideoRender.remoteWidth == 0)
		return 1.0f;
	tempwidth = float(this->_vieRemoteVideoWidth);
	tempheight = float(this->_vieRemoteVideoHeight);
	_remoteConductor->rotation = this->_vieRecivedRotaton;
	_localConductor->rotation = this->_vieRecivedRotaton;
	if ((this->_vieRecivedRotaton + this->_vieRemoteSendRotation) % 180 != 0){
		temp = tempwidth;
		tempwidth = tempheight;
		tempheight = temp;
	}
	float x = g_pVideoEngine->_stVideoRender.remoteHeight - tempheight*g_pVideoEngine->_stVideoRender.remoteWidth / tempwidth;
	if (x >= 0)
	{
		x = g_pVideoEngine->_stVideoRender.remoteHeight*tempwidth / g_pVideoEngine->_stVideoRender.remoteWidth - tempheight;
		x = tempheight / (tempheight + x);
	}
	else
	{
		x = tempheight*g_pVideoEngine->_stVideoRender.remoteWidth / g_pVideoEngine->_stVideoRender.remoteHeight - tempwidth;
		x = -tempwidth / (tempwidth + x);
	}
	return x;
}


void RemoteConductor::FrameCallback(I420VideoFrame* video_frame)
{
	//printf("test");
	Clock*c = Clock::GetRealTimeClock();
	int ht = 0, numerator = 0;
	int64_t now = c->TimeInMilliseconds();
	video_frame->get_ht_rotation(ht, numerator);

	uint8_t* bufferY = video_frame->buffer(kYPlane);
	uint8_t* bufferU = video_frame->buffer(kUPlane);
	uint8_t* bufferV = video_frame->buffer(kVPlane);

	int stride_y = video_frame->stride(kYPlane);
	int stride_u = video_frame->stride(kUPlane);
	int stride_v = video_frame->stride(kVPlane);

	int width = video_frame->width();
	int height = video_frame->height();
	int width_origin = width;
	int height_origin = height;
	if (numerator >= 0)
	{
		height -= 2 * ht;
	}
	else{
		width -= 2 * ht;
	}
	decode_width = width;
	decode_height = height;
	

	if (now > previousTime + 1000)
	{
		fps++;
		decode_fps = fps;
		fps = 0;
		previousTime = now;
	}
	else
	{
		fps++;
	}

	if (g_remoteshotflag == false)
		return;

	if (rt_cb == NULL)
		return;
	int dst_stride = 0;
	//  bool iosplatfom=false;
	uint8_t* dst_argb = (uint8_t *)malloc(width   *  height * 4);
	uint8_t* rotate_Y = (uint8_t *)malloc(width   *  height);
	uint8_t* rotate_U = (uint8_t *)malloc(width / 2 * height / 2);
	uint8_t* rotate_V = (uint8_t *)malloc(width / 2 * height / 2);

	uint8_t* mirror_Y = (uint8_t *)malloc(width*height);
	uint8_t* mirror_U = (uint8_t *)malloc(width*height / 4);
	uint8_t* mirror_V = (uint8_t *)malloc(width*height / 4);
	//ROTATE  AND  REMOVE BLACK MARGIN
	switch (rotation)
	{
	case 0:
#ifdef ANDROID
		if (numerator >= 0){
			libyuv::RotatePlane(bufferY + ht*width,   stride_y, rotate_Y, \
			                width,   width,   height,   libyuv::kRotate0);
			libyuv::RotatePlane(bufferU + ht*width/4, stride_u, rotate_U, \
			                width/2, width/2, height/2, libyuv::kRotate0);
			libyuv::RotatePlane(bufferV + ht*width/4, stride_v, rotate_V, \
			                width/2, width/2, height/2, libyuv::kRotate0);
		}
		else
		{
			libyuv::RotatePlane(bufferY + ht,   stride_y, rotate_Y, width, \
			                width,   height,   libyuv::kRotate0);
			libyuv::RotatePlane(bufferU + ht/2, stride_u, rotate_U, width/2, \
			                width/2, height/2, libyuv::kRotate0);
			libyuv::RotatePlane(bufferV + ht / 2, stride_v, rotate_V, \
			                width / 2, width / 2, height / 2, libyuv::kRotate0);
		}
#else 
		if (numerator >= 0){
			libyuv::RotatePlane180(bufferY + ht*width, stride_y, mirror_Y, \
			                        width, width, height);
			libyuv::RotatePlane180(bufferU + ht*width / 4, stride_u, mirror_U, \
			                        width / 2, width / 2, height / 2);
			libyuv::RotatePlane180(bufferV + ht*width / 4, stride_v, mirror_V, \
			                        width / 2, width / 2, height / 2);
		}
		else
		{
			libyuv::RotatePlane180(bufferY + ht, stride_y, mirror_Y, \
			                        width, width, height);
			libyuv::RotatePlane180(bufferU + ht / 2, stride_u, mirror_U, \
			                        width / 2, width / 2, height / 2);
			libyuv::RotatePlane180(bufferV + ht / 2, stride_v, mirror_V, \
			                        width / 2, width / 2, height / 2);
		}
#ifdef WEBRTC_IOS

		libyuv::I420Mirror(mirror_Y, width, mirror_U, width/2, mirror_V, \
		                    width/2, rotate_Y, width, rotate_U, width/2, \
		                    rotate_V, width/2, width, height);
#else
		libyuv::RotatePlane(mirror_Y, width, rotate_Y, width, width, \
		                    height, libyuv::kRotate0);
		libyuv::RotatePlane(mirror_U, width / 2, rotate_U, width / 2, \
		                    width / 2, height / 2, libyuv::kRotate0);
		libyuv::RotatePlane(mirror_V, width / 2, rotate_V, width / 2, \
		                    width / 2, height / 2, libyuv::kRotate0);
#endif 
#endif 
		break;
	case 90:
#ifdef ANDROID
		if (numerator >= 0){
			libyuv::RotatePlane270(bufferY + ht*width, stride_y, rotate_Y, \
			                        height, width, height);
			libyuv::RotatePlane270(bufferU + ht*width / 4, stride_u, rotate_U, \
			                        height / 2, width / 2, height / 2);
			libyuv::RotatePlane270(bufferV + ht*width / 4, stride_v, rotate_V, \
			                        height / 2, width / 2, height / 2);
		}
		else{
			libyuv::RotatePlane270(bufferY + ht, stride_y, rotate_Y, \
			                        height, width, height);
			libyuv::RotatePlane270(bufferU + ht / 2, stride_u, rotate_U, \
			                        height / 2, width / 2, height / 2);
			libyuv::RotatePlane270(bufferV + ht / 2, stride_v, rotate_V, \
			                        height / 2, width / 2, height / 2);
		}
#else 
		if (numerator >= 0){
			libyuv::RotatePlane90(bufferY + ht*width, stride_y, mirror_Y, \
			                    height, width, height);
			libyuv::RotatePlane90(bufferU + ht*width / 4, stride_u, mirror_U, \
			                    height / 2, width / 2, height / 2);
			libyuv::RotatePlane90(bufferV + ht*width / 4, stride_v, mirror_V, \
			                    height / 2, width / 2, height / 2);
		}
		else{
			libyuv::RotatePlane90(bufferY + ht, stride_y, mirror_Y, \
			                height, width, height);
			libyuv::RotatePlane90(bufferU + ht / 2, stride_u, mirror_U, \
			                height / 2, width / 2, height / 2);
			libyuv::RotatePlane90(bufferV + ht / 2, stride_v, mirror_V, \
			                height / 2, width / 2, height / 2);
		}
		libyuv::I420Mirror(mirror_Y, height, mirror_U, height / 2, mirror_V, \
		                    height / 2, rotate_Y, height, rotate_U, height / 2, \
		                    rotate_V, height / 2, height, width);
#endif
		break;
	case 180:
#ifdef ANDROID
		if (numerator >= 0){
			libyuv::RotatePlane180(bufferY + ht*width, stride_y, rotate_Y, \
			                        width, width, height);
			libyuv::RotatePlane180(bufferU + ht*width / 4, stride_u, rotate_U, \
			                        width / 2, width / 2, height / 2);
			libyuv::RotatePlane180(bufferV + ht*width / 4, stride_v, rotate_V, \
			                        width / 2, width / 2, height / 2);
		}
		else
		{
			libyuv::RotatePlane180(bufferY + ht, stride_y, rotate_Y, \
			                        width, width, height);
			libyuv::RotatePlane180(bufferU + ht / 2, stride_u, rotate_U, \
			                        width / 2, width / 2, height / 2);
			libyuv::RotatePlane180(bufferV + ht / 2, stride_v, rotate_V, \
			                        width / 2, width / 2, height / 2);
		}
#else 
		if (numerator >= 0){
			libyuv::RotatePlane(bufferY + ht*width, stride_y, mirror_Y, \
			            width, width, height, libyuv::kRotate0);
			libyuv::RotatePlane(bufferU + ht*width / 4, stride_u, mirror_U, \
			            width / 2, width / 2, height / 2, libyuv::kRotate0);
			libyuv::RotatePlane(bufferV + ht*width / 4, stride_v, mirror_V, \
			            width / 2, width / 2, height / 2, libyuv::kRotate0);
		}
		else
		{
			libyuv::RotatePlane(bufferY + ht, stride_y, mirror_Y, width, \
			                width, height, libyuv::kRotate0);
			libyuv::RotatePlane(bufferU + ht / 2, stride_u, mirror_U, \
			                width / 2, width / 2, height / 2, libyuv::kRotate0);
			libyuv::RotatePlane(bufferV + ht / 2, stride_v, mirror_V, \
			                width / 2, width / 2, height / 2, libyuv::kRotate0);
		}
		libyuv::I420Mirror(mirror_Y, width, mirror_U, width / 2, mirror_V, \
		                    width / 2, rotate_Y, width, rotate_U, width / 2, \
		                    rotate_V, width / 2, width, height);
#endif
		break;
	case 270:
#ifdef ANDROID
		if (numerator >= 0){
			libyuv::RotatePlane90(bufferY + ht*width, stride_y, rotate_Y, \
			                        height, width, height);
			libyuv::RotatePlane90(bufferU + ht*width / 4, stride_u, rotate_U, \
			                        height / 2, width / 2, height / 2);
			libyuv::RotatePlane90(bufferV + ht*width / 4, stride_v, rotate_V, \
			                        height / 2, width / 2, height / 2);
		}
		else{
			libyuv::RotatePlane90(bufferY + ht, stride_y, rotate_Y, \
			                        height, width, height);
			libyuv::RotatePlane90(bufferU + ht / 2, stride_u, rotate_U, \
			                        height / 2, width / 2, height / 2);
			libyuv::RotatePlane90(bufferV + ht / 2, stride_v, rotate_V, \
			                        height / 2, width / 2, height / 2);
		}
#else
		if (numerator >= 0){
			libyuv::RotatePlane270(bufferY + ht*width, stride_y, mirror_Y, \
			                        height, width, height);
			libyuv::RotatePlane270(bufferU + ht*width / 4, stride_u, mirror_U, \
			                        height / 2, width / 2, height / 2);
			libyuv::RotatePlane270(bufferV + ht*width / 4, stride_v, mirror_V, \
			                        height / 2, width / 2, height / 2);
		}
		else{
			libyuv::RotatePlane270(bufferY + ht, stride_y, mirror_Y, \
			                        height, width, height);
			libyuv::RotatePlane270(bufferU + ht / 2, stride_u, mirror_U, \
			                        height / 2, width / 2, height / 2);
			libyuv::RotatePlane270(bufferV + ht / 2, stride_v, mirror_V, \
			                        height / 2, width / 2, height / 2);
		}
		libyuv::I420Mirror(mirror_Y, height, mirror_U, height / 2, mirror_V, \
		                    height / 2, rotate_Y, height, rotate_U, height / 2, \
		                    rotate_V, height / 2, height, width);
#endif
		break;
	default:
		break;
	}

	//CONVERT TO RGB
	if (rotation == 0 || rotation == 180){
		switch (g_screentype)
		{

		case ABGR:
			dst_stride = width * 4;
			libyuv::I420ToABGR(rotate_Y, width, rotate_U, width / 2, \
			        rotate_V, width / 2, dst_argb, dst_stride, width, height);
			break;

		case ARGB:
			dst_stride = width * 4;
			libyuv::I420ToARGB(rotate_Y, width, rotate_U, width / 2, \
			        rotate_V, width / 2, dst_argb, dst_stride, width, height);
			break;
		case RGB24:
			dst_stride = width * 3;
			libyuv::I420ToRGB24(rotate_Y, width, rotate_U, width / 2, \
			        rotate_V, width / 2, dst_argb, dst_stride, width, height);
			break;
		}
	}
	else
	{
		switch (g_screentype)
		{

		case ABGR:
			dst_stride = height * 4;
			libyuv::I420ToABGR(rotate_Y, height, rotate_U, height / 2, \
			        rotate_V, height / 2, dst_argb, dst_stride, height, width);
			break;

		case ARGB:
			dst_stride = height * 4;
			libyuv::I420ToARGB(rotate_Y, height, rotate_U, height / 2, \
			        rotate_V, height / 2, dst_argb, dst_stride, height, width);
			break;
		case RGB24:
			dst_stride = height * 3;
			libyuv::I420ToRGB24(rotate_Y, height, rotate_U, height / 2, \
			        rotate_V, height / 2, dst_argb, dst_stride, height, width);
			break;
		}
	}
	free(mirror_Y);
	free(mirror_U);
	free(mirror_V);
	free(rotate_Y);
	free(rotate_U);
	free(rotate_V);
	if (rotation == 0 || rotation == 180)
		rt_cb(dst_argb, dst_stride, width, height, 0, g_screentype);
	else
		rt_cb(dst_argb, dst_stride, height, width, 0, g_screentype);
	free(dst_argb);
	g_remoteshotflag = false;
}


void LocalConductor::FrameCallback(I420VideoFrame* video_frame)
{
	//printf("test LocalConductor");
	if (g_localshotflag == false)
		return;
	if (lc_cb == NULL)
		return;

	uint8_t* bufferY = video_frame->buffer(kYPlane);
	uint8_t* bufferU = video_frame->buffer(kUPlane);
	uint8_t* bufferV = video_frame->buffer(kVPlane);

	int stride_y = video_frame->stride(kYPlane);
	int stride_u = video_frame->stride(kUPlane);
	int stride_v = video_frame->stride(kVPlane);

	int width = video_frame->width();
	int height = video_frame->height();

	int dst_stride = 0;
	//  bool iosplatfom=false;

	uint8_t* dst_argb = (uint8_t *)malloc(width*height * 4);
	uint8_t* rotate_Y = (uint8_t *)malloc(stride_y*height);
	uint8_t* rotate_U = (uint8_t *)malloc(stride_u*height / 2);
	uint8_t* rotate_V = (uint8_t *)malloc(stride_v*height / 2);
	//
	//uint8_t* mirror_Y = (uint8_t *)malloc(stride_y*height);
	//uint8_t* mirror_U = (uint8_t *)malloc(stride_u*height / 2);
	//uint8_t* mirror_V = (uint8_t *)malloc(stride_v*height / 2);

	switch (rotation)
	{
	case 0:
#ifdef ANDROID
		libyuv::RotatePlane(bufferY, stride_y, rotate_Y, stride_y, \
		                        width, height, libyuv::kRotate0);
		libyuv::RotatePlane(bufferU, stride_u, rotate_U, stride_u, \
		                        width / 2, height / 2, libyuv::kRotate0);
		libyuv::RotatePlane(bufferV, stride_v, rotate_V, stride_v, \
		                        width / 2, height / 2, libyuv::kRotate0);
#else
		libyuv::RotatePlane180(bufferY, stride_y, rotate_Y, stride_y, \
		                        width, height);
		libyuv::RotatePlane180(bufferU, stride_u, rotate_U, stride_u, \
		                        width / 2, height / 2);
		libyuv::RotatePlane180(bufferV, stride_v, rotate_V, stride_v, \
		                        width / 2, height / 2);
#endif
		break;
	case 90:
#ifdef ANDROID
		libyuv::RotatePlane90(bufferY, stride_y, rotate_Y, height, \
		                        width, height);
		libyuv::RotatePlane90(bufferU, stride_u, rotate_U, height / 2, \
		                        width / 2, height / 2);
		libyuv::RotatePlane90(bufferV, stride_v, rotate_V, height / 2, \
		                        width / 2, height / 2);
#else
		libyuv::RotatePlane270(bufferY, stride_y, rotate_Y, height, \
		                        width, height);
		libyuv::RotatePlane270(bufferU, stride_u, rotate_U, height / 2, \
		                        width / 2, height / 2);
		libyuv::RotatePlane270(bufferV, stride_v, rotate_V, height / 2, \
		                        width / 2, height / 2);
#endif
		break;
	case 180:
#ifdef ANDROID
		libyuv::RotatePlane180(bufferY, stride_y, rotate_Y, stride_y, \
		                    width, height);
		libyuv::RotatePlane180(bufferU, stride_u, rotate_U, stride_u, \
		                    width / 2, height / 2);
		libyuv::RotatePlane180(bufferV, stride_v, rotate_V, stride_v, \
		                    width / 2, height / 2);
#else
		libyuv::RotatePlane(bufferY, stride_y, rotate_Y, stride_y, \
		                    width, height, libyuv::kRotate0);
		libyuv::RotatePlane(bufferU, stride_u, rotate_U, stride_u, \
		                    width / 2, height / 2, libyuv::kRotate0);
		libyuv::RotatePlane(bufferV, stride_v, rotate_V, stride_v, \
		                    width / 2, height / 2, libyuv::kRotate0);
#endif
		break;
	case 270:
#ifdef ANDROID
		libyuv::RotatePlane270(bufferY, stride_y, rotate_Y, height, \
		                        width, height);
		libyuv::RotatePlane270(bufferU, stride_u, rotate_U, height / 2, \
		                        width / 2, height / 2);
		libyuv::RotatePlane270(bufferV, stride_v, rotate_V, height / 2, \
		                        width / 2, height / 2);
#else
		libyuv::RotatePlane90(bufferY, stride_y, rotate_Y, height, \
		                        width, height);
		libyuv::RotatePlane90(bufferU, stride_u, rotate_U, height / 2, \
		                        width / 2, height / 2);
		libyuv::RotatePlane90(bufferV, stride_v, rotate_V, height / 2, \
		                        width / 2, height / 2);
#endif
		break;
	default:
		break;
	}

	if (rotation == 0 || rotation == 180){
		switch (g_screentype)
		{
		case ABGR:
			dst_stride = width * 4;
			libyuv::I420ToABGR(rotate_Y, stride_y, rotate_U, stride_u, \
			    rotate_V, stride_v, dst_argb, dst_stride, width, height);
			break;

		case ARGB:
			dst_stride = width * 4;
			libyuv::I420ToARGB(rotate_Y, stride_y, rotate_U, stride_u, \
			    rotate_V, stride_v, dst_argb, dst_stride, width, height);
			break;
		case RGB24:
			dst_stride = width * 3;
			libyuv::I420ToRGB24(rotate_Y, stride_y, rotate_U, stride_u, \
			    rotate_V, stride_v, dst_argb, dst_stride, width, height);
			break;
		}
	}
	else
	{
		switch (g_screentype)
		{

		case ABGR:
			dst_stride = height * 4;
			libyuv::I420ToABGR(rotate_Y, height, rotate_U, height / 2, \
			    rotate_V, height / 2, dst_argb, dst_stride, height, width);
			break;

		case ARGB:
			dst_stride = height * 4;
			libyuv::I420ToARGB(rotate_Y, height, rotate_U, height / 2, \
			    rotate_V, height / 2, dst_argb, dst_stride, height, width);
			break;
		case RGB24:
			dst_stride = height * 3;
			libyuv::I420ToRGB24(rotate_Y, height, rotate_U, height / 2, \
			    rotate_V, height / 2, dst_argb, dst_stride, height, width);
			break;
		}
	}

	free(rotate_Y);
	free(rotate_U);
	free(rotate_V);

	if (rotation == 0 || rotation == 180)
		lc_cb(dst_argb, dst_stride, width, height, 1, g_screentype);
	else
		lc_cb(dst_argb, dst_stride, height, width, 1, g_screentype);
	free(dst_argb);
	g_localshotflag = false;
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
	_voeExtMedia = NULL;
	_extMediaProc = NULL;
#endif // EXTERNAL_MEDIA_PROCESS

	_vieDefinition = emProfileLevel_None;

	_voiceChannelId = -1;
	_ptrThread = NULL;
	_recordDeviceNum = 0;
	_playDeviceNum = 0;

	_audioDeviceReady = eDEVICE_GET_FAIL;

	_recordMode = -1;  // default shutdown
	_recordFormat = 1; // default wav file
	_playFileDirect = -1;

	memset(&_voiceNetInfo, 0x00, sizeof(ME_net_calc_info_t));//added by luochengcong20141024
	memset(&_stEnvConfig, 0x00, sizeof(ME_ENV_cfg_t));//added by Rambo20141222
	memset(&_stVQEConfig, 0x00, sizeof(ME_VQE_cfg_t));
	memset(&_stCTRLConfig, 0x00, sizeof(ME_CTRL_cfg_t));

#ifdef EXTERNAL_AUDIO_DEVICE
	_extAudioDevice = NULL;
#endif

    _intVoiceTransportPtr = NULL;
	_intVideoTransportPtr = NULL;
#ifdef EXTERNAL_TRANSPORT
	_extVoiceTransportPtr = NULL;
    _extVideoTransportPtr = NULL;
#endif

#ifdef EXTERNAL_PLAYFILE
	_extPlayFile = NULL;
#endif
    _voeAudioTransport = NULL;

	_ptrTimeEvent = EventWrapper::Create();
	_critSect = CriticalSectionWrapper::CreateCriticalSection();
	_critSectAudio = CriticalSectionWrapper::CreateCriticalSection();
	_critSectVideo = CriticalSectionWrapper::CreateCriticalSection();

	_stCTRLConfig.ucRealTimeType = 0;
	_stCTRLConfig.ucVideoEnable = 0;
	_stCTRLConfig.ucEmodelEnable = 1;
	_stCTRLConfig.ucFecEnable = 0;//add by wuzhaoyang20140604
	_stCTRLConfig.ucIPv6Enable = 0;
    _stCTRLConfig.vieFecEnabled = false;
    _stCTRLConfig.vieNackEnabled = false;

	_stRTPConfig.uiRTPTimeout = COND_RTP_TIMEOUT_DEFAULT;
	_stRTPConfig.uiFixLowPayload = false;

    _stVQEConfig.Ec_enable = true;
	_stVQEConfig.Agc_enable = true;
	_stVQEConfig.Ns_enable = true;
    _stVQEConfig.Agc_targetDbfs = AGC_TARGET_DBOV_DEFAULT;
    _stVQEConfig.Agc_compressionGaindB = AGC_COMPRESSION_DB_DEFAULT;
    
	_curState = eME_done;
	_loadCnt = 0;
	_threadKeepAlive = true;
	_voeP2PRtt = 0;
	_voeRtppRtt = 0;
	_iceMode = eME_ICE_RTPP_MD;

	_consultFecEn = false;
	_consultRtppMultFec = false;//add by wuzhaoyang20151225

	/*reset audio stream param---rookie20140617*/
	ResetAudioStreamParam();

	_videoChannelID = -1;
	_videoCaptureID = -1;

	_vieSendRotation = 0;
	_vieRecivedRotaton = 0;
	_vieRemoteSendRotation = 0;
	_vieRemoteVideoWidth = 0;
	_vieRemoteVideoHeight = 0;
	_vieType = 0;
	_vieTypeSupposed = 0;
	memset(&_stVideoEnc, 0, sizeof(ME_video_enc_param_t));
	memset(&_stVideoDec, 0, sizeof(ME_video_dec_info_t));
	memset(&_stVideoRender, 0, sizeof(ME_video_render_param_t));
	memset(&_stVideoProces, 0, sizeof(ME_video_proces_module_t));
	memset(&_stVideoCamera, 0, sizeof(ME_video_camera_param_t));

	_usLastRate = 0;            //last stream rate contain codec and packet
	_usLastCodeRateIdx = 0;     //last codec rate index
	_usLastPktLenIdx = 0;       //last packet length index

	_smoothLossRate = 50;//add by wuzhaoyang20151225

	_vieExtCapture = NULL;

	_remoteConductor = new RemoteConductor();
	_localConductor = new LocalConductor();
}

Conductor::~Conductor(void)
{
	delete _ptrTimeEvent;
	delete _critSect;
	delete _critSectAudio;
	delete _critSectVideo;


	delete _remoteConductor;
	delete _localConductor;   
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
	_voeExtMedia = VoEExternalMedia::GetInterface(_voeEngine);
#endif // EXTERNAL_MEDIA_PROCESS

    // default level: report|error|warning
    uint32_t filter = kTraceReport | kTraceWarning | kTraceError;
	VoiceEngine::SetTraceFilter(filter);
	
#if defined(ANDROID)
	VoiceEngine::SetTraceFile("/mnt/sdcard/WebRtc_trace.txt");
#elif defined(_WIN32) && defined(WEBRTC_WRITE_TO_FILE)
	_voeEngine->SetTraceFile("WebRtc_trace.txt");
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

	_extVideoTransportPtr = new CExternalTransportImpl();
	if (_extVoiceTransportPtr == NULL)
	{
		TraceLog(kTraceError, "WebRTC! _extVideoTransportPtr handle null.");
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
	if (_extMediaProc == NULL)
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

	if (NULL == _voeEngine || NULL == _voeBase || NULL == _voeVqe || NULL == _voeCodec
		|| NULL == _voeHardware || NULL == _voeDtmf || NULL == _voeVolume || NULL == _voeRtp_Rtcp
		|| NULL == _voeFile || NULL == _voeNetwork || NULL == _voeNeteqStats
#ifdef EXTERNAL_ENCRYPT
		|| NULL == _voeEncrypt
#endif
#ifdef EXTERNAL_MEDIA_PROCESS
		|| NULL == _voeExtMedia
#endif // EXTERNAL_MEDIA_PROCESS
		)
	{
		TraceLog(kTraceError, "WebRTC! InitEngine handle null.");
		return -1;
	}

	/**add by Rookie John20120701 register callback */
	_voeBase->RegisterVoiceEngineObserver(*this);
   
	_videoEngine = VideoEngine::Create();
	_vieBase = ViEBase::GetInterface(_videoEngine);
	_vieCapture = ViECapture::GetInterface(_videoEngine);
	_vieRTPRTCP = ViERTP_RTCP::GetInterface(_videoEngine);
	_vieRender = ViERender::GetInterface(_videoEngine);
	_vieCodec = ViECodec::GetInterface(_videoEngine);
	_vieNetwork = ViENetwork::GetInterface(_videoEngine);
	//  _vieFile=ViEFile::GetInterface(_videoEngine);
	_vieImageProc = ViEImageProcess::GetInterface(_videoEngine);

	if ( NULL == _videoEngine || NULL == _vieBase 
	    || NULL == _vieCapture || NULL == _vieRTPRTCP 
	    || NULL == _vieRender || NULL == _vieCodec 
	    || NULL == _vieNetwork /*|| NULL == _vieFile */
	    || NULL == _vieImageProc)
	{
		TraceLog(kTraceError, "WebRTC! InitEngine handle null.");
		return -1;
	}

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
		if (-1 == iRet)
		{
			TraceLog(kTraceError, "InitEngine error\n");
			_audioDeviceReady = eDEVICE_GET_FAIL;
		}
		else
		{
			_audioDeviceReady = (eDevice_status)iRet;
		}

#ifdef _WIN32   
    	if (_voeHardware)
    	{
    		_voeHardware->GetNumOfRecordingDevices(_recordDeviceNum);
    		_voeHardware->GetNumOfPlayoutDevices(_playDeviceNum);
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

	int sdwRet;
	sdwRet = _vieBase->Init();
	TraceLog(kTraceReport, " _vieBase->Init()");
	if (sdwRet != 0)
	{
		TraceLog(kTraceError, "_vieBase->Init() failed");
		iRet = -2;
	}

	/* Initialize video device */
	VideoDefaultParam();

#ifdef EMODEL_ENABLED
	emd_init();
#endif

#ifdef ME_USE_EVENT_LIST
	/* Added by gonghuojin, 2013/9/3   PN:event list init */
	evt_init();
#endif

    /* add by Vinton.Liu on 20160301 for may dead lock when initialize*/
    /* Set trace callback must behind event queue created( evt_init() ) ,
           because Print() will call thread mute lock to push event queue. */
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
	return (_iceMode == eME_ICE_P2P_MD) ? _voeP2PRtt : _voeRtppRtt;
}

int Conductor::RatioRTT(int oRTT, int nRTT)
{
	int rtt = 0;

	if (oRTT == 0)
		rtt = nRTT;
	else
	{
		// Weighting of the old rtt value to new data.
		rtt = (RTT_RATIO * oRTT + nRTT) / (RTT_RATIO + 1);
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
		TraceLog(kTraceError, "RestartAudioStream _voiceChannelId error !");
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
		TraceLog(kTraceError, "webrtc StartReceive Failed!");
	}

	iRet = _voeBase->StartPlayout(_voiceChannelId);
	if (0 != iRet)
	{
		TraceLog(kTraceError, "webrtc StartPlayout Failed!");
	}

	iRet = _voeBase->StartSend(_voiceChannelId);
	if (0 != iRet)
	{
		_voeUpLinkSinglepassReason |= eME_REASON_START_SEND_FAIL;
		TraceLog(kTraceError, "webrtc StartSend Failed!");
	}

	iRet = _voeDtmf->SetSendTelephoneEventPayloadType(_voiceChannelId, 
                                                    DTMF_PAYLOAD_TYPE_DEFAULT);
	if (0 != iRet)
	{
		TraceLog(kTraceError, "SetSendTelephoneEventPayloadType Failed!");
	}

	return 0;
}
/* END:   Added by gonghuojin, 2014/1/2 */

int Conductor::RestartAudioStreamSend(void)//add by wuzhaoyang20140709
{
	int iRet;

	if (_voiceChannelId < 0)
	{
		TraceLog(kTraceError, "webrtc _voiceChannelId Failed!");
		return -1;
	}

	_voeBase->StopSend(_voiceChannelId);

	iRet = _voeBase->StartSend(_voiceChannelId);
	if (0 != iRet)
	{
		_voeUpLinkSinglepassReason |= eME_REASON_START_SEND_FAIL;
		TraceLog(kTraceError, "webrtc StartSend Failed!");
	}

	iRet = _voeDtmf->SetSendTelephoneEventPayloadType(_voiceChannelId, 
                                                    DTMF_PAYLOAD_TYPE_DEFAULT);
	if (0 != iRet)
	{
		TraceLog(kTraceError, "SetSendTelephoneEventPayloadType Failed!");
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
		return -1;

	eValue->delay.min = _voiceNetInfo.delay.min;
	eValue->delay.max = _voiceNetInfo.delay.max;
	eValue->delay.average = _voiceNetInfo.delay.average;
	eValue->jitbuf.min = _voiceNetInfo.jitbuf.min;
	eValue->jitbuf.max = _voiceNetInfo.jitbuf.max;
	eValue->jitbuf.average = _voiceNetInfo.jitbuf.average;

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

	if (strcasecmp("G729", inst.plname) == 0)
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
	else if (strcasecmp("AMR", inst.plname) == 0)
	{
		memcpy(pCodecRateAttr, iarAmrRate, sizeof(iarAmrRate));
		memcpy(pPktAttr, astPktInfo8K, sizeof(astPktInfo8K));
		max_codec_cnt = AMR_RATE_CNT;
		max_pkt_cnt = MAX_PKT_CNT;
#ifdef __INTER_DEBUG__
		printf("Current send Codec is AMR\n");
#endif
	}
	else if (strcasecmp("SILK", inst.plname) == 0)
	{
		memcpy(pCodecRateAttr, iarSilkRate, sizeof(iarSilkRate));
		memcpy(pPktAttr, astPktInfo8K, sizeof(astPktInfo8K));
		max_codec_cnt = SILK_RATE_CNT;
		max_pkt_cnt = MAX_PKT_CNT;
#ifdef __INTER_DEBUG__
		printf("Current send Codec is SILK\n");
#endif
	}
	else if (strcasecmp("SILKWB", inst.plname) == 0)
	{
		memcpy(pCodecRateAttr, iarSilkWBRate, sizeof(iarSilkWBRate));
		memcpy(pPktAttr, astPktInfo16K, sizeof(astPktInfo16K));
		max_codec_cnt = SILKWB_RATE_CNT;
		max_pkt_cnt = MAX_PKT_CNT;
#ifdef __INTER_DEBUG__
		printf("Current send Codec is SILKWB\n");
#endif
	}
	else if (strcasecmp("opus", inst.plname) == 0)
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
	/* Added by WeiYouquan in 2016.01.13*/
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
	else
	{
		TraceLog(kTraceError, "Don't exist codec in get rate: %s", inst.plname);
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

void Conductor::RegisterScreenshotCallback(me_screenshot_cb_f screenshot_cb)
{

	_remoteConductor->rt_cb = screenshot_cb;     /* remote screenshot callback*/
	_localConductor->lc_cb = screenshot_cb;      /* local screenshot callback*/
}

int Conductor::CallBackVtable(ME_cb_vtable_t* pvtable)
{
	if (pvtable != NULL)
	{
		memcpy(&g_cvtablecb, pvtable, sizeof(ME_cb_vtable_t));
		evt_callback((evt_cb_f)g_cvtablecb.event_cb);

#ifdef EXTERNAL_TRANSPORT
		if (g_cvtablecb.send_cb && _extVoiceTransportPtr)
			_extVoiceTransportPtr->RegisterSendCallback((exsend_data_cb_f)g_cvtablecb.send_cb);

		if (g_cvtablecb.send_cb && _extVideoTransportPtr)
			_extVideoTransportPtr->RegisterSendCallback((exsend_data_cb_f)g_cvtablecb.send_cb);
#endif

		RegisterScreenshotCallback(g_cvtablecb.screenshot_cb);

#ifdef EXTERNAL_MEDIA_PROCESS
		if (g_cvtablecb.extmedia_cb && _extMediaProc)
		{
			_extMediaProc->RegisterExternMediaProcessingCb((ex_media_proc_cb_f)g_cvtablecb.extmedia_cb);
		}
#endif // EXTERNAL_MEDIA_PROCESS

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

	}
	TraceLog(kTraceReport, "callback vtable register ok!");
	return 0;
}

void Conductor::CallbackOnError(const int channel, const int errCode)
{
	char strTmp[256] = { 0 };

	switch (errCode)
	{
	case VE_RECEIVE_PACKET_TIMEOUT:
		if (GetCurState() == eME_Connected)
		{
			_snprintf(strTmp, 256, "Channel[%d]RTP timeout!!!\n", channel);
			evt_appand(eME_RTP_TIMEOUT_EV, \
			    eME_REASON_RTP_TIMEOUT, strTmp, NULL, 0);
			TraceLog(kTraceError, "%s", strTmp);
		}
		break;
	case VE_PACKET_RECEIPT_STARTED:
	case VE_PACKET_RECEIPT_RESTARTED:
		if (GetCurState() != eME_Connected)
		{
			_snprintf(strTmp, 256, "Channel[%d]RTP recevied!!!\n", channel);
			evt_appand(eME_EARLY_MEDIA_EV, \
			    eME_REASON_EARLY_MEDIA, strTmp, NULL, 0);
			TraceLog(kTraceInfo, "%s", strTmp);
		}
		break;
	case VE_FILE_PLAY_TO_ENDED:
		if (GetCurState() == eME_Connected)
		{
			_snprintf(strTmp, 256, "Channel[%d] file play to end!!!\n", channel);
			evt_appand(eME_FILE_PLAY_TO_ENDED, \
			    eME_REASON_FILE_PLAY_TO_ENDED, strTmp, NULL, 0);
			TraceLog(kTraceInfo, "%s", strTmp);
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
			    
			TraceLog(kTraceError,"%s", strTmp); 
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
		TraceLog(kTraceInfo, "%s", strTmp);
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
		TraceLog(kTraceInfo, "%s", strTmp);
	}
	return;
}

//Channel callback
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
	stPktInfo   stPktTypeTbl[MAX_PKT_CNT + 1];
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

	CodecInst inst;
	memset(&inst, 0, sizeof(CodecInst));
	//get codec type and type number
	_voeCodec->GetSendCodec(_voiceChannelId, inst);

	if (_videoChannelID >= 0)   // Added by WeiYouquan in 2016.2.1
	{
		if (inst.pltype == 111 || inst.pltype == 115)
			usCurCodeRateIdx = 4;
		else
			usCurCodeRateIdx = 0;

		usCurCodeRate = stCodeTypeTbl[usCurCodeRateIdx].usRate;    // Cancelled by WeiYouquan in 2016.2.1 

		_voeCodec->SetBitRate(_voiceChannelId, usCurCodeRate);
		_usLastRate = usCurCodeRate;
		TraceLog(kTraceWarning, "change code bitrate from %d to %d", \
			stCodeTypeTbl[_usLastCodeRateIdx].usRate, usCurCodeRate);
		_usLastCodeRateIdx = usCurCodeRateIdx;

		//update emodel codec type
		UpdateEmodelCodecIeValue(stCodeTypeTbl[usCurCodeRateIdx].usRateType);
	}
	else
	{    
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
    	//set bitrate    
    	if (usCurCodeRateIdx != _usLastCodeRateIdx)
    	{
    		usCurCodeRate = stCodeTypeTbl[usCurCodeRateIdx].usRate;    // Cancelled by WeiYouquan in 2016.2.1 

    		_voeCodec->SetBitRate(_voiceChannelId, usCurCodeRate);
    		_usLastRate = usCurCodeRate;
    		TraceLog(kTraceWarning, "change code bitrate from %d to %d", \
    			stCodeTypeTbl[_usLastCodeRateIdx].usRate, usCurCodeRate);
    		_usLastCodeRateIdx = usCurCodeRateIdx;

    		//update emodel codec type
    		UpdateEmodelCodecIeValue(stCodeTypeTbl[usCurCodeRateIdx].usRateType);
    	}
	}

#if 0
	else if (usCurPktLenIdx != _usLastPktLenIdx)
	{
		usCurPktLen = stPktTypeTbl[usCurPktLenIdx].usLen;
		_voeCodec->SetPayloadLen(_voiceChannelId, usCurPktLen);
		TraceLog(kTraceWarning, "change rtp packet lenght from %d to %d", \
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
	/*if (level == kTraceError)
	{
		evt_appand(eME_LOG_TRACE_EV, eME_REASON_LOG_LEVEL_ERR, \
		    traceString, NULL, 0);
	}
	else  if (level == kTraceReport)
	{
		evt_appand(eME_LOG_TRACE_EV, eME_REASON_LOG_LEVEL_INFO, \
		    traceString, NULL, 0);
	}*/
}


/* Init video device, return 0 if succeed, else -1 */
int Conductor::VideoInit(void)
{
	int ret = -1;
	int i;
	char ucCameraUniqueName[256] = { 0 };
	char ucCameraCommonName[256] = { 0 };
	CriticalSectionScoped lock(_critSectVideo);

	if (_videoCaptureID >= 0)
		return 0;

	_vieExtCapture = NULL;

	if (_stVideoExtCapture.useExternCapture)
	{
		ret = _vieCapture->AllocateExternalCaptureDevice(_videoCaptureID, _vieExtCapture);
		return ret;
	}

	/* Check camera count */
	if (_vieCapture->NumberOfCaptureDevices() <= 0)
	{
		TraceLog(kTraceError, "VideoInit not find any capture devices.");
		return -1;
	}

	for (i = 0; i < _vieCapture->NumberOfCaptureDevices(); i++)
	{
		memset(ucCameraUniqueName, 0, sizeof(ucCameraUniqueName));
		_vieCapture->GetCaptureDevice(_stVideoCamera.usCameraIdx, 
		                            ucCameraCommonName, 256, 
		                            ucCameraUniqueName, 256);
		if (_vieCPM)
			_vieCPM->Release();
		_vieCPM = VideoCaptureFactory::Create(_stVideoCamera.usCameraIdx, ucCameraUniqueName);
		if (_vieCPM == NULL)
		{
			if (_stVideoCamera.usCameraIdx < _vieCapture->NumberOfCaptureDevices())
				_stVideoCamera.usCameraIdx++;

			_stVideoCamera.usCameraIdx = \
			    (_stVideoCamera.usCameraIdx == _vieCapture->NumberOfCaptureDevices()) ? 0 : _stVideoCamera.usCameraIdx;
			break;
		}

		ret = _vieCapture->AllocateCaptureDevice(*_vieCPM, _videoCaptureID);
		_vieCPM->AddRef();
		if (!ret)
		{
			TraceLog(kTraceError, "Video allocate capture %d [%s] successed.", \
			    _stVideoCamera.usCameraIdx, ucCameraUniqueName);
			return ret;
		}

		_stVideoCamera.usCameraIdx++;
		_stVideoCamera.usCameraIdx = \
		    (_stVideoCamera.usCameraIdx == _vieCapture->NumberOfCaptureDevices()) ? 0 : _stVideoCamera.usCameraIdx;
	}
	/* -1 means no camera device to use */
	_stVideoCamera.usCameraIdx = 0;
	_videoCaptureID = -1;
	TraceLog(kTraceReport, "Video init  successed.");

	return ret;
}

/* Release video resource all at once, return 0 if succeed, else -1 */
int Conductor::VideoRelease(void)
{
	int ret = 0;
	CriticalSectionScoped lock(_critSectVideo);
	/* Release capture device if needed */
	if (_videoCaptureID >= 0)
	{
		ret = _vieCapture->ReleaseCaptureDevice(_videoCaptureID);
		if (_vieCPM)
		{
			_vieCPM->Release();
			_vieCPM = NULL;
		}
		_videoCaptureID = -1;
	}
	_vieExtCapture = NULL;
	TraceLog(kTraceReport, "Realse _vieExtCapture ");
	return ret;
}

/* Configure render, return 0 if succeed, else -1 */
int Conductor::VideoConfigRender(void)
{
	CriticalSectionScoped lock(_critSectVideo);
	if (_stVideoRender.pWindowLocal && (_videoCaptureID >= 0))
	{
		//      _vieRender->ConfigureRender(_videoCaptureID, 0, _viePositionLocal.left, 
		//      _viePositionLocal.top, _viePositionLocal.right, _viePositionLocal.bottom);
		//
		//      _vieRender->MirrorRenderStream(_videoCaptureID, _viePositionLocal.enableMirrorX | _viePositionLocal.enableMirrorY, 
		//      _viePositionLocal.enableMirrorX, _viePositionLocal.enableMirrorY);
	}

	if (_stVideoRender.pWindowRemote && (_videoChannelID >= 0))
	{
		//      _vieRender->ConfigureRender(_videoChannelID, 0, _viePositionRemote.left, 
		//      _viePositionRemote.top, _viePositionRemote.right, _viePositionRemote.bottom);
		//
		//      _vieRender->MirrorRenderStream(_videoChannelID, _viePositionRemote.enableMirrorX | _viePositionRemote.enableMirrorY, 
		//      _viePositionRemote.enableMirrorX, _viePositionRemote.enableMirrorY);
	}

	return 0;
}


void Conductor::VideoPresetDefinition()
{
	CriticalSectionScoped lock(_critSectVideo);

	int ret;

	int capCount;
	char ucCameraUniqueName[256];
	char ucCameraCommonName[256];
	CaptureCapability captureCap;
	int numCamera;
	int i;

	_stVideoCamera.usWidth = 640;
	_stVideoCamera.usHeight = 480;

	_stVideoEnc.usWidth = 320;
	_stVideoEnc.usHeight = 240;


	switch (_vieDefinition)
	{
	case emProfileLevel_None:
		break;
	case emProfileLevel_Low:
#ifdef ANDROID 
		_stVideoCamera.usRotateAngle = -1;
		_stVideoDec.ucMaxFramerate = 30;
		_stVideoEnc.usWidth = 240;
		_stVideoEnc.usHeight = 320;
		_stVideoEnc.ucComplexity = 1;
		_stVideoEnc.usStartBitrate = 150;
		_stVideoDec.uiMaxDefinition = 640 * 480;
#endif

#ifdef WEBRTC_IOS 
		_stVideoEnc.ucComplexity = 1;
		_stVideoCamera.usRotateAngle = 0;
		_stVideoEnc.usWidth = 240;
		_stVideoEnc.usHeight = 320;
		_stVideoDec.ucMaxFramerate = 30;
		_stVideoEnc.usStartBitrate = 150;
		_stVideoDec.uiMaxDefinition = 640 * 480;
#endif

#ifdef WEBRTC_MAC 
		_stVideoCamera.usRotateAngle = 0;
		_stVideoEnc.usWidth = 320;
		_stVideoEnc.usHeight = 240;
		_stVideoDec.ucMaxFramerate = 30;
		_stVideoDec.uiMaxDefinition = 640 * 480;
#endif

#ifdef _WIN32 
		_stVideoCamera.usRotateAngle = 0;
		_stVideoEnc.usWidth = 320;
		_stVideoEnc.usHeight = 240;
		_stVideoDec.ucMaxFramerate = 30;
		_stVideoDec.uiMaxDefinition = 640 * 480;
#endif  
		break;

	case  emProfileLevel_Low_Andriod_0:
		_stVideoCamera.usRotateAngle = 0;
		_stVideoDec.ucMaxFramerate = 30;
		_stVideoEnc.ucComplexity = 1;
		_stVideoDec.uiMaxDefinition = 640 * 480;
		break;

	case emProfileLevel_Normal:
#ifdef ANDROID 
		_stVideoEnc.ucComplexity = 1;
		_stVideoCamera.usRotateAngle = -1;
		_stVideoEnc.usWidth = 360;
		_stVideoEnc.usHeight = 480;
		_stVideoDec.ucMaxFramerate = 30;
		_stVideoEnc.usStartBitrate = 200;
		_stVideoDec.uiMaxDefinition = 640 * 480;
#endif

#ifdef WEBRTC_IOS 
		_stVideoEnc.ucComplexity = 1;    
		_stVideoCamera.usRotateAngle = 0;
		_stVideoEnc.usWidth = 360;
		_stVideoEnc.usHeight = 480;
		_stVideoDec.ucMaxFramerate = 30;
		_stVideoEnc.usStartBitrate = 200;
		_stVideoDec.uiMaxDefinition = 640 * 480;
#endif

#ifdef WEBRTC_MAC 
		_stVideoCamera.usRotateAngle = 0;
		_stVideoEnc.usWidth = 480;
		_stVideoEnc.usHeight = 360;
		_stVideoDec.ucMaxFramerate = 30;
		_stVideoEnc.usStartBitrate = 200;
		_stVideoDec.uiMaxDefinition = 640 * 480;
#endif

#ifdef _WIN32 
		_stVideoCamera.usRotateAngle = 0;
		_stVideoEnc.usWidth = 480;
		_stVideoEnc.usHeight = 360;
		_stVideoDec.ucMaxFramerate = 30;
		_stVideoEnc.usStartBitrate = 200;
		_stVideoDec.uiMaxDefinition = 640 * 480;
#endif
		break;

	case emProfileLevel_Normal_Andriod_0:
		_stVideoEnc.ucComplexity = 1;
		_stVideoCamera.usRotateAngle = 0;
		_stVideoDec.ucMaxFramerate = 30;
		_stVideoDec.uiMaxDefinition = 640 * 480;
		break;

	case emProfileLevel_High:
#ifdef ANDROID 
		_stVideoEnc.ucComplexity = 1;
		_stVideoCamera.usRotateAngle = -1;
		_stVideoDec.ucMaxFramerate = 15;
		_stVideoEnc.usWidth = 480;
		_stVideoEnc.usHeight = 640;
		_stVideoEnc.usStartBitrate = 300;
		_stVideoDec.uiMaxDefinition = 640 * 480;
#endif
#ifdef WEBRTC_IOS 
		_stVideoEnc.ucComplexity = 1;
		_stVideoCamera.usRotateAngle = 0;
		_stVideoDec.ucMaxFramerate = 15;
		_stVideoEnc.usWidth = 480;
		_stVideoEnc.usHeight = 640;
		_stVideoEnc.usStartBitrate = 300;
		_stVideoDec.uiMaxDefinition = 640 * 480;
#endif

#ifdef WEBRTC_MAC 
		_stVideoCamera.usRotateAngle = 0;
		_stVideoEnc.usWidth = 640;
		_stVideoEnc.usHeight = 480;
		_stVideoDec.ucMaxFramerate = 30;
		_stVideoEnc.usStartBitrate = 300;
		_stVideoDec.uiMaxDefinition = 640 * 480;
#endif

#ifdef _WIN32 
		_stVideoCamera.usRotateAngle = 0;
		_stVideoEnc.usWidth = 640;
		_stVideoEnc.usHeight = 480;
		_stVideoDec.ucMaxFramerate = 30;
		_stVideoEnc.usStartBitrate = 300;
		_stVideoDec.uiMaxDefinition = 640 * 480;
#endif
		break;
	case emProfileLevel_1280720:
#ifdef _WIN32 
		_stVideoEnc.usWidth = 1280;
		_stVideoEnc.usHeight = 720;
		_stVideoEnc.ucMaxFramerate = 15;
		_stVideoEnc.usMaxBitrate = 600;
		_stVideoEnc.usMinBitrate = 300;
		_stVideoEnc.usStartBitrate = 500;
		_stVideoCamera.usWidth = 1280;
		_stVideoCamera.usHeight = 720;
		_stVideoCamera.usRotateAngle = 0;
		_stVideoDec.ucMaxFramerate = 30;
		_stVideoDec.uiMaxDefinition = 1280 * 720;
#endif
		break;
	default:
		break;
	}
    
#ifdef __APPLE__ 
	TraceLog(kTraceReport, "Set video capture definition  %d x %d \n", \
	    captureCap.width, captureCap.height);
	TraceLog(kTraceReport, "Set video encoder definition  %d x %d \n", \
	    _stVideoEnc.usWidth, _stVideoEnc.usHeight);
    return;
#endif

	numCamera = _vieCapture->NumberOfCaptureDevices();
	for (i = 0; i < numCamera; i++)
	{
		int j;
		int bget = 0;

		ret = _vieCapture->GetCaptureDevice(i, ucCameraCommonName, 256, ucCameraUniqueName, 256);
		if (ret != 0)
			return;

		capCount = _vieCapture->NumberOfCapabilities(ucCameraUniqueName, 256);

		for (j = 0; j < capCount; j++)
		{
			ret = _vieCapture->GetCaptureCapability(ucCameraUniqueName, 256, j, captureCap);

			
			if (captureCap.width == _stVideoCamera.usWidth  && captureCap.height == _stVideoCamera.usHeight)
			{
				bget = 1;
				break;
			}
		}
		if (bget == 0)
		{
			TraceLog(kTraceReport, "reset video definition\n");
#ifdef ANDROID 
			_stVideoEnc.usWidth = 240;
			_stVideoEnc.usHeight = 320;
			_stVideoCamera.usWidth = 320;
			_stVideoCamera.usHeight = 240;   
			if(_vieDefinition == emProfileLevel_Normal_Andriod_0)
			{
				_stVideoEnc.usWidth = 320;
				_stVideoEnc.usHeight = 240;
			}
#endif

#ifdef WEBRTC_IOS 
			_stVideoEnc.usWidth = 480;
			_stVideoEnc.usHeight = 640;
			_stVideoCamera.usWidth = 1280;
			_stVideoCamera.usHeight = 720;

#endif

#ifdef _WIN32 
			_stVideoEnc.usWidth = 320;
			_stVideoEnc.usHeight = 240;
			_stVideoCamera.usWidth = 320;
			_stVideoCamera.usHeight = 240;
#endif
		}

	}
	TraceLog(kTraceReport, "Set video capture definition  %d x %d \n", \
	    captureCap.width, captureCap.height);
	TraceLog(kTraceReport, "Set video encoder definition  %d x %d \n", \
	    _stVideoEnc.usWidth, _stVideoEnc.usHeight);
}
int Conductor::VideoPreset(char*key, char*value)
{
	if (key == NULL || value == NULL)
		return -1;
	if (strcmp(key, "definition") == 0)
	{
		if (strcmp(value, "low") == 0)
		{
			TraceLog(kTraceReport, "VideoPreset low\n");
			_vieDefinition = emProfileLevel_Low;
			VideoPresetDefinition();
			return 0;
		}

		if (strcmp(value, "low_andriod0") == 0)
		{
			TraceLog(kTraceReport, "VideoPreset low\n");
			_vieDefinition = emProfileLevel_Low_Andriod_0;
			VideoPresetDefinition();
			return 0;
		}

		if (strcmp(value, "normal") == 0)
		{
			TraceLog(kTraceReport, "VideoPreset normal\n");
			_vieDefinition = emProfileLevel_Normal;
			VideoPresetDefinition();
			return 0;
		}

		if (strcmp(value, "norma_android0l") == 0)
		{
			TraceLog(kTraceReport, "VideoPreset normal\n");
			_vieDefinition = emProfileLevel_Normal_Andriod_0;
			VideoPresetDefinition();
			return 0;
		}


		if (strcmp(value, "high") == 0)
		{
			TraceLog(kTraceReport, "VideoPreset high\n");
			_vieDefinition = emProfileLevel_High;
			VideoPresetDefinition();
			return 0;
		}

		if (strcmp(value, "1280x720") == 0)
		{
			TraceLog(kTraceReport, "VideoPreset 1280x720\n");
			_vieDefinition = emProfileLevel_1280720;
			VideoPresetDefinition();
			return 0;
		}

		return -3;
	}
	return -2;
}

#if defined(ANDROID)
int presets[3][3] = { { 0, 0, -1 }, { 1, 1, -1 }, { 5, 5, -1 } };
int bitrates[3][3] = { { 150, -1, -1 }, { 200, -1, -1 }, { 200, -1, -1 } };
int fps[3][3] = { { 8, 10, -1 }, { 12, 15, -1 }, { 15, 15, -1 } };
#elif defined(__APPLE__)
int presets[3][3] = { { 1, 1, -1 }, { 5, 5, 5 }, { 5, 5, 5 } };
int bitrates[3][3] = { { 200, -1, -1 }, { 200, 400, -1 }, { 200, 400, -1 } };
int fps[3][3] = { { 10, 10, -1 }, { 12, 15, 15 }, { 12, 15, 15 } };
#else
int presets[3][3] = { { 0, 0, -1 }, { 1, 1, -1 }, { 5, 5, -1 } };
int bitrates[3][3] = { { 150, -1, -1 }, { 200, -1, -1 }, { 200, -1, -1 } };
int fps[3][3] = { { 8, 10, -1 }, { 12, 15, -1 }, { 15, 15, -1 } };
#endif


/* Configure codec, return 0 if succeed, else -1 */
int Conductor::VideoConfigCodec(const unsigned char type/*0x01-enc, 0x02-dec*/)
{
	VideoCodec videoCodec;
	int numCodecs;
	int ret = -1;

	static const VideoCodecComplexity c[4] = { kComplexityNormal, kComplexityHigh, kComplexityHigher, kComplexityMax };

	CriticalSectionScoped lock(_critSectVideo);
	numCodecs = _vieCodec->NumberOfCodecs();
	for (int i = 0; i < numCodecs; i++)
	{
		if (0 == _vieCodec->GetCodec(i, videoCodec))
		{
			if (((_vieEncoderPt == videoCodec.plType) && (type & 0x01)) ||
				((_vieDecoderPt == videoCodec.plType) && (type & 0x02)))
			{
				/* Match ok */
				break;
			}
		}
	}

	videoCodec.maxFramerate = _stVideoEnc.ucMaxFramerate;

	/* Configure Codec */
	if (type & 0x01)
	{
		videoCodec.width = _stVideoEnc.usWidth;
		videoCodec.height = _stVideoEnc.usHeight;

		/*video encoder width and height should not be larger than remote reslution*/
		if ((videoCodec.maxFramerate > _vieRemoteFps) && (_vieRemoteFps > 0))
			videoCodec.maxFramerate = _vieRemoteFps;
	}


	videoCodec.startBitrate = _stVideoEnc.usStartBitrate;
	videoCodec.minBitrate = _stVideoEnc.usMinBitrate;
	videoCodec.maxBitrate = _stVideoEnc.usMaxBitrate;
	/*bit rate consultion*/
	/*if (videoCodec.startBitrate > _vieRemoteStartBitrate && _vieRemoteStartBitrate > 0) //起始码率发送端和接收端可以不对等
		videoCodec.startBitrate = _vieRemoteStartBitrate;  
	if (videoCodec.minBitrate > _vieRemoteMinBitrate && _vieRemoteMinBitrate > 0)
		videoCodec.minBitrate = _vieRemoteMinBitrate;
	if (videoCodec.maxBitrate > _vieRemoteMaxBitrate && _vieRemoteMaxBitrate > 0)
		videoCodec.maxBitrate = _vieRemoteMaxBitrate;*/ 

	if (videoCodec.codecType == kVideoCodecVP8)
	{
		videoCodec.codecSpecific.VP8.feedbackModeOn = true;
		videoCodec.codecSpecific.VP8.pictureLossIndicationOn = true;
		if (_stVideoEnc.ucComplexity < 0)
			_stVideoEnc.ucComplexity = 0;
		else if (_stVideoEnc.ucComplexity > 3)
			_stVideoEnc.ucComplexity = 3;
		videoCodec.codecSpecific.VP8.complexity = c[_stVideoEnc.ucComplexity];
	}
	else if (videoCodec.codecType == kVideoCodecH264)
	{
		if (_stVideoEnc.ucComplexity < 0)
			_stVideoEnc.ucComplexity = 0;
		else if (_stVideoEnc.ucComplexity > 3)
			_stVideoEnc.ucComplexity = 3;
		videoCodec.codecSpecific.H264.profile = c[_stVideoEnc.ucComplexity];
#if defined(ANDROID)
		videoCodec.codecSpecific.H264.isUseHwEnc = (_stVideoEnc.ucUseHwEnc == 1);
		videoCodec.codecSpecific.H264.isUseHwDec = (_stVideoEnc.ucUseHwDec == 1);
#elif defined(__APPLE__)
		videoCodec.codecSpecific.H264.isUseHwEnc = (_stVideoEnc.ucUseHwEnc == 1);
		videoCodec.codecSpecific.H264.isUseHwDec = (_stVideoEnc.ucUseHwDec == 1);
#else
		_stVideoEnc.ucUseHwEnc = 0;
		_stVideoEnc.ucUseHwDec = 0;
		videoCodec.codecSpecific.H264.isUseHwEnc = (_stVideoEnc.ucUseHwEnc == 1);
		videoCodec.codecSpecific.H264.isUseHwDec = (_stVideoEnc.ucUseHwDec == 1);
#endif
	}
	else if (videoCodec.codecType == kVideoCodecH265)
	{
		if (_stVideoEnc.ucComplexity < 0)
			_stVideoEnc.ucComplexity = 0;
		else if (_stVideoEnc.ucComplexity > 3)
			_stVideoEnc.ucComplexity = 3;
		videoCodec.codecSpecific.H265.profile = c[_stVideoEnc.ucComplexity];
#if defined(ANDROID)
		videoCodec.codecSpecific.H265.isUseHwEnc = (_stVideoEnc.ucUseHwEnc == 1);
		videoCodec.codecSpecific.H265.isUseHwDec = (_stVideoEnc.ucUseHwDec == 1);
#elif defined(__APPLE__)
		_stVideoEnc.ucUseHwEnc = 0;
		_stVideoEnc.ucUseHwDec = 0;
		videoCodec.codecSpecific.H265.isUseHwEnc = (_stVideoEnc.ucUseHwEnc == 1);
		videoCodec.codecSpecific.H265.isUseHwDec = (_stVideoEnc.ucUseHwDec == 1);
#else
		_stVideoEnc.ucUseHwEnc = 0;
		_stVideoEnc.ucUseHwDec = 0;
		videoCodec.codecSpecific.H265.isUseHwEnc = (_stVideoEnc.ucUseHwEnc == 1);
		videoCodec.codecSpecific.H265.isUseHwDec = (_stVideoEnc.ucUseHwDec == 1);
#endif
	}

	if(_stVideoEnc.enc_param_t.bitrates[0][0]==0)
	{
		memcpy(videoCodec.encParam_t.bitrates, bitrates, sizeof(bitrates));
	}
	else
	{
		memcpy(videoCodec.encParam_t.bitrates, _stVideoEnc.enc_param_t.bitrates, sizeof(_stVideoEnc.enc_param_t.bitrates));
	}

	if(_stVideoEnc.enc_param_t.presets[0][0]==0)
	{
		memcpy(videoCodec.encParam_t.presets, presets, sizeof(presets));
	}
	else
	{
		memcpy(videoCodec.encParam_t.presets, _stVideoEnc.enc_param_t.presets, sizeof(_stVideoEnc.enc_param_t.presets));
	}

	if(_stVideoEnc.enc_param_t.fps[0][0]==0)
	{
		memcpy(videoCodec.encParam_t.fps, fps, sizeof(fps));
	}
	else
	{
		memcpy(videoCodec.encParam_t.fps, _stVideoEnc.enc_param_t.fps, sizeof(_stVideoEnc.enc_param_t.fps));
	}

	if (type & 0x01)
	{
		_vieRTPRTCP->SetEncoderResolution(_videoChannelID, videoCodec.width, videoCodec.height);
		_vieRTPRTCP->SetBitRatesAndFrameRate(_videoChannelID, videoCodec.startBitrate, videoCodec.maxFramerate);
	}
	/* Set enable */
	if (type & 0x01)
		ret = _vieCodec->SetSendCodec(_videoChannelID, videoCodec);
	else if (type & 0x02)
		ret = _vieCodec->SetReceiveCodec(_videoChannelID, videoCodec);
    
	return ret;
}
VideoRotation Conductor::VideoGetRotate(int r)
{

	switch (r)
	{
	case 0:
		return kVideoRotation_0;
	case 90:
		return  kVideoRotation_90;
	case 180:
		return  kVideoRotation_180;
	case 270:
		return  kVideoRotation_270;
	default:
#ifdef ANDROID 
    	if(_stVideoCamera.usCameraIdx == 0)
    		return kVideoRotation_90;
    	return kVideoRotation_270;
#endif
		return kVideoRotation_0;

	}

}

/* Start video, interface also for user, return 0 if succeed */
int Conductor::VideoStart(const unsigned int type1)
{
	int ret = 0;
	unsigned int type;

	CriticalSectionScoped lock(_critSectVideo);

	/* Parameter check */
	if (type1 > 31)
	{
		TraceLog(kTraceError, "Parameter error\n");
		return ret;
	}

	if (_videoChannelID < 0)
	{
		TraceLog(kTraceError, "VideoStart error:_videoChannelID not created\n");
		return ret;
	}

	TraceLog(kTraceReport, "Video start  %d", type1);

	type = type1 | _vieTypeSupposed;
	_vieTypeSupposed = type;

//	if (_videoChannelID >= 0)
//		_vieCodec->RotateDecodedFrame(_videoChannelID, _vieRecivedRotaton);

	if (_videoCaptureID < 0)
	{
		if (VideoInit() == -1 && (type & (0x01)))
		{
			evt_appand(eME_VIDEO_EV, eME_REASON_OPEN_CAMERA_FAILED, \
			    "Open camera failed.", NULL, 0);
			_vieUpLinkSinglepassReason |= eME_REASON_OPEN_CAMERA_FAILED;
		}
		
		if (_videoCaptureID >= 0)
		{
			_vieCapture->ConnectCaptureDevice(_videoCaptureID, _videoChannelID);
		}
	}

	if ((type & (0x01)) && ((_vieType & (0x01)) == 0) && (_videoCaptureID >= 0))
	{
		TraceLog(kTraceReport, "Video start camera(%d) (%d) for android", \
		    _stVideoCamera.usCameraIdx, _stVideoCamera.usRotateAngle);

		VideoRotation  rf = VideoGetRotate(_stVideoCamera.usRotateAngle);

		/* Configure capture parameters */
		CaptureCapability captureCap;
        captureCap.width = _stVideoCamera.usWidth;
        captureCap.height = _stVideoCamera.usHeight;
        captureCap.maxFPS = _stVideoCamera.ucMaxFps;
		ret = _vieCapture->StartCapture(_videoCaptureID, captureCap);
		_vieCapture->SetVideoRotation(_videoCaptureID, rf);
		_vieType |= 0x01;
	}


	/* bit1=1, Start send video stream */
	if ((type & 0x02) && (_videoChannelID >= 0))
	{
		ret = _vieBase->StartSend(_videoChannelID);
		_vieType |= 0x02;
		TraceLog(kTraceReport, "Videostart  send ");
	}

	/* bit2=1, Start receive video stream */
	if ((type & 0x04) && (_videoChannelID >= 0))
	{
		ret = _vieBase->StartReceive(_videoChannelID);
		_vieType |= 0x04;
		TraceLog(kTraceReport, "Videostart  receive ");
	}

#ifndef ANDROID
	if (_videoCaptureID >= 0)
	{
		_vieRender->StopRender(_videoCaptureID);
		_vieRender->RemoveRenderer(_videoCaptureID);
	}

	if (_videoChannelID >= 0)
	{
		_vieRender->StopRender(_videoChannelID);
		_vieRender->RemoveRenderer(_videoChannelID);
	}
#endif
#ifdef __APPLE__
	if(_stVideoRender.pWindowLocal)
	{
		resetVideoRenderView(_stVideoRender.pWindowLocal);
	}
	if(_stVideoRender.pWindowRemote)
	{
		resetVideoRenderView(_stVideoRender.pWindowRemote);
	}
#endif


	/* bit0=1, Start Capture and Render */
	if ((type & 0x08) && (_videoCaptureID >= 0))
	{
		TraceLog(kTraceReport, "Videostart local render");
#ifndef ANDROID
		_vieRender->AddRenderer(_videoCaptureID, _stVideoRender.pWindowLocal, 0, _viePositionLocal.left,
			_viePositionLocal.top, _viePositionLocal.right, _viePositionLocal.bottom);
		ret = _vieRender->StartRender(_videoCaptureID);
#endif
		_vieType |= 0x08;
	}

	/* bit2=1, Start remote video stream render */
	if ((type & 0x10) && (_videoChannelID >= 0))
	{
#ifndef ANDROID
		if (_stVideoRender.pWindowRemote)
		{
			_vieRender->AddRenderer(_videoChannelID, _stVideoRender.pWindowRemote, 0, _viePositionRemote.left,
				_viePositionRemote.top, _viePositionRemote.right, _viePositionRemote.bottom);
		}
#endif
		if (_videoChannelID >= 0 && _stVideoRender.renderMode == 1)
		{
			float scale = VideoCalcScale();
			_vieCodec->SetDecodeFrame(_videoChannelID, scale);
		}
		ret = _vieRender->StartRender(_videoChannelID);
		_vieType |= 0x10;
		TraceLog(kTraceReport, "Videostart remote render");
	}

    if ( _vieStreamInfo.iMediaSignalEnable )
    {
        VideoSendStatePackets(2);
    }
    
	_vieRemoteSendRotation = 360;
	_vieRecivedRotaton = 360;
	_vieSendRotation = 360;

	return ret;
}

/* Stop video, interface also for user, return 0 if succeed */
int Conductor::VideoStop(const unsigned int type1)
{
	int ret = -1;
	unsigned int type;

	CriticalSectionScoped lock(_critSectVideo);

	/* Parameter check */
	if (type1 > 31)
	{
		TraceLog(kTraceError, "Parameter error\n");
		return ret;
	}

	TraceLog(kTraceReport, "Videostop  type:%d", type1);

	type = type1&_vieTypeSupposed;
	_vieTypeSupposed -= type;
	type = type1&_vieType;
	_vieType -= type;

    if ( _vieStreamInfo.iMediaSignalEnable )
    {
        VideoSendStatePackets(2);
    }
    
	/* bit0=1, Stop Capture*/
	if ((type & 0x01) && (_videoCaptureID >= 0))
	{

		ret = _vieCapture->StopCapture(_videoCaptureID);
		TraceLog(kTraceReport, "Videostop caputure");
	}

	if (_videoChannelID < 0)
		return ret;

	/* bit1=1, Stop send video stream */
	if (type & 0x02)
	{
		ret = _vieBase->StopSend(_videoChannelID);
		TraceLog(kTraceReport, "Videostop send");
	}

	/* bit2=1, Start receive video stream */
	if (type & 0x04)
	{
		ret = _vieBase->StopReceive(_videoChannelID);
		TraceLog(kTraceReport, "Videostop receive");
	}

	if (type & 0x08)
	{
		ret = _vieRender->StopRender(_videoCaptureID);
		TraceLog(kTraceReport, "Videostop local render");
	}
	if (type & 0x10)
	{
		ret = _vieRender->StopRender(_videoChannelID);
		TraceLog(kTraceReport, "Videostop remote render");
	}

	return ret;
}

void Conductor::UpdateVideoProtectionMethod()
{
    if ( _vieStreamInfo.vieFecEnabled && _vieStreamInfo.vieNackEnabled )
    {
        _vieRTPRTCP->SetHybridNACKFECStatus(_videoChannelID, true, 96, 97);
    }
    else if ( _vieStreamInfo.vieFecEnabled )
    {
        _vieRTPRTCP->SetFECStatus(_videoChannelID, true, 96, 97);
    }
    else if ( _vieStreamInfo.vieNackEnabled )
    {
        _vieRTPRTCP->SetNACKStatus(_videoChannelID, true);
    }
    else
    {
        _vieRTPRTCP->SetHybridNACKFECStatus(_videoChannelID, false, 96, 97);
    }
}

/* Set video stream, return 0 if succeed, else -1 */
int Conductor::VideoSetStream(const ME_video_stream_info_t* pVideoInfo)
{
	int ret = -1;
    int local_port  = 0;
    int remote_port = 0;
    char remote_ip[64] = {0};
	/*
	1. Net
	2. Codec
	3. Render
	*/
	/* Parameter check */
	if (NULL == pVideoInfo)
	{
		TraceLog(kTraceError, "No valid parameters");
		return -1;
	}
	/* No camera device, we can still receive video stream, so check channel ID only */
	if (_videoChannelID < 0)
	{
		TraceLog(kTraceError, "No video channel created");
		return -1;
	}

    CriticalSectionScoped lock(_critSectVideo);

    local_port = _vieStreamInfo.iLocalVideoPort;
    remote_port = _vieStreamInfo.iRemoteVideoPort;
    memcpy(remote_ip, _vieStreamInfo.cRemoteVideoIp, sizeof(char) * 64);
    // save video stream param info
    memcpy(&_vieStreamInfo, pVideoInfo, ME_VIDEO_STREAM_INFO_ST_SIZE);
    
    
	_vieRTPRTCP->RegisterRTPObserver(_videoChannelID, *this);
    
	_vieRTPRTCP->SetKeyFrameRequestMethod(_videoChannelID, kViEKeyFrameRequestPliRtcp);
	
	_vieRTPRTCP->SetRembStatus(_videoChannelID, true, true); //yangjj add 2016-1-27

    // reserved 50kbps for voice channel
    _vieRTPRTCP->SetReservedTransmitBitrate(_videoChannelID, 50000);

    UpdateVideoProtectionMethod();
        
	/* Configure network parameters */
	{
#ifdef EXTERNAL_TRANSPORT
		if (pVideoInfo->ucExTransportEnable)
		{
			TraceLog(kTraceReport, "video pvideoinfo->ucExTransportEnable enabled");
			_vieNetwork->DeregisterSendTransport(_videoChannelID);//yangjj add
			ret = _extVideoTransportPtr->SetVideoChannel(_videoChannelID);
			ret = _vieNetwork->RegisterSendTransport(_videoChannelID, *_extVideoTransportPtr);
			if (0 != ret)
			{
				ret = _voeBase->LastError();
				TraceLog(kTraceError, \
				    "video webrtc RegisterExternalTransport Failed!, error = %d", ret);
				 
                if (local_port != pVideoInfo->iLocalVideoPort)
                {
                  /* Warning, External transport register error, then using internal */
                  local_port = SetLocalReceiver(1, _videoChannelID, pVideoInfo->iLocalVideoPort);
                  if (local_port < 0)
                  {
                    TraceLog(kTraceError, "VideoSetStream SetLocalReceiver Failed!");
                    return -1;
                  }
                  else if (local_port > 0)
                  {
                    _vieStreamInfo.iLocalVideoPort = local_port;
                  }
                }

                if (remote_port != pVideoInfo->iRemoteVideoPort
                  || strcmp(remote_ip, pVideoInfo->cRemoteVideoIp) != 0)
                {
                  ret = _intVideoTransportPtr->SetSendDestination(pVideoInfo->cRemoteVideoIp, pVideoInfo->iRemoteVideoPort);
                  if (0 != ret)
                  {
                    TraceLog(kTraceError, \
                      "webrtc VideoSetStream(%s:%d) Failed(Error = %d)!", \
                      pVideoInfo->cRemoteVideoIp, pVideoInfo->iRemoteVideoPort, \
                      _intVideoTransportPtr->LastError());
                  }
                }
			}
		}
		else
#endif
		{
          if (local_port != pVideoInfo->iLocalVideoPort)
          {
            local_port = SetLocalReceiver(1, _videoChannelID, pVideoInfo->iLocalVideoPort);
            if (local_port > 0)
            {
              _vieStreamInfo.iLocalVideoPort = local_port;
            }
          }
			
          if (remote_port != pVideoInfo->iRemoteVideoPort 
            || strcmp(remote_ip, pVideoInfo->cRemoteVideoIp) != 0)
          {
            ret = _intVideoTransportPtr->SetSendDestination(pVideoInfo->cRemoteVideoIp, pVideoInfo->iRemoteVideoPort);
            if (0 != ret)
            {
              TraceLog(kTraceError, \
                "webrtc VideoSetStream(%s:%d) Failed(Error = %d)!", \
                pVideoInfo->cRemoteVideoIp, pVideoInfo->iRemoteVideoPort, \
                _intVideoTransportPtr->LastError());
            }
          }
		}
	}
    
	/* Codec parameter was not match with sdp result, then reconfigure sender codec */
	_vieEncoderPt = pVideoInfo->iPayLoadType;
	_vieDecoderPt = pVideoInfo->iPayLoadType;
	VideoConfigCodec(0x01);

	/* Receive codec */
	VideoConfigCodec(0x02);

	//--niyuanxun add
	if (_vieStreamInfo.vieFecEnabled)
	{
		//Register RED PayloadType
		_vieDecoderPt = 96;
		VideoConfigCodec(0x02);

		//Register UEPFEC PayloadType
		_vieDecoderPt = 97;
		VideoConfigCodec(0x02);
		_vieDecoderPt = pVideoInfo->iPayLoadType;
	}
	//--end


	/* Render */
	VideoConfigRender();

	if (!ret)
		return ret;
	else
		return _vieBase->LastError();
}

/* Create video stream, return 0 if succeed, else -1 */
int Conductor::VideoCreateStream(void)
{
	int ret = -1;

    _vieUpLinkSinglepassReason = 0;
    _vieDnLinkSinglepassReason = 0;
    
	/*
	1. Allocate video device, create video channel
	2. Synchronize audio channel
	3. Connect capture device
	4. Create render
	5. Register observer
	*/
	CriticalSectionScoped lock(_critSectVideo);
	VideoInit();
	memset(&_vieStreamInfo, 0, ME_VIDEO_STREAM_INFO_ST_SIZE);

	if (_videoChannelID < 0)
	{
		ret = _vieBase->CreateChannel(_videoChannelID);

		/* There are no more resources for video channel, return error */
		if (ret != 0)
		{
			TraceLog(kTraceError, "There are no more resources for video channel\n");
			return ret;
		}

		_vieImageProc->RegisterPreEncodeCallback(_videoChannelID, (I420FrameCallback*)_localConductor);
		_vieImageProc->RegisterPreRenderCallback(_videoChannelID, (I420FrameCallback*)_remoteConductor);

		bool ipv6Enabled = _stCTRLConfig.ucIPv6Enable == 0 ? false : true;
        _intVideoTransportPtr = new VideoChannelTransport(_vieNetwork, 
                                                  _videoChannelID, ipv6Enabled);

		/* Synchonize audio and video */
		_vieBase->SetVoiceEngine(_voeEngine);
		if (_voiceChannelId >= 0)
			_vieBase->ConnectAudioChannel(_videoChannelID, _voiceChannelId);

		/* Add render for remote and local window */
		{

#ifdef __APPLE__
			if(_stVideoRender.pWindowLocal && (_videoCaptureID >= 0))
			{
				_vieRender->AddRenderer(_videoCaptureID, _stVideoRender.pWindowLocal, 0, _viePositionLocal.left, 
					_viePositionLocal.top, _viePositionLocal.right, _viePositionLocal.bottom);
			}

			if(_stVideoRender.pWindowRemote)
			{
				_vieRender->AddRenderer(_videoChannelID, _stVideoRender.pWindowRemote, 0, _viePositionRemote.left, 
					_viePositionRemote.top, _viePositionRemote.right, _viePositionRemote.bottom);
			}
#endif
#ifdef ANDROID
			_stVideoRender.pWindowRemote = NULL;
			_stVideoRender.remoteHeight = 0;
			_stVideoRender.remoteWidth = 0;
			_stVideoRender.renderMode = 0;
			if ( _stVideoRender.pWindowRemote )
			{
				_vieRender->AddRenderer(_videoChannelID, _stVideoRender.pWindowRemote, 0, _viePositionRemote.left, 
					_viePositionRemote.top, _viePositionRemote.right, _viePositionRemote.bottom);
			}
#endif

#ifdef _WIN32
			if (_stVideoRender.pWindowLocal && (_videoCaptureID >= 0))
			{
				_vieRender->AddRenderer(_videoCaptureID, _stVideoRender.pWindowLocal, 0, _viePositionLocal.left,
					_viePositionLocal.top, _viePositionLocal.right, _viePositionLocal.bottom);
			}

			if (_stVideoRender.pWindowRemote)
			{
				_vieRender->AddRenderer(_videoChannelID, _stVideoRender.pWindowRemote, 0, _viePositionRemote.left,
					_viePositionRemote.top, _viePositionRemote.right, _viePositionRemote.bottom);
			}
#endif
		}

		/* Connect capture device */
		if (_videoCaptureID >= 0)
			ret = _vieCapture->ConnectCaptureDevice(_videoCaptureID, _videoChannelID);
	}

	if (!_videoChannelID)
	{
		//      ret = _vieCodec->RegisterDecoderObserver(_videoChannelID, *this);
		if (!ret)
			ret = _vieCodec->SetKeyFrameRequestCallbackStatus(_videoChannelID, TRUE);
	}

	//uiRemoteSendRotation = 0;
	//uiRecivedRotaton = 0;
	//uiSendRotation = 0;

	if (!ret)
		return ret;
	else
		return _vieBase->LastError();
}

/* Delete video stream, return 0 if succeed, else -1 */
int Conductor::VideoDeleteStream(void)
{
	int ret = 0;
	CriticalSectionScoped lock(_critSectVideo);

	if (_videoChannelID >= 0)
	{
		VideoStop(31);

		delete _intVideoTransportPtr;
		_intVideoTransportPtr = NULL;

		if (_videoCaptureID >= 0)
			_vieRender->RemoveRenderer(_videoCaptureID);
		_vieRender->RemoveRenderer(_videoChannelID);

		_stVideoRender.pWindowLocal = NULL;
		_stVideoRender.pWindowRemote = NULL;


		if (_videoCaptureID >= 0)
			_vieCapture->DisconnectCaptureDevice(_videoChannelID);

		//VideoRelease();

		_vieBase->DisconnectAudioChannel(_videoChannelID);

		ret = _vieBase->DeleteChannel(_videoChannelID);

		_videoChannelID = -1;
	}
	VideoRelease();

	_vieRemoteRes = 0;
	_vieRemoteFps = 0;
	_vieRemoteStartBitrate = 0;
	_vieRemoteMinBitrate = 0;
	_vieRemoteMaxBitrate = 0;
	_vieType = 0;
	_vieTypeSupposed = 0;
	_vieDnLinkSinglepassReason = 0;
	_vieUpLinkSinglepassReason = 0;

	if (!ret)
		return ret;
	return _vieBase->LastError();
}

/* Set video device, return 0 if succeed, else -1 */
int Conductor::VideoSetCamera(const ME_video_camera_param_t* param)
{
	int ret;
	CaptureCapability captureCap;
	char ucCameraUniqueName[256];
	char ucCameraCommonName[256];
	CriticalSectionScoped lock(_critSectVideo);

	if (_vieCapture->NumberOfCaptureDevices() < 0)
	{
		TraceLog(kTraceError, "Faild on capture devices number(%d).", \
			_vieCapture->NumberOfCaptureDevices());
		return -2;
	}

	/* To avoid changed camera devices count */
	if (param->usCameraIdx >= _vieCapture->NumberOfCaptureDevices())
	{
		TraceLog(kTraceError, \
		    "Faild on CameraIdx(%d) over capture devices number(%d).", \
			param->usCameraIdx, _vieCapture->NumberOfCaptureDevices());
		return -1;
	}

	if (param->ucMaxFps > 0)
	{
		_stVideoCamera.ucMaxFps = param->ucMaxFps;
	}

	if (param->usCameraIdx >= 0)
		_stVideoCamera.usCameraIdx = param->usCameraIdx;


	if (param->usHeight > 0)
	{
		_stVideoCamera.usHeight = param->usHeight;
	}

	if (param->usWidth > 0)
	{
		_stVideoCamera.usWidth = param->usWidth;
	}

	_stVideoCamera.usRotateAngle = param->usRotateAngle;


	if (GetCurState() != eME_Connected)
	{
		return 0;
	}
	else if (_stVideoExtCapture.useExternCapture)
	{
		return 0;
	}
	else
	{

#ifdef __APPLE__
		if((_vieType & 0x01) == 0)
		{
			TraceLog(kTraceError, "VideoSetCamera:  capture is not running\n");
			return 0;
		}
		_vieCapture->GetCaptureDevice(_stVideoCamera.usCameraIdx, ucCameraCommonName, 256, ucCameraUniqueName, 256);
		captureCap.width = _stVideoCamera.usWidth;
		captureCap.height = _stVideoCamera.usHeight;
		captureCap.maxFPS = _stVideoCamera.ucMaxFps;
		captureCap.bRestart = 1;
		captureCap.uniqueIdUTF8 = ucCameraUniqueName;
		captureCap.uniqueIdUTF8Length = strlen(ucCameraUniqueName);
		_vieCapture->StartCapture(_videoCaptureID, captureCap);

		/* Stop Render */
		_vieRender->StopRender(_videoCaptureID);
		/* Remove Render */
		_vieRender->RemoveRenderer(_videoCaptureID);

		if(_stVideoRender.pWindowLocal)
		{
			resetVideoRenderView(_stVideoRender.pWindowLocal);
		}

		/* Stop Render */
		_vieRender->StopRender(_videoChannelID);
		/* Remove Render */
		_vieRender->RemoveRenderer(_videoChannelID);

		if(_stVideoRender.pWindowRemote)
		{
			resetVideoRenderView(_stVideoRender.pWindowRemote);
		}


		_vieRender->AddRenderer(_videoChannelID, _stVideoRender.pWindowRemote, 0, _viePositionRemote.left,
			_viePositionRemote.top, _viePositionRemote.right, _viePositionRemote.bottom);

		_vieRender->StartRender(_videoChannelID);


		if(_stVideoCamera.usCameraIdx>0)
		{
			_viePositionLocal.left = 1.0;
			_viePositionLocal.right= 0.0;
		}
		else
		{
			_viePositionLocal.left = 0.0;
			_viePositionLocal.right= 1.0;
		}
		_vieRender->AddRenderer(_videoCaptureID, _stVideoRender.pWindowLocal, 0, _viePositionLocal.left,
			_viePositionLocal.top, _viePositionLocal.right, _viePositionLocal.bottom);

		_vieRender->StartRender(_videoCaptureID);

        
		return 0;

#endif
		/* Disconnect capture device */
		if (_videoChannelID >= 0)
		{
			_vieCapture->DisconnectCaptureDevice(_videoChannelID);
		}
		/* Stop capture device */
		_vieCapture->StopCapture(_videoCaptureID);
		/* Stop Render */
		_vieRender->StopRender(_videoCaptureID);
		/* Remove Render */
		_vieRender->RemoveRenderer(_videoCaptureID);

		/* Release capture device */
		_vieCapture->ReleaseCaptureDevice(_videoCaptureID);
		_videoCaptureID = -1;

		if (_vieCPM)
		{
			_vieCPM->Release();
			_vieCPM = NULL;
		}
		TraceLog(kTraceError, "VideoSetCamera: release capture\n");

		if ((_vieType & 0x01) == 0)
		{
			TraceLog(kTraceError, "VideoSetCamera:  capture is not running\n");
			return 0;
		}

#ifdef WEBRTC_IOS


		if(_stVideoCamera.usCameraIdx == 0)
		{
			_viePositionLocal.right = 1.0;
			_viePositionLocal.left = 0.0;
		}
		else  if(_stVideoCamera.usCameraIdx == 1)
		{
			_viePositionLocal.right = 0.0;
			_viePositionLocal.left = 1.0;
		}

		/* Disconnect capture device */
		if (_videoChannelID >= 0)
		{
			_vieRender->StopRender(_videoChannelID);
			_vieRender->RemoveRenderer(_videoChannelID);
		}

		if(_stVideoRender.pWindowLocal)
		{
			resetVideoRenderView(_stVideoRender.pWindowLocal);
		}
		if(_stVideoRender.pWindowRemote)
		{
			resetVideoRenderView(_stVideoRender.pWindowRemote);
		}
#endif

	}
	for (int i = 0; i < _vieCapture->NumberOfCaptureDevices(); i++)
	{
		_vieCapture->GetCaptureDevice(_stVideoCamera.usCameraIdx, ucCameraCommonName, 256, ucCameraUniqueName, 256);
		if (_vieCPM)
			_vieCPM->Release();
		_vieCPM = VideoCaptureFactory::Create(_stVideoCamera.usCameraIdx, ucCameraUniqueName);
		if (_vieCPM == NULL)
		{
			evt_appand(eME_VIDEO_EV, eME_REASON_OPEN_CAMERA_FAILED, \
			    "VideoSetCamera: open camera failed.", NULL, 0);
			_vieUpLinkSinglepassReason |= eME_REASON_OPEN_CAMERA_FAILED;
			return -1;
		}

		ret = _vieCapture->AllocateCaptureDevice(*_vieCPM, _videoCaptureID);
		_vieCPM->AddRef();
		if (!ret)
		{
			/* Configure capture parameters */
			captureCap.width = _stVideoCamera.usWidth;
			captureCap.height = _stVideoCamera.usHeight;
			captureCap.maxFPS = _stVideoCamera.ucMaxFps;

			TraceLog(kTraceError, "VideoSetCamera wh: %d x %d", \
			    captureCap.width, captureCap.height);
			/* Add and start render */
			if (_stVideoRender.pWindowLocal)
			{
				/* Add render for remote and local window */
				{
#ifdef __APPLE__

					if(_stVideoRender.pWindowLocal && (_videoCaptureID >= 0))
					{
						_vieRender->AddRenderer(_videoCaptureID, _stVideoRender.pWindowLocal, 0, _viePositionLocal.left,
							_viePositionLocal.top, _viePositionLocal.right, _viePositionLocal.bottom);

					}
#endif


#ifdef _WIN32
					if (_stVideoRender.pWindowLocal && (_videoCaptureID >= 0))
					{
						_vieRender->AddRenderer(_videoCaptureID, _stVideoRender.pWindowLocal, 0, _viePositionLocal.left,
							_viePositionLocal.top, _viePositionLocal.right, _viePositionLocal.bottom);

					}
#endif
				}
#ifdef __APPLE__


				if(_stVideoRender.pWindowRemote && (_videoChannelID >= 0) )
				{
					_vieRender->AddRenderer(_videoChannelID, _stVideoRender.pWindowRemote, 0, _viePositionRemote.left,
						_viePositionRemote.top, _viePositionRemote.right, _viePositionRemote.bottom);
					_vieRender->StartRender(_videoChannelID);
				}

#endif

				if (_videoChannelID >= 0)
				{
					ret = _vieRender->StartRender(_videoCaptureID);
				}
			}
			/* Start capture */
			_vieCapture->StartCapture(_videoCaptureID, captureCap);
			VideoRotation rf = VideoGetRotate(_stVideoCamera.usRotateAngle);
			_vieCapture->SetVideoRotation(_videoCaptureID, rf);
			if (_videoChannelID >= 0)
				ret = _vieCapture->ConnectCaptureDevice(_videoCaptureID, _videoChannelID);
			TraceLog(kTraceError, "VideoSetCamera: start capture\n");

			return 0;
		}

		_stVideoCamera.usCameraIdx++;
		_stVideoCamera.usCameraIdx = (_stVideoCamera.usCameraIdx == _vieCapture->NumberOfCaptureDevices()) ? 0 : _stVideoCamera.usCameraIdx;
	}

	/* No idle device */
	return -1;
}

/* Get camera count, return value = camera count */
int Conductor::VideoGetCameraCount(void)
{
	CriticalSectionScoped lock(_critSectVideo);
	if (NULL == _vieCapture)
		return -1;
	else
		return _vieCapture->NumberOfCaptureDevices();
}

/* Get camera state which was used currently */
int Conductor::VideoGetCameraState(ME_video_camera_param_t* camera_state)
{
	CriticalSectionScoped lock(_critSectVideo);
	memcpy(camera_state, &_stVideoCamera, sizeof(ME_video_camera_param_t));
#ifndef ANDROID
	if (camera_state->usRotateAngle == -1)
		camera_state->usRotateAngle = 0;
#endif

	if (_videoCaptureID < 0)
		camera_state->usCameraIdx = -1;

	TraceLog(kTraceError, "VideoGetCameraState wh: %d x %d", _stVideoCamera.usWidth, _stVideoCamera.usHeight);

	return 0;
}

/* Get camera info */
int Conductor::VideoGetCameraInfo(const unsigned int camera_idx, ME_video_camera_info_t* camera_info)
{
	CriticalSectionScoped lock(_critSectVideo);
	if (camera_idx >= (unsigned int)_vieCapture->NumberOfCaptureDevices())
		return -1;
	else
		_vieCapture->GetCaptureDevice(camera_idx, camera_info->ucCameraCommonName, 256, camera_info->ucCameraUniqueName, 256);

	return 0;
}

/* Get camera Capability */
int Conductor::VideoGetCameraCapability(const ucpaas_uint8 camera_idx, const ucpaas_uint8 cap_idx, ME_video_camera_capability_t* camera_capability)
{
	int ret = -1;
	int capCount;
	char ucCameraUniqueName[256];
	char ucCameraCommonName[256];
	CaptureCapability captureCap;
	CriticalSectionScoped lock(_critSectVideo);

	if (_vieCapture == NULL || camera_idx < 0 || cap_idx < 0 || camera_capability == NULL)
		return ret;

	ret = _vieCapture->GetCaptureDevice(camera_idx, ucCameraCommonName, 256, ucCameraUniqueName, 256);
	if (ret != 0)
		return ret;

	capCount = _vieCapture->NumberOfCapabilities(ucCameraUniqueName, 256);
	if (cap_idx >= capCount)
		return -1;

	ret = _vieCapture->GetCaptureCapability(ucCameraUniqueName, 256, cap_idx, captureCap);
	if (ret != 0)
		return ret;

	camera_capability->usWidth = captureCap.height;
	camera_capability->usHeight = captureCap.width;
	camera_capability->maxFps = captureCap.maxFPS;
	return 0;
}

/* Reset video parameters, return 0 if succeed, else -1 */
int Conductor::VideoResetParam(void)
{
	_videoChannelID = -1;
	_videoCaptureID = -1;

	return 0;
}

/* Video default parameters, return 0 if succeed, else -1 */
int Conductor::VideoDefaultParam(void)
{
	TraceLog(kTraceError, "VideoDefaultParam begin  ----------!");
	
	/* Encoder */
	memset(&_stVideoEnc, 0, sizeof(_stVideoEnc));
	/*
	sdp  上层实现， condutor 不用管
	strcpy((char*)_stVideoEnc.baseCodec[0].ucPlName, "VP8");
	_stVideoEnc.baseCodec[0].enable = 1;
	_stVideoEnc.baseCodec[0].ucPlType = VCM_VP8_PAYLOAD_TYPE;
	strcpy((char*)_stVideoEnc.baseCodec[1].ucPlName, "H264");
	_stVideoEnc.baseCodec[0].enable = 1;
	_stVideoEnc.baseCodec[0].ucPlType = VCM_H264_PAYLOAD_TYPE;
	_stVideoEnc.usNum = 2;
	*/
	_stVideoEnc.usWidth = DEFAULT_CODEC_WIDTH;
	_stVideoEnc.usHeight = DEFAULT_CODEC_HEIGHT;
	_stVideoEnc.ucMaxFramerate = DEFAULT_FRAME_RATE;
	_stVideoEnc.usStartBitrate = 150;
	_stVideoEnc.usMinBitrate = DEFAULT_MIN_BITRATE;
	_stVideoEnc.usMaxBitrate = DEFAULT_MAX_BITRATE;/* Reference to video engine, its value should be zero */
	_stVideoEnc.ucComplexity = 3;
#ifdef ANDROID
	_stVideoEnc.ucComplexity = 1;
	_stVideoEnc.usWidth = DEFAULT_CODEC_HEIGHT;
	_stVideoEnc.usHeight = DEFAULT_CODEC_WIDTH;
#endif
#if defined (__APPLE__)
	_stVideoEnc.ucComplexity = 2;
	_stVideoEnc.usWidth = DEFAULT_CODEC_HEIGHT;
	_stVideoEnc.usHeight = DEFAULT_CODEC_WIDTH;
#endif

	/* Decoder */
	_stVideoDec.uiMaxDefinition = 640 * 480;

	/* Image process */
	_stVideoProces.deflickingEnable = FALSE;
	_stVideoProces.denoisingEnable = FALSE;
	_stVideoProces.enhancementEnable = FALSE;

	/* Camera device */
	_stVideoCamera.usWidth = 640;// DEFAULT_CODEC_WIDTH;
	_stVideoCamera.usHeight = 480;// DEFAULT_CODEC_HEIGHT;
	_stVideoCamera.usCameraIdx = (_vieCapture->NumberOfCaptureDevices() > 1) ? 1 : 0;/* Allocated device */
	_stVideoCamera.ucMaxFps = DEFAULT_FRAME_RATE;
	_stVideoCamera.usRotateAngle = -1;
#ifdef _WIN32
	_stVideoCamera.usRotateAngle = 0;
#endif

	/* Render */
	_viePositionLocal.left = 0.0;
	_viePositionLocal.top = 0.0;
	_viePositionLocal.right = 1.0;
	_viePositionLocal.bottom = 1.0;
	_viePositionLocal.enableMirrorX = FALSE;
	_viePositionLocal.enableMirrorY = FALSE;
	_viePositionRemote.left = 0.0;
	_viePositionRemote.top = 0.0;
	_viePositionRemote.right = 1.0;
	_viePositionRemote.bottom = 1.0;
	_viePositionRemote.enableMirrorX = FALSE;
	_viePositionRemote.enableMirrorY = FALSE;
#if defined (WEBRTC_IOS)
	_viePositionLocal.top = 1.0;
	_viePositionLocal.bottom = 0.0;
	_viePositionRemote.top = 1.0;
	_viePositionRemote.bottom = 0.0;
	if(_stVideoCamera.usCameraIdx>0)
	{
		_viePositionLocal.left = 1.0;
		_viePositionLocal.right= 0.0;
	}
#endif

	_vieRemoteRes = 0;

	_vieEncoderPt = 0;
	_vieDecoderPt = 0;

	_vieFecEnable = false;
	_vieCPM = NULL;
	memset(&_stVideoExtCapture, 0, sizeof(ME_video_extern_capture_param_t));

	TraceLog(kTraceError, "VideoDefaultParam end  ----------!");
	return 0;
}

/* Get video parameters, return 0 if succeed, else -1 */
int Conductor::VideoGetParam(const int type_param, void *para)
{
	int ret = -1;
	ME_video_enc_param_t *encPara = (ME_video_enc_param_t *)para;
	ME_video_dec_info_t *decpara = (ME_video_dec_info_t *)para;
	CriticalSectionScoped lock(_critSectVideo);

	switch (type_param)
	{
	case ME_VIDEO_ENC_CFG_MODULE_ID:/* Encoder parameters included */
		memcpy(encPara, &_stVideoEnc, sizeof(_stVideoEnc));
		break;
	case ME_VIDEO_DEC_CFG_MODULE_ID:/* Decoder parameters included */
		memcpy(decpara, &_stVideoDec, sizeof(_stVideoDec));
		break;
	case ME_VIDEO_RENDER_CFG_MODULE_ID:/* Render parameters included */
		memcpy(para, &g_pVideoEngine->_stVideoRender, sizeof(ME_video_render_param_t));
		break;
	case ME_VIDEO_PROCES_CFG_MODULE_ID:/* Image process parameters included */
		memcpy(para, &g_pVideoEngine->_stVideoProces, sizeof(ME_video_proces_module_t));
		break;
	default:
		break;
	}

	return ret;
}

/* Set video parameters, return 0 if succeed, else -1 */
int Conductor::VideoSetParam(const int type_param)
{
	int ret = -1;
	CriticalSectionScoped lock(_critSectVideo);

	VideoCheckParam();

	switch (type_param)
	{
	case ME_VIDEO_ENC_CFG_MODULE_ID:/* Encoder parameters included */
		VideoConfigCodec(0x01);
		break;
	case ME_VIDEO_DEC_CFG_MODULE_ID:/* Decoder parameters included */
		VideoConfigCodec(0x02);
		break;
	case ME_VIDEO_RENDER_CFG_MODULE_ID:/* Render parameters included */
		VideoConfigRender();


#ifdef ANDROID
		if(_stVideoRender.pWindowRemote && (_videoChannelID >= 0))
		{
			_vieRender->RemoveRenderer(_videoChannelID);
			_vieRender->AddRenderer(_videoChannelID, _stVideoRender.pWindowRemote, 0, _viePositionRemote.left, 
				_viePositionRemote.top, _viePositionRemote.right, _viePositionRemote.bottom);
		}
#endif

		//devinyu 2015.05.18
#ifdef __APPLE__
		if(_stVideoRender.pWindowLocal && (_videoCaptureID >= 0))
		{
			_vieRender->RemoveRenderer(_videoCaptureID);
			_vieRender->AddRenderer(_videoCaptureID, _stVideoRender.pWindowLocal, 0, _viePositionLocal.left,
				_viePositionLocal.top, _viePositionLocal.right, _viePositionLocal.bottom);
		}

		if(_stVideoRender.pWindowRemote && (_videoChannelID >= 0))
		{
			_vieRender->RemoveRenderer(_videoChannelID);
			_vieRender->AddRenderer(_videoChannelID, _stVideoRender.pWindowRemote, 0, _viePositionRemote.left,
				_viePositionRemote.top, _viePositionRemote.right, _viePositionRemote.bottom);
		}
#endif
		break;
	case ME_VIDEO_PROCES_CFG_MODULE_ID:/* Image process parameters included */
		_vieImageProc->EnableDeflickering(_videoCaptureID, _stVideoProces.deflickingEnable);
		_vieImageProc->EnableDenoising(_videoCaptureID, _stVideoProces.denoisingEnable);
		_vieImageProc->EnableColorEnhancement(_videoChannelID, _stVideoProces.enhancementEnable);
		break;
	default:
		break;
	}

	return ret;
}

/* Check video parameters, modify illegal parameters to default value, succeed always, return 0 */
int Conductor::VideoCheckParam(void)
{
	/* Encode Parameters */
	if (_stVideoEnc.usWidth * _stVideoEnc.usHeight > 1920 * 1080)
	{
		_stVideoEnc.usWidth = 1920;
		_stVideoEnc.usHeight = 1080;
	}
	if (_stVideoEnc.ucMaxFramerate > 20)
		_stVideoEnc.ucMaxFramerate = 20;
	if (_stVideoEnc.usMaxBitrate > 1000 || _stVideoEnc.usMaxBitrate <= 0 )
		_stVideoEnc.usMaxBitrate = 1000;

	if ( _stVideoEnc.usMinBitrate <= 0 )
	{
	    _stVideoEnc.usMinBitrate = 10;
	}

    if ( _stVideoEnc.usStartBitrate <= 0 )
    {
        _stVideoEnc.usStartBitrate = 200;
    }
    
	if (_stVideoEnc.usMinBitrate > _stVideoEnc.usMaxBitrate)
		_stVideoEnc.usMinBitrate = _stVideoEnc.usMaxBitrate;

	if (_stVideoEnc.usStartBitrate > _stVideoEnc.usMaxBitrate)
		_stVideoEnc.usStartBitrate = _stVideoEnc.usMaxBitrate;

	if (_stVideoEnc.usStartBitrate < _stVideoEnc.usMinBitrate)
		_stVideoEnc.usStartBitrate = _stVideoEnc.usMinBitrate;

	/* Decode parameters */

	/* Camera parameters */

	return 0;
}

/* Start recording Video, parameter:0-output, 1-input, return 0 if succeed, else -1 */
int Conductor::VideoStartRecord(const ME_video_record_param_t* record_param)
{
	/* Configure parameters */
	//  AudioSource audioSource = record_param->streamSelect ? PLAYOUT : MICROPHONE;
	//  webrtc::CodecInst audioCodec;
	//    strcpy(audioCodec.plname,"L16");
	//    audioCodec.rate     = 256000;
	//    audioCodec.plfreq   = 16000;
	//    audioCodec.pacsize  = 160;
	//
	//    webrtc::VideoCodec videoCodec;
	//    memset(&videoCodec,0,sizeof(videoCodec));
	//    strcpy(videoCodec.plName,"VP8");
	//    videoCodec.maxBitrate=500;
	//    videoCodec.startBitrate=200;
	//    videoCodec.width=320;
	//    videoCodec.height=240;
	//    videoCodec.codecType=webrtc::kVideoCodecVP8;
	//    videoCodec.maxFramerate=30;
	//
	//  if(1 == record_param->streamSelect)
	//      _vieFile->StartRecordIncomingVideo(_videoChannelID, record_param->fileName, audioSource, audioCodec, videoCodec);
	//  else
	//      _vieFile->StartRecordOutgoingVideo(_videoChannelID, record_param->fileName, audioSource, audioCodec, videoCodec);

	return 0;
}

/* Stop recording Video, parameter:0-output, 1-input, return 0 if succeed, else -1 */
int Conductor::VideoStopRecord(const unsigned int stream_select)
{
	//  if(1 == stream_select)
	//      _vieFile->StopRecordIncomingVideo(_videoChannelID);
	//  else
	//      _vieFile->StopRecordOutgoingVideo(_videoChannelID);

	return 0;
}

/* This method will be called periodically if the average system CPU usage exceeds 75% */
void Conductor::PerformanceAlarm(const unsigned int cpuLoad)
{
	/* Performance alarm, to be continued... */
	// printf("Current cpu load is %d\n", cpuLoad);
}

/* This method is called when the decoder needs a new key frame from encoder on the sender. */
void Conductor::RequestNewKeyFrame(const int videoChannel)
{
	_vieCodec->SendKeyFrame(videoChannel);
}

void Conductor::IncomingCodecChanged(const int videoChannel, const VideoCodec& videoCodec)
{
	return;
}

void Conductor::IncomingRate(const int videoChannel, const unsigned int framerate, const unsigned int bitrate)
{
	return;
}


#ifdef EXTERNAL_TRANSPORT
int Conductor::RecvDataPacket(const recv_pm_t* recv_pm)
{
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
		else
		{   /* Modified By MNY@2014-08-22 */
			if (recv_pm->data_type == eME_MEDIA_RTP && _videoChannelID >= 0)
			{
				//                PacketTime time;
				_vieNetwork->ReceivedRTPPacket(_videoChannelID, recv_pm->data, recv_pm->len, PacketTime());
			}
			else
			{
				_vieNetwork->ReceivedRTCPPacket(_videoChannelID, recv_pm->data, recv_pm->len);
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

		iRet = (type == 0) ? _intVoiceTransportPtr->SetLocalReceiver(port) :
			_intVideoTransportPtr->SetLocalReceiver(port);
	}

	if (0 != iRet)
	{
		int i, j;
		i = channel%NUMOFLOCALPORT;
		j = rand_count() % NUMOFLOCALPORTUNIT;

		do{ //will try to apply 100 * NUMOFLOCALPORT times if failed
			port = ausLocalPort[i++] + j;

			iRet = (type == 0) ? _intVoiceTransportPtr->SetLocalReceiver(port) :
				_intVideoTransportPtr->SetLocalReceiver(port);
			if (0 == iRet)
			{
				break;
			}
			else if (i == NUMOFLOCALPORT)
			{
				i = 0;
				j += 2;
				if (j > NUMOFLOCALPORTUNIT)
				{
					break;
				}
			}
		} while (0 != iRet);
	}

	if (0 != iRet)
	{
		if (type == 0)
			_voeDnLinkSinglepassReason |= eME_REASON_SET_LOCAL_RECEIVER_FAIL;

		else if (type == 1)
			_vieDnLinkSinglepassReason |= eME_REASON_SET_LOCAL_RECEIVER_FAIL;

		TraceLog(kTraceError, \
		    "webrtc SetLocalReceiver(%d) Failed(Error = %d)!", \
		    port, 
		    (type == 0) ? _intVoiceTransportPtr->LastError() :
		        _intVideoTransportPtr->LastError());

        return -1;
	}
    iRet = port;
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

	if (0 != SetSendCodec(paudioinfo->iPayLoadType, paudioinfo))     //modified by king on 20160120
	{
		_voeBase->DeleteChannel(_voiceChannelId);
		_voiceChannelId = -1;
		TraceLog(kTraceError, "SetSendCodec failure");
		return -1;
	}

	if (0 != SetExTransport(paudioinfo->ucExTransportEnable ? true : false,
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

	TraceLog(kTraceStateInfo, "SetAudioChannel:pt=%d, RemoteAddr[%s:%d]", \
	    paudioinfo->iPayLoadType, paudioinfo->cRemoteAudioIp, \
	    paudioinfo->iRemoteAudioPort);

	return 0;
}

int Conductor::CreateChannel(void)
{
	int iRet = 0;
	CriticalSectionScoped lock(_critSectAudio);

	if (_voiceChannelId < 0)
	{
#if defined (__APPLE__)
		AudioDeviceInit();
#endif

		if ((_audioDeviceReady == eDEVICE_GET_FAIL) || (_audioDeviceReady == eDEVICE_INIT_MIC_FAIL))
		{
			_voeUpLinkSinglepassReason |= eME_REASON_AUDIO_DEVICE_INIT_FAIL;
			evt_appand(eME_LOG_TRACE_EV, eME_REASON_LOG_LEVEL_ERR, \
			    "audio device fail:audio device initialization fail", NULL, 0);
		}

		_voiceChannelId = _voeBase->CreateChannel(); //yangjj add
		if (_voiceChannelId == -1)
		{
			TraceLog(kTraceError, "CreateChannel failure(Error=%d)", _voeBase->LastError());
			return -1;
		}

		bool ipv6Enabled = _stCTRLConfig.ucIPv6Enable == 0 ? false : true;
        _intVoiceTransportPtr = new VoiceChannelTransport(_voeNetwork, 
                                                _voiceChannelId, ipv6Enabled);

#ifdef EXTERNAL_ENCRYPT
		if (_stCTRLConfig.ucRtpEncEnable)
		{
			_voeEncrypt->RegisterExternalEncryption(_voiceChannelId, *this);
		}
#endif

#ifdef EXTERNAL_MEDIA_PROCESS
		if (_stCTRLConfig.ucExtMediaProcEn)
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
		TraceLog(kTraceStateInfo, "CreateChannel(%d) Successed!!", _voiceChannelId);
	}
	else
	{
		TraceLog(kTraceWarning, "CreateChannel error, pre channel haven't deleted.");
	}

	return 0;
}

int Conductor::DeleteChannel(void)
{
	CriticalSectionScoped lock(_critSectAudio);
	UpdateState(eME_idle);

	if (SoundIsStarted())
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
		memset(&_stEnvConfig, 0, sizeof(ME_ENV_cfg_t));

        // Avoid memory leak if app haven't stop file
        _playFileDirect = -1;     //reset play file mode	
#ifdef EXTERNAL_PLAYFILE
        if ( _extPlayFile )
        {
    	    _extPlayFile->ResetPlayFile();
        }
#endif
	}

	//this->uiSendRotation = 0;
	//this->uiRecivedRotaton = 0;
	//this->uiRemoteSendRotation = 0;
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

	if (0 != _voeBase->StopReceive(_voiceChannelId))
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
		TraceLog(kTraceError, "EnableFec _voiceChannelId < 0!");
		return -1;
	}

	if (enable)
	{
		_consultFecEn = true;
	}
	else
	{
		_consultFecEn = false;
	}

	if (_stCTRLConfig.ucFecEnable && enable == 1)
	{
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
		TraceLog(kTraceError, "GetFecStatus _voiceChannelId < 0!");
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
	_voeRtp_Rtcp->SetFECStatus(_voiceChannelId, true, AUDIO_RED_PAYLOAD_TYPE_DEFAULT);
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
		TraceLog(kTraceError, "StartSend _voiceChannelId < 0!");
		return -1;
	}

	if (0 != _voeBase->StartSend(_voiceChannelId))
	{
		_voeUpLinkSinglepassReason |= eME_REASON_START_SEND_FAIL;
		TraceLog(kTraceError, "webrtc StartSend Failed!");
		return -1;
	}

	return 0;
}

int Conductor::StopSend(void)
{
	CriticalSectionScoped lock(_critSectAudio);

	if (_voiceChannelId < 0)
	{
		TraceLog(kTraceError, "StopSend _voiceChannelId < 0!");
		return -1;
	}

	if (0 != _voeBase->StopSend(_voiceChannelId))
	{
		TraceLog(kTraceError, "webrtc StopSend Failed!");
		return -1;
	}

	return 0;
}

// DTMF
#define TEV_DTMF_0          (0)
#define TEV_DTMF_1          (1)
#define TEV_DTMF_2          (2)
#define TEV_DTMF_3          (3)
#define TEV_DTMF_4          (4)
#define TEV_DTMF_5          (5)
#define TEV_DTMF_6          (6)
#define TEV_DTMF_7          (7)
#define TEV_DTMF_8          (8)
#define TEV_DTMF_9          (9)
#define TEV_DTMF_STAR       (10)
#define TEV_DTMF_POUND      (11)
#define TEV_DTMF_A          (12)
#define TEV_DTMF_B          (13)
#define TEV_DTMF_C          (14)
#define TEV_DTMF_D          (15)
#define TEV_FLASH           (16)

// DTMF
int Conductor::SendDTMF(char chDtmf)
{
	int iTevType;

	if (_voiceChannelId < 0)
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

	/*use out of band mode, modified by Rambo Fu 2013-01-23*/
	int iRet = _voeDtmf->SendTelephoneEvent(_voiceChannelId, iTevType);

	return iRet;
}

/* Add by Vinton.Liu on 20151123 for play dtmf tone to local */
int Conductor::PlayDTMF(char chDtmf)
{
	int iTevType;

	if (_voiceChannelId < 0)
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
	char  tmp_path[RECORDE_FILE_PATH_LEN] = { 0 };
	char *p = NULL;
	char *s = NULL;
	int len = 0;
	CodecInst dummyCodec = { 100, "L16", 8000, 160, 1, 8000 }; //format of recording

	if (NULL == pm || !strlen(pm->filepath)
		|| (strlen(pm->filepath) >= RECORDE_FILE_PATH_LEN))
	{
		return -1;
	}

	//to save record mode
	_recordMode = pm->iMode;
	_recordFormat = pm->iFileFormat;

	s = (char *)pm->filepath;

	len = strlen(pm->filepath);
	p = s + len;
	for (; s != p && (*p != '\\' && *p != '/'); *p--)
		;

	len = p - s;
	if (len)
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
		_snprintf(_recordMic, RECORDE_FILE_PATH_LEN, "%s\\%s", tmp_path, tmp1);
		_snprintf(_recordSpeaker, RECORDE_FILE_PATH_LEN, "%s\\%s", tmp_path, tmp2);
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
	default:
		return -2;
		break;
	}

	return iRet;
}

int Conductor::StopRecord(void)
{
	int iRet = 0;

	/* return if not recording */
	if (_recordMode < 0)
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
		if (_recordFormat == kFileFormatWavFile)
		{
			iRet = _voeFile->MixPCMToWAV(_recordMic, _recordSpeaker, _recordMixFile);
			if (iRet < 0)
			{
				TraceLog(kTraceError, "Conductor::StopRecord MixPCMToWAV failed.");
			}
		}
		remove(_recordMic);
		remove(_recordSpeaker);
		break;
	case eME_RECORD_MIC:
		iRet = _voeFile->StopRecordingMicrophone();
		if (-1 == iRet)
		{
			TraceLog(kTraceError, "Conductor::StopRecord StopRecordingMicrophone failed.");
		}
		break;
	case eME_RECORD_SPEAKER:
		iRet = _voeFile->StopRecordingPlayout(_voiceChannelId);
		if (-1 == iRet)
		{
			TraceLog(kTraceError, "Conductor::StopRecord StopRecordingPlayout failed.");
		}
		break;
	default:
		break;
	}

	_recordMode = -1;   //reset recording mode
	return iRet;
}

int Conductor::PlayFile(int32_t mode, const media_file_play_pm_t* pm)
{
	int iRet = -1;

	if ( NULL == pm )
	{
		return -1;
	}

	_playFileDirect = pm->iDirect;
    if ( _playFileDirect < 0 )
    {
        TraceLog(kTraceError, "Play file failed on wrong direct(%d).",
                _playFileDirect);
        return -1;
    }
    
    if ( kME_FILE_PATHNAME == mode && pm->filepath.filename[0] != '\0' )
    {
    	if ( 0 == _playFileDirect )
    	{
    		iRet = _voeFile->StartPlayingFileLocally(_voiceChannelId,
                                        			(char *)pm->filepath.filename,
                                        			(pm->iLoop != 0),
                                        			(FileFormats)pm->iFileFormat);
    	}
    	else
    	{
    		iRet = _voeFile->StartPlayingFileAsMicrophone(_voiceChannelId,
                                        			(char *)pm->filepath.filename,
                                        			(pm->iLoop != 0),
                                        			false,   //mixWithMicrophone
                                        			(FileFormats)pm->iFileFormat);
    	}

        if ( iRet < 0 )
		{
			TraceLog(kTraceError, "Play file to %s failed.", 
                _playFileDirect == 0 ? "local" : "mic");
		}
    }
#ifdef EXTERNAL_PLAYFILE    
    else if ( kME_FILE_HANDLE == mode 
        || kME_FILE_STREAM == mode )
    {
        playfile_pm_t filepm;
        
        if ( _extPlayFile == NULL )
    	{
    		TraceLog(kTraceError, "Play file failed on NULL param.");
    		return -1;
    	}
        
    	memset(&filepm, 0x00, sizeof(filepm));

    	filepm.loop = (pm->iLoop ? true : false);
    	filepm.mode = mode;

    	if ( mode == kME_FILE_HANDLE && pm->filehandle.fd )
    	{
    		filepm.fd = (FILE*)pm->filehandle.fd;
    	}
    	else if ( mode == kME_FILE_STREAM 
            && pm->filestream.size > 0 
            && pm->filestream.data )
    	{
    		filepm.fstream.size = pm->filestream.size;
    		filepm.fstream.data = (char*)pm->filestream.data;
    	}
        else
        {
            TraceLog(kTraceError, "Play file failed on wrong param.");
    		return -1;
        }

    	_extPlayFile->SetPlayFile(&filepm);

    	_playFileDirect = pm->iDirect;

    	if ( 0 == _playFileDirect )
    	{
    		iRet = _voeFile->StartPlayingFileLocally(_voiceChannelId, 
                                                    _extPlayFile, 
                                                    (FileFormats)pm->iFileFormat);
    	}
    	else
    	{
    		iRet = _voeFile->StartPlayingFileAsMicrophone(_voiceChannelId, 
                                                        _extPlayFile, 
                                                        false, 
                                                        (FileFormats)pm->iFileFormat);
    	}

        if ( iRet < 0 )
		{
			TraceLog(kTraceError, "Play file to %s failed.", 
                _playFileDirect == 0 ? "local" : "mic");
		}
    }
#endif
    else
    {
        TraceLog(kTraceError, "Play file failed on wrong mode(%d).", mode);
    }
    
	return iRet;
}

int Conductor::StopFile()
{
	int iRet = 0;

	if ( _playFileDirect != -1 )
	{
		if ( 0 == _playFileDirect )
		{
			iRet = _voeFile->StopPlayingFileLocally(_voiceChannelId);
		}
		else
		{
			iRet = _voeFile->StopPlayingFileAsMicrophone(_voiceChannelId);
		}

        if ( iRet < 0 )
    	{
    		TraceLog(kTraceError, "Stop Play file to %s failed.", 
                _playFileDirect == 0 ? "local" : "mic");
    	}
	}
    
	_playFileDirect = -1;     //reset play file mode
	
#ifdef EXTERNAL_PLAYFILE
    if ( _extPlayFile )
    {
	    _extPlayFile->ResetPlayFile();
    }
#endif
	return iRet;
}

int Conductor::SetSpeakerVolume(unsigned int iVolumeValue)
{
	int nRet = _voeVolume->SetSpeakerVolume(iVolumeValue);

	if (-1 == nRet)
	{
		TraceLog(kTraceError, "webrtc SetMicVolume failed");
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

// LoudSpeaker control
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

	if (_voeVolume)
	{
		if ((GetCurState() == eME_Running) || (GetCurState() == eME_Connected))
		{
			nRet = _voeVolume->SetInputMute(_voiceChannelId, enable);
		}
	}

	if (-1 == nRet)
	{
		TraceLog(kTraceError, "webrtc SetMicVolume failed");
	}
	return nRet;
}

/* add by Vinton.Liu on 20140922 for get microphone status */
int Conductor::GetMicrophoneMute()
{
	bool mute = false;
	int nRet = -1;

	if (_voeVolume)
	{
		if ((GetCurState() == eME_Running) || (GetCurState() == eME_Connected))
		{
			nRet = _voeVolume->GetInputMute(_voiceChannelId, mute);
		}
	}

	if (-1 == nRet)
	{
		TraceLog(kTraceError, "webrtc GetMicVolume failed");
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

	TraceLog(kTraceReport, \
	    "Conductor::GetRecordingDeviceStatus() isAvailable(%d)", isAvailable);
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
		TraceLog(kTraceError, "webrtc SetRecordingDevice failed");

	nRet = _voeHardware->SetPlayoutDevice(pm->wav_dev_out);
	if (-1 == nRet)
		TraceLog(kTraceError, "webrtc SetPlayoutDevice failed");

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
	_videoEngine->SetTraceFilter(level);
	return 0;
}

Conductor* Conductor::GetInstance(int iDataSource)
{
	if (m_pInstance == NULL)
	{
		m_pInstance = new Conductor;
		if (-1 == m_pInstance->LoadAPIs(iDataSource))
		{
			delete m_pInstance;
			m_pInstance = NULL;
			return NULL;
		}

		if (-1 == m_pInstance->AuthAndInit())
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

		if (loadcnt == 0)
		{
			m_pInstance->StopThread();//stop thread to detect event;
			m_pInstance->DeleteChannel();

			/* Modified By MNY@2014-08-22, Stop stream */
			m_pInstance->VideoDeleteStream();

			m_pInstance->AudioDeviceTerminate();
			m_pInstance->releaseWebrtcSubApi();
			m_pInstance->UpdateState(eME_done);
			delete m_pInstance;
			m_pInstance = NULL;

			g_pVideoEngine = NULL;

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
	return 0;
	//return(_voeHardware->MicrophoneBoostIsAvailable(available));
}

int Conductor::SetMicrophoneBoost(bool enable)
{
	return 0;
	//return(_voeHardware->SetMicrophoneBoost(enable));
}

int Conductor::MicrophoneBoost(bool* enable)
{
	return 0;
	//return(_voeHardware->MicrophoneBoost(enable));
}

// Volume control based on the Windows Wave API (Windows only)
int Conductor::SetWaveOutVolume(uint16_t volumeLeft, uint16_t volumeRight)
{
	return 0;
	//return(_voeHardware->SetWaveOutVolume(volumeLeft, volumeRight));
}

int Conductor::WaveOutVolume(uint16_t* volumeLeft, uint16_t* volumeRight)
{
	return 0;
	//return(_voeHardware->WaveOutVolume(volumeLeft, volumeRight));
}

#endif


/*
** release sub api resource called when delete voice engine
*/
void Conductor::releaseWebrtcSubApi()
{
	if (_voeBase)
	{
		_voeBase->Release();
		_voeBase = NULL;
	}
	if (_voeVqe)
	{
		_voeVqe->Release();
		_voeVqe = NULL;
	}
	if (_voeCodec)
	{
		_voeCodec->Release();
		_voeCodec = NULL;
	}
	if (_voeHardware)
	{
		_voeHardware->Release();
		_voeHardware = NULL;
	}
	if (_voeDtmf)
	{
		_voeDtmf->Release();
		_voeDtmf = NULL;
	}
	if (_voeVolume)
	{
		_voeVolume->Release();
		_voeVolume = NULL;
	}
	if (_voeRtp_Rtcp)
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

    /* add by Vinton.Liu on 2016-03-29 for support external audio device push pcm to and pull pcm from engine begin */
    if ( _voeAudioTransport )
    {
        delete _voeAudioTransport;
        _voeAudioTransport = NULL;
    }
    /* Vinton.Liu add end */
	
	if(_voeEngine)
	{
		VoiceEngine::Delete(_voeEngine);
		_voeEngine = NULL;
	}

	if (_vieCapture)
	{
		_vieCapture->Release();
		_vieCapture = NULL;
	}

	if (_vieRTPRTCP)
	{
		_vieRTPRTCP->Release();
		_vieRTPRTCP = NULL;
	}

	if (_vieRender)
	{
		_vieRender->Release();
		_vieRender = NULL;
	}

	if (_vieCodec)
	{
		_vieCodec->Release();
		_vieCodec = NULL;
	}

	if (_vieNetwork)
	{
		_vieNetwork->Release();
		_vieNetwork = NULL;
	}

#ifdef EXTERNAL_TRANSPORT
	if (_extVideoTransportPtr)
	{
		delete _extVideoTransportPtr;
		_extVideoTransportPtr = NULL;
	}
#endif

	//    if(_vieFile)
	//    {
	//      _vieFile->Release();
	//        _vieFile = NULL;
	//    }

	if (_vieBase)
	{
		_vieBase->Release();
		_vieBase = NULL;
	}

	if (_vieImageProc)
	{
		_vieImageProc->Release();
		_vieImageProc = NULL;
	}

	if (_videoEngine)
	{
		VideoEngine::Delete(_videoEngine);
		_videoEngine = NULL;
	}


}

/*
** webrtc support the codec?
*/
bool Conductor::webrtcCodecSupport(const char* mime)
{
	if (_voeCodec)
	{
		CodecInst inst;
		int nNumCodec = _voeCodec->NumOfCodecs();
		for (int k = 0; k < nNumCodec; ++k)
		{
			_voeCodec->GetCodec(k, inst);
			if (strcasecmp(inst.plname, mime) == 0)
				return true;
		}
	}


	if (_vieCodec)
	{
		VideoCodec inst;
		int nNumCodec = _vieCodec->NumberOfCodecs();
		for (int k = 0; k < nNumCodec; ++k)
		{
			_vieCodec->GetCodec(k, inst);
			if (strcasecmp(inst.plName, mime) == 0)
				return true;
		}
	}



	return false;
}

bool Conductor::webrtcCodecSupport(int payload)
{
	if (_voeCodec)
	{
		CodecInst inst;
		int nNumCodec = _voeCodec->NumOfCodecs();
		for (int k = 0; k < nNumCodec; ++k)
		{
			_voeCodec->GetCodec(k, inst);
			if (inst.pltype == payload)
				return true;
		}
	}


	if (_vieCodec)
	{
		VideoCodec inst;
		int nNumCodec = _vieCodec->NumberOfCodecs();
		for (int k = 0; k < nNumCodec; ++k)
		{
			_vieCodec->GetCodec(k, inst);
			if (inst.plType == payload)
				return true;
		}
	}


	return false;
}

int Conductor::getCodecs(ME_codec_list_t* codecs_list)
{
	int total = 0;

	if (codecs_list == NULL)
		return -1;

    memset(codecs_list, 0x00, ME_CODEC_LIST_T_SIZE);

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

	VideoEngine::SetAndroidObjects(pm_obj->jvm, pm_obj->context);

	VoiceEngine::SetAndroidObjects(pm_obj->jvm, pm_obj->context);

    /* add by Vinton.Liu on 2016-03-08 for android thread jni begin */    
	ThreadWrapper::SetAndroidObjects(pm_obj->jvm, pm_obj->context);
    /* Vinton.Liu add end */
}

void Conductor::SetAndroidApi(int level)
{
	//not define
#if 0
	VoiceEngine::SetAndroidApiLevel(level);
#endif    
}
#endif


//add by wuzhaoyang20151225 start
int Conductor::SetDirectCallFecStatus(char enable)
{
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
	signed short diff = 0;                              //modified by king 20160120
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
#define GET_CODEC_NAME(x) (((x)==kVideoCodecVP8)?("VP8"):(((x)==kVideoCodecH264)?("H.264"):(((x)==kVideoCodecH265)?("H.265"):("H.264"))))
void Conductor::updateRtpStatus(void)
{
	char strLog[256] = { 0 };
	unsigned short upLost = 0;
	unsigned short downLost = 0;
    unsigned int jitter = 0;
	bool NewUplinkSinglepassFlag = false;
	bool NewDnlinkSinglepassFlag = false;
	int Newuplink_lostrate = 0;
	int Newdnlink_lostrate = 0;
	bool fecflag = false;
	int redPtype = 0;
	int bitrate_bps = 0;
	int16_t target_fps = 0;
	int rec_bitrate = 0;
	int32_t width = 0;
	int32_t height = 0;
    CallStatisticsReport statReport = { 0 };

	if (_voeRtp_Rtcp == NULL || _voiceChannelId < 0)
		return;

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
	UpdateRTT(statReport.rttMs);

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
	if (statReport.packets_sent == _voePktSent)
	{
		if (_voeUpLinkSinglepassReason & eME_REASON_AUDIO_DEVICE_INIT_FAIL)
		{
			_voeUpLinkSinglepassReason ^= eME_REASON_AUDIO_DEVICE_INIT_FAIL;
			NewUplinkSinglepassFlag = true;

			evt_appand(eME_RTP_UPSINGLEPASS_EV, eME_REASON_AUDIO_DEVICE_INIT_FAIL, \
			    "uplink singlepass:maybe voice mic initialization fail.", \
			    NULL, 0);
		}
		else if (_voeUpLinkSinglepassReason & eME_REASON_START_SEND_FAIL)
		{
            // This error couldn't not be restore except redial
            //_voeUpLinkSinglepassReason ^= eME_REASON_START_SEND_FAIL;
			NewUplinkSinglepassFlag = true;

			evt_appand(eME_RTP_UPSINGLEPASS_EV, eME_REASON_START_SEND_FAIL, \
			    "uplink singlepass:maybe voice start send fail.", NULL, 0);
		}
        else if (statReport.packets_sent > 0)
		{
			NewUplinkSinglepassFlag = true;

			evt_appand(eME_RTP_UPSINGLEPASS_EV, eME_REASON_NETWORK_PROBLEM, \
			    "uplink singlepass:maybe network problem.", NULL, 0);
		}
		
	}

	if (_voeUplinkSinglepassFlag && !NewUplinkSinglepassFlag)
	{
		evt_appand(eME_RTP_UPSINGLEPASS_EV, eME_REASON_RTP_RESTORE_OK, \
		    "uplink singlepass: restore ok.", NULL, 0);
	}

	_voePktSent = statReport.packets_sent;
	_voeUplinkSinglepassFlag = NewUplinkSinglepassFlag;

	/* check if have downlink single pass,
	    should exclude the packets is 0 at the beginning time to receive */
	if (statReport.packets_received == _voePktReceived)
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

	if (_voeDownlinkSinglepassFlag && !NewDnlinkSinglepassFlag)
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

	/* return while video is disabled*/
	if (_vieRTPRTCP == NULL || _videoChannelID < 0)
		return;

	// Gets RTCP data from incoming RTCP Sender Reports.
    ME_video_network_state_t vnet = { 0 };
    CallStatisticsReport vieCallReport = { 0 };
    
	//>!------eME_VIDEO_NETWORK_STATE_EV参数初始化-------//
	vnet.bitrate_bps = bitrate_bps / 1000;
	vnet.target_fps = target_fps;
	vnet.rec_bitrate = rec_bitrate;
	vnet.width = width;
	vnet.height = height;
	vnet.decode_width = decode_width;
	vnet.decode_height = decode_height;
	vnet.decode_fps = decode_fps;
	vnet.EncoderPt = enc_pltype;
	vnet.DecoderPt = dec_pltype;
	strcpy(vnet.encCodec, enc_codecname);
	strcpy(vnet.decCodec, dec_codecname);

    NewDnlinkSinglepassFlag = false;
    NewUplinkSinglepassFlag = false;
    
    if (_vieRTPRTCP->GetCallStatisticsReport(_videoChannelID, vieCallReport) < 0)
    {
        return;
    }

    _vieRTPRTCP->GetBitRatesAndFrameRate(_videoChannelID, bitrate_bps, target_fps, rec_bitrate);

    _vieRTPRTCP->GetEncoderResolution(_videoChannelID, width, height);

	_vieRTPRTCP->GetPayloadTypeAndCodecType(_videoChannelID, enc_pltype, enc_codectype, dec_pltype, dec_codectype);
	enc_codecname = (ucpaas_int8*)GET_CODEC_NAME(enc_codectype);
	dec_codecname = (ucpaas_int8*)GET_CODEC_NAME(dec_codectype);

	if (vieCallReport.packets_received == _viePktReceived)
	{
        if (_vieDnLinkSinglepassReason & eME_REASON_SET_LOCAL_RECEIVER_FAIL)
        {
            NewDnlinkSinglepassFlag = true;
            evt_appand(eME_RTP_DNSINGLEPASS_EV, \
                        eME_REASON_SET_LOCAL_RECEIVER_FAIL, \
                        "video dnlink singlepass: maybe set local receiver failed.", \
                        NULL, 0);
        }
        else if (_vieDnLinkSinglepassReason & eME_REASON_START_RECEIVE_FAIL)
        {
            NewDnlinkSinglepassFlag = true;
            evt_appand(eME_RTP_DNSINGLEPASS_EV, \
                        eME_REASON_START_RECEIVE_FAIL, \
                        "video dnlink singlepass:maybe video start receive failed.", \
                        NULL, 0);
        }
        else if (vieCallReport.packets_received > 0)
        {
            NewDnlinkSinglepassFlag = true;
            evt_appand(eME_RTP_DNSINGLEPASS_EV, \
                        eME_REASON_NETWORK_PROBLEM, \
                        "video dnlink singlepass: maybe network problem.", \
                        NULL, 0);
        }
	}
    _viePktReceived = vieCallReport.packets_received;
	_vieDownlinkSinglepassFlag = NewDnlinkSinglepassFlag;

	if (vieCallReport.packets_sent == _viePktSent)
	{

        if (_vieUpLinkSinglepassReason & eME_REASON_OPEN_CAMERA_FAILED)
        {
            NewUplinkSinglepassFlag = true;
            evt_appand(eME_RTP_UPSINGLEPASS_EV, \
                        eME_REASON_OPEN_CAMERA_FAILED, \
                        "uplink singlepass: maybe open camera failed.", \
                        NULL, 0);
        }
        else if (_vieUpLinkSinglepassReason & eME_REASON_START_SEND_FAIL)
        {
            NewUplinkSinglepassFlag = true;
            evt_appand(eME_RTP_UPSINGLEPASS_EV, eME_REASON_START_SEND_FAIL, \
                    "uplink singlepass:maybe video start send failed.", NULL, 0);
        }
        else if (vieCallReport.packets_sent > 0)
        {
            NewUplinkSinglepassFlag = true;
            evt_appand(eME_RTP_UPSINGLEPASS_EV, eME_REASON_NETWORK_PROBLEM, \
                        "uplink singlepass:maybe network problem.", NULL, 0);
        }
	}
    _viePktSent = vieCallReport.packets_sent;
	_vieUplinkSinglepassFlag = NewUplinkSinglepassFlag;

	if ( !_vieDownlinkSinglepassFlag )
	{
		vnet.ice = _iceMode;
		vnet.rttMs = (unsigned int)vieCallReport.rttMs;
		vnet.uplinkLost = vieCallReport.up_lost;
		vnet.downlinkLost = vieCallReport.down_lost;
        vnet.upJitter = vieCallReport.up_jitter;
        vnet.downJitter = vieCallReport.down_jitter;
		vnet.bitrate_bps = bitrate_bps / 1000;
		vnet.target_fps = target_fps;
		vnet.rec_bitrate = rec_bitrate;
		vnet.width = width;
		vnet.height = height;
		vnet.decode_width = decode_width;
		vnet.decode_height = decode_height;
		vnet.decode_fps = decode_fps;
		vnet.EncoderPt = enc_pltype;
		vnet.DecoderPt = dec_pltype;
		strcpy(vnet.encCodec, enc_codecname);
		strcpy(vnet.decCodec, dec_codecname);

		if (_stVideoEnc.ucUseHwEnc && (_stVideoEnc.ucFirstSelectedPt == vnet.EncoderPt))
			strcat(vnet.encCodec, "-hw");
		else
			strcat(vnet.encCodec, "-sw");
		if (_stVideoEnc.ucUseHwDec && (_stVideoEnc.ucFirstSelectedPt == vnet.DecoderPt))
			strcat(vnet.decCodec, "-hw");
		else
			strcat(vnet.decCodec, "-sw");
		vnet.rtPOT_nCount = vieCallReport.rtPOT.nCount;
		for (int i = 0; i <= vnet.rtPOT_nCount; i++)
		{
			vnet.rtPOT_SendIP[i] = vieCallReport.rtPOT.SendIP[i];
			vnet.rtPOT_SendValue_a[i] = (statReport.rtPOT.SendValue[i]>>10);    // audio send flow (KB)
			vnet.rtPOT_SendValue_v[i] = (vieCallReport.rtPOT.SendValue[i] >> 10); // vidoe send flow (KB)
			vnet.rtPOT_RecvIP[i] = vieCallReport.rtPOT.RecvIP[i];
			vnet.rtPOT_RecvValue_a[i] = (statReport.rtPOT.RecvValue[i] >> 10);    // audio receive flow (KB)
			vnet.rtPOT_RecvValue_v[i] = (vieCallReport.rtPOT.RecvValue[i] >> 10); // vidoe receive flow (KB)
		}

		if (vieCallReport.down_lost <= VIDEO_NETSTATE_FRACT_NICE_VALUE)
		{
			vnet.net_state = eME_REASON_NETWORK_NICE;		
		}
        else if (vieCallReport.down_lost <= VIDEO_NETSTATE_FRACT_WELL_VALUE)
		{
			vnet.net_state = eME_REASON_NETWORK_GENERAL;			
		}
		else
		{
			vnet.net_state = eME_REASON_NETWORK_BAD;
		}

        evt_appand(eME_VIDEO_NETWORK_STATE_EV, vnet.net_state, \
        	    "network state changed.", \
        	    &vnet, ME_VIDEO_NETWORK_STATE_T_SIZE);
	}
}

void Conductor::updateSoundDevice()
{
#ifdef _WIN32   
	if (_voeHardware)
	{
		int recordNum = 0;
		int playNum = 0;

		_voeHardware->GetNumOfRecordingDevices(recordNum);
		_voeHardware->GetNumOfPlayoutDevices(playNum);

		if (recordNum != _recordDeviceNum || playNum != _playDeviceNum)
		{
			_recordDeviceNum = recordNum;    //update record num
			_playDeviceNum = playNum;        // update play num
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
	if (CFG_UPDATE_VOICE & _updateCfgFlag)
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
        _voeVqe->SetEcStatus(_stVQEConfig.Ec_enable, EcModeCfg);
#else
		_voeVqe->SetEcStatus(_stVQEConfig.Ec_enable, EcModeCfg);
#endif
		_voeVqe->SetEcStatus(_stVQEConfig.Ec_enable, EcModeCfg);
		if (kEcAecm == EcModeCfg)
		{
			AecmModes enumSpeakerMode = _stEnvConfig.status ? kAecmLoudSpeakerphone : kAecmEarpiece;
			_voeVqe->SetAecmMode(enumSpeakerMode, true);
		}

		//Set voice cfg if meeting mode;
		if (_stEnvConfig.dialogScene)
		{			
			// const AgcConfig meetConfigDefault = { 6, 10, 1 };
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
			_voeVqe->SetNsStatus(_stVQEConfig.Ns_enable, kNsVeryHighSuppression);
		}
		_voeVqe->SetRxNsStatus(_voiceChannelId, _stVQEConfig.Ns_Rx_enable, kNsVeryHighSuppression);
		//const AgcConfig RX_config = { 6, 9, 1 };
		//_voeVqe->SetRxAgcConfig(_voiceChannelId, RX_config);
		_voeVqe->SetRxAgcStatus(_voiceChannelId, _stVQEConfig.Agc_Rx_enable, RX_AgcModeCfg);
		_voeVqe->SetDualMicNsStatus(_stVQEConfig.Dual_Mic_Ns_enable);//added by chgx 20160504

		_updateCfgFlag &= ~CFG_UPDATE_VOICE;
	}

	//config rtp
	if (CFG_UPDATE_RTP & _updateCfgFlag)
	{
		_voeNetwork->SetPacketTimeout(_voiceChannelId, true, _stRTPConfig.uiRTPTimeout);
        
		TraceLog(kTraceReport, "update RTP cfg uiRTPTimeout=%d uiFixLowPayload=%d", \
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
		_voeRtp_Rtcp->SetREDStatus(_voiceChannelId, 
                                    _stCTRLConfig.ucFecEnable && _consultFecEn, 
                                    AUDIO_RED_PAYLOAD_TYPE_DEFAULT);
		TraceLog(kTraceError, "update FEC cfg uiFECconfig=%d", _stCTRLConfig.ucFecEnable);
		_updateCfgFlag &= ~CFG_UPDATE_FEC;
	}

	//config codec
	if (CFG_UPDATE_CODEC & _updateCfgFlag)
	{
		if (SetSendCodec(_stCodecConfig.codecs[0].pltype, NULL))     //modified by king on 20160120
		{
			TraceLog(kTraceError, "update codec cfg fail, pt=%d", _stCodecConfig.codecs[0].pltype);
		}
		_updateCfgFlag &= ~CFG_UPDATE_CODEC;
	}

	//envionment update
	if (CFG_UPDATE_ENV & _updateCfgFlag)
	{
		//Set voice cfg if meeting mode;
		if (_stEnvConfig.dialogScene)
		{
			// const AgcConfig configDefault = { 6, 10, 1 };//default {3, 9, 1}				
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
	if ((_voiceChannelId >= 0)
		&& ((_audioDeviceReady == eDEVICE_GET_FAIL)
		|| (_audioDeviceReady == eDEVICE_INIT_MIC_FAIL)))
	{
		int iLoopCnt = 10;
		while (iLoopCnt--)
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
	memset(&_audioStreamInfo, 0x00, ME_AUDIO_STREAM_INFO_ST_SIZE);

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
	_voePktSent = -1;       //-1 is invalid that means sending isn't begin, Rambo on 20137-29
	_voePktReceived = -1;   //-1 is invalid that means receiveing isn't begin, Rambo on 20137-29
	_voeUplinkSinglepassFlag = false;
	_voeDownlinkSinglepassFlag = false;
	_voeFractionLost = 0;

	return 0;
}

int Conductor::ResetEmodel(int pt)
{
#ifdef EMODEL_ENABLED
	CriticalSectionScoped lock(_critSect);

	stCodecRateInfo codecRate[MAX_RATE_CNT];
	int max_cnt = 0;
	stPktInfo   stPktTypeTbl[MAX_PKT_CNT + 1];
	int max_pkt_cnt = 0; //the number of packet

	emd_calculate_clean();
	//get codec type and type number
	memset(codecRate, 0, sizeof(codecRate));
	memset(stPktTypeTbl, 0, sizeof(stPktTypeTbl));
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
	if (pAudioinfo->isDirectCall == FALSE)
	{
		//Close Rx agc/ns when p2p call
		_stVQEConfig.Agc_Rx_enable = false;
		_stVQEConfig.Ns_Rx_enable = true;
	}
	else
	{
		//Open Rx agc/ns when call phone
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
		if (0 != iRet)
		{
			iRet = _voeBase->LastError();
			TraceLog(kTraceError, "voice webrtc DeRegisterExternalTransport Failed!, error = %d", iRet);
			return -1;
		}

		iRet = _voeNetwork->RegisterExternalTransport(_voiceChannelId, *_extVoiceTransportPtr);
		if (0 != iRet)
		{
			iRet = _voeBase->LastError();
			TraceLog(kTraceError, "voice webrtc RegisterExternalTransport Failed!, error = %d", iRet);
			return -1;
		}
	}
	else
#endif
	{
		if (_audioStreamInfo.iLocalAudioPort != lport)
		{
			iRet = SetLocalReceiver(0, _voiceChannelId, lport);
			if (iRet < 0)
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
		_voeCodec->SetVADStatus(_voiceChannelId, false, kVadConventional, false);
#else
		/*For ios and android platform, the count of packets is very important beacuse of bandwidth,so kVadAggressiveHigh is more fit*/
		_voeCodec->SetVADStatus(_voiceChannelId, false, kVadAggressiveHigh, false);
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
	if ( _voeCodec->GetSendCodec(_voiceChannelId, inst) < 0 )
	{
	    return -1;
	}
	
	if ( inst.pltype != pt )
	{
		int idx = -1;
		int nNumCodec = 0;
		CodecInst codecInst = { 0 };

		nNumCodec = _voeCodec->NumOfCodecs();
		for (int k = 0; k < nNumCodec; ++k)
		{
			_voeCodec->GetCodec(k, codecInst);
			if ( codecInst.pltype == pt )
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

			SetCngMode(paudioinfo);           //added by king on 20160120

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

	/*excute once every TIMER_PERIOD_MS, comment by Rookie*/
	switch (_ptrTimeEvent->Wait(TIMER_PERIOD_MS))
	{
	case kEventSignaled:
		break;
	case kEventError:
		TraceLog(kTraceWarning, "EventWrapper::Wait() failed => restarting timer");
		//_ptrTimeEvent->StopTimer();
		//_ptrTimeEvent->StartTimer(true, TIMER_PERIOD_MS);
		return true;
	case kEventTimeout:
		//return true;
		break;
	}

	if (!_threadKeepAlive)
	{
		TraceLog(kTraceWarning, "_threadKeepAlive is false, thread exit!!");
		return true;
	}

	state = GetCurState();
	if (state == eME_Running || state == eME_Connected)
	{
		//send cn packet when audio device initiation is failed. 
		if ((_audioDeviceReady == eDEVICE_GET_FAIL) || (_audioDeviceReady == eDEVICE_INIT_MIC_FAIL))
		{
			int iTransferedByte;// for sent udp packet;
			if (_voiceChannelId >= 0)
				_voeNetwork->SendUDPPacket(_voiceChannelId, &g_ucRtpCnPkt, 21, iTransferedByte);
		}

		/* add by Vinton.Liu on 2016-03-25 for send keep live packet when local camera start failed begin */
		if ( _videoChannelID >= 0 
		    && ((_vieUpLinkSinglepassReason & eME_REASON_OPEN_CAMERA_FAILED)
		    || (!(_vieTypeSupposed & 0x01))))
		{
		    unsigned char packet[2] = { 0 };
            int32_t len = 0;
            _vieNetwork->SendUDPPacket(_videoChannelID, &packet[0], sizeof(packet), len);
		}
        /* Vinton.Liu add end */
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

	if (_ptrThread == NULL)
	{
		_threadKeepAlive = true;
		_ptrThread = ThreadWrapper::CreateThread(ThreadFunc, this, kNormalPriority, "Conductor_Thread");
		_ptrThread->Start(threadID);
	}

#if 0
	if (!_ptrTimeEvent->StartTimer(true, TIMER_PERIOD_MS))
	{
		TraceLog(kTraceWarning, "failed to start the timer event in conductor");
		res = _ptrThread->Stop();
		if (!res)
		{
			TraceLog(kTraceWarning, "unable to stop the activated thread");
		}

		delete _ptrThread;
		_ptrThread = NULL;
	}
#endif    
}

void Conductor::StopThread()
{
	if (_ptrThread)
	{
		TraceLog(kTraceDebug, "_ptrThread->Stop()---in");
		_threadKeepAlive = false;
		_ptrTimeEvent->Set();
		_ptrThread->Stop();//this will wait 3s until thread stop
		delete _ptrThread;
		_ptrThread = NULL;
		TraceLog(kTraceDebug, "_ptrThread->Stop()---out");
	}
	//_ptrTimeEvent->StopTimer();
}

int Conductor::VideoSetRemoteReslution(int res)
{
	CriticalSectionScoped lock(_critSectVideo);
	this->_vieRemoteRes = res;

	return 0;
}

int Conductor::VideoSetRemoteCameraReslution(int width, int height)
{
	this->_vieRemoteVideoWidth = width;
	this->_vieRemoteVideoHeight = height;

	return 0;
}

int Conductor::VideoSetRemoteFps(int decfps)
{
	CriticalSectionScoped lock(_critSectVideo);
	this->_vieRemoteFps = decfps;
	return 0;
}

int Conductor::VideoSetRemoteBitRate(unsigned int start, unsigned int min, unsigned int max)
{
	CriticalSectionScoped lock(_critSectVideo);
	this->_vieRemoteStartBitrate = start;
	this->_vieRemoteMinBitrate = min;
	this->_vieRemoteMaxBitrate = max;
	return 0;
}

/* Set Video remote send rotation */
int Conductor::VideoSetRemoteSendRotation(const int angle)
{
    int rotation = angle * 90;
    CriticalSectionScoped lock(_critSectVideo);

    if ( angle < 0 )
    {
        return -1;
    }
    
    if (this->_vieRemoteSendRotation == rotation)
		return 0;
    
	this->_vieRemoteSendRotation = (angle % 4) * 90;
    
	if ( _videoChannelID >= 0 )
	{
        if ( _stVideoRender.renderMode == 1 )
        {
    		float scale = VideoCalcScale();
    		_vieCodec->SetDecodeFrame(_videoChannelID, scale);
        }

        _vieCodec->RotateDecodedFrame(_videoChannelID, 
            (this->_vieRemoteSendRotation + this->_vieRecivedRotaton) % 360);
	}

    return 0;
}

/* Get Video Send Rotation and State */
int Conductor::VideoGetSendRotationAndState(int& angle, int& state)
{
    CriticalSectionScoped lock(_critSectVideo);
    state = _vieType;
    if ( _stVideoCamera.usCameraIdx )
    {
        angle = _vieSendRotation / 90;
    }
    else
    {
        angle = 4 - ( _vieSendRotation / 90 );
    }

    return 0;
}

/* Set video rotation angle via rtp channel or not */
int Conductor::VideoSetMediaSignalStatus(int status)
{
    CriticalSectionScoped lock(_critSectVideo);
    _vieStreamInfo.iMediaSignalEnable = status;

    return 0;
}

int Conductor::VideoSendStatePackets( uint16_t type )
{
    uint8_t packet[2] = { 0 };
    int32_t len = 0;
    bool needSend = false;

    if ( _videoChannelID < 0 )
    {
        return -1;
    }

    if ( type == 1 || type == 3 )
    {
        packet[0] = 1;            
    	if (_stVideoCamera.usCameraIdx)
        {
    		packet[1] = (unsigned char)(this->_vieSendRotation / 90);
    	}
    	else
        {
    		packet[1] = (unsigned char)(4 - this->_vieSendRotation / 90);
    	}
        
        if ( type == 3 )
        {
            packet[1] |= 0x08;
        }
        needSend = true;
    }
    else if ( type == 2 )
    {
        packet[0] = 2;
        packet[1] = (unsigned char)(_vieType);
        needSend = true;
    }

    if ( needSend )
    {
        TraceLog(kTraceReport, "VideoSendStatePackets() type[%d] _vieSendRotation[%d]", type, _vieSendRotation);
        _vieNetwork->SendUDPPacket(_videoChannelID, &packet[0], sizeof(packet), len);
        // send again to ensure remote side received
        _vieNetwork->SendUDPPacket(_videoChannelID, &packet[0], sizeof(packet), len);
    }

    return 0;
}

int Conductor::VideoSetSendReciveRotation(unsigned int sendRotation, unsigned int recivedRotation)
{
	int ret = 0;
	CriticalSectionScoped lock(_critSectVideo);
    bool isInitAngle = false;
	if (this->_vieRecivedRotaton == 360)
		isInitAngle = true;

	this->_vieSendRotation = sendRotation;
	if (this->_vieRecivedRotaton == recivedRotation)
		return 0;
	this->_vieRecivedRotaton = recivedRotation;

	if (_videoChannelID >= 0 )
	{
        if ( _stVideoRender.renderMode == 1 )
        {
    		float scale = VideoCalcScale();
    		_vieCodec->SetDecodeFrame(_videoChannelID, scale);
        }

        ret = _vieCodec->RotateDecodedFrame(_videoChannelID, 
                        (_vieRemoteSendRotation + _vieRecivedRotaton) % 360);
	}

    if ( _vieStreamInfo.iMediaSignalEnable )
    {
	    VideoSendStatePackets( isInitAngle ? 3 : 1);
    }
	return ret;
}

int Conductor::VideoIncomingFrame(const ucpaas_uint8* data, const ucpaas_uint32 len)
{
	int ret = 0;
	CriticalSectionScoped lock(_critSectVideo);
	Clock*c = Clock::GetRealTimeClock();

	ViEVideoFrameI420 frame;
	frame.formate = _stVideoExtCapture.ucExternformate;
	if (_stVideoExtCapture.ucExternformate == 0)
	{
		frame.y_plane = (unsigned char*)data;
		frame.u_plane = (unsigned char*)data + _stVideoCamera.usWidth * _stVideoCamera.usHeight;
		frame.v_plane = (unsigned char*)data + _stVideoCamera.usWidth * _stVideoCamera.usHeight * 5 / 4;
		frame.height = _stVideoCamera.usHeight;
		frame.width = _stVideoCamera.usWidth;
		frame.y_pitch = _stVideoCamera.usWidth;
		frame.u_pitch = frame.v_pitch = _stVideoCamera.usWidth / 2;
		frame.data_len = len;
	}
	else if (_stVideoExtCapture.ucExternformate == 1)
	{
		frame.y_plane = (unsigned char*)data;
		frame.height = _stVideoCamera.usHeight;
		frame.width = _stVideoCamera.usWidth;
		frame.y_pitch = _stVideoCamera.usWidth;
		frame.u_pitch = frame.v_pitch = _stVideoCamera.usWidth / 2;
		frame.data_len = len;
	}

	if (_vieExtCapture)
		_vieExtCapture->IncomingFrameI420(frame, c->CurrentNtpInMilliseconds());
	return ret;
}

int Conductor::VideoSetFilter(const int filter_type, const ucpaas_float32 filter_sensity)
{
    _vieCapture->SetFilter(_videoCaptureID,filter_type, filter_sensity);
    //printf("conduct : %d %lf",filter_type,filter_sensity);
    return 0;
}

void Conductor::IncomingSSRCChanged(const int video_channel,
	const unsigned int SSRC) {};

// This method is called if a field in CSRC changes or if the number of
// CSRCs changes.
void Conductor::IncomingCSRCChanged(const int video_channel,
	const unsigned int CSRC,
	const bool added) {};

void Conductor::OnRecivedShortMessage(const int id,
	const unsigned short m)
{
	if (m / 256 == 1)
	{
		//if (this->_vieRemoteSendRotation == (m % 4) * 90)
		//	return;

		this->_vieRemoteSendRotation = (m % 4) * 90;
        
		if (m & 0x0008)
		{
			if (_videoChannelID >= 0)
			{
				//+1 to request key frame
				_vieCodec->RotateDecodedFrame(_videoChannelID, 
				    (_vieRemoteSendRotation + _vieRecivedRotaton + 1) % 360);
			}
			else
				return;
            
            if ( _vieStreamInfo.iMediaSignalEnable )
            {
                VideoSendStatePackets(1);
            }
		}

		if (_videoChannelID >= 0 && _stVideoRender.renderMode == 1)
		{
			float scale = VideoCalcScale();
			_vieCodec->SetDecodeFrame(_videoChannelID, scale);
		}
		if (_videoChannelID >= 0)
		{
			_vieCodec->RotateDecodedFrame(_videoChannelID, 
                    (_vieRemoteSendRotation + _vieRecivedRotaton) % 360);
		}
	}
	else if (m / 256 == 2)
	{
		int type = m % 256;
		//void *type1 =&type;
		evt_appand(eME_REMOTE_VIDEO_STATE_NOTIFY_EV, type, NULL, NULL, 0);
	}
};

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
	return (_voeBase->SetTestMode(_voiceChannelId, iMode));
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

#ifdef IOS_SET_FILEPATH
int Conductor::set_filepath(char *filepath)//add by wuzhaoyang20160407
{
	int iRet = -1;
	iRet = _voeVqe->Set_Filepath(filepath);
	iRet = _voeBase->Base_Set_Filepath(filepath);
	return iRet;
}
#endif
