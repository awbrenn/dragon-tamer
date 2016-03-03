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
#include <math.h>

#include "dragonTamer.h"
#include "viewPortNavigation.h"
#include "utilities.h"

#define EYEDX 0.04
#define WINDOW_WIDTH  1280
#define WINDOW_HEIGHT 720

/* ~~~ Global Variables ~~~ */
// Dragon Identifiers
unsigned int dragonRed = 0;
unsigned int dragonGreen = 0;
unsigned int dragonBlue = 0;
unsigned int dragonYellow = 0;

// Display mode
int lookAround = 0;

/* ~~~ end Global Variables ~~~ */

struct point {
    float x, y, z;
};

char* readShaderProgram(char* filename) {
    FILE *fp;
    char *content = NULL;
    int fd, count;
    fd = open(filename,O_RDONLY);
    count = (int)lseek(fd,0,SEEK_END);
    close(fd);
    content = (char *)calloc(1,(size_t)(count+1));
    fp = fopen(filename,"r");
    count = (int)fread(content,sizeof(char),(size_t)count,fp);
    content[count] = '\0';
    fclose(fp);
    return content;
}

unsigned int setShaders() {
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

void vv(float xt) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-0.5+xt*(1.0/5.0),0.5+xt*(1.0/5.0),-0.5,0.5,1.25,20.0);
    glTranslatef(xt,0.0,0.0);
}

void doViewVolume() {
    if(!lookAround) vv(0.0);
    else {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45.0,16.0f/9.0f,1.0,20.0);
    }

    struct point eye, view, up;
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    eye.x  = 5.5; eye.y  = 4.5; eye.z  = 5.5;
    view.x = 0.0; view.y = 0.0; view.z = 0.0;
    up.x   = 0.0; up.y   = 1.0; up.z   = 0.0;

    gluLookAt(eye.x, eye.y, eye.z, view.x, view.y, view.z, up.x, up.y, up.z);
}

void doMaterialRed() {
    float matAmbient[] = {0.0,0.0,1.0,1.0};
    float matDiffuse[] = {0.9,0.1,0.1,1.0};
    float matSpecular[] = {1.0,1.0,1.0,1.0};
    float matShininess[] = {2.0};

    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);
}

void doMaterialBlue() {
    float matAmbient[] = {0.0,0.0,1.0,1.0};
    float matDiffuse[] = {0.1,0.1,0.9,1.0};
    float matSpecular[] = {1.0,1.0,1.0,1.0};
    float matShininess[] = {2.0};

    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);
}
void doMaterialYellow() {
    float matAmbient[] = {0.0,0.0,1.0,1.0};
    float matDiffuse[] = {0.9,0.9,0.1,1.0};
    float matSpecular[] = {1.0,1.0,1.0,1.0};
    float matShininess[] = {2.0};

    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);
}
void doMaterialGreen() {
    float matAmbient[] = {0.0,0.0,1.0,1.0};
    float matDiffuse[] = {0.1,0.9,0.1,1.0};
    float matSpecular[] = {1.0,1.0,1.0,1.0};
    float matShininess[] = {2.0};
    
    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);
}  

void rotateLightX(float* pos, float angle) {
    struct point temp;
    temp.x = pos[0];
    temp.y = pos[1];
    temp.z = pos[2];

    pos[0] = 1 * temp.x + 0          * temp.y + 0          * temp.z;
    pos[1] = 0 * temp.x + cosf(angle) * temp.y - sinf(angle) * temp.z;
    pos[2] = 0 * temp.x + sinf(angle) * temp.y + cosf(angle) * temp.z;
}
void rotateLightY(float* pos, float angle) {
    struct point temp;
    temp.x = pos[0];
    temp.y = pos[1];
    temp.z = pos[2];

    pos[0] = cosf(angle)  * temp.x + 0 * temp.y + sinf(angle) * temp.z;
    pos[1] = 0            * temp.x + 1 * temp.y + 0          * temp.z;
    pos[2] = -sinf(angle) * temp.x + 0 * temp.y + cosf(angle) * temp.z;
}

