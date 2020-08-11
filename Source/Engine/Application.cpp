#include <iostream>
#include <algorithm>
#include <memory>

#include "Application.h"
#include "Camera.h"
#include "Entity.h"
#include "Framebuffer.h"
#include "Viewport.h"

namespace MouseData
{
    float lastMouseX, lastMouseY;
    float mouseOffsetX, mouseOffsetY;
    float mouseScrollY;
    bool firstMouse = true;
    bool processMouseMovement = false;
    bool processMouseScroll = false;
}

using namespace glm;
using namespace MouseData;

void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void MouseCallback(GLFWwindow* window, double posX, double posY);
void ScrollCallback(GLFWwindow* window, double offsetX, double offsetY);
    
Application::Application() {};

Application& Application::Instance()
{
    static Application *instance = new Application();
    return *instance;
}    

int Application::Init()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    window_ = glfwCreateWindow(windowSize_.x, windowSize_.y, "Camera", NULL, NULL);
    if (window_ == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwSetWindowPos(window_, windowPosition_.x, windowPosition_.y);
    
    glfwMakeContextCurrent(window_);
    glfwSetFramebufferSizeCallback(window_, FramebufferSizeCallback);

    glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window_, MouseCallback);

    glfwSetScrollCallback(window_, ScrollCallback);
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state and init screen frambuffer
    // -----------------------------
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    InitViewport(windowSize_.x, windowSize_.y);
    camera_ = std::make_shared<Camera>();
    scene_ = std::make_shared<Scene>(camera_);

    // configure mouse position
    // -----------------------------
    lastMouseX = (float)windowSize_.x / 2;
    lastMouseX = (float)windowSize_.y / 2;

    return 0;
};

void Application::InitViewport(unsigned int width, unsigned int height)
{
    windowSize_ = glm::vec2(width, height);
    viewport_ = std::make_shared<Viewport>(width, height);
}

void Application::ProcessInput(float deltaTime)
{
    if(glfwGetKey(window_, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window_, true);
    }
    
    if (glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS)
        camera_->ProcessKeyboard(CAM_FORWARD, deltaTime);
    if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS)
        camera_->ProcessKeyboard(CAM_BACKWARD, deltaTime);
    if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS)
        camera_->ProcessKeyboard(CAM_LEFT, deltaTime);
    if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS)
        camera_->ProcessKeyboard(CAM_RIGHT, deltaTime);
    if (glfwGetKey(window_, GLFW_KEY_E) == GLFW_PRESS)
        camera_->ProcessKeyboard(CAM_UP, deltaTime);
    if (glfwGetKey(window_, GLFW_KEY_Q) == GLFW_PRESS)
        camera_->ProcessKeyboard(CAM_DOWN, deltaTime);

    if (processMouseMovement) {
        camera_->ProcessMouseMovement(mouseOffsetX, mouseOffsetY);    
        processMouseMovement = false;
    }

    if (processMouseScroll) {
        camera_->ProcessMouseScroll(mouseScrollY);
        processMouseScroll = false;
    }
}

void Application::AddEntity(std::shared_ptr<Entity> entity)
{
    scene_->AddEntity(entity);
}

void Application::Update()
{
    float currentFrame = glfwGetTime();
    deltaTime_ = currentFrame - lastFrame_;
    lastFrame_ = currentFrame;

    ProcessInput(deltaTime_);

    scene_->Update(deltaTime_);

    viewport_->Bind();
    scene_->Draw();
    viewport_->Render();
    viewport_->Draw();
    
    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    // -------------------------------------------------------------------------------
    glfwSwapBuffers(window_);
    glfwPollEvents();
}

bool Application::ShouldClose()
{
    return glfwWindowShouldClose(window_);    
};

void Application::Terminate()
{
    // entities_.clear();
    glfwTerminate();
};

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    Application::Instance().InitViewport(width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void MouseCallback(GLFWwindow* window, double posX, double posY)
{
    if (firstMouse) {
        lastMouseX = posX;
        lastMouseY = posY;
        firstMouse = false;
    }

    mouseOffsetX = posX - lastMouseX;
    mouseOffsetY = posY - lastMouseY;
    lastMouseX = posX;
    lastMouseY = posY;

    processMouseMovement = true;    
};

void ScrollCallback(GLFWwindow* window, double offsetX, double offsetY)
{
    mouseScrollY = offsetY;
    processMouseScroll = true;
}
