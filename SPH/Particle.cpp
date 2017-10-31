#include "header\Particle.h"
#include <iostream>

using namespace std;

Particle::~Particle()
{
}

ivec3 Particle::reCalculateGridCell() {
	cellPosition = ivec3((position.x + OFFSET_X) / KERNEL_RADIUS, (position.y + OFFSET_Y) / KERNEL_RADIUS, (position.z + OFFSET_Z) / KERNEL_RADIUS);
	return cellPosition;
}

void Particle::printDebug() {
	std::cout << "particle: [" << position.x << "," << position.y << "," << position.z << "]" << "--- cell: [" << cellPosition.x << "," << cellPosition.y << "," << cellPosition.z << "]"<< endl;
}
