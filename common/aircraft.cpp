// Include C++ headers
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>


// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include "aircraft.h"

using namespace glm;
using namespace std;



Aircraft::Aircraft(vec3 pos,vec3 vel,float mass,vec3 t,int a,int i) : 
    Moving_obj(pos,vel,mass,t) {
        ammo = a;
        id = i;
        initial_target = t;
        find_ammo = false;
}


void Aircraft::sortest_path_for_ammo(std::vector<package_ammo> *ammo_packages){
    float dis;
    float min_dis = length(x-(*ammo_packages)[0].position);
    int thesi=0;

    for (int i=1; i<(*ammo_packages).size(); i++){
        if ((*ammo_packages)[i].available == true){
            dis = length((*ammo_packages)[i].position - x);
            if (dis<min_dis) {
                min_dis = dis;
                thesi = i;
            }
        }
    }
    // set ammo_package available to false
    (*ammo_packages)[thesi].available = false;
    (*ammo_packages)[thesi].who_can_erase_it = id;
    target = (*ammo_packages)[thesi].position;

}

vec3 get_random_pos(){
    int N = 10;
    float x = rand() % N + 4.0f;
    float y = rand() % N + 0.5f;
    float z = rand() % N + 4.0f;
    return vec3(x,y,z);
}


void generate_package(std::vector<package_ammo> *ammo_packages){
    package_ammo temp_package_ammo;
    vec3 position;

    temp_package_ammo.position = get_random_pos();
    temp_package_ammo.available = true;
    (*ammo_packages).push_back(temp_package_ammo);
}

void Aircraft::erase_package(std::vector<package_ammo> *ammo_packages){
    vector<package_ammo>::iterator it;
    it = (*ammo_packages).begin();
    
    for (int i=0; i<(*ammo_packages).size(); i+=1){
        if ((*ammo_packages)[i].available == false && (*ammo_packages)[i].who_can_erase_it == id){
            (*ammo_packages).erase(it+i);
        }
    }
    generate_package(ammo_packages);
}


// true->it fires in tower
bool Aircraft::handle_ammo(std::vector<package_ammo> *ammo_packages){
    float distance = length(x-target);
    // cout << ammo << endl;
    if (ammo>0 && distance<5.0f){
        // start shoot... decreases ammo
        ammo-=1;
        return true;
    }
    else if  (ammo==0 && distance<1.0f && find_ammo == true){
        // aircraft now has ammo... return to the tower
        erase_package(ammo_packages);
        cout << "ammo full. RETURN TO TOWER" << endl;
        target = initial_target;
        ammo = 500;
        find_ammo = false;
        arrives = true;
        return false;
    }

    else if (ammo==0 && find_ammo==false){
        // target now is to find ammo
        cout << "out of ammo" << endl;
        sortest_path_for_ammo(ammo_packages);
        find_ammo = true;
        arrives = false;
        return false;
    }
    return false;
}

void Aircraft::update(float t, float dt) {
    //integration
    advanceState(t, dt);

    vec3 v1 = v;
    v1.y = 0;

    float angle_x = acos(dot(glm::vec3(1.0f, 0.0f, 0.0f), normalize(v1)));
    vec3 axis_x = cross(vec3(1.0f, 0.0f, 0.0f), normalize(v1));
    mat4 rotation_x = rotate(glm::mat4(1.0f), angle_x, axis_x);
    mat4 tranlation = translate(mat4(), vec3(x.x, x.y, x.z));
    mat4 scale = glm::scale(mat4(), vec3(size, size, size));
    mat4 rotation = glm::rotate(mat4(), glm::radians(180.0f), vec3(0.0f,1.0f,0.0f));
    modelMatrix = tranlation * rotation * rotation_x * scale;
}