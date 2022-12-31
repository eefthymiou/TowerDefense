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
Drawable* model;
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
std::vector<vec3> Vertices, Normals, ninjaVertices, ninjaNormals;
std::vector<vec2> UVs, ninjaUVs;
std::vector<vec2> quadUVs;

GLuint useTextureLocation;


//load a file with AssImp
#include <assimp/cimport.h> // C importer
#include <assimp/scene.h> // collects data
#include <assimp/postprocess.h> // various extra operations
#include <stdlib.h> // memory management
#include <assert.h>
#define MESH_FILE "../Models/model.dae" // file to load ...
#define TEXTURE_FILE "../Models/Ninja_T.png"
#define MAX_BONES 15
#define _USE_MATH_DEFINES

/* load the mesh using assimp */

// GLuint monkey_vao;
// my_mat4 monkey_bone_offset_matrices[20];
// int monkey_point_count = 0;
// int monkey_bone_count = 0;
// Skeleton_Node* monkey_root_node = NULL;
// my_mat4 g_local_anims[20];

// int bone_matrices_locations[20];
// my_mat4 monkey_bone_animation_mats[20];
// float y = 0.0; // position of head
// double monkey_anim_duration = 0.0;
// float elapsed_seconds = 0.01f;

// float theta = 3.14/4.0f;
// float rot_speed = 1.0f;

// rotate around z axis by an angle in degrees

