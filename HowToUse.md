# Dx11library の使い方

Dx11library は、Win32 ウィンドウ、Direct3D 11 による 2D/3D 描画、DirectWrite によるテキスト描画、WAV/MIDI 再生、キーボード状態の取得をまとめた Windows 向け C++ ライブラリです。アプリケーション側でゲームループやリソースの読み込みタイミングを管理し、各クラスの初期化・描画・解放を呼び出します。

## ヘッダーと名前空間

全機能を使う場合は `Dx11library/Dx11library.hpp` をインクルードします。個別のヘッダーをインクルードして使うこともできます。公開クラスと型は `library` 名前空間にあります。

```cpp
#include "Dx11library/Dx11library.hpp"

library::App app;
```

ライブラリは C++17 と Windows SDK を前提とします。Direct3D のデバイスやコンテキストは `App` が所有し、描画用クラスはそれらを利用します。描画リソースや音源を所有するクラスは、対応する `App` / `AudioEngine` より先に破棄してください。これらのクラスはコピーできません。

## アプリケーションとフレーム

### `library::App` と `library::AppOptions`

`App` はウィンドウ、D3D11 デバイス、スワップチェーン、バックバッファを管理します。`Initialize` に成功した後、メッセージ処理と描画を繰り返し、終了時に `Shutdown` を呼びます。デストラクターも解放を行います。

`AppOptions` の既定値は幅 640、高さ 480、タイトル `DirectX 11 application`、ウィンドウ表示、VSync 有効、ソフトウェアのFPS上限なし、アンチエイリアス有効です。VSync とソフトウェア上限は同時に使わず、VSyncを有効にするとディスプレイの更新タイミングに同期します。VSyncを無効にした場合は `targetFps` がソフトウェア上限として働きます。

- `width`, `height`: 論理描画領域の幅と高さ（ピクセル）。ウィンドウをリサイズしてもこの座標系を保ち、描画領域はアスペクト比を維持して中央に表示されます。
- `title`: ウィンドウタイトル。ワイド文字列を指定します。
- `windowed`: `true` なら通常のウィンドウ、`false` なら `fullscreenMode` で選んだフルスクリーン表示です。
- `fullscreenMode`: `FullscreenMode::Borderless` はデスクトップ解像度のボーダーレス表示、`FullscreenMode::Exclusive` はDXGIの排他的フルスクリーンです。指定解像度と現在のモニター解像度の比率が一致する場合、指定解像度へ切り替えてモニター全体に拡大します。比率が異なる場合はモニター解像度の描画バッファを使い、指定した論理描画領域を比率を保って最大サイズに拡大します。この場合、上下または左右に黒帯が入ります。排他的切替が利用できない場合は `Initialize` が `false` を返します。
- `vsync`: `true` なら `Present` 時に垂直同期を有効にします。この場合、`targetFps` によるソフトウェア上限は適用しません。
- `targetFps`: VSyncを無効にした場合のソフトウェア上限。`0` なら上限処理をしません。VSync有効時は無視されます。上限待機には高解像度タイマーを使いますが、処理負荷やOSのスケジューリング状況により指定FPSを下回ることがあります。
- `antialias`: 利用可能な場合に 4x、次に 2x MSAA を試します。使用されたサンプル数は `App::SampleCount()` で取得できます。

排他的フルスクリーンを使う場合は、たとえば `options.width = 1280; options.height = 720; options.windowed = false; options.fullscreenMode = library::FullscreenMode::Exclusive;` のように設定します。比率が一致する場合に指定寸法のディスプレイモードがないときや、DXGIが全画面切替を拒否したときは `Initialize` が `false` を返します。

`Initialize(HINSTANCE)` は既定の `AppOptions` を使う簡易形式です。`Initialize(HINSTANCE, const AppOptions&)` は指定した設定で初期化し、成功時に `true` を返します。失敗時は `false` です。

### フレーム処理

`PumpMessages()` は Windows メッセージを処理します。ウィンドウが閉じられると `false` を返すため、通常はゲームループの条件にします。`IsActive()` はアプリがフォアグラウンドか、`ShouldClose()` は終了要求が出ているかを返します。

