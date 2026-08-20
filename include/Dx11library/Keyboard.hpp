#pragma once

#include <windows.h>

namespace library {

// Common virtual-key codes. Keyboard also accepts any Win32 VK_* value
// directly, so this list does not limit the keys an application can use.
enum class Key : unsigned int
{
    Backspace = VK_BACK,
    Tab = VK_TAB,
    Enter = VK_RETURN,
    Escape = VK_ESCAPE,
    Space = VK_SPACE,
    Left = VK_LEFT,
    Up = VK_UP,
    Right = VK_RIGHT,
    Down = VK_DOWN,
    Shift = VK_SHIFT,
    Control = VK_CONTROL,
    Alt = VK_MENU,
    F1 = VK_F1,
    F2 = VK_F2,
    F3 = VK_F3,
    F4 = VK_F4,
    F5 = VK_F5,
    F6 = VK_F6,
    F7 = VK_F7,
    F8 = VK_F8,
    F9 = VK_F9,
    F10 = VK_F10,
    F11 = VK_F11,
    F12 = VK_F12,
};

// Lightweight per-frame keyboard state.
class Keyboard
{
public:
    Keyboard();

    // window may be null to read the system keyboard without focus filtering.
    bool Initialize(HWND window = 0);
    void Shutdown();
    void Update();

    bool IsDown(unsigned int virtualKey) const;
    bool WasPressed(unsigned int virtualKey) const;
    bool WasReleased(unsigned int virtualKey) const;

    bool IsDown(Key key) const { return IsDown(static_cast<unsigned int>(key)); }
    bool WasPressed(Key key) const { return WasPressed(static_cast<unsigned int>(key)); }
    bool WasReleased(Key key) const { return WasReleased(static_cast<unsigned int>(key)); }

    HWND Window() const { return window_; }
    bool IsInitialized() const { return initialized_; }

private:
    enum { kKeyCount = 256 };

    HWND window_;
    bool initialized_;
    bool current_[kKeyCount];
    bool previous_[kKeyCount];

    Keyboard(const Keyboard &);
    Keyboard &operator=(const Keyboard &);
};

} // namespace library
