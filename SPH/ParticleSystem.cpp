#include "header\ParticleSystem.h"

ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::createParticleSystem()
{
	shared_ptr<Particle> p(new Particle(Vec3(1.0, 1.0, 1.0)));
	particles.push_back(p);
}