各フレームでは `BeginFrame` でバックバッファをクリアし、描画クラスを呼び出してから `EndFrame` で画面に表示します。`BeginFrame` の色は `Color` の RGBA 浮動小数値で指定します。既定色は不透明な黒です。

```cpp
#include "Dx11library/Dx11library.hpp"

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int)
{
    library::App app;
    library::AppOptions options;
    options.title = L"Sample";
    options.width = 1280;
    options.height = 720;
    options.vsync = true;
    options.targetFps = 0; // VSyncとFPS上限を重ねない

    if (!app.Initialize(instance, options)) return 1;

    while (app.PumpMessages())
    {
        if (!app.IsActive()) {
            Sleep(10);
            continue;
        }

        if (app.BeginFrame(library::Color(0.08f, 0.10f, 0.16f))) {
            // Renderer2D / Renderer3D による描画
            app.EndFrame();
        }
    }
    app.Shutdown();
    return 0;
}
```

VSyncを使わず60 FPSに制限する場合は、`options.vsync = false; options.targetFps = 60;` とします。VSync有効時の実際のフレームレートはディスプレイの更新頻度に従うため、60 Hz環境でも約59.94 FPSになることがあります。

`Width()` と `Height()` は論理描画領域の寸法です。`ViewportScale()` と `ViewportOffsetX()` / `ViewportOffsetY()` は、実際のバックバッファ上で論理領域を表示するスケールと余白を返します。描画時に必要な D3D11 オブジェクトは `Device()`、`Context()`、`SwapChain()` から取得できます。`Window()` は HWND を返します。これらのポインターは `App` が所有します。`Antialias()` は設定値、`SampleCount()` は実際に使われる MSAA サンプル数です。

`ResolveMultisample()` は MSAA 描画結果をスワップチェーンのバックバッファへ解決します。通常は `EndFrame` が実行します。テキストの合成処理など、フレーム途中でバックバッファの内容が必要な場面ではライブラリ内部から使われます。

### `Color`

`Color(red, green, blue, alpha)` は各成分を `0.0f` から `1.0f` の範囲で指定する RGBA 色です。`alpha` の既定値は `1.0f` です。スプライトや文字では色の乗算・透明度にも使います。

## 2D テクスチャ描画

### `Renderer2D`

`Renderer2D` は 2D スプライト用のシェーダーや頂点バッファを保持します。描画前に `Initialize(app.Device())` を一度呼び、使用終了時に `Shutdown()` を呼びます。初期化に失敗した場合は描画できません。

`Texture::Draw` は `Renderer2D` と `App` を受け取り、アプリの D3D11 コンテキストと論理画面サイズを使って描画します。描画呼び出しは `App::BeginFrame` と `App::EndFrame` の間に行います。

### `Texture` と `RectF`

`Texture` は WIC を使って画像を読み込み、D3D11 のシェーダーリソースとして保持します。`LoadFile(device, path)` は画像ファイルから、`LoadMemory(device, data, size)` はメモリ上の画像データから読み込みます。対応形式は Windows Imaging Component がデコードできる形式（一般的な PNG、JPEG、BMP など）です。どちらも成功時は `true` を返します。

`Width()` と `Height()` は読み込んだ画像の寸法、`View()` は内部のシェーダーリソースビューを返します。通常は直接使わず `Draw` で描画します。別の画像を読み込む前や使用終了時は `Release()` で解放できます。デストラクターも解放します。

`RectF(left, top, right, bottom)` は論理画面上の描画先矩形です。`Texture::Draw(renderer, app, destination, source, color)` の引数は次のとおりです。

- `destination`: 画面上の矩形。座標は論理画面のピクセル単位です。
- `source`: 画像内の切り出し矩形を表す `RECT` へのポインター。`nullptr` なら画像全体を使います。右端・下端は `RECT` の通常の半開区間に従います。
- `color`: テクスチャに乗算する色。既定値は不透明な白で、元画像の色をそのまま表示します。

