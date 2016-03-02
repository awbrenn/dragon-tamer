#include <GL/gl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "utilities.h"

const size_t LINE_SIZE = 512;
const size_t FACE_SIZE = 13;
const size_t DRAGON_MAX = 256;

typedef struct Vertex {
    GLfloat x, y, z;
} Vertex;

Vertex* vertices = NULL;
GLuint* indices = NULL; 
GLuint vertexCount = 0;
GLuint indexCount = 0;

Vertex** dragons = NULL;

void plyLoad(char* filePath) {

    // Populate the array of potential dragons
    dragons = (Vertex**)calloc(DRAGON_MAX, sizeof(Vertex*));
    int i;
    for(i = 0; i < DRAGON_MAX; ++i) dragons[i] = NULL;

    // Open the file
    FILE* file = fopen(filePath, "r");
    char* line = (char*)calloc(LINE_SIZE, sizeof(char));
    char* token = NULL, * delim = " \n\t";

    // Parse through unnecessary header information
    fgets(line, LINE_SIZE, file); // "ply"
    fgets(line, LINE_SIZE, file); // "format binary_little_endian 1.0"
    fgets(line, LINE_SIZE, file); // "comment VCGLIB generated"

    // Parse out the vertex count
    fgets(line, LINE_SIZE, file); // "element vertex 1105352"    
    token = strtok(line, delim); // "element"
    token = strtok(NULL, delim); // "vertex"
    token = strtok(NULL, delim); // "1105352"
    vertexCount = atoi(token);

    // Parse through unnecessary header information
    fgets(line, LINE_SIZE, file); // "property float x"
    fgets(line, LINE_SIZE, file); // "property float y"
    fgets(line, LINE_SIZE, file); // "property float z"

    // Parse out the face count
    fgets(line, LINE_SIZE, file); // "element face 2210673"
    token = strtok(line, delim); // "element"
    token = strtok(NULL, delim); // "face"
    token = strtok(NULL, delim); // "2210673"
    int faceCount = atoi(token);

    // Parse through unnecessary header information
    fgets(line, LINE_SIZE, file); // "property list uchar int vertex_indices"
    fgets(line, LINE_SIZE, file); // "end_header"

    // Read in all of the vertex data
    vertices = (Vertex*)calloc(vertexCount, sizeof(Vertex));
    fread(vertices, sizeof(Vertex), vertexCount, file);

    // Read in all of the face data
    char* faceData = (char*)calloc(faceCount, FACE_SIZE);
    fread(faceData, FACE_SIZE, faceCount, file);

    // Store the vertex indices of the faces
    indexCount = faceCount * 3;
    indices = (GLuint*)calloc(indexCount, sizeof(GLuint));
    for(i = 0; i < faceCount; ++i) {
        if(*(unsigned char*)&faceData[FACE_SIZE*i] != 3) exit(-1);
        indices[i*3 + 0] = *(GLuint *)&faceData[FACE_SIZE*i + 1];
        indices[i*3 + 1] = *(GLuint *)&faceData[FACE_SIZE*i + 5];
        indices[i*3 + 2] = *(GLuint *)&faceData[FACE_SIZE*i + 9];
    }

    free(faceData);
    fclose(file);
}

unsigned int plyNewDragon() {
    // Get the next open dragon ID
    unsigned int dragonID = 0;
    while(dragons[dragonID] != NULL) {
        if(dragonID >= DRAGON_MAX) dieWithError("No available space for new dragons.");
        ++dragonID;
    }

    dragons[dragonID] = (Vertex*)calloc(vertexCount, sizeof(Vertex)); 
    memcpy(dragons[dragonID], vertices, vertexCount*sizeof(Vertex));

    return dragonID;
}

Vertex* plyCheckDragon(unsigned int dragonID) {
    if(dragonID >= DRAGON_MAX) dieWithError("A request was made for a dragon with an out-of-bounds ID.");
    if(dragons[dragonID] == NULL) dieWithError("A request was made for a dragon with invalid ID.");
    return dragons[dragonID];
}

GLfloat* plyGetDragon(unsigned int dragonID) {
    return (GLfloat*) plyCheckDragon(dragonID);
}

void plyDeleteDragon(unsigned int dragonID) {
    Vertex* dragon = dragons[dragonID];

    free(dragon);
    dragon = NULL;
}

void plyUnload() {
    free(vertices);
    free(indices);

    vertices = NULL;
    indices = NULL;

    indexCount = 0;
    vertexCount = 0;

    int i;
    for(i = 0; i < DRAGON_MAX; ++i) plyDeleteDragon(i);
}

void plyScale(unsigned int dragonID, GLfloat factor) {
    // Attempt to recall the dragon
    Vertex* dragon = plyCheckDragon(dragonID);

    float maxDim = 0.0f;
    int i;
    for(i = 0; i < vertexCount; ++i) {
        if(maxDim < fabs(dragon[i].x)) maxDim = fabs(dragon[i].x);
        if(maxDim < fabs(dragon[i].y)) maxDim = fabs(dragon[i].y);
        if(maxDim < fabs(dragon[i].z)) maxDim = fabs(dragon[i].z);
    }
    for(i = 0; i < vertexCount; ++i) {
        dragon[i].x /= maxDim;
        dragon[i].y /= maxDim;
        dragon[i].z /= maxDim;
        
        dragon[i].x *= factor;
        dragon[i].y *= factor;
        dragon[i].z *= factor;
    }
}

void plyCenter(unsigned int dragonID, GLfloat x, GLfloat y, GLfloat z) {
    // Attempt to recall the dragon
    Vertex* dragon = plyCheckDragon(dragonID);

    // Translate the dragon to the origin
    GLfloat xOffset = 0.0f;
    GLfloat yOffset = 0.0f;
    GLfloat zOffset = 0.0f;

    int i;
    for(i = 0; i < vertexCount; ++i) {
        xOffset += dragon[i].x;
        yOffset += dragon[i].y;
        zOffset += dragon[i].z;
    }
    
    xOffset /= vertexCount;
    yOffset /= vertexCount;
    zOffset /= vertexCount;
    
    for(i = 0; i < vertexCount; ++i) {
        dragon[i].x -= xOffset - x;
        dragon[i].y -= yOffset - y;
        dragon[i].z -= zOffset - z;
    }
}

void plyRotate(unsigned int dragonID /* more parameters */) {
    // Attempt to recall the dragon
    Vertex* dragon = plyCheckDragon(dragonID);
}

GLuint* plyIndices() { return indices; }
GLuint plyIndexCount() { return indexCount; }
GLfloat* plyVertices() { return (GLfloat*)vertices; }

void plyDisplayDragon(unsigned int dragonID) {
    Vertex* dragon = plyCheckDragon(dragonID);

    int i;
    for(i = 0; i < vertexCount; ++i) printf("{ %f, %f, %f }\n", dragon[i].x, dragon[i].y, dragon[i].z);
}
