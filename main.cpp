#include <main.h>

void opengl_render()
{
    glutSetWindow(ui_win[WIN_OPENGL]);

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (options.lighting) {
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
        glLightfv(GL_LIGHT0, GL_POSITION, light_position);
        glEnable(GL_COLOR_MATERIAL);
    } else {
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
        glDisable(GL_COLOR_MATERIAL);
    }

    if (options.ccw) {
        glFrontFace(GL_CCW);
    } else {
        glFrontFace(GL_CW);
    }

    if (options.backface_culling) {
        glEnable(GL_CULL_FACE);
    } else {
        glDisable(GL_CULL_FACE);
    }

    switch (options.primitives) {
        case 0:
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            break;
        case 1:
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            break;
        case 2:
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            break;
    }

    if (m != NULL) {
        gluLookAt(camera->position.x, camera->position.y, camera->position.z,
                camera->position.x - camera->n.x, camera->position.y - camera->n.y, camera->position.z - camera->n.z,
                camera->v.x, camera->v.y, camera->v.z);
        glColor3f(options.r, options.g, options.b);
        opengl_draw_model(m);
    }

    glutSwapBuffers();
}

void opengl_reshape(int w, int h)
{
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float scale = camera->vFov / camera->hFov;
    gluPerspective(camera->vFov, scale, options.near, options.far);
    glMatrixMode(GL_MODELVIEW);
}

void opengl_draw_model(Model *m)
{
    glBegin(GL_TRIANGLES);
    if (options.smooth_shading) {
        for (int i = 0; i < m->triangles_count; i++) {
            glNormal3f(m->triangles[i].normal[0].x, m->triangles[i].normal[0].y, -m->triangles[i].normal[0].z);
            glVertex3f(m->triangles[i].v0.x, m->triangles[i].v0.y, m->triangles[i].v0.z);
            glNormal3f(m->triangles[i].normal[1].x, m->triangles[i].normal[1].y, -m->triangles[i].normal[1].z);
            glVertex3f(m->triangles[i].v1.x, m->triangles[i].v1.y, m->triangles[i].v1.z);
            glNormal3f(m->triangles[i].normal[2].x, m->triangles[i].normal[2].y, -m->triangles[i].normal[2].z);
            glVertex3f(m->triangles[i].v2.x, m->triangles[i].v2.y, m->triangles[i].v2.z);
        }
    } else {
        for (int i = 0; i < m->triangles_count; i++) {
            glNormal3f(m->triangles[i].face_normal.x, m->triangles[i].face_normal.y, -m->triangles[i].face_normal.z);
            glVertex3f(m->triangles[i].v0.x, m->triangles[i].v0.y, m->triangles[i].v0.z);
            glVertex3f(m->triangles[i].v1.x, m->triangles[i].v1.y, m->triangles[i].v1.z);
            glVertex3f(m->triangles[i].v2.x, m->triangles[i].v2.y, m->triangles[i].v2.z);
        }
    }
    glEnd();
}




void close2gl_render()
{
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    close2gl_set_modelview();

    if (m != NULL) {
        close2gl_draw_model(m);
    }

    glutSwapBuffers();
}

void close2gl_reshape(int w, int h)
{
    close2gl_w = w;
    close2gl_h = h;
    close2gl_reset_buffers();

    glViewport(0, 0, (GLsizei) close2gl_w, (GLsizei) close2gl_h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, close2gl_w, 0, close2gl_h);

    glMatrixMode(GL_MODELVIEW);
    float scale = camera->vFov / camera->hFov;
    close2gl_set_projection(scale, options.near, options.far);
    close2gl_set_viewport(0, close2gl_w, 0, close2gl_h);
}

void close2gl_draw_model(Model *m)
{
    float *v0 = new float[4];
    float *v1 = new float[4];
    float *v2 = new float[4];
    for (int i = 0; i < m->triangles_count; i++) {
        // backface culling
        if (options.backface_culling) {
            vector3f a = m->triangles[i].v0 - m->triangles[i].v1;
            vector3f b = m->triangles[i].v0 - m->triangles[i].v2;
            vector3f normal = crossProduct(a, b);
            bool cull = dotProduct(-camera->n, normal) >= 0;
            if (!options.ccw) cull = !cull;
            if (cull) continue;
        }

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
        v0 = *close2gl_modelview * v0;
        v1 = *close2gl_modelview * v1;
        v2 = *close2gl_modelview * v2;

        // projection transformation
        v0 = *close2gl_projection * v0;
        v1 = *close2gl_projection * v1;
        v2 = *close2gl_projection * v2;

        // clipping
        bool clip = abs(v0[0]) <= abs(v0[3]) && abs(v0[1]) <= abs(v0[3]) && abs(v0[2]) <= abs(v0[3]) &&
                abs(v1[0]) <= abs(v1[3]) && abs(v1[1]) <= abs(v1[3]) && abs(v1[2]) <= abs(v1[3]) &&
                abs(v2[0]) <= abs(v2[3]) && abs(v2[1]) <= abs(v2[3]) && abs(v2[2]) <= abs(v2[3]);
        if (!clip) continue;

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
        v0 = *close2gl_viewport * v0;
        v1 = *close2gl_viewport * v1;
        v2 = *close2gl_viewport * v2;

        // drawing
        close2gl_raster(v0, v1, v2);
    }

    glRasterPos2i(0, 0);
    glDrawPixels(close2gl_w, close2gl_h, GL_RGBA, GL_FLOAT, close2gl_color_buffer);

    delete [] v0;
    delete [] v1;
    delete [] v2;
}

