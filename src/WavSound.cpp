#include "Dx11library/WavSound.hpp"
#include "Dx11library/AudioEngine.hpp"
#include "Dx11library/File.hpp"

#include <xaudio2.h>
#include <cmath>
#include <stdint.h>
#include <string.h>

namespace library
{

    namespace
    {

        unsigned int ReadUInt32(const unsigned char *data)
        {
            return static_cast<unsigned int>(data[0]) |
                   (static_cast<unsigned int>(data[1]) << 8) |
                   (static_cast<unsigned int>(data[2]) << 16) |
                   (static_cast<unsigned int>(data[3]) << 24);
        }

        bool ParseWavBuffer(const unsigned char *data, unsigned int size,
                            WAVEFORMATEX *waveFormat, std::vector<unsigned char> *pcmData)
        {
            if (!data || size < 12 || !waveFormat || !pcmData)
            {
                return false;
            }

            if (memcmp(data, "RIFF", 4) != 0 || memcmp(data + 8, "WAVE", 4) != 0)
            {
                return false;
            }

            const uint64_t riffEnd = 8ull + ReadUInt32(data + 4);
            if (riffEnd > size || riffEnd < 12)
            {
                return false;
            }

            uint64_t offset = 12;
            bool foundFmt = false;
            bool foundData = false;
            pcmData->clear();
            memset(waveFormat, 0, sizeof(WAVEFORMATEX));

            while (offset + 8 <= riffEnd)
            {
                const unsigned char *chunk = data + offset;
                const unsigned int chunkSize = ReadUInt32(chunk + 4);
                offset += 8;

                if (chunkSize > riffEnd - offset)
                {
                    return false;
                }

                if (memcmp(chunk, "fmt ", 4) == 0)
                {
                    if (chunkSize < 16)
                    {
                        return false;
                    }

                    const unsigned int copySize = (chunkSize < sizeof(WAVEFORMATEX))
                                                      ? chunkSize
                                                      : static_cast<unsigned int>(sizeof(WAVEFORMATEX));
                    memcpy(waveFormat, data + offset, copySize);
                    foundFmt = true;
                }
                else if (memcmp(chunk, "data", 4) == 0)
                {
                    if (chunkSize == 0)
                    {
                        return false;
                    }
                    pcmData->resize(chunkSize);
                    memcpy(&(*pcmData)[0], data + offset, chunkSize);
                    foundData = true;
                }

                offset += chunkSize;
                if ((chunkSize & 1u) != 0 && offset < riffEnd)
                {
                    ++offset;
                }
                if (foundFmt && foundData)
                    break;
            }

            return foundFmt && foundData && waveFormat->nBlockAlign != 0 &&
                   waveFormat->nSamplesPerSec != 0;
        }

    } // namespace

    WavSound::WavSound()
        : sourceVoice_(0), volume_(1.0f), loop_(false), paused_(false)
    {
        memset(&waveFormat_, 0, sizeof(waveFormat_));
    }

    WavSound::~WavSound()
    {
        Release();
    }

    bool WavSound::LoadFile(AudioEngine &engine, const wchar_t *path)
    {
        Release();
        if (!path)
            return false;

        std::vector<unsigned char> rawData;
        if (!ReadBinaryFile(path, &rawData))
        {
            return false;
        }

        return LoadMemory(engine, rawData.empty() ? 0 : &rawData[0], static_cast<unsigned int>(rawData.size()));
    }

    bool WavSound::LoadFile(AudioEngine &engine, const char *path)
    {
        Release();
        if (!path)
            return false;

        std::vector<unsigned char> rawData;
        if (!ReadBinaryFile(path, &rawData))
        {
            return false;
        }

        return LoadMemory(engine, rawData.empty() ? 0 : &rawData[0], static_cast<unsigned int>(rawData.size()));
    }

    bool WavSound::LoadMemory(AudioEngine &engine, const void *data, unsigned int size)
    {
        Release();
        if (!engine.IsInitialized() || !engine.XAudio2() || !data || size == 0)
        {
            return false;
        }

        const unsigned char *byteData = static_cast<const unsigned char *>(data);
        if (!ParseWavBuffer(byteData, size, &waveFormat_, &audioData_))
        {
            Release();
            return false;
        }

        HRESULT hr = engine.XAudio2()->CreateSourceVoice(&sourceVoice_, &waveFormat_);
        if (FAILED(hr) || !sourceVoice_)
        {
            Release();
            return false;
        }

        sourceVoice_->SetVolume(volume_);
        return true;
    }

