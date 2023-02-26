#include "shader.h"
#include "common.hpp"
#include "cubelight.h"
#include "model.h"
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

const int SCR_WIDTH = 1280;
const int SCR_HEIGHT = 960;

float lastTime_instance = 0.0f;
float deltaTime_instance = 0.0f;
float pitch_instance = 0.0f;
float yaw_instance = -90.0f;
float lastPosX_instance = SCR_WIDTH/2.0f;
float lastPosY_instance = SCR_HEIGHT/2.0f;
float firstMouse_instance = true;

glm::vec3 cameraPos_instance = glm::vec3(0.0f, 0.0f, 150.0f);
glm::vec3 cameraFront_instance = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraRight_instance = glm::vec3(1.0f, 0.0f, 0.0f);
glm::vec3 cameraUp_instance = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 cameraWorldUp_instance = glm::vec3(0.0f, 1.0f, 0.0f);

void mouse_callback_instance(GLFWwindow *window, double xPos, double yPos);
void processInput_instance(GLFWwindow *window);

int main_instance()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    #endif

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL Instance", NULL, NULL);
    if(window == NULL)
    {
        std::cout<<"window create failed"<<std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback_instance);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout<<"glad loader failed"<<std::endl;
        glfwTerminate();
        return -1;
    }

    const char *glsl_version = "#version 150";
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    glEnable(GL_DEPTH_TEST);

    glm::mat4 model(1.0f);
    glm::mat4 view = glm::lookAt(cameraPos_instance, cameraPos_instance+cameraFront_instance, cameraUp_instance);
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 1000.0f);
    Shader lightShader("/Users/cabin/Documents/cpp/learngl/shaders/color.vert", 
                      "/Users/cabin/Documents/cpp/learngl/shaders/color.frag");
    lightShader.use();
    lightShader.setMat4("model", model);
    lightShader.setMat4("view", view);
    lightShader.setMat4("projection", projection);

    CubeLight cubeLight(lightShader);

    Model planet("/Users/cabin/Documents/cpp/learngl/models/planet/planet.obj");
    Model rock("/Users/cabin/Documents/cpp/learngl/models/rock/rock.obj");

    Shader planetShader("/Users/cabin/Documents/cpp/learngl/shaders/mvp.vert", 
                      "/Users/cabin/Documents/cpp/learngl/shaders/mvp.frag");
    planetShader.use();
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
    model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
    planetShader.setMat4("model", model);
    planetShader.setMat4("view", view);
    planetShader.setMat4("projection", projection);

    Shader asteroidShader("/Users/cabin/Documents/cpp/learngl/shaders/asteroids.vert", 
                          "/Users/cabin/Documents/cpp/learngl/shaders/asteroids.frag");
    asteroidShader.use();
    asteroidShader.setMat4("view", view);
    asteroidShader.setMat4("projection", projection);

    unsigned int amount = 10000;
    glm::mat4 *modelMatrices;
    modelMatrices = new glm::mat4[amount];
    srand(static_cast<unsigned int>(glfwGetTime()));
    float radius = 150.0f;
    float offset = 25.0f;
    for(int i = 0; i < amount; i++)
    {
        glm::mat4 model = glm::mat4(1.0f);
        float angle = (float)i / (float)amount * 360.0f;
        float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float x = sin(angle) * radius + displacement;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float y = displacement * 0.4f;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float z = cos(angle) * radius + displacement;
        model = glm::translate(model, glm::vec3(x, y, z));

        float scale = static_cast<float>((rand() % 20) / 100.0f + 0.05);
        model = glm::scale(model, glm::vec3(scale*2));

        float rotAngle = static_cast<float>((rand() % 360));
        model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

        modelMatrices[i] = model;
    }
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::vec4), &modelMatrices[0], GL_STATIC_DRAW);

    for(int i = 0; i < rock.meshes.size(); i++)
    {
        unsigned int VAO = rock.meshes[i].VAO;
        glBindVertexArray(VAO);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), 0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2*sizeof(glm::vec4)));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3*sizeof(glm::vec4)));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);

        glBindVertexArray(0);
    }

    while (!glfwWindowShouldClose(window))
    {
        float currentTime = glfwGetTime();
        deltaTime_instance = currentTime - lastTime_instance;
        lastTime_instance = currentTime;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.3f, 0.2f, 0.2f, 1.0f);

        processInput_instance(window);
        view = glm::lookAt(cameraPos_instance, cameraPos_instance+cameraFront_instance, cameraUp_instance);

        float R = 150.0f;
        float angle = glm::radians(glfwGetTime()*20);
        glm::vec3 lightPos = glm::vec3(R * cos(angle), 0.0f, R * sin(angle));
        cubeLight.Draw(lightPos, glm::vec3(1.0f, 1.0f, 0.0f));
        planetShader.use();
        planetShader.setMat4("view", view);
        planet.Draw(planetShader);

        asteroidShader.use();
        asteroidShader.setMat4("view", view);
        asteroidShader.setInt("texture_diffuse", 0);
        asteroidShader.setVec3("lightPos", lightPos);
        asteroidShader.setVec3("viewPos", cameraPos_instance);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, rock.textures_loaded[0].id);
        for(int i = 0; i < rock.meshes.size(); i++)
        {
            glBindVertexArray(rock.meshes[i].VAO);
            glDrawElementsInstanced(GL_TRIANGLES, rock.meshes[i].indices.size(), GL_UNSIGNED_INT, 0, amount);
            glBindVertexArray(0);
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        {
            ImGui::Begin("opengl instance");
            float rate = 1000.0f / ImGui::GetIO().Framerate;
            ImGui::Text("average %.3f ms/frame (%.1f FPS)", rate, ImGui::GetIO().Framerate);
            ImGui::End();
        }
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();

    return 0;
}

void processInput_instance(GLFWwindow *window)
{
    float speed = 0.1f;
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        cameraPos_instance -= cameraRight_instance * speed;
    }
    else if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        cameraPos_instance += cameraRight_instance * speed;
    }
    else if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        cameraPos_instance += cameraFront_instance * speed;
    }
    else if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        cameraPos_instance -= cameraFront_instance * speed;
    }
}

void mouse_callback_instance(GLFWwindow *window, double xPos, double yPos)
{
    float deltaX = xPos - lastPosX_instance;
    float deltaY = yPos - lastPosY_instance;
    if(firstMouse_instance)
    {
        deltaX = 0.0f;
        deltaY = 0.0f;
        firstMouse_instance = false;
    }
    lastPosX_instance = xPos;
    lastPosY_instance = yPos;

    float sensitivity = 0.1f;
    yaw_instance += deltaX * sensitivity;

    pitch_instance += deltaY * sensitivity;
    if(pitch_instance > 89.0f)
    {
        pitch_instance = 89.0f;
    }
    else if(pitch_instance < -89.0f)
    {
        pitch_instance = -89.0f;
    }

    glm::vec3 front;
    front.x = cos(glm::radians(pitch_instance)) * cos(glm::radians(yaw_instance));
    front.y = sin(glm::radians(pitch_instance));
    front.z = cos(glm::radians(pitch_instance)) * sin(glm::radians(yaw_instance));
    cameraFront_instance = glm::normalize(front);
    cameraRight_instance = glm::normalize(glm::cross(cameraFront_instance, cameraWorldUp_instance));
    cameraUp_instance = glm::normalize(glm::cross(cameraRight_instance, cameraFront_instance));
}