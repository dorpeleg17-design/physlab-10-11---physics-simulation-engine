#pragma once
#include "pch.h"
#include "Object.h"
#include "Box.h"
class ObjectsManager
{
private:
    std::mutex _managerMtx;
    std::vector<Object*> objectVec;
    std::vector <Wire> wireVec;
public:
    ObjectsManager();
    ~ObjectsManager();
    void addObject(Object* obj);
    void addWire(const Wire& wire);
    void updateObjects(glm::mat4& pvMatrix, const float dt);
    static int getNewVelDirection(const Face& face);
    static std::map<char, std::pair<float, float>> calcNewSpeeds(const Object& obj1, const Object& obj2, const Face& face);
    void resolveCollision(Object &obj1, Object &obj2, Face face);
    void positionCorrection(Object &obj1, Object &obj2, Face face, const glm::vec3 &normal);
    glm::vec3 getNormalFromFace(Face face);
};
