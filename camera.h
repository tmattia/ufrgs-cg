#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <model.h>
#include <vector3f.h>

using namespace std;

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
         * Sets the camera
         *
         * @param pos_x Eye X
         * @param pos_y Eye Y
         * @param pos_z Eye Z
         * @param look_x Look X
         * @param look_y Look Y
         * @param look_z Look Z
         * @param up_x Up X
         * @param up_y Up Y
         * @param up_z Up Z
         */
        void set(float pos_x, float pos_y, float pos_z,
                float look_x, float look_y, float look_z,
                float up_x, float up_y, float up_z);

        /**
         * Slides (translates) the camera over it's own coordinate system
         *
         * @param du Translation in u
         * @param dv Translation in v
         * @param dn Translation in n
         */
        void slide(float du, float dv, float dn);

        /**
         * Slides (translates) the camera over it's own coordinate system
         * around a given model object
         *
         * @param model Model object
         * @param du    Translation in u
         * @param dv    Translation in v
         * @param dn    Translation in n
         */
        void slide_around_model(Model *m, float du, float dv, float dn);

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

        /**
         * Defines the camera's position and look vector
         * so that the model fits the screen
         *
         * @param m Model object
         */
        void look_at_model(Model *m);

        /**
         * Horizontal FOV angle
         */
        float hFov;

        /*
         * Vertical FOV angle
         */
        float vFov;

        vector3f u;
        vector3f v;
        vector3f n;

        vector3f position;
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

void Camera::set(float pos_x, float pos_y, float pos_z,
        float look_x, float look_y, float look_z,
        float up_x, float up_y, float up_z)
{
    position.set(pos_x, pos_y, pos_z);

    n.set(pos_x - look_x, pos_y - look_y, pos_z - look_z);

    vector3f up(up_x, up_y, up_z);
    u = crossProduct(up, n);

    n.normalize();
    u.normalize();

    v = crossProduct(n, u);
}

void Camera::slide(float du, float dv, float dn)
{
    position.x += du * u.x + dv * v.x + dn * n.x;
    position.y += du * u.y + dv * v.y + dn * n.y;
    position.z += du * u.z + dv * v.z + dn * n.z;
}

void Camera::slide_around_model(Model *m, float du, float dv, float dn)
{
    slide(du, dv, dn);
    set(position.x, position.y, position.z,
            m->center->x, m->center->y, m->center->z,
            v.x, v.y, v.z);
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

void Camera::look_at_model(Model *m)
{
    // calculate z distance so that the model fits the screen
    float z = m->bbox.z_max;
    z += max(abs(m->bbox.x_max - m->bbox.x_min), abs(m->bbox.y_max - m->bbox.y_min));

    set(m->center->x, m->center->y, z,
            m->center->x, m->center->y, m->center->z,
            0, 1, 0);
}

#endif
