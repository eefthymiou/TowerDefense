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

Aircraft::Aircraft(vec3 pos,vec3 vel,float mass,vec3 t,int a) : 
    Moving_obj(pos,vel,mass,t) {
        ammo = a;
        initial_target = t;
        find_ammo = false;
}

vec3 Aircraft::seek(){
    vec3 desired = target-x;
    vec3 steer;
    // cout << length(v) << endl;
    float distance = length(x-target);
    // cout << distance << endl;
    if (distance<4.0f && arrives) {
        // the aircraft soon arrives to the target point
        float m = (distance/4.0f) * (maxspeed);
        desired = normalize(desired) * m; 
        steer = desired-v;
    }
    else {
        desired = normalize(desired) * maxspeed;
        steer = desired-v;
    }
    steer = glm::clamp(steer, -maxforce, maxforce);
    // cout << length(steer) << endl;
    return steer;
}


bool Aircraft::handle_ammo(vec3 ammo_pos){
    float distance = length(x-target);
    // cout << ammo << endl;
    if (ammo>0 && distance<5.0f){
        // start shoot... decreases ammo
        ammo-=1;
        return false;
    }
    else if  (ammo==0 && distance<1.0f && find_ammo == true){
        // aircraft now has ammo... return to the tower
        cout << "ammo full. RETURN TO TOWER" << endl;
        target = initial_target;
        ammo = 1000;
        find_ammo = false;
        arrives = true;
        return true;
    }

    else if (ammo==0 && find_ammo==false){
        // target now is to find ammo
        cout << "out of ammo" << endl;
        target = ammo_pos;
        find_ammo = true;
        arrives = false;
        return false;
    }
    return false;
}