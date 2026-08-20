#pragma once

#include <windows.h>
#include <string>

namespace library {

// Plays Standard MIDI Files (.mid) using the Windows MCI subsystem.
class MidiPlayer
{
public:
    MidiPlayer();
    ~MidiPlayer();

    bool LoadFile(const wchar_t *path);
    bool LoadFile(const char *path);
    bool LoadMemory(const void *data, unsigned int size);
    void Release();

    bool Play(bool loop = false);
    void Stop();
    void Pause();
    void Resume();
    void SetVolume(float volume);

    bool IsPlaying() const;
    bool IsLooping() const { return loop_; }
    float Volume() const { return volume_; }

    // Call periodically (e.g. in the main update loop) to handle loop repetition if looping.
    void Update();

private:
    std::wstring alias_;
    std::wstring tempFilePath_;
    float volume_;
    bool loop_;
    bool paused_;

    static unsigned int instanceCounter_;

    MidiPlayer(const MidiPlayer &);
    MidiPlayer &operator=(const MidiPlayer &);
};

} // namespace library
