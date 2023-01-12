#ifndef AIRCRAFT_H
#define AIRCRAFT_H

#include <GL/glew.h>
#include <string>
#include <map>
#include <glm/glm.hpp>

class Aircraft {
public:
    Aircraft(std::string mesh_path);
    ~Aircraft();

    void loadTexture(const std::string& filename);
    void bind();
    void update();
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
