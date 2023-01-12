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
#include <common/aircraft.h>
// #include "common/gl_utils.h"

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>


using namespace std;
using namespace glm;

// Function prototypes
void initialize();
void createContext();
void mainLoop();
void free();

#define W_WIDTH 1024
#define W_HEIGHT 768
#define TITLE "Tower Defense"

// Global variables
GLFWwindow* window;
Camera* camera;
GLuint shaderProgram;
GLuint assimp_shader;
GLuint gridshader;
GLuint MVPLocation;
GLuint gMVPLocation;
GLuint assimpMVPLocation;
GLuint translationsLocation;
GLuint textureSampler;
GLuint gtextureSampler;
GLuint texture,ninjatexture;
GLuint quadtexture;
GLuint movingTexture;
GLuint movingTextureSampler;
GLuint displacementTexture;
GLuint displacementTextureSampler;
GLuint timeUniform;
GLuint VAO;
GLuint quadVAO, ninjaVAO;
GLuint quadVerticiesVBO, ninjaVerticiesVBO, ninjaUVVBO;
GLuint VerticiesVBO, UVVBO;
GLuint quadUVVBO;
GLuint sphereVAO, sphereTexture,sphereUVVBO, sphereVerticiesVBO;
GLuint cubeVAO, cubeVerticiesVBO;
GLuint aircraftVAO,aircraftTexture,aircraftUVVBO,aircraftVerticiesVBO;
std::vector<vec3> Vertices, Normals, ninjaVertices, ninjaNormals, sphereVertices, sphereNormals;
std::vector<vec2> UVs, ninjaUVs,sphereUVs;
std::vector<vec2> quadUVs;

std::vector<vec3> aircraftVertices, aircraftNormals;
std::vector<vec2> aircraftUVs;
std::vector<vec3> cubeVertices, cubeNormals;
std::vector<vec2> cubeUVs;


GLuint useTextureLocation;

//load a file with AssImp
#include <assimp/cimport.h> // C importer
#include <assimp/scene.h> // collects data
#include <assimp/postprocess.h> // various extra operations
#include <stdlib.h> // memory management
#include <assert.h>
#define MESH_FILE "../Models/monkey_with_anim.dae" // file to load ...
#define MESH_FILE "../Models/hwgrunt.dae" // file to load ...
#define MESH_FILE "../Models/my_model.dae" // file to load ...
#define TEXTURE_FILE "../Models/Ninja_T.png"
#define MAX_BONES 20
#define _USE_MATH_DEFINES

/* load the mesh using assimp */

GLuint monkey_vao;
int monkey_point_count = 0;
int monkey_bone_count = 0;
double monkey_anim_duration = 0.0;
int bone_matrices_locations[MAX_BONES];
Skeleton_Node* monkey_root_node = NULL;
my_mat4 g_local_anims[MAX_BONES];
my_mat4 monkey_bone_animation_mats[MAX_BONES];
my_mat4 monkey_bone_offset_matrices[MAX_BONES];
GLuint model_mat_location;
GLuint view_mat_location;
GLuint proj_mat_location;

// aircraft
Aircraft* first_aircraft;


