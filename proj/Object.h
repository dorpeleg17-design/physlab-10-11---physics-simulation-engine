#pragma once
#include "pch.h"

enum class Face 
{
    None,
    Front, Back,
    Left, Right,
    Top, Bottom
};

class Object
{
protected:
    GLuint _VAO, _VBO, _EBO, _shaderProgram;
    std::vector<GLfloat> _vertices;

    // Movement and Position
    bool _isMoving, _isAirborne, _checkCollision;
    glm::vec3 _position; 
    glm::vec3 _velocity;
    float _volume, _mass, _halfHeight;
    float _I;
    glm::vec3 _forceAccumulator;
    //glm::vec3 _torqueAccumulator;
    //glm::vec3 _rotation;
    //glm::vec3 _angularVelocity;
    glm::vec3 _acceleration;
    glm::vec3 _displayAcceleration;
    Color _color;

    void updateShaderProg(const char* vs, const char* fs);
    virtual void initVO(const std::vector<GLfloat>& vertices) = 0;

public:
    Object(std::vector<GLfloat> vertices, const Color& color);
    virtual ~Object();

    // The 'pvMatrix' is the combined Projection * View
    virtual void draw(const glm::mat4& pvMatrix) = 0;

    // Physics
    virtual void move(float dt);
    
    // Getters/Setters
    float getXpos() const { return _position.x; }
    float getYpos() const { return _position.y; }
    float getZpos() const { return _position.z; }
    float getXvelocity() const { return _velocity.x; }
    float getYvelocity() const { return _velocity.y; }
    float getZvelocity() const { return _velocity.z; }
    
    glm::vec3 getVelocity() const { return _velocity; }
    void setVelocity(const glm::vec3& vel) { _velocity = vel; }
    void setAcceleration(const glm::vec3& acc) { _acceleration = acc; }
    
    void setXpos(float x) { _position.x = x; }
    void setYpos(float y) { _position.y = y; }
    void setZpos(float z) { _position.z = z; }

    void newXVelocity(const float x) { _velocity.x = x; }
    void newYVelocity(const float y) { _velocity.y = y; }
    void newZVelocity(const float z) { _velocity.z = z; }
    bool checkFloorCollision();
    bool operator==(const Object& other) const;
    virtual Face getHitFace(const Object &other) const = 0;
    float getVolume() const { return _volume; }
    float getMass() const {return _mass; }
    void applyForce(const glm::vec3& force);
    void applyForceAtPoint(const glm::vec3 &force, const glm::vec3 &hitPoint);
    void movePosition(const glm::vec3& posVector);
    bool isAirborne() const {return _isAirborne; }
    //glm::vec3 getAngularVelocity() const { return _angularVelocity; }
    bool getCollisionBool() const {return _checkCollision; }
    glm::vec3 getForceAccumulator() const {return _forceAccumulator; }
    void setForceAccumulator(const glm::vec3& force);
    glm::vec3 getPosVector() const {return _position; }
    glm::vec3 getAcceleration() const {return _acceleration; }
    glm::vec3 getDisplayAcceleration() const {return _displayAcceleration; }
    //void setAngularVelocity(const glm::vec3& w) { _angularVelocity = w; }
    float getInertia() const { return _I; }
};

struct Wire
{
    GLuint _VAO, _VBO, _shaderProgram;
    std::vector<GLfloat> _vertices;
    glm::vec3 _anchorPoint;
    glm::vec3 _objectPoint;
    float _maxLength;
    glm::vec3 _tension;
    Object* _connectedObj;

    Wire(Object* connectedObj, const glm::vec3& anchorPoint, float maxLength) : _connectedObj(connectedObj), _anchorPoint(anchorPoint), _maxLength(maxLength)
    {
        setUp();
    }

    void setUp()
    {
        // Pre-fill the vertices array with 6 floats (2 points * 3 coordinates)
        _vertices.resize(6, 0.0f);

        glGenVertexArrays(1, &_VAO);
        glGenBuffers(1, &_VBO);

        glBindVertexArray(_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, _VBO);
        
        // Use GL_DYNAMIC_DRAW since we will update endpoints every single frame
        glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(GLfloat), _vertices.data(), GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0); 
        glBindVertexArray(0);
        
        compileProg();
    }

    void compileProg()
    {
        // Modified to accept a Uniform Project-View Matrix to match your camera
        const char* vertexShaderSource = "#version 330 core\n"
            "layout (location = 0) in vec3 aPos;\n"
            "uniform mat4 pvMatrix;\n"
            "void main()\n"
            "{\n"
            "   gl_Position = pvMatrix * vec4(aPos, 1.0);\n"
            "}\0";

        const char* fragmentShaderSource = "#version 330 core\n"
            "out vec4 FragColor;\n"
            "uniform vec3 wireColor;\n" // Dynamic coloring to visually represent tension!
            "void main()\n"
            "{\n"
            "   FragColor = vec4(wireColor, 1.0f);\n"
            "}\n\0";
        
        unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
        glCompileShader(vertexShader);

        unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
        glCompileShader(fragmentShader);

        _shaderProgram = glCreateProgram();
        glAttachShader(_shaderProgram, vertexShader);
        glAttachShader(_shaderProgram, fragmentShader);
        glLinkProgram(_shaderProgram);

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }
    
    void draw(const glm::mat4 &pvMatrix)
    {
        _objectPoint = _connectedObj->getPosVector();
        
        _vertices[0] = _anchorPoint.x;
        _vertices[1] = _anchorPoint.y;
        _vertices[2] = _anchorPoint.z;
        _vertices[3] = _objectPoint.x;
        _vertices[4] = _objectPoint.y;
        _vertices[5] = _objectPoint.z;

        glBindBuffer(GL_ARRAY_BUFFER, _VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, _vertices.size() * sizeof(GLfloat), _vertices.data());
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glUseProgram(_shaderProgram);
        
        GLint pvLoc = glGetUniformLocation(_shaderProgram, "pvMatrix");
        glUniformMatrix4fv(pvLoc, 1, GL_FALSE, &pvMatrix[0][0]);

        GLint colorLoc = glGetUniformLocation(_shaderProgram, "wireColor");
        float tensionFactor = glm::length(_tension) / 1000.0f;
        if (tensionFactor > 1.0f) tensionFactor = 1.0f;
        
        glm::vec3 dynamicColor = glm::mix(glm::vec3(1.0f, 0.5f, 0.2f), glm::vec3(1.0f, 0.0f, 0.0f), tensionFactor);
        glUniform3fv(colorLoc, 1, &dynamicColor[0]);

        glBindVertexArray(_VAO);
        glDrawArrays(GL_LINES, 0, 2); 
        glBindVertexArray(0);
    }
};