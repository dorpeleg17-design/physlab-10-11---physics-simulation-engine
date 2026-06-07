
#include "physicsHUD.h"

int physicsHUD::setUpWindow(GLFWwindow*& m_window)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Window", NULL, NULL);
    m_window = window;
    if (window == NULL) 
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) 
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    return 0;
}

mode physicsHUD::getModeUserOpt(GLFWwindow *window)
{
    Button blueButton(160.0f,  280.0f, 150.0f, 40.0f, {0.2f, 0.4f, 0.8f, 1.0f}, "MOMENTUM");
    Button greenButton(325.0f, 280.0f, 150.0f, 40.0f, {0.2f, 0.7f, 0.3f, 1.0f}, "DYNAMICS");
    Button redButton(490.0f,   280.0f, 150.0f, 40.0f, {0.8f, 0.2f, 0.2f, 1.0f}, "KINEMATICS");
    glm::mat4 orthoMatrix = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, -1.0f, 1.0f);
    while (!glfwWindowShouldClose(window)) 
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (blueButton.isClicked(window))
        {
            return mode::Momentum;
        }
        
        if (greenButton.isClicked(window))
        {
            return mode::Dynamics;
        }

        if (redButton.isClicked(window))
        {
            return mode::Kinematics;
        }

        blueButton.draw(orthoMatrix);
        greenButton.draw(orthoMatrix);
        redButton.draw(orthoMatrix);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    return mode::Momentum;
}

