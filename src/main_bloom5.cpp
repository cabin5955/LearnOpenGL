#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include "shader.h"
#include "camera.h"
#include "common.hpp"
#include "render_util.h"
#include "texutils.h"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

// camera value
glm::vec3 cameraPos_bloom5 = glm::vec3(0.0f, 0.0f, 6.0f);
glm::vec3 cameraFront_bloom5 = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp_bloom5 = glm::vec3(0.0f, 1.0f, 0.0f);

// delta time
float deltaTime_bloom5 = 0.0f;
float lastTime_bloom5= 0.0f;

float lastX_bloom5 = SCREEN_WIDTH / 2.0f; // 鼠标上一帧的位置
float lastY_bloom5 = SCREEN_HEIGHT / 2.0f;

Camera camera_bloom5(glm::vec3(0.0, 1.0, 18.0));

void mouse_callback_bloom5(GLFWwindow *window, double xpos, double ypos);
void processInput_bloom5(GLFWwindow *window);

using namespace std;

int main_bloom5(int argc, char *argv[])
{
    glfwInit();
  // 设置主要和次要版本
  const char *glsl_version = "#version 330";

  // 片段着色器将作用域每一个采样点（采用4倍抗锯齿，则每个像素有4个片段（四个采样点））
  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  
  #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  #endif

  // 窗口对象
  GLFWwindow *window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "LearnOpenGL", NULL, NULL);
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

  // 设置视口
  glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
  glEnable(GL_PROGRAM_POINT_SIZE);

  // 启用混合
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // 深度测试
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  // 鼠标键盘事件
  // 1.注册窗口变化监听
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  // 2.鼠标事件
  glfwSetCursorPosCallback(window, mouse_callback_bloom5);

  Shader sceneShader("/Users/cabin/Documents/cpp/learngl/shaders/start/bloom_scene_vert.glsl",
                     "/Users/cabin/Documents/cpp/learngl/shaders/start/bloom_scene_frag.glsl");

  Shader lightShader("/Users/cabin/Documents/cpp/learngl/shaders/start/light_object_vert.glsl", 
                     "/Users/cabin/Documents/cpp/learngl/shaders/start/light_object_frag.glsl");

  Shader blurShader("/Users/cabin/Documents/cpp/learngl/shaders/start/blur_scene_vert.glsl", 
                    "/Users/cabin/Documents/cpp/learngl/shaders/start/blur_scene_frag.glsl");

  Shader finalShader("/Users/cabin/Documents/cpp/learngl/shaders/start/bloom_final_vert.glsl", 
                     "/Users/cabin/Documents/cpp/learngl/shaders/start/bloom_final_frag.glsl");

  unsigned int woodMap = loadTexture("/Users/cabin/Documents/cpp/learngl/textures/wood.png");                         // 地面
  unsigned int brickMap = loadTexture("/Users/cabin/Documents/cpp/learngl/textures/container2.png");               // 砖块
  unsigned int grassMap = loadTexture("/Users/cabin/Documents/cpp/learngl/textures/blending_transparent_window.png"); // 草丛

  float factor = 0.0;

  unsigned int hdrFBO;
  glGenFramebuffers(1, &hdrFBO);
  glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

  // 创建两个颜色缓冲区，一个用于渲染，一个用于亮度提取
  // **********************
  unsigned int colorBuffers[2];
  glGenTextures(2, colorBuffers);
  for (unsigned int i = 0; i < 2; i++)
  {
    glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // 避免模糊过滤器重复采样
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
  }
  unsigned int rboDepth;
  glGenRenderbuffers(1, &rboDepth);
  glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCREEN_WIDTH, SCREEN_HEIGHT);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
  // 使用帧缓冲区的颜色附件
  unsigned int attachments[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
  glDrawBuffers(2, attachments);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "Framebuffer not complete!" << std::endl;
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // 创建用于模糊的帧缓冲区
  // **********************
  unsigned int pingpongFBO[2];
  unsigned int pingpongColorbuffers[2];
  glGenFramebuffers(2, pingpongFBO);
  glGenTextures(2, pingpongColorbuffers);
  for (unsigned int i = 0; i < 2; i++)
  {
    glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
    glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCREEN_WIDTH*2, SCREEN_HEIGHT*2, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      std::cout << "Framebuffer not complete!" << std::endl;
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // 点光源的位置
  glm::vec3 pointLightPositions[] = {
      glm::vec3(0.7f, 1.0f, 1.5f),
      glm::vec3(2.3f, 3.0f, -4.0f),
      glm::vec3(-4.0f, 2.0f, 1.0f),
      glm::vec3(1.4f, 2.0f, 1.3f)};

  // 点光源颜色
  glm::vec3 pointLightColors[] = {
      glm::vec3(2.0f, 1.0f, 1.0f),
      glm::vec3(4.0f, 0.0f, 4.0f),
      glm::vec3(1.0f, 1.0f, 4.0f),
      glm::vec3(0.0f, 2.0f, 0.0f)};

glm::vec3 lightPosition = glm::vec3(1.0, 2.5, 2.0); // 光照位置

  blurShader.use();
  blurShader.setInt("image", 0);

  finalShader.use();
  finalShader.setInt("scene", 0);
  finalShader.setInt("bloomBlur", 1);

  // 设置平行光光照属性
  sceneShader.use();
  sceneShader.setInt("diffuseTexture", 0);

  sceneShader.setVec3("directionLight.ambient", glm::vec3(0.01f, 0.01f, 0.01f));
  sceneShader.setVec3("directionLight.diffuse", glm::vec3(0.05f, 0.05f, 0.05f)); // 将光照调暗了一些以搭配场景
  sceneShader.setVec3("directionLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));

  // 设置衰减
  sceneShader.setFloat("light.constant", 1.0f);
  sceneShader.setFloat("light.linear", 0.09f);
  sceneShader.setFloat("light.quadratic", 0.032f);

  while (!glfwWindowShouldClose(window))
  {
    processInput_bloom5(window);

    float currentFrame = glfwGetTime();
    deltaTime_bloom5 = currentFrame - lastTime_bloom5;
    lastTime_bloom5 = currentFrame;

    // 渲染指令
    glClearColor(25.0 / 255.0, 25.0 / 255.0, 25.0 / 255.0, 1.0);

    // 1.将场景渲染至帧缓冲区
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    sceneShader.use();

    // 修改光源颜色
    glm::vec3 lightColor;
    lightColor.x = sin(glfwGetTime() * 2.0f);
    lightColor.y = sin(glfwGetTime() * 0.7f);
    lightColor.z = sin(glfwGetTime() * 1.3f);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, woodMap);

    float radius = 5.0f;
    float camX = sin(glfwGetTime() * 0.5) * radius;
    float camZ = cos(glfwGetTime() * 0.5) * radius;

    glm::mat4 view = camera_bloom5.GetViewMatrix();
    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(45.0f), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);

    glm::vec3 lightPos = glm::vec3(lightPosition.x * glm::sin(glfwGetTime()) * 2.0, lightPosition.y, lightPosition.z);
    sceneShader.use();
    sceneShader.setMat4("view", view);
    sceneShader.setMat4("projection", projection);

    sceneShader.setVec3("directionLight.direction", lightPos); // 光源位置
    sceneShader.setVec3("viewPos", camera_bloom5.Position);

    pointLightPositions[0].z = camZ;
    pointLightPositions[0].x = camX;

    for (unsigned int i = 0; i < 4; i++)
    {
      // 设置点光源属性
      sceneShader.setVec3("pointLights[" + std::to_string(i) + "].position", pointLightPositions[i]);
      sceneShader.setVec3("pointLights[" + std::to_string(i) + "].ambient", glm::vec3(0.01f, 0.01f, 0.01f));
      sceneShader.setVec3("pointLights[" + std::to_string(i) + "].diffuse", pointLightColors[i]);
      sceneShader.setVec3("pointLights[" + std::to_string(i) + "].specular", glm::vec3(0.1f, 0.1f, 0.1f));

      // // 设置衰减
      sceneShader.setFloat("pointLights[" + std::to_string(i) + "].constant", 1.0f);
      sceneShader.setFloat("pointLights[" + std::to_string(i) + "].linear", 0.09f);
      sceneShader.setFloat("pointLights[" + std::to_string(i) + "].quadratic", 0.032f);
    }

    sceneShader.setFloat("uvScale", 1.0f);
    glBindTexture(GL_TEXTURE_2D, woodMap);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0));
    model = glm::scale(model, glm::vec3(12.5f, 0.5f, 12.5f));
    sceneShader.setMat4("model", model);
    renderCube();

        // then create multiple cubes as the scenery
        glBindTexture(GL_TEXTURE_2D, brickMap);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
        model = glm::scale(model, glm::vec3(0.5f));
        sceneShader.setMat4("model", model);
        renderCube();

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
        model = glm::scale(model, glm::vec3(0.5f));
        sceneShader.setMat4("model", model);
        renderCube();

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.0f, -1.0f, 2.0));
        model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
        sceneShader.setMat4("model", model);
        renderCube();

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 2.7f, 4.0));
        model = glm::rotate(model, glm::radians(23.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
        model = glm::scale(model, glm::vec3(1.25));
        sceneShader.setMat4("model", model);
        renderCube();

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-2.0f, 1.0f, -3.0));
        model = glm::rotate(model, glm::radians(124.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
        sceneShader.setMat4("model", model);
        renderCube();

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-3.0f, 0.0f, 0.0));
        model = glm::scale(model, glm::vec3(0.5f));
        sceneShader.setMat4("model", model);
        renderCube();

    // 绘制灯光物体
    // ************************************************************
    lightShader.use();
    lightShader.setMat4("view", view);
    lightShader.setMat4("projection", projection);

    model = glm::mat4(1.0f);
    model = glm::translate(model, lightPos);

    lightShader.setMat4("model", model);
    lightShader.setVec3("lightColor", glm::vec3(2.0f, 2.0f, 2.0f));

    for (unsigned int i = 0; i < 4; i++)
    {
        model = glm::mat4(1.0f);
        model = glm::translate(model, pointLightPositions[i]);
        model = glm::scale(model, glm::vec3(0.1f));
        lightShader.setMat4("model", model);
        lightShader.setVec3("lightColor", pointLightColors[i]);
        renderCube();
    }
    // ************************************************************
    // 2.高斯模糊明亮的片段
    bool horizontal = true, first_iteration = true;
    unsigned int amount = 10;
    blurShader.use();
    for (unsigned int i = 0; i < amount; i++)
    {
      glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
      blurShader.setInt("horizontal", horizontal);
      glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : pingpongColorbuffers[!horizontal]);
      renderQuad();
      horizontal = !horizontal;
      if (first_iteration)
      {
        first_iteration = false;
      }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    // 3.绘制hdr输出的texture
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    finalShader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D,  pingpongColorbuffers[!horizontal]);
    finalShader.setInt("bloom", true);
    finalShader.setFloat("exposure", 1.0);
    renderQuad();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();

  return 0;
}

// 键盘输入监听
void processInput_bloom5(GLFWwindow *window)
{
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
  {
    glfwSetWindowShouldClose(window, true);
  }

  // 相机按键控制
  // 相机移动
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
  {
    camera_bloom5.ProcessKeyboard(FORWARD, deltaTime_bloom5);
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
  {
    camera_bloom5.ProcessKeyboard(BACKWARD, deltaTime_bloom5);
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
  {
    camera_bloom5.ProcessKeyboard(LEFT, deltaTime_bloom5);
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
  {
    camera_bloom5.ProcessKeyboard(RIGHT, deltaTime_bloom5);
  }
}

// 鼠标移动监听
void mouse_callback_bloom5(GLFWwindow *window, double xpos, double ypos)
{

  float xoffset = xpos - lastX_bloom5;
  float yoffset = lastY_bloom5 - ypos;

  lastX_bloom5 = xpos;
  lastY_bloom5 = ypos;

  camera_bloom5.ProcessMouseMovement(xoffset, yoffset);
}
