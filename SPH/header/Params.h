#pragma once
class Params
{
public:
	Params();
	~Params();

	double restDensity;//kg/m^3
	double collisionDamping;
	double gasConstant;
	//viscosity related
	double viscosity;//kg/m/s
	double viscoLapl;
	//
	double tensionCoef;
	double tensionThresh;

public:
	void setViscosVarSet(double vis);
};

