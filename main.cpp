#include <main.h>

void read_file(int i)
{
    // create new model from given file
    m = new Model(file->get_text());
    camera->look_at_model(m);
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

void camera_reset(int id)
{
    if (m == NULL) {
        camera->reset();
    } else {
        camera->look_at_model(m);
    }
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
    }
}

void idle()
{
    glutSetWindow(win_id[OPENGL_WINDOW]);
    glutPostRedisplay();
    glutSetWindow(win_id[CLOSE2GL_WINDOW]);
    glutPostRedisplay();
}

void set_modelview_matrix()
{
    float m[16];
    m[0] = camera->u.x; m[4] = camera->u.y; m[8]  = camera->u.z; m[12] = -dotProduct(camera->position, camera->u);
    m[1] = camera->v.x; m[5] = camera->v.y; m[9]  = camera->v.z; m[13] = -dotProduct(camera->position, camera->v);
    m[2] = camera->n.x; m[6] = camera->n.y; m[10] = camera->n.z; m[14] = -dotProduct(camera->position, camera->n);
    m[3] = 0;           m[7] = 0;           m[11] = 0;           m[15] = 1.0;
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glLoadMatrixf(m);
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
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(camera->vFov, 320.0 / 240.0, atof(near->get_text()), atof(far->get_text()));

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    set_rendering_options();

    if (m != NULL) {
        gluLookAt(camera->position.x, camera->position.y, camera->position.z,
                camera->position.x - camera->n.x, camera->position.y - camera->n.y, camera->position.z - camera->n.z,
                camera->v.x, camera->v.y, camera->v.z);
                //0, 1, 0);
        draw_model(m);
    }

    glutSwapBuffers();
}

void reshapeOpenGL(int w, int h)
{
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float scale = w > h ? (float) w / (float) h : (float) h / (float) w;
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
        draw_model(m);
    }

    glutSwapBuffers();
}

void reshapeClose2GL(int w, int h)
{
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float scale = w > h ? (float) w / (float) h : (float) h / (float) w;
    gluPerspective(camera->vFov, scale, atof(near->get_text()), atof(far->get_text()));

    set_modelview_matrix();
}

// /GLUT -------------------------------------------------------


// GUI ---------------------------------------------------------

void create_gui()
{
    GLUI *glui = GLUI_Master.create_glui("Options", false, 1000, 0);
    glui->set_main_gfx_window(win_id[OPENGL_WINDOW]);
    GLUI_Master.set_glutIdleFunc(idle);

    // primitives
    GLUI_Panel *primitives_panel = glui->add_panel("Primitives");
    GLUI_RadioGroup *primitives = glui->add_radiogroup_to_panel(primitives_panel, &opt.primitives);
    glui->add_radiobutton_to_group(primitives, "Points");
    glui->add_radiobutton_to_group(primitives, "Wireframe");
    glui->add_radiobutton_to_group(primitives, "Solid");

    // lighting
    glui->add_checkbox("Lighting", &opt.lighting);

    // backface orientation
    glui->add_checkbox("CCW", &opt.ccw);

    // backface culling
    glui->add_checkbox("Backface Culling", &opt.backface_culling);

    // reset camera
    GLUI_Panel *camera_panel = glui->add_panel("Camera");
    glui->add_checkbox_to_panel(camera_panel, "Centered on Object", &opt.camera_centered);
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
    GLUI_Spinner *r = glui->add_spinner("R:", GLUI_SPINNER_FLOAT, &opt.r);
    r->set_float_limits(0, 1, GLUI_LIMIT_WRAP);
    GLUI_Spinner *g = glui->add_spinner("G:", GLUI_SPINNER_FLOAT, &opt.g);
    g->set_float_limits(0, 1, GLUI_LIMIT_WRAP);
    GLUI_Spinner *b = glui->add_spinner("B:", GLUI_SPINNER_FLOAT, &opt.b);
    b->set_float_limits(0, 1, GLUI_LIMIT_WRAP);

    // read model file
    glui->add_separator();
    glui->add_statictext("Model File");
    file = glui->add_edittext("Path:", GLUI_EDITTEXT_TEXT, NULL, 0, read_file);
    file->set_text("/Users/tomasmattia/Dropbox/ufrgs/cg/trabalhos/3/vaca.in");
}


int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    // initialize OpenGL window
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(500, 375);
    win_id[OPENGL_WINDOW] = glutCreateWindow("OpenGL");
    glutDisplayFunc(renderOpenGL);
    glutReshapeFunc(reshapeOpenGL);
    glutKeyboardFunc(keyboard);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.35f, 0.53f, 0.7f, 1.0f);

    // initialize Close2GL window
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition(500, 0);
    glutInitWindowSize(500, 375);
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

    // GLUT main loop
    glutMainLoop();
    return 0;
}
