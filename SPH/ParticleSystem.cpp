#include "header/ParticleSystem.h"
#include "header/Constant.h"
#include <iostream>
#include <glm/gtx/norm.hpp>
#include <fstream>
#include <thread>
#include <mutex>
#include <string>

mutex mtx;

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
	updateCounter = 0;
	//Breaking Dam set
	double widthX = xCnt * INIT_PARTICLE_DISTANCE + 0.6 * BOX_SIZE_X;
	double widthY = yCnt * INIT_PARTICLE_DISTANCE + 0.2 * BOX_SIZE_Y;
	double widthZ = zCnt * INIT_PARTICLE_DISTANCE + 0.6 * BOX_SIZE_Z;
	
	int id = 1;
	for (int i = 0; i < xCnt; ++i) {
		for (int j = 0; j < yCnt; ++j) {
			for (int k = 0; k < zCnt; ++k) {
				dvec3 pos(- widthX / 2 + i * INIT_PARTICLE_DISTANCE, - widthY / 2 + j * INIT_PARTICLE_DISTANCE, - widthZ / 2 + k * INIT_PARTICLE_DISTANCE);
				particles.emplace_back(make_shared<Particle>(pos, id++, params.restDensity));
			}
		}
	}

	//initialize neighbor grid
	int xCell = int(ceil(BOX_SIZE_X / CELL_SIZE));
	int yCell = int(ceil(BOX_SIZE_Y / CELL_SIZE));
	int zCell = int(ceil(BOX_SIZE_Z / CELL_SIZE));
	//
	cellCount = ivec3(xCell, yCell, zCell);
	int temp = xCell * yCell * zCell;
	grid.resize(temp, vector<shared_ptr<Particle>>());
	populateNeighborGrid();
}

void ParticleSystem::update() {
	if (!running) return;
	
	//exportFrames();

	++updateCounter;
	//update system
	calcDensityPressureByThead(USING_MULTITHREADING);
	calcForcesByThread(USING_MULTITHREADING);
	//update acceleration and position
	//for all particle
	updatePositionByThread(USING_MULTITHREADING);
	////////////////////////////
	// position change 
	// Need to update neighbor
	/////////////////////////////
	populateNeighborGrid();
}

void ParticleSystem::updatePositionByThread(bool useThread) {
	if (useThread) {
		thread t[NUM_THREAD];
		int gap = particles.size() / (NUM_THREAD - 1);
		for (int i = 0; i < NUM_THREAD; ++i) { //for each thread
			t[i] = thread(&ParticleSystem::updatePositionByIndex, this, i * gap, (i + 1) * gap - 1);
		}
		for (int i = 0; i < NUM_THREAD; ++i) { //for each thread
			t[i].join();
		}
	}
	else {
		updatePositionByIndex(0, grid.size() - 1);
	}
}

