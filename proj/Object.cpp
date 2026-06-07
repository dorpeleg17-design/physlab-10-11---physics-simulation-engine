#include "object.h"

void Object::updateShaderProg(const char *vertexShaderSource, const char *fragmentShaderSource)
{
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    _shaderProgram = shaderProgram;
}

Object::Object(std::vector<GLfloat> vertices, const Color &color) 
    : _vertices(vertices), _VAO(0), _VBO(0), _EBO(0), _mass(1.0f), _volume(1.0f), _halfHeight(0.5f)
{
    const char* vs = "#version 330 core\n"
    "layout (location = 0) in vec3 pos;\n"
    "uniform mat4 uMVP;\n"
    "void main() {\n"
    "   gl_Position = uMVP * vec4(pos, 1.0);\n"
    "}\0";

    const char* fs = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "uniform vec4 uColor;\n"
    "void main() {\n"
    "   FragColor = uColor;\n"
    "}\0";

    updateShaderProg(vs, fs);
    
    _position = glm::vec3(0.0f);
    _velocity = glm::vec3(0.0f);
    
    _color = color;
    _isMoving = false;
    _isAirborne = false;
    
    _forceAccumulator = glm::vec3(0.0f, -GRAVITY * getMass(), 0.0f);
}

Object::~Object()
{
    glDeleteVertexArrays(1, &_VAO);
    glDeleteBuffers(1, &_VBO);
    glDeleteBuffers(1, &_EBO);
    glDeleteProgram(_shaderProgram);
}

void Object::move(float dt) 
{
    std::lock_guard<std::mutex> lock(_posMtx);

    _acceleration = _forceAccumulator / _mass;
    _displayAcceleration = _acceleration;

    _velocity += _acceleration * dt;
    _position += _velocity * dt;

    //glm::vec3 angularAcceleration = _torqueAccumulator / _I;
    //_angularVelocity += angularAcceleration * dt;
    //_rotation += _angularVelocity * dt; 

    // --- X-AXIS SEAMLESS WRAPPING ---
    const float RIGHT_BOUNDARY = 5.0f;
    const float LEFT_BOUNDARY = -5.0f;
    const float SPAN_WIDTH = RIGHT_BOUNDARY - LEFT_BOUNDARY;

    if (_position.x > RIGHT_BOUNDARY)
    {
        float overshoot = _position.x - RIGHT_BOUNDARY;
        _position.x = LEFT_BOUNDARY + overshoot;
    }
    else if (_position.x < LEFT_BOUNDARY)
    {
        float overshoot = LEFT_BOUNDARY - _position.x;
        _position.x = RIGHT_BOUNDARY - overshoot;
    }

    // --- Z-AXIS SEAMLESS WRAPPING ---
    const float FRONT_BOUNDARY = 10.0f;
    const float BACK_BOUNDARY = -10.0f;
    const float SPAN_DEPTH = FRONT_BOUNDARY - BACK_BOUNDARY;

    if (_position.z > FRONT_BOUNDARY)
    {
        float overshoot = _position.z - FRONT_BOUNDARY;
        _position.z = BACK_BOUNDARY + overshoot;
    }
    else if (_position.z < BACK_BOUNDARY)
    {
        float overshoot = BACK_BOUNDARY - _position.z;
        _position.z = FRONT_BOUNDARY - overshoot;
    }

    const float TOP_BOUNDARY = 5.0f;
    if (_position.y > TOP_BOUNDARY)
    {
        float overshoot = _position.y - TOP_BOUNDARY;
        _position.y = 0.0f + overshoot; 
    }

    if (_position.y - _halfHeight <= 0.0f && _velocity.y < 0.0f) 
    {
        _position.y = _halfHeight; 
        _velocity.y = 0.0f;
        
        //_angularVelocity *= 0.999f; 

        _isAirborne = false;
    }
    else
    {
        _isAirborne = true;
    }

    _forceAccumulator = glm::vec3(0.0f, -GRAVITY * _mass, 0.0f);
    //_torqueAccumulator = glm::vec3(0.0f, 0.0f, 0.0f);
    //_acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
}

bool Object::checkFloorCollision()
{
    float halfHeight = std::cbrt(_volume) * 0.5f;
    return (_position.y - halfHeight <= 0.0f) && (_velocity.y <= 0);
}

bool Object::operator==(const Object &other) const
{
    return ((this->getXpos() == other.getXpos()) && (this->getYpos() == other.getYpos()) && (this->getZpos() == other.getZpos()));
}

void Object::applyForce(const glm::vec3 &force)
{
    _forceAccumulator += force;
}

void Object::applyForceAtPoint(const glm::vec3& force, const glm::vec3& hitPoint) 
{
    glm::vec3 centerOfMass(_position.x, _position.y, _position.z);
    glm::vec3 r = hitPoint - centerOfMass;
    glm::vec3 torque = glm::cross(r, force);
    //_torqueAccumulator += torque;
    applyForce(force);
}

void Object::movePosition(const glm::vec3& posVector)
{
    std::lock_guard<std::mutex> lock(_posMtx);
    _position += posVector;
}

void Object::setForceAccumulator(const glm::vec3& force)
{
    _forceAccumulator = force;
}