void doLights() {
    /* key light */
    float keyAmbient[]   = { 0.0, 0.0, 0.0, 0.0 };
    float keyDiffuse[]   = { 1.2, 1.2, 1.2, 0.0 };
    float keySpecular[]  = { 1.2, 1.2, 1.2, 0.0 };
    float keyDirection[] = { 0.0, 0.0, 0.0, 0.0};
    float keyPosition[]  = { 5.5, 4.5, 5.5, 1.0 };

    // Align to Y-Z Plane
    rotateLightY(keyPosition, (float)-M_PI/4.0f);

    // Rotate upwards 30 degrees
    rotateLightX(keyPosition, (float)-M_PI/6.0f);

    // Rotate back to eye
    rotateLightY(keyPosition, (float)M_PI/4.0f);

    // Rotate 30 degrees left of the eye
    rotateLightY(keyPosition, (float)-M_PI/6.0f);

    /* fill light */
    float fillAmbient[]   = { 0.0, 0.0, 0.0, 0.0 };
    float fillDiffuse[]   = { 0.6, 0.6, 0.6, 0.0 };
    float fillSpecular[]  = { 0.6, 0.6, 0.6, 0.0 };
    float fillDirection[] = { 0.0, 0.0, 0.0, 1.0 };
    float fillPosition[]  = { 5.5, 4.5, 5.5, 1.0 };

    // Align to Y-Z Plane
    rotateLightY(fillPosition, (float)-M_PI/4.0f);

    // Rotate downwards 30 degrees
    rotateLightX(fillPosition, (float)M_PI/6.0f);

    // Rotate back to eye
    rotateLightY(fillPosition, (float)M_PI/4.0f);

    // Rotate 45 degrees right of the eye
    rotateLightY(fillPosition, (float)M_PI/4.0f);

    /* back light */
    float backAmbient[]   = { 0.0f, 0.0f, 0.0f, 0.0f };
    float backDiffuse[]   = { 0.6f, 0.6f, 0.6f, 0.0f };
    float backSpecular[]  = { 0.6f, 0.6f, 0.6f, 0.0f };
    float backDirection[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    float backPosition[]  = { -5.5f, -4.5f, -5.5f, 1.0f };

    /* turn off scene default ambient */
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, keyAmbient);

    /* make specular correct for spots */
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,1);

    glLightfv(GL_LIGHT0,GL_AMBIENT,keyAmbient);
    glLightfv(GL_LIGHT0,GL_DIFFUSE,keyDiffuse);
    glLightfv(GL_LIGHT0,GL_SPECULAR,keySpecular);
    glLightf(GL_LIGHT0,GL_SPOT_EXPONENT,1.0);
    glLightf(GL_LIGHT0,GL_SPOT_CUTOFF,180.0);
    glLightf(GL_LIGHT0,GL_CONSTANT_ATTENUATION,0.5);
    glLightf(GL_LIGHT0,GL_LINEAR_ATTENUATION,0.1);
    glLightf(GL_LIGHT0,GL_QUADRATIC_ATTENUATION,0.01);
    glLightfv(GL_LIGHT0,GL_POSITION,keyPosition);
    glLightfv(GL_LIGHT0,GL_SPOT_DIRECTION,keyDirection);

    glLightfv(GL_LIGHT1,GL_AMBIENT,fillAmbient);
    glLightfv(GL_LIGHT1,GL_DIFFUSE,fillDiffuse);
    glLightfv(GL_LIGHT1,GL_SPECULAR,fillSpecular);
    glLightf(GL_LIGHT1,GL_SPOT_EXPONENT,1.0);
    glLightf(GL_LIGHT1,GL_SPOT_CUTOFF,180.0);
    glLightf(GL_LIGHT1,GL_CONSTANT_ATTENUATION,0.5);
    glLightf(GL_LIGHT1,GL_LINEAR_ATTENUATION,0.1);
    glLightf(GL_LIGHT1,GL_QUADRATIC_ATTENUATION,0.01);
    glLightfv(GL_LIGHT1,GL_POSITION,fillPosition);
    glLightfv(GL_LIGHT1,GL_SPOT_DIRECTION,fillDirection);
   
    glLightfv(GL_LIGHT2,GL_AMBIENT,backAmbient);
    glLightfv(GL_LIGHT2,GL_DIFFUSE,backDiffuse);
    glLightfv(GL_LIGHT2,GL_SPECULAR,backSpecular);
    glLightf(GL_LIGHT2,GL_SPOT_EXPONENT,1.0);
    glLightf(GL_LIGHT2,GL_SPOT_CUTOFF,180.0);
    glLightf(GL_LIGHT2,GL_CONSTANT_ATTENUATION,0.5);
    glLightf(GL_LIGHT2,GL_LINEAR_ATTENUATION,0.1);
    glLightf(GL_LIGHT2,GL_QUADRATIC_ATTENUATION,0.01);
    glLightfv(GL_LIGHT2,GL_POSITION,backPosition);
    glLightfv(GL_LIGHT2,GL_SPOT_DIRECTION,backDirection);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHT2);
}

