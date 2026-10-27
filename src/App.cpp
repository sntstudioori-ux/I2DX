#include "Dx11library/App.hpp"

#include <string.h>

namespace library {

namespace {
const wchar_t kWindowClass[] = L"Dx11libraryLibraryWindow";

template <typename T>
void Release(T *&object)
{
    if (object) {
        object->Release();
        object = 0;
    }
}
}

App::App()
    : instance_(0), window_(0), device_(0), context_(0), swapChain_(0), renderTargetView_(0),
      multisampleTexture_(0), multisampleRenderTargetView_(0), multisampleRasterizerState_(0),
      sampleCount_(1), multisampleResolved_(false),
      width_(0), height_(0), bufferWidth_(0), bufferHeight_(0), viewportScale_(1.0f),
      frameCounterStarted_(false), comInitialized_(false), active_(true), closing_(false)
{
    memset(&viewport_, 0, sizeof(viewport_));
    QueryPerformanceFrequency(&frameCounterFrequency_);
    lastFrameCounter_.QuadPart = 0;
}

App::~App() { Shutdown(); }

bool App::Initialize(HINSTANCE instance, const AppOptions &options)
{
    Shutdown();
    if (!instance || !options.width || !options.height || !options.title) return false;
    instance_ = instance;
    options_ = options;
    width_ = options.width;
    height_ = options.height;
    bufferWidth_ = width_;
    bufferHeight_ = height_;
    frameCounterStarted_ = false;
    active_ = true;
    closing_ = false;

    const HRESULT comResult = CoInitializeEx(0, COINIT_APARTMENTTHREADED);
    comInitialized_ = SUCCEEDED(comResult);
    if (FAILED(comResult) && comResult != RPC_E_CHANGED_MODE) return false;

    WNDCLASSW windowClass;
    memset(&windowClass, 0, sizeof(windowClass));
    windowClass.hInstance = instance_;
    windowClass.hCursor = LoadCursor(0, IDC_ARROW);
    windowClass.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
    windowClass.lpfnWndProc = WindowProc;
    windowClass.lpszClassName = kWindowClass;
    if (!RegisterClassW(&windowClass) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
        Shutdown();
        return false;
    }

    const bool exclusive = !options_.windowed && options_.fullscreenMode == FullscreenMode::Exclusive;
    bool exclusiveModeAvailable = exclusive;
    const DWORD style = options_.windowed ? (WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX) : WS_POPUP;
    RECT rect = { 0, 0, static_cast<LONG>(width_), static_cast<LONG>(height_) };
    int x = CW_USEDEFAULT;
    int y = CW_USEDEFAULT;
    if (options_.windowed) {
        AdjustWindowRect(&rect, style, FALSE);
    } else {
        MONITORINFOEXW monitor = {};
        monitor.cbSize = sizeof(monitor);
        HMONITOR display = MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY);
        GetMonitorInfoW(display, &monitor);
        rect = monitor.rcMonitor;
        x = rect.left;
        y = rect.top;
        DEVMODEW currentMode = {};
        currentMode.dmSize = sizeof(currentMode);
        if (EnumDisplaySettingsW(monitor.szDevice, ENUM_CURRENT_SETTINGS, &currentMode) &&
            currentMode.dmPelsWidth && currentMode.dmPelsHeight) {
            bufferWidth_ = currentMode.dmPelsWidth;
            bufferHeight_ = currentMode.dmPelsHeight;
        } else {
            bufferWidth_ = static_cast<unsigned int>(rect.right - rect.left);
            bufferHeight_ = static_cast<unsigned int>(rect.bottom - rect.top);
            // Without the physical display mode, exclusive presentation could
            // select a virtualized desktop size and leave the monitor without
            // a usable signal. Keep the full-size windowed swap chain instead.
            exclusiveModeAvailable = false;
        }
    }
    window_ = CreateWindowExW(0, kWindowClass, options_.title, style, x, y,
                              rect.right - rect.left, rect.bottom - rect.top, 0, 0, instance_, this);
    if (!window_ || !CreateDeviceResources()) {
        Shutdown();
        return false;
    }
    ShowWindow(window_, SW_SHOW);
    UpdateWindow(window_);
    if (exclusiveModeAvailable) {
        IDXGIOutput *output = 0;
        SetForegroundWindow(window_);
        HRESULT result = swapChain_->GetContainingOutput(&output);
        if (SUCCEEDED(result)) result = swapChain_->SetFullscreenState(TRUE, output);
        if (SUCCEEDED(result)) {
            BOOL fullscreen = FALSE;
            result = swapChain_->GetFullscreenState(&fullscreen, 0);
            if (SUCCEEDED(result) && !fullscreen) result = DXGI_ERROR_NOT_CURRENTLY_AVAILABLE;
        }
        if (output) output->Release();
        if (FAILED(result)) {
            Shutdown();
            return false;
        }
    }
    return true;
}

