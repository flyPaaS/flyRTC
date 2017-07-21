/******************************************************************************

  Copyright (C), 2001-2015, DCN Co., Ltd.

 ******************************************************************************
  File Name     : exl_mediaprocess.cpp
  Version       : Initial Draft
  Author        : Vinton.Liu
  Created       : 2015/11/26
  Last Modified :
  Description   : external VoE pcm media process
  Function List :
  History       :
  1.Date        : 2013/11/26
    Author      : Vinton.Liu
    Modification: Created file

******************************************************************************/

#include "exl_mediaprocess.h"
#include "webrtc/system_wrappers/interface/trace.h"
using namespace webrtc;

MediaProcessImpl::MediaProcessImpl()
{
    channelId = -1;
    ex_media_proc_cb = NULL;
}

void MediaProcessImpl::Process(int channel, 
                               ProcessingTypes type, 
                               int16_t audio_10ms [ ], 
                               int length, 
                               int samplingFreqHz, 
                               bool stereo)
{
    if ( channelId != channel )
    {
        WEBRTC_TRACE(kTraceWarning, kTraceVoice, channel,
                 "MediaProcessImpl::Process failed. channel(%d) != channelId(%d)",
                 channel, channelId);
        return;
    }
    
    if ( type == kRecordingPerChannel && ex_media_proc_cb != NULL)
    {
      char audio_out[160];
      int  samples_out = 0;
      ex_media_proc_cb((int16_t*)audio_10ms, length, samplingFreqHz, stereo, (int16_t *)audio_out, &samples_out);

      if (length == samples_out)
      {
        memcpy(audio_10ms, audio_out, samples_out);
      }
    }
}

int MediaProcessImpl::RegisterExternMediaProcessingCb(ex_media_proc_cb_f cb)
{
    ex_media_proc_cb = cb;
    return 0;
}