void close2gl_raster(float *v0, float *v1, float *v2)
{
    switch (options.primitives) {
        case 0:
            close2gl_raster_point((int) v0[0], (int) v0[1], (int) v0[2]);
            close2gl_raster_point((int) v1[0], (int) v1[1], (int) v1[2]);
            close2gl_raster_point((int) v2[0], (int) v2[1], (int) v2[2]);
            break;
        case 1:
            close2gl_raster_line((int) v0[0], (int) v0[1], (int) v0[2],
                    (int) v1[0], (int) v1[1], (int) v1[2]);
            close2gl_raster_line((int) v0[0], (int) v0[1], (int) v0[2],
                    (int) v2[0], (int) v2[1], (int) v2[2]);
            close2gl_raster_line((int) v1[0], (int) v1[1], (int) v1[2],
                    (int) v2[0], (int) v2[1], (int) v2[2]);
            break;
    }
}

void close2gl_raster_point(int x, int y, int z)
{
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x >= close2gl_w) x = close2gl_w - 1;
    if (y >= close2gl_h) x = close2gl_h - 1;
    int pos = 4 * (x + y * close2gl_w);
    if (z < close2gl_depth_buffer[pos / 4]) {
        close2gl_depth_buffer[pos / 4] = z;
        close2gl_color_buffer[pos] = options.r;
        close2gl_color_buffer[pos + 1] = options.g;
        close2gl_color_buffer[pos + 2] = options.b;
        close2gl_color_buffer[pos + 3] = 1.0;
    }
}

void close2gl_raster_line(int x0, int y0, int z0,
        int x1, int y1, int z1)
{
    int dx = x1 - x0;
    int dy = y1 - y0;
    int dz = z1 - z0;

    int points = max(abs(dx), abs(dy));

    float inc_x = dx / (float) points;
    float inc_y = dy / (float) points;
    float inc_z = dz / (float) points;

    close2gl_raster_point(x0, y0, z0);
    for (int i = 0; i < points; i++) {
        close2gl_raster_point(x0 + inc_x * i, y0 + inc_y * i, z0 + inc_z * i);
    }
}

void close2gl_reset_buffers()
{
    if (close2gl_depth_buffer != NULL) {
        delete [] close2gl_depth_buffer;
    }
    if (close2gl_color_buffer != NULL) {
        delete [] close2gl_color_buffer;
    }
    close2gl_depth_buffer = new float[close2gl_w * close2gl_h];
    for (int i = 0; i < close2gl_w * close2gl_h; i++) close2gl_depth_buffer[i] = INT_MAX;
    close2gl_color_buffer = new float[close2gl_w * close2gl_h * 4];
    memset(close2gl_color_buffer, 0.0, close2gl_w * close2gl_h * 4 * sizeof(float));
}

void close2gl_set_modelview()
{
    close2gl_modelview = new matrix4x4f(
            camera->u.x, camera->u.y, camera->u.z, -dotProduct(camera->position, camera->u),
            camera->v.x, camera->v.y, camera->v.z, -dotProduct(camera->position, camera->v),
            camera->n.x, camera->n.y, camera->n.z, -dotProduct(camera->position, camera->n),
            0.0,         0.0,         0.0,         1.0);
}

void close2gl_set_projection(float a, float n, float f)
{
    float t = n * tan((camera->vFov / 2.0) * (3.141592654 / 180.0));
    float r = t * a;
    float b = -t;
    float l = -r;

    float w = r - l;
    float h = t - b;
    float d = f - n;

    close2gl_projection = new matrix4x4f(
            (2.0 * n) / w, 0.0,           (r + l) / w,  0.0,
            0.0,           (2.0 * n) / h, (t + b) / h,  0.0,
            0.0,           0.0,           -(f + n) / d, -(2.0 * f * n) / d,
            0.0,           0.0,           -1.0,         0.0);
}

void close2gl_set_viewport(float lv, float rv, float bv, float tv)
{
    close2gl_viewport = new matrix4x4f(
            (rv - lv) / 2.0, 0.0,             0.0, (lv + rv) / 2.0,
            0.0,             (tv - bv) / 2.0, 0.0, (bv + tv) / 2.0,
            0.0,             0.0,             1.0, 0.0,
            0.0,             0.0,             0.0, 1.0);
}




