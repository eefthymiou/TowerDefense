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
#include <common/light.h>


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
#define TITLE "Tower Defence"

#define SHADOW_WIDTH 1024
#define SHADOW_HEIGHT 1024

struct robot_info {
  glm::vec3 position;
  glm::vec3 vel;
  int team;
  float maxspeed;
};

struct Material {
	vec4 Ka;
	vec4 Kd;
	vec4 Ks;
	float Ns;
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
Drawable* tower;
Drawable* model2;
Drawable* model3;
Drawable* plane;
Drawable* ammo;
Drawable* rock;
GLuint sun_texture;
GLuint shaderProgram;
GLuint depthProgram;
GLuint lightingProgram;
GLuint MVPLocation;
GLuint gMVPLocation;
GLuint assimpMVPLocation;
GLuint translationsLocation;
GLuint textureSampler;
GLuint assimptextureSampler;
GLuint gtextureSampler;

GLuint tower_main_texture;
GLuint tower_diffuse_texture;
GLuint tower_specular_texture;

GLuint planetexture;
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

// lighting 
Light* light1;
GLuint KaLocation, KdLocation, KsLocation, NsLocation;
GLuint LaLocation1, LdLocation1, LsLocation1;
GLuint lightPositionLocation1;
GLuint lightPowerLocation1;
GLuint depthMapSampler1;
GLuint lightVPLocation1;
GLuint lightDirectionLocation1;	
GLuint lightFarPlaneLocation1;
GLuint lightNearPlaneLocation1;
GLuint diffuseColorSampler; 
GLuint specularColorSampler;
GLuint useTextureLocation;
GLuint useSpecularTextureLocation;
GLuint is_animation;

// locations for depthProgram
GLuint shadowViewProjectionLocation; 
GLuint shadowModelLocation;
GLuint depthFrameBuffer;
GLuint depthTexture;

// gui variables
int health_tower1 = 20000;
int health_tower2 = 20000;
bool game = true;
bool game_ends = false;

// lighting
const Material yellow_plastic{
	vec4{0.1,0.0,0.0,.25},
	vec4{0.9,0.5,0.0,.25},
	vec4{0.9,0.6,0.5,.25},
	12.8f
};


glm::vec4 background_color = glm::vec4(0.5f, 0.5f, 0.5f, 0.0f);


vector<package_ammo> ammo_packages;


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

void uploadLight(const Light& light,GLuint LaLocation, GLuint LdLocation, GLuint LsLocation, 
				GLuint lightPositionLocation,GLuint lightPowerLocation  ) {
	glUniform4f(LaLocation, light.La.r, light.La.g, light.La.b, light.La.a);
	glUniform4f(LdLocation, light.Ld.r, light.Ld.g, light.Ld.b, light.Ld.a);
	glUniform4f(LsLocation, light.Ls.r, light.Ls.g, light.Ls.b, light.Ls.a);
	glUniform3f(lightPositionLocation, light.lightPosition_worldspace.x,
	light.lightPosition_worldspace.y, light.lightPosition_worldspace.z);
	glUniform1f(lightPowerLocation, light.power);
}

void uploadMaterial(const Material& mtl) {
	glUniform4f(KaLocation, mtl.Ka.r, mtl.Ka.g, mtl.Ka.b, mtl.Ka.a);
	glUniform4f(KdLocation, mtl.Kd.r, mtl.Kd.g, mtl.Kd.b, mtl.Kd.a);
	glUniform4f(KsLocation, mtl.Ks.r, mtl.Ks.g, mtl.Ks.b, mtl.Ks.a);
	glUniform1f(NsLocation, mtl.Ns);
}



void createContext() {
    // Create and compile our GLSL program from the shaders
    shaderProgram = loadShaders("../shaders/texture.vert", "../shaders/texture.frag");
    particleShaderProgram = loadShaders("../shaders/ParticleShader.vert","../shaders/ParticleShader.frag");
    lightingProgram = loadShaders("../shaders/ShadowMapping.vert","../shaders/ShadowMapping.frag");
    depthProgram = loadShaders("../shaders/Depth.vert","../shaders/Depth.frag");

    // Draw wire frame triangles or fill: GL_LINE, or GL_FILL
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // lighting shader
    // --- shaderProgram ---
	projectionMatrixLocation = glGetUniformLocation(lightingProgram, "P");
	viewMatrixLocation = glGetUniformLocation(lightingProgram, "V");
	modelMatrixLocation = glGetUniformLocation(lightingProgram, "M");
	// for phong lighting
	KaLocation = glGetUniformLocation(lightingProgram, "mtl.Ka");
	KdLocation = glGetUniformLocation(lightingProgram, "mtl.Kd");
	KsLocation = glGetUniformLocation(lightingProgram, "mtl.Ks");
	NsLocation = glGetUniformLocation(lightingProgram, "mtl.Ns");
	LaLocation1 = glGetUniformLocation(lightingProgram, "light1.La");
	LdLocation1 = glGetUniformLocation(lightingProgram, "light1.Ld");
	LsLocation1 = glGetUniformLocation(lightingProgram, "light1.Ls");
	lightPositionLocation1 = glGetUniformLocation(lightingProgram, "light1.lightPosition_worldspace");
	lightPowerLocation1 = glGetUniformLocation(lightingProgram, "light1.power");

	diffuseColorSampler = glGetUniformLocation(lightingProgram, "diffuseColorSampler");
	specularColorSampler = glGetUniformLocation(lightingProgram, "specularColorSampler");

	useTextureLocation = glGetUniformLocation(lightingProgram, "useTexture"); 
    useSpecularTextureLocation = glGetUniformLocation(lightingProgram, "useSpecularTexture"); 
    is_animation = glGetUniformLocation(lightingProgram, "is_animation");

    char name[64];
    for ( int i = 0; i < MAX_BONES; i++ ) {
        sprintf( name, "bone_matrices[%i]", i );
        bone_matrices_locations[i] = glGetUniformLocation( lightingProgram, name );
        glUniformMatrix4fv( bone_matrices_locations[i], 1, GL_FALSE, identity_mat4().m );
    }

	// locations for shadow rendering
	depthMapSampler1 = glGetUniformLocation(lightingProgram, "shadowMapSampler1");
	lightVPLocation1 = glGetUniformLocation(lightingProgram, "lightVP1");

	// --- depthProgram ---
	shadowViewProjectionLocation = glGetUniformLocation(depthProgram, "VP");
	shadowModelLocation = glGetUniformLocation(depthProgram, "M");

    // shaderProgram
    MVPLocation = glGetUniformLocation(shaderProgram, "MVP");
    textureSampler = glGetUniformLocation(shaderProgram, "textureSampler");
    
    // particle shader
    projectionAndViewMatrix = glGetUniformLocation(particleShaderProgram, "PV");
    diffuceColorSampler = glGetUniformLocation(particleShaderProgram, "texture0");


	// -  depth framebuffer and a texture to store the depthmap - //
	// ---------------------------------------------------------------------------- //
	//*/
	// Tell opengl to generate a framebuffer
	glGenFramebuffers(1, &depthFrameBuffer);
	// Binding the framebuffer, all changes bellow will affect the binded framebuffer
	// **Don't forget to bind the default framebuffer at the end of initialization
	glBindFramebuffer(GL_FRAMEBUFFER, depthFrameBuffer);

	// We need a texture to store the depth image
	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	// Telling opengl the required information about the texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0,
		GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);							// Task 4.5 Texture wrapping methods
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);							// GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
	//*/

	// Task 4.5 Don't shadow area out of light's viewport
	
	// Step 1 : (Don't forget to comment out the respective lines above
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	// Set color to set out of border 
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	// Next go to fragment shader and add an iff statement, so if the distance in the z-buffer is equal to 1, 
	// meaning that the fragment is out of the texture border (or further than the far clip plane) 
	// then the shadow value is 0.
	//*/

	//*/ Task 3.2 Continue
	// Attaching the texture to the framebuffer, so that it will monitor the depth component
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

	// Since the depth buffer is only for the generation of the depth texture, 
	// there is no need to have a color output
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);


	// Finally, we have to always check that our frame buffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		glfwTerminate();
		throw runtime_error("Frame buffer 1 not initialized correctly");
	}

	// Binding the default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


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
    
    // tower 
    tower = new Drawable("../OBJ_files/Plasma.obj");
    tower_main_texture = loadSOIL("../Textures/Maps/Bakedtexture.png");
    tower_specular_texture = loadSOIL("../Textures/Maps/template.jpg");
    tower_diffuse_texture = loadSOIL("../Textures/Maps/Powergenorator_diffuse.png");
    
    // sun
    model2 = new Drawable("../OBJ_files/sphere.obj");

	std::vector<vec3> objNormals,objVertices;
	std::vector<vec2> objUVs;
	
	for (int i=0; i<model2->indices.size(); i++){
		objVertices.push_back(model2->vertices[i]);
	}
	for (int i=0; i<model2->normals.size(); i+=1){
		objNormals.push_back(- model2->normals[i]);
	}
	for (int i=0; i<model2->uvs.size(); i++){
		objUVs.push_back(model2->uvs[i]);
	}
	model3 = new Drawable(objVertices,objUVs,objNormals);
    sun_texture = loadSOIL("../Textures/2k_mars.jpg");


    // plane
	vector<vec3> floorVertices = {
		vec3(-20.0f, 0.0f, -20.0f),
		vec3(-20.0f, 0.0f,  20.0f),
		vec3(20.0f,  0.0f,  20.0f),
		vec3(20.0f,  0.0f,  20.0f),
		vec3(20.0f,  0.0f, -20.0f),
		vec3(-20.0f, 0.0f, -20.0f),

	};

	// plane normals
	vector<vec3> floorNormals = {
		vec3(0.0f, 1.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f)
	};

	// plane uvs
	vector<vec2> floorUVs = {
		vec2(0.0f, 0.0f),
		vec2(0.0f, 1.0f),
		vec2(1.0f, 1.0f),
		vec2(1.0f, 1.0f),
		vec2(1.0f, 0.0f),
		vec2(0.0f, 0.0f),
	};

    plane = new Drawable(floorVertices, floorUVs, floorNormals);
    planetexture = loadSOIL("../Textures/grid/DALL·E 2023-01-22 18.51.45.png");


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
    ammo = new Drawable("../OBJ_files/ammo.obj");
    ammoTexture = loadSOIL("../Textures/ammo/box_fin_DefaultMaterial_BaseColor.tga");

    // aircraft
    vec3 position = vec3(14.0f,2.0f,0.0f);
    vec3 target = vec3(14.0f,2.0f,14.0f);
    vec3 vel = vec3(0.01f,0.01f,0.01f);
    float mass = 2.0f;
    int ammo = 500;
    int id = 0;
    first_aircraft = new Aircraft("../OBJ_files/aircraft.obj",position,vel,mass,target,ammo,id,vec3(16.0f,0.0f,16.0f));
    first_aircraft->at_tower_pos =  vec3(16.0f,0.0f,16.0f);
    first_aircraft->size =  0.08;

    position = vec3(0.0f,2.0f,14.0f);
    target = vec3(4.0f,2.0f,4.0f);
    vel = vec3(1.01f,0.01f,0.01f);
    mass = 2.0f;
    ammo = 500;
    id = 1;
    second_aircraft = new Aircraft("../OBJ_files/aircraft.obj",position,vel,mass,target,ammo,id,vec3(-2.0f,0.0f,-2.0f));
    second_aircraft->at_tower_pos = vec3(2.0f,0.0f,2.0f);
    second_aircraft->size =  0.08;

    aircraftTexture = loadSOIL("../Textures/aircraft/aircrafttank_DefaultMaterial_BaseColor.png");


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


    // rock
    rock = new Drawable("../OBJ_files/Cliff_Rock_One_OBJ.obj");
    rockTexture = loadSOIL("../Textures/Cliff_Rock_One_Texture4K/Cliff_Rock_One_BaseColor.png");

    glfwSetKeyCallback(window, pollKeyboard);
}

