#include "header\Params.h"
#include "header/Constant.h"
#include <cmath>


Params::Params()
{
	restDensity = 500.0;
	collisionDamping = 0.3;
	gasConstant = 1.0;
	//viscos
	setViscosVarSet(1.0);
	//
	tensionCoef = 1.0;
	tensionThresh = 0.4;
}


Params::~Params()
{
}

void Params::setViscosVarSet(double vis) {
	viscosity = vis;
	viscoLapl = viscosity * PARTICLE_MASS * 45.0 / (PI * pow(KERNEL_RADIUS, 6));
}
