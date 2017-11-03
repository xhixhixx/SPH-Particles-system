#ifndef _H_CONSTANT_
#define _H_CONSTANT_

#define WINDOW_WIDTH  960
#define WINDOW_HEIGHT 720

//THREADING
#define NUM_THREAD 10

//container size
#define BOX_SIZE_X 1.0
#define BOX_SIZE_Y 1.2
#define BOX_SIZE_Z 1.0
#define OFFSET_X (BOX_SIZE_X / 2.0)
#define OFFSET_Y (BOX_SIZE_Y / 2.0)
#define OFFSET_Z (BOX_SIZE_Z / 2.0)

//num particle
#define NUM_PARTICLE_X 20
#define NUM_PARTICLE_Y 40
#define NUM_PARTICLE_Z 20
#define INIT_PARTICLE_DISTANCE 0.02
#define PARTICLE_SIZE 5.0f

//world
#define TIMESTEP 0.004 //second
#define GRAVITY dvec3(0.0, -9.8, 0.0)
#define PARTICLE_MASS 0.01 //kg
#define COLLISION_DAMPING 0.3;
#define GAS_CONSTANT 1.0
#define REST_DENSITY 1000.0 //kg/m^3
#define VISCOSITY 1.0 //kg/m/s
#define TENSION_COEF 0.2
#define TENSION_THRESHOLD 0.3

//kernel radius
#define KERNEL_RADIUS 0.04
#define SQUARED_KERNEL_RADIUS (KERNEL_RADIUS * KERNEL_RADIUS)
#define CELL_SIZE (KERNEL_RADIUS * 1.1)

//kernel
#define PI 3.14159265359
#define POLY6 (315.0 / (64.0 * PI * pow(KERNEL_RADIUS, 9)))
#define SPIKY_GRAD (PARTICLE_MASS / 2.0 * -45.0 / (PI * pow(KERNEL_RADIUS, 6))) //spiky gradient = -45 / (pi.h^6) * (h-r)^2   ----PARTICLE_MASS / 2 is pre-calc
#define VISCO_LAPL (VISCOSITY * PARTICLE_MASS * 45.0 / (PI * pow(KERNEL_RADIUS, 6))) //viso laplacian = 45 / (pi.h^6) * (h-r) -- VISCO * MASS is pre-calc
#define POLY6_GRAD (PARTICLE_MASS / 2.0 * -945 / (32 * PI * pow(KERNEL_RADIUS, 9)))// = -945/(32*pi*h^9) * r * (h^2 - r^2) ^ 2
#define POLY6_LAPL POLY6_GRAD//-945 / (32 * pi * h^9) * (r^2 - h^2) * (5 * r^2 - h^2)

//DEBUG
#define PARTICLE_IN_FOCUS 15500


#endif

