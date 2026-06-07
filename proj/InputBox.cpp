#include "pch.h"
#include "InputBox.h"

InputBox::InputBox(float x, float y, float width, float height, const Color& boxColor)
    : TextBox(x, y, width, height, boxColor), _x(x), _y(y), _width(width), _height(height),
      _currentInput(""), _isFocused(false), _backspacePressedLastFrame(false)
{
    setText("_"); // Visual cursor indicator initially
}

InputBox::~InputBox() {}

void InputBox::checkFocus(GLFWwindow* window)
{
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        int windowWidth, windowHeight;
        glfwGetWindowSize(window, &windowWidth, &windowHeight);

        float actualX = static_cast<float>(mouseX);
        float actualY = static_cast<float>(windowHeight) - static_cast<float>(mouseY);

        // If clicked inside bounds, focus it. Otherwise, unfocus.
        _isFocused = (actualX >= _x && actualX <= (_x + _width) &&
                      actualY >= _y && actualY <= (_y + _height));
    }
}

void InputBox::handleKeyboardInput(GLFWwindow* window)
{
    if (!_isFocused) return;

    static bool keyState[GLFW_KEY_LAST] = { false };

    auto checkKeyTrigger = [&](int key) -> bool {
        bool isPressed = (glfwGetKey(window, key) == GLFW_PRESS);
        if (isPressed && !keyState[key]) {
            keyState[key] = true;
            return true;
        } else if (!isPressed) {
            keyState[key] = false;
        }
        return false;
    };

    for (int key = GLFW_KEY_0; key <= GLFW_KEY_9; key++) {
        if (checkKeyTrigger(key)) {
            _currentInput += std::to_string(key - GLFW_KEY_0);
        }
    }

    if (checkKeyTrigger(GLFW_KEY_PERIOD)) {
        if (_currentInput.find('.') == std::string::npos) {
            _currentInput += ".";
        }
    }

    if (checkKeyTrigger(GLFW_KEY_MINUS)) {
        if (_currentInput.empty()) {
            _currentInput += "-";
        } else if (_currentInput[0] == '-') {
            _currentInput.erase(0, 1);
        } else {
            _currentInput.insert(0, "-");
        }
    }

    if (checkKeyTrigger(GLFW_KEY_BACKSPACE)) {
        if (!_currentInput.empty()) {
            _currentInput.pop_back();
        }
    }

    setText(_currentInput + (_isFocused ? "_" : ""));
}

void InputBox::clearInput()
{
    _currentInput = "";
    setText(_isFocused ? "_" : "");
}