void physicsHUD::momentumLoop(const std::vector<Object *> &objVec, const std::vector<Wire *> &wireVec, ObjectsManager &objManager, GLFWwindow *window)
{
    for (const auto& obj : objVec)
    {
        if (obj != nullptr)
        {
            objManager.addObject(obj);
        }
    }

    for (const auto& wire : wireVec)
    {
        if (wire != nullptr)
        {
            objManager.addWire(*wire);
        }
    }
    
    glm::mat4 projection = glm::perspective(glm::radians(15.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 22.0f), 
                                 glm::vec3(0.0f, 0.0f, 0.0f), 
                                 glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 pvMatrix = projection * view;

    glm::mat4 orthoMatrix = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, -1.0f, 1.0f);
    
    TextBox statsDisplay(20.0f, 500.0f, 320.0f, 80.0f, {0.15f, 0.15f, 0.15f, 0.8f});
    TextBox fullScreenBg(0.0f, 0.0f, 800.0f, 600.0f, {0.05f, 0.05f, 0.05f, 1.0f});
    TextBox finalResultsBox(150.0f, 150.0f, 500.0f, 300.0f, {0.15f, 0.15f, 0.2f, 1.0f});

    float lastFrame = (float)glfwGetTime();
    float postCollisionTimer = 0.0f;
    bool hasCapturedFinalStats = false;

    while (!glfwWindowShouldClose(window)) 
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        
        float currentFrame = (float)glfwGetTime(); 
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        if (!stopSimulation)
        {
            objManager.updateObjects(pvMatrix, deltaTime);

            if (objVec.size() >= 2 && objVec[0] != nullptr && objVec[1] != nullptr)
            {
                std::ostringstream ss;
                ss << std::fixed << std::setprecision(2);
                ss << "Obj 1 | M: " << objVec[0]->getMass() << " V: " << objVec[0]->getXvelocity() << "\n"
                   << "Obj 2 | M: " << objVec[1]->getMass() << " V: " << objVec[1]->getXvelocity();
                
                statsDisplay.setText(ss.str());
            }

            glDisable(GL_DEPTH_TEST);
            statsDisplay.draw(orthoMatrix);
            glEnable(GL_DEPTH_TEST);
        }
        else
        {
            if (objVec.size() >= 2 && objVec[0] != nullptr && objVec[1] != nullptr)
            {
                if (postCollisionTimer < 1.0f)
                {
                    postCollisionTimer += deltaTime;
                    objManager.updateObjects(pvMatrix, deltaTime);

                    std::ostringstream ss;
                    ss << std::fixed << std::setprecision(2);
                    ss << "Obj 1 | M: " << objVec[0]->getMass() << " V: " << objVec[0]->getXvelocity() << "\n"
                       << "Obj 2 | M: " << objVec[1]->getMass() << " V: " << objVec[1]->getXvelocity();
                    statsDisplay.setText(ss.str());

                    glDisable(GL_DEPTH_TEST);
                    statsDisplay.draw(orthoMatrix);
                    glEnable(GL_DEPTH_TEST);
                }
                else 
                {
                    if (!hasCapturedFinalStats)
                    {
                        std::ostringstream ss;
                        ss << std::fixed << std::setprecision(2);
                        ss << "SIMULATION TERMINATED (1s Post-Collision)\n\n"
                           << "Object 1 Final Mass: " << objVec[0]->getMass() << "\n"
                           << "Object 1 Final Velocity: " << objVec[0]->getXvelocity() << "\n\n"
                           << "Object 2 Final Mass: " << objVec[1]->getMass() << "\n"
                           << "Object 2 Final Velocity: " << objVec[1]->getXvelocity();

                        finalResultsBox.setText(ss.str());
                        hasCapturedFinalStats = true; 
                    }

                    glDisable(GL_DEPTH_TEST);
                    fullScreenBg.draw(orthoMatrix);
                    finalResultsBox.draw(orthoMatrix);
                    glEnable(GL_DEPTH_TEST);
                }
            }
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void physicsHUD::getCollisionModeUserOpt(GLFWwindow* window)
{
    Button elasticButton(240.0f, 280.0f, 150.0f, 40.0f, {0.2f, 0.6f, 0.4f, 1.0f}, "ELASTIC");
    Button plasticButton(410.0f, 280.0f, 150.0f, 40.0f, {0.7f, 0.4f, 0.2f, 1.0f}, "PLASTIC");
    glm::mat4 orthoMatrix = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, -1.0f, 1.0f);
    
    bool skipInitialClickCheck = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);

    while (!glfwWindowShouldClose(window)) 
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (skipInitialClickCheck && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
        {
            skipInitialClickCheck = false;
        }

        if (!skipInitialClickCheck)
        {
            if (elasticButton.isClicked(window))
            {
                collisionConstant = ELASTIC;
                return;
            }

            if (plasticButton.isClicked(window))
            {   
                collisionConstant = PLASTIC;
                return;
            }
        }

        elasticButton.draw(orthoMatrix);
        plasticButton.draw(orthoMatrix);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    collisionConstant = ELASTIC;
}

std::pair<Object*, Object*> physicsHUD::getObjectsStats(GLFWwindow* window)
{
    static float boxWidth = 140.0f;
    static float boxHeight = 35.0f;
    static float gap = 40.0f;
    static float startX = 60.0f;
    static float inputY = 400.0f;
    static float labelY = 435.0f;
    
    static Color labelColor = { 0.9f, 0.9f, 0.9f, 1.0f };
    static Color inputBgColor = { 0.15f, 0.15f, 0.15f, 1.0f };

    static float x1 = startX;
    static float x2 = x1 + boxWidth + gap;
    static float x3 = x2 + boxWidth + gap;
    static float x4 = x3 + boxWidth + gap;

    static TextBox lblVel1(x1, labelY, boxWidth, boxHeight, labelColor);
    static TextBox lblVel2(x2, labelY, boxWidth, boxHeight, labelColor);
    static TextBox lblMass1(x3, labelY, boxWidth, boxHeight, labelColor);
    static TextBox lblMass2(x4, labelY, boxWidth, boxHeight, labelColor);

    static InputBox inVel1(x1, inputY, boxWidth, boxHeight, inputBgColor);
    static InputBox inVel2(x2, inputY, boxWidth, boxHeight, inputBgColor);
    static InputBox inMass1(x3, inputY, boxWidth, boxHeight, inputBgColor);
    static InputBox inMass2(x4, inputY, boxWidth, boxHeight, inputBgColor);

    static Button applyConfigBtn(325.0f, 320.0f, 150.0f, 40.0f, {0.2f, 0.5f, 0.2f, 1.0f}, "APPLY VALUES");

    static bool variablesConfigured = false;
    if (!variablesConfigured)
    {
        lblVel1.setText("Velocity 1");
        lblVel2.setText("Velocity 2");
        lblMass1.setText("Mass 1");
        lblMass2.setText("Mass 2");
        variablesConfigured = true;
    }

    bool skipInitialClickCheck = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);

    while (!glfwWindowShouldClose(window))
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        inVel1.checkFocus(window);  inVel1.handleKeyboardInput(window);
        inVel2.checkFocus(window);  inVel2.handleKeyboardInput(window);
        inMass1.checkFocus(window); inMass1.handleKeyboardInput(window);
        inMass2.checkFocus(window); inMass2.handleKeyboardInput(window);

        if (skipInitialClickCheck && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
        {
            skipInitialClickCheck = false;
        }

        if (!skipInitialClickCheck)
        {
            if (applyConfigBtn.isClicked(window))
            {
                try {
                    float v1 = inVel1.getInputText().empty() ? 0.0f : std::stof(inVel1.getInputText());
                    float v2 = inVel2.getInputText().empty() ? 0.0f : std::stof(inVel2.getInputText());
                    float m1 = std::abs(inMass1.getInputText().empty() ? 1.0f : std::stof(inMass1.getInputText()));
                    float m2 = std::abs(inMass2.getInputText().empty() ? 1.0f : std::stof(inMass2.getInputText()));

                    std::cout << "V1: " << v1 << " | V2: " << v2 << " | M1: " << m1 << " | M2: " << m2 << std::endl;
                    
                    float volume1 = m1 / MATTER_DENSITY;
                    float volume2 = m2 / MATTER_DENSITY;
                    float width1 = std::cbrt(volume1);
                    float width2 = std::cbrt(volume2);

                    // Adjusted spawn gaps so objects reside fully inside the [-5.0f, 5.0f] tracking frame
                    float travelGap = 1.5f; 
                    
                    float leftSpawnX  = -(width1 * 0.5f + travelGap);
                    float rightSpawnX =  (width2 * 0.5f + travelGap);

                    float leftSpawnY  = width1 * 0.5f;
                    float rightSpawnY = width2 * 0.5f;

                    Box* leftBox = new Box({1.0f, 0.2f, 0.2f, 1.0f}, leftSpawnX, leftSpawnY, 0.0f, m1);
                    Box* rightBox = new Box({0.2f, 0.4f, 1.0f, 1.0f}, rightSpawnX, rightSpawnY, 0.0f, m2);
                    
                    leftBox->newXVelocity(v1);
                    rightBox->newXVelocity(v2);
                    
                    return std::pair<Object*, Object*>(leftBox, rightBox);
                } 
                catch (...) 
                {
                    return std::pair<Object*, Object*>(nullptr, nullptr);
                }
            }
        }

        glm::mat4 orthoMatrix = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, -1.0f, 1.0f);

        lblVel1.draw(orthoMatrix);  inVel1.draw(orthoMatrix);
        lblVel2.draw(orthoMatrix);  inVel2.draw(orthoMatrix);
        lblMass1.draw(orthoMatrix); inMass1.draw(orthoMatrix);
        lblMass2.draw(orthoMatrix); inMass2.draw(orthoMatrix);
        applyConfigBtn.draw(orthoMatrix);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    return std::pair<Object*, Object*>(nullptr, nullptr);
}

