#pragma once

#include "Dx11library/App.hpp"
#include "Dx11library/Texture.hpp"

namespace library {

// Compiles a small built-in shader pair and draws textured screen-space quads.
class Renderer2D
{
public:
    Renderer2D();
    ~Renderer2D();

    bool Initialize(ID3D11Device *device);
    void Shutdown();
    bool Draw(ID3D11DeviceContext *context, ID3D11ShaderResourceView *texture,
              unsigned int textureWidth, unsigned int textureHeight,
              unsigned int targetWidth, unsigned int targetHeight,
              const RectF &destination, const RECT *source, const Color &color);

private:
    ID3D11VertexShader *vertexShader_;
    ID3D11PixelShader *pixelShader_;
    ID3D11InputLayout *inputLayout_;
    ID3D11Buffer *vertexBuffer_;
    ID3D11Buffer *constantBuffer_;
    ID3D11SamplerState *sampler_;
    ID3D11BlendState *blendState_;

    Renderer2D(const Renderer2D &);
    Renderer2D &operator=(const Renderer2D &);
};

} // namespace library
