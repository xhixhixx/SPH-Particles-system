#ifndef _H_CONSTANT_
#define _H_CONSTANT_

#define WINDOW_WIDTH  960
#define WINDOW_HEIGHT 720

//container size
#define BOX_SIZE_X 1.0
#define BOX_SIZE_Y 1.0
#define BOX_SIZE_Z 1.0
#define OFFSET_X (BOX_SIZE_X / 2.0)
#define OFFSET_Y (BOX_SIZE_Y / 2.0)
#define OFFSET_Z (BOX_SIZE_Z / 2.0)

//num particle
#define NUM_PARTICLE_X 10
#define NUM_PARTICLE_Y 10
#define NUM_PARTICLE_Z 10
#define INIT_PARTICLE_DISTANCE 0.02
#define PARTICLE_SIZE 6.0f

//world
#define TIMESTEP 0.003 //second
#define GRAVITY dvec3(0.0, -9.8, 0.0)
#define PARTICLE_MASS 0.01 //kg
#define COLLISION_DAMPING 0.6;
#define GAS_CONSTANT 0.50
#define REST_DENSITY 500.0 //kg/m^3
#define VISCOSITY 1.0 //kg/m/s

//kernel radius
#define KERNEL_RADIUS 0.05
#define SQUARED_KERNEL_RADIUS (KERNEL_RADIUS * KERNEL_RADIUS)
#define CELL_SIZE (KERNEL_RADIUS + 0.1)

#define MAX_VELOCITY (KERNEL_RADIUS / TIMESTEP) //we do not want the particle to move further than 1 radius per timestep

//kernel
#define PI 3.14159265359
#define POLY6 (315.0 / (64.0 * PI * pow(KERNEL_RADIUS, 9)))
#define SPIKY_GRAD (PARTICLE_MASS / 2.0 * -45.0 / (PI * pow(KERNEL_RADIUS, 6))) //spiky gradient = -45 / (pi.h^6) * (h-r)^2   ----PARTICLE_MASS / 2 is pre-calc
#define VISCO_LAPL (VISCOSITY * PARTICLE_MASS * 45.0 / (PI * pow(KERNEL_RADIUS, 6))) //viso laplacian = 45 / (pi.h^6) * (h-r) -- VISCO * MASS is pre-calc

//DEBUG
#define PARTICLE_IN_FOCUS 100


#endif

