#pragma once

#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>

namespace library {

struct Color
{
    Color(float red = 0.0f, float green = 0.0f, float blue = 0.0f, float alpha = 1.0f)
        : r(red), g(green), b(blue), a(alpha)
    {
    }

    float r;
    float g;
    float b;
    float a;
};

struct AppOptions
{
    AppOptions()
        : width(640), height(480), title(L"Th06 DirectX 11 application"), windowed(true), vsync(true), targetFps(60), antialias(true)
    {
    }

    unsigned int width;
    unsigned int height;
    const wchar_t *title;
    bool windowed;
    bool vsync;
    unsigned int targetFps;
    bool antialias;
};

// Owns a Win32 window, D3D11 device/context, swap chain, and back-buffer view.
class App
{
public:
    App();
    ~App();

    bool Initialize(HINSTANCE instance) { return Initialize(instance, AppOptions()); }
    bool Initialize(HINSTANCE instance, const AppOptions &options);
    void Shutdown();

    bool PumpMessages();
    bool BeginFrame(const Color &clearColor = Color());
    bool EndFrame();

    bool IsActive() const { return active_; }
    bool ShouldClose() const { return closing_; }
    unsigned int Width() const { return width_; }
    unsigned int Height() const { return height_; }
    float ViewportScale() const { return viewportScale_; }
    float ViewportOffsetX() const { return viewport_.TopLeftX; }
    float ViewportOffsetY() const { return viewport_.TopLeftY; }
    bool Antialias() const { return options_.antialias; }
    HWND Window() const { return window_; }
    ID3D11Device *Device() const { return device_; }
    ID3D11DeviceContext *Context() const { return context_; }
    IDXGISwapChain *SwapChain() const { return swapChain_; }


private:
    static LRESULT CALLBACK WindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
    bool CreateDeviceResources();
    bool CreateBackBufferView();
    bool Resize(unsigned int width, unsigned int height);
    void LimitFrameRate();
    void ReleaseBackBufferView();

    HINSTANCE instance_;
    HWND window_;
    ID3D11Device *device_;
    ID3D11DeviceContext *context_;
    IDXGISwapChain *swapChain_;
    ID3D11RenderTargetView *renderTargetView_;
    D3D11_VIEWPORT viewport_;
    AppOptions options_;
    unsigned int width_;
    unsigned int height_;
    unsigned int bufferWidth_;
    unsigned int bufferHeight_;
    float viewportScale_;
    LARGE_INTEGER frameCounterFrequency_;
    LARGE_INTEGER lastFrameCounter_;
    bool frameCounterStarted_;
    bool comInitialized_;
    bool active_;
    bool closing_;

    App(const App &);
    App &operator=(const App &);
};

} // namespace library (DirectX 11)