void createContext() {
    // Create and compile our GLSL program from the shaders
    shaderProgram = loadShaders("../shaders/texture.vertexshader", "../shaders/texture.fragmentshader");

    // Draw wire frame triangles or fill: GL_LINE, or GL_FILL
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Get a pointer location to model matrix in the vertex shader
    MVPLocation = glGetUniformLocation(shaderProgram, "MVP");
    textureSampler = glGetUniformLocation(shaderProgram, "textureSampler");

    // loadOBJ("../OBJ_files/Tower.obj",
    //     Vertices, 
    //     UVs,
    //     Normals);

    loadOBJ("../OBJ_files/Plasma.obj",
        Vertices, 
        UVs,
        Normals);
    
    // VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // vertex VBO
    glGenBuffers(1, &VerticiesVBO);
    glBindBuffer(GL_ARRAY_BUFFER, VerticiesVBO);
    glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(glm::vec3),
                 &Vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);


    // use texture
    // texture = loadSOIL("../Textures/menara_kl_tex/T_menara_kl_DIFF.jpg");
    texture = loadSOIL("../Textures/Maps/Bakedtexture.png");
    
    // uvs VBO
    glGenBuffers(1, &UVVBO);
    glBindBuffer(GL_ARRAY_BUFFER, UVVBO);
    glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(glm::vec2),&UVs[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);

    // cube
    loadOBJ("../OBJ_files/cube.obj",
        cubeVertices, 
        cubeUVs,
        cubeNormals);

    // VAO
    glGenVertexArrays(1, &cubeVAO);
    glBindVertexArray(cubeVAO);

    // vertex VBO
    glGenBuffers(1, &cubeVerticiesVBO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVerticiesVBO);
    glBufferData(GL_ARRAY_BUFFER, cubeVertices.size() * sizeof(glm::vec3),
                 &cubeVertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    // sphere
    loadOBJ("../OBJ_files/sphere.obj",
        sphereVertices, 
        sphereUVs,
        sphereNormals);

    // VAO
    glGenVertexArrays(1, &sphereVAO);
    glBindVertexArray(sphereVAO);

    // vertex VBO
    glGenBuffers(1, &sphereVerticiesVBO);
    glBindBuffer(GL_ARRAY_BUFFER, sphereVerticiesVBO);
    glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(glm::vec3),
                 &sphereVertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    sphereTexture = loadSOIL("../Textures/2k_jupiter.jpg");

    // uvs VBO
    glGenBuffers(1, &sphereUVVBO);
    glBindBuffer(GL_ARRAY_BUFFER, sphereUVVBO);
    glBufferData(GL_ARRAY_BUFFER, sphereUVs.size() * sizeof(glm::vec2),&sphereUVs[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);

    // aircraft
    vec3 position = vec3(0.0f,2.0f,0.0f);
    vec3 vel = vec3(1.0f,5.0f,1.0f);
    vec3 target = vec3(14.0f,2.0f,14.0f);
    float mass = 2.0f;
    first_aircraft = new Aircraft("../OBJ_files/aircraft.obj",position,vel,mass,target);
    first_aircraft->loadTexture("../Textures/aircraft/aircrafttank_DefaultMaterial_BaseColor.png");
    
    // assimp
    for ( int i = 0; i < MAX_BONES; i++ ) {
        monkey_bone_animation_mats[i]  = identity_mat4();
        monkey_bone_offset_matrices[i] = identity_mat4();
    }
    
    load_mesh(
         MESH_FILE, 
         &monkey_vao, 
         &monkey_point_count, 
         monkey_bone_offset_matrices, 
         &monkey_bone_count, 
         &monkey_root_node,
         &monkey_anim_duration 
    );

    assimp_shader = loadShaders("../shaders/assimp.vertexshader", "../shaders/assimp.fragmentshader");
    model_mat_location = glGetUniformLocation(assimp_shader, "model");
    view_mat_location = glGetUniformLocation(assimp_shader, "view");
    proj_mat_location = glGetUniformLocation(assimp_shader, "proj");
    printf ("monkey bone count %i\n", monkey_bone_count);

    char name[64];
    for ( int i = 0; i < MAX_BONES; i++ ) {
        sprintf( name, "bone_matrices[%i]", i );
        bone_matrices_locations[i] = glGetUniformLocation( assimp_shader, name );
        glUniformMatrix4fv( bone_matrices_locations[i], 1, GL_FALSE, identity_mat4().m );
    }

    // use texture
    // ninjatexture = loadSOIL("../Models/Ninja_T.png");
    
    gridshader = loadShaders("../shaders/grid.vertexshader", "../shaders//grid.fragmentshader");

    gMVPLocation = glGetUniformLocation(gridshader, "MVP");
    translationsLocation = glGetUniformLocation(gridshader, "offsets");
    gtextureSampler = glGetUniformLocation(gridshader, "textureSampler");

    //grid 
    const GLfloat quadVertices[] = {
        // positions     
        -0.5f,  0.5f, 0.00f,
        0.5f, 0.5f, 0.00f,
        -0.5f, -0.5f, 0.00f,

        0.5f,  0.5f, 0.00f,
        0.5f, -0.5f,  0.00f, 
        -0.5f,  -0.5f, 0.00f	 		
    };

    const GLfloat quadUVs[] = {
        0.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 0.0f,

        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f
    };

    // VAO
    glGenVertexArrays(1, &quadVAO);
    glBindVertexArray(quadVAO);

    // vertex VBO
    glGenBuffers(1, &quadVerticiesVBO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVerticiesVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);


    // load texture for quad
    // quadtexture = loadSOIL("../Textures/floor_grass.jpg");
    // quadtexture = loadBMP("../BML_files/lava.bmp");
    quadtexture = loadSOIL("../Textures/1.png");

    // uvs VBO
    glGenBuffers(1, &quadUVVBO);
    glBindBuffer(GL_ARRAY_BUFFER, quadUVVBO);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(glm::vec2),&quadUVs[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

}


void skeleton_animate (
    Skeleton_Node* node,
    double anim_time,
    my_mat4 parent_mat,
    my_mat4* bone_offset_mats,
    my_mat4* bone_animation_mats) {

    assert (node);
    /* the animation of a node after inheriting its parent's animation */
    my_mat4 our_mat = parent_mat;
    /* the animation for a particular bone at this time */
    my_mat4 local_anim = identity_mat4 ();

    my_mat4 node_T = identity_mat4 ();
    if (node->num_pos_keys > 0) {
        int prev_key = 0;
        int next_key = 0;
        for (int i = 0; i < node->num_pos_keys - 1; i++) {
            prev_key = i;
            next_key = i + 1;
            if (node->pos_key_times[next_key] >= anim_time) {
                break;
            }
        } // endfor
        float total_t = node->pos_key_times[next_key] - node->pos_key_times[prev_key];
        float t = (anim_time - node->pos_key_times[prev_key]) / total_t;
        my_vec3 vi = node->pos_keys[prev_key];
        my_vec3 vf = node->pos_keys[next_key];
        my_vec3 lerped = vi * (1.0f - t) + vf * t;
        node_T = translate (identity_mat4 (), lerped);
    } // endif num_pos_keys > 0

    my_mat4 node_R = identity_mat4 ();
    if (node->num_rot_keys > 0) {
        // find next and previous keys
        int prev_key = 0;
        int next_key = 0;
        for (int i = 0; i < node->num_rot_keys - 1; i++) {
            prev_key = i;
            next_key = i + 1;
            if (node->rot_key_times[next_key] >= anim_time) break;
        }
        float total_t = node->rot_key_times[next_key] - node->rot_key_times[prev_key];
        float t = (anim_time - node->rot_key_times[prev_key]) / total_t;
        versor qi = node->rot_keys[prev_key];
        versor qf = node->rot_keys[next_key];
        versor slerped = slerp (qi, qf, t);
        node_R = quat_to_mat4 (slerped);
    } // endif num_rot_keys > 0

    local_anim = node_T * node_R;

    // if node has a weighted bone...
    int bone_i = node->bone_index;
    if (bone_i > -1) {
        // ... then get offset matrices
        my_mat4 bone_offset = bone_offset_mats[bone_i];
        // no more inverse
        our_mat = parent_mat * local_anim;
        bone_animation_mats[bone_i] = parent_mat * local_anim * bone_offset;
    } // endif
    for (int i = 0; i < node->num_children; i++) {
        skeleton_animate (
            node->children[i],
            anim_time,
            our_mat,
            bone_offset_mats,
            bone_animation_mats
    );
    } // endfor
}

void free() {
    glDeleteBuffers(1, &VerticiesVBO);
    glDeleteBuffers(1, &UVVBO);
    glDeleteTextures(1, &texture);
    glDeleteVertexArrays(1, &VAO);

    // quad
    glDeleteBuffers(1, &quadVerticiesVBO);
    glDeleteBuffers(1, &quadUVVBO);
    glDeleteTextures(1, &quadtexture);
    glDeleteVertexArrays(1, &quadVAO);

    glDeleteProgram(shaderProgram);
    glDeleteProgram(gridshader);
    glDeleteProgram(assimp_shader);
    glfwTerminate();
}

vec3 get_root(vec3 prev_translation) {
    glm::vec3 translation;
    if (prev_translation.x<=18.0f){
        translation.x = prev_translation.x += 0.01;
        translation.y = 2.0f;
        translation.z = prev_translation.z += 0.01;
    }
    else translation=prev_translation;
    return translation;
}

void mainLoop() {
    glm::vec3 translations[100];
    int index = 0;
    float offset = 5.0f;
    for(int z = -10; z <= 10; z += 2){
        for(int x = -10; x < 10; x += 2){
            glm::vec3 translation;
            translation.x = (float)x / 2.0f + offset ;
            translation.z = (float)z / 2.0f + offset ;
            translation.y = 0;
            // std::cout << index << glm::to_string(translation) << std::endl;
            translations[index++] = translation;
        }
    }    
    double anim_time = 0.0;
    mat4 projectionMatrix,viewMatrix;
    mat4 Scaling,Rotate,Translate;
    vec3 prev_translation = vec3(0.0f,0.0f,0.0f);

    float t = glfwGetTime();

    do {
        static double previous_seconds = glfwGetTime();
        double current_seconds         = glfwGetTime();
        double elapsed_seconds         = current_seconds - previous_seconds;
        previous_seconds               = current_seconds;

        anim_time += elapsed_seconds * 200.0;
        if ( anim_time >= monkey_anim_duration ) { anim_time = monkey_anim_duration - anim_time; }

        float currentTime = glfwGetTime();
        float dt = currentTime - t;
        // float dt = 0.001;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        camera->update();
        projectionMatrix = camera->projectionMatrix;
        viewMatrix = camera->viewMatrix;

        // use grid shader
        glUseProgram(gridshader);
        glBindVertexArray(quadVAO);
        float size = 2.0f;
        mat4 quadScaling = glm::scale(mat4(), vec3(size,0.0f,size));
        mat4 quadRotate = glm::rotate(mat4(),glm::radians(90.0f),vec3(1.0f,0.0f,0.0f));
        mat4 quadModelMatrix = quadScaling * quadRotate;
        mat4 quadMVP = projectionMatrix * viewMatrix * quadModelMatrix;
        glUniformMatrix4fv(gMVPLocation, 1, GL_FALSE, &quadMVP[0][0]);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, quadtexture);
        glUniform1i(gtextureSampler, 0);   
        glUniform3fv(translationsLocation, 100, &translations[0].z); 
        glDrawArraysInstanced(GL_TRIANGLES, 0, 2*3, 100);

        // use shaderProgram
        // First Tower
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO); // bind building vao
        size = 0.02f;
        Scaling = glm::scale(mat4(), vec3(size,size,size));
        Translate = glm::translate(mat4(), vec3(2.0f,0.0f,2.0f));
        mat4 Tower1ModelMatrix = Translate * Scaling;
        mat4 Tower1MVP = projectionMatrix * viewMatrix * Tower1ModelMatrix;
        glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, &Tower1MVP[0][0]);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(textureSampler, 0); 
        glDrawArrays(GL_TRIANGLES, 0, Vertices.size());
        // Second Tower
        size = 0.02f;
        Scaling = glm::scale(mat4(), vec3(size,size,size));
        Translate = glm::translate(mat4(), vec3(16.0f,0.0f,16.0f));
        mat4 Tower2ModelMatrix = Translate * Scaling;
        mat4 Tower2MVP = projectionMatrix * viewMatrix * Tower2ModelMatrix;
        glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, &Tower2MVP[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, Vertices.size());

        // Sphere
        glBindVertexArray(sphereVAO);
        size = 20.0f;
        Scaling = glm::scale(mat4(), vec3(size,size,size));
        Translate = glm::translate(mat4(), vec3(10.0f,0.0f,10.0f));
        mat4 sphereModelMatrix = Translate * Scaling;
        mat4 sphereMVP = projectionMatrix * viewMatrix * sphereModelMatrix;
        glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, &sphereMVP[0][0]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, sphereTexture);
        glUniform1i(textureSampler, 1); 
        glDrawArrays(GL_TRIANGLES, 0, sphereVertices.size());

        // cube 1
        glBindVertexArray(cubeVAO);
        size = 2.5f;
        Scaling = glm::scale(mat4(), vec3(size,4.0*size,size));
        Translate = glm::translate(mat4(), vec3(2.0f,0.0f,2.0f));
        mat4 cube1ModelMatrix = Translate * Scaling;
        mat4 cube1MVP = projectionMatrix * viewMatrix * cube1ModelMatrix;
        glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, &cube1MVP[0][0]);
        glUniform1i(textureSampler, 1); 
        // glDrawArrays(GL_TRIANGLES, 0, cubeVertices.size());

        // cube 2
        glBindVertexArray(cubeVAO);
        size = 2.5f;
        Scaling = glm::scale(mat4(), vec3(size,4.0*size,size));
        Translate = glm::translate(mat4(), vec3(16.0f,0.0f,16.0f));
        mat4 cube2ModelMatrix = Translate * Scaling;
        mat4 cube2MVP = projectionMatrix * viewMatrix * cube2ModelMatrix;
        glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, &cube2MVP[0][0]);
        glUniform1i(textureSampler, 1); 
        // glDrawArrays(GL_TRIANGLES, 0, cubeVertices.size());

        // aircraft
        vec3 force = first_aircraft->seek();
        first_aircraft->forcing = [&](float t, const vector<float>& y)->vector<float> {
            vector<float> f(6, 0.0f);
            f[0] = force.x;
            f[1] = force.y;
            f[2] = force.z;
            return f;
        };
        // cout <<"force: " + to_string(force) << endl;
        cout <<"vel: " + to_string(first_aircraft->v) << endl;
        first_aircraft->update(t,dt);
        mat4 aircraftMVP = projectionMatrix * viewMatrix * first_aircraft->modelMatrix;
        glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, &aircraftMVP[0][0]);
        glActiveTexture(GL_TEXTURE2);
        first_aircraft->bindTexture();
        glUniform1i(textureSampler, 2);
        first_aircraft->bind();
        first_aircraft->draw();

        // ninja 
        // glBindVertexArray(ninjaVAO);
        // Translate = glm::translate(mat4(), vec3(100.0f,0.0f,100.0f));
        // size = 0.03f;
        // Scaling = glm::scale(mat4(), vec3(size,size,size));
        
        // mat4 Rotate = glm::rotate(mat4(),glm::radians(-90.0f),vec3(0.0f,1.0f,.0f));
        // modelModelMatrix = Scaling*Translate;
        // mat4 ninjamodelMVP = projectionMatrix * viewMatrix * modelModelMatrix;
        // glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, &ninjamodelMVP[0][0]);

        // glActiveTexture(GL_TEXTURE1);
        // glBindTexture(GL_TEXTURE_2D, ninjatexture);
        // glUniform1i(textureSampler, 1);

        // // draw
        // glDrawArrays(GL_TRIANGLES, 0, ninjaVertices.size());

        // assimp
        glEnable( GL_DEPTH_TEST );
        glUseProgram(assimp_shader);
        glBindVertexArray( monkey_vao );

        skeleton_animate( monkey_root_node, anim_time, identity_mat4(), monkey_bone_offset_matrices, monkey_bone_animation_mats );
        size = 0.1f;
        Translate = glm::translate(mat4(), vec3(10.0f,3.0f,10.0f));
        Rotate = glm::rotate(mat4(),glm::radians(-90.0f),vec3(0.0f,0.0f,1.0f));
        Scaling = glm::scale(mat4(), vec3(1.0f,1.0f,size));
        mat4 assimp_modelMatrix = Translate * Rotate * Scaling;
        glUniformMatrix4fv( model_mat_location, 1, GL_FALSE, &assimp_modelMatrix[0][0]);
        glUniformMatrix4fv( view_mat_location, 1, GL_FALSE, &viewMatrix[0][0] );
        glUniformMatrix4fv( proj_mat_location, 1, GL_FALSE, &projectionMatrix[0][0]);
        glUniformMatrix4fv( view_mat_location, 1, GL_FALSE, &viewMatrix[0][0] );
        glUniformMatrix4fv( bone_matrices_locations[0], monkey_bone_count, GL_FALSE, monkey_bone_animation_mats[0].m );
        glDrawArrays( GL_TRIANGLES, 0, monkey_point_count );


        glfwSwapBuffers(window);
        glfwPollEvents();
        
        t +=dt;
        
    } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
             glfwWindowShouldClose(window) == 0);
}