void createContext() {
    // Create and compile our GLSL program from the shaders
    shaderProgram = loadShaders("../shaders/texture.vertexshader", "../shaders/texture.fragmentshader");

    // Draw wire frame triangles or fill: GL_LINE, or GL_FILL
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Get a pointer location to model matrix in the vertex shader
    MVPLocation = glGetUniformLocation(shaderProgram, "MVP");
    textureSampler = glGetUniformLocation(shaderProgram, "textureSampler");

    // Building 1
    // model = new Drawable("../OBJ_FILES/cube.obj");
    // loadOBJ("../Building_2/cottage_obj/new.obj",
    //     Vertices, 
    //     UVs,
    //     Normals);
    
    //Building 2
    loadOBJ("../Building_3/new2.obj",
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
    // texture = loadSOIL("../Building_2/cottage_textures/cottage_textures/cottage_diffuse.png");
    texture = loadSOIL("../Building_3/Farmhouse Texture.jpg");
    
    // uvs VBO
    glGenBuffers(1, &UVVBO);
    glBindBuffer(GL_ARRAY_BUFFER, UVVBO);
    glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(glm::vec2),&UVs[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // loadOBJ("../Models/Mini_Ninja_triangles.obj",
    //     ninjaVertices, 
    //     ninjaUVs,
    //     ninjaNormals);

    //  // VAO
    // glGenVertexArrays(1, &ninjaVAO);
    // glBindVertexArray(ninjaVAO);

    // // vertex VBO
    // glGenBuffers(1, &ninjaVerticiesVBO);
    // glBindBuffer(GL_ARRAY_BUFFER, ninjaVerticiesVBO);
    // glBufferData(GL_ARRAY_BUFFER, ninjaVertices.size() * sizeof(glm::vec3),
    //              &ninjaVertices[0], GL_STATIC_DRAW);
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    // glEnableVertexAttribArray(0);

    // // uvs VBO
    // glGenBuffers(1, &ninjaUVVBO);
    // glBindBuffer(GL_ARRAY_BUFFER, ninjaUVVBO);
    // glBufferData(GL_ARRAY_BUFFER, ninjaUVs.size() * sizeof(glm::vec2),&ninjaUVs[0], GL_STATIC_DRAW);
    // glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    // glEnableVertexAttribArray(1);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // assimp_shader = loadShaders("../shaders/assimp.vertexshader", "../shaders/assimp.fragmentshader");
    // assimpMVPLocation = glGetUniformLocation(assimp_shader, "MVP");

    // // assimp
    // load_mesh(
    //      MESH_FILE, 
    //      &monkey_vao, 
    //      &monkey_point_count, 
    //      monkey_bone_offset_matrices, 
    //      &monkey_bone_count, 
    //      &monkey_root_node,
    //      &monkey_anim_duration 
    // );
    
    // printf ("monkey bone count %i\n", monkey_bone_count);

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



// void skeleton_animate (
//     Skeleton_Node* node,
//     my_mat4 parent_mat,
//     my_mat4* bone_offset_mats,
//     my_mat4* bone_animation_mats) {

//     assert (node);
//     /* the animation of a node after inheriting its parent's animation */
//     my_mat4 our_mat = identity_mat4 ();
//     /* the animation for a particular bone at this time */
//     my_mat4 local_anim = identity_mat4 ();

//     // if node has a bone...
//     int bone_i = node->bone_index;
//     if (bone_i > -1) {
//         // ... then get offset matrices
//         my_mat4 bone_offset = bone_offset_mats[bone_i];
//         my_mat4 inv_bone_offset = inverse (bone_offset);
//         // ... at the moment get the per-bone animation from keyboard input
//         local_anim = g_local_anims[bone_i];
//         our_mat = parent_mat * inv_bone_offset * local_anim * bone_offset;
//         bone_animation_mats[bone_i] = our_mat;
//     } else {
//         our_mat = parent_mat;
//     }

//     for (int i = 0; i < node->num_children; i++) {
//         skeleton_animate (
//             node->children[i],
//             our_mat,
//             bone_offset_mats,
//             bone_animation_mats
//         );
//     }
// }

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

    // ninja
    glDeleteBuffers(1, &ninjaVerticiesVBO);
    glDeleteBuffers(1, &ninjaUVVBO);
    glDeleteTextures(1, &ninjatexture);
    glDeleteVertexArrays(1, &ninjaVAO);

    glDeleteProgram(shaderProgram);
    glDeleteProgram(gridshader);
    glfwTerminate();
}

void mainLoop() {
    glm::vec3 translations[100];
    int index = 0;
    float offset = 5.0f;
    for(int z = -10; z < 10; z += 2){
        for(int x = -10; x < 10; x += 2){
            glm::vec3 translation;
            translation.x = (float)x / 2.0f + offset ;
            translation.z = (float)z / 2.0f + offset ;
            translation.y = 0;
            // std::cout << index << glm::to_string(translation) << std::endl;
            translations[index++] = translation;
        }
    }


    float identity[] = {
        1.0f, 0.0f, 0.0f, 0.0f, // first column
        0.0f, 1.0f, 0.0f, 0.0f, // second column
        0.0f, 0.0f, 1.0f, 0.0f, // third column
        0.0f, 0.0f, 0.0f, 1.0f // fourth column
    };

    double anim_timer = 0.0;

    do {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // anim_timer += elapsed_seconds;
        // if (anim_timer >= monkey_anim_duration) {
        //     anim_timer -= monkey_anim_duration;
        // }

        mat4 projectionMatrix,viewMatrix,modelMatrix;
        int width, height;

        camera->update();
        projectionMatrix = camera->projectionMatrix;
        viewMatrix = camera->viewMatrix;


        // use shaderProgram
        glUseProgram(shaderProgram);
        
        // Bind model VAO
        glBindVertexArray(VAO);

        float size = 0.1f;
        mat4 Scaling = glm::scale(mat4(), vec3(size,size,size));
        mat4 Rotate = glm::rotate(mat4(),glm::radians(-90.0f),vec3(0.0f,1.0f,0.0f));
        mat4 Translate = glm::translate(mat4(), vec3(2.0f,0.0f,1.0f));
        mat4 modelModelMatrix = Translate * Scaling * Rotate;
        mat4 modelMVP = projectionMatrix * viewMatrix * modelModelMatrix;
        glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, &modelMVP[0][0]);

        // Bind our texture in Texture Unit 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(textureSampler, 0); 
        
        // draw
        glDrawArrays(GL_TRIANGLES, 0, Vertices.size());
        

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

        // //load suzzane with assimp
        // glUseProgram(assimp_shader);
        // glBindVertexArray (monkey_vao);
        // char name[64];
        // for (int i = 0; i < 20; i++) {
        //     sprintf (name, "bone_matrices[%i]", i);
        //     bone_matrices_locations[i] = glGetUniformLocation (assimp_shader, name);
        //     glUniformMatrix4fv (bone_matrices_locations[i], 1, GL_FALSE, identity);
        // }
        // size = 1.0;
        // Scaling = glm::scale(mat4(), vec3(size,size,size));
        // Translate = glm::translate(mat4(), vec3(10.0f,0.0f,10.0f));
        // Rotate = glm::rotate(mat4(),glm::radians(-90.0f),vec3(1.0f,0.0f,0.0f));
        // mat4 assimp_modelMatrix = Translate*Scaling*Rotate;
        // mat4 assimpmodelMVP = projectionMatrix * viewMatrix * assimp_modelMatrix;
        // glUniformMatrix4fv(assimpMVPLocation, 1, GL_FALSE, &assimpmodelMVP[0][0]);
        // glDrawArrays (GL_TRIANGLES, 0, monkey_point_count);    
        
        // use grid shader
        glUseProgram(gridshader);

        glBindVertexArray(quadVAO);

        size = 2.0f;
        mat4 quadScaling = glm::scale(mat4(), vec3(size,0.0f,size));
        mat4 quadRotate = glm::rotate(mat4(),glm::radians(90.0f),vec3(1.0f,0.0f,0.0f));
        mat4 quadModelMatrix = mat4(1) * quadScaling * quadRotate;
        mat4 quadMVP = projectionMatrix * viewMatrix * quadModelMatrix;
        glUniformMatrix4fv(gMVPLocation, 1, GL_FALSE, &quadMVP[0][0]);

        // Bind our texture in Texture Unit 1
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, quadtexture);
        glUniform1i(gtextureSampler, 2);   

        glUniform3fv(translationsLocation, 100, &translations[0].z); 

        // draw
        glDrawArraysInstanced(GL_TRIANGLES, 0, 2*3, 100);

        glfwSwapBuffers(window);
        glfwPollEvents();
        
        
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