#include <main.h>

void read_file(int i)
{
    // create new model from given file
    m = new Model(file->get_text());
    camera->look_at_model(m);
    idle(0);
}

void draw_model(Model *m)
{
    for (int i = 0; i < m->triangles_count; i++) {
        glBegin(GL_TRIANGLES);
            glNormal3f(m->triangles[i].normal[0].x, m->triangles[i].normal[0].y, -m->triangles[i].normal[0].z);
            glVertex3f(m->triangles[i].v0.x, m->triangles[i].v0.y, m->triangles[i].v0.z);
            glNormal3f(m->triangles[i].normal[1].x, m->triangles[i].normal[1].y, -m->triangles[i].normal[1].z);
            glVertex3f(m->triangles[i].v1.x, m->triangles[i].v1.y, m->triangles[i].v1.z);
            glNormal3f(m->triangles[i].normal[2].x, m->triangles[i].normal[2].y, -m->triangles[i].normal[2].z);
            glVertex3f(m->triangles[i].v2.x, m->triangles[i].v2.y, m->triangles[i].v2.z);
        glEnd();
    }
}

void draw_model_close2gl(Model *m)
{
    clear_buffers(close2gl_window_width, close2gl_window_height);

    float *v0 = new float[4];
    float *v1 = new float[4];
    float *v2 = new float[4];
    for (int i = 0; i < m->triangles_count; i++) {
        // transform points to homogeneous coordinates
        v0[0] = m->triangles[i].v0.x;
        v0[1] = m->triangles[i].v0.y;
        v0[2] = m->triangles[i].v0.z;
        v0[3] = 1;

        v1[0] = m->triangles[i].v1.x;
        v1[1] = m->triangles[i].v1.y;
        v1[2] = m->triangles[i].v1.z;
        v1[3] = 1;

        v2[0] = m->triangles[i].v2.x;
        v2[1] = m->triangles[i].v2.y;
        v2[2] = m->triangles[i].v2.z;
        v2[3] = 1;


        // modelview transformation
        v0 = *modelview * v0;
        v1 = *modelview * v1;
        v2 = *modelview * v2;

        // projection transformation
        v0 = *projection * v0;
        v1 = *projection * v1;
        v2 = *projection * v2;

        // backface culling
        bool cull = false;
        if (opt.backface_culling) {
            cull = dotProduct(camera->n, m->triangles[i].face_normal) >= 0;
            if (opt.ccw) cull = !cull;
        }

        // clipping
        if (!cull &&
                abs(v0[0]) <= abs(v0[3]) && abs(v0[1]) <= abs(v0[3]) && abs(v0[2]) <= abs(v0[3]) &&
                abs(v1[0]) <= abs(v1[3]) && abs(v1[1]) <= abs(v1[3]) && abs(v1[2]) <= abs(v1[3]) &&
                abs(v2[0]) <= abs(v2[3]) && abs(v2[1]) <= abs(v2[3]) && abs(v2[2]) <= abs(v2[3])) {
            // perspective division
            v0[0] = v0[0] / v0[3];
            v0[1] = v0[1] / v0[3];
            v0[2] = v0[2] / v0[3];
            v0[3] = v0[3] / v0[3];

            v1[0] = v1[0] / v1[3];
            v1[1] = v1[1] / v1[3];
            v1[2] = v1[2] / v1[3];
            v1[3] = v1[3] / v1[3];

            v2[0] = v2[0] / v2[3];
            v2[1] = v2[1] / v2[3];
            v2[2] = v2[2] / v2[3];
            v2[3] = v2[3] / v2[3];

            // viewport transformation
            v0 = *viewport * v0;
            v1 = *viewport * v1;
            v2 = *viewport * v2;

            // drawing
            draw_triangle(v0, v1, v2);
        }
    }

    glRasterPos2i(0, 0);
    float color_buffer_pixels[close2gl_window_height][close2gl_window_width][4];
    for (int i = 0; i < close2gl_window_width; i++) {
        for (int j = 0; j < close2gl_window_height; j++) {
            for (int k = 0; k < 4; k++) {
                color_buffer_pixels[j][i][k] = color_buffer[i][j][k];
            }
        }
    }
    glDrawPixels(close2gl_window_width, close2gl_window_height, GL_RGBA, GL_FLOAT, color_buffer_pixels);
}

