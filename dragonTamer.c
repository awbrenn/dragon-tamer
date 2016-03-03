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
Vertex* normals = NULL;
GLuint* indices = NULL; 
GLuint vertexCount = 0;
GLuint indexCount = 0;

Vertex** dragonVertices = NULL;
Vertex** dragonNormals = NULL;

void plyCalculateNormals() {
    normals = (Vertex*)calloc(vertexCount, sizeof(Vertex));

    int i;
    for(i = 0; i < indexCount; i += 3) {
        Vertex v0 = vertices[indices[i+0]];
        Vertex v1 = vertices[indices[i+1]];
        Vertex v2 = vertices[indices[i+2]];
        
        Vertex u;
        u.x = v0.x - v2.x;
        u.y = v0.y - v2.y;
        u.z = v0.z - v2.z;

        Vertex v;
        v.x = v1.x - v2.x;
        v.y = v1.y - v2.y;
        v.z = v1.z - v2.z;

        Vertex ucrossv;
        ucrossv.x = u.y*v.z - u.z*v.y;
        ucrossv.y = u.z*v.x - u.x*v.z;
        ucrossv.z = u.x*v.y - u.y*v.x;
        
    double length = 0.;
        length += ucrossv.x * ucrossv.x;
        length += ucrossv.y * ucrossv.y;
        length += ucrossv.z * ucrossv.z;
        length = sqrt(length);

        ucrossv.x /= length;
        ucrossv.y /= length;
        ucrossv.z /= length;

        // Assign the normals
        normals[indices[i+0]] = ucrossv;
        normals[indices[i+1]] = ucrossv;
        normals[indices[i+2]] = ucrossv;
    }
}

void plyLoad(char* filePath) {

    // Populate the array of potential dragonVertices
    dragonVertices = (Vertex**)calloc(DRAGON_MAX, sizeof(Vertex*));
    int i;
    for(i = 0; i < DRAGON_MAX; ++i) dragonVertices[i] = NULL;
    
    // Populate the array of potential dragonVertices
    dragonNormals = (Vertex**)calloc(DRAGON_MAX, sizeof(Vertex*));
    for(i = 0; i < DRAGON_MAX; ++i) dragonNormals[i] = NULL;

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

    plyCalculateNormals();
}

unsigned int plyNewDragon() {
    // Get the next open dragon ID
    unsigned int dragonID = 0;
    while(dragonVertices[dragonID] != NULL) {
        if(dragonID >= DRAGON_MAX) dieWithError("No available space for new dragonVertices.");
        ++dragonID;
    }

    dragonVertices[dragonID] = (Vertex*)calloc(vertexCount, sizeof(Vertex)); 
    memcpy(dragonVertices[dragonID], vertices, vertexCount*sizeof(Vertex));

    dragonNormals[dragonID] = (Vertex*)calloc(vertexCount, sizeof(Vertex));
    memcpy(dragonNormals[dragonID], normals, vertexCount*sizeof(Vertex));

    return dragonID;
}

void plyCheckDragon(unsigned int dragonID) {
    if(dragonID >= DRAGON_MAX) dieWithError("A request was made for a dragon with an out-of-bounds ID.");
    if(dragonVertices[dragonID] == NULL) dieWithError("A request was made for a dragon with invalid ID.");
}

GLfloat* plyDragonVertices(unsigned int dragonID) {
    plyCheckDragon(dragonID);
    return (GLfloat*)dragonVertices[dragonID];
}
GLfloat* plyDragonNormals(unsigned int dragonID) {
    plyCheckDragon(dragonID);
    return (GLfloat*)dragonNormals[dragonID];
}

void plyDeleteDragon(unsigned int dragonID) {
    Vertex* vertexData = dragonVertices[dragonID];
    Vertex* normalData = dragonNormals[dragonID];

    free(vertexData);
    free(normalData);
    dragonVertices[dragonID] = NULL;
    dragonNormals[dragonID] = NULL;
}

