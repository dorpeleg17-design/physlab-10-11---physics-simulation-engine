#pragma once
#include "TextBox.h"

class Button : public TextBox
{
private:
    float _x, _y, _width, _height;

public:
    Button(float x, float y, float width, float height, const Color& buttonColor, const std::string& text);
    ~Button() override;

    bool isClicked(GLFWwindow* window);
};