void draw_triangle(float* v0, float* v1, float* v2)
{
    if (v0[2] < depth_buffer[v0[0]][v0[1]]) {
        depth_buffer[v0[0]][v0[1]] = v0[2];
        color_buffer[v0[0]][v0[1]][0] = opt.r;
        color_buffer[v0[0]][v0[1]][1] = opt.g;
        color_buffer[v0[0]][v0[1]][2] = opt.b;
        color_buffer[v0[0]][v0[1]][3] = 1;
    }

    if (v1[2] < depth_buffer[v1[0]][v1[1]]) {
        depth_buffer[v1[0]][v1[1]] = v1[2];
        color_buffer[v1[0]][v1[1]][0] = opt.r;
        color_buffer[v1[0]][v1[1]][1] = opt.g;
        color_buffer[v1[0]][v1[1]][2] = opt.b;
        color_buffer[v1[0]][v1[1]][3] = 1;
    }

    if (v2[2] < depth_buffer[v2[0]][v2[1]]) {
        depth_buffer[v2[0]][v2[1]] = v2[2];
        color_buffer[v2[0]][v2[1]][0] = opt.r;
        color_buffer[v2[0]][v2[1]][1] = opt.g;
        color_buffer[v2[0]][v2[1]][2] = opt.b;
        color_buffer[v2[0]][v2[1]][3] = 1;
    }
}

void camera_reset(int id)
{
    camera->reset();
    if (m != NULL) {
        camera->look_at_model(m);
    }
    idle(0);
}

void keyboard(unsigned char key, int x, int y)
{
    if (opt.camera_centered) {
        switch (key) {
            case 'a': camera->slide_around_model(m, -50, 0, 0); break;
            case 's': camera->slide_around_model(m, 0, -50, 0); break;
            case 'd': camera->slide_around_model(m, 50, 0, 0); break;
            case 'w': camera->slide_around_model(m, 0, 50, 0); break;
            case 'q': camera->slide_around_model(m, 0, 0, -50); break;
            case 'e': camera->slide_around_model(m, 0, 0, 50); break;
        }
    } else {
        switch (key) {
            case 'a': camera->slide(-50, 0, 0); break;
            case 's': camera->slide(0, -50, 0); break;
            case 'd': camera->slide(50, 0, 0); break;
            case 'w': camera->slide(0, 50, 0); break;
            case 'q': camera->slide(0, 0, -50); break;
            case 'e': camera->slide(0, 0, 50); break;
        }
    }

    switch (key) {
        case 'z': camera->yaw(15); break;
        case 'Z': camera->yaw(-15); break;
        case 'x': camera->pitch(15); break;
        case 'X': camera->pitch(-15); break;
        case 'c': camera->roll(15); break;
        case 'C': camera->roll(-15); break;
        case 'f': camera->vFov += 5; break;
        case 'F': camera->vFov -= 5; break;
        case 'v': camera->hFov += 5; break;
        case 'V': camera->hFov -= 5; break;
    }

    idle(0);
}

void idle(int id)
{
    float scale = camera->vFov / camera->hFov;

    // OpenGL
    glutSetWindow(win_id[OPENGL_WINDOW]);
    // TODO refactor duplicated code
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(camera->vFov, scale, atof(near->get_text()), atof(far->get_text()));
    glMatrixMode(GL_MODELVIEW);
    glutPostRedisplay();

    // Close2GL
    glutSetWindow(win_id[CLOSE2GL_WINDOW]);
    // TODO refactor duplicated code
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, close2gl_window_width, 0, close2gl_window_height);
    glMatrixMode(GL_MODELVIEW);
    set_projection_matrix(scale, atof(near->get_text()), atof(far->get_text()));
    set_viewport_matrix(0, close2gl_window_width, 0, close2gl_window_height);
    glutPostRedisplay();
}

