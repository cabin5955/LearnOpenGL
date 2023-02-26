#include "shader.h"
#include "common.hpp"
#include "texutils.h"
#include "cubelight.h"
#include "floor.h"
#include "stb_image.h"
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

const int SCR_WIDTH = 1280;
const int SCR_HEIGHT = 960;

void processInput_cubemap(GLFWwindow *window);
void mouse_callback_cubemap(GLFWwindow *window, double xPos, double yPos);

float lastTime_cubemap = 0.0f;
float deltaTime_cubemap = 0.0f;
glm::vec3 cameraPos_cubemap = glm::vec3(0.0f, 0.0f, 7.0f);
glm::vec3 cameraFront_cubemap = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraRight_cubemap = glm::vec3(1.0f, 0.0f, 0.0f);
glm::vec3 cameraWorldUp_cubemap = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 cameraUp_cubemap = glm::vec3(0.0f, 1.0f, 0.0f);
float pitch_cubemap = 0.0f;
float yaw_cubemap = -90.0f;
float lastPosX_cubemap = SCR_WIDTH/2.0f;
float lastPosY_cubemap = SCR_HEIGHT/2.0f;
bool firstMouse_cubemap = true;

int main_cubemap()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_REFRESH_RATE, 60);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    #endif

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "cubemap window", NULL, NULL);
    if(window == NULL)
    {
        std::cout<<"create window failed"<<std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback_cubemap);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout<<"load glad failed"<<std::endl;
        glfwTerminate();
        return -1;
    }

    const char* glsl_version = "#version 150";
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    float vertices[] = {
        //back
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
         0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
         0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,
         0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        //front
        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
         0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
         0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
         0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        //left
        -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, -1.0f,0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, -1.0f,0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, -1.0f,0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, -1.0f,0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, -1.0f,0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, -1.0f,0.0f, 0.0f,
        //right
         0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 1.0f,0.0f, 0.0f,
         0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f,0.0f, 0.0f,
         0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f,0.0f, 0.0f,
         0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f,0.0f, 0.0f,
         0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f,0.0f, 0.0f,
         0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 1.0f,0.0f, 0.0f,
        //bottom
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,-1.0f, 0.0f,
         0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f,-1.0f, 0.0f,
         0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f,-1.0f, 0.0f,
         0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f,-1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 0.0f,-1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,-1.0f, 0.0f,
        //top
        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f,1.0f, 0.0f,
         0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 0.0f,1.0f, 0.0f,
         0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f,1.0f, 0.0f,
         0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f,1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 0.0f,1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f,1.0f, 0.0f,
    };

    unsigned int VAO,VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5*sizeof(float)));

    unsigned int texture_container, texture_floor;
    texture_container = loadTexture("/Users/cabin/Documents/cpp/learngl/textures/container2.png");
    texture_floor = loadTexture("/Users/cabin/Documents/cpp/learngl/textures/wood.png");

    Shader shader("/Users/cabin/Documents/cpp/learngl/shaders/cubemap.vert",
                  "/Users/cabin/Documents/cpp/learngl/shaders/cubemap.frag");
    shader.use();
    shader.setMat4("model", glm::mat4(1.0f));
    glm::mat4 view = glm::lookAt(cameraPos_cubemap, cameraPos_cubemap + cameraFront_cubemap, glm::vec3(0.0f,1.0f,0.0f));
    shader.setMat4("view",view);
    glm::mat4 projection = glm::perspective(glm::radians(45.0f),(float)SCR_WIDTH/(float)SCR_HEIGHT,0.1f,100.0f);
    shader.setMat4("projection",projection);
    float rotateAngle = 0.0f;

    Shader outlineShader("/Users/cabin/Documents/cpp/learngl/shaders/outline.vert",
                         "/Users/cabin/Documents/cpp/learngl/shaders/outline.frag");
    outlineShader.use();
    outlineShader.setMat4("view", view);
    outlineShader.setMat4("projection", projection);

    Shader lightShader("/Users/cabin/Documents/cpp/learngl/shaders/color.vert",
                       "/Users/cabin/Documents/cpp/learngl/shaders/color.frag");
    lightShader.use();
    lightShader.setMat4("view", view);
    lightShader.setMat4("projection", projection);
    CubeLight cubeLight(lightShader);
    Floor floor(shader, glm::vec3(0.0f, -1.5f, 0.0f), 10, 10, texture_floor);
    
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    unsigned int framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    unsigned int texColorBuffer;
    glGenTextures(1, &texColorBuffer);
    glBindTexture(GL_TEXTURE_2D, texColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH*2, SCR_HEIGHT*2, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH*2, SCR_HEIGHT*2);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout<<"error:framebuffer is not complete "<<std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    float quadVertices[] = {
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
         1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
         1.0f,-1.0f, 0.0f, 1.0f, 0.0f,
         1.0f,-1.0f, 0.0f, 1.0f, 0.0f,
        -1.0f,-1.0f, 0.0f, 0.0f, 0.0f,
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f 
    };
    unsigned quadVAO,quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    Shader screenShader("/Users/cabin/Documents/cpp/learngl/shaders/screen.vert",
                        "/Users/cabin/Documents/cpp/learngl/shaders/screen.frag");

    unsigned int texture_skybox;
    glGenTextures(1, &texture_skybox);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_skybox);

    int width, height, nChannel;
    unsigned char *data;
    data =stbi_load("/Users/cabin/Documents/cpp/learngl/textures/skybox/right.jpg", &width, &height, &nChannel, 0);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    
    data = stbi_load("/Users/cabin/Documents/cpp/learngl/textures/skybox/left.jpg", &width, &height, &nChannel, 0);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    data = stbi_load("/Users/cabin/Documents/cpp/learngl/textures/skybox/top.jpg", &width, &height, &nChannel, 0);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    data = stbi_load("/Users/cabin/Documents/cpp/learngl/textures/skybox/bottom.jpg", &width, &height, &nChannel, 0);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    data = stbi_load("/Users/cabin/Documents/cpp/learngl/textures/skybox/front.jpg", &width, &height, &nChannel, 0);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    data = stbi_load("/Users/cabin/Documents/cpp/learngl/textures/skybox/back.jpg", &width, &height, &nChannel, 0);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
    unsigned int skyVAO, skyVBO;
    glGenVertexArrays(1, &skyVAO);
    glGenBuffers(1, &skyVBO);

    glBindVertexArray(skyVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyVBO);
    glEnableVertexAttribArray(0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

    Shader skyboxShader("/Users/cabin/Documents/cpp/learngl/shaders/skybox.vert",
                        "/Users/cabin/Documents/cpp/learngl/shaders/skybox.frag");
    skyboxShader.use();
    skyboxShader.setMat4("view", view);
    skyboxShader.setMat4("projection", projection);

    while(!glfwWindowShouldClose(window))
    {
        float currentTime = glfwGetTime();
        deltaTime_cubemap = currentTime - lastTime_cubemap;
        lastTime_cubemap = currentTime;
        rotateAngle += deltaTime_cubemap * 10;

        processInput_cubemap(window);

        view = glm::lookAt(cameraPos_cubemap, cameraPos_cubemap + cameraFront_cubemap, cameraUp_cubemap);

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glEnable(GL_DEPTH_TEST);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glClearColor(0.3f,0.2f,0.2f,1.0f);
        
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);

        glDepthMask(GL_FALSE);
        glm::mat4 skyView = glm::mat4(glm::mat3(view));
        skyboxShader.use();
        skyboxShader.setMat4("view", skyView);
        glBindVertexArray(skyVAO);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture_skybox);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthMask(GL_TRUE);

        float R = 2.0f;
        float angle = glm::radians(glfwGetTime()*20);
        glm::vec3 lightPos = glm::vec3(R * cos(angle), 0.0f, R * sin(angle));
        shader.use();
        shader.setMat4("view", view);
        shader.setVec3("lightPos", lightPos);

        glm::mat4 model2(1.0);
        model2 = glm::translate(model2, glm::vec3(2.0f, 1.0f, 0.0f));
        model2 = glm::rotate(model2, glm::radians(60.0f), glm::vec3(1.0f,1.0f,0.0f));
        shader.setMat4("model", model2);
        glBindTexture(GL_TEXTURE_2D, texture_container);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0 , 36);

        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);
        glDisable(GL_DEPTH_TEST);
        outlineShader.use();
        outlineShader.setMat4("view", view);
        outlineShader.setVec3("color",glm::vec3(1.0));
        outlineShader.setFloat("scale", 0.05f);
        outlineShader.setMat4("model", model2);
        glBindTexture(GL_TEXTURE_2D, texture_container);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0 , 36);

        glStencilMask(0xFF);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glEnable(GL_DEPTH_TEST);

        lightShader.use();
        lightShader.setMat4("view", view);
        cubeLight.Draw(lightPos, glm::vec3(1.0f,1.0f,0.0f));
        floor.Draw();

        glm::mat4 model(1.0f);
        model = glm::rotate(model, glm::radians(rotateAngle), glm::vec3(0.0f,1.0f,0.0f));
        shader.setMat4("model", model);
        glActiveTexture(0);
        glBindTexture(GL_TEXTURE_2D, texture_container);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0 , 36);

        glm::mat4 model3(1.0);
        model3 = glm::translate(model3, glm::vec3(-2.0f, -1.5f, -1.0f));
        model3 = glm::rotate(model3, glm::radians(124.0f), glm::vec3(1.0f,1.0f,0.0f));
        shader.setMat4("model", model3);
        glBindTexture(GL_TEXTURE_2D, texture_container);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0 , 36);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.3f,0.2f,0.2f,1.0f);

        screenShader.use();
        glBindVertexArray(quadVAO);
        glDisable(GL_DEPTH_TEST);
        glBindTexture(GL_TEXTURE_2D, texColorBuffer);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        {
            ImGui::Begin("cubemap panel");
            ImGui::Text("rotate speed %.2f", rotateAngle);
            ImGui::Text("delta time %.4f", deltaTime_cubemap);
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

void processInput_cubemap(GLFWwindow *window)
{
    float cameraMoveSpeed = 10.0f * deltaTime_cubemap;
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        cameraPos_cubemap -= cameraRight_cubemap * cameraMoveSpeed;
    }
    else if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        cameraPos_cubemap += cameraRight_cubemap * cameraMoveSpeed;
    }
    else if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        cameraPos_cubemap += cameraFront_cubemap * cameraMoveSpeed;
    }
    else if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        cameraPos_cubemap -= cameraFront_cubemap * cameraMoveSpeed;
    }
}

