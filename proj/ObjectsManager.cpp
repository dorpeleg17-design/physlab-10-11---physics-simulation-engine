#include "ObjectsManager.h"

extern bool stopSimulation;
extern float collisionConstant;

ObjectsManager::ObjectsManager()
{
}

ObjectsManager::~ObjectsManager()
{
    objectVec.clear();
}

void ObjectsManager::addObject(Object *obj)
{
    std::lock_guard<std::mutex> lock(_managerMtx);
    objectVec.push_back(obj);
}

void ObjectsManager::addWire(const Wire &wire)
{
    std::lock_guard<std::mutex> lock(_managerMtx);
    wireVec.push_back(wire);
}

void ObjectsManager::updateObjects(glm::mat4 &pvMatrix, const float dt)
{
    std::lock_guard<std::mutex> lock(_managerMtx);
    for (size_t i = 0; i < wireVec.size(); i++)
    {
        if (!wireVec[i]._connectedObj) continue;

        wireVec[i]._objectPoint = wireVec[i]._connectedObj->getPosVector();
        glm::vec3 wireDir = wireVec[i]._anchorPoint - wireVec[i]._objectPoint;
        float currentLength = glm::length(wireDir);

        if (currentLength > 0.0001f)
        {
            wireDir = wireDir / currentLength; 

            if (currentLength > wireVec[i]._maxLength)
            {
                float overshoot = currentLength - wireVec[i]._maxLength;
                glm::vec3 correctionVector = wireDir * overshoot; 
                
                wireVec[i]._connectedObj->movePosition(correctionVector);
                
                wireVec[i]._objectPoint = wireVec[i]._connectedObj->getPosVector();
                currentLength = wireVec[i]._maxLength;
            }

            if (currentLength >= wireVec[i]._maxLength)
            {
                glm::vec3 velocity = wireVec[i]._connectedObj->getVelocity();
                
                float outwardVelocity = glm::dot(velocity, -wireDir);

                if (outwardVelocity > 0.0f)
                {
                    glm::vec3 separationVelocity = (-wireDir) * outwardVelocity;
                    glm::vec3 constrainedVelocity = velocity - separationVelocity;

                    wireVec[i]._connectedObj->setVelocity(constrainedVelocity);
                }

                float outwardForceMagnitude = glm::dot(wireVec[i]._connectedObj->getForceAccumulator(), -wireDir);
                if (outwardForceMagnitude > 0.0f)
                {
                    wireVec[i]._tension = wireDir * outwardForceMagnitude;
                    wireVec[i]._connectedObj->applyForceAtPoint(wireVec[i]._tension, wireVec[i]._objectPoint);
                }
                else
                {
                    wireVec[i]._tension = glm::vec3(0.0f);
                }
            }
            else
            {
                wireVec[i]._tension = glm::vec3(0.0f);
            }
        }
        else
        {
            wireVec[i]._tension = glm::vec3(0.0f);
        }
    }

    for (size_t i = 0; i < objectVec.size(); ++i)
    {
        objectVec[i]->move(dt);
    }

    Face currFace;
    for (size_t i = 0; i < objectVec.size(); ++i)
    {
        for (size_t j = i + 1; j < objectVec.size(); ++j)
        {
            auto& obj = objectVec[i];
            auto& alsoObj = objectVec[j];

            currFace = obj->getHitFace(*alsoObj);      
            if (currFace != Face::None && (obj->getCollisionBool() && alsoObj->getCollisionBool()))
            {
                stopSimulation = true;
                resolveCollision(*obj, *alsoObj, currFace);

                glm::vec3 normal = getNormalFromFace(currFace);
                positionCorrection(*obj, *alsoObj, currFace, normal);
            }
        }
    }

    for (size_t i = 0; i < objectVec.size(); ++i)
    {
        objectVec[i]->draw(pvMatrix);
    }
    for (size_t i = 0; i < wireVec.size(); i++)
    {
        wireVec[i].draw(pvMatrix);
    }
}

int ObjectsManager::getNewVelDirection(const Face &face)
{
    if (face == Face::Left)
    {
        return 1;
    }
    else if (face == Face::Right)
    {
        return -1;
    }
    else
    {
        return 1;
    }
}

