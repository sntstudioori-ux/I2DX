#pragma once

#include "Dx11library/App.hpp"
#include "Dx11library/Texture.hpp"

#include <string>

struct ID2D1Factory1;
struct ID2D1Device;
struct ID2D1DeviceContext;
struct ID2D1Bitmap1;
struct IDWriteFactory5;
struct IDWriteFontCollection1;
struct IDWriteTextFormat;

namespace library {

struct TextSize
{
    TextSize(float textWidth = 0.0f, float textHeight = 0.0f) : width(textWidth), height(textHeight) {}

    float width;
    float height;
};

// Owns Direct2D/DirectWrite resources shared by Font instances.
class FontRenderer
{
public:
    FontRenderer();
    ~FontRenderer();

    bool Initialize(const App &app);
    void Shutdown();
    bool Begin(const App &app);
    bool End(const App &app);

    IDWriteFactory5 *WriteFactory() const { return writeFactory_; }
    ID2D1DeviceContext *Context() const { return context_; }
    bool IsDrawing() const { return drawing_; }

private:
    bool CreateTarget(const App &app);

    ID2D1Factory1 *factory_;
    ID2D1Device *device_;
    ID2D1DeviceContext *context_;
    ID2D1Bitmap1 *target_;
    ID2D1Bitmap1 *textBitmap_;
    IDWriteFactory5 *writeFactory_;
    unsigned int targetWidth_;
    unsigned int targetHeight_;
    bool drawing_;

    FontRenderer(const FontRenderer &);
    FontRenderer &operator=(const FontRenderer &);
};

// A reusable text style. Each instance can use either a system or a private font file.
class Font
{
public:
    Font();
    ~Font();

    bool LoadSystem(FontRenderer &renderer, const wchar_t *familyName, float size = 16.0f);
    bool LoadFile(FontRenderer &renderer, const wchar_t *path, const wchar_t *familyName, float size = 16.0f);
    void Release();

    bool SetSize(float size);
    void SetColor(const Color &color) { color_ = color; }
    void SetOpacity(float opacity);
    void SetGradient(const Color &top, const Color &bottom);
    void ClearGradient() { gradient_ = false; }

    TextSize Measure(const wchar_t *text) const;
    bool Draw(FontRenderer &renderer, const wchar_t *text, const RectF &destination) const;

private:
    bool CreateTextFormat(const wchar_t *familyName, float size);

    IDWriteFactory5 *writeFactory_;
    IDWriteFontCollection1 *collection_;
    IDWriteTextFormat *format_;
    Color color_;
    Color gradientTop_;
    Color gradientBottom_;
    std::wstring familyName_;
    float opacity_;
    bool gradient_;

    Font(const Font &);
    Font &operator=(const Font &);
};

} // namespace library
