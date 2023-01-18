#ifndef ROBOT_H
#define ROBOT_H

#include "animation.h"
#include "moving_object.h"

class Robot : public Animation, public Moving_obj{
public:
    Robot(const char* path,glm::vec3 pos,glm::vec3 vel,float mass,glm::vec3 t);
   
    glm::mat4 modelMatrix;
    float size;
    float anim_time;
    void update(float t, float dt);
};

#endif