void mouse_callback_cubemap(GLFWwindow *window, double xPos, double yPos)
{
    float deltaX = xPos - lastPosX_cubemap;
    float deltaY = yPos - lastPosY_cubemap;
    if(firstMouse_cubemap)
    {
        deltaX = 0.0f;
        deltaY = 0.0f;
        firstMouse_cubemap = false;
    }
    lastPosX_cubemap = xPos;
    lastPosY_cubemap = yPos;

    float sensitivity = 0.1f;
    pitch_cubemap += deltaY * sensitivity;
    
    if(pitch_cubemap > 89.0f)
    {
        pitch_cubemap = 89.0f;
    }
    if(pitch_cubemap < -89.0f)
    {
        pitch_cubemap = -89.0f;
    }

    yaw_cubemap += deltaX * sensitivity;

    glm::vec3 front;
    front.x = cos(glm::radians(pitch_cubemap)) * cos(glm::radians(yaw_cubemap));
    front.y = sin(glm::radians(pitch_cubemap));
    front.z = cos(glm::radians(pitch_cubemap)) * sin(glm::radians(yaw_cubemap));

    cameraFront_cubemap = glm::normalize(front);
    cameraRight_cubemap = glm::normalize(glm::cross(cameraFront_cubemap, cameraWorldUp_cubemap));
    cameraUp_cubemap = glm::normalize(glm::cross(cameraRight_cubemap, cameraFront_cubemap));
}