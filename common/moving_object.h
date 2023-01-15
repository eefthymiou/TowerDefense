#ifndef MOVING_OBJECT_H
#define MOVING_OBJECT_H

#include <GL/glew.h>
#include <string>
#include <map>
#include <glm/glm.hpp>
#include "RigidBody.h"

class Moving_obj :  public RigidBody {
public:
    float maxspeed,maxforce;
    glm::vec3 target;
    bool moving;
    bool arrives;

    Moving_obj(glm::vec3 pos,glm::vec3 vel,float mass,glm::vec3 t);
    ~Moving_obj();

    void load_mesh(std::string mesh_path);
    void loadTexture(const std::string& filename);
    void bind();
    glm::vec3 seek();
    void update(float t, float dt, float size);
    void bindTexture();
    void draw();

public:
    std::vector<glm::vec3> Vertices, Normals;
    std::vector<glm::vec2> UVs;
    glm::mat4 modelMatrix;

    glm::vec3 initial_pos;
    GLuint VAO, verticesVBO, UVVBO;
    GLuint Texture;

private:
    void createContext();
    float map(float x, float in_min, float in_max, float out_min, float out_max);
};



#endif