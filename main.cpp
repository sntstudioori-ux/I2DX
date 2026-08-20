#include <windows.h>

#include "Dx11library/Dx11library.hpp"

// Minimal standalone application: creates a 640 x 480 DirectX 11 window,
// clears it every frame, and exits when the user closes the window.
int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int)
{
    library::App app;
    library::AppOptions Options;
    Options.title = L"th06 library test";
    Options.width = 640;
    Options.height = 480;
    Options.vsync = true;
    Options.targetFps = 60;
    Options.antialias = true;
    Options.windowed = true;
    if (!app.Initialize(instance, Options))
    {
        MessageBoxW(0, L"DirectX 11の初期化に失敗したようです。", L"th06 library", MB_OK | MB_ICONERROR);
        return 1;
    }

    library::AudioEngine audio;
    if (!audio.Initialize())
    {
        MessageBoxW(0, L"XAudioの初期化に失敗したようです。", L"th06 library", MB_OK | MB_ICONERROR);
        return 1;
    }

    library::Renderer2D render2D;

    if (!render2D.Initialize(app.Device()))
    {
        MessageBoxW(0, L"DirectXの初期化に失敗したようです。", L"th06 library", MB_OK | MB_ICONERROR);
        return 1;
    }

    library::Renderer3D render3D;

    if (!render3D.Initialize(app.Device()))
    {
        MessageBoxW(0, L"DirectXの初期化に失敗したようです（3D）。", L"th06 library", MB_OK | MB_ICONERROR);
        return 1;
    }


    library::Camera3D camera;

    camera.Reset();
	
	library::Camera3D *Camera = &camera;

    render3D.SetCamera(Camera);

    library::FontRenderer fontRenderer;

    if (!fontRenderer.Initialize(app))
    {
        MessageBoxW(0, L"DirectWriteの初期化に失敗したようです。", L"th06 library", MB_OK | MB_ICONERROR);
        return 1;
    }

    library::Keyboard keyboard;

    if (!keyboard.Initialize(app.Window()))
    {
        MessageBoxW(0, L"キー入力の初期化に失敗したようです。", L"th06 library", MB_OK | MB_ICONERROR);
        return 1;
    }

    while (app.PumpMessages())
    {
        // Do not continuously render while the application is in the background.
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

