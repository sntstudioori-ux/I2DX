#include "Dx11library/Renderer3D.hpp"
#include "Dx11library/Camera3D.hpp"

#include <d3dcompiler.h>
#include <string.h>
#include <cmath>

namespace library
{

        namespace
        {

            // Vertex with 3D position, UV and colour.
            struct Vertex3D
            {
                float x, y, z, u, v, r, g, b, a;
            };

            // Constants passed to the vertex shader every draw call.
            struct Constants3D
            {
                float projection[16]; // column-major 4×4
            };

            // The shader computes clip-space positions from 3D screen-pixel coords.
            // The projection matrix maps:
            //   X  from [0, viewportWidth] → [-1, 1]
            //   Y  from [0, viewportHeight] → [1, -1]  (Y-down in screen space)
            //   Z  depth via perspective
            //
            // The camera sits at Z = -d (where d is the distance derived from the FOV
            // so that the Z = 0 plane maps 1:1 to screen pixels).
            const char kShader3DSource[] =
                "cbuffer Constants : register(b0) { float4x4 projection; };\n"
                "struct Input  { float3 position : POSITION; float2 uv : TEXCOORD; float4 color : COLOR; };\n"
                "struct Output { float4 position : SV_POSITION; float2 uv : TEXCOORD; float4 color : COLOR; };\n"
                "Output VS(Input i) {\n"
                "  Output o;\n"
                "  o.position = mul(projection, float4(i.position, 1.0));\n"
                "  o.uv    = i.uv;\n"
                "  o.color = i.color;\n"
                "  return o;\n"
                "}\n"
                "Texture2D    image        : register(t0);\n"
                "SamplerState imageSampler : register(s0);\n"
                "float4 PS(Output i) : SV_TARGET { return image.Sample(imageSampler, i.uv) * i.color; }\n";

            template <typename T>
            void Release(T *&object)
            {
                if (object)
                {
                    object->Release();
                    object = 0;
                }
            }

            // Build a column-major off-centre perspective matrix.
            // Maps screen-pixel coords at Z=0 to clip space so they match the 2D
            // renderer pixel-for-pixel.
            //
            // Camera is at Z = -eyeZ, looking towards +Z.
            // nearZ = eyeZ   (the Z=0 plane is at the near clip),
            // farZ  = some large depth behind the screen.
            void BuildProjection(float out[16],
                                 float width, float height,
                                 float eyeZ, float nearZ, float farZ)
            {
                // At the near plane (Z=0 in world, distance=eyeZ from the camera),
                // the visible rectangle is [0, width] × [0, height].
                // This is an off-centre frustum (left=0, right=width, top=0, bottom=height).
                //
                // Standard off-centre perspective (column-major):
                //
                //  2n/(r-l)     0       (r+l)/(r-l)        0
                //    0       2n/(t-b)   (t+b)/(t-b)        0
                //    0          0      -(f+n)/(f-n)   -2fn/(f-n)
                //    0          0          -1              0
                //
                // But our world Z goes into screen (+Z = away), so we shift:
                //   zEye = z_world + eyeZ     (camera-relative)
                // We bake a translation of +eyeZ into Z before the projection.

                memset(out, 0, sizeof(float) * 16);

                const float n = nearZ;
                const float f = farZ;
                const float l = 0.0f;
                const float r = width;
                const float t = 0.0f;   // top = 0 (Y-down)
                const float b = height; // bottom = height

                // Column 0
                out[0] = 2.0f * n / (r - l);
                // Column 1
                out[5] = 2.0f * n / (t - b); // negative because top < bottom
                // Column 2
                out[8] = (r + l) / (r - l); // = 1
                out[9] = (t + b) / (t - b); // = -1
                out[10] = -(f + n) / (f - n);
                out[11] = -1.0f;
                // Column 3  (includes the eyeZ translation baked in)
                out[14] = -2.0f * f * n / (f - n);

                // We also need to shift world Z by +eyeZ before this projection,
                // which means adding eyeZ * column2 to column3.
                out[12] += eyeZ * out[8];
                out[13] += eyeZ * out[9];
                out[14] += eyeZ * out[10];
                out[15] += eyeZ * out[11];
            }

