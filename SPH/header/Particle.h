#ifndef _H_PARTICLE_
#define _H_PARTICLE_
#include <glm\vec3.hpp>
#include "Constant.h"

using glm::vec3;
using glm::ivec3;

class Particle
{
public:
	Particle() {
		position = vec3(0.0, 0.0, 0.0);
		//default color blue
		color = vec3(0.0, 0.0, 1.0);
		reCalculateGridCell();
	};
	Particle(vec3& pos, int _id) {
		position = vec3(pos.x, pos.y, pos.z);
		id = _id;
		velocity = vec3(0.0f);
		acceleration = vec3(0.0f);
		prevAcceleration = vec3(0.0f);
		reCalculateGridCell();
	};

	~Particle();

	vec3 reCalculateGridCell();//need to call on position change

	void printDebug();

public:
	int id;
	vec3 position;
	ivec3 cellPosition;
	vec3 color;
	//sph field
	vec3 velocity;
	vec3 acceleration;
	vec3 prevAcceleration;//for leapfrog
};

#endif
