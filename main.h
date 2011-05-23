#include <iostream>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include <GLUI/glui.h>
#include <vector3f.h>
#include <model.h>
#include <camera.h>

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