    void WavSound::Release()
    {
        Stop();
        if (sourceVoice_)
        {
            sourceVoice_->DestroyVoice();
            sourceVoice_ = 0;
        }
        audioData_.clear();
        memset(&waveFormat_, 0, sizeof(waveFormat_));
        paused_ = false;
        loop_ = false;
    }

    bool WavSound::Play(bool loop)
    {
        if (!sourceVoice_ || audioData_.empty())
            return false;

        sourceVoice_->Stop(0);
        sourceVoice_->FlushSourceBuffers();

        XAUDIO2_BUFFER buffer;
        memset(&buffer, 0, sizeof(buffer));
        buffer.AudioBytes = static_cast<UINT32>(audioData_.size());
        buffer.pAudioData = &audioData_[0];
        buffer.Flags = XAUDIO2_END_OF_STREAM;
        buffer.LoopCount = loop ? XAUDIO2_LOOP_INFINITE : 0;

        HRESULT hr = sourceVoice_->SubmitSourceBuffer(&buffer);
        if (FAILED(hr))
            return false;

        hr = sourceVoice_->Start(0);
        if (FAILED(hr))
            return false;

        loop_ = loop;
        paused_ = false;
        return true;
    }

    bool WavSound::PlayLoop(double loopStartSeconds, double loopEndSeconds)
    {
        if (!sourceVoice_ || audioData_.empty())
            return false;

        if (!std::isfinite(loopStartSeconds) || !std::isfinite(loopEndSeconds) ||
            loopStartSeconds < 0.0 || loopEndSeconds <= loopStartSeconds ||
            waveFormat_.nSamplesPerSec == 0 || waveFormat_.nBlockAlign == 0)
        {
            return false;
        }

        const double sampleRate =
            static_cast<double>(waveFormat_.nSamplesPerSec);

        const double totalSamples =
            static_cast<double>(audioData_.size() / waveFormat_.nBlockAlign);
        const double loopBeginValue = loopStartSeconds * sampleRate;
        const double loopEndValue = loopEndSeconds * sampleRate;

        if (loopBeginValue < 0.0 || loopBeginValue >= totalSamples ||
            loopEndValue > totalSamples || loopEndValue <= loopBeginValue ||
            loopEndValue > static_cast<double>(UINT32_MAX))
        {
            return false;
        }

        const UINT32 loopBegin = static_cast<UINT32>(loopBeginValue);
        const UINT32 loopEnd = static_cast<UINT32>(loopEndValue);
        const UINT32 loopLength = loopEnd - loopBegin;
        if (loopLength == 0)
            return false;

        sourceVoice_->Stop(0);
        sourceVoice_->FlushSourceBuffers();

        XAUDIO2_BUFFER buffer;
        memset(&buffer, 0, sizeof(buffer));

        buffer.AudioBytes =
            static_cast<UINT32>(audioData_.size());

        buffer.pAudioData = &audioData_[0];

        buffer.Flags = XAUDIO2_END_OF_STREAM;

        // 追加する部分
        buffer.LoopBegin = loopBegin;
        buffer.LoopLength = loopLength;
        buffer.LoopCount = XAUDIO2_LOOP_INFINITE;

        HRESULT hr = sourceVoice_->SubmitSourceBuffer(&buffer);

        if (FAILED(hr))
        {
            return false;
        }

        hr = sourceVoice_->Start(0);

        if (FAILED(hr))
        {
            return false;
        }

        loop_ = true;
        paused_ = false;

        return true;
    }

    void WavSound::Stop()
    {
        if (sourceVoice_)
        {
            sourceVoice_->Stop(0);
            sourceVoice_->FlushSourceBuffers();
        }
        paused_ = false;
        loop_ = false;
    }

    void WavSound::Pause()
    {
        if (sourceVoice_ && IsPlaying())
        {
            sourceVoice_->Stop(0);
            paused_ = true;
        }
    }

    void WavSound::Resume()
    {
        if (sourceVoice_ && paused_)
        {
            if (SUCCEEDED(sourceVoice_->Start(0)))
            {
                paused_ = false;
            }
        }
    }

    void WavSound::SetVolume(float volume)
    {
        volume_ = (volume < 0.0f) ? 0.0f : ((volume > 1.0f) ? 1.0f : volume);
        if (sourceVoice_)
        {
            sourceVoice_->SetVolume(volume_);
        }
    }

    bool WavSound::IsPlaying() const
    {
        if (!sourceVoice_ || paused_)
            return false;
        XAUDIO2_VOICE_STATE state;
        sourceVoice_->GetState(&state);
        return state.BuffersQueued > 0;
    }

} // namespace library
