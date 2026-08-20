#include "Dx11library/MidiPlayer.hpp"
#include "Dx11library/File.hpp"

#include <mmsystem.h>
#include <stdio.h>
#include <wchar.h>

#pragma comment(lib, "winmm.lib")

namespace library {

unsigned int MidiPlayer::instanceCounter_ = 0;

MidiPlayer::MidiPlayer()
    : volume_(1.0f), loop_(false), paused_(false)
{
}

MidiPlayer::~MidiPlayer()
{
    Release();
}

bool MidiPlayer::LoadFile(const wchar_t *path)
{
    if (!path || path[0] == L'\0') return false;
    std::wstring pathCopy = path;

    Release();

    wchar_t aliasBuffer[64];
    swprintf_s(aliasBuffer, 64, L"Th06Midi_%u_%u", GetCurrentProcessId(), ++instanceCounter_);
    alias_ = aliasBuffer;

    wchar_t command[1024];
    swprintf_s(command, 1024, L"open \"%s\" type sequencer alias %s", pathCopy.c_str(), alias_.c_str());

    MCIERROR err = mciSendStringW(command, 0, 0, 0);
    if (err != 0) {
        swprintf_s(command, 1024, L"open \"%s\" alias %s", pathCopy.c_str(), alias_.c_str());
        err = mciSendStringW(command, 0, 0, 0);
    }

    if (err != 0) {
        alias_.clear();
        return false;
    }

    SetVolume(volume_);
    return true;
}

bool MidiPlayer::LoadFile(const char *path)
{
    if (!path) return false;
    wchar_t wpath[MAX_PATH];
    MultiByteToWideChar(CP_ACP, 0, path, -1, wpath, MAX_PATH);
    return LoadFile(wpath);
}

bool MidiPlayer::LoadMemory(const void *data, unsigned int size)
{
    Release();
    if (!data || size == 0) return false;

    wchar_t tempDir[MAX_PATH];
    if (GetTempPathW(MAX_PATH, tempDir) == 0) return false;

    wchar_t tempFile[MAX_PATH];
    swprintf_s(tempFile, MAX_PATH, L"%sTh06Midi_%u_%u.mid", tempDir, GetCurrentProcessId(), ++instanceCounter_);

    FILE *f = 0;
    if (_wfopen_s(&f, tempFile, L"wb") != 0 || !f) {
        return false;
    }

    const bool written = (fwrite(data, 1, size, f) == size);
    fclose(f);

    if (!written) {
        DeleteFileW(tempFile);
        return false;
    }

    if (!LoadFile(tempFile)) {
        DeleteFileW(tempFile);
        return false;
    }

    tempFilePath_ = tempFile;
    return true;
}

void MidiPlayer::Release()
{
    Stop();
    if (!alias_.empty()) {
        MCI_GENERIC_PARMS genericParms;
        memset(&genericParms, 0, sizeof(genericParms));
        mciSendStringW((L"close " + alias_).c_str(), 0, 0, 0);
        alias_.clear();
    }

    if (!tempFilePath_.empty()) {
        DeleteFileW(tempFilePath_.c_str());
        tempFilePath_.clear();
    }

    paused_ = false;
    loop_ = false;
}

bool MidiPlayer::Play(bool loop)
{
    if (alias_.empty()) return false;

    std::wstring seekCmd = L"seek " + alias_ + L" to start";
    mciSendStringW(seekCmd.c_str(), 0, 0, 0);

    std::wstring playCmd = L"play " + alias_;
    MCIERROR err = mciSendStringW(playCmd.c_str(), 0, 0, 0);
    if (err != 0) return false;

    loop_ = loop;
    paused_ = false;
    return true;
}

void MidiPlayer::Stop()
{
    if (alias_.empty()) return;

    std::wstring stopCmd = L"stop " + alias_;
    mciSendStringW(stopCmd.c_str(), 0, 0, 0);

    std::wstring seekCmd = L"seek " + alias_ + L" to start";
    mciSendStringW(seekCmd.c_str(), 0, 0, 0);

    paused_ = false;
}

void MidiPlayer::Pause()
{
    if (alias_.empty()) return;

    std::wstring pauseCmd = L"pause " + alias_;
    mciSendStringW(pauseCmd.c_str(), 0, 0, 0);
    paused_ = true;
}

void MidiPlayer::Resume()
{
    if (alias_.empty()) return;

    std::wstring resumeCmd = L"resume " + alias_;
    mciSendStringW(resumeCmd.c_str(), 0, 0, 0);
    paused_ = false;
}

void MidiPlayer::SetVolume(float volume)
{
    volume_ = (volume < 0.0f) ? 0.0f : ((volume > 1.0f) ? 1.0f : volume);

    if (!alias_.empty()) {
        const int mciVol = static_cast<int>(volume_ * 1000.0f);
        wchar_t command[256];
        swprintf_s(command, 256, L"setaudio %s volume to %d", alias_.c_str(), mciVol);
        mciSendStringW(command, 0, 0, 0);
    }

    const DWORD midiVol = static_cast<DWORD>(volume_ * 0xFFFF);
    midiOutSetVolume(0, MAKELONG(midiVol, midiVol));
}

bool MidiPlayer::IsPlaying() const
{
    if (alias_.empty()) return false;

    wchar_t command[256];
    wchar_t returnString[128];
    swprintf_s(command, 256, L"status %s mode", alias_.c_str());

    if (mciSendStringW(command, returnString, 128, 0) == 0) {
        return (wcsstr(returnString, L"playing") != 0);
    }

    return false;
}

void MidiPlayer::Update()
{
    if (!alias_.empty() && loop_ && !paused_) {
        if (!IsPlaying()) {
            Play(true);
        }
    }
}

} // namespace library
