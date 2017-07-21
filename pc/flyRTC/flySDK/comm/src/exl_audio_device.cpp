#include "../inc/exl_audio_device.h"

// ----------------------------------------------------------------------------
// External AudioDeviceModule implementation
// ----------------------------------------------------------------------------
#ifdef EXTERNAL_AUDIO_DEVICE

// static
ExlAudioDeviceModuleImpl* ExlAudioDeviceModuleImpl::Create() {
  ExlAudioDeviceModuleImpl* xADM = new ExlAudioDeviceModuleImpl();
  if (xADM)
    xADM->AddRef();
  return xADM;
}

int32_t ExlAudioDeviceModuleImpl::Init()
{
    if (_ptrThread)
    {
        // thread is already created and active
        return 0;
    }

    const char* threadName = "webrtc_audio_module_thread";
    _ptrThread = ThreadWrapper::CreateThread(ThreadFunc, 
                                             this, 
                                             kRealtimePriority,
                                             threadName);
    if (_ptrThread == NULL)
    {
        WEBRTC_TRACE(kTraceCritical, kTraceAudioDevice, _id,
                     "failed to create the audio thread");
        return -1;
    }

    unsigned int threadID(0);
    if (!_ptrThread->Start(threadID))
    {
        WEBRTC_TRACE(kTraceCritical, kTraceAudioDevice, _id,
                     "failed to start the audio thread");
        delete _ptrThread;
        _ptrThread = NULL;
        return -1;
    }
    _threadID = threadID;

    const bool periodic(true);
    if (!_timeEvent.StartTimer(periodic, TIMER_PERIOD_MS))
    {
        WEBRTC_TRACE(kTraceCritical, kTraceAudioDevice, _id,
                     "failed to start the timer event");
        if (_ptrThread->Stop())
        {
            delete _ptrThread;
            _ptrThread = NULL;
        }
        else
        {
            WEBRTC_TRACE(kTraceWarning, kTraceAudioDevice, _id,
                         "unable to stop the activated thread");
        }
        return -1;
    }
    WEBRTC_TRACE(kTraceInfo, kTraceAudioDevice, _id,
                 "periodic timer (dT=%d) is now active", TIMER_PERIOD_MS);
}

// static
bool ExlAudioDeviceModuleImpl::Destroy(ExlAudioDeviceModuleImpl* adm) {
  if (!adm)
    return false;
  int32_t count = adm->Release();
  if (count != 0) {
    return false;
  } else {
    delete adm;
    return true;
  }

    _mixerManager.Close();

    if (_ptrThread)
    {
        ThreadWrapper* tmpThread = _ptrThread;
        _ptrThread = NULL;

        tmpThread->SetNotAlive();
        _timeEvent.Set();

        if (tmpThread->Stop())
        {
            delete tmpThread;
        }
        else
        {
            WEBRTC_TRACE(kTraceWarning, kTraceAudioDevice, _id,
                         "failed to close down the audio thread");
            return -1;
        }
    }
}

ExlAudioDeviceModuleImpl::ExlAudioDeviceModuleImpl() :
  _ref_count(0) {
    memset(_recBuffer, 0, kMaxBufferSizeBytes);
    memset(_playBuffer, 0, kMaxBufferSizeBytes);   
}

ExlAudioDeviceModuleImpl::~ExlAudioDeviceModuleImpl() {
}

int32_t ExlAudioDeviceModuleImpl::AddRef() {
  return ++_ref_count;
}

// ----------------------------------------------------------------------------
//  RegisterAudioCallback
// ----------------------------------------------------------------------------
WebRtc_Word32 ExlAudioDeviceModuleImpl::RegisterAudioCallback(AudioTransport* audioCallback)
{
    _ptrCbAudioTransport = audioCallback;
    return 0;
}

// ----------------------------------------------------------------------------
//  SetRecordedBuffer
//
//  Store recorded audio buffer in local memory ready for the actual
//  "delivery" using a callback.
//
//  This method can also parse out left or right channel from a stereo
//  input signal, i.e., emulate mono.
//
//  Examples:
//
//  16-bit,48kHz mono,  10ms => nSamples=480 => _recSize=2*480=960 bytes
//  16-bit,48kHz stereo,10ms => nSamples=480 => _recSize=4*480=1920 bytes
// ----------------------------------------------------------------------------

