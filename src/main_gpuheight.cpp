#include "shader.h"
#include "camera.h"
#include "common.hpp"
#include "stb_image.h"
#include <iostream>
#include <vector>

void key_callback_gpuheight(GLFWwindow* window, int key, int scancode, int action, int modifiers);
void mouse_callback_gpuheight(GLFWwindow* window, double xpos, double ypos);
void scroll_callback_gpuheight(GLFWwindow* window, double xoffset, double yoffset);
void processInput_gpuheight(GLFWwindow *window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

Camera camera_gpuheight(glm::vec3(67.0f, 627.5f, 169.9f),
                        glm::vec3(0.0f, 1.0f, 0.0f),
                        -128.1f, -42.4f);

float lastX_gpuheight = SCR_WIDTH / 2.0f;
float lastY_gpuheight = SCR_HEIGHT / 2.0f;
bool firstMouse_gpuheight = true;

// timing
float deltaTime_gpuheight = 0.0f;
float lastFrame_gpuheight = 0.0f;

int main_gpuheight()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL: Terrain CPU", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback_gpuheight);
    glfwSetCursorPosCallback(window, mouse_callback_gpuheight);
    glfwSetScrollCallback(window, scroll_callback_gpuheight);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout<<"Failed to load glad loader"<<std::endl;
        glfwTerminate();
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    Shader tessHeightMapShader("/Users/cabin/Documents/cpp/learngl/shaders/gpuheight.vert",
                               "/Users/cabin/Documents/cpp/learngl/shaders/gpuheight.frag",
                               nullptr,
                               "/Users/cabin/Documents/cpp/learngl/shaders/gpuheight.tcs",
                               "/Users/cabin/Documents/cpp/learngl/shaders/gpuheight.tes");

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime_gpuheight = currentFrame - lastFrame_gpuheight;
        lastFrame_gpuheight = currentFrame;
        processInput_gpuheight(window);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.3f, 0.2f, 0.2f, 1.0f);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();   
    return 0;
}


void key_callback_gpuheight(GLFWwindow* window, int key, int scancode, int action, int modifiers)
{
    if(action == GLFW_PRESS)
    {
        switch(key)
        {
            case GLFW_KEY_SPACE:
                //useWireframe = 1 - useWireframe;
                break;
            case GLFW_KEY_G:
                //displayGrayscale = 1 - displayGrayscale;
                break;
            default:
                break;
        }
    }
}

void mouse_callback_gpuheight(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse_gpuheight)
    {
        lastX_gpuheight = xpos;
        lastY_gpuheight = ypos;
        firstMouse_gpuheight = false;
    }

    float xoffset = xpos - lastX_gpuheight;
    float yoffset = lastY_gpuheight - ypos; // reversed since y-coordinates go from bottom to top

    lastX_gpuheight = xpos;
    lastY_gpuheight = ypos;

    camera_gpuheight.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback_gpuheight(GLFWwindow* window, double xoffset, double yoffset)
{
    camera_gpuheight.ProcessMouseScroll(yoffset);
}

void processInput_gpuheight(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera_gpuheight.ProcessKeyboard(FORWARD, deltaTime_gpuheight);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera_gpuheight.ProcessKeyboard(BACKWARD, deltaTime_gpuheight);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera_gpuheight.ProcessKeyboard(LEFT, deltaTime_gpuheight);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera_gpuheight.ProcessKeyboard(RIGHT, deltaTime_gpuheight);
}