void drawStuff() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    doMaterialRed();
    glNormalPointer(GL_FLOAT, 3*sizeof(GLfloat), plyDragonNormals(dragonRed));
    glVertexPointer(3, GL_FLOAT, 3*sizeof(GLfloat), plyDragonVertices(dragonRed));
    glDrawElements(GL_TRIANGLES, plyIndexCount(), GL_UNSIGNED_INT, plyIndices());

    doMaterialBlue();
    glNormalPointer(GL_FLOAT, 3*sizeof(GLfloat), plyDragonNormals(dragonBlue));
    glVertexPointer(3, GL_FLOAT, 3*sizeof(GLfloat), plyDragonVertices(dragonBlue));
    glDrawElements(GL_TRIANGLES, plyIndexCount(), GL_UNSIGNED_INT, plyIndices());

    doMaterialYellow();
    glNormalPointer(GL_FLOAT, 3*sizeof(GLfloat), plyDragonNormals(dragonYellow));
    glVertexPointer(3, GL_FLOAT, 3*sizeof(GLfloat), plyDragonVertices(dragonYellow));
    glDrawElements(GL_TRIANGLES, plyIndexCount(), GL_UNSIGNED_INT, plyIndices());

    doMaterialGreen();
    glNormalPointer(GL_FLOAT, 3*sizeof(GLfloat), plyDragonNormals(dragonGreen));
    glVertexPointer(3, GL_FLOAT, 3*sizeof(GLfloat), plyDragonVertices(dragonGreen));
    glDrawElements(GL_TRIANGLES, plyIndexCount(), GL_UNSIGNED_INT, plyIndices());


    GLfloat standNormals[] = { 1.0f, 0.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f, 0.0f,
                               0.0f, 1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f, 0.0f,
                               0.0f, 0.0f, 1.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, 1.0f,
                              -1.0f, 0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
                               0.0f,-1.0f, 0.0f,   0.0f,-1.0f, 0.0f,   0.0f,-1.0f, 0.0f,   0.0f,-1.0f, 0.0f,
                               0.0f, 0.0f,-1.0f,   0.0f, 0.0f,-1.0f,   0.0f, 0.0f,-1.0f,   0.0f, 0.0f,-1.0f };

    GLfloat standRed[] = { 2.0f,-0.5f, 2.0f,   2.0f,-0.5f, 0.0f,   2.0f,-1.0f, 0.0f,   2.0f,-1.0f, 2.0f,
                           0.0f,-0.5f, 0.0f,   0.0f,-0.5f, 2.0f,   2.0f,-0.5f, 2.0f,   2.0f,-0.5f, 0.0f,
                           0.0f,-0.5f, 2.0f,   2.0f,-0.5f, 2.0f,   2.0f,-1.0f, 2.0f,   0.0f,-1.0f, 2.0f,
                           0.0f,-0.5f, 2.0f,   0.0f,-0.5f, 0.0f,   0.0f,-1.0f, 0.0f,   0.0f,-1.0f, 2.0f,
                           0.0f,-1.0f, 0.0f,   0.0f,-1.0f, 2.0f,   2.0f,-1.0f, 2.0f,   2.0f,-1.0f, 0.0f,
                           0.0f,-0.5f, 0.0f,   2.0f,-0.5f, 0.0f,   2.0f,-1.0f, 0.0f,   0.0f,-1.0f, 0.0f };


    GLfloat standGreen[] = { 0.0f, 0.5f, 0.0f,   0.0f, 0.5f,-2.0f,   0.0f,-1.0f,-2.0f,   0.0f,-1.0f, 0.0f,
                            -2.0f, 0.5f,-2.0f,  -2.0f, 0.5f, 0.0f,   0.0f, 0.5f, 0.0f,   0.0f, 0.5f,-2.0f,
                            -2.0f, 0.5f, 0.0f,   0.0f, 0.5f, 0.0f,   0.0f,-1.0f, 0.0f,  -2.0f,-1.0f, 0.0f,
                            -2.0f, 0.5f,-2.0f,  -2.0f, 0.5f, 0.0f,  -2.0f,-1.0f, 0.0f,  -2.0f,-1.0f,-2.0f,
                            -2.0f,-1.0f,-2.0f,  -2.0f,-1.0f, 0.0f,   0.0f,-1.0f, 0.0f,   0.0f,-1.0f,-2.0f,
                            -2.0f, 0.5f,-2.0f,   0.0f, 0.5f,-2.0f,   0.0f,-1.0f,-2.0f,  -2.0f,-1.0f,-2.0f };


    GLfloat standBlue[] = { 2.0f, 0.0f, 0.0f,   2.0f, 0.0f,-2.0f,   2.0f,-1.0f,-2.0f,   2.0f,-1.0f, 0.0f,
                            0.0f, 0.0f,-2.0f,   0.0f, 0.0f, 0.0f,   2.0f, 0.0f, 0.0f,   2.0f, 0.0f,-2.0f,
                            0.0f, 0.0f, 0.0f,   2.0f, 0.0f, 0.0f,   2.0f,-1.0f, 0.0f,   0.0f,-1.0f, 0.0f,
                            0.0f, 0.0f, 0.0f,   0.0f, 0.0f,-2.0f,   0.0f,-1.0f,-2.0f,   0.0f,-1.0f, 0.0f,
                            0.0f,-1.0f,-2.0f,   0.0f,-1.0f, 0.0f,   2.0f,-1.0f, 0.0f,   2.0f,-1.0f,-2.0f,
                            0.0f, 0.0f,-2.0f,   2.0f, 0.0f,-2.0f,   2.0f,-1.0f,-2.0f,   0.0f,-1.0f,-2.0f };
    
    GLfloat standYellow[] = { 0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 2.0f,   0.0f,-1.0f, 2.0f,   0.0f,-1.0f, 0.0f,
                             -2.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 2.0f,  -2.0f, 0.0f, 2.0f,
                              0.0f, 0.0f, 2.0f,  -2.0f, 0.0f, 2.0f,  -2.0f,-1.0f, 2.0f,   0.0f,-1.0f, 2.0f,
                             -2.0f, 0.0f, 0.0f,  -2.0f, 0.0f, 2.0f,  -2.0f,-1.0f, 2.0f,  -2.0f,-1.0f, 0.0f,
                             -2.0f,-1.0f, 0.0f,   0.0f,-1.0f, 0.0f,   0.0f,-1.0f, 2.0f,  -2.0f,-1.0f, 2.0f,
                              0.0f, 0.0f, 0.0f,  -2.0f, 0.0f, 0.0f,  -2.0f,-1.0f, 0.0f,   0.0f,-1.0f, 0.0f };
    
    glNormalPointer(GL_FLOAT, 3*sizeof(GLfloat), standNormals);

    doMaterialRed();
    glVertexPointer(3, GL_FLOAT, 3*sizeof(GLfloat), standRed);
    glDrawArrays(GL_QUADS, 0, 24);

    doMaterialGreen();
    glVertexPointer(3, GL_FLOAT, 3*sizeof(GLfloat), standGreen);
    glDrawArrays(GL_QUADS, 0, 24);
    
    doMaterialBlue();
    glVertexPointer(3, GL_FLOAT, 3*sizeof(GLfloat), standBlue);
    glDrawArrays(GL_QUADS, 0, 24);
    
    doMaterialYellow();
    glVertexPointer(3, GL_FLOAT, 3*sizeof(GLfloat), standYellow);
    glDrawArrays(GL_QUADS, 0, 24);
    
    glFlush();
}

