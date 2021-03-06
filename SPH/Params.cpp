#include "header\Params.h"
#include "header/Constant.h"
#include <cmath>


Params::Params()
{
	restDensity = 1000.0;
	collisionDamping = 0.3;
	gasConstant = 0.80;
	//viscos
	setViscosVarSet(1.0);
	//
	tensionCoef = 1.0;
	tensionThresh = 40.0;

	gravity = 2.8;
	threadingMode = MULTITHREADING;
}


Params::~Params()
{
}

void Params::setViscosVarSet(double vis) {
	viscosity = vis;
	viscoLapl = viscosity * PARTICLE_MASS * 45.0 / (PI * pow(KERNEL_RADIUS, 6));
}
