#include "cfg.h"
#include "audio_device.h"
#include "MediaEngineInterface.h"

using namespace webrtc;

#ifdef EXTERNAL_AUDIO_DEVICE
#ifndef EXL_AUDIO_DEVICE_H
#define EXL_AUDIO_DEVICE_H
// ----------------------------------------------------------------------------
//  AudioDeviceModule
//
//  Implementation of the ADM to be used as external ADM in VoiceEngine.
//  This implementation is only a mock class, i.e., it does not provide
//  any real audio support.
// ----------------------------------------------------------------------------

#define MAXBUfSIZEBYTES		32767	//1s 16K 16bit data

class ExlAudioDeviceModuleImpl : public AudioDeviceModule {
 public:
  // Factory methods
  static ExlAudioDeviceModuleImpl* Create();
  static bool Destroy(ExlAudioDeviceModuleImpl* adm);

  // Helper methods which allows us to get some handy information about
  // this mock implementation.
  int32_t ReferenceCounter() const {
    return _ref_count;
  }

  // RefCountedModule implementation (mocks default implementation)
  virtual int32_t AddRef();
  virtual int32_t Release();

  // Module implementation
  virtual int32_t Version(char* version,
                          uint32_t& remaining_buffer_in_bytes,
                          uint32_t& position) const {
    return 0;
  }
  virtual int32_t ChangeUniqueId(const int32_t id) {
    return 0;
  }
  virtual int32_t TimeUntilNextProcess() {
    return -1;
  }
  virtual int32_t Process() {
    return 0;
  }

  // AudioDeviceModule implementation
  virtual int32_t ActiveAudioLayer(AudioLayer* audioLayer) const {
    return 0;
  }

  virtual ErrorCode LastError() const {
    return static_cast<ErrorCode> (0);
  }
  virtual int32_t RegisterEventObserver(AudioDeviceObserver* eventCallback) {
    return 0;
  }

  virtual int32_t RegisterAudioCallback(AudioTransport* audioCallback);

  virtual int32_t Init();
  virtual int32_t Terminate() {
    return 0;
  }
  virtual bool Initialized() const {
    return true;
  }

  virtual int16_t PlayoutDevices() {
    return -1;
  }
  virtual int16_t RecordingDevices() {
    return -1;
  }
  virtual int32_t PlayoutDeviceName(uint16_t index,
                                    char name[kAdmMaxDeviceNameSize],
                                    char guid[kAdmMaxGuidSize]) {
    return -1;
  }
  virtual int32_t RecordingDeviceName(uint16_t index,
                                      char name[kAdmMaxDeviceNameSize],
                                      char guid[kAdmMaxGuidSize]) {
    return -1;
  }

  virtual int32_t SetPlayoutDevice(uint16_t index) {
    return 0;
  }
  virtual int32_t SetPlayoutDevice(WindowsDeviceType device) {
    return 0;
  }
  virtual int32_t SetRecordingDevice(uint16_t index) {
    return 0;
  }
  virtual int32_t SetRecordingDevice(WindowsDeviceType device) {
    return 0;
  }

  virtual int32_t PlayoutIsAvailable(bool* available) {
    *available = true;
    return 0;
  }
  virtual int32_t InitPlayout() {
    return 0;
  }
  virtual bool PlayoutIsInitialized() const {
    return true;
  }
  virtual int32_t RecordingIsAvailable(bool* available) {
    *available = true;
    return 0;
  }
  virtual int32_t InitRecording() {
    return 0;
  }
  virtual bool RecordingIsInitialized() const {
    return true;
  }

  virtual int32_t StartPlayout() {
    return 0;
  }
  virtual int32_t StopPlayout() {
    return 0;
  }
  virtual bool Playing() const {
    return true;
  }
  virtual int32_t StartRecording() {
    return 0;
  }
  virtual int32_t StopRecording() {
    return 0;
  }
  virtual bool Recording() const {
    return true;
  }

