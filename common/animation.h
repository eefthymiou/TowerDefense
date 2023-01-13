#ifndef ANIMATION_H
#define ANIMATION_H

//load a file with AssImp
#include <assimp/cimport.h> // C importer
#include <assimp/scene.h> // collects data
#include <assimp/postprocess.h> // various extra operations
#include <stdlib.h> // memory management
#include <assert.h>
#include <GL/glew.h>
#include <string>
#include <map>
#include <glm/glm.hpp>
#include "maths_funcs.h"
#define MAX_BONES 20
#define _USE_MATH_DEFINES

struct Skeleton_Node {
    Skeleton_Node* children[20];
    /* key frames */
    my_vec3* pos_keys; /* array of XYZ positions */
    versor* rot_keys; /* array of quaternion WXYZ rotations */
    my_vec3* sca_keys; /* array of XYZ scales */
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

class Animation {
public:
    Animation(const char* path);
    ~Animation();

public:
    GLuint vao;
    int point_count = 0;
    int bone_count = 0;
    double anim_duration = 0.0;
    Skeleton_Node* root_node = NULL;
    my_mat4 g_local_anims[MAX_BONES];
    my_mat4 bone_animation_mats[MAX_BONES];
    my_mat4 bone_offset_matrices[MAX_BONES];
    glm::mat4 modelMatrix;
    GLuint Texture;

    glm::vec3 get_root(glm::vec3 prev_translation);

    void skeleton_animate(
        Skeleton_Node* node,
        double anim_time,
        my_mat4 parent_mat,
        my_mat4* bone_offset_mats,
        my_mat4* bone_animation_mats);

    void bind();
    void update();
    void draw();
    void loadTexture(const std::string& filename);
    void bindTexture();
};

#endif