bool physicsHUD::getDynamicsModeUserOpt(GLFWwindow* window)
{
    Button wireButton(240.0f, 280.0f, 150.0f, 40.0f, {0.2f, 0.6f, 0.4f, 1.0f}, "WIRE");
    Button noWireButton(410.0f, 280.0f, 150.0f, 40.0f, {0.7f, 0.4f, 0.2f, 1.0f}, "NO WIRE");
    glm::mat4 orthoMatrix = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, -1.0f, 1.0f);
    
    bool skipInitialClickCheck = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);

    while (!glfwWindowShouldClose(window)) 
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (skipInitialClickCheck && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
        {
            skipInitialClickCheck = false;
        }

        if (!skipInitialClickCheck)
        {
            if (wireButton.isClicked(window))
            {
                return true;
            }

            if (noWireButton.isClicked(window))
            {   
                return false;
            }
        }

        wireButton.draw(orthoMatrix);
        noWireButton.draw(orthoMatrix);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    return false;
}

float physicsHUD::getWireMaxLength(GLFWwindow* window)
{
    static float boxWidth = 280.0f;
    static float boxHeight = 35.0f;
    static float gap = 40.0f;
    static float startX = 200.0f;
    static float inputY = 400.0f;
    static float labelY = 435.0f;
    
    static Color labelColor = { 0.9f, 0.9f, 0.9f, 1.0f };
    static Color inputBgColor = { 0.15f, 0.15f, 0.15f, 1.0f };

    static float x1 = startX;

    static TextBox lblLen1(x1, labelY, boxWidth, boxHeight, labelColor);
    static InputBox inLen1(x1, inputY, boxWidth, boxHeight, inputBgColor);


    static Button applyConfigBtn(325.0f, 320.0f, 150.0f, 40.0f, {0.2f, 0.5f, 0.2f, 1.0f}, "APPLY VALUES");

    static bool variablesConfigured = false;
    if (!variablesConfigured)
    {
        lblLen1.setText("Wire max length:");
        variablesConfigured = true;
    }

    bool skipInitialClickCheck = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);

    while (!glfwWindowShouldClose(window))
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        inLen1.checkFocus(window);  inLen1.handleKeyboardInput(window);

        if (skipInitialClickCheck && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
        {
            skipInitialClickCheck = false;
        }

        if (!skipInitialClickCheck)
        {
            if (applyConfigBtn.isClicked(window))
            {
                try {
                    float len = inLen1.getInputText().empty() ? 0.0f : std::stof(inLen1.getInputText());
                    std::cout << "Max length: " << len << std::endl;               
                    return len;
                     
                } 
                catch (...) 
                {
                    return 1.0f;
                }
            }
        }

        glm::mat4 orthoMatrix = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, -1.0f, 1.0f);

        lblLen1.draw(orthoMatrix);  inLen1.draw(orthoMatrix);
        applyConfigBtn.draw(orthoMatrix);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    return 1.0f;
}

