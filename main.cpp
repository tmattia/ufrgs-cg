#include <main.h>

// -------------------- GUI GLOBALS --------------------

/*
 * Main window id
 */
int window_id;

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

// /GUI GLOBALS ----------------------------------------


// CAMERA ------------------------------------------------------
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
// /CAMERA ------------------------------------------------------


// GLUT ---------------------------------------------------------

void render()
{

}

// /GLUT --------------------------------------------------------

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_ALPHA);

    glutInitWindowPosition(0, 0);
    glutInitWindowSize(640, 480);
    window_id = glutCreateWindow("INF01009 - Trabalho de Implementação 2");

    glutDisplayFunc(render);

    // create GUI
    GLUI *glui = GLUI_Master.create_glui("Options", false, 640, 0);
    glui->set_main_gfx_window(window_id);

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
    glui->add_button_to_panel(camera_panel, "Reset");

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
    file = glui->add_edittext("Path:", GLUI_EDITTEXT_TEXT);

    glClearColor(0.35f, 0.53f, 0.7f, 1.0f);
    glutMainLoop();
    return 0;
}
