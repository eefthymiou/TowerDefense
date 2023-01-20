#ifndef MOVING_OBJECT_H
#define MOVING_OBJECT_H

#include "RigidBody.h"

class Moving_obj :  public RigidBody {
public:
    float maxspeed,maxforce;
    glm::vec3 target;
    glm::vec3 initial_pos;
    bool moving;

    Moving_obj(glm::vec3 pos,glm::vec3 vel,float mass,glm::vec3 t);
    glm::vec3 seek();
};



#endif