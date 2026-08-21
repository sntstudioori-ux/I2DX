# Dx11library -- Standalone DirectX 11 & Audio Library

Windows 用の最小限の DirectX 11 描画およびオーディオ再生ライブラリです。
軽量化を重視して設計されています。

---

## 目次
1. [アプリケーション & ウィンドウ管理 (`App`)](#1-アプリケーション--ウィンドウ管理-app)
2. [2D グラフィックス (`Texture` & `Renderer2D`)](#2-2d-グラフィックス-texture--renderer2d)
3. [3D グラフィックス & カメラ (`Texture3D` & `Renderer3D` & `Camera3D`)](#3-3d-グラフィックス--カメラ-texture3d--renderer3d--camera3d)
4. [テキスト描画 (`Font` & `FontRenderer`)](#4-テキスト描画-font--fontrenderer)
5. [オーディオ再生 (`AudioEngine` & `WavSound` & `MidiPlayer`)](#5-オーディオ再生-audioengine--wavsound--midiplayer)
6. [ユーティリティ (`File`)](#6-ユーティリティ-file)
7. [ビルド方法](#7-ビルド方法)
8. [キーボード入力 (`Keyboard`)](#8-キーボード入力-keyboard)

---

## 1. アプリケーション & ウィンドウ管理 (`App`)

### `library::App`
Win32 ウィンドウ、Direct3D 11 デバイス、スワップチェーン、およびバックバッファのライフサイクルを管理します。

#### 主要メンバー関数
*   `bool Initialize(HINSTANCE instance, const AppOptions &options = AppOptions())`
    *   ウィンドウと DirectX 11 デバイス環境を初期化します。
*   `void Shutdown()`
    *   確保したリソースを解放し、ウィンドウを破棄します。
*   `bool PumpMessages()`
    *   Windows メッセージを処理します。ウィンドウが閉じられると `false` を返します。
*   `bool BeginFrame(const Color &clearColor = Color())`
    *   描画フレームを開始し、指定された色でバックバッファをクリアします。
*   `bool EndFrame()`
    *   描画されたフレームを画面に表示（Present）します。

#### ヘルパー構造体
*   `Color(float red, float green, float blue, float alpha = 1.0f)`: RGBAカラー。
*   `AppOptions`: ウィンドウの幅、高さ、タイトル、ウィンドウモード、VSync の有無などを設定する構造体。

#### 使用例
```cpp
#include "Dx11library/App.hpp"

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int)
{
    library::App app;
    library::AppOptions options;
    options.title = L"My Game";
    options.width = 640;
    options.height = 480;

    if (!app.Initialize(instance, options)) return 1;

    while (app.PumpMessages())
    {
        if (!app.IsActive()) {
            Sleep(10);
            continue;
        }

        // フレーム開始（背景色を青っぽくクリア）
        if (app.BeginFrame(library::Color(0.1f, 0.2f, 0.4f)))
        {
            // ここで描画処理を行う

            app.EndFrame(); // フレーム終了と表示
        }
    }
    return 0;
}
```

---

## 2. 2D グラフィックス (`Texture` & `Renderer2D`)

### `library::Renderer2D`
スプライトを描画するためのシェーダーや頂点バッファを管理します。

### `library::Texture`
WIC (Windows Imaging Component) を使用して、PNG、JPEG、BMP などの画像ファイルを読み込み、描画します。

#### 主要メンバー関数（`Texture`）
*   `bool LoadFile(ID3D11Device *device, const wchar_t *path)`
    *   ファイルから画像をロードします。
*   `bool LoadMemory(ID3D11Device *device, const void *data, unsigned int size)`
    *   メモリ上のバイナリデータから画像をロードします。
*   `bool Draw(Renderer2D &renderer, const App &app, const RectF &destination, const RECT *source = nullptr, const Color &color = Color(1.0f, 1.0f, 1.0f, 1.0f)) const`
    *   テクスチャの指定した範囲 (`source`) を画面の指定矩形 (`destination`) に描画します。

#### 使用例
```cpp
#include "Dx11library/Dx11library.hpp"

library::Renderer2D renderer;
renderer.Initialize(app.Device());

library::Texture background;
background.LoadFile(app.Device(), L"img/background.png");

// 描画ループ内（BeginFrame と EndFrame の間）
library::RectF destRect(0.0f, 0.0f, 640.0f, 480.0f);
background.Draw(renderer, app, destRect);
```

> [!IMPORTANT]
> `Renderer2D` と `Texture` は、`App` の破棄（または `App::Shutdown`）よりも前に破棄（または `Release()`）されるようにしてください。これは、Direct3D デバイスが参照されている状態でデバイスが解放されるのを防ぐためです。

---

## 3. 3D グラフィックス & カメラ (`Texture3D` & `Renderer3D` & `Camera3D`)

### `library::Renderer3D`
Z座標および遠近投影（パースペクティブ射影）に対応した3D描画用のレンダラーです。デフォルトでは Z=0 の平面が通常の 2D 画面上の座標とピクセル単位で1:1に一致するように自動計算されます。

### `library::Texture3D`
`Texture` クラスを継承しており、X, Y に加えて Z 座標を指定して描画できる 3D 対応のテクスチャクラスです。

#### 主要メンバー関数（`Texture3D`）
*   `bool Draw(Renderer3D &renderer, const App &app, float x1, float y1, float z1, float x2, float y2, float z2, const RECT *source = nullptr, const Color &color = Color(1.0f, 1.0f, 1.0f, 1.0f)) const`
    *   指定した3D空間上の2つの対角点 `(x1, y1, z1)`（左上相当）と `(x2, y2, z2)`（右下相当）にテクスチャを描画します。
*   `bool Draw(Renderer3D &renderer, const App &app, const RectF3D &destination, const RECT *source = nullptr, const Color &color = Color(1.0f, 1.0f, 1.0f, 1.0f)) const`
    *   3D範囲指定用の構造体 `RectF3D` を使用して描画します。

### `library::Camera3D`
画面中央をピボット（回転中心）として、3D空間におけるカメラ位置の移動（スクロール、ズーム）や回転（ピッチ、ヨー、ロール）を行う軽量なカメラ管理クラスです。

#### 主要メンバー関数（`Camera3D`）
*   `void SetPosition(float x, float y, float z)`
    *   カメラの座標を設定します。X, Y はピクセル単位の平行移動（スクロール）、Z はデフォルト位置からの奥行き移動（ズーム）です。
*   `void SetRotation(float pitch, float yaw, float roll)`
    *   カメラの回転角度（度数法）を設定します。
*   `void Reset()`
    *   カメラの移動および回転をすべて初期状態（デフォルトの2Dビューポートと一致する状態）にリセットします。

#### 使用例
```cpp
#include "Dx11library/Dx11library.hpp"

library::Renderer3D renderer3D;
renderer3D.Initialize(app.Device());

library::Texture3D tex;
tex.LoadFile(app.Device(), L"img/background_3d.png");

library::Camera3D camera;
renderer3D.SetCamera(&camera); // レンダラーにカメラをアタッチ

// --- 描画ループ内（BeginFrame と EndFrame の間） ---

// カメラを操作する（例：画面を少し傾け、Z方向にズームインし、右に少しスクロール）
camera.SetPosition(50.0f, 0.0f, 100.0f);
camera.SetRotation(15.0f, 0.0f, 5.0f);

// 3D空間上に描画（奥行きを持たせて奥に傾ける）
tex.Draw(renderer3D, app,
         0.0f,   0.0f,   0.0f,     // 左上点 (x1, y1, z1)
         640.0f, 480.0f, 200.0f);  // 右下点 (x2, y2, z2) -- 奥(Z=200)へ向かう傾き
```

> [!NOTE]
> `Renderer3D` は内部的に深度バッファテスト（`D3D11_COMPARISON_LESS_EQUAL`）を有効にして描画するため、Z座標の前後関係が正しく処理されます。また、カメラを無効にするには `renderer3D.SetCamera(nullptr)` を呼び出します。

---

## 4. テキスト描画 (`Font` & `FontRenderer`)

### `library::Font`
`Font` はフォント、サイズ、色、透明度、グラデーションを保持する再利用可能なテキストスタイルです。`FontRenderer` は Direct2D / DirectWrite の共有リソースと描画セッションを管理します。複数の `Font` を同時に使用できます。

### `library::FontRenderer`
アプリケーションごとに一つ作成します。`Begin` と `End` の間で `Font::Draw` を呼び出します。テキストは通常、2D/3D の描画後、`App::EndFrame` の前に描画してください。

#### 主要メンバー関数
*   `bool FontRenderer::Initialize(const App &app)` / `bool Begin(const App &app)` / `bool End()`
    *   共有リソースを初期化し、テキスト描画を開始・終了します。
*   `bool Font::LoadSystem(FontRenderer &renderer, const wchar_t *familyName, float size = 16.0f)`
    *   システムにインストール済みのフォントを読み込みます。
*   `bool Font::LoadFile(FontRenderer &renderer, const wchar_t *path, const wchar_t *familyName, float size = 16.0f)`
    *   `.ttf` または `.otf` をこの `Font` 専用のコレクションとして読み込みます。`familyName` はフォントファイルの内部ファミリー名を指定します。
*   `bool SetSize(float size)`, `SetColor`, `SetOpacity`, `SetGradient`, `ClearGradient`
    *   文字サイズ、単色、透明度、上から下へのグラデーションを設定します。
*   `TextSize Measure(const wchar_t *text) const`
    *   テキストを描画したときの幅と高さをピクセル単位で取得します。失敗時は `{0, 0}` を返します。
*   `bool Draw(FontRenderer &renderer, const wchar_t *text, const RectF &destination) const`
    *   描画矩形にテキストを描画します。

#### 使用例
```cpp
#include "Dx11library/Dx11library.hpp"

library::FontRenderer fontRenderer;
if (!fontRenderer.Initialize(app)) return 1;

// システムフォント、または TTF/OTF を読み込む
library::Font myFont;
myFont.LoadFile(fontRenderer, L"fonts/my_custom_font.ttf", L"My Font", 24.0f);
// myFont.LoadSystem(fontRenderer, L"MS Gothic", 24.0f);
myFont.SetGradient(library::Color(1.0f, 0.9f, 0.2f),
                   library::Color(1.0f, 0.2f, 0.2f));
myFont.SetOpacity(0.9f);

// 3. 描画ループ内（BeginFrame と EndFrame の間）
if (app.BeginFrame(...))
{
    if (fontRenderer.Begin(app))
    {
        myFont.Draw(fontRenderer, L"Score: 123450", library::RectF(10.0f, 10.0f, 400.0f, 50.0f));
        fontRenderer.End();
    }

    app.EndFrame();
}

fontRenderer.Shutdown();
```

---

## 5. オーディオ再生 (`AudioEngine` & `WavSound` & `MidiPlayer`)

### `library::AudioEngine`
XAudio2 エンジンおよびマスタリングボイスを統括・管理します。

### `library::WavSound`
WAVサウンドデータをメモリにロードし、XAudio2 経由で再生する効果音・BGM用クラスです。

### `library::MidiPlayer`
Windows の MCI (Media Control Interface) サブシステムを利用して MIDI ファイルを再生するクラスです。

#### 主要メンバー関数（`WavSound` & `MidiPlayer` 共通）
*   `bool Play(bool loop = false)`: 再生を開始します。`loop` を `true` にするとループ再生されます。
*   `void Stop()`: 再生を停止します。
*   `void Pause()` / `void Resume()`: 一時停止と再開。
*   `void SetVolume(float volume)`: 音量を設定します（`0.0f` 〜 `1.0f`）。

> [!NOTE]
> `MidiPlayer` でループ再生を行う場合は、定期的に（ゲームのアップデートフレームごとに） `MidiPlayer::Update()` を呼び出す必要があります。

#### 使用例
```cpp
#include "Dx11library/Dx11library.hpp"

// 1. オーディオエンジンの初期化
library::AudioEngine audioEngine;
audioEngine.Initialize();

// 2. 音源のロード
library::WavSound seShoot;
seShoot.LoadFile(audioEngine, L"sound/shoot.wav");

library::MidiPlayer bgmTitle;
bgmTitle.LoadFile(L"bgm/title.mid");

// 3. 再生
bgmTitle.Play(true); // MIDIをループ再生
seShoot.Play(false); // SEを一発再生

// 4. ループ内での更新処理（MIDIのループ再生用）
while (app.PumpMessages())
{
    // ... ゲームロジックの更新 ...

    bgmTitle.Update(); // MIDIのループ状態更新
}
```

---

## 6. ユーティリティ (`File`)

### `ReadBinaryFile` / `WriteBinaryFile`
バイナリファイルを一括で読み込み・書き込みするシンプルなユーティリティ関数です。

*   `bool ReadBinaryFile(const char *path, std::vector<unsigned char> *data)`
*   `bool WriteBinaryFile(const char *path, const void *data, unsigned int size)`

---

## 7. ビルド方法

Windows 10/11 SDK と CMake がインストールされている環境でビルド可能です。

```powershell
# 構成ファイルの生成（例: out/build/x64-Debug）
cmake -S . -B out/build/x64-Debug -G Ninja

# ビルドの実行
cmake --build out/build/x64-Debug
```

シンプルな構成（`out/build/x64-Debug` のような詳細指定なし）でも可能です。

```powershell
cmake -S . -B build
cmake --build build --config debug
```

---

## 8. キーボード入力 (`Keyboard`)

`Keyboard` は Windows 標準の `GetAsyncKeyState` を使用した、軽量なキーボード入力クラスです。
ウィンドウハンドルを指定した場合、ウィンドウが非アクティブになると入力状態をリセットします。

### 使用例

```cpp
#include "Dx11library/Dx11library.hpp"

library::Keyboard keyboard;
keyboard.Initialize(app.Window());

while (app.PumpMessages())
{
    keyboard.Update();

    if (keyboard.IsDown(library::Key::Left))
    {
        // 左キーを押している間
    }

    if (keyboard.WasPressed(library::Key::Space))
    {
        // スペースキーを押した瞬間
    }

    if (keyboard.WasReleased(library::Key::Space))
    {
        // スペースキーを離した瞬間
    }
}

keyboard.Shutdown();
```

`Update()` はゲームループの各フレームで呼び出してください。

- `IsDown`: キーを押している間 `true`
- `WasPressed`: 前フレームでは押されておらず、現在押されている場合に `true`
- `WasReleased`: 前フレームでは押されており、現在離されている場合に `true`

`Key` に定義されていないキーは、Windows の仮想キーコードを直接渡して取得できます。
キーボード入力は独立したクラスとして実装されているため、将来的にゲームパッド入力を追加できます。
