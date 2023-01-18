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

#include "robot.h"

using namespace glm;
using namespace std;

Robot::Robot(const char*  path,vec3 pos,vec3 vel,float mass,vec3 t)
    : Animation(path), Moving_obj(pos,vel,mass,t) {
    maxspeed = 0.2;
    anim_time = 0.0f;
}


void Robot::update(float t, float dt){
    advanceState(t, dt);
    
    if ( anim_time >= anim_duration ) { anim_time = anim_duration - anim_time; }
    anim_time += length(v)*150.0f;
    
    vec3 v1 = v;
    v1.y = 0;
    

    float angle_x = acos(dot(glm::vec3(1.0f, 0.0f, 0.0f), normalize(v1)));
    vec3 axis_x = cross(vec3(1.0f, 0.0f, 0.0f), normalize(v1));
    mat4 rotation_x = rotate(glm::mat4(1.0f), angle_x, axis_x);
    mat4 rotation = rotate(glm::mat4(1.0f),3.14f,vec3(0.0f,1.0f,0.0f));

    mat4 Translate = glm::translate(mat4(), x);
    mat4 Scaling = glm::scale(mat4(), vec3(size,size,size));
    modelMatrix = Translate * Scaling * rotation * rotation_x;
}

