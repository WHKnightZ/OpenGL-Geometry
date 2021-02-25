#ifndef KZ_MATH_H
#define KZ_MATH_H

#include <math.h>

#define PI 3.141592f
#define PI_DOUBLE 6.283185f
#define PI_HALF 1.570796f
#define RADIAN 57.295780f

class Math {
  public:
    static float map_sin[3142], map_cos[3142];

    static void init() {
        for (int i = 0; i < 3142; i++) {
            map_sin[i] = sin(i / 500.0f);
            map_cos[i] = cos(i / 500.0f);
        }
    }

    static float get_sin(float x) {
        int y = (int)(x / PI_DOUBLE);
        if (x < 0.0f)
            y -= 1;
        x -= y * PI_DOUBLE;
        return map_sin[(int)(x * 500)];
    }

    static float get_cos(float x) {
        int y = (int)(x / PI_DOUBLE);
        if (x < 0.0f)
            y -= 1;
        x -= y * PI_DOUBLE;
        return map_cos[(int)(x * 500)];
    }

    static void cross_product_normalize(float *a, float *b, float *c) {
        c[0] = a[1] * b[2] - a[2] * b[1];
        c[1] = a[2] * b[0] - a[0] * b[2];
        c[2] = a[0] * b[1] - a[1] * b[0];
        float distance = sqrt(c[0] * c[0] + c[1] * c[1] + c[2] * c[2]);
        c[0] /= distance;
        c[1] /= distance;
        c[2] /= distance;
    }

    static float radian_to_degree(float x) { return x * RADIAN; }
    static float degree_to_radian(float x) { return x / RADIAN; }
    static int clamp(int value, int min, int max) { return value < min ? min : value > max ? max : value; }
    static float clamp(float value, float min, float max) { return value < min ? min : value > max ? max : value; }
};

#endif