            // Column-major 4x4 multiply:  out = A * B
            void MatMultiply4x4(float out[16], const float a[16], const float b[16])
            {
                float tmp[16];
                for (int col = 0; col < 4; ++col)
                {
                    for (int row = 0; row < 4; ++row)
                    {
                        tmp[col * 4 + row] =
                            a[0 * 4 + row] * b[col * 4 + 0] +
                            a[1 * 4 + row] * b[col * 4 + 1] +
                            a[2 * 4 + row] * b[col * 4 + 2] +
                            a[3 * 4 + row] * b[col * 4 + 3];
                    }
                }
                memcpy(out, tmp, sizeof(float) * 16);
            }

        } // anonymous namespace

        Renderer3D::Renderer3D()
            : vertexShader_(0), pixelShader_(0), inputLayout_(0),
              vertexBuffer_(0), constantBuffer_(0), sampler_(0),
              blendState_(0), depthStencilState_(0), fovDegrees_(60.0f),
              camera_(0)
        {
        }

        Renderer3D::~Renderer3D() { Shutdown(); }

        bool Renderer3D::Initialize(ID3D11Device *device)
        {
            Shutdown();
            if (!device)
                return false;

            ID3DBlob *vertexCode = 0;
            ID3DBlob *pixelCode = 0;
            ID3DBlob *errors = 0;
            HRESULT result = D3DCompile(kShader3DSource, strlen(kShader3DSource), 0, 0, 0,
                                        "VS", "vs_4_0", 0, 0, &vertexCode, &errors);
            Release(errors);
            if (SUCCEEDED(result))
                result = D3DCompile(kShader3DSource, strlen(kShader3DSource), 0, 0, 0,
                                    "PS", "ps_4_0", 0, 0, &pixelCode, &errors);
            Release(errors);

            if (SUCCEEDED(result))
                result = device->CreateVertexShader(vertexCode->GetBufferPointer(),
                                                    vertexCode->GetBufferSize(), 0, &vertexShader_);
            if (SUCCEEDED(result))
                result = device->CreatePixelShader(pixelCode->GetBufferPointer(),
                                                   pixelCode->GetBufferSize(), 0, &pixelShader_);

            // Input layout: POSITION float3, TEXCOORD float2, COLOR float4
            D3D11_INPUT_ELEMENT_DESC elements[] = {
                {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
                {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
                {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0}};
            if (SUCCEEDED(result))
                result = device->CreateInputLayout(elements, ARRAYSIZE(elements),
                                                   vertexCode->GetBufferPointer(),
                                                   vertexCode->GetBufferSize(), &inputLayout_);

            // Vertex buffer (4 vertices per quad)
            D3D11_BUFFER_DESC bufferDesc;
            ZeroMemory(&bufferDesc, sizeof(bufferDesc));
            bufferDesc.ByteWidth = sizeof(Vertex3D) * 4;
            bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
            bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            if (SUCCEEDED(result))
                result = device->CreateBuffer(&bufferDesc, 0, &vertexBuffer_);

            // Constant buffer
            bufferDesc.ByteWidth = sizeof(Constants3D);
            bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            if (SUCCEEDED(result))
                result = device->CreateBuffer(&bufferDesc, 0, &constantBuffer_);

            // Sampler
            D3D11_SAMPLER_DESC samplerDesc;
            ZeroMemory(&samplerDesc, sizeof(samplerDesc));
            samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
            samplerDesc.AddressU = samplerDesc.AddressV = samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
            samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
            if (SUCCEEDED(result))
                result = device->CreateSamplerState(&samplerDesc, &sampler_);

            // Blend state (same alpha-blending as Renderer2D)
            D3D11_BLEND_DESC blendDesc;
            ZeroMemory(&blendDesc, sizeof(blendDesc));
            blendDesc.RenderTarget[0].BlendEnable = TRUE;
            blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
            blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
            blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
            blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
            blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
            blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
            blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
            if (SUCCEEDED(result))
                result = device->CreateBlendState(&blendDesc, &blendState_);

            // Depth-stencil state — depth testing enabled so that
            // 3D quads are correctly ordered by Z.
            D3D11_DEPTH_STENCIL_DESC dsDesc;
            ZeroMemory(&dsDesc, sizeof(dsDesc));
            dsDesc.DepthEnable = TRUE;
            dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
            dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
            if (SUCCEEDED(result))
                result = device->CreateDepthStencilState(&dsDesc, &depthStencilState_);

            Release(pixelCode);
            Release(vertexCode);
            if (FAILED(result))
                Shutdown();
            return SUCCEEDED(result);
        }

        void Renderer3D::Shutdown()
        {
            Release(depthStencilState_);
            Release(blendState_);
            Release(sampler_);
            Release(constantBuffer_);
            Release(vertexBuffer_);
            Release(inputLayout_);
            Release(pixelShader_);
            Release(vertexShader_);
        }

        bool Renderer3D::Draw(ID3D11DeviceContext *context, ID3D11ShaderResourceView *texture,
                              unsigned int textureWidth, unsigned int textureHeight,
                              unsigned int targetWidth, unsigned int targetHeight,
                              float x1, float y1, float z1,
                              float x2, float y2, float z2,
                              const RECT *source, const Color &color)
        {
            if (!context || !texture || !textureWidth || !textureHeight ||
                !targetWidth || !targetHeight || !vertexBuffer_)
                return false;

            // --- UV coordinates ------------------------------------------------
            RECT full = {0, 0, static_cast<LONG>(textureWidth), static_cast<LONG>(textureHeight)};
            const RECT &src = source ? *source : full;
            const float u0 = static_cast<float>(src.left) / textureWidth;
            const float v0 = static_cast<float>(src.top) / textureHeight;
            const float u1 = static_cast<float>(src.right) / textureWidth;
            const float v1 = static_cast<float>(src.bottom) / textureHeight;

            // Four corners of the quad in 3D screen-pixel space.
            // The quad is defined by a box with two opposite corners:
            //   (x1,y1,z1) — top-left  and  (x2,y2,z2) — bottom-right.
            // Each corner can have a different Z, producing a 3D-tilted quad.
            const Vertex3D vertices[] = {
                {x1, y1, z1, u0, v0, color.r, color.g, color.b, color.a},
                {x2, y1, z1, u1, v0, color.r, color.g, color.b, color.a},
                {x1, y2, z2, u0, v1, color.r, color.g, color.b, color.a},
                {x2, y2, z2, u1, v1, color.r, color.g, color.b, color.a}};

            D3D11_MAPPED_SUBRESOURCE mapped;
            if (FAILED(context->Map(vertexBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
                return false;
            memcpy(mapped.pData, vertices, sizeof(vertices));
            context->Unmap(vertexBuffer_, 0);

            // --- Build projection matrix ----------------------------------------
            // eyeZ = half-height / tan(fov/2)  so that Z=0 is pixel-perfect.
            const float fovRad = fovDegrees_ * 3.14159265f / 180.0f;
            const float halfH = static_cast<float>(targetHeight) * 0.5f;
            const float baseEyeZ = halfH / tanf(fovRad * 0.5f);

            // Camera Z offset adjusts the eye distance (zoom).
            float eyeZ = baseEyeZ;
            if (camera_)
            {
                eyeZ = baseEyeZ - camera_->Z();
                if (eyeZ < 1.0f)
                    eyeZ = 1.0f; // prevent degenerate near plane
            }

            float projMat[16];
            BuildProjection(projMat,
                            static_cast<float>(targetWidth),
                            static_cast<float>(targetHeight),
                            eyeZ, eyeZ, eyeZ + 10000.0f);

            // Combine with the camera view matrix (position + rotation).
            Constants3D constants;
            if (camera_)
            {
                float viewMat[16];
                camera_->BuildViewMatrix(viewMat,
                                         static_cast<float>(targetWidth),
                                         static_cast<float>(targetHeight));
                MatMultiply4x4(constants.projection, projMat, viewMat);
            }
            else
            {
                memcpy(constants.projection, projMat, sizeof(float) * 16);
            }

            if (FAILED(context->Map(constantBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
                return false;
            memcpy(mapped.pData, &constants, sizeof(constants));
            context->Unmap(constantBuffer_, 0);

            // --- Issue draw call ------------------------------------------------
            const UINT stride = sizeof(Vertex3D);
            const UINT offset = 0;
            const float blendFactor[] = {0, 0, 0, 0};

            context->IASetInputLayout(inputLayout_);
            context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
            context->IASetVertexBuffers(0, 1, &vertexBuffer_, &stride, &offset);

            context->VSSetShader(vertexShader_, 0, 0);
            context->VSSetConstantBuffers(0, 1, &constantBuffer_);
            context->PSSetShader(pixelShader_, 0, 0);
            context->PSSetShaderResources(0, 1, &texture);
            context->PSSetSamplers(0, 1, &sampler_);

            context->OMSetBlendState(blendState_, blendFactor, 0xffffffff);
            context->OMSetDepthStencilState(depthStencilState_, 0);

            context->Draw(4, 0);
            return true;
        }

} // namespace library