void ui_create()
{
    GLUI *glui = GLUI_Master.create_glui("Options", false, DEFAULT_W * 2, 0);
    glui->set_main_gfx_window(ui_win[WIN_OPENGL]);

    GLUI_Panel *primitives_panel = glui->add_panel("Primitives");
    GLUI_RadioGroup *primitives = glui->add_radiogroup_to_panel(primitives_panel, &options.primitives, 0, ui_callback);
    glui->add_radiobutton_to_group(primitives, "Points");
    glui->add_radiobutton_to_group(primitives, "Wireframe");
    glui->add_radiobutton_to_group(primitives, "Solid");

    glui->add_checkbox("Lighting", &options.lighting, 0, ui_callback);
    glui->add_checkbox("Smooth Shading", &options.smooth_shading, 0, ui_callback);
    glui->add_checkbox("CCW", &options.ccw, 0, ui_callback);
    glui->add_checkbox("Backface Culling", &options.backface_culling, 0, ui_callback);

    GLUI_Panel *camera_panel = glui->add_panel("Camera");
    glui->add_checkbox_to_panel(camera_panel, "Centered on Object", &options.camera_centered, 0, ui_callback);
    glui->add_button_to_panel(camera_panel, "Reset", UI_RESET_CAMERA, ui_callback);

    glui->add_separator();
    GLUI_Panel *clipping_panel = glui->add_panel("Clipping Planes");
    glui->add_spinner_to_panel(clipping_panel, "Near:", GLUI_SPINNER_FLOAT, &options.near, 0, ui_callback);
    glui->add_spinner_to_panel(clipping_panel, "Far:", GLUI_SPINNER_FLOAT, &options.far, 0, ui_callback);


    glui->add_separator();
    glui->add_statictext("Model Colors");
    GLUI_Spinner *r = glui->add_spinner("R:", GLUI_SPINNER_FLOAT, &options.r, 0, ui_callback);
    r->set_float_limits(0, 1, GLUI_LIMIT_WRAP);
    GLUI_Spinner *g = glui->add_spinner("G:", GLUI_SPINNER_FLOAT, &options.g, 0, ui_callback);
    g->set_float_limits(0, 1, GLUI_LIMIT_WRAP);
    GLUI_Spinner *b = glui->add_spinner("B:", GLUI_SPINNER_FLOAT, &options.b, 0, ui_callback);
    b->set_float_limits(0, 1, GLUI_LIMIT_WRAP);

    glui->add_separator();
    glui->add_statictext("Model File");
    ui_file = glui->add_edittext("Path:", GLUI_EDITTEXT_TEXT, NULL, UI_READ_FILE, ui_callback);
    ui_file->set_text("models/cubo_tex.in");
}

void ui_callback(int action)
{
    switch (action) {
        case UI_RESET_CAMERA:
            options.near = 1.0;
            options.far = 3000.0;
            camera->reset();
            if (m != NULL) camera->look_at_model(m);
            break;
        case UI_READ_FILE:
            m = new Model(ui_file->get_text());
            camera->look_at_model(m);
            break;
        default:
            break;
    }

    glutSetWindow(ui_win[WIN_OPENGL]);
    opengl_reshape(opengl_w, opengl_h);
    glutPostRedisplay();

    glutSetWindow(ui_win[WIN_CLOSE2GL]);
    close2gl_reshape(close2gl_w, close2gl_h);
    glutPostRedisplay();
}

void ui_keyboard(unsigned char key, int x, int y)
{
    if (options.camera_centered) {
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
    ui_callback(0);
}



int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowPosition(0, 0);
    glutInitWindowSize(DEFAULT_W, DEFAULT_H);
    ui_win[WIN_OPENGL] = glutCreateWindow("OpenGL");
    glutDisplayFunc(opengl_render);
    glutReshapeFunc(opengl_reshape);
    glutKeyboardFunc(ui_keyboard);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glEnable(GL_DEPTH_TEST);
    glClearColor(BG_COLOR_R, BG_COLOR_G, BG_COLOR_B, BG_COLOR_A);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition(DEFAULT_W, 0);
    glutInitWindowSize(DEFAULT_W, DEFAULT_H);
    ui_win[WIN_CLOSE2GL] = glutCreateWindow("Close2GL");
    glutDisplayFunc(close2gl_render);
    glutReshapeFunc(close2gl_reshape);
    glutKeyboardFunc(ui_keyboard);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_ALPHA);
    glClearColor(BG_COLOR_R, BG_COLOR_G, BG_COLOR_B, BG_COLOR_A);

    close2gl_w = DEFAULT_W;
    close2gl_h = DEFAULT_H;
    opengl_w = DEFAULT_W;
    opengl_h = DEFAULT_H;

    options.r = 1.0;
    options.g = 0.0;
    options.b = 0.0;
    options.near = 1.0;
    options.far = 3000.0;

    ui_create();

    camera = new Camera();

    glutMainLoop();
    return 0;
}
