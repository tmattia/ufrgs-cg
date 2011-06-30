#ifndef _MAIN_H_
#define _MAIN_H_

#include <iostream>
#include <float.h>

#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include <GLUI/glui.h>

#include <vector3f.h>
#include <matrix4x4f.h>
#include <model.h>
#include <camera.h>
#include <texture_manager.h>

using namespace std;

#define WIN_OPENGL 0
#define WIN_CLOSE2GL 1

#define UI_RESET_CAMERA 1
#define UI_READ_FILE 2
#define UI_SET_LIGHTING 3

#define BG_COLOR_R 0.0f
#define BG_COLOR_G 0.0f
#define BG_COLOR_B 0.0f
#define BG_COLOR_A 1.0f

#define DEFAULT_W 500
#define DEFAULT_H 500

enum {
    TEX_MANDRILL,
    TEX_CHECKERS
};

Model *m;
Camera *camera;

int opengl_w;
int opengl_h;
void opengl_render();
void opengl_reshape(int w, int h);
void opengl_draw_model(Model *m);

int close2gl_w;
int close2gl_h;
float *close2gl_depth_buffer;
float *close2gl_color_buffer;
matrix4x4f *close2gl_modelview;
matrix4x4f *close2gl_projection;
matrix4x4f *close2gl_viewport;
void close2gl_render();
void close2gl_reshape(int w, int h);
void close2gl_draw_model(Model *m);

void close2gl_raster(float *v0, float *v1, float *v2);
void close2gl_raster_point(int x, int y, int z);
void close2gl_raster_line(int x0, int y0, int z0, int x1, int y1, int z1);

void close2gl_reset_buffers();
void close2gl_set_modelview();
void close2gl_set_projection(float a, float n, float f);
void close2gl_set_viewport(float lv, float rv, float bv, float tv);

struct options {
    int lighting;
    int ccw;
    int backface_culling;
    int primitives;
    float r;
    float g;
    float b;
    int camera_centered;
    float near;
    float far;
    int smooth_shading;
    int textures_on;
    int texture;
} options;
int ui_win[2];
void ui_create();
void ui_callback(int action);
void ui_keyboard(unsigned char key, int x, int y);
GLUI_EditText *ui_file;

GLfloat light_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
GLfloat light_diffuse[] = { 0.6, 0.6, 0.6, 1.0 };
GLfloat light_specular[] = { 0.4, 0.4, 0.4, 1.0 };
GLfloat light_position[] = { 1000, 1000, 1000, 1.0 };

int main(int argc, char **argv);

#endif
