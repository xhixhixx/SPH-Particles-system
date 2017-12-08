#ifndef _H_PARTICLE_SYSTEM_
#define _H_PARTICLE_SYSTEM_
#include <memory>
#include <vector>
#include "Particle.h"
#include <unordered_map>
#include "Params.h"
using namespace std;
class ParticleSystem
{
public:
	ParticleSystem(int _xCnt, int _yCnt, int _zCnt, const Params& _params) : params(_params) {
		xCnt = _xCnt;
		yCnt = _yCnt;
		zCnt = _zCnt;
		running = false;
		sceneNum = BREAKING_DAM_1_SCENE;
	}
	virtual ~ParticleSystem();

public:
	void createParticleSystem();
	void update();

	vector<shared_ptr<Particle>>& getParticles() { return particles; }
	
	vector<ivec3> getNeighborCells(int pId); //get neighbor cell by particle id
	vector<ivec3> getNeighborCells(dvec3 pos); //get neighbor cell by coordinate
	
	ivec3 getCellCount() const { return cellCount; }
	bool checkIsNeighbor(int pId1, int pId2) const;

	void startSystem() { running = true; }
	void stopSystem() { running = false; }
	void resetSystem();
	
	int getState() { return running ? 1 : 0; }
	
	void changeScene(int sceneId) { sceneNum = sceneId; }
	int checkScene() { return sceneNum; }

	//marching cube utility
	double estimateColorFieldAtLocation(dvec3 pos);

	void exportFrames();

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

	void initBreakingDam(double wX, double wY, double wZ);

private:
	const Params& params;
	ivec3 cellCount;
	int xCnt, yCnt, zCnt;
	vector<shared_ptr<Particle>> particles;
	vector<vector<shared_ptr<Particle>>> grid;

	//
	int updateCounter;

	//State
	bool running;
	int sceneNum;
};

#endif
