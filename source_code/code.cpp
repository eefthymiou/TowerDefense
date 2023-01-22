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
#include <common/animation.h>
#include <common/moving_object.h>
#include <common/robot.h>
#include <common/FireEmitter.h>
#include <common/BulletEmitter.h>
// #include "common/gl_utils.h"

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>

#include <chrono>
using namespace std::chrono;

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

using namespace std;
using namespace glm;

// Function prototypes
void initialize();
void createContext();
void mainLoop();
void free();
void pollKeyboard(GLFWwindow* window, int key, int scancode, int action, int mods);


#define W_WIDTH 1024
#define W_HEIGHT 768
#define TITLE "Tower Defense"

struct robot_info {
  glm::vec3 position;
  glm::vec3 vel;
  int team;
  float maxspeed;
};

// Global variables
GLFWwindow* window;
Camera* camera;
Aircraft* first_aircraft;
Aircraft* second_aircraft;
std::vector<Robot*> robots;
FireEmitter* f_emitter;
BulletEmitter* b_emitter;
std::vector<BulletEmitter> bullet_emitters;
std::vector<robot_info> robots_info;
Moving_obj* planet1;
GLuint shaderProgram;
GLuint assimp_shader;
GLuint gridshader;
GLuint MVPLocation;
GLuint gMVPLocation;
GLuint assimpMVPLocation;
GLuint translationsLocation;
GLuint textureSampler;
GLuint assimptextureSampler;
GLuint gtextureSampler;
GLuint texture,ninjatexture;
GLuint quadtexture;
GLuint bulletTexture;
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
GLuint ammoVAO, ammoTexture, ammoUVVBO, ammoVerticesVBO;
GLuint aircraftVAO,aircraftTexture,aircraftUVVBO,aircraftVerticiesVBO;
GLuint rockVAO,rockTexture,rockUVVBO,rockVerticiesVBO;

std::vector<vec3> Vertices, Normals, ninjaVertices, ninjaNormals, sphereVertices, sphereNormals;
std::vector<vec2> UVs, ninjaUVs,sphereUVs;
std::vector<vec2> quadUVs;

std::vector<vec3> cubeVertices, cubeNormals;
std::vector<vec2> cubeUVs;
std::vector<vec3> ammoVertices, ammoNormals;
std::vector<vec2> ammoUVs;
std::vector<vec3> rockVertices, rockNormals;
std::vector<vec2> rockUVs;

// particles 
GLuint particleShaderProgram;
GLuint projectionMatrixLocation, viewMatrixLocation, modelMatrixLocation, projectionAndViewMatrix;
GLuint translationMatrixLocation, rotationMatrixLocation, scaleMatrixLocation;
GLuint diffuceColorSampler, fireTexture;
glm::vec3 slider_emitter_pos;


// assimp model locations
GLuint model_mat_location;
GLuint view_mat_location;
GLuint proj_mat_location;
int bone_matrices_locations[MAX_BONES];

// gui variables
int health_tower1 = 20000;
int health_tower2 = 20000;
bool game = true;
bool game_ends = false;

glm::vec4 background_color = glm::vec4(0.5f, 0.5f, 0.5f, 0.0f);

