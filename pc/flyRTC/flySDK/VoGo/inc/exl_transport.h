/******************************************************************************

  Copyright (C), 2001-2011, DCN Co., Ltd.

 ******************************************************************************
  File Name     : external_transport.h
  Version       : Initial Draft
  Author        : gonghuojin
  Created       : 2013/11/5
  Last Modified :
  Description   : external transport for ice
  Function List :
  History       :
  1.Date        : 2013/11/5
    Author      : gonghuojin
    Modification: Created file

******************************************************************************/
#ifndef EXTERNAL_TRANSPORT_T
#define EXTERNAL_TRANSPORT_T

#include "cfg.h"

#ifdef EXTERNAL_TRANSPORT
#include "common_types.h"

using namespace webrtc;

enum tag_exmedia_type
{
	eTR_AUDIO   = 0,
	eTR_VIDEO   = 1
};


enum tag_exdata_type
{
	eTR_RTP   = 0,
	eTR_RTCP  = 1
};

typedef int (*exsend_data_cb_f)(int media_type, int data_type, const void* data, int len);

class CExternalTransportImpl : public Transport
{
public:
	// 构造函数
	CExternalTransportImpl(void);

	// 析构函数
	virtual ~CExternalTransportImpl(void);

	//回调函数
	int RegisterSendCallback(exsend_data_cb_f f_cb);

	int SetAudioChannel(int channel);

	int SetVideoChannel(int channel);

	// 发送函数
	virtual int SendPacket(int channel, const void *data, size_t size);

	virtual int SendRTCPPacket(int channel, const void *data, size_t size);


private:
	int audio_chnnl;
	int video_chnnl;
	exsend_data_cb_f send_cb;

};


#endif

#endif /* EXTERNAL_TRANSPORT_T */





