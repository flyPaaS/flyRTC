/******************************************************************************

  Copyright (C), 2001-2016, DCN Co., Ltd.

 ******************************************************************************
  File Name     : ext_audiotransport.h
  Version       : Initial Draft
  Author        : Vinton.Liu
  Created       : 2015/11/26
  Last Modified :
  Description   : external VoE audio transport
  Function List :
  History       :
  1.Date        : 2016/3/28
    Author      : Vinton.Liu
    Modification: Created file

******************************************************************************/
#ifndef __EXL_AUDIO_TRANSPORT_H__
#define __EXL_AUDIO_TRANSPORT_H__

#include "voe_base.h"

using namespace webrtc;

/* add by Vinton.Liu on 2016-03-18 for  support external audio device push pcm to and pull pcm from engine begin */
typedef void (*init_playout_cb_f)(uint32_t sample_rate, 
                                     uint32_t bytes_per_sample,
                                     uint32_t num_of_channels);

typedef void (*init_recording_cb_f)(uint32_t sample_rate, 
                                     uint32_t bytes_per_sample,
                                     uint32_t num_of_channels);

typedef int (*pull_capture_data_cb_f)(int8_t* audio_data, 
                                            uint32_t nsize);

typedef int (*push_render_data_cb_f)(const int8_t* audio_data, 
                                            uint32_t nsize);
/* Vinton.Liu add end */

typedef struct AudioTransportApiCb_Tag
{
    init_playout_cb_f initplayout_cb;            /* init playout param */
    init_recording_cb_f initrecording_cb;        /* init reocrding param */
    pull_capture_data_cb_f pull_capture_cb;      /* pull audio capture data */
    push_render_data_cb_f push_render_cb;        /* push audio playout data */
} AudioTransportApiCb_t;

class VoEAudioTransportImpl : public VoEAudioTransport
{
public:
    VoEAudioTransportImpl();
    ~VoEAudioTransportImpl() {}
    
    void RegisterCallbackTable(AudioTransportApiCb_t *callbacks);
    
    void InitRecording(uint32_t sample_rate, 
                            uint32_t bytes_per_sample,
                            uint32_t num_of_channels);

    void InitPlayout(uint32_t sample_rate, 
                    uint32_t bytes_per_sample,
                    uint32_t num_of_channels);

    // Method to pull the capture audio data from the active VoE channels.
    // The data will be processed by APM and send to network
    int32_t PullCaptureData(int8_t* audio_data, 
                               uint32_t nSize);

    // Method to push the mixed render audio to the active VoE channel.
    // The data have been processed by APM which received from network
    int32_t PushRenderData(const int8_t* audio_data, 
                               uint32_t nSize);
    
private:
    AudioTransportApiCb_t callbacks_;
};

#endif // __EXL_AUDIO_TRANSPORT_H__