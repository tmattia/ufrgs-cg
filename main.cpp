#include <main.h>

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
// /CAMERA -----------------------------------------------------


// MODEL -------------------------------------------------------

Model::Model(const char *path)
{
    read_file(path);
}

void Model::read_file(const char *path)
{
    // open file
    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        cout << "ERROR: unable to read file " << path << endl;
        return;
    }

    char ch;
    fscanf(fp, "%c", &ch);
    while (ch != '\n') fscanf(fp, "%c", &ch); // skip the first line - object's name

    int material_count;
    fscanf(fp, "# triangles = %d\n", &triangles_count);
    fscanf(fp, "Material count = %d\n", &material_count);

    vector3f ambient[material_count],
             diffuse[material_count],
             specular[material_count];
    float shine[material_count];
    for (int i = 0; i < material_count; i++) {
        fscanf(fp, "ambient color %f %f %f\n",
                &(ambient[i].x), &(ambient[i].y), &(ambient[i].z));
        fscanf(fp, "diffuse color %f %f %f\n",
                &(diffuse[i].x), &(diffuse[i].y), &(diffuse[i].z));
        fscanf(fp, "specular color %f %f %f\n",
                &(specular[i].x), &(specular[i].y), &(specular[i].z));
        fscanf(fp, "material shine %f\n", &(shine[i]));
    }

    fscanf(fp, "%c", &ch);
    while (ch != '\n') fscanf(fp, "%c", &ch); // skip documentation line

    int color_index[3];
    for (int i = 0; i < triangles_count; i++) {
        fscanf(fp, "v0 %f %f %f %f %f %f %d\n",
                &(triangles[i].v0.x), &(triangles[i].v0.y), &(triangles[i].v0.z),
                &(triangles[i].normal[0].x), &(triangles[i].normal[0].y), &(triangles[i].normal[0].z),
                &(color_index[0]));
        fscanf(fp, "v1 %f %f %f %f %f %f %d\n",
                &(triangles[i].v1.x), &(triangles[i].v1.y), &(triangles[i].v1.z),
                &(triangles[i].normal[1].x), &(triangles[i].normal[1].y), &(triangles[i].normal[1].z),
                &(color_index[1]));
        fscanf(fp, "v2 %f %f %f %f %f %f %d\n",
                &(triangles[i].v2.x), &(triangles[i].v2.y), &(triangles[i].v2.z),
                &(triangles[i].normal[2].x), &(triangles[i].normal[2].y), &(triangles[i].normal[2].z),
                &(color_index[2]));
        fscanf(fp, "face normal %f %f %f\n",
                &(triangles[i].face_normal.x),
                &(triangles[i].face_normal.y),
                &(triangles[i].face_normal.z));

        triangles[i].color[0] = (unsigned char) (int) (255 * (diffuse[color_index[0]].x));
        triangles[i].color[1] = (unsigned char) (int) (255 * (diffuse[color_index[0]].y));
        triangles[i].color[2] = (unsigned char) (int) (255 * (diffuse[color_index[0]].z));

        // define bounding box
        if (triangles[i].v0.x > bbox.x_max) bbox.x_max = triangles[i].v0.x;
        if (triangles[i].v0.x < bbox.x_min) bbox.x_min = triangles[i].v0.x;
        if (triangles[i].v0.y > bbox.y_max) bbox.y_max = triangles[i].v0.y;
        if (triangles[i].v0.y < bbox.y_min) bbox.y_min = triangles[i].v0.y;
        if (triangles[i].v0.z > bbox.z_max) bbox.z_max = triangles[i].v0.z;
        if (triangles[i].v0.z < bbox.z_min) bbox.z_min = triangles[i].v0.z;

        if (triangles[i].v1.x > bbox.x_max) bbox.x_max = triangles[i].v1.x;
        if (triangles[i].v1.x < bbox.x_min) bbox.x_min = triangles[i].v1.x;
        if (triangles[i].v1.y > bbox.y_max) bbox.y_max = triangles[i].v1.y;
        if (triangles[i].v1.y < bbox.y_min) bbox.y_min = triangles[i].v1.y;
        if (triangles[i].v1.z > bbox.z_max) bbox.z_max = triangles[i].v1.z;
        if (triangles[i].v1.z < bbox.z_min) bbox.z_min = triangles[i].v1.z;

        if (triangles[i].v2.x > bbox.x_max) bbox.x_max = triangles[i].v2.x;
        if (triangles[i].v2.x < bbox.x_min) bbox.x_min = triangles[i].v2.x;
        if (triangles[i].v2.y > bbox.y_max) bbox.y_max = triangles[i].v2.y;
        if (triangles[i].v2.y < bbox.y_min) bbox.y_min = triangles[i].v2.y;
        if (triangles[i].v2.z > bbox.z_max) bbox.z_max = triangles[i].v2.z;
        if (triangles[i].v2.z < bbox.z_min) bbox.z_min = triangles[i].v2.z;
    }

    fclose(fp);

}

void read_file(int i)
{
    // create new model from given file
    m = new Model(file->get_text());
}

// /MODEL ------------------------------------------------------


// GLUT --------------------------------------------------------

void renderOpenGL()
{

}

void renderClose2GL()
{

}

// /GLUT -------------------------------------------------------


// GUI ---------------------------------------------------------

void create_gui()
{
    GLUI *glui = GLUI_Master.create_glui("Options", false, 1000, 0);
    glui->set_main_gfx_window(win_id[OPENGL_WINDOW]);

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
    file = glui->add_edittext("Path:", GLUI_EDITTEXT_TEXT, NULL, 0, read_file);
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
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glClearColor(0, 0, 0, 0);
    glEnable(GL_DEPTH_TEST);

    // initialize Close2GL window
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition(500, 0);
    glutInitWindowSize(500, 375);
    win_id[CLOSE2GL_WINDOW] = glutCreateWindow("Close2GL");
    glutDisplayFunc(renderClose2GL);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_ALPHA);
    glClearColor(0, 0, 0, 0);
    glEnable(GL_DEPTH_TEST);

    // create GUI
    create_gui();

    // GLUT main loop
    glutMainLoop();
    return 0;
}