  virtual int32_t SetAGC(bool enable) {
    return -1;
  }
  virtual bool AGC() const {
    return false;
  }

  virtual int32_t SetWaveOutVolume(uint16_t volumeLeft,
                                   uint16_t volumeRight) {
    return -1;
  }
  virtual int32_t WaveOutVolume(uint16_t* volumeLeft,
                                uint16_t* volumeRight) const {
    return -1;
  }

  virtual int32_t SpeakerIsAvailable(bool* available) {
    *available = true;
    return 0;
  }
  virtual int32_t InitSpeaker() {
    return 0;
  }
  virtual bool SpeakerIsInitialized() const {
    return true;
  }
  virtual int32_t MicrophoneIsAvailable(bool* available) {
    *available = true;
    return 0;
  }
  virtual int32_t InitMicrophone() {
    return 0;
  }
  virtual bool MicrophoneIsInitialized() const {
    return true;
  }

  virtual int32_t SpeakerVolumeIsAvailable(bool* available) {
    return -1;
  }
  virtual int32_t SetSpeakerVolume(uint32_t volume) {
    return -1;
  }
  virtual int32_t SpeakerVolume(uint32_t* volume) const {
    return -1;
  }
  virtual int32_t MaxSpeakerVolume(uint32_t* maxVolume) const {
    return -1;
  }
  virtual int32_t MinSpeakerVolume(uint32_t* minVolume) const {
    return -1;
  }
  virtual int32_t SpeakerVolumeStepSize(uint16_t* stepSize) const {
    return -1;
  }

  virtual int32_t MicrophoneVolumeIsAvailable(bool* available) {
    return -1;
  }
  virtual int32_t SetMicrophoneVolume(uint32_t volume) {
    return -1;
  }
  virtual int32_t MicrophoneVolume(uint32_t* volume) const {
    return -1;
  }
  virtual int32_t MaxMicrophoneVolume(uint32_t* maxVolume) const {
    return -1;
  }
  virtual int32_t MinMicrophoneVolume(uint32_t* minVolume) const {
    return -1;
  }
  virtual int32_t MicrophoneVolumeStepSize(uint16_t* stepSize) const {
    return -1;
  }

  virtual int32_t SpeakerMuteIsAvailable(bool* available) {
    return -1;
  }
  virtual int32_t SetSpeakerMute(bool enable) {
    return -1;
  }
  virtual int32_t SpeakerMute(bool* enabled) const {
    return -1;
  }

  virtual int32_t MicrophoneMuteIsAvailable(bool* available) {
    return -1;
  }
  virtual int32_t SetMicrophoneMute(bool enable) {
    return -1;
  }
  virtual int32_t MicrophoneMute(bool* enabled) const {
    return -1;
  }

  virtual int32_t MicrophoneBoostIsAvailable(bool* available) {
    return -1;
  }
  virtual int32_t SetMicrophoneBoost(bool enable) {
    return -1;
  }
  virtual int32_t MicrophoneBoost(bool* enabled) const {
    return -1;
  }

  virtual int32_t StereoPlayoutIsAvailable(bool* available) const {
    return -1;
  }
  virtual int32_t SetStereoPlayout(bool enable) {
    return -1;
  }
  virtual int32_t StereoPlayout(bool* enabled) const {
    return -1;
  }
  virtual int32_t StereoRecordingIsAvailable(bool* available) const {
    return -1;
  }
  virtual int32_t SetStereoRecording(bool enable) {
    return -1;
  }
  virtual int32_t StereoRecording(bool* enabled) const {
    return -1;
  }
  virtual int32_t SetRecordingChannel(const ChannelType channel) {
    return -1;
  }
  virtual int32_t RecordingChannel(ChannelType* channel) const {
    return -1;
  }

