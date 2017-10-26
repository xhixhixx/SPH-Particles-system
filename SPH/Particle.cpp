#include "header\Particle.h"
#include <iostream>

using namespace std;

Particle::~Particle()
{
}

vec3 Particle::reCalculateGridCell() {
	cellPosition = ivec3((position.x + BOX_SIZE_X / 2) / KERNEL_RADIUS, (position.y + BOX_SIZE_Y / 2) / KERNEL_RADIUS, (position.z + BOX_SIZE_Z / 2) / KERNEL_RADIUS);
	return cellPosition;
}

void Particle::printDebug() {
	std::cout << "particle: [" << position.x << "," << position.y << "," << position.z << "]" << "--- cell: [" << cellPosition.x << "," << cellPosition.y << "," << cellPosition.z << "]"<< endl;
}