WebRtc_Word32 ExlAudioDeviceModuleImpl::SetRecordedBuffer(const void* audioBuffer,
                                                   WebRtc_UWord32 nSamples)
{
    CriticalSectionScoped lock(&_critSect);

    if (_recBytesPerSample == 0)
    {
        assert(false);
        return -1;
    }

    _recSamples = nSamples;
    _recSize = _recBytesPerSample*nSamples; // {2,4}*nSamples
    if (_recSize > kMaxBufferSizeBytes)
    {
        assert(false);
        return -1;
    }

    if (nSamples != _recSamples)
    {
        WEBRTC_TRACE(kTraceWarning, kTraceAudioDevice, _id, "invalid number of recorded samples (%d)", nSamples);
        return -1;
    }

    if (_recChannel == AudioDeviceModule::kChannelBoth)
    {
        // (default) copy the complete input buffer to the local buffer
        memcpy(&_recBuffer[0], audioBuffer, _recSize);
    }
    else
    {
        WebRtc_Word16* ptr16In = (WebRtc_Word16*)audioBuffer;
        WebRtc_Word16* ptr16Out = (WebRtc_Word16*)&_recBuffer[0];

        if (AudioDeviceModule::kChannelRight == _recChannel)
        {
            ptr16In++;
        }

        // exctract left or right channel from input buffer to the local buffer
        for (WebRtc_UWord32 i = 0; i < _recSamples; i++)
        {
            *ptr16Out = *ptr16In;
            ptr16Out++;
            ptr16In++;
            ptr16In++;
        }
    }

    if (_recFile.Open())
    {
        // write to binary file in mono or stereo (interleaved)
        _recFile.Write(&_recBuffer[0], _recSize);
    }

    return 0;
}

// ----------------------------------------------------------------------------
//  DeliverRecordedData
// ----------------------------------------------------------------------------

WebRtc_Word32 ExlAudioDeviceModuleImpl::DeliverRecordedData()
{
    CriticalSectionScoped lock(&_critSectCb);

    // Ensure that user has initialized all essential members
    if ((_recSampleRate == 0)     ||
        (_recSamples == 0)        ||
        (_recBytesPerSample == 0) ||
        (_recChannels == 0))
    {
        assert(false);
        return -1;
    }

    if (_ptrCbAudioTransport == NULL)
    {
        WEBRTC_TRACE(kTraceWarning, kTraceAudioDevice, _id, "failed to deliver recorded data (AudioTransport does not exist)");
        return 0;
    }

    WebRtc_Word32 res(0);
    WebRtc_UWord32 newMicLevel(0);
    WebRtc_UWord32 totalDelayMS = _playDelayMS +_recDelayMS;

    if (_measureDelay)
    {
        CriticalSectionScoped lock(&_critSect);

        memset(&_recBuffer[0], 0, _recSize);
        WebRtc_UWord32 time = AudioDeviceUtility::GetTimeInMS();
        if (time - _lastPulseTime > 500)
        {
            _pulseList.PushBack(time);
            _lastPulseTime = time;

            WebRtc_Word16* ptr16 = (WebRtc_Word16*)&_recBuffer[0];
            *ptr16 = 30000;
        }
    }
#ifdef TIME_DEBUG
	WebRtc_Word64 startTime = TickTime::MicrosecondTimestamp();
	count ++;
#endif
    res = _ptrCbAudioTransport->RecordedDataIsAvailable(&_recBuffer[0],
                                                        _recSamples,
                                                        _recBytesPerSample,
                                                        _recChannels,
                                                        _recSampleRate,
                                                        totalDelayMS,
                                                        _clockDrift,
                                                        _currentMicLevel,
                                                        newMicLevel);
#ifdef TIME_DEBUG
	WebRtc_Word64 endTime = TickTime::MicrosecondTimestamp();
	totalTime+=(endTime - startTime);
	avrageTime = totalTime/count;
	if(count%200 == 0)
		__android_log_print(ANDROID_LOG_DEBUG, "time", "apm+encode = %lld count = %lld",avrageTime,count);
	
#endif
    if (res != -1)
    {
        _newMicLevel = newMicLevel;
    }

    return 0;
}

// ----------------------------------------------------------------------------
//  RequestPlayoutData
// ----------------------------------------------------------------------------

