#include "header\ParticleSystem.h"
#include "header/Constant.h"
#include <iostream>
#include <glm\gtx\norm.hpp>
#include <algorithm>
#include <thread>

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
	double widthX = xCnt * INIT_PARTICLE_DISTANCE - 0.5 * BOX_SIZE_X;
	double widthY = yCnt * INIT_PARTICLE_DISTANCE - 0.2 * BOX_SIZE_Y;
	double widthZ = zCnt * INIT_PARTICLE_DISTANCE;
	
	int id = 1;
	for (int i = 0; i < xCnt; ++i) {
		for (int j = 0; j < yCnt; ++j) {
			for (int k = 0; k < zCnt; ++k) {
				dvec3 pos(- widthX / 2 + i * INIT_PARTICLE_DISTANCE, - widthY / 2 + j * INIT_PARTICLE_DISTANCE, - widthZ / 2 + k * INIT_PARTICLE_DISTANCE);
				particles.emplace_back(make_shared<Particle>(pos, id++));
			}
		}
	}

	//initialize neighbor grid
	int xCell = int(ceil(BOX_SIZE_X / CELL_SIZE));
	int yCell = int(ceil(BOX_SIZE_Y / CELL_SIZE));
	int zCell = int(ceil(BOX_SIZE_Z / CELL_SIZE));
	//
	cellCount = ivec3(xCell, yCell, zCell);
	grid.resize(xCell, vector<vector<unordered_map<shared_ptr<Particle>, int>>>(yCell, vector<unordered_map<shared_ptr<Particle>, int>>(zCell, unordered_map<shared_ptr<Particle>, int>())));
	populateNeighborGrid();
}

