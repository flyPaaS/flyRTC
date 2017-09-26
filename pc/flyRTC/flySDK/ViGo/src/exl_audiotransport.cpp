/******************************************************************************

  Copyright (C), 2001-2016, DCN Co., Ltd.

 ******************************************************************************
  File Name     : ext_audiotransport.cc
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

#include <string.h>
#include "exl_audiotransport.h"
#include "webrtc/system_wrappers/interface/trace.h"

using namespace webrtc;

VoEAudioTransportImpl:: VoEAudioTransportImpl()
{
    callbacks_.initplayout_cb = NULL;
    callbacks_.initrecording_cb = NULL;
    callbacks_.pull_capture_cb = NULL;
    callbacks_.push_render_cb = NULL;
}

void VoEAudioTransportImpl::RegisterCallbackTable(AudioTransportApiCb_t *callbacks)
{
    memcpy(&callbacks_, callbacks, sizeof(AudioTransportApiCb_t));
}

void VoEAudioTransportImpl::InitRecording(uint32_t sample_rate, 
                            uint32_t bytes_per_sample,
                            uint32_t num_of_channels)
{
    if ( callbacks_.initrecording_cb )
    {
        callbacks_.initrecording_cb(sample_rate, bytes_per_sample, num_of_channels);
    }
}

void VoEAudioTransportImpl::InitPlayout(uint32_t sample_rate, 
                uint32_t bytes_per_sample,
                uint32_t num_of_channels)
{
    if ( callbacks_.initplayout_cb )
    {
        callbacks_.initplayout_cb(sample_rate, bytes_per_sample, num_of_channels);
    }
}

// Method to pull the capture audio data from the active VoE channels.
// The data will be processed by APM and send to network
int32_t VoEAudioTransportImpl::PullCaptureData(int8_t* audio_data, 
                           uint32_t nSize)
{
    if ( callbacks_.pull_capture_cb )
    {
        return callbacks_.pull_capture_cb(audio_data, nSize);
    }
    else
    {
        return -1;
    }
}

// Method to push the mixed render audio to the active VoE channel.
// The data have been processed by APM which received from network
int32_t VoEAudioTransportImpl::PushRenderData(const int8_t* audio_data, 
                           uint32_t nSize)
{
    if ( callbacks_.push_render_cb )
    {
        return callbacks_.push_render_cb(audio_data, nSize);
    }
    else
    {
        return -1;
    }
}

