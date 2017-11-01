#include "header\Particle.h"
#include <iostream>

using namespace std;

Particle::~Particle()
{
}

ivec3 Particle::reCalculateGridCell() {
	cellPosition = ivec3((position.x + OFFSET_X) / CELL_SIZE, (position.y + OFFSET_Y) / CELL_SIZE, (position.z + OFFSET_Z) / CELL_SIZE);
	return cellPosition;
}

void Particle::printDebug() {
	std::cout << "particle: [" << position.x << "," << position.y << "," << position.z << "]" << "--- cell: [" << cellPosition.x << "," << cellPosition.y << "," << cellPosition.z << "]"<< endl;
}
