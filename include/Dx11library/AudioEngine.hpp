#pragma once

#include <windows.h>

struct IXAudio2;
struct IXAudio2MasteringVoice;

namespace library {

// Manages the XAudio2 audio engine and mastering voice.
class AudioEngine
{
public:
    AudioEngine();
    ~AudioEngine();

    bool Initialize();
    void Shutdown();

    bool IsInitialized() const { return initialized_; }
    IXAudio2 *XAudio2() const { return xaudio2_; }
    IXAudio2MasteringVoice *MasteringVoice() const { return masteringVoice_; }

private:
    IXAudio2 *xaudio2_;
    IXAudio2MasteringVoice *masteringVoice_;
    bool comInitialized_;
    bool initialized_;

    AudioEngine(const AudioEngine &);
    AudioEngine &operator=(const AudioEngine &);
};

} // namespace library
