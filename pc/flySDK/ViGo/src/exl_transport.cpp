#include "exl_transport.h"

#ifdef EXTERNAL_TRANSPORT

// 构造函数
CExternalTransportImpl::CExternalTransportImpl(void) 
{
	audio_chnnl = -1;
	video_chnnl = -1;
	send_cb  = NULL;
}

int CExternalTransportImpl::RegisterSendCallback(exsend_data_cb_f f_cb)
{
	send_cb = f_cb;
	return 0;
}

int CExternalTransportImpl::SetAudioChannel(int channel)
{
	audio_chnnl = channel;
	return 0;
}

int CExternalTransportImpl::SetVideoChannel(int channel)
{
	video_chnnl = channel;
	return 0;
}


// 发送RTP包
int CExternalTransportImpl::SendPacket(int channel, const void *data, size_t size)
{
    /* Modified By MNY@2015-01-19 */
    int res = 0;
    
    if (NULL != send_cb)
	{
		if (channel == audio_chnnl)
			res = send_cb(eTR_AUDIO, eTR_RTP, data, size);
		else
			res = send_cb(eTR_VIDEO, eTR_RTP, data, size);
	}
	return res == 0 ? size : res;
}

// 发送RTCP包
int CExternalTransportImpl::SendRTCPPacket(int channel, const void *data, size_t size)
{
    /* Modified By MNY@2015-01-19 */
    int res = 0;
    
    if (NULL != send_cb)
	{
		if (channel == audio_chnnl)
			res = send_cb(eTR_AUDIO, eTR_RTCP, data, size);
		else
			res = send_cb(eTR_VIDEO, eTR_RTCP, data, size);
	}
	return res == 0 ? size : res;
}

#endif