void set_modelview_matrix()
{
    modelview = new matrix4x4f(
            camera->u.x, camera->u.y, camera->u.z, -dotProduct(camera->position, camera->u),
            camera->v.x, camera->v.y, camera->v.z, -dotProduct(camera->position, camera->v),
            camera->n.x, camera->n.y, camera->n.z, -dotProduct(camera->position, camera->n),
            0.0,         0.0,         0.0,         1.0);
}

void set_projection_matrix(float a, float n, float f)
{
    float t = n * tan((camera->vFov / 2.0) * (3.141592654 / 180.0));
    float r = t * a;
    float b = -t;
    float l = -r;

    float w = r - l;
    float h = t - b;
    float d = f - n;

    projection = new matrix4x4f(
            (2.0 * n) / w, 0.0,           (r + l) / w,  0.0,
            0.0,           (2.0 * n) / h, (t + b) / h,  0.0,
            0.0,           0.0,           -(f + n) / d, -(2.0 * f * n) / d,
            0.0,           0.0,           -1.0,         0.0);
}

void set_viewport_matrix(float lv, float rv, float bv, float tv)
{
    viewport = new matrix4x4f(
            (rv - lv) / 2.0, 0.0,             0.0, (lv + rv) / 2.0,
            0.0,             (tv - bv) / 2.0, 0.0, (bv + tv) / 2.0,
            0.0,             0.0,             1.0, 0.0,
            0.0,             0.0,             0.0, 1.0);
}

void set_rendering_options()
{
    // set up lighting
    if (opt.lighting) {
        GLfloat ambient[] = { 0.2, 0.2, 0.2, 1.0 };
        GLfloat diffuse[] = { 0.7, 0.7, 0.7, 1.0 };
        GLfloat position[] = { 10000, 10000, 10000, 0 }; // TODO refactor hard-coded values

        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
        glLightfv(GL_LIGHT0, GL_POSITION, position);

        float color[] = {opt.r, opt.g, opt.b};
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, color);
    } else {
        glDisable(GL_LIGHTING);
        glColor3f(opt.r, opt.g, opt.b);
    }

    // set up front face orientation
    if (opt.ccw) {
        glFrontFace(GL_CCW);
    } else {
        glFrontFace(GL_CW);
    }

    // set up backface culling
    if (opt.backface_culling) {
        glEnable(GL_CULL_FACE);
    } else {
        glDisable(GL_CULL_FACE);
    }

    // set up rendering primitives
    switch (opt.primitives) {
        case 0:
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            break;
        case 1:
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            break;
        case 2:
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            break;
        default: break;
    }
}

void renderOpenGL()
{
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    set_rendering_options();

    if (m != NULL) {
        gluLookAt(camera->position.x, camera->position.y, camera->position.z,
                camera->position.x - camera->n.x, camera->position.y - camera->n.y, camera->position.z - camera->n.z,
                camera->v.x, camera->v.y, camera->v.z);
        draw_model(m);
    }

    glutSwapBuffers();
}

void reshapeOpenGL(int w, int h)
{
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float scale = camera->vFov / camera->hFov;
    gluPerspective(camera->vFov, scale, atof(near->get_text()), atof(far->get_text()));
    glMatrixMode(GL_MODELVIEW);
}

void renderClose2GL()
{
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    set_modelview_matrix();

    set_rendering_options();

    if (m != NULL) {
        draw_model_close2gl(m);
    }

    glutSwapBuffers();
}

void reshapeClose2GL(int w, int h)
{
    close2gl_window_width = w;
    close2gl_window_height = h;

    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, close2gl_window_width, 0, close2gl_window_height);

    clear_buffers(w, h);

    glMatrixMode(GL_MODELVIEW);

    float scale = camera->vFov / camera->hFov;
    set_projection_matrix(scale, atof(near->get_text()), atof(far->get_text()));
    set_viewport_matrix(0, close2gl_window_width, 0, close2gl_window_height);
}

