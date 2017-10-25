#ifndef _H_PARTICLE_
#define _H_PARTICLE_
#include <glm\vec3.hpp>

using glm::vec3;

class Particle
{
public:
	Particle() {
		position = vec3(0.0, 0.0, 0.0);
		//default color blue
		color = vec3(0.0, 0.0, 1.0);
	};
	Particle(vec3& pos) {
		position = vec3(pos.x, pos.y, pos.z);
	};
	Particle(vec3& pos, vec3& col) {
		position = vec3(pos.x, pos.y, pos.z);
		color = vec3(col.x, col.y, col.z);
	} ;
	~Particle();

public:
	vec3 position;
	vec3 velocity;
	vec3 color;
};

#endif