void initOpenGL(int argc, char** argv) {
    srandom(123456789);
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE | GLUT_ACCUM);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(100, 50);
    glutCreateWindow("The Welsh Dragons");

    glClearColor(0.0, 0.0, 0.0, 0.0);
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
    plyRotateX(dragonRed, (float)(3*M_PI/2));
    plyRotateY(dragonRed, (float)(5*M_PI/4));
    plyCenter(dragonRed, 1.0, 0.1, 1.0);
    
    dragonBlue = plyNewDragon();
    plyCenter(dragonBlue, 0.0, 0.0, 0.0);
    plyScale(dragonBlue, 1.0);
    plyRotateX(dragonBlue, (float)(3*M_PI/2));
    plyRotateY(dragonBlue, (float)(3*M_PI/2));
    plyCenter(dragonBlue, 1.0f, 0.6f, -1.0f);
    
    dragonYellow = plyNewDragon();
    plyCenter(dragonYellow, 0.0, 0.0, 0.0);
    plyScale(dragonYellow, 1.0);
    plyRotateX(dragonYellow, (float)(3*M_PI/2));
    plyRotateY(dragonYellow, (float)M_PI);
    plyCenter(dragonYellow, -1.0f, 0.6f, 1.0f);

    dragonGreen = plyNewDragon();
    plyCenter(dragonGreen, 0.0, 0.0, 0.0);
    plyScale(dragonGreen, 1.0);
    plyRotateX(dragonGreen, (float)(3*M_PI/2));
    plyRotateY(dragonGreen, (float)(5*M_PI/4));
    plyCenter(dragonGreen, -1.0f, 1.1f, -1.0f);
}