void renderHelpingWindow(){
    ImGui::Begin("Helper Window");                          // Create a window called "Hello, world!" and append into it.

    ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
    ImGui::Text("Health tower 1: %d",health_tower1/200);
    ImGui::Text("Health tower 2: %d",health_tower2/200);

    string health;
    int asterakia;
    for (int i=0; i<robots.size(); i++){
        health = "";
        asterakia = robots[i]->health * 10 + 0.5;
        for (int j=0; j<asterakia; j++) health += "*";
        char* c = const_cast<char*>(health.c_str());
        ImGui::Text("robot %d: %s",robots[i]->team_tower,c);
    }
    if (!game_ends){
        if (!game) ImGui::Text("Game Paused.");
        else ImGui::Text("Press P to pause the game.");
    }
    else {
        if (health_tower1<=0.0) ImGui::Text("TOWER 2 WINS");
        else ImGui::Text("TOWER 1 WINS");
    }
    ImGui::Text("Performance %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();
    
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


void generate_robots_info(){
    robot_info temp_robot_info;
    std::vector<vec3> positions1;
    std::vector<vec3> positions2;
    float random_speed;

    positions1.push_back(vec3(0.0f,0.9f,8.0f));
    positions1.push_back(vec3(3.5f,0.9f,3.5f));
    positions1.push_back(vec3(8.0f,0.9f,0.0f));

    
    for (int i=0; i<positions1.size(); i++){
        random_speed = static_cast<float>(std::rand()) / RAND_MAX * 0.5 + 0.5;
        temp_robot_info.position = positions1[i];
        temp_robot_info.vel = vec3(0.0f,0.0f,0.0f);
        temp_robot_info.team = 1;
        temp_robot_info.maxspeed = random_speed*0.2;
        robots_info.push_back(temp_robot_info);
    }

    positions2.push_back(vec3(14.5f,0.9f,14.5f));
    positions2.push_back(vec3(18.0f,0.9f,10.0f));
    positions2.push_back(vec3(10.0f,0.9f,18.0f));
    
    for (int i=0; i<positions2.size(); i++){
        random_speed = static_cast<float>(std::rand()) / RAND_MAX * 0.5 + 0.5;
        temp_robot_info.position = positions2[i];
        temp_robot_info.vel = vec3(0.0f,0.0f,0.0f);
        temp_robot_info.team = 2;
        temp_robot_info.maxspeed = random_speed*0.2;
        robots_info.push_back(temp_robot_info);
    }
}

void createContext() {
    // Create and compile our GLSL program from the shaders
    shaderProgram = loadShaders("../shaders/texture.vertexshader", "../shaders/texture.fragmentshader");
    particleShaderProgram = loadShaders("../shaders/ParticleShader.vertexshader","../shaders/ParticleShader.fragmentshader");
    gridshader = loadShaders("../shaders/grid.vertexshader", "../shaders/grid.fragmentshader");
    assimp_shader = loadShaders("../shaders/assimp.vertexshader", "../shaders/assimp.fragmentshader");

    // Draw wire frame triangles or fill: GL_LINE, or GL_FILL
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // shaderProgram
    MVPLocation = glGetUniformLocation(shaderProgram, "MVP");
    textureSampler = glGetUniformLocation(shaderProgram, "textureSampler");

    // grid shader
    gMVPLocation = glGetUniformLocation(gridshader, "MVP");
    translationsLocation = glGetUniformLocation(gridshader, "grid");
    gtextureSampler = glGetUniformLocation(gridshader, "textureSampler");

    // assimp shader
    model_mat_location = glGetUniformLocation(assimp_shader, "model");
    view_mat_location = glGetUniformLocation(assimp_shader, "view");
    proj_mat_location = glGetUniformLocation(assimp_shader, "proj");
    assimptextureSampler = glGetUniformLocation(assimp_shader, "textureSampler");
    
    // particle shader
    projectionAndViewMatrix = glGetUniformLocation(particleShaderProgram, "PV");
    diffuceColorSampler = glGetUniformLocation(particleShaderProgram, "texture0");

    char name[64];
    for ( int i = 0; i < MAX_BONES; i++ ) {
        sprintf( name, "bone_matrices[%i]", i );
        bone_matrices_locations[i] = glGetUniformLocation( assimp_shader, name );
        glUniformMatrix4fv( bone_matrices_locations[i], 1, GL_FALSE, identity_mat4().m );
    }

    // fire
    fireTexture = loadSOIL("../Textures/fire.png");
    auto* quad = new Drawable("../OBJ_files/quad.obj");
    f_emitter = new FireEmitter(quad,  2000);
    (*f_emitter).height_threshold = 5.0f;

    bulletTexture = loadSOIL("../Textures/bullet/bullet_DefaultMaterial_BaseColor.png");
    auto* bullet = new Drawable("../OBJ_files/bullet.obj");
    int num_of_bullets = 1;

    for (int i=0; i<16; i++){
        b_emitter = new BulletEmitter(bullet, num_of_bullets);
        bullet_emitters.push_back(*b_emitter);
    }
    

    

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

    // ammo
    loadOBJ("../OBJ_files/ammo.obj",
        ammoVertices, 
        ammoUVs,
        ammoNormals);

    // VAO
    glGenVertexArrays(1, &ammoVAO);
    glBindVertexArray(ammoVAO);

    // vertex VBO
    glGenBuffers(1, &ammoVerticesVBO);
    glBindBuffer(GL_ARRAY_BUFFER, ammoVerticesVBO);
    glBufferData(GL_ARRAY_BUFFER, ammoVertices.size() * sizeof(glm::vec3),
                 &ammoVertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    ammoTexture = loadSOIL("../Textures/ammo/box_fin_DefaultMaterial_BaseColor.tga");
    
    // uvs VBO
    glGenBuffers(1, &ammoUVVBO);
    glBindBuffer(GL_ARRAY_BUFFER, ammoUVVBO);
    glBufferData(GL_ARRAY_BUFFER, ammoUVs.size() * sizeof(glm::vec2),&ammoUVs[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);

    // aircraft
    vec3 position = vec3(14.0f,2.0f,0.0f);
    vec3 target = vec3(14.0f,2.0f,14.0f);
    vec3 vel = vec3(0.01f,0.01f,0.01f);
    float mass = 2.0f;
    int ammo = 500;
    int id = 0;
    first_aircraft = new Aircraft(position,vel,mass,target,ammo,id,vec3(16.0f,0.0f,16.0f));
    first_aircraft->load_mesh("../OBJ_files/aircraft.obj");
    first_aircraft->loadTexture("../Textures/aircraft/aircrafttank_DefaultMaterial_BaseColor.png");
    first_aircraft->at_tower_pos =  vec3(16.0f,0.0f,16.0f);
    first_aircraft->size =  0.08;

    position = vec3(0.0f,2.0f,14.0f);
    target = vec3(4.0f,2.0f,4.0f);
    vel = vec3(1.01f,0.01f,0.01f);
    mass = 2.0f;
    ammo = 500;
    id = 1;
    second_aircraft = new Aircraft(position,vel,mass,target,ammo,id,vec3(-2.0f,0.0f,-2.0f));
    second_aircraft->at_tower_pos = vec3(2.0f,0.0f,2.0f);
    second_aircraft->size =  0.08;

    // planet
    // position = vec3(4.0f, 2.0f, 14.0f);
    // target = vec3(6.0f,2.0f,4.0f);
    // vel = vec3(1.0f,1.0f,1.0f);
    // planet1 = new Moving_obj(position,vel,mass,target);
    // planet1->load_mesh("../OBJ_files/sphere.obj");
    // planet1->loadTexture("../Textures/2k_mars.jpg");
    // planet1->arrives = false;
    // planet1->maxforce = 10.0f;
    // planet1->maxspeed = 10.0f;

    // amimation
    // generate positions for robots 
    generate_robots_info();
    Robot* robot;
    vec3 tower1_pos = vec3(2.0f,0.5f,2.0f);
    vec3 tower2_pos = vec3(16.0,0.5f,16.0f);

    robot_info c_r_info;
    c_r_info = robots_info[0];
    target = vec3(15.0f,0.9f,15.0f);
    
    robot = new Robot("../Models/finale5.dae",c_r_info.position,c_r_info.vel,mass,target,1,c_r_info.maxspeed);
    robot->loadTexture("../Models/Texture_0.jpg");
    robot->health = 1.0f;
    robots.push_back(robot);

    c_r_info = robots_info[1];
    robot = new Robot("../Models/finale5.dae",c_r_info.position,c_r_info.vel,mass,target,1,c_r_info.maxspeed);
    robot->loadTexture("../Models/Texture_0.jpg");
    robot->health = 1.0f;
    robots.push_back(robot);

    c_r_info = robots_info[2];
    robot = new Robot("../Models/finale5.dae",c_r_info.position,c_r_info.vel,mass,target,1,c_r_info.maxspeed);
    robot->loadTexture("../Models/Texture_0.jpg");
    robot->health = 1.0f;
    robots.push_back(robot);

    c_r_info = robots_info[3];
    target = vec3(3.0f,0.9f,3.0f);
    robot = new Robot("../Models/finale5.dae",c_r_info.position,c_r_info.vel,mass,target,2,c_r_info.maxspeed);
    robot->loadTexture("../Models/Texture_0.jpg");
    robot->health = 1.0f;
    robots.push_back(robot);
    
    c_r_info = robots_info[4];
    robot = new Robot("../Models/finale5.dae",c_r_info.position,c_r_info.vel,mass,target,2,c_r_info.maxspeed);
    robot->loadTexture("../Models/Texture_0.jpg");
    robot->health = 1.0f;
    robots.push_back(robot);

    c_r_info = robots_info[5];
    robot = new Robot("../Models/finale5.dae",c_r_info.position,c_r_info.vel,mass,target,2,c_r_info.maxspeed);
    robot->loadTexture("../Models/Texture_0.jpg");
    robot->health = 1.0f;
    robots.push_back(robot);


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
    // quadtexture = loadSOIL("../Textures/grid/DALL·E 2023-01-22 17.51.15 - texture for a grid in a space game.png");
    quadtexture = loadSOIL("../Textures/grid/DALL·E 2023-01-22 18.51.45.png");

    // uvs VBO
    glGenBuffers(1, &quadUVVBO);
    glBindBuffer(GL_ARRAY_BUFFER, quadUVVBO);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(glm::vec2),&quadUVs[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // rock
    loadOBJ("../OBJ_files/Cliff_Rock_One_OBJ.obj",
        rockVertices, 
        rockUVs,
        rockNormals);
    
    // VAO
    glGenVertexArrays(1, &rockVAO);
    glBindVertexArray(rockVAO);

    // vertex VBO
    glGenBuffers(1, &rockVerticiesVBO);
    glBindBuffer(GL_ARRAY_BUFFER, rockVerticiesVBO);
    glBufferData(GL_ARRAY_BUFFER, rockVertices.size() * sizeof(glm::vec3),
                 &rockVertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);


    // use texture
    rockTexture = loadSOIL("../Textures/Cliff_Rock_One_Texture4K/Cliff_Rock_One_BaseColor.png");
    
    // uvs VBO
    glGenBuffers(1, &rockUVVBO);
    glBindBuffer(GL_ARRAY_BUFFER, rockUVVBO);
    glBufferData(GL_ARRAY_BUFFER, rockUVs.size() * sizeof(glm::vec2),&rockUVs[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);

    glfwSetKeyCallback(window, pollKeyboard);
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
    glDeleteProgram(particleShaderProgram);
    glfwTerminate();
}


void check_game(){
    if (health_tower2<=0) {
        game_ends = true;
        game = false; 
        (*f_emitter).emitter_pos = vec3(16.0f,0.1f,16.0f);
    }
    if (health_tower1<=0){
        game_ends = true;
        game = false;
        (*f_emitter).emitter_pos = vec3(2.0f,0.1f,2.0f);
    }
}

void mainLoop() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");


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

    mat4 projectionMatrix,viewMatrix;
    mat4 Scaling,Rotate,Translate;

    float t = glfwGetTime();
    float t_particles = glfwGetTime();

    vector<package_ammo> ammo_packages;
    package_ammo temp_package_ammo;
    vec3 position;

    vec3 ammo_position;
    mat4 ammoModelMatrix;
    mat4 ammoMVP;
    mat4 bulletMVP;
    float size;

    for (int i=0; i<5; i++) { 
        temp_package_ammo.position = get_random_pos();
        temp_package_ammo.available = true;
        ammo_packages.push_back(temp_package_ammo);
        cout<<ammo_packages.size()<<endl;
    }
    do {

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        glClearColor(background_color[0], background_color[1], background_color[2], background_color[3]);

        check_game();

        // float dt = glfwGetTime()-t;
        float dt = 0.1;

        
        camera->update();
        projectionMatrix = camera->projectionMatrix;
        viewMatrix = camera->viewMatrix;
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable( GL_DEPTH_TEST );

        glUseProgram(assimp_shader);
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(assimptextureSampler, 0);

        vector<float> f0(6, 0.0f);
        vec3 force0;
        vec3 katheto;
        int j;
        for (int i=0; i<robots.size(); i++){
            j=i+2;
            Robot* robot = robots[i];
            
            if (robot->health>0.0f){
                if (game){ 
                    robot->findTarget(&robots);
                    if (robot->team_tower == 1 ) robot->handleShooting(&health_tower2);
                    else robot->handleShooting(&health_tower1);
                    if (distance(robot->x, robot->target)>0.01){
                        force0 = robot->seek();
                        robot->forcing = [&](float t, const vector<float>& y)->vector<float> {
                            f0[0] = force0.x;
                            f0[2] = force0.z;
                            return f0;
                        };
                        robot->update(t,dt);
                    }
                }
                robot->bindTexture();
                robot->bind();
                robot->skeleton_animate(robot->root_node,robot->anim_time, identity_mat4(),robot->bone_offset_matrices, robot->bone_animation_mats );
                glUniformMatrix4fv( model_mat_location, 1, GL_FALSE, &robot->modelMatrix[0][0]);
                glUniformMatrix4fv( view_mat_location, 1, GL_FALSE, &viewMatrix[0][0] );
                glUniformMatrix4fv( proj_mat_location, 1, GL_FALSE, &projectionMatrix[0][0]);
                glUniformMatrix4fv( bone_matrices_locations[0], robot->bone_count, GL_FALSE, robot->bone_animation_mats[0].m );
                robot->draw();
            }
            else if (game) robot->handleHealth(); 
        
            katheto = glm::cross(normalize(robot->direction),vec3(0.0f,1.0f,0.0f));
            
            bullet_emitters[2*j+1].emitter_pos = robot->x + katheto*0.3f;
            bullet_emitters[2*j+1].bullet_target = robot->target;

            bullet_emitters[2*j].emitter_pos = robot->x - katheto*0.3f;
            bullet_emitters[2*j].bullet_target = robot->target;

            // robot0 -> 4,5
            // robot1 -> 6,7
            // robot3 -> 8,9
            // robot4 -> 10,11
            // robot5 -> 12,13
        }  

        // use grid shader
        // glUseProgram(gridshader);
        // glBindVertexArray(quadVAO);
        // float size = 2.0f;
        // mat4 quadScaling = glm::scale(mat4(), vec3(size,0.0f,size));
        // mat4 quadRotate = glm::rotate(mat4(),glm::radians(90.0f),vec3(1.0f,0.0f,0.0f));
        // mat4 quadModelMatrix = quadScaling * quadRotate;
        // mat4 quadMVP = projectionMatrix * viewMatrix * quadModelMatrix;
        // glUniformMatrix4fv(gMVPLocation, 1, GL_FALSE, &quadMVP[0][0]);
        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, quadtexture);
        // glUniform1i(gtextureSampler, 0);   
        // glUniform3fv(translationsLocation, 100, &translations[0].z); 
        // glDrawArraysInstanced(GL_TRIANGLES, 0, 2*3, 100);
        

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

        // first aircraft
        if (game) {
            first_aircraft->handle_ammo(&ammo_packages,&health_tower2);
            if (distance(first_aircraft->x, first_aircraft->target)>0.01){
                vec3 force1 = first_aircraft->seek();
                first_aircraft->forcing = [&](float t, const vector<float>& y)->vector<float> {
                    vector<float> f1(6, 0.0f);
                    f1[0] = force1.x;
                    f1[1] = force1.y;
                    f1[2] = force1.z;
                    return f1;
                };
                first_aircraft->update(t,dt);
            }
        }
        mat4 aircraftMVP = projectionMatrix * viewMatrix * first_aircraft->modelMatrix;
        glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, &aircraftMVP[0][0]);
        glActiveTexture(GL_TEXTURE2);
        first_aircraft->bindTexture();
        glUniform1i(textureSampler, 2);
        first_aircraft->bind();
        first_aircraft->draw();
        katheto = glm::cross(normalize(first_aircraft->direction),vec3(0.0f,1.0f,0.0f));
        bullet_emitters[0].emitter_pos = first_aircraft->x + katheto*0.5f + vec3(0.0f,0.2f,0.0f);
        bullet_emitters[0].bullet_target = vec3(16.0f,2.0f,16.0f);
        bullet_emitters[1].emitter_pos = first_aircraft->x - katheto*0.5f + vec3(0.0f,0.2f,0.0f);
        bullet_emitters[1].bullet_target = vec3(16.0f,2.0f,16.0f);

        // second aircraft
        if (game){
            second_aircraft->handle_ammo(&ammo_packages,&health_tower1);
            if (distance(second_aircraft->x, second_aircraft->target)>0.01){
                vec3 force2 = second_aircraft->seek();
                second_aircraft->forcing = [&](float t, const vector<float>& y)->vector<float> {
                    vector<float> f2(6, 0.0f);
                    f2[0] = force2.x;
                    f2[1] = force2.y;
                    f2[2] = force2.z;
                    return f2;
                };
                second_aircraft->update(t,dt);
            }
        }
        aircraftMVP = projectionMatrix * viewMatrix * second_aircraft->modelMatrix;
        glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, &aircraftMVP[0][0]);
        glActiveTexture(GL_TEXTURE2);
        glUniform1i(textureSampler, 2);
        second_aircraft->bind();
        second_aircraft->draw();
        katheto = glm::cross(normalize(second_aircraft->direction),vec3(0.0f,1.0f,0.0f));
        bullet_emitters[2].emitter_pos = second_aircraft->x + katheto*0.5f + vec3(0.0f,0.2f,0.0f);
        bullet_emitters[2].bullet_target = vec3(2.0f,2.0f,2.0f);
        bullet_emitters[3].emitter_pos = second_aircraft->x - katheto*0.5f + vec3(0.0f,0.2f,0.0f);
        bullet_emitters[3].bullet_target = vec3(2.0f,2.0f,2.0f);

        // ammo
        glBindVertexArray(ammoVAO);
        size = 0.03;
        Scaling = glm::scale(mat4(), vec3(size,size,size));
        Rotate = glm::rotate(mat4(), t*3.14f/5.0f, vec3(0.0f,1.0f,0.0f));
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, ammoTexture);
        glUniform1i(textureSampler, 3); 
        for (int i=0; i<ammo_packages.size(); i++){
            ammo_position = ammo_packages[i].position;
            Translate = glm::translate(mat4(),ammo_position);
            ammoModelMatrix = Translate * Rotate * Scaling;
            ammoMVP = projectionMatrix * viewMatrix * ammoModelMatrix;
            glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, &ammoMVP[0][0]);
            glDrawArrays(GL_TRIANGLES, 0, ammoVertices.size());
        }

        glBindVertexArray(quadVAO);
        size = 20.0f;
        mat4 quadScaling = glm::scale(mat4(), vec3(size,0.0f,size));
        mat4 quadRotate = glm::rotate(mat4(),glm::radians(90.0f),vec3(1.0f,0.0f,0.0f));
        mat4 quadTranslate = glm::translate(mat4(), vec3(9.0f,0.0f,9.0f));
        mat4 quadModelMatrix = quadTranslate * quadScaling * quadRotate;
        mat4 quadMVP = projectionMatrix * viewMatrix * quadModelMatrix;
        glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, &quadMVP[0][0]);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, quadtexture);
        glUniform1i(textureSampler, 4);   
        glDrawArrays(GL_TRIANGLES, 0, 2*3);

        // rock
        glBindVertexArray(rockVAO);
        size = 0.01f;
        Scaling = glm::scale(mat4(), vec3(size,size,size));
        Translate = glm::translate(mat4(), vec3(16.0f,0.0f,2.0f));
        mat4 rockModelMatrix = Translate * Scaling;
        mat4 rockMVP = projectionMatrix * viewMatrix * rockModelMatrix;
        glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, &rockMVP[0][0]);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, rockTexture);
        glUniform1i(textureSampler, 5);   
        glDrawArrays(GL_TRIANGLES, 0, rockVertices.size());

        // // bullet
        // bullet->bind();
        // glActiveTexture(GL_TEXTURE4);
        // glBindTexture(GL_TEXTURE_2D, bulletTexture);
        // glUniform1i(textureSampler, 4); 
        // size = 4.0f;
        // Translate = glm::translate(mat4(), vec3(0.0f,2.0f,0.0f));
        // Scaling = glm::scale(mat4(), vec3(size,size,size));
        // mat4 bulletModelMatrix = Translate * Scaling;
        // bulletMVP = projectionMatrix * viewMatrix * bulletModelMatrix;
        // glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, &bulletMVP[0][0]);
        // bullet->draw();



        // float distance = length(planet1->x-planet1->target);
        // if (distance<2.0f) {
        //     vec3 prev_target = planet1->target;
        //     if (prev_target.x==14.0) direction = -1;
        //     if (prev_target.x==4.0)  direction = 1;
        //     planet1->target = vec3(prev_target.x+direction*2.0f, prev_target.y,18-prev_target.z);
        // }
        // vec3 force2 = planet1->seek();
        // planet1->forcing = [&](float t, const vector<float>& y)->vector<float> {
        //     vector<float> f2(6, 0.0f);
        //     f2[0] = force2.x;
        //     f2[1] = force2.y;
        //     f2[2] = force2.z;
        //     return f2;
        // };
        // planet1->update(t,dt,0.2);
        // mat4 planet1MVP = projectionMatrix * viewMatrix * planet1->modelMatrix;
        // glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, &planet1MVP[0][0]);
        // glActiveTexture(GL_TEXTURE3);
        // planet1->bindTexture();
        // glUniform1i(textureSampler, 3);
        // planet1->bind();
        // planet1->draw();

        glUseProgram(particleShaderProgram);
        float currentTime = glfwGetTime();
        float dt_particles = currentTime - t_particles;

        auto PV = projectionMatrix * viewMatrix;
        glUniformMatrix4fv(projectionAndViewMatrix, 1, GL_FALSE, &PV[0][0]);

        if (game_ends){
            (*f_emitter).use_rotations = true;
            (*f_emitter).use_sorting = false;
            
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, fireTexture);
            glUniform1i(diffuceColorSampler, 0);

            (*f_emitter).updateParticles(currentTime, dt_particles, camera->position);
            (*f_emitter).renderParticles();
        }
        // bullet 
        
        bullet_emitters[0].use_rotations = true;
        bullet_emitters[0].use_sorting = false;
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, bulletTexture);
        glUniform1i(diffuceColorSampler, 1);

        for (int i=0; i<robots.size(); i++){
            j=i+2;
            if (game){
                bullet_emitters[2*j].updateParticles(currentTime, dt_particles, camera->position);
                bullet_emitters[2*j+1].updateParticles(currentTime, dt_particles, camera->position);
            }
            if (robots[i]->shoots){              
                bullet_emitters[2*j].renderParticles();
                bullet_emitters[2*j+1].renderParticles();
            }
            else {
                bullet_emitters[2*j].createNewParticle(0);
                bullet_emitters[2*j+1].createNewParticle(0);
            }  
        }
        
        if (game){
            bullet_emitters[0].updateParticles(currentTime, dt_particles, camera->position);
            bullet_emitters[1].updateParticles(currentTime, dt_particles, camera->position);
            bullet_emitters[2].updateParticles(currentTime, dt_particles, camera->position);
            bullet_emitters[3].updateParticles(currentTime, dt_particles, camera->position);
        }
        if (first_aircraft->shoots){              
            bullet_emitters[0].renderParticles();
            bullet_emitters[1].renderParticles();
        }
        else {
            bullet_emitters[0].createNewParticle(0);
            bullet_emitters[1].createNewParticle(0);
        } 
        if (second_aircraft->shoots){              
            bullet_emitters[2].renderParticles();
            bullet_emitters[3].renderParticles();
        }
        else {
            bullet_emitters[2].createNewParticle(0);
            bullet_emitters[3].createNewParticle(0);
        } 
    
        

        
        renderHelpingWindow();
        glfwSwapBuffers(window);
        glfwPollEvents();
        
        t +=dt;
        t_particles = currentTime;

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
    if (key == GLFW_KEY_P && action == GLFW_PRESS && !game_ends) {
        game = !game;
    }
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