void plyUnload() {
    free(normals);
    free(vertices);
    free(indices);

    vertices = NULL;
    indices = NULL;

    indexCount = 0;
    vertexCount = 0;

    int i;
    for(i = 0; i < DRAGON_MAX; ++i) plyDeleteDragon(i);
    free(dragonVertices);
    free(dragonNormals);
}

void plyScale(unsigned int dragonID, GLfloat factor) {
    // Attempt to recall the dragon
    plyCheckDragon(dragonID);
    Vertex* dragon = (Vertex*)plyDragonVertices(dragonID);

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
    plyCheckDragon(dragonID);
    Vertex* dragon = (Vertex*)plyDragonVertices(dragonID);

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

void plyRotateX(unsigned int dragonID, float angle) {
    // Attempt to recall the dragon
    plyCheckDragon(dragonID);
    Vertex* vertexData = (Vertex*)dragonVertices[dragonID];
    Vertex* normalData = (Vertex*)dragonNormals[dragonID];

    int i;
    for(i = 0; i < vertexCount; ++i) {
        Vertex vertTemp = vertexData[i];
        vertexData[i].x = vertTemp.x*1 + vertTemp.y*0          + vertTemp.z*0;
        vertexData[i].y = vertTemp.x*0 + vertTemp.y*cos(angle) - vertTemp.z*sin(angle);
        vertexData[i].z = vertTemp.x*0 + vertTemp.y*sin(angle) + vertTemp.z*cos(angle);
        
        Vertex normTemp = normalData[i];
        normalData[i].x = normTemp.x*1 + normTemp.y*0          + normTemp.z*0;
        normalData[i].y = normTemp.x*0 + normTemp.y*cos(angle) - normTemp.z*sin(angle);
        normalData[i].z = normTemp.x*0 + normTemp.y*sin(angle) + normTemp.z*cos(angle);
    }
}
void plyRotateY(unsigned int dragonID, float angle) {
    // Attempt to recall the dragon
    plyCheckDragon(dragonID);
    Vertex* vertexData = dragonVertices[dragonID];
    Vertex* normalData = dragonNormals[dragonID];

    int i;
    for(i = 0; i < vertexCount; ++i) {
        Vertex vertTemp = vertexData[i];    
        vertexData[i].x =  vertTemp.x*cos(angle) + vertTemp.y*0 + vertTemp.z*sin(angle);
        vertexData[i].y =  vertTemp.x*0          + vertTemp.y*1 + vertTemp.z*0;
        vertexData[i].z = -vertTemp.x*sin(angle) + vertTemp.y*0 + vertTemp.z*cos(angle);
        
        Vertex normTemp = normalData[i];    
        normalData[i].x =  normTemp.x*cos(angle) + normTemp.y*0 + normTemp.z*sin(angle);
        normalData[i].y =  normTemp.x*0          + normTemp.y*1 + normTemp.z*0;
        normalData[i].z = -normTemp.x*sin(angle) + normTemp.y*0 + normTemp.z*cos(angle);
    }
}
void plyRotateZ(unsigned int dragonID, float angle) {
    // Attempt to recall the dragon
    plyCheckDragon(dragonID);
    Vertex* vertexData = dragonVertices[dragonID];
    Vertex* normalData = dragonNormals[dragonID];

    int i;
    for(i = 0; i < vertexCount; ++i) {
        Vertex vertTemp = vertexData[i];       
        vertexData[i].x = vertTemp.x*cos(angle) - vertTemp.y*sin(angle) + vertTemp.z*0;
        vertexData[i].y = vertTemp.x*sin(angle) + vertTemp.y*cos(angle) + vertTemp.z*0;
        vertexData[i].z = vertTemp.x*0          + vertTemp.y*0          + vertTemp.z*1;
        
        Vertex normTemp = normalData[i];    
        normalData[i].x =  normTemp.x*cos(angle) + normTemp.y*0 + normTemp.z*sin(angle);
        normalData[i].y =  normTemp.x*0          + normTemp.y*1 + normTemp.z*0;
        normalData[i].z = -normTemp.x*sin(angle) + normTemp.y*0 + normTemp.z*cos(angle);
    }
}

GLuint* plyIndices() { return indices; }
GLuint plyIndexCount() { return indexCount; }
