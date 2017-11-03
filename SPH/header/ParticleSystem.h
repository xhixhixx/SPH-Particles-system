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

	void clearGridByThread(bool useThread = true);
	void clearGridByIndex(int start, int end);

	void calcCellPosByThread(bool useThread = true);
	void calcCellPosByIndex(int start, int end);
	
	void calcDensityPressureByThead(bool useThread = true);
	void calcDensityPressureByIndex(int start, int end);

	void calcForcesByThread(bool useThread = true);
	void calcForcesByIndex(int start, int end);

	void updatePositionByThread(bool useThread = true);
	void updatePositionByIndex(int start, int end);

private: 
	ivec3 cellCount;
	int xCnt, yCnt, zCnt;
	vector<shared_ptr<Particle>> particles;
	vector<vector<shared_ptr<Particle>>> grid;
};

#endif
