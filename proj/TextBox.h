#pragma once
#include "Object2D.h"
#include <string>
#include <vector>

class TextBox : public Object2D
{
private:
    GLuint _textureID;
    float _width, _height;
    std::string _text;

    void compileProg();
    GLuint generateFont();

protected:
    void initVO(const std::vector<GLfloat>& vertices) override;

public:
    TextBox(float x, float y, float width, float height, const Color& boxColor);
    virtual ~TextBox() override;

    void draw(const glm::mat4& orthoMatrix) override;
    void setText(const std::string& text);

    Face getHitFace(const Object& other) const override;
};