std::pair<Object*, glm::vec3> physicsHUD::getForces(GLFWwindow* window)
{
    static float boxWidth = 140.0f;
    static float boxHeight = 35.0f;
    static float gap = 40.0f;
    static float startX = 60.0f;
    static float inputY = 400.0f;
    static float labelY = 435.0f;
    
    static Color labelColor = { 0.9f, 0.9f, 0.9f, 1.0f };
    static Color inputBgColor = { 0.15f, 0.15f, 0.15f, 1.0f };

    static float x1 = startX;
    static float x2 = x1 + boxWidth + gap;
    static float x3 = x2 + boxWidth + gap;
    static float x4 = x3 + boxWidth + gap;

    static TextBox lblforceX(x1, labelY, boxWidth, boxHeight, labelColor);
    static TextBox lblforceY(x2, labelY, boxWidth, boxHeight, labelColor);
    static TextBox lblforceZ(x3, labelY, boxWidth, boxHeight, labelColor);
    static TextBox lblMass(x4, labelY, boxWidth, boxHeight, labelColor);

    static InputBox inForceX(x1, inputY, boxWidth, boxHeight, inputBgColor);
    static InputBox inForceY(x2, inputY, boxWidth, boxHeight, inputBgColor);
    static InputBox inForceZ(x3, inputY, boxWidth, boxHeight, inputBgColor);
    static InputBox inMass(x4, inputY, boxWidth, boxHeight, inputBgColor);

    static Button applyConfigBtn(325.0f, 320.0f, 150.0f, 40.0f, {0.2f, 0.5f, 0.2f, 1.0f}, "APPLY VALUES");

    static bool variablesConfigured = false;
    if (!variablesConfigured)
    {
        lblforceX.setText("Force X:");
        lblforceY.setText("Force Y:");
        lblforceZ.setText("Force Z:");
        lblMass.setText("Object mass:");
        variablesConfigured = true;
    }

    bool skipInitialClickCheck = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);

    while (!glfwWindowShouldClose(window))
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        inForceX.checkFocus(window);  inForceX.handleKeyboardInput(window);
        inForceY.checkFocus(window);  inForceY.handleKeyboardInput(window);
        inForceZ.checkFocus(window); inForceZ.handleKeyboardInput(window);
        inMass.checkFocus(window); inMass.handleKeyboardInput(window);

        if (skipInitialClickCheck && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
        {
            skipInitialClickCheck = false;
        }

        if (!skipInitialClickCheck)
        {
            if (applyConfigBtn.isClicked(window))
            {
                try {
                    float forceX = inForceX.getInputText().empty() ? 0.0f : std::stof(inForceX.getInputText());
                    float forceY = inForceY.getInputText().empty() ? 0.0f : std::stof(inForceY.getInputText());
                    float forceZ = inForceZ.getInputText().empty() ? 0.0f : std::stof(inForceZ.getInputText());
                    float mass = std::abs(inMass.getInputText().empty() ? 1.0f : std::stof(inMass.getInputText()));

                    std::cout << "Force X: " << forceX << " | force Y: " << forceY << " | force Z: " << forceZ << " | mass: " << mass << std::endl;

                    Box* obj = new Box({1.0f, 0.2f, 0.2f, 1.0f}, 0.0f, 0.0f, 0.0f, mass);
                    glm::vec3 appliedForce(forceX, forceY, forceZ);
                    
                    return std::pair<Object*, glm::vec3>(obj, appliedForce);
                } 
                catch (...) 
                {
                    return std::pair<Object*, glm::vec3>(nullptr, glm::vec3(0.0f));
                }
            }
        }

        glm::mat4 orthoMatrix = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, -1.0f, 1.0f);

        lblforceX.draw(orthoMatrix);  inForceX.draw(orthoMatrix);
        lblforceY.draw(orthoMatrix);  inForceY.draw(orthoMatrix);
        lblforceZ.draw(orthoMatrix); inForceZ.draw(orthoMatrix);
        lblMass.draw(orthoMatrix);    inMass.draw(orthoMatrix);
        applyConfigBtn.draw(orthoMatrix);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    return std::pair<Object*, glm::vec3>(nullptr, glm::vec3(0.0f));
}

