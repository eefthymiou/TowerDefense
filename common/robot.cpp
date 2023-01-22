// Include C++ headers
#include <iostream>
#include <string>
#include <vector>
#include <math.h>

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
    initial_pos = pos;
    initial_vel = vel;
}

void Robot::findTarget(std::vector<Robot*> *robots){
    float dis;
    float min_dis = 100.0f;
    float thesi = 0;
    has_enemy_robot = false;

    for (int i=0; i<(*robots).size(); i++){
        // cheack if robots are not in the same team
        if ((*robots)[i]->team_tower != team_tower && (*robots)[i]->health > 0.0f) {
            dis = length((*robots)[i]->x - x);
            if (dis<=min_dis){
                thesi = i;
                has_enemy_robot = true;
                min_dis = dis;
            }
        }
    }
    if (has_enemy_robot){
        target = (*robots)[thesi]->x;
        enemy_robot = (*robots)[thesi];
    }
    else target = enemy_tower_pos;
}


void Robot::handleShooting(int *enemy_tower_health){
    float dis;
    shoots = false;
    if (has_enemy_robot){
        dis = length(x-enemy_robot->x);
        direction = v;
        direction.y = 0;
        if (dis<10.0f) {
            enemy_robot->health -= (0.01 * (1/dis));
            shoots = true;
        }
    }
    else {
        dis = length(x-enemy_tower_pos);
        if (team_tower==2) direction = -enemy_tower_pos;
        else direction = enemy_tower_pos;
        direction.y = 0;
        if (dis<10.0f){
            (*enemy_tower_health) -= 1 * (1/length(x-enemy_tower_pos)<5.0f);
            shoots = true;
        }
    }
}

void Robot::handleHealth(){
    shoots = false;
    int r = std::rand() % 800;
    if (r==0){
        health = 1.0f;
        x = initial_pos;
        v = initial_vel;
    }
}



void Robot::update(float t, float dt){
    advanceState(t, dt);
    
    if ( anim_time >= anim_duration ) { anim_time = anim_duration - anim_time; }
    anim_time += length(v)*150.0f;
    
    float size = 0.01;
    float angle_x = acos(dot(glm::vec3(1.0f, 0.0f, 0.0f), normalize(direction)));
    vec3 axis_x = cross(vec3(1.0f, 0.0f, 0.0f), normalize(direction));
    mat4 rotation_x = rotate(glm::mat4(1.0f), angle_x, axis_x);
    mat4 rotation = rotate(glm::mat4(1.0f),3.14f,vec3(0.0f,1.0f,0.0f));

    mat4 Translate = glm::translate(mat4(), x);
    mat4 Scaling = glm::scale(mat4(), vec3(size,size,size));
    modelMatrix = Translate * Scaling * rotation * rotation_x;
}

