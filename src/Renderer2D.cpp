#include "Dx11library/Renderer2D.hpp"

#include <d3dcompiler.h>
#include <string.h>

namespace library {

namespace {
struct Vertex { float x, y, u, v, r, g, b, a; };
struct Constants { float inverseWidth, inverseHeight, padding[2]; };

const char kShaderSource[] =
    "cbuffer Constants : register(b0) { float2 inverseViewport; float2 padding; };"
    "struct Input { float2 position : POSITION; float2 uv : TEXCOORD; float4 color : COLOR; };"
    "struct Output { float4 position : SV_POSITION; float2 uv : TEXCOORD; float4 color : COLOR; };"
    "Output VS(Input input) { Output output;"
    " output.position = float4(input.position.x * inverseViewport.x * 2 - 1,"
    " 1 - input.position.y * inverseViewport.y * 2, 0, 1);"
    " output.uv = input.uv; output.color = input.color; return output; }"
    "Texture2D image : register(t0); SamplerState imageSampler : register(s0);"
    "float4 PS(Output input) : SV_TARGET { return image.Sample(imageSampler, input.uv) * input.color; }";

template <typename T>
void Release(T *&object)
{
    if (object) { object->Release(); object = 0; }
}
}

Renderer2D::Renderer2D()
    : vertexShader_(0), pixelShader_(0), inputLayout_(0), vertexBuffer_(0), constantBuffer_(0), sampler_(0), blendState_(0)
{
}

Renderer2D::~Renderer2D() { Shutdown(); }

bool Renderer2D::Initialize(ID3D11Device *device)
{
    Shutdown();
    if (!device) return false;
    ID3DBlob *vertexCode = 0;
    ID3DBlob *pixelCode = 0;
    HRESULT result = D3DCompile(kShaderSource, strlen(kShaderSource), 0, 0, 0, "VS", "vs_4_0", 0, 0, &vertexCode, 0);
    if (SUCCEEDED(result)) result = D3DCompile(kShaderSource, strlen(kShaderSource), 0, 0, 0, "PS", "ps_4_0", 0, 0, &pixelCode, 0);
    if (SUCCEEDED(result)) result = device->CreateVertexShader(vertexCode->GetBufferPointer(), vertexCode->GetBufferSize(), 0, &vertexShader_);
    if (SUCCEEDED(result)) result = device->CreatePixelShader(pixelCode->GetBufferPointer(), pixelCode->GetBufferSize(), 0, &pixelShader_);
    D3D11_INPUT_ELEMENT_DESC elements[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    if (SUCCEEDED(result)) result = device->CreateInputLayout(elements, ARRAYSIZE(elements), vertexCode->GetBufferPointer(), vertexCode->GetBufferSize(), &inputLayout_);
    D3D11_BUFFER_DESC buffer;
    ZeroMemory(&buffer, sizeof(buffer));
    buffer.ByteWidth = sizeof(Vertex) * 4;
    buffer.Usage = D3D11_USAGE_DYNAMIC;
    buffer.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    buffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    if (SUCCEEDED(result)) result = device->CreateBuffer(&buffer, 0, &vertexBuffer_);
    buffer.ByteWidth = sizeof(Constants);
    buffer.Usage = D3D11_USAGE_DYNAMIC;
    buffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    buffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    if (SUCCEEDED(result)) result = device->CreateBuffer(&buffer, 0, &constantBuffer_);
    D3D11_SAMPLER_DESC sampler;
    ZeroMemory(&sampler, sizeof(sampler));
    sampler.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    sampler.AddressU = sampler.AddressV = sampler.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampler.MaxLOD = D3D11_FLOAT32_MAX;
    if (SUCCEEDED(result)) result = device->CreateSamplerState(&sampler, &sampler_);
    D3D11_BLEND_DESC blend;
    ZeroMemory(&blend, sizeof(blend));
    blend.RenderTarget[0].BlendEnable = TRUE;
    blend.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blend.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blend.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blend.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blend.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
    blend.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blend.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    if (SUCCEEDED(result)) result = device->CreateBlendState(&blend, &blendState_);
    Release(pixelCode);
    Release(vertexCode);
    if (FAILED(result)) Shutdown();
    return SUCCEEDED(result);
}

void Renderer2D::Shutdown()
{
    Release(blendState_);
    Release(sampler_);
    Release(constantBuffer_);
    Release(vertexBuffer_);
    Release(inputLayout_);
    Release(pixelShader_);
    Release(vertexShader_);
}

bool Renderer2D::Draw(ID3D11DeviceContext *context, ID3D11ShaderResourceView *texture,
                      unsigned int textureWidth, unsigned int textureHeight,
                      unsigned int targetWidth, unsigned int targetHeight,
                      const RectF &destination, const RECT *source, const Color &color)
{
    if (!context || !texture || !textureWidth || !textureHeight || !targetWidth || !targetHeight || !vertexBuffer_) return false;
    RECT full = { 0, 0, static_cast<LONG>(textureWidth), static_cast<LONG>(textureHeight) };
    const RECT &src = source ? *source : full;
    const float u0 = static_cast<float>(src.left) / textureWidth;
    const float v0 = static_cast<float>(src.top) / textureHeight;
    const float u1 = static_cast<float>(src.right) / textureWidth;
    const float v1 = static_cast<float>(src.bottom) / textureHeight;
    const Vertex vertices[] = {
        { destination.left, destination.top, u0, v0, color.r, color.g, color.b, color.a },
        { destination.right, destination.top, u1, v0, color.r, color.g, color.b, color.a },
        { destination.left, destination.bottom, u0, v1, color.r, color.g, color.b, color.a },
        { destination.right, destination.bottom, u1, v1, color.r, color.g, color.b, color.a }
    };
    D3D11_MAPPED_SUBRESOURCE mapped;
    if (FAILED(context->Map(vertexBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) return false;
    memcpy(mapped.pData, vertices, sizeof(vertices));
    context->Unmap(vertexBuffer_, 0);
    Constants constants = { 1.0f / targetWidth, 1.0f / targetHeight, { 0.0f, 0.0f } };
    if (FAILED(context->Map(constantBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) return false;
    memcpy(mapped.pData, &constants, sizeof(constants));
    context->Unmap(constantBuffer_, 0);
    const UINT stride = sizeof(Vertex);
    const UINT offset = 0;
    const float blendFactor[] = { 0, 0, 0, 0 };
    context->IASetInputLayout(inputLayout_);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    context->IASetVertexBuffers(0, 1, &vertexBuffer_, &stride, &offset);
    context->VSSetShader(vertexShader_, 0, 0);
    context->VSSetConstantBuffers(0, 1, &constantBuffer_);
    context->PSSetShader(pixelShader_, 0, 0);
    context->PSSetShaderResources(0, 1, &texture);
    context->PSSetSamplers(0, 1, &sampler_);
    context->OMSetBlendState(blendState_, blendFactor, 0xffffffff);
    context->Draw(4, 0);
    return true;
}

} // namespace library