  virtual int32_t SetPlayoutBuffer(const BufferType type, uint16_t sizeMS = 0) {
    return -1;
  }
  virtual int32_t PlayoutBuffer(BufferType* type, uint16_t* sizeMS) const {
    return -1;
  }
  virtual int32_t PlayoutDelay(uint16_t* delayMS) const {
    return -1;
  }
  virtual int32_t RecordingDelay(uint16_t* delayMS) const {
    return -1;
  }

  virtual int32_t CPULoad(uint16_t* load) const {
    return -1;
  }

  virtual int32_t StartRawOutputFileRecording(
      const char pcmFileNameUTF8[kAdmMaxFileNameSize]) {
    return -1;
  }
  virtual int32_t StopRawOutputFileRecording() {
    return -1;
  }
  virtual int32_t StartRawInputFileRecording(
      const char pcmFileNameUTF8[kAdmMaxFileNameSize]) {
    return -1;
  }
  virtual int32_t StopRawInputFileRecording() {
    return -1;
  }

  virtual int32_t SetRecordingSampleRate(const uint32_t samplesPerSec) {
    return -1;
  }
  virtual int32_t RecordingSampleRate(uint32_t* samplesPerSec) const {
    return -1;
  }
  virtual int32_t SetPlayoutSampleRate(const uint32_t samplesPerSec) {
    return -1;
  }
  virtual int32_t PlayoutSampleRate(uint32_t* samplesPerSec) const {
    return -1;
  }

  virtual int32_t ResetAudioDevice() {
    return -1;
  }
  virtual int32_t SetLoudspeakerStatus(bool enable) {
    return -1;
  }
  virtual int32_t GetLoudspeakerStatus(bool* enabled) const {
    return -1;
  }

  int readRecord(ME_exl_pcm_output_itf_t* stOutput);
  int writeRecord(ME_exl_pcm_input_itf_t* stInput);
  int readPlayout(ME_exl_pcm_output_itf_t* stOutput);
  int writePlayout(ME_exl_pcm_input_itf_t* stInput);

 protected:
  ExlAudioDeviceModuleImpl();
  ~ExlAudioDeviceModuleImpl();

 private:
	bool verifyRecBufIn(int iIn);
	bool verifyRecBufOut(int iOut);
	bool verifyPlayBufIn(int iIn);
	bool verifyPlayBufOut(int iOut);

	WebRtc_Word32 SetRecordedBuffer(const void* audioBuffer,
	                            WebRtc_UWord32 nSamples);
	WebRtc_Word32 DeliverRecordedData();
	WebRtc_Word32 RequestPlayoutData(WebRtc_UWord32 nSamples);

	static bool ThreadFunc(void*);
    bool ThreadProcess();

    ThreadWrapper*                          _ptrThread;
    WebRtc_UWord32                          _threadID;

    volatile int32_t _ref_count;
	AudioTransport*                 _ptrCbAudioTransport;

    WebRtc_UWord32                  _recSampleRate;
    WebRtc_UWord32                  _playSampleRate;

    WebRtc_UWord8                   _recChannels;
    WebRtc_UWord8                   _playChannels;

    // 2 or 4 depending on mono or stereo
    WebRtc_UWord8                   _recBytesPerSample;
    WebRtc_UWord8                   _playBytesPerSample;

    int8_t                          _recBuffer[MAXBUfSIZEBYTES];
    int32							_iRecRd;
    int32							_iRecWr;
    WebRtc_UWord32                  _recSamples;    // one sample <=> 2 or 4 bytes
    WebRtc_UWord32                  _recSize;           // in bytes

    int8_t                          _playBuffer[MAXBUfSIZEBYTES];
    int32							_iPlayRd;
    int32							_iPlayWr;
    WebRtc_UWord32                  _playSamples;    // one sample <=> 2 or 4 bytes
    WebRtc_UWord32                  _playSize;          // in bytes	

    WebRtc_UWord32                  _FrameSize;          // in bytes	
};

#endif
#endif