void go() {
    if(lookAround) {
        drawStuff();
        return;
    }

    float xt;
    glClear(GL_ACCUM_BUFFER_BIT);
    for(xt = (float)-EYEDX; xt < EYEDX; xt += EYEDX / 10.0){
        vv(xt);
        drawStuff();
        glAccum(GL_ACCUM,0.05);
    }
    glAccum(GL_RETURN,1.0);
    glFlush();
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

void printUsage() {
    fprintf(stdout,
            "Use left click + drag to rotate around viewpoint\n"
            "Use middle mouse button click + drag to pan the camera and viewpoint\n"
            "Use middle mouse scroll wheel to zoom in and out of viewpoint\n");
}

int main(int argc, char** argv) {
    if(argc != 2) dieWithError("No commandline parameter given.\nEnter parameter 0 for a static image with depth of field.\nEnter parameter 1 for a look-around mode with mouse movements.");
    lookAround = atoi(argv[1]);

    initOpenGL(argc, argv);
    intDragons();
    glutDisplayFunc(go);

    if(lookAround) {
        printUsage();
        glutIdleFunc(idle);
        glutKeyboardFunc(handleKeys);
        glutMouseFunc(handleMouse);
        glutMotionFunc(handleMovedMouse);
    }
    else glutKeyboardFunc(getOut);
    glutMainLoop();

    return 0;
}
