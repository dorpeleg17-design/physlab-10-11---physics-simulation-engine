#include "pch.h"
#include "Object2D.h"
#include "Object3D.h"
#include "Box.h"
#include "ObjectsManager.h"
#include "TextBox.h"
#include "Button.h"
#include "InputBox.h"
#include <sstream>
#include <iomanip>

struct kinematicsInitializationData 
{
    glm::vec3 initialVelocity;
    glm::vec3 initialAcceleration;
    glm::vec3 startingPosition;
    bool isValid = false;
};

class physicsHUD
{
public:
    static int setUpWindow(GLFWwindow*& window);
    static mode getModeUserOpt(GLFWwindow* window);
    static void momentumLoop(const std::vector<Object*>& objVec, const std::vector<Wire*>& wireVec, ObjectsManager& objManager, GLFWwindow* window);
    static void getCollisionModeUserOpt(GLFWwindow* window);
    static bool getDynamicsModeUserOpt(GLFWwindow* window);
    static float getWireMaxLength(GLFWwindow* window);
    static std::pair<Object*, glm::vec3> getForces(GLFWwindow* window);
    static std::pair<Object*, Object*> getObjectsStats(GLFWwindow* window);
    static void dynamicsLoop(const std::vector<Object *> &objVec, const std::vector<Wire *> &wireVec, ObjectsManager &objManager, const glm::vec3& appliedForce, GLFWwindow *window);
    static kinematicsInitializationData getInitialKinematicsStats(GLFWwindow* window);
    static void kinematicsLoop(const std::vector<Object *> &objVec, const std::vector<Wire *> &wireVec, ObjectsManager &objManager, const kinematicsInitializationData &initData, GLFWwindow *window);
    static bool getTrackingPreference(GLFWwindow *window);
    static float getSpecificTimeValue(GLFWwindow *window);
    static glm::vec3 getSpecificPositionValue(GLFWwindow *window);
};
