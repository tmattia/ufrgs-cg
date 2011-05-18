#include <vector3f.h>

using namespace std;

// CAMERA ------------------------------------------------------
class Camera
{

    public:
        Camera();
        ~Camera();

        /**
         * Resets the camera to it's default position
         *
         * position = (0, 0, 0)
         * u = (1, 0, 0)
         * v = (0, 1, 0)
         * n = (0, 0, 1)
         * hFov = 60
         * vFov = 60
         */
        void reset();

        /**
         * Slides (translates) the camera over it's own coordinate system
         *
         * @param du Translation in u
         * @param dv Translation in v
         * @param dn Translation in n
         */
        void slide(float du, float dv, float dn);

        /**
         * Rotates the CCS around a vector defined by the cross
         * product between two vectors
         *
         * @param a     First vector
         * @param b     Second vector
         * @param angle Rotation angle (in degrees)
         */
        void rotate(vector3f &a, vector3f &b, float angle);

        /**
         * Rotates the camera around n
         *
         * @param angle Rotation angle (in degrees)
         */
        void roll(float angle);

        /**
         * Rotates the camera around u
         *
         * @param angle Rotation angle (in degrees)
         */
        void pitch(float angle);

        /**
         * Rotates the camera around v
         *
         * @param angle Rotation angle (in degrees)
         */
        void yaw(float angle);


    private:
        vector3f u;
        vector3f v;
        vector3f n;

        vector3f position;

        float hFov;
        float vFov;
};

Camera::Camera()
{
    reset();
}

Camera::~Camera()
{

}

void Camera::reset()
{
    position.set(0, 0, 0);
    u.set(1, 0, 0);
    v.set(0, 1, 0);
    n.set(0, 0, 1);
    hFov = 60;
    vFov = 60;
}

void Camera::slide(float du, float dv, float dn)
{
    position.x += du * u.x + dv * v.x + dn * n.x;
    position.y += du * u.y + dv * v.y + dn * n.y;
    position.z += du * u.z + dv * v.z + dn * n.z;
}

void Camera::rotate(vector3f &a, vector3f &b, float angle)
{
    float ang = 3.14159265 / 180.0 * angle;
    float C = cos(ang);
    float S = sin(ang);
    vector3f t(C * a.x + S * b.x, C * a.y + S * b.y, C * a.z + S * b.z);
    b.set(-S * a.x + C * b.x, -S * a.y + C * b.y, -S * a.z + C * b.z);
    a.set(t.x, t.y, t.z);
}

void Camera::roll(float angle)
{
    rotate(u, v, angle);
}

void Camera::pitch(float angle)
{
    rotate(n, v, angle);
}

void Camera::yaw(float angle)
{
    rotate(u, n, angle);
}
// /CAMERA -----------------------------------------------------

int main()
{
    return 0;
}