void ParticleSystem::updatePositionByIndex(int start, int end) {
	for (int i = start; i <= end && i < particles.size(); ++i) {
		shared_ptr<Particle> p = particles[i];
		//p->acceleration += GRAVITY;
		//Leap frog iteration
		p->position += p->velocity * TIMESTEP + (p->prevAcceleration + dvec3(0.0, -params.gravity, 0.0)) * (0.5 * TIMESTEP * TIMESTEP);
		p->velocity += (p->acceleration + dvec3(0.0, -params.gravity, 0.0) + p->prevAcceleration + dvec3(0.0, -params.gravity, 0.0)) * (0.5 * TIMESTEP);

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
			p->velocity.x = -p->velocity.x * params.collisionDamping;
		}
		if (negateY) {
			p->velocity.y = -p->velocity.y * params.collisionDamping;
		}
		if (negateZ) {
			p->velocity.z = -p->velocity.z * params.collisionDamping;
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

vector<ivec3> ParticleSystem::getNeighborCells(dvec3 pos) {
	vector<ivec3> res;
	ivec3 centerCell = ivec3((pos.x + OFFSET_X) / CELL_SIZE, (pos.y + OFFSET_Y) / CELL_SIZE, (pos.z + OFFSET_Z) / CELL_SIZE);
	for (auto d : DIRECTION3D) {
		int newX = centerCell.x + d[0];
		int newY = centerCell.y + d[1];
		int newZ = centerCell.z + d[2];
		if (newX >= 0 && newY >= 0 && newZ >= 0 && newX < cellCount.x && newY < cellCount.y && newZ < cellCount.z) {
			res.emplace_back(ivec3(newX, newY, newZ));
		}
	}
	if (centerCell.x >= 0 && centerCell.x < cellCount.x 
		&& centerCell.y >= 0 && centerCell.y < cellCount.y
		&& centerCell.z >= 0 && centerCell.z < cellCount.z) {
		res.emplace_back(centerCell);
	}
	return res;
}

double ParticleSystem::estimateColorFieldAtLocation(dvec3 pos) {
	double density = 0.0;
	//find neighbors cell
	vector<ivec3> nCells = getNeighborCells(pos);
	//for each possible neighbor in cells
	for (auto cellPos : nCells) {
		for (auto np : grid[cellPos.x + cellCount.x * (cellPos.y + cellCount.y * cellPos.z)]) {
			double sqrDist = sqrDist = glm::distance2(pos, np->position);

			if (sqrDist >= SQUARED_KERNEL_RADIUS) continue; //not neighbor
			double temp = SQUARED_KERNEL_RADIUS - sqrDist;
			density += temp * temp * temp; //optimization : not using pow
		}
	}
	density *= PARTICLE_MASS * POLY6; //optimization
	return density;
	/*
	 dvec3 surfaceNormal = dvec3(0.0);
	//find neighbors cell
	vector<ivec3> nCells = getNeighborCells(pos);
	//for each possible neighbor in cells
	for (auto cellPos : nCells) {
		for (auto np : grid[cellPos.x + cellCount.x * (cellPos.y + cellCount.y * cellPos.z)]) {
			double sqrDist = sqrDist = glm::distance2(pos, np->position);

			if (sqrDist >= SQUARED_KERNEL_RADIUS) continue; //not neighbor
			surfaceNormal = -POLY6_GRAD * (pos - np->position) / np->density * pow(SQUARED_KERNEL_RADIUS - sqrDist, 2);
		}
	}
	return glm::length2(surfaceNormal);
	 */
}

void ParticleSystem::exportFrames() {
	string path = "OBJ\\";
	
	ofstream myfile(path + "example" + to_string(updateCounter) + ".obj");
	if (myfile.is_open())
	{
		for (auto p : particles) {
			myfile << "v ";
			myfile << to_string(p->position.x) + " " + to_string(p->position.y) + " " + to_string(p->position.z);
			myfile << "\n";
		}
		myfile.close();
	}
	myfile.close();
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

void ParticleSystem::resetSystem() {
	running = false;
	particles.clear();
	grid.clear();
	createParticleSystem();
}

void ParticleSystem::populateNeighborGrid() {
	//clear previous
	fill(grid.begin(), grid.end(), vector<shared_ptr<Particle>>());
	
	//repopulate
	calcCellPosByThread(USING_MULTITHREADING);
}

void ParticleSystem::clearGridByThread(bool useThread) {
	if (useThread) {
		thread t[NUM_THREAD];
		int gap = grid.size() / (NUM_THREAD - 1);
		for (int i = 0; i < NUM_THREAD; ++i) { //for each thread
			t[i] = thread(&ParticleSystem::clearGridByIndex, this, i * gap, (i + 1) * gap - 1);
		}
		for (int i = 0; i < NUM_THREAD; ++i) { //for each thread
			t[i].join();
		}
	}
	else {
		clearGridByIndex(0, grid.size() - 1);
	}
}

void ParticleSystem::clearGridByIndex(int start, int end) {
	for (unsigned int i = start; i <= end && i < grid.size(); ++i) {
		grid[i].clear();
	}
}

void ParticleSystem::calcCellPosByThread(bool useThread) {
	if (useThread) {
		thread t[NUM_THREAD];
		int gap = particles.size() / (NUM_THREAD - 1);
		for (int i = 0; i < NUM_THREAD; ++i) { //for each thread
			t[i] = thread(&ParticleSystem::calcCellPosByIndex, this, i * gap, (i + 1) * gap - 1);
		}
		for (int i = 0; i < NUM_THREAD; ++i) { //for each thread
			t[i].join();
		}
	}
	else {
		calcCellPosByIndex(0, particles.size() - 1);
	}
}

void ParticleSystem::calcCellPosByIndex(int start, int end) {
	for (int i = start; i <= end && i < particles.size(); ++i) { //for each particle
		ivec3 cell = particles[i]->reCalculateGridCell();
		//lock
		//vector is not thread safe
		mtx.lock();
		grid[cell.x + cellCount.x * (cell.y + cellCount.y * cell.z)].push_back(particles[i]);
		mtx.unlock();
	}
}

void ParticleSystem::calcDensityPressureByThead(bool useThread) {
	if (useThread) {
		thread t[NUM_THREAD];
		int gap = particles.size() / (NUM_THREAD - 1);
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
			for (auto np : grid[cellPos.x + cellCount.x * (cellPos.y + cellCount.y * cellPos.z)]) {
				double sqrDist = 0.0; //itself
				if (p->id != np->id) {
					sqrDist = glm::distance2(p->position, np->position);
				}
				if (sqrDist >= SQUARED_KERNEL_RADIUS) continue; //not neighbor
				double temp = SQUARED_KERNEL_RADIUS - sqrDist;
				p->density += temp * temp * temp; //optimization : not using pow
			}
		}
		p->density *= PARTICLE_MASS * POLY6; //optimization
		p->pressure = params.gasConstant * (p->density - params.restDensity);
	}
}

void ParticleSystem::calcForcesByThread(bool useThread) {
	if (useThread) {
		thread t[NUM_THREAD];
		int gap = particles.size() / (NUM_THREAD - 1);
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
			for (auto np : grid[cellPos.x + cellCount.x * (cellPos.y + cellCount.y * cellPos.z)]) {
				if (p->id == np->id) { //itself
					continue;
				}
				double sqrDist = glm::distance2(p->position, np->position);
				if (sqrDist >= SQUARED_KERNEL_RADIUS) continue; //not neighbor
				double dist = sqrt(sqrDist);
				double temp = KERNEL_RADIUS - dist;
				//////////////////////////
				//pressure forces
				//////////////////////////
				double forcePressure = -(p->pressure + np->pressure) / np->density * temp * temp * SPIKY_GRAD;
				//pressure move particle i further from particle j
				dvec3 accDir = (p->position - np->position) / dist;//normalized direction
				totalPressureForce += forcePressure * accDir;

				//////////////////////////
				//viscosity forces
				//////////////////////////
				totalViscoForce += (np->velocity - p->velocity) / np->density * params.viscoLapl * temp;

				//////////////////////////
				//tension forces
				//////////////////////////
				surfaceNormal = -POLY6_GRAD * (p->position - np->position) / np->density * pow(SQUARED_KERNEL_RADIUS - sqrDist, 2);
				totalTensionForce += POLY6_LAPL / np->density * (sqrDist - SQUARED_KERNEL_RADIUS) * (sqrDist - 3 / 4 * (SQUARED_KERNEL_RADIUS - sqrDist));
			}
		}
		double sqrNormalLength = glm::length2(surfaceNormal);
		if (sqrNormalLength > params.tensionThresh) {
			totalTensionForce = params.tensionCoef * (totalTensionForce + POLY6_LAPL / p->density * SQUARED_KERNEL_RADIUS * 3 / 4 * SQUARED_KERNEL_RADIUS) * surfaceNormal / sqrt(sqrNormalLength);
			p->isSurface = true;
		}
		else {
			totalTensionForce = dvec3(0.0);
			p->isSurface = false;
		}
		//totalTensionForce = dvec3(0.0);
		//a = f / density
		dvec3 temp2 = (totalPressureForce + totalViscoForce + totalTensionForce) / p->density;
		if (glm::any(glm::isnan(temp2))) {//numerical error
			continue;
		}
		p->acceleration = temp2;
	}
}
