#include "pch.h"
#include "Button.h"

Button::Button(float x, float y, float width, float height, const Color& buttonColor, const std::string& text)
    : TextBox(x, y, width, height, buttonColor), _x(x), _y(y), _width(width), _height(height)
{
    setText(text);
}

Button::~Button()
{
}

bool Button::isClicked(GLFWwindow* window)
{
    if (!window) return false; 

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        int windowWidth, windowHeight;
        glfwGetWindowSize(window, &windowWidth, &windowHeight);

        float actualX = static_cast<float>(mouseX);
        float actualY = static_cast<float>(windowHeight) - static_cast<float>(mouseY);

        if (actualX >= _x && actualX <= (_x + _width) &&
            actualY >= _y && actualY <= (_y + _height))
        {
            return true;
        }
    }
    return false;
}