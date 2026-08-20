#include "Dx11library/Camera3D.hpp"

#include <cmath>
#include <cstring>

namespace library
{

        namespace
        {

            const float kDegToRad = 3.14159265358979323846f / 180.0f;

            // All matrices are stored in column-major order (same as HLSL float4x4).
            // Layout:  m[col*4 + row]
            //
            //   m[0]  m[4]  m[8]   m[12]
            //   m[1]  m[5]  m[9]   m[13]
            //   m[2]  m[6]  m[10]  m[14]
            //   m[3]  m[7]  m[11]  m[15]

            void MatIdentity(float m[16])
            {
                std::memset(m, 0, sizeof(float) * 16);
                m[0] = m[5] = m[10] = m[15] = 1.0f;
            }

            void MatTranslation(float m[16], float tx, float ty, float tz)
            {
                MatIdentity(m);
                m[12] = tx;
                m[13] = ty;
                m[14] = tz;
            }

            void MatRotationX(float m[16], float rad)
            {
                MatIdentity(m);
                const float c = cosf(rad), s = sinf(rad);
                m[5] = c;
                m[6] = s;
                m[9] = -s;
                m[10] = c;
            }

            void MatRotationY(float m[16], float rad)
            {
                MatIdentity(m);
                const float c = cosf(rad), s = sinf(rad);
                m[0] = c;
                m[2] = -s;
                m[8] = s;
                m[10] = c;
            }

            void MatRotationZ(float m[16], float rad)
            {
                MatIdentity(m);
                const float c = cosf(rad), s = sinf(rad);
                m[0] = c;
                m[1] = s;
                m[4] = -s;
                m[5] = c;
            }

            // Column-major 4x4 multiply:  out = A * B
            void MatMultiply(float out[16], const float a[16], const float b[16])
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
                std::memcpy(out, tmp, sizeof(float) * 16);
            }

        } // anonymous namespace

        Camera3D::Camera3D()
            : x_(0.0f), y_(0.0f), z_(0.0f),
              pitch_(0.0f), yaw_(0.0f), roll_(0.0f)
        {
        }

        void Camera3D::SetPosition(float x, float y, float z)
        {
            x_ = x;
            y_ = y;
            z_ = z;
        }
        void Camera3D::Move(float dx, float dy, float dz)
        {
            x_ += dx;
            y_ += dy;
            z_ += dz;
        }

        void Camera3D::SetRotation(float pitch, float yaw, float roll)
        {
            pitch_ = pitch;
            yaw_ = yaw;
            roll_ = roll;
        }

        void Camera3D::Rotate(float dp, float dy, float dr)
        {
            pitch_ += dp;
            yaw_ += dy;
            roll_ += dr;
        }

        void Camera3D::Reset()
        {
            x_ = y_ = z_ = 0.0f;
            pitch_ = yaw_ = roll_ = 0.0f;
        }

        void Camera3D::BuildViewMatrix(float out[16],
                                       float screenWidth, float screenHeight) const
        {
            // View = T2 * Rot * T1
            //
            // T1 : translate world so that the rotation centre (screen centre +
            //      camera offset) moves to the origin.
            // Rot: combined rotation  Rz(roll) * Rx(pitch) * Ry(yaw).
            // T2 : translate back so that the screen centre maps to the screen centre.
            //
            // When all values are zero, View = Identity (no effect).

            const float cx = screenWidth * 0.5f;
            const float cy = screenHeight * 0.5f;

            // T1 — move (camera target + camera offset) to origin
            float t1[16];
            MatTranslation(t1, -cx - x_, -cy - y_, 0.0f);

            // Combined rotation (applied right-to-left: Yaw → Pitch → Roll)
            float ry[16], rx[16], rz[16];
            MatRotationY(ry, yaw_ * kDegToRad);
            MatRotationX(rx, pitch_ * kDegToRad);
            MatRotationZ(rz, roll_ * kDegToRad);

            float rxry[16], rot[16];
            MatMultiply(rxry, rx, ry);
            MatMultiply(rot, rz, rxry);

            // T2 — translate back to screen centre
            float t2[16];
            MatTranslation(t2, cx, cy, 0.0f);

            // Combine: View = T2 * Rot * T1
            float rotT1[16];
            MatMultiply(rotT1, rot, t1);
            MatMultiply(out, t2, rotT1);
        }

} // namespace library