void physicsHUD::dynamicsLoop(const std::vector<Object *> &objVec, const std::vector<Wire *> &wireVec, ObjectsManager &objManager, const glm::vec3& appliedForce, GLFWwindow *window)
{
    for (const auto& obj : objVec)
    {
        objManager.addObject(obj);
    }

    for (const auto& wire : wireVec)
    {
        objManager.addWire(*wire);
    }
    
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    glm::mat4 orthoMatrix = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, -1.0f, 1.0f);
    
    TextBox statsDisplay(20.0f, 480.0f, 360.0f, 100.0f, {0.15f, 0.15f, 0.15f, 0.8f});
    TextBox fullScreenBg(0.0f, 0.0f, 800.0f, 600.0f, {0.05f, 0.05f, 0.05f, 1.0f});
    TextBox finalResultsBox(150.0f, 150.0f, 500.0f, 300.0f, {0.15f, 0.15f, 0.2f, 1.0f});

    float lastFrame = (float)glfwGetTime();

    while (!glfwWindowShouldClose(window)) 
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        
        float currentFrame = (float)glfwGetTime(); 
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::vec3 cameraPos(2.5f, 2.5f, 5.5f);
        glm::vec3 targetPos(0.0f, 0.0f, 0.0f);
        
        glm::mat4 view = glm::lookAt(cameraPos, targetPos, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 pvMatrix = projection * view;
        
        if (!stopSimulation)
        {
            if (!objVec.empty() && objVec[0] != nullptr)
            {
                objVec[0]->applyForce(appliedForce);
            }

            objManager.updateObjects(pvMatrix, deltaTime);

            if (!objVec.empty() && objVec[0] != nullptr)
            {
                glm::vec3 acc = objVec[0]->getAcceleration();

                std::string statsText = std::string("OBJECT ACCELERATION:\n") +
                                        std::string("Acc X: ") + std::to_string(acc.x) + std::string(" m/s^2\n") +
                                        std::string("Acc Y: ") + std::to_string(acc.y) + std::string(" m/s^2\n") +
                                        std::string("Acc Z: ") + std::to_string(acc.z) + std::string(" m/s^2\n");
                
                statsDisplay.setText(statsText);
            }

            glDisable(GL_DEPTH_TEST);
            statsDisplay.draw(orthoMatrix);
            glEnable(GL_DEPTH_TEST);
        }
        else
        {
            if (!objVec.empty() && objVec[0] != nullptr)
            {
                glm::vec3 acc = objVec[0]->getAcceleration();
                float mass = objVec[0]->getMass();

                std::string finalStatsText = "SIMULATION TERMINATED\n\n"
                                             "Final Acceleration Status:\n"
                                             "X: " + std::to_string(acc.x) + " m/s^2\n" +
                                             "Y: " + std::to_string(acc.y) + " m/s^2\n" +
                                             "Z: " + std::to_string(acc.z) + " m/s^2\n" +
                                             "Object Mass: " + std::to_string(mass) + " kg";
                finalResultsBox.setText(finalStatsText);
            }

            glDisable(GL_DEPTH_TEST);
            fullScreenBg.draw(orthoMatrix);
            finalResultsBox.draw(orthoMatrix);
            glEnable(GL_DEPTH_TEST);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}



kinematicsInitializationData physicsHUD::getInitialKinematicsStats(GLFWwindow *window)
{
    static float boxWidth = 140.0f;
    static float boxHeight = 32.0f;
    static float gapX = 220.0f;       
    static float gapY = 100.0f;       
    static float inputOffsetY = 38.0f; 
    
    static float startX = 60.0f;       
    static float labelYStart = 480.0f;
    static float inputYStart = labelYStart - inputOffsetY;
    
    static Color labelColor = { 0.15f, 0.15f, 0.15f, 0.6f };  
    static Color inputBgColor = { 0.22f, 0.22f, 0.22f, 0.9f }; 

    static TextBox lblVelX(startX, labelYStart, boxWidth, boxHeight, labelColor);
    static TextBox lblVelY(startX + gapX, labelYStart, boxWidth, boxHeight, labelColor);
    static TextBox lblVelZ(startX + (gapX * 2), labelYStart, boxWidth, boxHeight, labelColor);
    
    static InputBox inVelX(startX, inputYStart, boxWidth, boxHeight, inputBgColor);
    static InputBox inVelY(startX + gapX, inputYStart, boxWidth, boxHeight, inputBgColor);
    static InputBox inVelZ(startX + (gapX * 2), inputYStart, boxWidth, boxHeight, inputBgColor);

    float r2LabelY = labelYStart - gapY;
    float r2InputY = r2LabelY - inputOffsetY;
    static TextBox lblAccX(startX, r2LabelY, boxWidth, boxHeight, labelColor);
    static TextBox lblAccY(startX + gapX, r2LabelY, boxWidth, boxHeight, labelColor);
    static TextBox lblAccZ(startX + (gapX * 2), r2LabelY, boxWidth, boxHeight, labelColor);
    
    static InputBox inAccX(startX, r2InputY, boxWidth, boxHeight, inputBgColor);
    static InputBox inAccY(startX + gapX, r2InputY, boxWidth, boxHeight, inputBgColor);
    static InputBox inAccZ(startX + (gapX * 2), r2InputY, boxWidth, boxHeight, inputBgColor);

    float r3LabelY = r2LabelY - gapY;
    float r3InputY = r3LabelY - inputOffsetY;
    static TextBox lblPosX(startX, r3LabelY, boxWidth, boxHeight, labelColor);
    static TextBox lblPosY(startX + gapX, r3LabelY, boxWidth, boxHeight, labelColor);
    static TextBox lblPosZ(startX + (gapX * 2), r3LabelY, boxWidth, boxHeight, labelColor);
    
    static InputBox inPosX(startX, r3InputY, boxWidth, boxHeight, inputBgColor);
    static InputBox inPosY(startX + gapX, r3InputY, boxWidth, boxHeight, inputBgColor);
    static InputBox inPosZ(startX + (gapX * 2), r3InputY, boxWidth, boxHeight, inputBgColor);

    static float btnWidth = 200.0f;
    static float btnHeight = 42.0f;
    static float btnX = startX + gapX - ((btnWidth - boxWidth) / 2.0f);
    static float btnY = r3InputY - 80.0f;
    static Button applyConfigBtn(btnX, btnY, btnWidth, btnHeight, {0.12f, 0.45f, 0.22f, 1.0f}, "CONFIRM OBJECT");

    static bool variablesConfigured = false;
    if (!variablesConfigured)
    {
        lblVelX.setText("Velocity X:");   lblVelY.setText("Velocity Y:");   lblVelZ.setText("Velocity Z:");
        lblAccX.setText("Accel X:");      lblAccY.setText("Accel Y:");      lblAccZ.setText("Accel Z:");
        lblPosX.setText("Spawn Pos X:");  lblPosY.setText("Spawn Pos Y:");  lblPosZ.setText("Spawn Pos Z:");
        variablesConfigured = true;
    }

    bool skipInitialClickCheck = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);

    while (!glfwWindowShouldClose(window))
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        inVelX.checkFocus(window); inVelX.handleKeyboardInput(window);
        inVelY.checkFocus(window); inVelY.handleKeyboardInput(window);
        inVelZ.checkFocus(window); inVelZ.handleKeyboardInput(window);

        inAccX.checkFocus(window); inAccX.handleKeyboardInput(window);
        inAccY.checkFocus(window); inAccY.handleKeyboardInput(window);
        inAccZ.checkFocus(window); inAccZ.handleKeyboardInput(window);

        inPosX.checkFocus(window); inPosX.handleKeyboardInput(window);
        inPosY.checkFocus(window); inPosY.handleKeyboardInput(window);
        inPosZ.checkFocus(window); inPosZ.handleKeyboardInput(window);

        if (skipInitialClickCheck && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
        {
            skipInitialClickCheck = false;
        }

        if (!skipInitialClickCheck)
        {
            if (applyConfigBtn.isClicked(window))
            {
                try {
                    kinematicsInitializationData data;
                    
                    data.initialVelocity.x = inVelX.getInputText().empty() ? 0.0f : std::stof(inVelX.getInputText());
                    data.initialVelocity.y = inVelY.getInputText().empty() ? 0.0f : std::stof(inVelY.getInputText());
                    data.initialVelocity.z = inVelZ.getInputText().empty() ? 0.0f : std::stof(inVelZ.getInputText());

                    data.initialAcceleration.x = inAccX.getInputText().empty() ? 0.0f : std::stof(inAccX.getInputText());
                    data.initialAcceleration.y = inAccY.getInputText().empty() ? 0.0f : std::stof(inAccY.getInputText());
                    data.initialAcceleration.z = inAccZ.getInputText().empty() ? 0.0f : std::stof(inAccZ.getInputText());

                    data.startingPosition.x = inPosX.getInputText().empty() ? 0.0f : std::stof(inPosX.getInputText());
                    data.startingPosition.y = inPosY.getInputText().empty() ? 0.0f : std::stof(inPosY.getInputText());
                    data.startingPosition.z = inPosZ.getInputText().empty() ? 0.0f : std::stof(inPosZ.getInputText());
                    
                    data.isValid = true;
                    return data;
                } 
                catch (...) 
                {
                    kinematicsInitializationData errorData;
                    errorData.isValid = false;
                    return errorData;
                }
            }
        }

        glm::mat4 orthoMatrix = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, -1.0f, 1.0f);

        lblVelX.draw(orthoMatrix); inVelX.draw(orthoMatrix);
        lblVelY.draw(orthoMatrix); inVelY.draw(orthoMatrix);
        lblVelZ.draw(orthoMatrix); inVelZ.draw(orthoMatrix);

        lblAccX.draw(orthoMatrix); inAccX.draw(orthoMatrix);
        lblAccY.draw(orthoMatrix); inAccY.draw(orthoMatrix);
        lblAccZ.draw(orthoMatrix); inAccZ.draw(orthoMatrix);

        lblPosX.draw(orthoMatrix); inPosX.draw(orthoMatrix);
        lblPosY.draw(orthoMatrix); inPosY.draw(orthoMatrix);
        lblPosZ.draw(orthoMatrix); inPosZ.draw(orthoMatrix);

        applyConfigBtn.draw(orthoMatrix);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    kinematicsInitializationData closeData;
    closeData.isValid = false;
    return closeData;
}

