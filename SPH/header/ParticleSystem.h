#ifndef _H_PARTICLE_SYSTEM_
#define _H_PARTICLE_SYSTEM_
#include <memory>
#include <vector>
#include "Particle.h"
#include <unordered_map>
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
	void update();

	vector<shared_ptr<Particle>>& getParticles() { return particles; }
	vector<ivec3> getNeighborCells(int pId);
	ivec3 getCellCount() const { return cellCount; }
	bool checkIsNeighbor(int pId1, int pId2) const;

private:
	void populateNeighborGrid();
	void calcDensityPressure();
	void calcForces();

private: 
	ivec3 cellCount;
	int xCnt, yCnt, zCnt;
	vector<shared_ptr<Particle>> particles;
	vector<vector<vector<unordered_map<shared_ptr<Particle>, int>>>> grid;
};

#endif
