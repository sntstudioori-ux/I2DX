#include <windows.h>

#include "Dx11library/Dx11library.hpp"

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int)
{
    library::App app;
    library::AppOptions options;
    options.title = L"library test";
    options.width = 640;
    options.height = 480;
    options.vsync = true;
    options.targetFps = 0; // VSync controls pacing; targetFps is used when VSync is off.
    options.antialias = true;
    options.windowed = true;
    if (!app.Initialize(instance, options))
    {
        MessageBoxW(0, L"DirectX 11の初期化に失敗したようです。", L"library", MB_OK | MB_ICONERROR);
        return 1;
    }

    library::AudioEngine audio;
    if (!audio.Initialize())
    {
        MessageBoxW(0, L"XAudioの初期化に失敗したようです。", L"library", MB_OK | MB_ICONERROR);
        return 1;
    }

    library::Renderer2D render2D;

    if (!render2D.Initialize(app.Device()))
    {
        MessageBoxW(0, L"DirectXの初期化に失敗したようです。", L"library", MB_OK | MB_ICONERROR);
        return 1;
    }

    library::Renderer3D render3D;

    if (!render3D.Initialize(app.Device()))
    {
        MessageBoxW(0, L"DirectXの初期化に失敗したようです（3D）。", L"library", MB_OK | MB_ICONERROR);
        return 1;
    }

    library::Camera3D camera;
    render3D.SetCamera(&camera);

    library::FontRenderer fontRenderer;

    if (!fontRenderer.Initialize(app))
    {
        MessageBoxW(0, L"DirectWriteの初期化に失敗したようです。", L"library", MB_OK | MB_ICONERROR);
        return 1;
    }

    library::Keyboard keyboard;

    if (!keyboard.Initialize(app.Window()))
    {
        MessageBoxW(0, L"キー入力の初期化に失敗したようです。", L"library", MB_OK | MB_ICONERROR);
        return 1;
    }

    while (app.PumpMessages())
    {
        if (!app.IsActive())
        {
            Sleep(10);
            continue;
        }

        keyboard.Update();

        if (app.BeginFrame(library::Color(0, 0, 0)))
        {
            app.EndFrame();
        }
        else
        {
            Sleep(10);
        }
    }
    return 0;
}
