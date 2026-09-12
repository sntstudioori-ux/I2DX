cmake_minimum_required(VERSION 3.16)

project(Dx11library LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- ライブラリ定義 (Dx11library) ---
add_library(Dx11library STATIC
    src/App.cpp
    src/AudioEngine.cpp
    src/Camera3D.cpp
    src/File.cpp
    src/Font.cpp
    src/Keyboard.cpp
    src/MidiPlayer.cpp
    src/Renderer2D.cpp
    src/Renderer3D.cpp
    src/Texture.cpp
    src/Texture3D.cpp
    src/WavSound.cpp)
add_library(Dx11library::Dx11library ALIAS Dx11library)

target_compile_features(Dx11library PUBLIC cxx_std_17)
target_include_directories(Dx11library
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/Dx11library>
        $<INSTALL_INTERFACE:include>
)
target_link_libraries(Dx11library PUBLIC
    d3d11
    dxgi
    d3dcompiler
    d2d1
    dwrite
    user32
    gdi32
    ole32
    windowscodecs
    winmm
    xaudio2)

if(MSVC)
    target_compile_options(Dx11library PRIVATE /W4 /utf-8)
endif()

# --- アプリケーション定義 (library) ---
# アプリケーションのエントリーポイントは明示的に指定し、
# ビルドディレクトリ内の生成物を誤って取り込まないようにする。
add_executable(library WIN32 main.cpp)

target_compile_features(library PRIVATE cxx_std_17)
target_include_directories(library PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(library PRIVATE Dx11library::Dx11library)

if(MSVC)
    target_compile_options(library PRIVATE /W4 /utf-8)
endif()

