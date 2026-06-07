#pragma once
#include "TextBox.h"

class InputBox : public TextBox
{
private:
    float _x, _y, _width, _height;
    std::string _currentInput;
    bool _isFocused;
    bool _backspacePressedLastFrame;

public:
    InputBox(float x, float y, float width, float height, const Color& boxColor);
    ~InputBox() override;

    void checkFocus(GLFWwindow* window);
    void handleKeyboardInput(GLFWwindow* window);
    
    std::string getInputText() const { return _currentInput; }
    void clearInput();
    bool isFocused() const { return _isFocused; }
};