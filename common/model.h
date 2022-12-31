#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H

#include <GL/glew.h>
#include <string>
#include <map>
#include <glm/glm.hpp>
// #include "maths_funcs.h"

static std::vector<unsigned int> VEC_UINT_DEFAUTL_VALUE{};
static std::vector<glm::vec3> VEC_VEC3_DEFAUTL_VALUE{};
static std::vector<glm::vec2> VEC_VEC2_DEFAUTL_VALUE{};
static std::map<std::string, GLuint> MAP_STRING_GLUINT_DEFAULT_VALUE{};
/**
* A very simple .obj loader. Use only for teaching purposes. Use loadOBJWithTiny()
* instead.
*/
struct Skeleton_Node {
    Skeleton_Node* children[20];
    /* key frames */
    vec3* pos_keys; /* array of XYZ positions */
    versor* rot_keys; /* array of quaternion WXYZ rotations */
    vec3* sca_keys; /* array of XYZ scales */
    double* pos_key_times; /* array of times for position keys */
    double* rot_key_times;
    double* sca_key_times;
    int num_pos_keys; /* number of position keys for node */
    int num_rot_keys;
    int num_sca_keys;
    /* existing data */
    char name[64];
    int num_children;
    int bone_index;
};

bool load_mesh(
    const char* file_name,
    GLuint* vao,
    int* point_count,
    mat4* bone_offset_mats,
    int* bone_count,
    Skeleton_Node** root_node,
    double* anim_duration
);

void loadOBJ(
    const std::string& path,
    std::vector<glm::vec3>& vertices,
    std::vector<glm::vec2>& uvs,
    std::vector<glm::vec3>& normals,
    std::vector<unsigned int>& indices = VEC_UINT_DEFAUTL_VALUE
);

/**
* A .vtp loader.
*/
void loadVTP(
    const std::string& path,
    std::vector<glm::vec3>& vertices,
    std::vector<glm::vec2>& uvs,
    std::vector<glm::vec3>& normals,
    std::vector<unsigned int>& indices = VEC_UINT_DEFAUTL_VALUE
);

/**
* An .obj loader that uses tinyobjloader library. Any mesh (quad) is triangulated.
*
* https://github.com/syoyo/tinyobjloader
*/
void loadOBJWithTiny(
    const std::string& path,
    std::vector<glm::vec3>& verticies,
    std::vector<glm::vec2>& uvs,
    std::vector<glm::vec3>& normals,
    std::vector<unsigned int>& indices = VEC_UINT_DEFAUTL_VALUE
);

/**
* Create VBO indexing.
* http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-9-vbo-indexing/
*/
void indexVBO(
    const std::vector<glm::vec3>& in_vertices,
    const std::vector<glm::vec2>& in_uvs,
    const std::vector<glm::vec3>& in_normals,
    std::vector<unsigned int> & out_indices,
    std::vector<glm::vec3> & out_vertices,
    std::vector<glm::vec2> & out_uvs,
    std::vector<glm::vec3> & out_normals
);

class Drawable {
public:
    Drawable(std::string path);

    Drawable(
        const std::vector<glm::vec3>& vertices,
        const std::vector<glm::vec2>& uvs = VEC_VEC2_DEFAUTL_VALUE,
        const std::vector<glm::vec3>& normals = VEC_VEC3_DEFAUTL_VALUE);

    ~Drawable();

    void bind();

    /* Bind VAO before calling draw */
    void draw(int mode = GL_TRIANGLES);

public:
    std::vector<glm::vec3> vertices, normals, indexedVertices, indexedNormals;
    std::vector<glm::vec2> uvs, indexedUVS;
    std::vector<unsigned int> indices;

    GLuint VAO, verticesVBO, uvsVBO, normalsVBO, elementVBO;

private:
    void createContext();
};

/*****************************************************************************/

namespace ogl {
    struct Material {
        glm::vec4 Ka;
        glm::vec4 Kd;
        glm::vec4 Ks;
        float Ns;
        GLuint texKa;
        GLuint texKd;
        GLuint texKs;
        GLuint texNs;
    };

    class Mesh {
    public:
        Mesh(const std::vector<glm::vec3>& vertices,
             const std::vector<glm::vec2>& uvs,
             const std::vector<glm::vec3>& normals,
             const Material& mtl);
        Mesh(const Mesh&) = delete;
        Mesh(Mesh&& other);
        ~Mesh();
        void bind();
        void draw(int mode = GL_TRIANGLES);
    public:
        std::vector<glm::vec3> vertices, normals, indexedVertices, indexedNormals;
        std::vector<glm::vec2> uvs, indexedUVS;
        std::vector<unsigned int> indices;
        Material mtl;
        GLuint VAO, verticesVBO, uvsVBO, normalsVBO, elementVBO;
    private:
        void createContext();
    };

    class Model {
    public:
        using MTLUploadFunction = void(const Material&);
        Model(std::string path, MTLUploadFunction* uploader = nullptr);
        ~Model();
        void draw();
    private:
        std::vector<Mesh> meshes;
        std::map<std::string, GLuint> textures;
        MTLUploadFunction* uploadFunction;
    private:
        void loadOBJWithTiny(const std::string& filename);
        void loadTexture(const std::string& filename);
    };
}

#endif
