#ifndef _H_PARTICLE_SYSTEM_
#define _H_PARTICLE_SYSTEM_
#include <memory>
#include <vector>
#include "Particle.h"
using namespace std;
class ParticleSystem
{
public:
	ParticleSystem() {};
	virtual ~ParticleSystem();

public:
	void createParticleSystem();
	vector<shared_ptr<Particle>>& getParticles() { return particles; }

private: 
	vector<shared_ptr<Particle>> particles;
};

#endif
