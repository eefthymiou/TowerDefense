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

Aircraft::Aircraft(string model_path,vec3 pos,vec3 vel,float mass,vec3 t)
    : RigidBody() {
    loadOBJ(model_path,Vertices,UVs,Normals);

    m = mass;
    x = pos;
    v = vel;
    P = m * v;
    target = t;
    acceleration = vec3(0.0f,0.0f,0.0f);
    maxspeed = 10.0f;
    maxforce = 5.0f;
    size = 0.08f;
    createContext();
}

Aircraft::~Aircraft() {
    glDeleteBuffers(1, &VAO);
    glDeleteBuffers(1, &verticesVBO);
    glDeleteBuffers(1, &UVVBO);
}

void Aircraft::applyForce(vec3 force) {
    acceleration = acceleration + force;
}

vec3 Aircraft::seek(){
    vec3 desired = target-x;
    desired = glm::normalize(desired) * maxspeed;
    vec3 steer = desired - v;
    steer = glm::clamp(steer, -maxforce, maxforce);
    return steer;
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


void Aircraft::update(float t, float dt) {
    //integration
    advanceState(t, dt);

    // compute model matrix
    mat4 scale = glm::scale(mat4(), vec3(size, size, size));
    mat4 tranlation = translate(mat4(), vec3(x.x, x.y, x.z));
    mat4 rotation = glm::rotate(mat4(), glm::radians(180.0f), vec3(0.0f,1.0f,0.0f));
    
    vec3 v1 = v;
    v1.y = 0;
    vec3 direction = normalize(v1);
    if (direction!=vec3(0.0f,0.0f,0.0f)){
        float angle_x = acos(dot(glm::vec3(1.0f, 0.0f, 0.0f), direction));
        vec3 axis_x = cross(vec3(1.0f, 0.0f, 0.0f), direction);
        mat4 rotation_x = rotate(glm::mat4(1.0f), angle_x, axis_x);
        mat4 tranlation = translate(mat4(), vec3(x.x, x.y, x.z));
        mat4 rotation = glm::rotate(mat4(), glm::radians(180.0f), vec3(0.0f,1.0f,0.0f));  
        modelMatrix = tranlation * rotation * rotation_x * scale;
    }
    else modelMatrix = tranlation * rotation * scale;
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
