#ifndef _H_PARTICLE_
#define _H_PARTICLE_
#include <glm\vec3.hpp>
#include "Constant.h"

using glm::dvec3;
using glm::ivec3;

class Particle
{
public:
	Particle() {
		position = dvec3(0.0, 0.0, 0.0);
		//default color blue
		color = dvec3(0.0, 0.0, 1.0);
		isSurface = false;
		normal = dvec3(0.0, 0.0, 0.0);
		reCalculateGridCell();
	};
	Particle(dvec3& pos, int _id, double restDens) {
		position = dvec3(pos.x, pos.y, pos.z);
		id = _id;
		
		velocity = dvec3(0.0);
		acceleration = dvec3(0.0);
		prevAcceleration = dvec3(0.0);
		normal = dvec3(0.0, 0.0, 0.0);

		density = restDens;
		pressure = 0.0;

		isSurface = false;
		
		reCalculateGridCell();
	};

	~Particle();

	ivec3 reCalculateGridCell();//need to call on position change

	void printDebug();

public:
	int id;
	dvec3 position;
	ivec3 cellPosition;
	dvec3 color;
	//sph field
	dvec3 velocity;
	dvec3 acceleration;
	dvec3 prevAcceleration;//for leapfrog

	dvec3 normal;
	
	double density;
	double pressure;
	
	//surface tracking
	bool isSurface;
};

#endif
