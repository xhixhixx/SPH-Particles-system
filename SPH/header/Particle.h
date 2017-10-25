#ifndef _H_PARTICLE_
#define _H_PARTICLE_
#include "Vec3.h"

class Particle
{
public:
	Particle() {
		position = Vec3(0.0, 0.0, 0.0);
		//default color blue
		color = Vec3(0.0, 0.0, 1.0);
	};
	Particle(Vec3& pos) {
		position = Vec3(pos.x, pos.y, pos.z);
	};
	Particle(Vec3& pos, Vec3& col) {
		position = Vec3(pos.x, pos.y, pos.z);
		color = Vec3(col.x, col.y, col.z);
	} ;
	~Particle();

public:
	Vec3 position;
	Vec3 velocity;
	Vec3 color;
};

#endif
