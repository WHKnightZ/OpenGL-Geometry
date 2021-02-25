#include "main.h"
#include "geometry.h"

float rotate_x = -70.0f, rotate_y = 0.0f, rotate_z = 0.0f;

Geometry *geometry;
Sphere *sphere;
Cylinder *cylinder;

void init() {
    printf("%s\n", glGetString(GL_VENDOR));
    printf("%s\n", glGetString(GL_RENDERER));
    printf("%s\n", glGetString(GL_VERSION));

    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, WIDTH, HEIGHT);
    gluPerspective(30.0, 1, 1.0, 2000.0);
    glTranslatef(0.0f, 0.0f, -400.0f);
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    Math::init();

    sphere = new Sphere(40.0f, 20, 10);
    cylinder = new Cylinder(30.0f, 50.0f, 80.0f, 20, 10);
    
    geometry = cylinder;
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glRotatef(rotate_x, 1.0f, 0.0f, 0.0f);
    glRotatef(rotate_y, 0.0f, 1.0f, 0.0f);
    glRotatef(rotate_z, 0.0f, 0.0f, 1.0f);

    geometry->render();

    glutSwapBuffers();
}

void timer(int v) {
    // rotate_x += 0.5f;
    // rotate_y += 0.5f;
    rotate_z += 0.5f;
    glutPostRedisplay();
    glutTimerFunc(INTERVAL, timer, 0);
}

void idle() {
    static float fps = 0.0f;
    static int time_old = clock();
    int time_new = clock();
    int time_offset = time_new - time_old;
    if (time_offset > CLOCKS_PER_SEC) {
        time_old = time_new;
        printf("FPS: %.2f\n", fps * CLOCKS_PER_SEC / time_offset);
        fps = 0.0f;
    }
    fps += 1.0f;
    rotate_z += 0.5f;
    glutPostRedisplay();
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glEnable(GL_MULTISAMPLE);
    glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);
    glutSetOption(GLUT_MULTISAMPLE, 8);
    int POS_X, POS_Y;
    POS_X = (glutGet(GLUT_SCREEN_WIDTH) - WIDTH) >> 1;
    POS_Y = (glutGet(GLUT_SCREEN_HEIGHT) - HEIGHT) >> 1;
    glutInitWindowPosition(POS_X, POS_Y);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Geometry");
    init();
    glutTimerFunc(0, timer, 0);
    // glutIdleFunc(idle);
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