void ObjectsManager::resolveCollision(Object& obj1, Object& obj2, Face face)
{
    if (face == Face::None) return;

    glm::vec3 normal = getNormalFromFace(face);

    glm::vec3 pos1 = obj1.getPosVector();
    glm::vec3 pos2 = obj2.getPosVector();

    float halfSize1 = std::cbrt(obj1.getVolume()) * 0.5f;

    glm::vec3 contactPoint = pos1;
    if (face == Face::Front)  contactPoint.z += halfSize1;
    if (face == Face::Back)   contactPoint.z -= halfSize1;
    if (face == Face::Right)  contactPoint.x += halfSize1;
    if (face == Face::Left)   contactPoint.x -= halfSize1;
    if (face == Face::Top)    contactPoint.y += halfSize1;
    if (face == Face::Bottom) contactPoint.y -= halfSize1;

    if (face == Face::Front || face == Face::Back || face == Face::Top || face == Face::Bottom)
    {
        contactPoint.x = (pos1.x + pos2.x) * 0.5f;
    }
    if (face == Face::Left || face == Face::Right || face == Face::Top || face == Face::Bottom)
    {
        contactPoint.z = (pos1.z + pos2.z) * 0.5f;
    }
    if (face == Face::Left || face == Face::Right || face == Face::Front || face == Face::Back)
    {
        contactPoint.y = (pos1.y + pos2.y) * 0.5f;
    }

    glm::vec3 r1 = contactPoint - pos1;
    glm::vec3 r2 = contactPoint - pos2;

    glm::vec3 v1 = obj1.getVelocity();
    glm::vec3 v2 = obj2.getVelocity();
    
    glm::vec3 relativeVelocity = v2 - v1; 
    float velAlongNormal = glm::dot(relativeVelocity, normal);

    // if (velAlongNormal > 0.0f) return;

    float e = collisionConstant;
    
    float invMass1 = 1.0f / obj1.getMass();
    float invMass2 = 1.0f / obj2.getMass();

    glm::vec3 cross1 = glm::cross(r1, normal);
    glm::vec3 cross2 = glm::cross(r2, normal);
    float r1CrossN_Sq = glm::dot(cross1, cross1) / obj1.getInertia();
    float r2CrossN_Sq = glm::dot(cross2, cross2) / obj2.getInertia();

    float totalDistribution = invMass1 + invMass2 + r1CrossN_Sq + r2CrossN_Sq;
    if (totalDistribution == 0.0f) return;

    float impulseMagnitude = -(1.0f + e) * velAlongNormal;
    impulseMagnitude /= totalDistribution;

    glm::vec3 impulseVector = impulseMagnitude * normal;

    obj1.setVelocity(v1 - impulseVector * invMass1);
    obj2.setVelocity(v2 + impulseVector * invMass2);

    // glm::vec3 angularImpulse1 = glm::cross(r1, -impulseVector);
    // glm::vec3 angularImpulse2 = glm::cross(r2, impulseVector);
    // obj1.setAngularVelocity(obj1.getAngularVelocity() + angularImpulse1 / obj1.getInertia());
    // obj2.setAngularVelocity(obj2.getAngularVelocity() + angularImpulse2 / obj2.getInertia());
}

glm::vec3 ObjectsManager::getNormalFromFace(Face face)
{
    if (face == Face::Right)  return glm::vec3( 1.0f,  0.0f,  0.0f);
    if (face == Face::Left)   return glm::vec3(-1.0f,  0.0f,  0.0f);
    if (face == Face::Top)    return glm::vec3( 0.0f,  1.0f,  0.0f);
    if (face == Face::Bottom) return glm::vec3( 0.0f, -1.0f,  0.0f);
    if (face == Face::Front)  return glm::vec3( 0.0f,  0.0f,  1.0f);
    if (face == Face::Back)   return glm::vec3( 0.0f,  0.0f, -1.0f);
    
    return glm::vec3(0.0f);
}

void ObjectsManager::positionCorrection(Object& obj1, Object& obj2, Face face, const glm::vec3& normal)
{
    if (face == Face::None) return;

    float aHalfX = std::cbrt(obj1.getVolume()) * 0.5f;
    float aHalfY = std::cbrt(obj1.getVolume()) * 0.5f;
    float aHalfZ = std::cbrt(obj1.getVolume()) * 0.5f;

    float bHalfX = std::cbrt(obj2.getVolume()) * 0.5f;
    float bHalfY = std::cbrt(obj2.getVolume()) * 0.5f;
    float bHalfZ = std::cbrt(obj2.getVolume()) * 0.5f;

    float overlapX = (aHalfX + bHalfX) - std::abs(obj1.getXpos() - obj2.getXpos());
    float overlapY = (aHalfY + bHalfY) - std::abs(obj1.getYpos() - obj2.getYpos());
    float overlapZ = (aHalfZ + bHalfZ) - std::abs(obj1.getZpos() - obj2.getZpos());

    float penetration = 0.0f;
    if (face == Face::Left || face == Face::Right)   penetration = overlapX;
    if (face == Face::Top || face == Face::Bottom)    penetration = overlapY;
    if (face == Face::Front || face == Face::Back)   penetration = overlapZ;

    if (penetration > 0.01f) 
    {
        const float percent = 0.8f; 
        
        float invMass1 = 1.0f / obj1.getMass();
        float invMass2 = 1.0f / obj2.getMass();
        float totalInvMass = invMass1 + invMass2;

        if (totalInvMass == 0.0f) return;

        float correctionMagnitude = (penetration / totalInvMass) * percent;
        glm::vec3 correctionVector = correctionMagnitude * normal;

        obj1.movePosition(correctionVector * invMass1);
        obj2.movePosition(-correctionVector * invMass2);
    }
}