#include "header\ParticleSystem.h"
#include "header/Constant.h"

vector<vector<int>> DIRECTION3D = { { 1, 0, 0 },{ -1, 0, 0 },{ 0, 1, 0 },{ 0, -1, 0 },{ 0, 0, 1 },{ 0, 0, -1 },
									{ 1, 1, 0 },{ 1, -1, 0 },{-1, 1, 0 },{ -1, -1, 0 },
									{ 1, 0, 1 },{ 1, 0, -1 },{ -1, 0, 1 },{ -1, 0, -1 }, 
									{ 0, 1, 1 },{ 0, -1, 1 },{ 0, 1, -1 },{ 0, -1, -1 }, 
									{ 1, 1, 1 },{ -1, 1, 1 },{ 1, -1, 1 },{ 1, 1, -1 }, 
									{ -1, -1, 1 },{ -1, 1, -1 },{ 1, -1, -1 },{ -1, -1, -1 } };

ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::createParticleSystem()
{
	double widthX = xCnt * INIT_PARTICLE_DISTANCE;
	double widthY = yCnt * INIT_PARTICLE_DISTANCE;
	double widthZ = zCnt * INIT_PARTICLE_DISTANCE;
	
	int id = 1;
	for (int i = 0; i < xCnt; ++i) {
		for (int j = 0; j < yCnt; ++j) {
			for (int k = 0; k < zCnt; ++k) {
				vec3 pos(- widthX / 2 + i * INIT_PARTICLE_DISTANCE, - widthY / 2 + j * INIT_PARTICLE_DISTANCE, - widthZ / 2 + k * INIT_PARTICLE_DISTANCE);
				particles.emplace_back(make_shared<Particle>(pos, id++));
			}
		}
	}

	//initialize neighbor grid
	int xCell = int(BOX_SIZE_X / KERNEL_RADIUS);
	int yCell = int(BOX_SIZE_Y / KERNEL_RADIUS);
	int zCell = int(BOX_SIZE_Z / KERNEL_RADIUS);
	//
	cellCount = ivec3(xCell, yCell, zCell);
	grid.resize(xCell, vector<vector<unordered_map<shared_ptr<Particle>, int>>>(yCell, vector<unordered_map<shared_ptr<Particle>, int>>(zCell, unordered_map<shared_ptr<Particle>, int>())));
	populateNeighborGrid();
}

vector<ivec3> ParticleSystem::getNeighborCells(int pId) {
	shared_ptr<Particle> p = particles[pId - 1];
	//add all map of neighbor cell if valid cell
	vector <ivec3> res;
	for (auto d : DIRECTION3D) {
		int newX = p->cellPosition.x + d[0];
		int newY = p->cellPosition.y + d[1];
		int newZ = p->cellPosition.z + d[2];
		if (newX >= 0 && newY >= 0 && newZ >= 0 && newX < cellCount.x && newY < cellCount.y && newZ < cellCount.z) {
			res.emplace_back(ivec3(newX, newY, newZ));
		}
	}
	return res;
}

bool ParticleSystem::checkIsNeighbor(int pId1, int pId2) const {
	shared_ptr<Particle> p1 = particles[pId1 - 1];
	shared_ptr<Particle> p2 = particles[pId2 - 1];
	if ((abs(p1->cellPosition.x - p2->cellPosition.x) <= 1) &&
		(abs(p1->cellPosition.y - p2->cellPosition.y) <= 1) &&
		(abs(p1->cellPosition.z - p2->cellPosition.z) <= 1)) {
		return true;
	}
	return false;
}

void ParticleSystem::populateNeighborGrid() {
	for (unsigned int i = 0; i < particles.size(); ++i) {
		ivec3 cell = particles[i]->cellPosition;
		grid[cell.x][cell.y][cell.z][particles[i]] = 1;
		//particles[i]->printDebug();
	}
}