void ParticleSystem::update() {
	//update system
	calcDensityPressureByThead(true);
	calcForcesByThread(true);

	//update acceleration and position
	//for all particle
	for (auto p : particles) {
		//p->acceleration += GRAVITY;
		//Leap frog iteration
		p->position += p->velocity * TIMESTEP + (p->prevAcceleration + GRAVITY) * (0.5 * TIMESTEP * TIMESTEP);
		p->velocity += (p->acceleration + GRAVITY + p->prevAcceleration + GRAVITY) * (0.5 * TIMESTEP);

		double temp = glm::length2(p->velocity);
		p->prevAcceleration = p->acceleration;

		//colision with container
		bool negateX = false;
		bool negateY = false;
		bool negateZ = false;
		if (p->position.x < -OFFSET_X) {
			p->position.x = -OFFSET_X;
			negateX = true;
		}
		else if (p->position.x > OFFSET_X) {
			p->position.x = OFFSET_X;
			negateX = true;
		}
		if (p->position.y < -OFFSET_Y) {
			p->position.y = -OFFSET_Y;
			negateY = true;
		}
		else if (p->position.y > OFFSET_Y) {
			p->position.y = OFFSET_Y;
			negateY = true;
		}
		if (p->position.z < -OFFSET_Z) {
			p->position.z = -OFFSET_Z;
			negateZ = true;
		}
		else if (p->position.z > OFFSET_Z) {
			p->position.z = OFFSET_Z;
			negateZ = true;
		}
		if (negateX) {
			p->velocity.x = -p->velocity.x * COLLISION_DAMPING;
		}
		if (negateY) {
			p->velocity.y = -p->velocity.y * COLLISION_DAMPING;
		}
		if (negateZ) {
			p->velocity.z = -p->velocity.z * COLLISION_DAMPING;
		}
		////////////////////////////
		// position change 
		// Need to update neighbor
		/////////////////////////////
		ivec3 oldCell = p->cellPosition;
		if (oldCell != p->reCalculateGridCell()) { //cell change, update grid
			grid[oldCell.x][oldCell.y][oldCell.z].erase(p);
			//cap cell
			p->cellPosition.x = max(min(cellCount.x - 1, p->cellPosition.x), 0);
			p->cellPosition.y = max(min(cellCount.y - 1, p->cellPosition.y), 0);
			p->cellPosition.z = max(min(cellCount.z - 1, p->cellPosition.z), 0);
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

void ParticleSystem::calcDensityPressureByThead(bool useThread) {
	if (useThread) {
		thread t[NUM_THREAD];
		int gap = particles.size() / NUM_THREAD;
		for (int i = 0; i < NUM_THREAD; ++i) { //for each thread
			t[i] = thread(&ParticleSystem::calcDensityPressureByIndex, this, i * gap, (i + 1) * gap - 1);
		}
		for (int i = 0; i < NUM_THREAD; ++i) { //for each thread
			t[i].join();
		}
	}
	else {
		calcDensityPressureByIndex(0, particles.size() - 1);
	}
}

void ParticleSystem::calcDensityPressureByIndex(int start, int end) {
	for (int i = start; i <= end && i < particles.size(); ++i) {
		shared_ptr<Particle> p = particles[i];
		p->density = 0.0;
		//find neighbors cell
		vector<ivec3> nCells = getNeighborCells(p->id);
		//for each possible neighbor in cells
		for (auto cellPos : nCells) {
			for (auto np : grid[cellPos.x][cellPos.y][cellPos.z]) {
				double sqrDist = 0.0; //itself
				if (p->id != np.first->id) {
					sqrDist = glm::distance2(p->position, np.first->position);
				}
				if (sqrDist >= SQUARED_KERNEL_RADIUS) continue; //not neighbor
				double temp = SQUARED_KERNEL_RADIUS - sqrDist;
				p->density += temp * temp * temp; //optimization : not using pow
			}
		}
		p->density *= PARTICLE_MASS * POLY6; //optimization
		p->pressure = GAS_CONSTANT * (p->density - REST_DENSITY);
	}
}

void ParticleSystem::calcForcesByThread(bool useThread) {
	if (useThread) {
		thread t[NUM_THREAD];
		int gap = particles.size() / NUM_THREAD;
		for (int i = 0; i < NUM_THREAD; ++i) { //for each thread
			t[i] = thread(&ParticleSystem::calcForcesByIndex, this, i * gap, (i + 1) * gap - 1);
		}
		for (int i = 0; i < NUM_THREAD; ++i) { //for each thread
			t[i].join();
		}
	}
	else {
		calcForcesByIndex(0, particles.size() - 1);
	}
}

void ParticleSystem::calcForcesByIndex(int start, int end) {
	for (int i = start; i <= end && i < particles.size(); ++i) {
		shared_ptr<Particle> p = particles[i];
		//find neighbors cell
		vector<ivec3> nCells = getNeighborCells(p->id);
		//for each possible neighbor in cells
		dvec3 totalPressureForce = dvec3(0.0);
		dvec3 totalViscoForce = dvec3(0.0);
		dvec3 surfaceNormal = dvec3(0.0);
		dvec3 totalTensionForce = dvec3(0.0);
		for (auto cellPos : nCells) {
			for (auto np : grid[cellPos.x][cellPos.y][cellPos.z]) {
				if (p->id == np.first->id) { //itself
					continue;
				}
				double dist = glm::distance(p->position, np.first->position);
				if (dist >= KERNEL_RADIUS) continue; //not neighbor
				double temp = KERNEL_RADIUS - dist;
				//////////////////////////
				//pressure forces
				//////////////////////////
				double forcePressure = -(p->pressure + np.first->pressure) / np.first->density * temp * temp * SPIKY_GRAD;
				//pressure move particle i further from particle j
				dvec3 accDir = (p->position - np.first->position) / dist;//normalized direction
				totalPressureForce += forcePressure * accDir;

				//////////////////////////
				//viscosity forces
				//////////////////////////
				totalViscoForce += (np.first->velocity - p->velocity) / np.first->density * VISCO_LAPL * temp;

				//////////////////////////
				//tension forces
				//////////////////////////
				double squareDist = dist * dist;
				surfaceNormal = -POLY6_GRAD * (p->position - np.first->position) / np.first->density * pow(SQUARED_KERNEL_RADIUS - squareDist, 2);
				totalTensionForce += POLY6_LAPL / np.first->density * (squareDist - SQUARED_KERNEL_RADIUS) * (5 * squareDist - SQUARED_KERNEL_RADIUS);
			}
		}
		double sqrNormalLength = glm::length2(surfaceNormal);
		if (sqrNormalLength > TENSION_THRESHOLD) {
			totalTensionForce = TENSION_COEF * (totalTensionForce + POLY6_LAPL / p->density * SQUARED_KERNEL_RADIUS * SQUARED_KERNEL_RADIUS) * surfaceNormal / sqrt(sqrNormalLength);
		}
		else {
			totalTensionForce = dvec3(0.0);
		}
		//a = f / density
		dvec3 temp2 = (totalPressureForce + totalViscoForce + totalTensionForce) / p->density;
		if (glm::any(glm::isnan(temp2))) {//numerical error
			continue;
		}
		p->acceleration = temp2;
	}
}
