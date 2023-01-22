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


glm::vec3 get_random_pos();

class Aircraft :  public Moving_obj {
public:
    Aircraft(glm::vec3 pos,glm::vec3 vel,float mass,glm::vec3 t,int a,int id,glm::vec3 tower_pos);
public:
    int ammo;
    int id;
    bool find_ammo;
    float size;
    glm::vec3 initial_target;
    glm::vec3 at_tower_pos;
    glm::mat4 modelMatrix;
    glm::vec3 direction;
    glm::vec3 real_tower_pos;
    bool shoots;

    void update(float t, float dt);
    void handle_ammo(std::vector<package_ammo> *ammo_posisions,int *enemy_tower_health);
    void sortest_path_for_ammo(std::vector<package_ammo> *ammo_positions);

    static void load_mesh(std::string mesh_path);
    static void loadTexture(const std::string& filename);
    void bind();
    void bindTexture();
    void draw();


private:
    static void createContext();
    void erase_package(std::vector<package_ammo> *ammo_packages);
};
#endif