void physicsHUD::kinematicsLoop(const std::vector<Object*>& objVec, const std::vector<Wire*>& wireVec, ObjectsManager& objManager, const kinematicsInitializationData& initData, GLFWwindow* window)
{
    bool useTimeTracking = getTrackingPreference(window);
    
    float targetTime = 0.0f;
    glm::vec3 targetPos(0.0f);

    if (useTimeTracking)
    {
        targetTime = getSpecificTimeValue(window);
    }
    else
    {
        targetPos = getSpecificPositionValue(window);
    }

    if (!objVec.empty() && objVec[0] != nullptr)
    {
        objVec[0]->setXpos(initData.startingPosition.x);
        objVec[0]->setYpos(initData.startingPosition.y);
        objVec[0]->setZpos(initData.startingPosition.z);
        objVec[0]->setVelocity(initData.initialVelocity);
    }

    for (const auto& obj : objVec)
    {
        objManager.addObject(obj);
    }

    for (const auto& wire : wireVec)
    {
        objManager.addWire(*wire);
    }

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    glm::mat4 orthoMatrix = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, -1.0f, 1.0f);

    TextBox kinematicsDisplay(20.0f, 400.0f, 420.0f, 180.0f, {0.1f, 0.15f, 0.2f, 0.85f});

    float lastFrame = (float)glfwGetTime();
    float totalElapsedTime = 0.0f;
    bool simulationRunning = true;

    // Fixed acceleration problem by using kinematics constants directly
    const glm::vec3 accel = initData.initialAcceleration; 
    const glm::vec3 startingPos = initData.startingPosition;
    const glm::vec3 initialVel = initData.initialVelocity;

    while (!glfwWindowShouldClose(window))
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        float currentFrame = (float)glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (!objVec.empty() && objVec[0] != nullptr)
        {
            Object* obj = objVec[0];
            
            // Overriding forces to ignore physics engine gravity updates
            obj->setAcceleration(glm::vec3(0.0f, 0.0f, 0.0f));
            obj->setForceAccumulator(glm::vec3(0.0f, 0.0f, 0.0f));

            if (simulationRunning)
            {
                totalElapsedTime += deltaTime;
            }

            float theoreticalX = startingPos.x + (initialVel.x * totalElapsedTime) + (0.5f * accel.x * totalElapsedTime * totalElapsedTime);
            float theoreticalY = startingPos.y + (initialVel.y * totalElapsedTime) + (0.5f * accel.y * totalElapsedTime * totalElapsedTime);
            float theoreticalZ = startingPos.z + (initialVel.z * totalElapsedTime) + (0.5f * accel.z * totalElapsedTime * totalElapsedTime);

            float theoreticalSpeedX = initialVel.x + (accel.x * totalElapsedTime);
            float theoreticalSpeedY = initialVel.y + (accel.y * totalElapsedTime);
            float theoreticalSpeedZ = initialVel.z + (accel.z * totalElapsedTime);
            
            if (simulationRunning)
            {
                if (useTimeTracking)
                {
                    if (targetTime > 0.0f && totalElapsedTime >= targetTime)
                    {
                        simulationRunning = false;
                    }
                }
                else 
                {
                    // FIX: Tracking condition now checks continuous theoretical properties 
                    // instead of the wrapped coordinate values.
                    bool conditionX = (targetPos.x == 0.0f) || (theoreticalSpeedX >= 0.0f ? theoreticalX >= targetPos.x : theoreticalX <= targetPos.x);
                    bool conditionY = (targetPos.y == 0.0f) || (theoreticalSpeedY >= 0.0f ? theoreticalY >= targetPos.y : theoreticalY <= targetPos.y);
                    bool conditionZ = (targetPos.z == 0.0f) || (theoreticalSpeedZ >= 0.0f ? theoreticalZ >= targetPos.z : theoreticalZ <= targetPos.z);

                    if (conditionX && conditionY && conditionZ)
                    {
                        simulationRunning = false;
                    }
                }
            }

            std::string hudText = std::string("THEORETICAL METRICS:\n") +
                                  "Theoretical X: " + std::to_string(theoreticalX) + " m\n" + 
                                  "Theoretical Y: " + std::to_string(theoreticalY) + " m\n" + 
                                  "Theoretical Z: " + std::to_string(theoreticalZ) + " m\n" + 
                                  "Theoretical Speed X: " + std::to_string(theoreticalSpeedX) + " m/s\n" +
                                  "Theoretical Speed Y: " + std::to_string(theoreticalSpeedY) + " m/s\n" +
                                  "Theoretical Speed Z: " + std::to_string(theoreticalSpeedZ) + " m/s\n" +
                                  (simulationRunning ? std::string("Status: Running") : std::string("Status: Target Met / Stopped"));
            
            kinematicsDisplay.setText(hudText);
        }

        glm::vec3 cameraPos(2.5f, 2.5f, 7.5f);
        glm::vec3 targetLookPos(0.0f, 0.0f, 0.0f);
        glm::mat4 view = glm::lookAt(cameraPos, targetLookPos, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 pvMatrix = projection * view;

        objManager.updateObjects(pvMatrix, simulationRunning ? deltaTime : 0.0f);

        glDisable(GL_DEPTH_TEST);
        kinematicsDisplay.draw(orthoMatrix);
        glEnable(GL_DEPTH_TEST);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

bool physicsHUD::getTrackingPreference(GLFWwindow* window)
{
    static float boxWidth = 140.0f;
    static float boxHeight = 42.0f;
    static float startX = 230.0f;
    static float gapX = 200.0f;
    static float startY = 280.0f;

    static Button btnTime(startX, startY, boxWidth, boxHeight, {0.12f, 0.45f, 0.22f, 1.0f}, "TIME");
    static Button btnPosition(startX + gapX, startY, boxWidth, boxHeight, {0.12f, 0.45f, 0.22f, 1.0f}, "POSITION");

    bool skipInitialClickCheck = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);

    while (!glfwWindowShouldClose(window))
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (skipInitialClickCheck && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
        {
            skipInitialClickCheck = false;
        }

        if (!skipInitialClickCheck)
        {
            if (btnTime.isClicked(window))
            {
                return true;
            }
            if (btnPosition.isClicked(window))
            {
                return false;
            }
        }

        glm::mat4 orthoMatrix = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, -1.0f, 1.0f);

        btnTime.draw(orthoMatrix);
        btnPosition.draw(orthoMatrix);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return false;
}

