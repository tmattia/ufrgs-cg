#include <iostream>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include <GLUI/glui.h>
#include <vector3f.h>

#define MAX_TRIANGLES 100000

#define OPENGL_WINDOW   0
#define CLOSE2GL_WINDOW 1

using namespace std;

/**
 * GUI options
 */
struct options {
    int lighting;
    int ccw;
    int backface_culling;
    int primitives;
    float r;
    float g;
    float b;
    int camera_centered;
};

/*
 * Triangle structure (read from model file)
 */
struct triangle {
    vector3f v0;
    vector3f v1;
    vector3f v2;
    vector3f normal[3];
    vector3f face_normal;
    char color[3];
};

/**
 * Model's bounding box
 */
struct bounding_box {
    float x_min;
    float x_max;
    float y_min;
    float y_max;
    float z_min;
    float z_max;
};


class Model
{
    public:
        Model(const char *path);
        ~Model();

        /**
         * Holds the coordinates of the object's bounding box
         */
        vector3f *center;

        /**
         * Object's bounding box
         */
        bounding_box bbox;

        /**
         * Total number of triangles in the object
         */
        int triangles_count;

        /**
         * All the triangles which compose the object
         */
        triangle triangles[MAX_TRIANGLES];

    private:
        /**
         * Reads a model file from the filesystem. Stores the model as
         * an array of triangles and a bounding box
         *
         * @param path Full path to the file
         */
        void read_file(const char *path);

        /**
         * Calculates the center of the object's bounding box
         */
        void find_center();

};

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


/*
 * Window ids
 */
int win_id[2];

/*
 * Model file text field
 */
GLUI_EditText *file;

/*
 * Near clipping plane text field
 */
GLUI_EditText *near;

/*
 * Far clipping plane text field
 */
GLUI_EditText *far;

/*
 * Viewing Options
 */
options opt;

/*
 * Model (read from file)
 */
Model *m;

/*
 * Camera
 */
Camera *camera;

/**
 * GUI callback for resetting the camera
 */
void camera_reset(int id);

/**
 * File text field callback handler
 *
 * Creates a new model and stores it in the model global variable
 */
void read_file(int i);

/**
 * Draws a given model
 *
 * @param m The model
 */
void draw_model(Model *m);

/**
 * Keyboard function
 *
 * A: slides camera to the left (decreases x in  CCS)
 * S: slides camera down (decrease y in CCS)
 * D: slides camera to the right (increase x in CCS)
 * W: slides camera up (increase y in CCS)
 * Q: zoom out (increase z in CCS)
 * E: zoom in (decrease z in CCS)
 * z: rotate cw around y (in CCS)
 * Z: rotate ccw around y (in CCS)
 * x: rotate cw around x (in CCS)
 * X: rotate ccw around x (in CCS)
 * c: rotate cw around z (in CCS)
 * C: rotate ccw around z (in CCS)
 */
void keyboard(unsigned char key, int x, int y);

/**
 * GLUT's idle function
 */
void idle();

/**
 * Calls all the OpenGL rendering options defined by the GUI
 */
void set_rendering_options();

/**
 * Sets the ModelView matrix for Close2GL
 */
void set_modelview_matrix();

/**
 * OpenGL render function
 */
void renderOpenGL();

/**
 * OpenGL reshape function
 */
void reshapeOpenGL();

/**
 * Close2GL render function
 */
void renderClose2GL();

/**
 * Close2GL reshape function
 */
void reshapeClose2GL();

int main(int argc, char *argv[]);
