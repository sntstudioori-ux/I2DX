#include "Dx11library/Font.hpp"

#include <d2d1_1.h>
#include <dwrite_3.h>
#include <float.h>
#include <wchar.h>

namespace library {

namespace {
template <typename T>
void ReleaseCom(T *&object)
{
    if (object) {
        object->Release();
        object = 0;
    }
}

float ClampOpacity(float value)
{
    return value < 0.0f ? 0.0f : (value > 1.0f ? 1.0f : value);
}

D2D1_COLOR_F ToD2DColor(const Color &color, float opacity)
{
    return D2D1::ColorF(color.r, color.g, color.b, ClampOpacity(color.a * opacity));
}
}

FontRenderer::FontRenderer()
    : factory_(0), device_(0), context_(0), target_(0), textBitmap_(0), writeFactory_(0),
      targetWidth_(0), targetHeight_(0), drawing_(false)
{
}

FontRenderer::~FontRenderer() { Shutdown(); }

bool FontRenderer::Initialize(const App &app)
{
    Shutdown();
    if (!app.Device()) return false;

    HRESULT result = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, IID_PPV_ARGS(&factory_));
    IDXGIDevice *dxgiDevice = 0;
    if (SUCCEEDED(result)) result = app.Device()->QueryInterface(IID_PPV_ARGS(&dxgiDevice));
    if (SUCCEEDED(result)) result = factory_->CreateDevice(dxgiDevice, &device_);
    if (SUCCEEDED(result)) result = device_->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &context_);
    if (SUCCEEDED(result))
        result = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory5),
                                    reinterpret_cast<IUnknown **>(&writeFactory_));
    ReleaseCom(dxgiDevice);
    if (FAILED(result)) Shutdown();
    return SUCCEEDED(result);
}

void FontRenderer::Shutdown()
{
    if (drawing_ && context_) context_->EndDraw();
    drawing_ = false;
    ReleaseCom(textBitmap_);
    ReleaseCom(target_);
    ReleaseCom(writeFactory_);
    ReleaseCom(context_);
    ReleaseCom(device_);
    ReleaseCom(factory_);
    targetWidth_ = targetHeight_ = 0;
}

bool FontRenderer::CreateTarget(const App &app)
{
    if (!app.SwapChain()) return false;
    ReleaseCom(textBitmap_);
    ReleaseCom(target_);
    IDXGISurface *surface = 0;
    HRESULT result = app.SwapChain()->GetBuffer(0, IID_PPV_ARGS(&surface));
    const D2D1_BITMAP_PROPERTIES1 properties = D2D1::BitmapProperties1(
        D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
        D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_IGNORE));
    if (SUCCEEDED(result)) result = context_->CreateBitmapFromDxgiSurface(surface, &properties, &target_);
    ReleaseCom(surface);
    if (SUCCEEDED(result)) {
        const D2D1_BITMAP_PROPERTIES1 textProperties = D2D1::BitmapProperties1(
            D2D1_BITMAP_OPTIONS_TARGET,
            D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
        result = context_->CreateBitmap(
            D2D1::SizeU(app.Width(), app.Height()), 0, 0, &textProperties, &textBitmap_);
    }
    if (SUCCEEDED(result)) {
        targetWidth_ = app.Width();
        targetHeight_ = app.Height();
    }
    return SUCCEEDED(result);
}

bool FontRenderer::Begin(const App &app)
{
    if (!context_ || drawing_ || !app.Context()) return false;
    if (!target_ || !textBitmap_ || targetWidth_ != app.Width() || targetHeight_ != app.Height()) {
        if (!CreateTarget(app)) return false;
    }
    app.Context()->Flush();
    context_->SetTarget(textBitmap_);
    context_->SetTransform(D2D1::Matrix3x2F::Identity());
    context_->SetTextAntialiasMode(app.Antialias()
        ? D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE
        : D2D1_TEXT_ANTIALIAS_MODE_ALIASED);
    context_->BeginDraw();
    context_->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));
    drawing_ = true;
    return true;
}