float physicsHUD::getSpecificTimeValue(GLFWwindow* window)
{
    static float boxWidth = 160.0f;
    static float boxHeight = 32.0f;
    static float startX = 320.0f;
    static float labelY = 340.0f;
    static float inputY = 300.0f;

    static Color labelColor = { 0.15f, 0.15f, 0.15f, 0.6f };
    static Color inputBgColor = { 0.22f, 0.22f, 0.22f, 0.9f };

    static TextBox lblTime(startX, labelY, boxWidth, boxHeight, labelColor);
    static InputBox inTime(startX, inputY, boxWidth, boxHeight, inputBgColor);

    static float btnWidth = 160.0f;
    static float btnHeight = 42.0f;
    static Button btnConfirm(startX, inputY - 80.0f, btnWidth, btnHeight, {0.12f, 0.45f, 0.22f, 1.0f}, "CONFIRM TIME");

    static bool configured = false;
    if (!configured)
    {
        lblTime.setText("Target Time (s):");
        configured = true;
    }

    bool skipInitialClickCheck = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);

    while (!glfwWindowShouldClose(window))
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        inTime.checkFocus(window);
        inTime.handleKeyboardInput(window);

        if (skipInitialClickCheck && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
        {
            skipInitialClickCheck = false;
        }

        if (!skipInitialClickCheck)
        {
            if (btnConfirm.isClicked(window))
            {
                try
                {
                    return inTime.getInputText().empty() ? 0.0f : std::stof(inTime.getInputText());
                }
                catch (...)
                {
                    return 0.0f;
                }
            }
        }

        glm::mat4 orthoMatrix = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, -1.0f, 1.0f);

        lblTime.draw(orthoMatrix);
        inTime.draw(orthoMatrix);
        btnConfirm.draw(orthoMatrix);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0.0f;
}