```cpp
library::Renderer2D renderer;
if (!renderer.Initialize(app.Device())) return 1;

library::Texture background;
if (!background.LoadFile(app.Device(), L"assets/background.png")) return 1;

// フレーム内
library::RectF destination(0.0f, 0.0f,
                            static_cast<float>(app.Width()),
                            static_cast<float>(app.Height()));
background.Draw(renderer, app, destination);
```

`Renderer2D` と `Texture` は `App` が提供する D3D11 デバイスに依存します。`App::Shutdown()` より前に両者を解放してください。

## 3D テクスチャ描画とカメラ

### `Renderer3D`、`Texture3D`、`RectF3D`

`Renderer3D` はテクスチャ付き四角形を遠近投影で描画し、深度テストを行います。`Initialize(app.Device())` を一度呼び、終了時に `Shutdown()` を呼びます。既定の視点では `Z=0` が通常の 2D 画面平面に一致し、正の Z は画面の奥へ進みます。X/Y 座標は左上原点のピクセル座標です。

`SetFov(degrees)` は視野角を度数で設定します。既定値は 60 度で、値が小さいほど遠近感は弱く、大きいほど強くなります。`GetFov()` で現在値を取得します。

`Texture3D` は `Texture` を継承するため、同じ `LoadFile`、`LoadMemory`、`Release`、寸法取得を使えます。`Draw` には座標を個別に渡す形式と `RectF3D` を渡す形式があります。座標形式では `(x1, y1, z1)` を左上側、`(x2, y2, z2)` を右下側の角として四角形を定義します。`RectF3D` は同じ6座標をまとめた型です。`source` と `color` の意味・既定値は `Texture::Draw` と同じです。

### `Camera3D`

`Camera3D` は標準視点からの移動と回転を保持します。`SetPosition(x, y, z)` は位置を設定し、`Move(dx, dy, dz)` は現在値に加算します。X/Y は画面ピクセル単位の視点移動、Z は奥行き方向の移動です。正の Z はカメラをシーンに近づけてズームインし、負の Z は遠ざけます。カメラを右または下へ動かすと、シーンは画面上で左または上へ動きます。

`SetRotation(pitch, yaw, roll)` は回転を度数で設定し、`Rotate(dPitch, dYaw, dRoll)` は現在値に加算します。回転中心は画面中央です。`X()` / `Y()` / `Z()` と `Pitch()` / `Yaw()` / `Roll()` で現在値を取得し、`Reset()` で移動と回転を初期値に戻します。

カメラを描画に適用するには `renderer.SetCamera(&camera)` を呼びます。レンダラーはポインターを保持するだけで所有しないため、描画中は `Camera3D` の寿命を保ちます。`SetCamera(nullptr)` で標準視点に戻ります。

```cpp
library::Renderer3D renderer3D;
if (!renderer3D.Initialize(app.Device())) return 1;

library::Texture3D sprite;
if (!sprite.LoadFile(app.Device(), L"assets/object.png")) return 1;

library::Camera3D camera;
renderer3D.SetCamera(&camera);
renderer3D.SetFov(60.0f);

// 更新時
camera.Move(1.0f, 0.0f, 0.0f);
camera.SetRotation(10.0f, 0.0f, 0.0f);

// フレーム内
sprite.Draw(renderer3D, app,
            library::RectF3D(100.0f, 100.0f, 0.0f,
                             300.0f, 260.0f, 150.0f));
```

## テキスト描画

### `FontRenderer` と `Font`

`FontRenderer` は D2D/DirectWrite の共有リソースを管理します。`App` の初期化後に `Initialize(app)` を呼びます。各フレームのテキスト描画は `Begin(app)` と `End(app)` で囲み、その間に `Font::Draw` を呼びます。2D/3D描画の後、`App::EndFrame()` の前に文字を描画する順序が基本です。使用終了時に `Shutdown()` を呼びます。

`Font` は個別の書体と表示スタイルを持ち、同じ `FontRenderer` で複数作成できます。`LoadSystem(renderer, familyName, size)` はシステムに登録されたフォントファミリー、`LoadFile(renderer, path, familyName, size)` はフォントファイルを使います。ファイル指定時の `familyName` はフォントファイル内に定義されたファミリー名です。サイズは正の値を指定します。読み込みに失敗した場合は `false` を返します。

