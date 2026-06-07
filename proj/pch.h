// pch.h
#ifndef PCH_H
#define PCH_H
inline float collisionConstant;
inline bool stopSimulation = false;
inline float lastFrame = 0.0f;
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <mutex>
#include <thread>
#include <map>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#define ACCELRATION_PER_FRAME 0.0000004f
#define GRAVITY 9.81f
#define APROX_ZERO 0.0000005f
#define HALF 0.5f
#define MATTER_DENSITY 1000.0f
#define PLASTIC 0.0f
#define ELASTIC 1.0f
#define WIRE 1
#define NO_WIRE 0
#define TIME_TRACKING 1
#define POS_TRACKING 0

struct Color 
{
    float r, g, b, a;
};


enum class mode
{
    Momentum, Dynamics, Kinematics
};

extern std::mutex _posMtx;
#endif