glm::vec3 physicsHUD::getSpecificPositionValue(GLFWwindow* window)
{
    static float boxWidth = 140.0f;
    static float boxHeight = 32.0f;
    static float gapX = 220.0f;
    static float inputOffsetY = 38.0f;

    static float startX = 60.0f;
    static float labelY = 340.0f;
    static float inputY = labelY - inputOffsetY;

    static Color labelColor = { 0.15f, 0.15f, 0.15f, 0.6f };
    static Color inputBgColor = { 0.22f, 0.22f, 0.22f, 0.9f };

    static TextBox lblX(startX, labelY, boxWidth, boxHeight, labelColor);
    static TextBox lblY(startX + gapX, labelY, boxWidth, boxHeight, labelColor);
    static TextBox lblZ(startX + (gapX * 2), labelY, boxWidth, boxHeight, labelColor);

    static InputBox inX(startX, inputY, boxWidth, boxHeight, inputBgColor);
    static InputBox inY(startX + gapX, inputY, boxWidth, boxHeight, inputBgColor);
    static InputBox inZ(startX + (gapX * 2), inputY, boxWidth, boxHeight, inputBgColor);

    static float btnWidth = 200.0f;
    static float btnHeight = 42.0f;
    static float btnX = startX + gapX - ((btnWidth - boxWidth) / 2.0f);
    static float btnY = inputY - 80.0f;
    static Button btnConfirm(btnX, btnY, btnWidth, btnHeight, {0.12f, 0.45f, 0.22f, 1.0f}, "CONFIRM POSITION");

    static bool configured = false;
    if (!configured)
    {
        lblX.setText("Target X:");
        lblY.setText("Target Y:");
        lblZ.setText("Target Z:");
        configured = true;
    }

    bool skipInitialClickCheck = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);

    while (!glfwWindowShouldClose(window))
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        inX.checkFocus(window); inX.handleKeyboardInput(window);
        inY.checkFocus(window); inY.handleKeyboardInput(window);
        inZ.checkFocus(window); inZ.handleKeyboardInput(window);

        if (skipInitialClickCheck && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
        {
            skipInitialClickCheck = false;
        }

        if (!skipInitialClickCheck)
        {
            bool dynamicValid = !inX.getInputText().empty() || !inY.getInputText().empty() || !inZ.getInputText().empty();

            if (dynamicValid && btnConfirm.isClicked(window))
            {
                try
                {
                    glm::vec3 pos;
                    pos.x = inX.getInputText().empty() ? 0.0f : std::stof(inX.getInputText());
                    pos.y = inY.getInputText().empty() ? 0.0f : std::stof(inY.getInputText());
                    pos.z = inZ.getInputText().empty() ? 0.0f : std::stof(inZ.getInputText());
                    return pos;
                }
                catch (...)
                {
                    return glm::vec3(0.0f);
                }
            }
        }

        glm::mat4 orthoMatrix = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, -1.0f, 1.0f);

        lblX.draw(orthoMatrix); inX.draw(orthoMatrix);
        lblY.draw(orthoMatrix); inY.draw(orthoMatrix);
        lblZ.draw(orthoMatrix); inZ.draw(orthoMatrix);

        btnConfirm.draw(orthoMatrix);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return glm::vec3(0.0f);
}