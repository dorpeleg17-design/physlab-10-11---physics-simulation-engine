#pragma once
#include "Object.h"
#include "pch.h"
class Object3D : public Object {
public:
    using Object::Object;

    void draw(const glm::mat4& pvMatrix) override;
};