- `SetSize(size)`: フォントサイズを変更します。失敗時は `false` を返します。
- `SetColor(color)`: 単色を設定します。グラデーションが有効な間は単色設定は描画に使われません。
- `SetOpacity(opacity)`: 文字全体の追加不透明度を設定します。値は `0` から `1` に収められます。色の alpha 値にも乗算されます。
- `SetGradient(top, bottom)`: 描画矩形の上から下に向かうグラデーションを有効にします。
- `ClearGradient()`: グラデーションを解除し、`SetColor` の色を使う状態に戻します。
- `Measure(text)`: 描画に必要な文字幅・高さをピクセル単位の `TextSize`（`width`, `height`）で返します。引数が無効、または計測に失敗した場合は幅・高さとも 0 です。
- `Draw(renderer, text, destination)`: 指定矩形に文字列を描画します。レンダラーの `Begin(app)` と `End(app)` の間で呼び出します。
- `Release()`: フォント形式やフォントコレクションを解放します。デストラクターも解放します。

```cpp
library::FontRenderer fontRenderer;
if (!fontRenderer.Initialize(app)) return 1;

library::Font font;
if (!font.LoadSystem(fontRenderer, L"Yu Gothic UI", 24.0f)) return 1;
font.SetColor(library::Color(1.0f, 1.0f, 0.8f));
font.SetOpacity(0.9f);

// 2D/3D 描画の後、App::EndFrame() の前
if (fontRenderer.Begin(app)) {
    font.Draw(fontRenderer, L"Score: 12345",
              library::RectF(16.0f, 16.0f, 320.0f, 56.0f));
    fontRenderer.End(app);
}
```

`FontRenderer::Begin` / `End` は描画セッションの状態に応じて `false` を返すことがあります。`Font` が利用する DirectWrite ファクトリーは初期化済み `FontRenderer` に由来するため、フォントはそのレンダラーより先に破棄してください。

## オーディオ

### `AudioEngine`

`AudioEngine` は XAudio2 とマスタリングボイスを所有します。WAV 音源を読み込む前に `Initialize()` を呼びます。初期化状態は `IsInitialized()` で確認でき、終了時には音源を解放してから `Shutdown()` を呼びます。`XAudio2()` と `MasteringVoice()` は内部オブジェクトへのポインターを返します。

### `WavSound`

`WavSound` は WAV データをメモリに保持し、XAudio2 で再生します。`LoadFile(engine, path)` は `char` または `wchar_t` のパスを受け取り、`LoadMemory(engine, data, size)` は WAV ファイル全体のバイト列を受け取ります。読み込み時には初期化済み `AudioEngine` を渡します。通常の PCM/WAVE 形式で、妥当な WAV ヘッダーと `fmt ` / `data` チャンクを含むデータが必要です。

`Play(loop)` は音源の先頭から再生し、`loop=true` なら全体を繰り返します。`PlayLoop(loopStartSeconds, loopEndSeconds)` は WAV 内の指定秒数区間を繰り返します。開始は 0 以上、終了は開始より後かつ音源の長さ以内でなければなりません。再生に関する共通操作は次のとおりです。

- `Stop()` は再生を停止してキューを空にします。
- `Pause()` / `Resume()` は一時停止と再開です。
- `SetVolume(volume)` は音量を設定します。値は内部で 0〜1 に制限されます。
- `IsPlaying()` は再生バッファがキューにあるかを返します。一時停止中は `false` です。
- `IsLooping()` と `Volume()` は現在のループ状態・音量を返します。
- `Release()` はボイスと音声データを解放します。

### `MidiPlayer`

`MidiPlayer` は Windows MCI で Standard MIDI File（`.mid`）を再生します。`LoadFile(path)` は `char` または `wchar_t` のパス、`LoadMemory(data, size)` は MIDI ファイルのバイト列を受け取ります。ロード後は `Play(loop)`、`Stop`、`Pause`、`Resume`、`SetVolume`、`IsPlaying`、`IsLooping`、`Volume` を使えます。MIDI の音量はシステムの MIDI 出力にも反映されます。

