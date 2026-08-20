#include "Dx11library/AudioEngine.hpp"

#include <xaudio2.h>

#pragma comment(lib, "xaudio2.lib")

namespace library {

namespace {
template <typename T>
void ReleaseCom(T *&object)
{
    if (object) {
        object->Release();
        object = 0;
    }
}
}

AudioEngine::AudioEngine()
    : xaudio2_(0), masteringVoice_(0), comInitialized_(false), initialized_(false)
{
}

AudioEngine::~AudioEngine()
{
    Shutdown();
}

bool AudioEngine::Initialize()
{
    Shutdown();

    const HRESULT comResult = CoInitializeEx(0, COINIT_MULTITHREADED);
    comInitialized_ = SUCCEEDED(comResult);
    if (FAILED(comResult) && comResult != RPC_E_CHANGED_MODE) {
        return false;
    }

    HRESULT hr = XAudio2Create(&xaudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
    if (FAILED(hr) || !xaudio2_) {
        Shutdown();
        return false;
    }

    hr = xaudio2_->CreateMasteringVoice(&masteringVoice_);
    if (FAILED(hr) || !masteringVoice_) {
        Shutdown();
        return false;
    }

    initialized_ = true;
    return true;
}

void AudioEngine::Shutdown()
{
    if (masteringVoice_) {
        masteringVoice_->DestroyVoice();
        masteringVoice_ = 0;
    }

    ReleaseCom(xaudio2_);

    if (comInitialized_) {
        CoUninitialize();
        comInitialized_ = false;
    }

    initialized_ = false;
}

} // namespace library
