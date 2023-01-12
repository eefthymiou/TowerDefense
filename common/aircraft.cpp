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
#include "util.h"
#include "texture.h"


using namespace glm;
using namespace std;

Aircraft::Aircraft(std::string model_path){
    loadOBJ(model_path,Vertices,UVs,Normals);
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

void Aircraft::bindTexture() {
    glBindTexture(GL_TEXTURE_2D, Texture);
}

void Aircraft::loadTexture(const std::string& filename){
    if (filename.length() == 0) return;
    Texture = loadSOIL(filename.c_str());
}

void Aircraft::update(){
    float size = 0.1f;
    mat4 Translate = glm::translate(mat4(), vec3(0.0f,4.0f,0.0f));
    mat4 Rotate = glm::rotate(mat4(), glm::radians(90.0f), vec3(0.0f,1.0f,0.0f));
    mat4 Scaling = glm::scale(mat4(), vec3(size,size,size));
    modelMatrix = Translate * Rotate * Scaling;
}

void Aircraft::draw() {
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
