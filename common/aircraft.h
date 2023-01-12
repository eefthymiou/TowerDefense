#ifndef AIRCRAFT_H
#define AIRCRAFT_H

#include <GL/glew.h>
#include <string>
#include <map>
#include <glm/glm.hpp>
#include "RigidBody.h"

class Aircraft :  public RigidBody {
public:
    float maxspeed,maxforce,size;
    glm::vec3 acceleration,target;

    Aircraft(std::string mesh_path, glm::vec3 pos, glm::vec3 vel, float mass, glm::vec3 t);
    ~Aircraft();

    void loadTexture(const std::string& filename);
    void bind();
    glm::vec3 seek();
    void applyForce(glm::vec3 force);
    void update(float t, float dt);
    void bindTexture();
    void draw();

public:
    std::vector<glm::vec3> Vertices, Normals;
    std::vector<glm::vec2> UVs;
    glm::mat4 modelMatrix;

    GLuint VAO, verticesVBO, UVVBO;
    GLuint Texture;

private:
    void createContext();
};

#endif
