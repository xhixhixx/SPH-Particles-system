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

	double gravity;

	int threadingMode;

public:
	void setViscosVarSet(double vis);

	void setThreadingMode(int mode) { threadingMode = mode; }
	int getThreadingMode() const { return threadingMode; }
};

