#include "header\ParticleSystem.h"
#include "header/Constant.h"

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
				shared_ptr<Particle> p(new Particle(pos, id++));
				particles.push_back(p);
			}
		}
	}

	//initialize neighbor grid
	int xCell = BOX_SIZE_X / KERNEL_RADIUS;
	int yCell = BOX_SIZE_Y / KERNEL_RADIUS;
	int zCell = BOX_SIZE_Z / KERNEL_RADIUS;
	grid.resize(xCell, vector<vector<unordered_map<shared_ptr<Particle>, int>>>(yCell, vector<unordered_map<shared_ptr<Particle>, int>>(zCell, unordered_map<shared_ptr<Particle>, int>())));
	populateNeighborGrid();
}

void ParticleSystem::populateNeighborGrid() {
	for (int i = 0; i < particles.size(); ++i) {
		ivec3 cell = particles[i]->cellPosition;
		grid[cell.x][cell.y][cell.z][particles[i]] = 1;
		//particles[i]->printDebug();
	}
}
