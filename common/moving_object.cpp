// Include C++ headers
#include <iostream>
#include <string>
#include <vector>

#include "moving_object.h"

using namespace glm;
using namespace std;

Moving_obj::Moving_obj(vec3 pos,vec3 vel,float mass,vec3 t)
    : RigidBody() {
    m = mass;
    x = pos;
    v = vel;
    P = m * v;
    target = t;
    initial_pos = pos;
    maxspeed = 1.0f;
    maxforce = 1.0f;
    moving = true;
}

vec3 Moving_obj::seek(){
    vec3 desired = target-x;
    vec3 steer;
    // cout << length(v) << endl;
    float distance = length(x-target);
    // cout << distance << endl;

    if (distance<6.0f) {
        // the aircraft soon arrives to the target point
        float m = (distance/6.0f) * (maxspeed);
        desired = normalize(desired) * m; 
    }

    else desired = normalize(desired) * maxspeed;
    steer = desired-v;

    steer = glm::clamp(steer, -maxforce, maxforce);
    // cout << length(steer) << endl;
    return steer;
}



