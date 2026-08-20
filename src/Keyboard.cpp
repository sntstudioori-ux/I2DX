#include "Dx11library/Keyboard.hpp"

#include <string.h>

namespace library {

Keyboard::Keyboard() : window_(0), initialized_(false)
{
    memset(current_, 0, sizeof(current_));
    memset(previous_, 0, sizeof(previous_));
}

bool Keyboard::Initialize(HWND window)
{
    Shutdown();
    window_ = window;
    initialized_ = true;
    return true;
}

void Keyboard::Shutdown()
{
    window_ = 0;
    initialized_ = false;
    memset(current_, 0, sizeof(current_));
    memset(previous_, 0, sizeof(previous_));
}

void Keyboard::Update()
{
    if (!initialized_) return;

    memcpy(previous_, current_, sizeof(current_));

    // A window-bound keyboard should not retain keys after losing focus.
    const bool focused = !window_ || GetForegroundWindow() == window_;
    for (unsigned int key = 0; key < kKeyCount; ++key) {
        current_[key] = focused && (GetAsyncKeyState(static_cast<int>(key)) & 0x8000) != 0;
    }
}

bool Keyboard::IsDown(unsigned int virtualKey) const
{
    return virtualKey < kKeyCount && current_[virtualKey];
}

bool Keyboard::WasPressed(unsigned int virtualKey) const
{
    return virtualKey < kKeyCount && current_[virtualKey] && !previous_[virtualKey];
}

bool Keyboard::WasReleased(unsigned int virtualKey) const
{
    return virtualKey < kKeyCount && !current_[virtualKey] && previous_[virtualKey];
}

} // namespace library
