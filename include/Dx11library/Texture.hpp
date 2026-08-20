#pragma once

#include "Dx11library/App.hpp"

namespace library {

class Renderer2D;

struct RectF
{
    RectF(float x0 = 0.0f, float y0 = 0.0f, float x1 = 0.0f, float y1 = 0.0f)
        : left(x0), top(y0), right(x1), bottom(y1)
    {
    }

    float left;
    float top;
    float right;
    float bottom;
};

// A WIC-decoded RGBA texture with a D3D11 shader-resource view.
class Texture
{
public:
    Texture();
    ~Texture();

    bool LoadFile(ID3D11Device *device, const wchar_t *path);
    bool LoadMemory(ID3D11Device *device, const void *data, unsigned int size);
    void Release();

    bool Draw(Renderer2D &renderer, const App &app, const RectF &destination,
              const RECT *source = 0, const Color &color = Color(1.0f, 1.0f, 1.0f, 1.0f)) const;

    ID3D11ShaderResourceView *View() const { return view_; }
    unsigned int Width() const { return width_; }
    unsigned int Height() const { return height_; }

private:
    ID3D11ShaderResourceView *view_;
    unsigned int width_;
    unsigned int height_;

    Texture(const Texture &);
    Texture &operator=(const Texture &);
};

} // namespace library
