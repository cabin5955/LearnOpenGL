#include "common.hpp"
#include "shader.h"
#include "camera.h"
#include "texutils.h"
#include "render_util.h"
#include "model.h"
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

const int SCR_WIDTH = 800;
const int SCR_HEIGHT = 600;

// camera
Camera camera_deferred(glm::vec3(0.0f, 0.0f, 5.0f));
float lastX_deferred = (float)SCR_WIDTH / 2.0;
float lastY_deferred = (float)SCR_HEIGHT / 2.0;
bool firstMouse_deferred = true;

// timing
float deltaTime_deferred = 0.0f;
float lastTime_deferred = 0.0f;

void mouse_callback_deferred(GLFWwindow *window, double xpos, double ypos);
void processInput_deferred(GLFWwindow *window);

int main_deferred()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    #endif

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "opengl deferred shading", NULL, NULL);
    if(window == NULL)
    {
        std::cout<<"create window failed"<<std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback_deferred);
    
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout<<"glad load failed"<<std::endl;
        glfwTerminate();
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    Shader shaderGeometryPass("/Users/cabin/Documents/cpp/learngl/shaders/gbuffer.vert", 
                              "/Users/cabin/Documents/cpp/learngl/shaders/gbuffer.frag");

    Shader shaderLightingPass("/Users/cabin/Documents/cpp/learngl/shaders/deferredshading.vert", 
                              "/Users/cabin/Documents/cpp/learngl/shaders/deferredshading.frag");

    Shader shaderLightBox("/Users/cabin/Documents/cpp/learngl/shaders/deferredlightbox.vert", 
                          "/Users/cabin/Documents/cpp/learngl/shaders/deferredlightbox.frag");

    unsigned int texture_diffuse = loadTexture("/Users/cabin/Documents/cpp/learngl/textures/container2.png");
    unsigned int texture_specular = loadTexture("/Users/cabin/Documents/cpp/learngl/textures/container2_specular.png");

    // load models
    // -----------
    Model backpack("/Users/cabin/Documents/cpp/learngl/models/cyborg/cyborg.obj");
    std::vector<glm::vec3> objectPositions;
    objectPositions.push_back(glm::vec3(-3.0,  -0.5, -3.0));
    objectPositions.push_back(glm::vec3( 0.0,  -0.5, -3.0));
    objectPositions.push_back(glm::vec3( 3.0,  -0.5, -3.0));
    objectPositions.push_back(glm::vec3(-3.0,  -0.5,  0.0));
    objectPositions.push_back(glm::vec3( 0.0,  -0.5,  0.0));
    objectPositions.push_back(glm::vec3( 3.0,  -0.5,  0.0));
    objectPositions.push_back(glm::vec3(-3.0,  -0.5,  3.0));
    objectPositions.push_back(glm::vec3( 0.0,  -0.5,  3.0));
    objectPositions.push_back(glm::vec3( 3.0,  -0.5,  3.0));

    unsigned int gBuffer;
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    unsigned int gPosition, gNormal, gAlbedoSpec;

    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH*2, SCR_HEIGHT*2, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D, gPosition, 0);

    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH*2, SCR_HEIGHT*2, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,GL_TEXTURE_2D, gNormal, 0);

    glGenTextures(1, &gAlbedoSpec);
    glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH*2, SCR_HEIGHT*2, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

    unsigned int attachments[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
    glDrawBuffers(3, attachments);

    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH*2, SCR_HEIGHT*2);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout<<"framebuffer not complete!" <<std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // lighting info
    // -------------
    const unsigned int NR_LIGHTS = 32;
    std::vector<glm::vec3> lightPositions;
    std::vector<glm::vec3> lightColors;
    srand(13);
    for (unsigned int i = 0; i < NR_LIGHTS; i++)
    {
        // calculate slightly random offsets
        float xPos = static_cast<float>(((rand() % 100) / 100.0) * 6.0 - 3.0);
        float yPos = static_cast<float>(((rand() % 100) / 100.0) * 6.0 - 4.0);
        float zPos = static_cast<float>(((rand() % 100) / 100.0) * 6.0 - 3.0);
        lightPositions.push_back(glm::vec3(xPos, yPos, zPos));
        // also calculate random color
        float rColor = static_cast<float>(((rand() % 100) / 200.0f) + 0.5); // between 0.5 and 1.0
        float gColor = static_cast<float>(((rand() % 100) / 200.0f) + 0.5); // between 0.5 and 1.0
        float bColor = static_cast<float>(((rand() % 100) / 200.0f) + 0.5); // between 0.5 and 1.0
        lightColors.push_back(glm::vec3(rColor, gColor, bColor));
    }

    shaderLightingPass.use();
    shaderLightingPass.setInt("gPosition", 0);
    shaderLightingPass.setInt("gNormal", 1);
    shaderLightingPass.setInt("gAlbedoSpec", 2);

    shaderGeometryPass.use();
    shaderGeometryPass.setInt("texture_diffuse1", 0);
    shaderGeometryPass.setInt("texture_specular1", 1);

    while (!glfwWindowShouldClose(window))
    {
        processInput_deferred(window);
        float currentFrame = glfwGetTime();
        deltaTime_deferred = currentFrame - lastTime_deferred;
        lastTime_deferred = currentFrame;

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::mat4 projection = glm::perspective(glm::radians(camera_deferred.Zoom), (float)SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera_deferred.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);
        shaderGeometryPass.use();
        shaderGeometryPass.setMat4("projection", projection);
        shaderGeometryPass.setMat4("view", view);
        for (unsigned int i = 0; i < objectPositions.size(); i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, objectPositions[i]);
            model = glm::scale(model, glm::vec3(0.5f));
            shaderGeometryPass.setMat4("model", model);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture_diffuse);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, texture_specular);
            renderCube();
            //backpack.Draw(shaderGeometryPass);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shaderLightingPass.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);

        // send light relevant uniforms
        for (unsigned int i = 0; i < lightPositions.size(); i++)
        {
            shaderLightingPass.setVec3("lights[" + std::to_string(i) + "].Position", lightPositions[i]);
            shaderLightingPass.setVec3("lights[" + std::to_string(i) + "].Color", lightColors[i]);
            // update attenuation parameters and calculate radius
            const float linear = 0.7f;
            const float quadratic = 1.8f;
            shaderLightingPass.setFloat("lights[" + std::to_string(i) + "].Linear", linear);
            shaderLightingPass.setFloat("lights[" + std::to_string(i) + "].Quadratic", quadratic);
        }
        shaderLightingPass.setVec3("viewPos", camera_deferred.Position);
        // finally render quad
        renderQuad();

        glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0, 0, SCR_WIDTH*2, SCR_HEIGHT*2, 0, 0, SCR_WIDTH*2, SCR_HEIGHT*2, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        shaderLightBox.use();
        shaderLightBox.setMat4("projection", projection);
        shaderLightBox.setMat4("view", view);
        for (unsigned int i = 0; i < lightPositions.size(); i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, lightPositions[i]);
            model = glm::scale(model, glm::vec3(0.125f));
            shaderLightBox.setMat4("model", model);
            shaderLightBox.setVec3("lightColor", lightColors[i]);
            renderCube();
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    return 0;
}

// 键盘输入监听
void processInput_deferred(GLFWwindow *window)
{
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
  {
    glfwSetWindowShouldClose(window, true);
  }

  // 相机按键控制
  // 相机移动
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
  {
    camera_deferred.ProcessKeyboard(FORWARD, deltaTime_deferred);
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
  {
    camera_deferred.ProcessKeyboard(BACKWARD, deltaTime_deferred);
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
  {
    camera_deferred.ProcessKeyboard(LEFT, deltaTime_deferred);
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
  {
    camera_deferred.ProcessKeyboard(RIGHT, deltaTime_deferred);
  }
}

// 鼠标移动监听
void mouse_callback_deferred(GLFWwindow *window, double xpos, double ypos)
{

  float xoffset = xpos - lastX_deferred;
  float yoffset = lastY_deferred - ypos;

  lastX_deferred = xpos;
  lastY_deferred = ypos;

  camera_deferred.ProcessMouseMovement(xoffset, yoffset);
}