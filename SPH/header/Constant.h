#ifndef _H_CONSTANT_
#define _H_CONSTANT_

#define WINDOW_WIDTH  960
#define WINDOW_HEIGHT 720

//container size
#define BOX_SIZE_X 20.0f
#define BOX_SIZE_Y 10.0f
#define BOX_SIZE_Z 10.0f
#define OFFSET_X (BOX_SIZE_X / 2)
#define OFFSET_Y (BOX_SIZE_Y / 2)
#define OFFSET_Z (BOX_SIZE_Z / 2)

//num particle
#define NUM_PARTICLE_X 60
#define NUM_PARTICLE_Y 30
#define NUM_PARTICLE_Z 1
#define INIT_PARTICLE_DISTANCE 0.3f
#define PARTICLE_SIZE 3.5f

//world
#define TIMESTEP 0.001f //second
#define GRAVITY vec3(0.0f, -.98f, 0.0f)
#define PARTICLE_MASS 0.01f //kg
#define COLLISION_DAMPING 0.7f;
#define GAS_CONSTANT 1.0f
#define REST_DENSITY 1000 //kg/m^3

//kernel
#define PI 3.14159265359f
#define POLY6 (315 / (64 * PI * pow(KERNEL_RADIUS, 9)))
#define SPIKY_GRAD (PARTICLE_MASS / 2 * -45 / (PI * pow(KERNEL_RADIUS, 6))) //spiky gradient = -45 / (pi.h^6) * (h-r)^2 -----PARTICLE_MASS / 2 is pre-calc
#define VISCO_LAPL (45 / (PI * pow(KERNEL_RADIUS, 6))) //viso laplacian = 45 / (pi.h^6) * (h-r)

//kernel radius
#define KERNEL_RADIUS 1.0f
#define SQUARED_KERNEL_RADIUS (KERNEL_RADIUS * KERNEL_RADIUS)

//DEBUG
#define PARTICLE_IN_FOCUS 900


#endif

