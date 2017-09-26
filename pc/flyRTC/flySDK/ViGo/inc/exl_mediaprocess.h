/******************************************************************************

  Copyright (C), 2001-2015, DCN Co., Ltd.

 ******************************************************************************
  File Name     : ext_mediaprocess.h
  Version       : Initial Draft
  Author        : Vinton.Liu
  Created       : 2015/11/26
  Last Modified :
  Description   : external VoE media process
  Function List :
  History       :
  1.Date        : 2013/11/26
    Author      : Vinton.Liu
    Modification: Created file

******************************************************************************/
#ifndef __EXTERNAL_MEDIA_PROCESS_H__
#define __EXTERNAL_MEDIA_PROCESS_H__

#include "webrtc/voice_engine/include/voe_external_media.h"
using namespace webrtc;

typedef void(*ex_media_proc_cb_f)(int16_t* audio_10ms,
                                  int samples,
                                  int samplingFreqHz,
                                  bool isStereo,
                                  int16_t *audio_out,
                                  int *samples_out);

// ----------------------------------------------------------------------------
//    VoEMediaProcess
// ----------------------------------------------------------------------------
class MediaProcessImpl : public VoEMediaProcess
{
public:
    MediaProcessImpl();
    ~MediaProcessImpl(){}
    void Process(int channel, //channelId
                 ProcessingTypes type,
                 int16_t audio_10ms[],
                 int length, //samples per channel
                 int samplingFreqHz,
                 bool stereo);

    int SetAudioChannelId(int channel) { channelId = channel; return channelId; }
    int RegisterExternMediaProcessingCb(ex_media_proc_cb_f cb);

private:
    int channelId;    
    ex_media_proc_cb_f ex_media_proc_cb;
};
#endif /* __EXTERNAL_MEDIA_PROCESS_H__ */