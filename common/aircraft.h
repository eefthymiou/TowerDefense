#ifndef AIRCRAFT_H
#define AIRCRAFT_H

#include <GL/glew.h>
#include <string>
#include <map>
#include <glm/glm.hpp>

class Aircraft {
public:
    Aircraft(
        std::string mesh_path,
        std::string texture_path
    );
    ~Aircraft();

    void bind();
    void draw();

public:
    std::vector<glm::vec3> Vertices, Normals;
    std::vector<glm::vec2> UVs;

    GLuint VAO, verticesVBO, UVVBO, Texture;

private:
    void createContext();
};

#endif
