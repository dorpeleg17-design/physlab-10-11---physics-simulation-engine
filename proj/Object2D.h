#pragma once
#include "Object.h"
#include "pch.h"
class Object2D : public Object 
{
public:
    using Object::Object;
    void draw(const glm::mat4& pvMatrix) override;

};