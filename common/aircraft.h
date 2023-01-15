#ifndef AIRCRAFT_H
#define AIRCRAFT_H

#include <GL/glew.h>
#include <string>
#include <map>
#include <glm/glm.hpp>
#include <vector>
#include "moving_object.h"


struct package_ammo {
  glm::vec3 position;
  bool available=true;
  int who_can_erase_it;
};

class Aircraft :  public Moving_obj {
public:
    Aircraft(glm::vec3 pos,glm::vec3 vel,float mass,glm::vec3 t,int a,int id);
public:
    int ammo;
    int id;
    bool find_ammo;
    glm::vec3 initial_target;
    
    glm::vec3 seek();
    bool handle_ammo(std::vector<package_ammo> *ammo_posisions);
    void sortest_path_for_ammo(std::vector<package_ammo> *ammo_positions);
private:
    void erase_package(std::vector<package_ammo> *ammo_packages);
};
#endif

