#include <GL/gl.h>

void plyLoad(char* filePath);
void plyUnload();

void plyCenter(unsigned int dragonID, float x, float y, float z);
void plyScale(unsigned int dragonID, float factor);
void plyRotate(unsigned int dragonID /* more parameters */);

unsigned int plyNewDragon();
GLfloat* plyGetDragon(unsigned int dragonID);
void plyDeleteDragon(unsigned int dragonID);

GLint* plyIndices();
GLfloat* plyVertices();
GLint plyIndexCount();

void plyDisplayDragon(unsigned int dragonID);
