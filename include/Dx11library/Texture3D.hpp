#pragma once

#include "Dx11library/Texture.hpp"

namespace library
{

        class Renderer3D;

        // A 3D rectangle in screen-pixel space with depth.
        // (x1,y1,z1) is the top-left corner and (x2,y2,z2) is the bottom-right.
        // Z = 0 is the standard 2D screen plane; positive Z moves into the screen.
        struct RectF3D
        {
            RectF3D(float ax = 0.0f, float ay = 0.0f, float az = 0.0f,
                    float bx = 0.0f, float by = 0.0f, float bz = 0.0f)
                : x1(ax), y1(ay), z1(az), x2(bx), y2(by), z2(bz)
            {
            }

            float x1, y1, z1;
            float x2, y2, z2;
        };

        // Extends Texture with a Draw overload that takes 3D coordinates.
        // Loads and manages the same texture data as the base class.
        class Texture3D : public Texture
        {
        public:
            Texture3D();
            ~Texture3D();

            // Draw the texture as a quad at the given 3D coordinates.
            // (x1,y1,z1) = top-left corner, (x2,y2,z2) = bottom-right corner.
            // source = optional sub-rectangle of the texture to sample (NULL = whole texture).
            // color  = multiplicative tint (default white/opaque).
            bool Draw(Renderer3D &renderer, const App &app,
                      float x1, float y1, float z1,
                      float x2, float y2, float z2,
                      const RECT *source = 0,
                      const Color &color = Color(1.0f, 1.0f, 1.0f, 1.0f)) const;

            // Convenience overload using a RectF3D.
            bool Draw(Renderer3D &renderer, const App &app, const RectF3D &destination,
                      const RECT *source = 0,
                      const Color &color = Color(1.0f, 1.0f, 1.0f, 1.0f)) const;
        };

} // namespace library
