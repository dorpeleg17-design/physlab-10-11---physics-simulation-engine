#pragma once
#include "Object3D.h"
#include "pch.h"

// enum class Face 
// {
//     None,
//     Front, Back,
//     Left, Right,
//     Top, Bottom
// };
class Box : public Object3D
{
private:
    // This is the internal setup for the cube's geometry
    void initVO(const std::vector<GLfloat>& vertices) override;

public:
    // Constructor takes color and initial 3D position
    Box(const Color& color, const float x, const float y, const float z, const float volume);
    ~Box() override;

    // The draw call specifically for 3D objects
    void draw(const glm::mat4& pvMatrix) override;
    virtual Face getHitFace(const Object &other) const override;
};