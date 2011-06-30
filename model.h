#ifndef _MODEL_H_
#define _MODEL_H_

#include <vector3f.h>

#define MAX_TRIANGLES 100000
#define MAX_MATERIALS 100000

using namespace std;

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
    float text_coord[3][2];
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

        bool hasTexture;

        vector3f ambient[MAX_MATERIALS],
                 diffuse[MAX_MATERIALS],
                 specular[MAX_MATERIALS];

        float shine[MAX_MATERIALS];

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

Model::Model(const char *path)
{
    read_file(path);
    find_center();
}

void Model::find_center()
{
    center = new vector3f();
    center->x = bbox.x_min + abs((bbox.x_max - bbox.x_min) / 2.0);
    center->y = bbox.y_min + abs((bbox.y_max - bbox.y_min) / 2.0);
    center->z = bbox.z_min + abs((bbox.z_max - bbox.z_min) / 2.0);
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

    for (int i = 0; i < material_count; i++) {
        fscanf(fp, "ambient color %f %f %f\n",
                &(ambient[i].x), &(ambient[i].y), &(ambient[i].z));
        fscanf(fp, "diffuse color %f %f %f\n",
                &(diffuse[i].x), &(diffuse[i].y), &(diffuse[i].z));
        fscanf(fp, "specular color %f %f %f\n",
                &(specular[i].x), &(specular[i].y), &(specular[i].z));
        fscanf(fp, "material shine %f\n", &(shine[i]));
    }

    fscanf(fp, "Texture = %c", &ch);
    hasTexture = (ch == 'Y');
    while (ch != '\n') fscanf(fp, "%c", &ch);

    fscanf(fp, "%c", &ch);
    while (ch != '\n') fscanf(fp, "%c", &ch); // skip documentation line

    int color_index[3];
    for (int i = 0; i < triangles_count; i++) {
        if (hasTexture) {
            fscanf(fp, "v0 %f %f %f %f %f %f %d %f %f\n",
                    &(triangles[i].v0.x), &(triangles[i].v0.y), &(triangles[i].v0.z),
                    &(triangles[i].normal[0].x), &(triangles[i].normal[0].y), &(triangles[i].normal[0].z),
                    &(color_index[0]),
                    &(triangles[i].text_coord[0][0]), &(triangles[i].text_coord[0][1]));
            fscanf(fp, "v1 %f %f %f %f %f %f %d %f %f\n",
                    &(triangles[i].v1.x), &(triangles[i].v1.y), &(triangles[i].v1.z),
                    &(triangles[i].normal[1].x), &(triangles[i].normal[1].y), &(triangles[i].normal[1].z),
                    &(color_index[1]),
                    &(triangles[i].text_coord[1][0]), &(triangles[i].text_coord[1][1]));
            fscanf(fp, "v2 %f %f %f %f %f %f %d %f %f\n",
                    &(triangles[i].v2.x), &(triangles[i].v2.y), &(triangles[i].v2.z),
                    &(triangles[i].normal[2].x), &(triangles[i].normal[2].y), &(triangles[i].normal[2].z),
                    &(color_index[2]),
                    &(triangles[i].text_coord[2][0]), &(triangles[i].text_coord[2][1]));
            fscanf(fp, "face normal %f %f %f\n",
                    &(triangles[i].face_normal.x),
                    &(triangles[i].face_normal.y),
                    &(triangles[i].face_normal.z));
        } else {
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
        }

        triangles[i].color[0] = (unsigned char) (int) (255 * (diffuse[color_index[0]].x));
        triangles[i].color[1] = (unsigned char) (int) (255 * (diffuse[color_index[1]].y));
        triangles[i].color[2] = (unsigned char) (int) (255 * (diffuse[color_index[2]].z));

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

#endif
