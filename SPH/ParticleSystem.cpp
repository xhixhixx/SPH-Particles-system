#include "header\ParticleSystem.h"
#include "header/Constant.h"
#include <iostream>
#include <glm\gtx\norm.hpp>

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
	float widthX = xCnt * INIT_PARTICLE_DISTANCE;
	float widthY = yCnt * INIT_PARTICLE_DISTANCE;
	float widthZ = zCnt * INIT_PARTICLE_DISTANCE;
	
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

void ParticleSystem::update() {
	//update system
	calcDensityPressure();
	calcForces();

	//update acceleration and position
	//for all particle
	for (auto p : particles) {
		if (p->id != PARTICLE_IN_FOCUS) 
			continue;
		p->acceleration += GRAVITY;
		//Leap frog iteration
		p->position += p->velocity * TIMESTEP + p->prevAcceleration * (0.5f * TIMESTEP * TIMESTEP);
		p->velocity += (p->acceleration + p->prevAcceleration) * (0.5f * TIMESTEP);
		p->prevAcceleration = p->acceleration;
		//colision with container
		bool negate = false;
		if (p->position.x <= -OFFSET_X) {
			p->position.x = -OFFSET_X;
			negate = true;
		}
		else if (p->position.x >= OFFSET_X) {
			p->position.x = OFFSET_X;
			negate = true;
		}
		if (p->position.y <= -OFFSET_Y) {
			p->position.y = -OFFSET_Y;
			negate = true;
		}
		else if (p->position.y >= OFFSET_Y) {
			p->position.y = OFFSET_Y;
			negate = true;
		}
		if (p->position.z <= -OFFSET_Z) {
			p->position.z = -OFFSET_Z;
			negate = true;
		}
		else if (p->position.z >= OFFSET_Z) {
			p->position.z = OFFSET_Z;
			negate = true;
		}
		if (negate) {
			p->velocity = -p->velocity * COLLISION_DAMPING;
		}

		////////////////////////////
		// position change 
		// Need to update neighbor
		/////////////////////////////
		ivec3 oldCell = p->cellPosition;
		if (oldCell != p->reCalculateGridCell()) { //cell change, update grid
			grid[oldCell.x][oldCell.y][oldCell.z].erase(p);
			grid[p->cellPosition.x][p->cellPosition.y][p->cellPosition.z][p] = 1;
		}
	}
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
	//including itself 
	res.emplace_back(p->cellPosition);
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

void ParticleSystem::calcDensityPressure() {
	for (auto p : particles) { //for each particle
		p->density = 0.0;
		//find neighbors cell
		vector<ivec3> nCells = getNeighborCells(p->id);
		//for each possible neighbor in cells
		for (auto cellPos : nCells) {
			for (auto np : grid[cellPos.x][cellPos.y][cellPos.z]) {
				float sqrDist = 0.0; //itself
				if (p->id != np.first->id) {
					sqrDist = glm::distance2(p->position, np.first->position);
				}
				if (sqrDist > SQUARED_KERNEL_RADIUS) continue; //not neighbor
				float temp = SQUARED_KERNEL_RADIUS - sqrDist;
				p->density += temp * temp * temp; //optimization : not using pow
			}
		}
		p->density *= PARTICLE_MASS * POLY6; //optimization
		p->pressure = GAS_CONSTANT * (p->density - REST_DENSITY);
	}
}

void ParticleSystem::calcForces() {
	for (auto p : particles) { //for each particle
		//find neighbors cell
		vector<ivec3> nCells = getNeighborCells(p->id);
		//for each possible neighbor in cells
		vec3 totalPressureForce = vec3(0.0);
		for (auto cellPos : nCells) {
			for (auto np : grid[cellPos.x][cellPos.y][cellPos.z]) {
				if (p->id == np.first->id) { //itself
					continue;
				}
				float dist = glm::distance(p->position, np.first->position);
				if (dist > KERNEL_RADIUS) continue; //not neighbor
				float temp = KERNEL_RADIUS - dist;
				float forcePressure = -(p->pressure + np.first->pressure) / np.first->density * temp * temp * SPIKY_GRAD;
				//pressure move particle i further from particle j
				vec3 accDir = vec3(p->position - np.first->position) / dist;//normalized direction
				totalPressureForce += forcePressure * accDir;
			}
		}
		//a = f / density
		p->acceleration += totalPressureForce / p->density;
	}
}
