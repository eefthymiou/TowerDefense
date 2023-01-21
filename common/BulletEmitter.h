#ifndef BULLETEMITTER_H
#define BULLETEMITTER_H

#include "IntParticleEmitter.h"


class BulletEmitter : public IntParticleEmitter {
    public:
        BulletEmitter(Drawable* _model, int number);

        // float 
        glm::vec3 bullet_target;
        glm::vec3 direction;
        float timmer=0.0f;
        bool checkForCollision(particleAttributes& particle);

        int active_particles = 0; //number of particles that have been instantiated
        void createNewParticle(int index) override;
        void updateParticles(float time, float dt, glm::vec3 camera_pos = glm::vec3(0, 0, 0)) override;
};
#endif