void free() {
    glDeleteProgram(shaderProgram);
    glDeleteProgram(particleShaderProgram);
    glDeleteProgram(lightingProgram);
    glDeleteProgram(depthProgram);

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

void lighting_pass(mat4 viewMatrix, mat4 projectionMatrix, float t, float dt){
    // Step 1: Binding a frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, W_WIDTH * 2, W_HEIGHT * 2);

	// Step 2: Clearing color and depth info
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Step 3: Selecting shader program
	glUseProgram(lightingProgram);

	// Making view and projection matrices uniform to the shader program
	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
	glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);

	// uploading the light parameters to the shader program
	
	uploadLight(*light1,LaLocation1,LdLocation1,LsLocation1,lightPositionLocation1,lightPowerLocation1);
	

	// Task 4.1 Display shadows on the scene
	//*/
	// Sending the shadow texture to the shaderProgram
	glActiveTexture(GL_TEXTURE0);								// Activate texture position
	glBindTexture(GL_TEXTURE_2D, depthTexture);			// Assign texture to position 
	glUniform1i(depthMapSampler1, 0);


	// Sending the light View-Projection matrix to the shader program
	mat4 light_VP1 = light1->lightVP();
	glUniformMatrix4fv(lightVPLocation1, 1, GL_FALSE, &light_VP1[0][0]);

    // tower 1
    float size = 0.02f;
    mat4 Scaling = glm::scale(mat4(), vec3(size,size,size));
    mat4 Translate = glm::translate(mat4(), vec3(2.0f,0.0f,2.0f));
    mat4 Tower1ModelMatrix = Translate * Scaling;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &Tower1ModelMatrix[0][0]);
	glActiveTexture(GL_TEXTURE1);								
	glBindTexture(GL_TEXTURE_2D, tower_diffuse_texture);			 						
	glActiveTexture(GL_TEXTURE2);								
	glBindTexture(GL_TEXTURE_2D, tower_specular_texture);
    glUniform1i(diffuseColorSampler, 1);		
	glUniform1i(specularColorSampler, 2);						
	glUniform1i(useTextureLocation, 1);
    glUniform1i(is_animation, 0);
    tower->bind();
    tower->draw();

    // tower 2
    size = 0.02f;
    Scaling = glm::scale(mat4(), vec3(size,size,size));
    Translate = glm::translate(mat4(), vec3(16.0f,0.0f,16.0f));
    mat4 Tower2ModelMatrix = Translate * Scaling;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &Tower2ModelMatrix[0][0]);
    glUniform1i(diffuseColorSampler, 1);
    glUniform1i(specularColorSampler, 2);
    glUniform1i(useTextureLocation, 1);
    glUniform1i(useSpecularTextureLocation, 1);
    tower->bind();
    tower->draw();


    size = 2.0f;
    Scaling = glm::scale(mat4(), vec3(0.5, 0.5, 0.5));
    Translate = glm::translate(mat4(), vec3(9.0f,0.0f,9.0f));
    mat4 planeMatrix = Translate * Scaling;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &planeMatrix[0][0]);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, planetexture);
    glUniform1i(diffuseColorSampler, 3);
    glUniform1i(specularColorSampler, 3);
    glUniform1i(useTextureLocation, 1);
    glUniform1i(useSpecularTextureLocation, 1);
    plane->bind();
    plane->draw();

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
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &first_aircraft->modelMatrix[0][0]);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, aircraftTexture);
    glUniform1i(diffuseColorSampler, 4);
    glUniform1i(useTextureLocation, 1);
    glUniform1i(useSpecularTextureLocation, 0);
    first_aircraft->bind();
    first_aircraft->draw();

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
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &second_aircraft->modelMatrix[0][0]);
    glUniform1i(diffuseColorSampler, 4);
    glUniform1i(useTextureLocation, 1);
    glUniform1i(useSpecularTextureLocation, 0);
    second_aircraft->bind();
    second_aircraft->draw();

    // ammo
    vec3 ammo_position;
    size = 0.03;
    Scaling = glm::scale(mat4(), vec3(size,size,size));
    mat4 Rotate = glm::rotate(mat4(), t*3.14f/5.0f, vec3(0.0f,1.0f,0.0f));
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, ammoTexture);
    glUniform1i(diffuseColorSampler, 5);
    glUniform1i(useTextureLocation, 1);
    glUniform1i(useSpecularTextureLocation, 0);
    ammo->bind();
    for (int i=0; i<ammo_packages.size(); i++){
        ammo_position = ammo_packages[i].position;
        Translate = glm::translate(mat4(),ammo_position);
        mat4 ammoModelMatrix = Translate * Rotate * Scaling;
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &ammoModelMatrix[0][0]);
        ammo->draw();
    }

    // robots
    vector<float> f0(6, 0.0f);
    vec3 force0;
    glActiveTexture(GL_TEXTURE6);
    robots[0]->bindTexture();
    glUniform1i(is_animation, 1);
    glUniform1i(diffuseColorSampler, 6);
    glUniform1i(useTextureLocation, 1);
    glUniform1i(useSpecularTextureLocation, 0);
    for (int i=0; i<robots.size(); i++){
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
            robot->skeleton_animate(robot->root_node,robot->anim_time, identity_mat4(),robot->bone_offset_matrices, robot->bone_animation_mats );
            glUniformMatrix4fv( modelMatrixLocation, 1, GL_FALSE, &robot->modelMatrix[0][0]);
            glUniformMatrix4fv( bone_matrices_locations[0], robot->bone_count, GL_FALSE, robot->bone_animation_mats[0].m );
            robot->bind();
            robot->draw();
        }
        else if (game) robot->handleHealth(); 
    }

    // rock
    size = 0.01f;
    Scaling = glm::scale(mat4(), vec3(size,size,size));
    Translate = glm::translate(mat4(), vec3(16.0f,0.0f,2.0f));
    mat4 rockModelMatrix = Translate * Scaling;
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &rockModelMatrix[0][0]);
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D, rockTexture);
    glUniform1i(is_animation, 0);
    glUniform1i(diffuseColorSampler, 7);
    glUniform1i(useTextureLocation, 1);
    glUniform1i(useSpecularTextureLocation, 0);
    rock->bind();
    rock->draw();

    Scaling = glm::scale(mat4(), vec3(0.1, 0.1, 0.1));
	Translate = translate(mat4(), light1->lightPosition_worldspace);
	mat4 modelMatrix3 = Translate * Scaling;
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix3[0][0]);
	glActiveTexture(GL_TEXTURE8);								
	glBindTexture(GL_TEXTURE_2D, sun_texture);
    glUniform1i(is_animation, 0);
	glUniform1i(useTextureLocation, 1);
    glUniform1i(diffuseColorSampler, 8);
    glUniform1i(useSpecularTextureLocation, 0);
	model3->bind();
	model3->draw();
}

