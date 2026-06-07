#include "box.h"

Box::Box(const Color& color, const float x, const float y, const float z, const float mass) 
    : Object3D({}, color) 
{
    setXpos(x);
    setYpos(y);
    setZpos(z);
    _mass = mass;
    _volume = _mass / MATTER_DENSITY;
    
    float dimensions = std::cbrt(_volume);
    _halfHeight = dimensions * 0.5f;
    
    _vertices = {
        -(dimensions * HALF), -(dimensions * HALF),  (dimensions * HALF), 
         (dimensions * HALF), -(dimensions * HALF),  (dimensions * HALF), 
         (dimensions * HALF),  (dimensions * HALF),  (dimensions * HALF), 
        -(dimensions * HALF),  (dimensions * HALF),  (dimensions * HALF), 
        -(dimensions * HALF), -(dimensions * HALF), -(dimensions * HALF), 
         (dimensions * HALF), -(dimensions * HALF), -(dimensions * HALF), 
         (dimensions * HALF),  (dimensions * HALF), -(dimensions * HALF), 
        -(dimensions * HALF),  (dimensions * HALF), -(dimensions * HALF)  
    };
    
    _I = (1.0f / 6.0f) * _mass * ((2.0f * _halfHeight) * (2.0f * _halfHeight));

    setVelocity(glm::vec3(0.0f));
    
    _forceAccumulator = glm::vec3(0.0f);
    _checkCollision = true;
    initVO(_vertices);
}

void Box::initVO(const std::vector<GLfloat>& vertices)
{
    unsigned int indices[] = {
        0, 1, 2,  2, 3, 0, // Front
        1, 5, 6,  6, 2, 1, // Right
        7, 6, 5,  5, 4, 7, // Back
        4, 0, 3,  3, 7, 4, // Left
        4, 5, 1,  1, 0, 4, // Bottom
        3, 2, 6,  6, 7, 3  // Top
    };

    glGenVertexArrays(1, &_VAO);
    glGenBuffers(1, &_VBO);
    glGenBuffers(1, &_EBO);

    glBindVertexArray(_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void Box::draw(const glm::mat4& pvMatrix)
{
    glUseProgram(_shaderProgram);

    glm::mat4 model = glm::mat4(1.0f);
    
    // Translation matrix handles positioning
    model = glm::translate(model, glm::vec3(getXpos(), getYpos(), getZpos()));

    // MVP calculation combined with custom local vertex configurations
    glm::mat4 mvp = pvMatrix * model;

    glUniformMatrix4fv(glGetUniformLocation(_shaderProgram, "uMVP"), 1, GL_FALSE, glm::value_ptr(mvp));
    glUniform4f(glGetUniformLocation(_shaderProgram, "uColor"), _color.r, _color.g, _color.b, _color.a);

    glBindVertexArray(_VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

Face Box::getHitFace(const Object& other) const
{
    const Box* otherBox = dynamic_cast<const Box*>(&other);
    if (!otherBox) 
    {
        return Face::None;
    }

    float aHalfX = _halfHeight;
    float aHalfY = _halfHeight;
    float aHalfZ = _halfHeight;

    // Utilize optimized spatial boundaries rather than reprocessing std::cbrt every collision tick
    float bHalfX = std::cbrt(otherBox->getVolume()) * 0.5f;
    float bHalfY = bHalfX;
    float bHalfZ = bHalfX;

    float deltaX = getXpos() - otherBox->getXpos();
    float deltaY = getYpos() - otherBox->getYpos();
    float deltaZ = getZpos() - otherBox->getZpos();

    float overlapX = (aHalfX + bHalfX) - std::abs(deltaX);
    float overlapY = (aHalfY + bHalfY) - std::abs(deltaY);
    float overlapZ = (aHalfZ + bHalfZ) - std::abs(deltaZ);

    if (overlapX <= 0.0f || overlapY <= 0.0f || overlapZ <= 0.0f) 
    {
        return Face::None;
    }

    if (overlapX < overlapY && overlapX < overlapZ) 
    {
        return (deltaX > 0.0f) ? Face::Right : Face::Left;
    }
    else if (overlapY < overlapX && overlapY < overlapZ) 
    {
        return (deltaY > 0.0f) ? Face::Top : Face::Bottom;
    }
    else 
    {
        return (deltaZ > 0.0f) ? Face::Front : Face::Back;
    }
}

Box::~Box() 
{
    glDeleteVertexArrays(1, &_VAO);
    glDeleteBuffers(1, &_VBO);
    glDeleteBuffers(1, &_EBO);
}