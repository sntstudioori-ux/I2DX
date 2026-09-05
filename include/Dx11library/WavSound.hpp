#pragma once

#include <windows.h>
#include <vector>

struct IXAudio2SourceVoice;

namespace library {

class AudioEngine;

// Represents a PCM / Wave audio file loaded in memory and played via XAudio2.
class WavSound
{
public:
    WavSound();
    ~WavSound();

    bool LoadFile(AudioEngine &engine, const wchar_t *path);
    bool LoadFile(AudioEngine &engine, const char *path);
    bool LoadMemory(AudioEngine &engine, const void *data, unsigned int size);
    void Release();

    bool Play(bool loop = false);
<<<<<<< HEAD
=======
	bool PlayLoop(double loopStartSeconds, double loopEndSeconds);
>>>>>>> abb6f4c (v1.01)
    void Stop();
    void Pause();
    void Resume();
    void SetVolume(float volume);

    bool IsPlaying() const;
    bool IsLooping() const { return loop_; }
    float Volume() const { return volume_; }

private:
    IXAudio2SourceVoice *sourceVoice_;
    std::vector<unsigned char> audioData_;
    WAVEFORMATEX waveFormat_;
    float volume_;
    bool loop_;
    bool paused_;

    WavSound(const WavSound &);
    WavSound &operator=(const WavSound &);
};

} // namespace library
