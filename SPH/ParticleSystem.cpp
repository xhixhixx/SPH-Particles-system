#include "header\ParticleSystem.h"

ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::createParticleSystem()
{
	double gap = 0.5;
	double widthX = xCnt * gap;
	double widthY = yCnt * gap;
	double widthZ = zCnt * gap;
	
	for (int i = 0; i < xCnt; ++i) {
		for (int j = 0; j < yCnt; ++j) {
			for (int k = 0; k < zCnt; ++k) {
				Vec3 pos(- widthX / 2 + i * gap, - widthY / 2 + j * gap, - widthZ / 2 + k * gap);
				shared_ptr<Particle> p(new Particle(pos));
				particles.push_back(p);
			}
		}
	}
}
