// Include C++ headers
#include <iostream>
#include <string>
#include <vector>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

// Shader loading utilities and other
#include <common/shader.h>
#include <common/util.h>
#include <common/camera.h>
#include <common/model.h>
#include <common/texture.h>

#include "aircraft.h"

using namespace glm;
using namespace std;

Aircraft::Aircraft(string model_path,string texture_path){
    loadOBJ(model_path,Vertices, UVs,Normals);
    // Texture = loadSOIL(texture_path);
    createContext();
}

Aircraft::~Aircraft() {
    glDeleteBuffers(1, &VAO);
    glDeleteBuffers(1, &verticesVBO);
    glDeleteBuffers(1, &UVVBO);
}


void Aircraft::bind() {
    glBindVertexArray(VAO);
}

void Aircraft::draw() {
    // float size = 0.1f; 
    // // Rotate = glm::rotate(mat4(), )
    // mat4 aircraftModelMatrix = mat4(1.0f);
    // mat4 aircraftMVP = projectionMatrix * viewMatrix * aircraftModelMatrix;
    // glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, &aircraftMVP[0][0]);
    // glActiveTexture(GL_TEXTURE2);
    // glBindTexture(GL_TEXTURE_2D, Texture);
    // glUniform1i(textureSampler, 2); 
    glDrawArrays(GL_TRIANGLES, 0, Vertices.size());
}

void Aircraft::createContext() {
    // VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // vertex VBO
    glGenBuffers(1, &verticesVBO);
    glBindBuffer(GL_ARRAY_BUFFER, verticesVBO);
    glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(glm::vec3),
                 &Vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    // uvs VBO
    glGenBuffers(1, &UVVBO);
    glBindBuffer(GL_ARRAY_BUFFER, UVVBO);
    glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(glm::vec2),&UVs[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);

}