WebRtc_Word32 ExlAudioDeviceModuleImpl::RequestPlayoutData(WebRtc_UWord32 nSamples)
{
    {
        CriticalSectionScoped lock(&_critSect);

        // Ensure that user has initialized all essential members
        if ((_playBytesPerSample == 0) ||
            (_playChannels == 0)       ||
            (_playSampleRate == 0))
        {
            assert(false);
            return -1;
        }

        _playSamples = nSamples;
        _playSize = _playBytesPerSample * nSamples;  // {2,4}*nSamples
        if (_playSize > kMaxBufferSizeBytes)
        {
            assert(false);
            return -1;
        }

        if (nSamples != _playSamples)
        {
            WEBRTC_TRACE(kTraceWarning, kTraceAudioDevice, _id, "invalid number of samples to be played out (%d)", nSamples);
            return -1;
        }
    }

    WebRtc_UWord32 nSamplesOut(0);

    CriticalSectionScoped lock(&_critSectCb);

    if (_ptrCbAudioTransport == NULL)
    {
        WEBRTC_TRACE(kTraceWarning, kTraceAudioDevice, _id, "failed to feed data to playout (AudioTransport does not exist)");
        return 0;
    }

    if (_ptrCbAudioTransport)
    {
        WebRtc_UWord32 res(0);

        res = _ptrCbAudioTransport->NeedMorePlayData(_playSamples,
                                                     _playBytesPerSample,
                                                     _playChannels,
                                                     _playSampleRate,
                                                     &_playBuffer[0],
                                                     nSamplesOut);
        if (res != 0)
        {
            WEBRTC_TRACE(kTraceError, kTraceAudioDevice, _id, "NeedMorePlayData() failed");
        }

        // --- Experimental delay-measurement implementation
        // *** not be used in released code ***

        if (_measureDelay)
        {
            CriticalSectionScoped lock(&_critSect);

            WebRtc_Word16 maxAbs = WebRtcSpl_MaxAbsValueW16((const WebRtc_Word16*)&_playBuffer[0], (WebRtc_Word16)nSamplesOut*_playChannels);
            if (maxAbs > 1000)
            {
                WebRtc_UWord32 nowTime = AudioDeviceUtility::GetTimeInMS();

                if (!_pulseList.Empty())
                {
                    ListItem* item = _pulseList.First();
                    if (item)
                    {
                        WebRtc_Word16 maxIndex = WebRtcSpl_MaxAbsIndexW16((const WebRtc_Word16*)&_playBuffer[0], (WebRtc_Word16)nSamplesOut*_playChannels);
                        WebRtc_UWord32 pulseTime = item->GetUnsignedItem();
                        WebRtc_UWord32 diff = nowTime - pulseTime + (10*maxIndex)/(nSamplesOut*_playChannels);
                        WEBRTC_TRACE(kTraceInfo, kTraceAudioDevice, _id, "diff time in playout delay (%d)", diff);
                    }
                    _pulseList.PopFront();
                }
            }
        }
    }

    return nSamplesOut;
}

bool ExlAudioDeviceModuleImpl::verifyRecBufIn(int iIn)
{
    return((_iRecRd-_iRecWr)&MAXBUfSIZEBYTES > iIn);
}

bool ExlAudioDeviceModuleImpl::verifyRecBufOut(int iOut)
{
    return((_iRecWr-_iRecRd)&MAXBUfSIZEBYTES > iOut);    
}

bool ExlAudioDeviceModuleImpl::verifyPlayBufIn(int iIn)
{
    return((_iPlayRd-_iPlayWr)&MAXBUfSIZEBYTES > iIn);    
}

bool ExlAudioDeviceModuleImpl::verifyPlayBufOut(int iOut)
{
    return((_iPlayWr-_iPlayRd)&MAXBUfSIZEBYTES > iOut);        
}
    
int ExlAudioDeviceModuleImpl::writeRecord(ME_exl_pcm_input_itf_t* stInput)
{
	void* 		  audioSamples;    	    //input buffer
	ucpaas_uint32 samples;       		//input count
	ucpaas_int8   bytesPerSample;      //byte count one sample
	ucpaas_uint32 samplesPerSec;  		//sampling rate 
	int iRet;
    if (verifyRecBufIn(stInput->samples))
    {
        memcpy(&_recBuffer[_iRecWr], stInput->audioSamples, stInput->samples);
        _iRecWr += stInput->audioSamples;
        _iRecWr &= MAXBUfSIZEBYTES;
        iRet = 0;
    }
    else
    {
        iRet = -1;
    }
    return iRet;
}
int ExlAudioDeviceModuleImpl::readRecord(ME_exl_pcm_output_itf_t* stOutput)
{

}

int ExlAudioDeviceModuleImpl::writePlayout(ME_exl_pcm_input_itf_t* stInput)
{

}
int ExlAudioDeviceModuleImpl::readPlayout(ME_exl_pcm_output_itf_t* stOutput)
{
	int iRet;
    if (verifyPlayBufOut(stOutput->samples))
    {
        memcpy(stOutput->audioSamples, &_PlayBuffer[_iRecWr], stOutput->samples);
        stOutput->SamplesOut = stOutput->samples;
        iRet = 0;
    }
    else
    {
        iRet = -1;
    }
    return iRet;
}

int32_t ExlAudioDeviceModuleImpl::Release() {
  // Avoid self destruction in this mock implementation.
  // Ensures that we can always check the reference counter while alive.
  return --_ref_count;
}

// ----------------------------------------------------------------------------
//  ThreadFunc
// ----------------------------------------------------------------------------

bool ExlAudioDeviceModuleImpl::ThreadFunc(void* pThis)
{
    return (static_cast<AudioDeviceWindowsWave*>(pThis)->ThreadProcess());
}

