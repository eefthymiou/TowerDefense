#ifndef AIRCRAFT_H
#define AIRCRAFT_H

#include <GL/glew.h>
#include <string>
#include <map>
#include <glm/glm.hpp>
#include "moving_object.h"

class Aircraft :  public Moving_obj {
public:
    Aircraft(glm::vec3 pos,glm::vec3 vel,float mass,glm::vec3 t,int a);
public:
    int ammo;
    bool find_ammo;
    glm::vec3 initial_target;
    
    glm::vec3 seek();
    bool handle_ammo(glm::vec3 pos_ammo);
};
#endif