void App::Shutdown()
{
    if (swapChain_) swapChain_->SetFullscreenState(FALSE, 0);
    ReleaseMultisampleTarget();
    Release(multisampleRasterizerState_);
    ReleaseBackBufferView();
    Release(swapChain_);
    if (context_) {
        context_->ClearState();
        context_->Flush();
    }
    Release(context_);
    Release(device_);
    if (window_) {
        DestroyWindow(window_);
        window_ = 0;
    }
    if (comInitialized_) {
        CoUninitialize();
        comInitialized_ = false;
    }
    instance_ = 0;
    width_ = height_ = 0;
}

bool App::CreateDeviceResources()
{
    DXGI_SWAP_CHAIN_DESC swapDescription;
    memset(&swapDescription, 0, sizeof(swapDescription));
    swapDescription.BufferCount = 1;
    swapDescription.BufferDesc.Width = bufferWidth_;
    swapDescription.BufferDesc.Height = bufferHeight_;
    swapDescription.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapDescription.BufferDesc.RefreshRate.Numerator = 0;
    swapDescription.BufferDesc.RefreshRate.Denominator = 0;
    swapDescription.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapDescription.OutputWindow = window_;
    swapDescription.SampleDesc.Count = 1;
    swapDescription.Windowed = TRUE;
    swapDescription.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    // Required for ResizeTarget to switch the display mode of a windowed
    // swap chain when entering exclusive fullscreen.
    swapDescription.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    const D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0
    };
    D3D_FEATURE_LEVEL createdLevel;
    const UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
    HRESULT result = D3D11CreateDeviceAndSwapChain(0, D3D_DRIVER_TYPE_HARDWARE, 0, flags,
                                                    featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION,
                                                    &swapDescription, &swapChain_, &device_, &createdLevel, &context_);
    if (FAILED(result))
        result = D3D11CreateDeviceAndSwapChain(0, D3D_DRIVER_TYPE_WARP, 0, flags,
                                                featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION,
                                                &swapDescription, &swapChain_, &device_, &createdLevel, &context_);
    if (FAILED(result) || !CreateBackBufferView()) return false;
    sampleCount_ = 1;
    if (options_.antialias) {
        UINT quality = 0;
        if (SUCCEEDED(device_->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &quality)) && quality) sampleCount_ = 4;
        else if (SUCCEEDED(device_->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 2, &quality)) && quality) sampleCount_ = 2;
    }
    if (sampleCount_ > 1 && !CreateMultisampleTarget()) sampleCount_ = 1;
    return true;
}

bool App::CreateMultisampleTarget()
{
    if (!multisampleRasterizerState_) {
        D3D11_RASTERIZER_DESC rasterizer = {};
        rasterizer.FillMode = D3D11_FILL_SOLID;
        rasterizer.CullMode = D3D11_CULL_BACK;
        rasterizer.DepthClipEnable = TRUE;
        rasterizer.MultisampleEnable = TRUE;
        if (FAILED(device_->CreateRasterizerState(&rasterizer, &multisampleRasterizerState_))) return false;
    }
    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = bufferWidth_; desc.Height = bufferHeight_; desc.MipLevels = 1; desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; desc.SampleDesc.Count = sampleCount_;
    desc.SampleDesc.Quality = 0; desc.Usage = D3D11_USAGE_DEFAULT; desc.BindFlags = D3D11_BIND_RENDER_TARGET;
    HRESULT result = device_->CreateTexture2D(&desc, 0, &multisampleTexture_);
    if (SUCCEEDED(result)) result = device_->CreateRenderTargetView(multisampleTexture_, 0, &multisampleRenderTargetView_);
    if (FAILED(result)) { ReleaseMultisampleTarget(); sampleCount_ = 1; return false; }
    return true;
}

void App::ReleaseMultisampleTarget()
{
    Release(multisampleRenderTargetView_);
    Release(multisampleTexture_);
}

bool App::ResolveMultisampleTarget()
{
    if (sampleCount_ == 1 || multisampleResolved_) return true;
    context_->OMSetRenderTargets(0, 0, 0);
    ID3D11Texture2D *backBuffer = 0;
    HRESULT result = swapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&backBuffer));
    if (SUCCEEDED(result)) context_->ResolveSubresource(backBuffer, 0, multisampleTexture_, 0, DXGI_FORMAT_R8G8B8A8_UNORM);
    Release(backBuffer);
    if (SUCCEEDED(result)) multisampleResolved_ = true;
    return SUCCEEDED(result);
}

bool App::ResolveMultisample() { return ResolveMultisampleTarget(); }

