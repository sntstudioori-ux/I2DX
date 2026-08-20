#include "Dx11library/Texture3D.hpp"

#include "Dx11library/Renderer3D.hpp"

namespace library {

Texture3D::Texture3D() {}
Texture3D::~Texture3D() {}

bool Texture3D::Draw(Renderer3D &renderer, const App &app,
                     float x1, float y1, float z1,
                     float x2, float y2, float z2,
                     const RECT *source, const Color &color) const
{
    return View() && renderer.Draw(app.Context(), View(),
                                   Width(), Height(),
                                   app.Width(), app.Height(),
                                   x1, y1, z1, x2, y2, z2,
                                   source, color);
}

bool Texture3D::Draw(Renderer3D &renderer, const App &app, const RectF3D &destination,
                     const RECT *source, const Color &color) const
{
    return Draw(renderer, app,
                destination.x1, destination.y1, destination.z1,
                destination.x2, destination.y2, destination.z2,
                source, color);
}

} // namespace library
