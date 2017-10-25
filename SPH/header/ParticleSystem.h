#ifndef _H_PARTICLE_SYSTEM_
#define _H_PARTICLE_SYSTEM_
#include <memory>
#include <vector>
#include "Particle.h"
using namespace std;
class ParticleSystem
{
public:
	ParticleSystem() {
		xCnt = 1;
		yCnt = 1;
		zCnt = 1;
	};
	ParticleSystem(int _xCnt, int _yCnt, int _zCnt) {
		xCnt = _xCnt;
		yCnt = _yCnt;
		zCnt = _zCnt;
	}
	virtual ~ParticleSystem();

public:
	void createParticleSystem();
	vector<shared_ptr<Particle>>& getParticles() { return particles; }

private: 
	int xCnt, yCnt, zCnt;
	vector<shared_ptr<Particle>> particles;
};

#endif