bool App::CreateBackBufferView()
{
    ID3D11Texture2D *backBuffer = 0;
    if (FAILED(swapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&backBuffer)))) return false;
    const HRESULT result = device_->CreateRenderTargetView(backBuffer, 0, &renderTargetView_);
    backBuffer->Release();
    if (FAILED(result)) return false;
    viewport_.TopLeftX = 0.0f;
    viewport_.TopLeftY = 0.0f;
    const float scaleX = static_cast<float>(bufferWidth_) / static_cast<float>(width_);
    const float scaleY = static_cast<float>(bufferHeight_) / static_cast<float>(height_);
    // Keep the logical render area at the requested aspect ratio in every
    // presentation mode. Any unused area is letterboxed by centering it.
    viewportScale_ = scaleX < scaleY ? scaleX : scaleY;
    viewport_.Width = static_cast<float>(width_) * viewportScale_;
    viewport_.Height = static_cast<float>(height_) * viewportScale_;
    viewport_.TopLeftX = (static_cast<float>(bufferWidth_) - viewport_.Width) * 0.5f;
    viewport_.TopLeftY = (static_cast<float>(bufferHeight_) - viewport_.Height) * 0.5f;
    viewport_.MinDepth = 0.0f;
    viewport_.MaxDepth = 1.0f;
    return true;
}

void App::ReleaseBackBufferView() { Release(renderTargetView_); }

bool App::Resize(unsigned int width, unsigned int height)
{
    if (!swapChain_ || !width || !height) return true;
    bufferWidth_ = width;
    bufferHeight_ = height;
    ReleaseMultisampleTarget();
    context_->OMSetRenderTargets(0, 0, 0);
    ReleaseBackBufferView();
    if (FAILED(swapChain_->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0))) return false;
    if (!CreateBackBufferView()) return false;
    if (sampleCount_ > 1 && !CreateMultisampleTarget()) sampleCount_ = 1;
    return true;
}

bool App::PumpMessages()
{
    MSG message;
    while (PeekMessageW(&message, 0, 0, 0, PM_REMOVE)) {
        if (message.message == WM_QUIT) {
            closing_ = true;
            return false;
        }
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }
    return !closing_;
}

bool App::BeginFrame(const Color &clearColor)
{
    if (!context_ || !renderTargetView_ || closing_) return false;
    const float color[] = { clearColor.r, clearColor.g, clearColor.b, clearColor.a };
    multisampleResolved_ = false;
    ID3D11RenderTargetView *target = multisampleRenderTargetView_ ? multisampleRenderTargetView_ : renderTargetView_;
    context_->OMSetRenderTargets(1, &target, 0);
    context_->RSSetViewports(1, &viewport_);
    context_->RSSetState(multisampleRasterizerState_);
    context_->ClearRenderTargetView(target, color);
    return true;
}

bool App::EndFrame()
{
    if (!swapChain_ || !ResolveMultisampleTarget() || FAILED(swapChain_->Present(options_.vsync ? 1 : 0, 0))) return false;
    LimitFrameRate();
    return true;
}

void App::LimitFrameRate()
{
    if (!options_.targetFps || frameCounterFrequency_.QuadPart <= 0) return;
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    if (!frameCounterStarted_) {
        lastFrameCounter_ = now;
        frameCounterStarted_ = true;
        return;
    }
    const double frameSeconds = 1.0 / static_cast<double>(options_.targetFps);
    const double elapsed = static_cast<double>(now.QuadPart - lastFrameCounter_.QuadPart) /
                           static_cast<double>(frameCounterFrequency_.QuadPart);
    const double remaining = frameSeconds - elapsed;
    if (remaining > 0.0) {
        if (remaining > 0.002) Sleep(static_cast<DWORD>(remaining * 1000.0) - 1);
        do {
            QueryPerformanceCounter(&now);
        } while (static_cast<double>(now.QuadPart - lastFrameCounter_.QuadPart) /
                     static_cast<double>(frameCounterFrequency_.QuadPart) < frameSeconds);
    }
    lastFrameCounter_ = now;
}

LRESULT CALLBACK App::WindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    App *app = reinterpret_cast<App *>(GetWindowLongPtrW(window, GWLP_USERDATA));
    if (message == WM_NCCREATE) {
        app = reinterpret_cast<App *>(reinterpret_cast<CREATESTRUCTW *>(lParam)->lpCreateParams);
        SetWindowLongPtrW(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(app));
    }
    if (app) {
        if (message == WM_ACTIVATEAPP) app->active_ = wParam != 0;
        if (message == WM_SIZE && wParam != SIZE_MINIMIZED && app->options_.windowed)
            app->Resize(LOWORD(lParam), HIWORD(lParam));
        if (message == WM_CLOSE) {
            app->closing_ = true;
            DestroyWindow(window);
            return 0;
        }
        if (message == WM_DESTROY) {
            PostQuitMessage(0);
            return 0;
        }
        if (message == WM_NCDESTROY) app->window_ = 0;
    }
    return DefWindowProcW(window, message, wParam, lParam);
}

} // namespace library
