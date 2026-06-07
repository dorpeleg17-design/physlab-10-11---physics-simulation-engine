#include "pch.h"
#include "physicsHUD.h"
#include "Object2D.h"
#include "Object3D.h"
#include "Box.h"
#include "ObjectsManager.h"
#include "TextBox.h"
#include "Button.h"
#include "InputBox.h"

Object* controlledObj = nullptr;
std::mutex _posMtx; 

int main() 
{
    GLFWwindow* window = nullptr;
    ObjectsManager manager;
    std::vector<Object *> objVec;
    std::vector<Wire *> wireVec;
    if(physicsHUD::setUpWindow(window) == -1)
    {
        return -1;
    }
    glEnable(GL_DEPTH_TEST);

    switch (physicsHUD::getModeUserOpt(window))
    {
    case mode::Momentum:
    {
        physicsHUD::getCollisionModeUserOpt(window);
        std::pair<Object*, Object*> objects = physicsHUD::getObjectsStats(window);
        objVec.push_back(objects.first);
        objVec.push_back(objects.second);        
        physicsHUD::momentumLoop(objVec, wireVec, manager, window);
        delete objects.first;
        delete objects.second;
    }
    break;
    case mode::Dynamics:
    {
        if (physicsHUD::getDynamicsModeUserOpt(window) == WIRE)
        {
            std::pair<Object*, glm::vec3> objAndForce = physicsHUD::getForces(window);
            Wire* wire = new Wire(objAndForce.first, glm::vec3(0.0f, 0.0f, 0.0f), physicsHUD::getWireMaxLength(window));
            wireVec.push_back(wire);
            objVec.push_back(objAndForce.first);
            physicsHUD::dynamicsLoop(objVec, wireVec, manager, objAndForce.second, window);
            delete wire;
            delete objAndForce.first;
        }
        else
        {
            std::pair<Object*, glm::vec3> objAndForce = physicsHUD::getForces(window);
            objVec.push_back(objAndForce.first);
            physicsHUD::dynamicsLoop(objVec, wireVec, manager, objAndForce.second, window);
            delete objAndForce.first;
        }
    }   
    break;
    case mode::Kinematics:
    {
        collisionConstant = PLASTIC;
        kinematicsInitializationData data = physicsHUD::getInitialKinematicsStats(window);
        if (data.isValid)
        {
            Color whiteColor = {1.0f, 1.0f, 1.0f, 1.0f};
            Object* kinematicObj = new Box(whiteColor, data.startingPosition.x, data.startingPosition.y, data.startingPosition.z, 1.0f);
            
            kinematicObj->setVelocity(data.initialVelocity);
            kinematicObj->setAcceleration(data.initialAcceleration);
            
            objVec.push_back(kinematicObj);
            
            physicsHUD::kinematicsLoop(objVec, wireVec, manager, data, window);
            delete kinematicObj;
        }
        
    }
    break;
    default:
        break;
    }
    
    glfwSetWindowShouldClose(window, true);
    return 0;
}