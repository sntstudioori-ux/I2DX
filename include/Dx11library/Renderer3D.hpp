#pragma once

#include "Dx11library/App.hpp"
#include "Dx11library/Texture.hpp"

namespace library
{

        class Camera3D;

        // Draws textured quads in 3D space using perspective projection.
        // Z = 0 coincides with the normal 2D screen plane so that 3D objects
        // can be mixed with the existing Renderer2D output.
        // An optional Camera3D can be attached via SetCamera() to add
        // view-space translation (scroll / zoom) and rotation.
        class Renderer3D
        {
        public:
            Renderer3D();
            ~Renderer3D();

            bool Initialize(ID3D11Device *device);
            void Shutdown();

            // Draw a textured quad at two 3D corners (x1,y1,z1)–(x2,y2,z2).
            // Coordinates are given in the same screen-pixel space as Renderer2D
            // (origin at top-left), with Z representing depth.
            // Z = 0 is the 2D screen plane; positive Z moves into the screen.
            bool Draw(ID3D11DeviceContext *context, ID3D11ShaderResourceView *texture,
                      unsigned int textureWidth, unsigned int textureHeight,
                      unsigned int targetWidth, unsigned int targetHeight,
                      float x1, float y1, float z1,
                      float x2, float y2, float z2,
                      const RECT *source, const Color &color);

            // Field-of-view in degrees (default 60). Affects perspective strength.
            void SetFov(float degrees) { fovDegrees_ = degrees; }
            float GetFov() const { return fovDegrees_; }

            // Attach a camera whose view matrix is applied every Draw call.
            // Pass 0 (null) to disable the camera and return to default behaviour.
            // The Renderer3D does NOT own the pointer; the caller must keep it alive.
            void SetCamera(const Camera3D *camera) { camera_ = camera; }
            const Camera3D *GetCamera() const { return camera_; }

        private:
            ID3D11VertexShader *vertexShader_;
            ID3D11PixelShader *pixelShader_;
            ID3D11InputLayout *inputLayout_;
            ID3D11Buffer *vertexBuffer_;
            ID3D11Buffer *constantBuffer_;
            ID3D11SamplerState *sampler_;
            ID3D11BlendState *blendState_;
            ID3D11DepthStencilState *depthStencilState_;

            float fovDegrees_;
            const Camera3D *camera_;

            Renderer3D(const Renderer3D &);
            Renderer3D &operator=(const Renderer3D &);
        };

} // namespace library
