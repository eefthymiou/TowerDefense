#ifndef ROBOT_H
#define ROBOT_H

#include "animation.h"
#include "moving_object.h"

class Robot : public Animation, public Moving_obj{
public:
    Robot(const char* path,glm::vec3 pos,glm::vec3 vel,float mass,glm::vec3 t,int tower,float m_speed);
   
    glm::mat4 modelMatrix;
    glm::vec3 enemy_tower_pos;
    glm::vec3 initial_pos;
    glm::vec3 initial_vel;
    glm::vec3 direction;
    float anim_time;
    int team_tower;
    float health = 1.0f;
    bool alive;
    float timmer=0.0f;
    Robot* enemy_robot;
    bool has_enemy_robot=false;

    void update(float t, float dt);
    void findTarget(std::vector<Robot*> *robots);
    void handleShooting(int *enemy_tower_health);
    void handleHealth();
};

#endif
