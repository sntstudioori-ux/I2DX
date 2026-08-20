#include "Dx11library/Texture.hpp"

#include "Dx11library/Renderer2D.hpp"

#include <wincodec.h>
#include <vector>

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

bool CreateTextureFromFrame(ID3D11Device *device, IWICBitmapFrameDecode *frame,
                            ID3D11ShaderResourceView **view, unsigned int *width, unsigned int *height)
{
    IWICImagingFactory *factory = 0;
    IWICFormatConverter *converter = 0;
    HRESULT result = CoCreateInstance(CLSID_WICImagingFactory, 0, CLSCTX_INPROC_SERVER,
                                      IID_IWICImagingFactory, reinterpret_cast<void **>(&factory));
    if (SUCCEEDED(result)) result = factory->CreateFormatConverter(&converter);
    if (SUCCEEDED(result))
        result = converter->Initialize(frame, GUID_WICPixelFormat32bppRGBA,
                                       WICBitmapDitherTypeNone, 0, 0.0, WICBitmapPaletteTypeCustom);
    UINT imageWidth = 0;
    UINT imageHeight = 0;
    if (SUCCEEDED(result)) result = converter->GetSize(&imageWidth, &imageHeight);
    const UINT rowPitch = imageWidth * 4;
    std::vector<unsigned char> pixels(rowPitch * imageHeight);
    if (SUCCEEDED(result))
        result = converter->CopyPixels(0, rowPitch, static_cast<UINT>(pixels.size()), &pixels[0]);
    if (SUCCEEDED(result)) {
        D3D11_TEXTURE2D_DESC description;
        ZeroMemory(&description, sizeof(description));
        description.Width = imageWidth;
        description.Height = imageHeight;
        description.MipLevels = 1;
        description.ArraySize = 1;
        description.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        description.SampleDesc.Count = 1;
        description.Usage = D3D11_USAGE_DEFAULT;
        description.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        D3D11_SUBRESOURCE_DATA initialData;
        initialData.pSysMem = &pixels[0];
        initialData.SysMemPitch = rowPitch;
        initialData.SysMemSlicePitch = 0;
        ID3D11Texture2D *texture = 0;
        result = device->CreateTexture2D(&description, &initialData, &texture);
        if (SUCCEEDED(result)) result = device->CreateShaderResourceView(texture, 0, view);
        ReleaseCom(texture);
    }
    ReleaseCom(converter);
    ReleaseCom(factory);
    if (FAILED(result)) return false;
    *width = imageWidth;
    *height = imageHeight;
    return true;
}
}

Texture::Texture() : view_(0), width_(0), height_(0) {}
Texture::~Texture() { Release(); }

bool Texture::LoadFile(ID3D11Device *device, const wchar_t *path)
{
    Release();
    if (!device || !path) return false;
    IWICImagingFactory *factory = 0;
    IWICBitmapDecoder *decoder = 0;
    IWICBitmapFrameDecode *frame = 0;
    HRESULT result = CoCreateInstance(CLSID_WICImagingFactory, 0, CLSCTX_INPROC_SERVER,
                                      IID_IWICImagingFactory, reinterpret_cast<void **>(&factory));
    if (SUCCEEDED(result)) result = factory->CreateDecoderFromFilename(path, 0, GENERIC_READ,
                                                                         WICDecodeMetadataCacheOnLoad, &decoder);
    if (SUCCEEDED(result)) result = decoder->GetFrame(0, &frame);
    const bool success = SUCCEEDED(result) && CreateTextureFromFrame(device, frame, &view_, &width_, &height_);
    ReleaseCom(frame);
    ReleaseCom(decoder);
    ReleaseCom(factory);
    return success;
}

bool Texture::LoadMemory(ID3D11Device *device, const void *data, unsigned int size)
{
    Release();
    if (!device || !data || !size) return false;
    IWICImagingFactory *factory = 0;
    IWICStream *stream = 0;
    IWICBitmapDecoder *decoder = 0;
    IWICBitmapFrameDecode *frame = 0;
    HRESULT result = CoCreateInstance(CLSID_WICImagingFactory, 0, CLSCTX_INPROC_SERVER,
                                      IID_IWICImagingFactory, reinterpret_cast<void **>(&factory));
    if (SUCCEEDED(result)) result = factory->CreateStream(&stream);
    if (SUCCEEDED(result)) result = stream->InitializeFromMemory(reinterpret_cast<BYTE *>(const_cast<void *>(data)), size);
    if (SUCCEEDED(result)) result = factory->CreateDecoderFromStream(stream, 0, WICDecodeMetadataCacheOnLoad, &decoder);
    if (SUCCEEDED(result)) result = decoder->GetFrame(0, &frame);
    const bool success = SUCCEEDED(result) && CreateTextureFromFrame(device, frame, &view_, &width_, &height_);
    ReleaseCom(frame);
    ReleaseCom(decoder);
    ReleaseCom(stream);
    ReleaseCom(factory);
    return success;
}

void Texture::Release()
{
    ReleaseCom(view_);
    width_ = height_ = 0;
}

bool Texture::Draw(Renderer2D &renderer, const App &app, const RectF &destination,
                   const RECT *source, const Color &color) const
{
    return view_ && renderer.Draw(app.Context(), view_, width_, height_, app.Width(), app.Height(),
                                  destination, source, color);
}

} // namespace library
