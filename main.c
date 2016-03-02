#define GL_GLEXT_PROTOTYPES

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <GL/glx.h>
#include <GL/glext.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>

#include "dragonTamer.h"

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 800

/* ~~~ Global Variables ~~~ */
// Dragon Identifiers
unsigned int dragonRed = 0;

/* ~~~ end Global Variables ~~~ */


struct point {
    float x, y, z;
};

char* readShaderProgram(char* filename) {
    FILE *fp;
    char *content = NULL;
    int fd, count;
    fd = open(filename,O_RDONLY);
    count = lseek(fd,0,SEEK_END);
    close(fd);
    content = (char *)calloc(1,(count+1));
    fp = fopen(filename,"r");
    count = fread(content,sizeof(char),count,fp);
    content[count] = '\0';
    fclose(fp);
    return content;
}

unsigned int setShaders() {
    GLint vertCompiled, fragCompiled;
    char *vs, *fs;
    GLuint v, f, p;

    v = glCreateShader(GL_VERTEX_SHADER);
    f = glCreateShader(GL_FRAGMENT_SHADER);
   
    vs = readShaderProgram("shader.vert");
    fs = readShaderProgram("shader.frag");

    glShaderSource(v,1,(const char **)&vs,NULL);
    glShaderSource(f,1,(const char **)&fs,NULL);

    free(vs);
    free(fs); 

    glCompileShader(v);
    glCompileShader(f);

    p = glCreateProgram();
    glAttachShader(p,f);
    glAttachShader(p,v);
    glLinkProgram(p);
    glUseProgram(p);

    return(p);
}

void doViewVolume() {
    struct point eye, view, up;
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 1.0, 0.1, 20.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    eye.x  = 4.0; eye.y  = 4.0; eye.z  = 4.0;
    view.x = 0.0; view.y = 0.0; view.z = 0.0;
    up.x   = 0.0; up.y   = 1.0; up.z   = 0.0;

    gluLookAt(eye.x, eye.y, eye.z, view.x, view.y, view.z, up.x, up.y, up.z);
}

void doMaterialRed() {
    float matAmbient[] = {0.0,0.0,0.0,1.0};
    float matDiffuse[] = {0.9,0.1,0.1,1.0};
    float matSpecular[] = {1.0,1.0,1.0,1.0};
    float matShininess[] = {2.0};

    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);
}

void doLights() {
    /* white light */
    float light0_ambient[] = { 0.0, 0.0, 0.0, 0.0 };
    float light0_diffuse[] = { 1.0, 1.0, 1.0, 0.0 };
    float light0_specular[] = { 1.0, 1.0, 1.0, 0.0 };
    float light0_position[] = { 1.5, 2.0, 2.0, 1.0 };
    float light0_direction[] = { -1.5, -2.0, -2.0, 1.0};

    /* turn off scene default ambient */
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT,light0_ambient);

    /* make specular correct for spots */
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,1);

    glLightfv(GL_LIGHT0,GL_AMBIENT,light0_ambient);
    glLightfv(GL_LIGHT0,GL_DIFFUSE,light0_diffuse);
    glLightfv(GL_LIGHT0,GL_SPECULAR,light0_specular);
    glLightf(GL_LIGHT0,GL_SPOT_EXPONENT,1.0);
    glLightf(GL_LIGHT0,GL_SPOT_CUTOFF,180.0);
    glLightf(GL_LIGHT0,GL_CONSTANT_ATTENUATION,0.5);
    glLightf(GL_LIGHT0,GL_LINEAR_ATTENUATION,0.1);
    glLightf(GL_LIGHT0,GL_QUADRATIC_ATTENUATION,0.01);
    glLightfv(GL_LIGHT0,GL_POSITION,light0_position);
    glLightfv(GL_LIGHT0,GL_SPOT_DIRECTION,light0_direction);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

}

void drawStuff() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnableClientState(GL_VERTEX_ARRAY);

    doMaterialRed();
    glVertexPointer(3, GL_FLOAT, 3*sizeof(GLfloat), plyGetDragon(dragonRed));
    glDrawElements(GL_TRIANGLES, plyIndexCount(), GL_UNSIGNED_INT, plyIndices());

    glFlush();
}

void initOpenGL(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(100, 50);
    glutCreateWindow("The Welsh Dragons");

    glClearColor(0.35, 0.35, 0.35, 0.0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE_ARB);

    doLights();
    setShaders();
    doViewVolume();
}

void intDragons() {
    plyLoad("welsh-dragon.ply");

    dragonRed = plyNewDragon();
    plyCenter(dragonRed, 0.0, 0.0, 0.0);
    plyScale(dragonRed, 1.0);
}

void getOut(unsigned char key, int x, int y) {
    switch(key) {
        case 'q':
            plyUnload();
            exit(1);
        default:
            break;
    }
}

int main(int argc, char** argv) {
    initOpenGL(argc, argv);
    intDragons();
    glutDisplayFunc(drawStuff);
    glutKeyboardFunc(getOut);
    glutMainLoop();

    return 0;
}
