#include "Dx11library/WavSound.hpp"
#include "Dx11library/AudioEngine.hpp"
#include "Dx11library/File.hpp"

#include <xaudio2.h>
#include <string.h>

namespace library {

namespace {

#pragma pack(push, 1)
struct RiffHeader
{
    char chunkId[4];
    unsigned int chunkSize;
    char format[4];
};

struct ChunkHeader
{
    char subChunkId[4];
    unsigned int subChunkSize;
};
#pragma pack(pop)

bool ParseWavBuffer(const unsigned char *data, unsigned int size,
                    WAVEFORMATEX *waveFormat, std::vector<unsigned char> *pcmData)
{
    if (!data || size < sizeof(RiffHeader) + sizeof(ChunkHeader) || !waveFormat || !pcmData) {
        return false;
    }

    const RiffHeader *riff = reinterpret_cast<const RiffHeader *>(data);
    if (memcmp(riff->chunkId, "RIFF", 4) != 0 || memcmp(riff->format, "WAVE", 4) != 0) {
        return false;
    }

    unsigned int offset = sizeof(RiffHeader);
    bool foundFmt = false;
    bool foundData = false;
    memset(waveFormat, 0, sizeof(WAVEFORMATEX));

    while (offset + sizeof(ChunkHeader) <= size) {
        const ChunkHeader *chunk = reinterpret_cast<const ChunkHeader *>(data + offset);
        offset += sizeof(ChunkHeader);

        if (offset + chunk->subChunkSize > size) {
            break;
        }

        if (memcmp(chunk->subChunkId, "fmt ", 4) == 0) {
            const unsigned int copySize = (chunk->subChunkSize < sizeof(WAVEFORMATEX))
                                              ? chunk->subChunkSize
                                              : static_cast<unsigned int>(sizeof(WAVEFORMATEX));
            memcpy(waveFormat, data + offset, copySize);
            foundFmt = true;
        } else if (memcmp(chunk->subChunkId, "data", 4) == 0) {
            pcmData->resize(chunk->subChunkSize);
            if (chunk->subChunkSize > 0) {
                memcpy(&(*pcmData)[0], data + offset, chunk->subChunkSize);
            }
            foundData = true;
        }

        offset += chunk->subChunkSize;
        if (foundFmt && foundData) break;
    }

    return foundFmt && foundData;
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
    if (!path) return false;

    char mbPath[MAX_PATH];
    WideCharToMultiByte(CP_ACP, 0, path, -1, mbPath, MAX_PATH, 0, 0);

    std::vector<unsigned char> rawData;
    if (!ReadBinaryFile(mbPath, &rawData)) {
        return false;
    }

    return LoadMemory(engine, rawData.empty() ? 0 : &rawData[0], static_cast<unsigned int>(rawData.size()));
}

bool WavSound::LoadFile(AudioEngine &engine, const char *path)
{
    Release();
    if (!path) return false;

    std::vector<unsigned char> rawData;
    if (!ReadBinaryFile(path, &rawData)) {
        return false;
    }

    return LoadMemory(engine, rawData.empty() ? 0 : &rawData[0], static_cast<unsigned int>(rawData.size()));
}

bool WavSound::LoadMemory(AudioEngine &engine, const void *data, unsigned int size)
{
    Release();
    if (!engine.IsInitialized() || !engine.XAudio2() || !data || size == 0) {
        return false;
    }

    const unsigned char *byteData = static_cast<const unsigned char *>(data);
    if (!ParseWavBuffer(byteData, size, &waveFormat_, &audioData_)) {
        Release();
        return false;
    }

    HRESULT hr = engine.XAudio2()->CreateSourceVoice(&sourceVoice_, &waveFormat_);
    if (FAILED(hr) || !sourceVoice_) {
        Release();
        return false;
    }

    sourceVoice_->SetVolume(volume_);
    return true;
}

void WavSound::Release()
{
    Stop();
    if (sourceVoice_) {
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
    if (!sourceVoice_ || audioData_.empty()) return false;

    sourceVoice_->Stop(0);
    sourceVoice_->FlushSourceBuffers();

    XAUDIO2_BUFFER buffer;
    memset(&buffer, 0, sizeof(buffer));
    buffer.AudioBytes = static_cast<UINT32>(audioData_.size());
    buffer.pAudioData = &audioData_[0];
    buffer.Flags = XAUDIO2_END_OF_STREAM;
    buffer.LoopCount = loop ? XAUDIO2_LOOP_INFINITE : 0;

    HRESULT hr = sourceVoice_->SubmitSourceBuffer(&buffer);
    if (FAILED(hr)) return false;

    hr = sourceVoice_->Start(0);
    if (FAILED(hr)) return false;

    loop_ = loop;
    paused_ = false;
    return true;
}

void WavSound::Stop()
{
    if (sourceVoice_) {
        sourceVoice_->Stop(0);
        sourceVoice_->FlushSourceBuffers();
    }
    paused_ = false;
}

void WavSound::Pause()
{
    if (sourceVoice_ && IsPlaying()) {
        sourceVoice_->Stop(0);
        paused_ = true;
    }
}

void WavSound::Resume()
{
    if (sourceVoice_ && paused_) {
        sourceVoice_->Start(0);
        paused_ = false;
    }
}

void WavSound::SetVolume(float volume)
{
    volume_ = (volume < 0.0f) ? 0.0f : ((volume > 1.0f) ? 1.0f : volume);
    if (sourceVoice_) {
        sourceVoice_->SetVolume(volume_);
    }
}

bool WavSound::IsPlaying() const
{
    if (!sourceVoice_) return false;
    XAUDIO2_VOICE_STATE state;
    sourceVoice_->GetState(&state);
    return state.BuffersQueued > 0;
}

} // namespace library