// ----------------------------------------------------------------------------
//  ThreadProcess
// ----------------------------------------------------------------------------

bool ExlAudioDeviceModuleImpl::ThreadProcess()
{
    WebRtc_UWord32 time(0);
    WebRtc_UWord32 playDiff(0);
    WebRtc_UWord32 recDiff(0);

    LONGLONG playTime(0);
    LONGLONG recTime(0);

    switch (_timeEvent.Wait(1000))
    {
    case kEventSignaled:
        break;
    case kEventError:
        WEBRTC_TRACE(kTraceWarning, kTraceAudioDevice, _id, "EventWrapper::Wait() failed => restarting timer");
        _timeEvent.StopTimer();
        _timeEvent.StartTimer(true, TIMER_PERIOD_MS);
        return true;
    case kEventTimeout:
        return true;
    }

    time = AudioDeviceUtility::GetTimeInMS();

    if (_startPlay)
    {
        if (PrepareStartPlayout() == 0)
        {
            _prevTimerCheckTime = time;
            _prevPlayTime = time;
            _startPlay = false;
            _playing = true;
            _playStartEvent.Set();
        }
    }

    if (_startRec)
    {
        if (PrepareStartRecording() == 0)
        {
            _prevTimerCheckTime = time;
            _prevRecTime = time;
            _prevRecByteCheckTime = time;
            _startRec = false;
            _recording = true;
            _recStartEvent.Set();
        }
    }

    if (_playing)
    {
        playDiff = time - _prevPlayTime;
    }

    if (_recording)
    {
        recDiff = time - _prevRecTime;
    }

    if (_playing || _recording)
    {
        RestartTimerIfNeeded(time);
    }

    if (_playing &&
        (playDiff > (WebRtc_UWord32)(_dTcheckPlayBufDelay - 1)) ||
        (playDiff < 0))
    {
        Lock();
        if (_playing)
        {
            if (PlayProc(playTime) == -1)
            {
                WEBRTC_TRACE(kTraceError, kTraceAudioDevice, _id, "PlayProc() failed");
            }
            _prevPlayTime = time;
            if (playTime != 0)
                _playAcc += playTime;
        }
        UnLock();
    }

    if (_playing && (playDiff > 12))
    {
        // It has been a long time since we were able to play out, try to
        // compensate by calling PlayProc again.
        //
        Lock();
        if (_playing)
        {
            if (PlayProc(playTime))
            {
                WEBRTC_TRACE(kTraceError, kTraceAudioDevice, _id, "PlayProc() failed");
            }
            _prevPlayTime = time;
            if (playTime != 0)
                _playAcc += playTime;
        }
        UnLock();
    }

    if (_recording &&
       (recDiff > REC_CHECK_TIME_PERIOD_MS) ||
       (recDiff < 0))
    {
        Lock();
        if (_recording)
        {
            WebRtc_Word32 nRecordedBytes(0);
            WebRtc_UWord16 maxIter(10);

            // Deliver all availiable recorded buffers and update the CPU load measurement.
            // We use a while loop here to compensate for the fact that the multi-media timer
            // can sometimed enter a "bad state" after hibernation where the resolution is
            // reduced from ~1ms to ~10-15 ms.
            //
            while ((nRecordedBytes = RecProc(recTime)) > 0)
            {
                maxIter--;
                _recordedBytes += nRecordedBytes;
                if (recTime && _perfFreq.QuadPart)
                {
                    // Measure the average CPU load:
                    // This is a simplified expression where an exponential filter is used:
                    //   _avgCPULoad = 0.99 * _avgCPULoad + 0.01 * newCPU,
                    //   newCPU = (recTime+playAcc)/f is time in seconds
                    //   newCPU / 0.01 is the fraction of a 10 ms period
                    // The two 0.01 cancels each other.
                    // NOTE - assumes 10ms audio buffers.
                    //
                    _avgCPULoad = (float)(_avgCPULoad*.99 + (recTime+_playAcc)/(double)(_perfFreq.QuadPart));
                    _playAcc = 0;
                }
                if (maxIter == 0)
                {
                    // If we get this message ofte, our compensation scheme is not sufficient.
                    WEBRTC_TRACE(kTraceInfo, kTraceAudioDevice, _id, "failed to compensate for reduced MM-timer resolution");
                }
            }

            if (nRecordedBytes == -1)
            {
                WEBRTC_TRACE(kTraceError, kTraceAudioDevice, _id, "RecProc() failed");
            }

            _prevRecTime = time;

            // Monitor the recording process and generate error/warning callbacks if needed
            MonitorRecording(time);
        }
        UnLock();
    }

    if (!_recording)
    {
        _prevRecByteCheckTime = time;
        _avgCPULoad = 0;
    }

    return true;
}
#endif
