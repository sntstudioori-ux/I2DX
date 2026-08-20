#pragma once

namespace library
{

        // Lightweight 3D camera for simple camera effects (parallax, shake, zoom, tilt).
        // All values are relative to the default viewpoint (the standard 2D screen).
        //
        //   Position:
        //     X/Y — shifts the view in screen-pixel units.
        //           Positive X moves the camera rightward (scene shifts left).
        //           Positive Y moves the camera downward  (scene shifts up).
        //     Z   — moves the camera toward (+) or away from (-) the scene.
        //           Positive Z zooms in; negative Z zooms out.
        //
        //   Rotation (degrees, around the screen centre):
        //     Pitch — rotation around the X axis (tilt forward/backward).
        //     Yaw   — rotation around the Y axis (pan left/right).
        //     Roll  — rotation around the Z axis (screen tilt).
        class Camera3D
        {
        public:
            Camera3D();

            // --- Position (default 0,0,0 = standard viewpoint) ---
            void SetPosition(float x, float y, float z);
            void Move(float dx, float dy, float dz);
            float X() const { return x_; }
            float Y() const { return y_; }
            float Z() const { return z_; }

            // --- Rotation in degrees (default 0,0,0) ---
            void SetRotation(float pitch, float yaw, float roll);
            void Rotate(float dPitch, float dYaw, float dRoll);
            float Pitch() const { return pitch_; }
            float Yaw() const { return yaw_; }
            float Roll() const { return roll_; }

            // Reset all transforms to default.
            void Reset();

            // Build a column-major 4x4 view matrix.
            // screenWidth / screenHeight define the rotation centre (screen centre).
            void BuildViewMatrix(float out[16], float screenWidth, float screenHeight) const;

        private:
            float x_, y_, z_;
            float pitch_, yaw_, roll_;
        };

} // namespace library