void create_gui()
{
    GLUI *glui = GLUI_Master.create_glui("Options", false, 1000, 0);
    glui->set_main_gfx_window(win_id[OPENGL_WINDOW]);

    // primitives
    GLUI_Panel *primitives_panel = glui->add_panel("Primitives");
    GLUI_RadioGroup *primitives = glui->add_radiogroup_to_panel(primitives_panel, &opt.primitives, 0, idle);
    glui->add_radiobutton_to_group(primitives, "Points");
    glui->add_radiobutton_to_group(primitives, "Wireframe");
    glui->add_radiobutton_to_group(primitives, "Solid");

    // lighting
    glui->add_checkbox("Lighting", &opt.lighting, 0, idle);

    // backface orientation
    glui->add_checkbox("CCW", &opt.ccw, 0, idle);

    // backface culling
    glui->add_checkbox("Backface Culling", &opt.backface_culling, 0, idle);

    // reset camera
    GLUI_Panel *camera_panel = glui->add_panel("Camera");
    glui->add_checkbox_to_panel(camera_panel, "Centered on Object", &opt.camera_centered, 0, idle);
    glui->add_button_to_panel(camera_panel, "Reset", 0, camera_reset);

    // near and far clipping planes
    glui->add_separator();
    glui->add_statictext("Clipping Planes");
    near = glui->add_edittext("Near:", GLUI_EDITTEXT_FLOAT);
    far = glui->add_edittext("Far:", GLUI_EDITTEXT_FLOAT);
    near->set_text("1.0");
    far->set_text("3000.0");

    // RGB colors of the models
    glui->add_separator();
    glui->add_statictext("Model Colors");
    GLUI_Spinner *r = glui->add_spinner("R:", GLUI_SPINNER_FLOAT, &opt.r, 0, idle);
    r->set_float_limits(0, 1, GLUI_LIMIT_WRAP);
    GLUI_Spinner *g = glui->add_spinner("G:", GLUI_SPINNER_FLOAT, &opt.g, 0, idle);
    g->set_float_limits(0, 1, GLUI_LIMIT_WRAP);
    GLUI_Spinner *b = glui->add_spinner("B:", GLUI_SPINNER_FLOAT, &opt.b, 0, idle);
    b->set_float_limits(0, 1, GLUI_LIMIT_WRAP);

    // read model file
    glui->add_separator();
    glui->add_statictext("Model File");
    file = glui->add_edittext("Path:", GLUI_EDITTEXT_TEXT, NULL, 0, read_file);
    file->set_text("/Users/tomasmattia/Dropbox/ufrgs/cg/trabalhos/3/vaca.in");
}

void clear_buffers(int w, int h)
{
    depth_buffer.clear();
    color_buffer.clear();

    for (int i = 0; i < w; i++) {
        depth_buffer.push_back(vector<float>());
        color_buffer.push_back(vector< vector<float> >());
        for (int j = 0; j < h; j++) {
            depth_buffer[i].push_back(INT_MAX);
            color_buffer[i].push_back(vector<float>());
            color_buffer[i][j].push_back(0.35);
            color_buffer[i][j].push_back(0.53);
            color_buffer[i][j].push_back(0.7);
            color_buffer[i][j].push_back(1);
        }
    }
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    // initialize OpenGL window
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(WINDOW_SIZE, WINDOW_SIZE);
    win_id[OPENGL_WINDOW] = glutCreateWindow("OpenGL");
    glutDisplayFunc(renderOpenGL);
    glutReshapeFunc(reshapeOpenGL);
    glutKeyboardFunc(keyboard);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.35f, 0.53f, 0.7f, 1.0f);

    // initialize Close2GL window
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition(WINDOW_SIZE, 0);
    glutInitWindowSize(WINDOW_SIZE, WINDOW_SIZE);
    win_id[CLOSE2GL_WINDOW] = glutCreateWindow("Close2GL");
    glutDisplayFunc(renderClose2GL);
    glutReshapeFunc(reshapeClose2GL);
    glutKeyboardFunc(keyboard);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.35f, 0.53f, 0.7f, 1.0f);

    // create GUI
    create_gui();

    // setup camera
    camera = new Camera();

    close2gl_window_width = WINDOW_SIZE;
    close2gl_window_height = WINDOW_SIZE;

    // GLUT main loop
    glutMainLoop();
    return 0;
}
