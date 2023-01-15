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
#include <common/maths_funcs.h>
#include "moving_object.h"

using namespace glm;
using namespace std;

Moving_obj::Moving_obj(vec3 pos,vec3 vel,float mass,vec3 t)
    : RigidBody() {
    m = mass;
    x = pos;
    v = vel;
    P = m * v;
    target = t;
    initial_pos = pos;
    maxspeed = 1.0f;
    maxforce = 2.0f;
    moving = true;
    arrives = true;
}


Moving_obj::~Moving_obj() {
    glDeleteBuffers(1, &VAO);
    glDeleteBuffers(1, &verticesVBO);
    glDeleteBuffers(1, &UVVBO);
}

void Moving_obj::load_mesh(string model_path){
    loadOBJ(model_path,Vertices,UVs,Normals);
    createContext();
}



float Moving_obj::map(float x, float in_min, float in_max, float out_min, float out_max) {
return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


void Moving_obj::bind() {
    glBindVertexArray(VAO);
}

void Moving_obj::bindTexture() {
    glBindTexture(GL_TEXTURE_2D, Texture);
}

void Moving_obj::loadTexture(const std::string& filename){
    if (filename.length() == 0) return;
    Texture = loadSOIL(filename.c_str());
}

void Moving_obj::update(float t, float dt, float size) {
    //integration
    advanceState(t, dt);

    // compute model matrix
    vec3 v1 = v;
    v1.y = 0;
    vec3 direction = normalize(v1);
    float angle_x = acos(dot(glm::vec3(1.0f, 0.0f, 0.0f), direction));
    vec3 axis_x = cross(vec3(1.0f, 0.0f, 0.0f), direction);
    mat4 rotation_x = rotate(glm::mat4(1.0f), angle_x, axis_x);
    mat4 tranlation = translate(mat4(), vec3(x.x, x.y, x.z));
    mat4 scale = glm::scale(mat4(), vec3(size, size, size));
    mat4 rotation = glm::rotate(mat4(), glm::radians(180.0f), vec3(0.0f,1.0f,0.0f));
    modelMatrix = tranlation * rotation * rotation_x * scale;
}

void Moving_obj::draw() {
    glDrawArrays(GL_TRIANGLES, 0, Vertices.size());
}

void Moving_obj::createContext() {
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