bool FontRenderer::End(const App &app)
{
    if (!context_ || !drawing_) return false;
    HRESULT result = context_->EndDraw();
    if (SUCCEEDED(result)) {
        context_->SetTarget(target_);
        context_->SetTransform(D2D1::Matrix3x2F::Identity());
        context_->BeginDraw();
        const D2D1_RECT_F viewport = D2D1::RectF(
            app.ViewportOffsetX(), app.ViewportOffsetY(),
            app.ViewportOffsetX() + static_cast<float>(targetWidth_) * app.ViewportScale(),
            app.ViewportOffsetY() + static_cast<float>(targetHeight_) * app.ViewportScale());
        context_->DrawBitmap(textBitmap_, &viewport, 1.0f,
                             D2D1_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
        result = context_->EndDraw();
    }
    drawing_ = false;
    return SUCCEEDED(result);
}

Font::Font()
    : writeFactory_(0), collection_(0), format_(0), color_(1.0f, 1.0f, 1.0f, 1.0f),
      gradientTop_(1.0f, 1.0f, 1.0f, 1.0f), gradientBottom_(1.0f, 1.0f, 1.0f, 1.0f),
      opacity_(1.0f), gradient_(false)
{
}

Font::~Font() { Release(); }

bool Font::LoadSystem(FontRenderer &renderer, const wchar_t *familyName, float size)
{
    Release();
    if (!renderer.WriteFactory() || !familyName || size <= 0.0f) return false;
    writeFactory_ = renderer.WriteFactory();
    writeFactory_->AddRef();
    if (!CreateTextFormat(familyName, size)) {
        Release();
        return false;
    }
    return true;
}

bool Font::LoadFile(FontRenderer &renderer, const wchar_t *path, const wchar_t *familyName, float size)
{
    Release();
    if (!renderer.WriteFactory() || !path || !familyName || size <= 0.0f) return false;

    IDWriteFontFile *fontFile = 0;
    IDWriteFontSetBuilder1 *builder = 0;
    IDWriteFontSet *fontSet = 0;
    HRESULT result = renderer.WriteFactory()->CreateFontFileReference(path, 0, &fontFile);
    if (SUCCEEDED(result)) result = renderer.WriteFactory()->CreateFontSetBuilder(&builder);
    if (SUCCEEDED(result)) result = builder->AddFontFile(fontFile);
    if (SUCCEEDED(result)) result = builder->CreateFontSet(&fontSet);
    if (SUCCEEDED(result)) result = renderer.WriteFactory()->CreateFontCollectionFromFontSet(fontSet, &collection_);
    ReleaseCom(fontSet);
    ReleaseCom(builder);
    ReleaseCom(fontFile);
    if (FAILED(result)) {
        Release();
        return false;
    }

    writeFactory_ = renderer.WriteFactory();
    writeFactory_->AddRef();
    if (!CreateTextFormat(familyName, size)) {
        Release();
        return false;
    }
    return true;
}

void Font::Release()
{
    ReleaseCom(format_);
    ReleaseCom(collection_);
    ReleaseCom(writeFactory_);
    familyName_.clear();
    gradient_ = false;
}

bool Font::CreateTextFormat(const wchar_t *familyName, float size)
{
    if (!writeFactory_ || !familyName || size <= 0.0f) return false;
    IDWriteTextFormat *format = 0;
    const HRESULT result = writeFactory_->CreateTextFormat(
        familyName, collection_, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL, size, L"", &format);
    if (FAILED(result)) return false;
    ReleaseCom(format_);
    format_ = format;
    familyName_ = familyName;
    return true;
}

bool Font::SetSize(float size)
{
    return !familyName_.empty() && CreateTextFormat(familyName_.c_str(), size);
}

void Font::SetOpacity(float opacity) { opacity_ = ClampOpacity(opacity); }

void Font::SetGradient(const Color &top, const Color &bottom)
{
    gradientTop_ = top;
    gradientBottom_ = bottom;
    gradient_ = true;
}

TextSize Font::Measure(const wchar_t *text) const
{
    if (!writeFactory_ || !format_ || !text) return TextSize();
    IDWriteTextLayout *layout = 0;
    HRESULT result = writeFactory_->CreateTextLayout(text, static_cast<UINT32>(wcslen(text)), format_,
                                                     FLT_MAX, FLT_MAX, &layout);
    DWRITE_TEXT_METRICS metrics = {};
    if (SUCCEEDED(result)) result = layout->GetMetrics(&metrics);
    ReleaseCom(layout);
    if (FAILED(result)) return TextSize();
    return TextSize(metrics.widthIncludingTrailingWhitespace, metrics.height);
}

bool Font::Draw(FontRenderer &renderer, const wchar_t *text, const RectF &destination) const
{
    ID2D1DeviceContext *context = renderer.Context();
    if (!renderer.IsDrawing() || !context || !format_ || !text) return false;
    const D2D1_RECT_F rectangle = D2D1::RectF(destination.left, destination.top, destination.right, destination.bottom);
    ID2D1Brush *brush = 0;
    HRESULT result;
    if (gradient_) {
        const D2D1_GRADIENT_STOP stops[] = {
            { 0.0f, ToD2DColor(gradientTop_, opacity_) },
            { 1.0f, ToD2DColor(gradientBottom_, opacity_) }
        };
        ID2D1GradientStopCollection *stopsCollection = 0;
        ID2D1LinearGradientBrush *gradientBrush = 0;
        result = context->CreateGradientStopCollection(stops, ARRAYSIZE(stops), &stopsCollection);
        if (SUCCEEDED(result)) {
            result = context->CreateLinearGradientBrush(
                D2D1::LinearGradientBrushProperties(
                    D2D1::Point2F(destination.left, destination.top),
                    D2D1::Point2F(destination.left, destination.bottom)),
                stopsCollection, &gradientBrush);
        }
        ReleaseCom(stopsCollection);
        brush = gradientBrush;
    } else {
        ID2D1SolidColorBrush *solidBrush = 0;
        result = context->CreateSolidColorBrush(ToD2DColor(color_, opacity_), &solidBrush);
        brush = solidBrush;
    }
    if (FAILED(result)) return false;
    context->DrawText(text, static_cast<UINT32>(wcslen(text)), format_, rectangle, brush);
    ReleaseCom(brush);
    return true;
}

} // namespace library
