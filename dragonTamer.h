#include <GL/gl.h>

void plyLoad(char* filePath);
void plyUnload();

void plyCenter(unsigned int dragonID, float x, float y, float z);
void plyScale(unsigned int dragonID, float factor);
void plyRotate(unsigned int dragonID /* more parameters */);

void plyRotateX(unsigned int dragonID, float angle);
void plyRotateY(unsigned int dragonID, float angle);
void plyRotateZ(unsigned int dragonID, float angle);

unsigned int plyNewDragon();
GLfloat* plyDragonVertices(unsigned int dragonID);
GLfloat* plyDragonNormals(unsigned int dragonID);
void plyDeleteDragon(unsigned int dragonID);

GLint* plyIndices();
GLint plyIndexCount();
