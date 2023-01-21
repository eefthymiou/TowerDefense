#include "BulletEmitter.h"
#include <iostream>
#include <algorithm>
using namespace glm;


BulletEmitter::BulletEmitter(Drawable *_model, int number): IntParticleEmitter(_model, number){}

void BulletEmitter::updateParticles(float time, float dt,  glm::vec3 camera_pos){
    float prev_distance;
    if (active_particles < number_of_particles && timmer > 10.0f) {
        timmer = 0.0f;
        createNewParticle(active_particles);
        active_particles++;
        
    }
    else {
        timmer += 0.1f;
    }

    for(int i = 0; i < active_particles; i++){
        particleAttributes & particle = p_attributes[i];

        prev_distance = particle.distance;
        particle.distance = length(particle.position-bullet_target);

        if(prev_distance < particle.distance || checkForCollision(particle)){
            createNewParticle(i);
        }

        particle.position = particle.position + particle.velocity*dt;

        particle.rot_axis = cross(vec3(1.0f, 0.0f, 0.0f), direction);
        particle.rot_angle = glm::degrees(acos(dot(glm::vec3(1.0f, 0.0f, 0.0f), direction)));

        particle.life = 1.0f;
    }
    
}

bool BulletEmitter::checkForCollision(particleAttributes& particle)
{
   if  (particle.position.x > 18.0f || particle.position.x<0.0f || particle.position.z>18.0f || particle.position.z<0.0f) return true;
}

void BulletEmitter::createNewParticle(int index){
    particleAttributes & particle = p_attributes[index];

    particle.position = emitter_pos;
    direction = bullet_target - particle.position ;
    particle.distance = length(direction);

    direction = normalize(direction);
    particle.velocity =  direction * 15.0f;
    
    
    particle.mass = 4.0f;
    particle.rot_axis = cross(vec3(1.0f, 0.0f, 0.0f), direction);
    particle.rot_angle = glm::degrees(acos(dot(glm::vec3(1.0f, 0.0f, 0.0f), direction)));
    particle.life = 1.0f; //mark it alive
}
