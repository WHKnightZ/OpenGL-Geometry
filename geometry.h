#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <vector>

#include "Library/kz_math.h"
#include "Library/kz_texture.h"

static float *color_default = new float[4]{1.0f, 1.0f, 1.0f, 1.0f};

class Face {
  public:
    int *vert_indices;
    float *normal;
    int drt;

    Face(int _vert_indices[], int _drt) {
        vert_indices = _vert_indices;
        drt = _drt;
        normal = new float[3];
    }
};

class Geometry {
  protected:
    std::vector<float *> vertices;
    std::vector<Face *> faces;
    float *color;
    GLuint list;

    void calc_normal(Face *face) {
        float _vertices[2][3];
        int index_0, index_1, index_2;
        index_0 = face->vert_indices[0];
        index_1 = face->vert_indices[1];
        index_2 = face->vert_indices[2];
        for (int i = 0; i < 3; i++) {
            _vertices[0][i] = vertices[index_1][i] - vertices[index_0][i];
            _vertices[1][i] = face->drt * (vertices[index_2][i] - vertices[index_0][i]);
        }
        Math::cross_product_normalize(_vertices[0], _vertices[1], face->normal);
    }

    void calc_normal() {
        for (Face *face : faces)
            calc_normal(face);
    }

    void draw() {
        glColor4fv(color);
        for (Face *face : faces) {
            // render_wire(face);
            glBegin(GL_POLYGON);
            glNormal3fv(face->normal);
            for (int i = 0; i < 3; i++)
                glVertex3fv(vertices[face->vert_indices[i]]);
            glEnd();
        }
    }

    void create_list() {
        list = glGenLists(1);
        glNewList(list, GL_COMPILE);
        draw();
        glEndList();
        int length = vertices.size();
        for (int i = 0; i < length; i++)
            delete vertices[i];
        length = faces.size();
        for (int i = 0; i < length; i++)
            delete faces[i];
    }

    void render_wire(Face *face) {
        glColor3f(0.0f, 0.0f, 0.0f);
        glBegin(GL_LINES);
        for (int i = 0; i < 2; i++) {
            glVertex3fv(vertices[face->vert_indices[i]]);
            glVertex3fv(vertices[face->vert_indices[i + 1]]);
        }
        glVertex3fv(vertices[face->vert_indices[2]]);
        glVertex3fv(vertices[face->vert_indices[0]]);
        glEnd();
    }

  public:
    Geometry(float *_color = color_default) {
        list = 0;
        color = _color;
    }

    void add_vertex(float *vertex) { vertices.push_back(vertex); }

    void add_face(Face *face) { faces.push_back(face); }

    void render() {
        if (list)
            glCallList(list);
        else
            draw();
    }
};

class Sphere : public Geometry {
  public:
    Sphere(float radius, int slices = 20, int stacks = 10) {
        slices = Math::clamp(slices, 3, 32);
        stacks = Math::clamp(stacks, 2, 32);
        float x, y, z, angle_increment = Math::degree_to_radian(360.0f / slices);
        float z_offset = Math::degree_to_radian(180.0f / stacks);
        // float z_offset = -radius * 2 / stacks;
        float stack_radius;
        add_vertex(new float[3]{0, 0, radius});
        add_vertex(new float[3]{0, 0, -radius});
        for (int i = 1; i < stacks; i++) {
            z = Math::get_sin(3.14159/2 - z_offset * i) * radius;
            stack_radius = sqrt(radius * radius - z * z);
            // stack_radius = Math::get_sin(z_offset * i) * radius;
            for (int j = 0; j < slices; j++) {
                x = Math::get_cos(j * angle_increment);
                y = Math::get_sin(j * angle_increment);
                add_vertex(new float[3]{x * stack_radius, y * stack_radius, z});
            }
        }
        int total_vertices = slices * (stacks - 1) + 2;
        int first_top, first_bot, second_top, second_bot;
        for (int i = 0; i < slices; i++) {
            first_top = 2 + i;
            first_bot = total_vertices - i - 1;
            if (i == slices - 1) {
                second_top = 2;
                second_bot = total_vertices - 1;
            } else {
                second_top = 3 + i;
                second_bot = total_vertices - i - 2;
            }
            add_face(new Face(new int[3]{0, first_top, second_top}, 1));
            add_face(new Face(new int[3]{1, first_bot, second_bot}, 1));
        }
        int stacks_1 = stacks - 2;
        for (int i = 0; i < slices; i++) {
            first_top = 2 + i;
            second_top = (i != slices - 1) ? first_top + 1 : 2;
            for (int j = 0; j < stacks_1; j++) {
                first_bot = first_top + slices;
                second_bot = second_top + slices;
                add_face(new Face(new int[3]{first_top, first_bot, second_top}, 1));
                add_face(new Face(new int[3]{first_bot, second_bot, second_top}, 1));
                first_top = first_bot;
                second_top = second_bot;
            }
        }
        calc_normal();
        create_list();
    }
};

class Cylinder : public Geometry {
  public:
    Cylinder(float radius_top, float radius_bottom, float height, int slices = 20, int stacks = 10,
             bool auto_calc_normal = true, bool is_list = true) {
        slices = Math::clamp(slices, 3, 64);
        stacks = Math::clamp(stacks, 1, 64);
        float x, y, angle_increment = Math::degree_to_radian(360.0f / slices);
        float xy_offset = (radius_bottom - radius_top) / stacks;
        float z_offset = -height / stacks;
        for (int i = 0; i < slices; i++) {
            x = Math::get_cos(i * angle_increment);
            y = Math::get_sin(i * angle_increment);
            for (int j = 0; j <= stacks; j++)
                add_vertex(new float[3]{x * (radius_top + xy_offset * j), y * (radius_top + xy_offset * j),
                                        height / 2 + z_offset * j});
        }
        int first_top, first_bot, second_top, second_bot, num_face = slices - 2;
        int stacks_1 = stacks + 1;
        for (int i = 1; i <= num_face; i++) {
            first_top = i * stacks_1;
            first_bot = first_top + stacks;
            second_top = (i + 1) * stacks_1;
            second_bot = second_top + stacks;
            add_face(new Face(new int[3]{0, first_top, second_top}, 1));
            add_face(new Face(new int[3]{stacks, first_bot, second_bot}, -1));
        }
        for (int i = 0; i < slices; i++) {
            first_top = i * stacks_1;
            second_top = (i != slices - 1) ? first_top + stacks_1 : 0;
            for (int j = 0; j < stacks; j++) {
                first_bot = first_top + 1;
                second_bot = second_top + 1;
                add_face(new Face(new int[3]{first_top, first_bot, second_top}, 1));
                add_face(new Face(new int[3]{first_bot, second_bot, second_top}, 1));
                first_top = first_bot;
                second_top = second_bot;
            }
        }

        if (auto_calc_normal)
            calc_normal();

        if (is_list)
            create_list();
    }
};

#endif