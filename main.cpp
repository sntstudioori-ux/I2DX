#include <windows.h>

<<<<<<< HEAD
#include "Dx11library.hpp"
=======
#include "Dx11library/Dx11library.hpp"
>>>>>>> abb6f4c (v1.01)

// Minimal standalone application: creates a 640 x 480 DirectX 11 window,
// clears it every frame, and exits when the user closes the window.
int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int)
{
    library::App app;
    library::AppOptions Options;
<<<<<<< HEAD
    Options.title = L"library test";
=======
    Options.title = L"th06 library test";
>>>>>>> abb6f4c (v1.01)
    Options.width = 640;
    Options.height = 480;
    Options.vsync = true;
    Options.targetFps = 60;
    Options.antialias = true;
    Options.windowed = true;
    if (!app.Initialize(instance, Options))
    {
<<<<<<< HEAD
        MessageBoxW(0, L"DirectX 11の初期化に失敗したようです。", L"library", MB_OK | MB_ICONERROR);
=======
        MessageBoxW(0, L"DirectX 11の初期化に失敗したようです。", L"th06 library", MB_OK | MB_ICONERROR);
>>>>>>> abb6f4c (v1.01)
        return 1;
    }

    library::AudioEngine audio;
    if (!audio.Initialize())
    {
<<<<<<< HEAD
        MessageBoxW(0, L"XAudioの初期化に失敗したようです。", L"library", MB_OK | MB_ICONERROR);
=======
        MessageBoxW(0, L"XAudioの初期化に失敗したようです。", L"th06 library", MB_OK | MB_ICONERROR);
>>>>>>> abb6f4c (v1.01)
        return 1;
    }

    library::Renderer2D render2D;

    if (!render2D.Initialize(app.Device()))
    {
<<<<<<< HEAD
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

    camera.Reset();

    library::Camera3D *Camera = &camera;

    render3D.SetCamera(Camera);
=======
        MessageBoxW(0, L"DirectXの初期化に失敗したようです。", L"th06 library", MB_OK | MB_ICONERROR);
        return 1;
    }

    // library::Renderer3D render3D;

    // if (!render3D.Initialize(app.Device()))
    // {
    //     MessageBoxW(0, L"DirectXの初期化に失敗したようです（3D）。", L"th06 library", MB_OK | MB_ICONERROR);
    //     return 1;
    // }


    // library::Camera3D camera;

    // camera.Reset();
	
	// library::Camera3D *Camera = &camera;

    // render3D.SetCamera(Camera);
>>>>>>> abb6f4c (v1.01)

    library::FontRenderer fontRenderer;

    if (!fontRenderer.Initialize(app))
    {
<<<<<<< HEAD
        MessageBoxW(0, L"DirectWriteの初期化に失敗したようです。", L"library", MB_OK | MB_ICONERROR);
=======
        MessageBoxW(0, L"DirectWriteの初期化に失敗したようです。", L"th06 library", MB_OK | MB_ICONERROR);
>>>>>>> abb6f4c (v1.01)
        return 1;
    }

    library::Keyboard keyboard;

    if (!keyboard.Initialize(app.Window()))
    {
<<<<<<< HEAD
        MessageBoxW(0, L"キー入力の初期化に失敗したようです。", L"library", MB_OK | MB_ICONERROR);
=======
        MessageBoxW(0, L"キー入力の初期化に失敗したようです。", L"th06 library", MB_OK | MB_ICONERROR);
>>>>>>> abb6f4c (v1.01)
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
<<<<<<< HEAD
=======

>>>>>>> abb6f4c (v1.01)