void mainLoop() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    light1->update();
	mat4 light_proj1 = light1->projectionMatrix;
	mat4 light_view1 = light1->viewMatrix;

    mat4 projectionMatrix,viewMatrix;
    mat4 Scaling,Rotate,Translate;

    float t = glfwGetTime();
    float t_particles = glfwGetTime();


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
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable( GL_DEPTH_TEST );

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        glClearColor(background_color[0], background_color[1], background_color[2], background_color[3]);

        check_game();

        // float dt = glfwGetTime()-t;
        float dt = 0.1;

        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS){
			light1->update();
			light_proj1 = light1->projectionMatrix;
			light_view1 = light1->viewMatrix;
		}

        camera->update();
        projectionMatrix = camera->projectionMatrix;
        viewMatrix = camera->viewMatrix;
        
        lighting_pass(viewMatrix,projectionMatrix,t,dt);

        // use shaderProgram
        glUseProgram(shaderProgram);

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
        vec3 katheto;
        int j;
        for (int i=0; i<robots.size(); i++){
            j=i+2;
            katheto = glm::cross(normalize(robots[i]->direction),vec3(0.0f,1.0f,0.0f));
            bullet_emitters[2*j+1].emitter_pos = robots[i]->x + katheto*0.3f;
            bullet_emitters[2*j+1].bullet_target = robots[i]->target;

            bullet_emitters[2*j].emitter_pos = robots[i]->x - katheto*0.3f;
            bullet_emitters[2*j].bullet_target = robots[i]->target;
        }
        // robot0 -> 4,5
        // robot1 -> 6,7
        // robot3 -> 8,9
        // robot4 -> 10,11
        // robot5 -> 12,13 
        katheto = glm::cross(normalize(first_aircraft->direction),vec3(0.0f,1.0f,0.0f));
        bullet_emitters[0].emitter_pos = first_aircraft->x + katheto*0.5f + vec3(0.0f,0.2f,0.0f);
        bullet_emitters[0].bullet_target = vec3(16.0f,2.0f,16.0f);
        bullet_emitters[1].emitter_pos = first_aircraft->x - katheto*0.5f + vec3(0.0f,0.2f,0.0f);
        bullet_emitters[1].bullet_target = vec3(16.0f,2.0f,16.0f);
        katheto = glm::cross(normalize(second_aircraft->direction),vec3(0.0f,1.0f,0.0f));
        bullet_emitters[2].emitter_pos = second_aircraft->x + katheto*0.5f + vec3(0.0f,0.2f,0.0f);
        bullet_emitters[2].bullet_target = vec3(2.0f,2.0f,2.0f);
        bullet_emitters[3].emitter_pos = second_aircraft->x - katheto*0.5f + vec3(0.0f,0.2f,0.0f);
        bullet_emitters[3].bullet_target = vec3(2.0f,2.0f,2.0f);


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
	// enable texturing and bind the depth texture
	glEnable(GL_TEXTURE_2D);


	// Task 1.1 Creating a light source
	// Creating a custom light 
	light1 = new Light(window,
		vec4{ 1, 0.7, 0.7, 1 },
		vec4{ 1, 0.7, 0.7, 1 },
		vec4{ 1, 0.7, 0.7, 1 },
		vec3{ 9, 4, 9 },
		2.0f
	);

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