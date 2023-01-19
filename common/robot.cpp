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

Robot::Robot(const char*  path,vec3 pos,vec3 vel,float mass,vec3 t,int tower,float m_speed)
    : Animation(path), Moving_obj(pos,vel,mass,t) {
    maxspeed = m_speed;
    anim_time = 0.0f;
    // team tower could be 1 or 2
    team_tower = tower;
    enemy_tower_pos = t;
}

void Robot::findTarget(std::vector<Robot*> *robots){
    Robot* target_robot;
    for (int i=0; i<(*robots).size(); i++){
        // cheack if robots are not in the same team
        if ((*robots)[i]->team_tower != team_tower && (*robots)[i]->health > 0.0f) {
            target = (*robots)[i]->x;
            enemy_robot = (*robots)[i];
            has_enemy_robot = true;
            return;
        }
    }
    has_enemy_robot = false;
    target = enemy_tower_pos;
}

void Robot::handleShooting(int *enemy_tower_health){
    if (has_enemy_robot){
        enemy_robot->health -= 0.002;
    }
    else {
        (*enemy_tower_health) -= 1;
    }
}


void Robot::update(float t, float dt){
    advanceState(t, dt);
    
    if ( anim_time >= anim_duration ) { anim_time = anim_duration - anim_time; }
    anim_time += length(v)*150.0f;
    
    vec3 v1 = v;
    v1.y = 0;
    
    float size = 0.01;
    float angle_x = acos(dot(glm::vec3(1.0f, 0.0f, 0.0f), normalize(v1)));
    vec3 axis_x = cross(vec3(1.0f, 0.0f, 0.0f), normalize(v1));
    mat4 rotation_x = rotate(glm::mat4(1.0f), angle_x, axis_x);
    mat4 rotation = rotate(glm::mat4(1.0f),3.14f,vec3(0.0f,1.0f,0.0f));

    mat4 Translate = glm::translate(mat4(), x);
    mat4 Scaling = glm::scale(mat4(), vec3(size,size,size));
    modelMatrix = Translate * Scaling * rotation * rotation_x;
}