MCI にはこのクラス独自の再生完了ループ設定がないため、`Play(true)` を使う場合はゲームループから定期的に `Update()` を呼びます。再生終了を検出すると先頭から再生し直します。`Release()` は MCI セッションを閉じ、`LoadMemory` が作成した一時ファイルも削除します。

```cpp
library::AudioEngine audio;
if (!audio.Initialize()) return 1;

library::WavSound effect;
if (!effect.LoadFile(audio, L"assets/click.wav")) return 1;
effect.SetVolume(0.7f);
effect.Play();

library::MidiPlayer music;
if (!music.LoadFile(L"assets/theme.mid")) return 1;
music.Play(true);

while (app.PumpMessages()) {
    music.Update(); // MIDI をループ再生する間、定期的に呼ぶ
}

// effect / music を audio.Shutdown() より先に解放する
```

## キーボード入力

### `Keyboard` と `Key`

`Keyboard` は `GetAsyncKeyState` を使い、最大 256 個の仮想キーコードについてフレームごとの状態を保持します。`Initialize(app.Window())` のようにウィンドウを渡すと、そのウィンドウがフォアグラウンドでない間はすべてのキーを離された状態として扱います。`Initialize()` または `Initialize(nullptr)` ならフォーカスによる絞り込みを行いません。

ゲームループの各フレームで `Update()` を呼び、その後に状態を読みます。

- `IsDown(key)`: 現在のフレームで押されている。
- `WasPressed(key)`: 前フレームでは離されており、現在のフレームで押されている。
- `WasReleased(key)`: 前フレームでは押されており、現在のフレームで離されている。

引数は `Key` または Win32 の仮想キーコード（`unsigned int`）です。`Key` には矢印、Enter、Escape、Space、Shift、Control、Alt、F1〜F12 などが定義されています。定義外のキーも `'A'` などの仮想キーコードを直接指定できます。範囲外のコード（256 以上）は常に `false` です。初期化状態は `IsInitialized()`、対象ウィンドウは `Window()` で取得できます。`Shutdown()` は状態とウィンドウ指定をリセットします。

```cpp
library::Keyboard keyboard;
keyboard.Initialize(app.Window());

while (app.PumpMessages()) {
    keyboard.Update();

    if (keyboard.IsDown(library::Key::Left)) {
        // 左キーを押している間の処理
    }
    if (keyboard.WasPressed(library::Key::Space)) {
        // Space キーを押したフレームだけの処理
    }
}
```

## バイナリファイルユーティリティ

`File.hpp` の `ReadBinaryFile` はファイル全体を `std::vector<unsigned char>` に読み込みます。`char` と `wchar_t` のパスを指定できます。`WriteBinaryFile` は `char` のパスとバイト列を書き出します。

```cpp
std::vector<unsigned char> bytes;
if (library::ReadBinaryFile(L"assets/image.bin", &bytes)) {
    // bytes.data() と bytes.size() を LoadMemory に渡せる
}

library::WriteBinaryFile("cache.bin", bytes.data(),
                         static_cast<unsigned int>(bytes.size()));
```

読み込みは 4 GiB 未満に制限され、失敗時は `false` を返します。書き込み時、サイズが 0 より大きい場合はデータポインターが必要です。これらの関数はファイルをそのまま扱い、パス解決やアーカイブ検索は行いません。

## ビルド

CMake 3.16 以降、Windows 10/11 SDK、および C++17 対応コンパイラーで構成・ビルドします。プロジェクトでは静的ライブラリ `Dx11library`（別名 `Dx11library::Dx11library`）と、サンプル実行ファイル `library` を定義しています。

```powershell
cmake -S . -B build
cmake --build build --config Debug
```

Ninja を使う場合はジェネレーターを指定します。

```powershell
cmake -S . -B out/build -G Ninja
cmake --build out/build
```

アプリケーションのターゲットでは `Dx11library::Dx11library` をリンクし、公開ヘッダーのインクルードパスと必要な Windows ライブラリは CMake ターゲット経由で引き継がれます。