void initialize() {
    // Initialize GLFW
    if (!glfwInit()) {
        throw runtime_error("Failed to initialize GLFW\n");
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    window = glfwCreateWindow(W_WIDTH, W_HEIGHT, TITLE, NULL, NULL);
    if (window == NULL) {
        glfwTerminate();
        throw runtime_error(string(string("Failed to open GLFW window.") +
                            " If you have an Intel GPU, they are not 3.3 compatible." +
                            "Try the 2.1 version.\n"));
    }
    glfwMakeContextCurrent(window);

    // Start GLEW extension handler
    glewExperimental = GL_TRUE;

    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        glfwTerminate();
        throw runtime_error("Failed to initialize GLEW\n");
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Hide the mouse and enable unlimited movement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, W_WIDTH / 2, W_HEIGHT / 2);

    // Gray background color
    glClearColor(0.5f, 0.5f, 0.5f, 0.0f);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    // Log
    logGLParameters();

    // Create camera
    camera = new Camera(window);
}
void pollKeyboard(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // bool monkey_moved = false;
    // if (glfwGetKey (window, 'Z')) {
    //     theta += rot_speed * elapsed_seconds;
    //     g_local_anims[0] = rotate_z_deg (identity_mat4 (), theta);
    //     g_local_anims[1] = rotate_z_deg (identity_mat4 (), -theta);
    //     monkey_moved = true;
    // }
    // if (glfwGetKey (window, 'X')) {
    //     theta -= rot_speed * elapsed_seconds;
    //     g_local_anims[0] = rotate_z_deg (identity_mat4 (), theta);
    //     g_local_anims[1] = rotate_z_deg (identity_mat4 (), -theta);
    //     monkey_moved = true;
    // }
    // if (glfwGetKey (window, 'C')) {
    //     y -= 0.5f * elapsed_seconds;
    //     g_local_anims[2] = translate (identity_mat4 (), vec3 (0.0f, y, 0.0f));
    //     monkey_moved = true;
    // }
    // if (glfwGetKey (window, 'V')) {
    //     y += 0.5f * elapsed_seconds;
    //     g_local_anims[2] = translate (identity_mat4 (), vec3 (0.0f, y, 0.0f));
    //     monkey_moved = true;
    // }
    // if (monkey_moved) {
    //     skeleton_animate (
    //     monkey_root_node,
    //     identity_mat4 (),
    //     monkey_bone_offset_matrices,
    //     monkey_bone_animation_mats
    // );
    // glUseProgram (assimp_shader);
    // glUniformMatrix4fv (
    //     bone_matrices_locations[0],
    //     monkey_bone_count,
    //     GL_FALSE,
    //     monkey_bone_animation_mats[0].m
    //     );
    // }
    
    
    // Task 2.1:
   

    /*
    // Task 3.4: toggle polygon mode
    if (key == GLFW_KEY_T && action == GLFW_PRESS) {
        GLint polygonMode[2];
        glGetIntegerv(GL_POLYGON_MODE, &polygonMode[0]);

        // if GL_LINE, if GL_FILL check with polygonMode[0]
        if (polygonMode[0] == GL_LINE) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        if (polygonMode[0] == GL_FILL) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    // Task 4.1a: change the detachment coefficient using U, O keys
    // Use variable: detachmentCoeff
    if (key == GLFW_KEY_U) {
        detachmentCoeff += 0.03; 
    }
    if (key == GLFW_KEY_O) {
        detachmentCoeff -= 0.03;
        if (detachmentCoeff<0) detachmentCoeff = 0.01f;
    }
    */
}

int main(void) {
    try {
        initialize();
        createContext();
        mainLoop();
        free();

    } catch (exception& ex) {
        cout << ex.what() << endl;
        getchar();
        free();
        return -1;
    }
    return 0;
}