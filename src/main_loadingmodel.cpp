#include "common.hpp"
#include "shader.h"
#include "texutils.h"
#include "model.h"
#include "camera.h"
#include "stb_image.h"
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

const int SCR_WIDTH = 1280;
const int SCR_HEIGHT =960;

glm::vec3 cameraPos_model = glm::vec3(0.0f, 1.0f, 3.0f);
glm::vec3 cameraFront_model = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp_model = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 cameraRight_model = glm::vec3(1.0f, 0.0f, 0.0f);
glm::vec3 worldUp_model = glm::vec3(0.0f, 1.0f, 0.0f);

float pitch_model = 0.0f;
float yaw_model = -90.0f;
float lastPosx_model = SCR_WIDTH/2.0f;
float lastPosy_model = SCR_HEIGHT/2.0F;
bool firstMouse_model = true;
float deltaTime_model = 0.0f;
float lastFrameTime_model= 0.0f;

void mouseCallback_model(GLFWwindow *window, double xPos, double yPos);
void processInput_model(GLFWwindow *window);
void renderCube(Shader &shader, glm::vec3 position, unsigned int texture,
                glm::vec3 scale=glm::vec3(1.0f,1.0f,1.0f),
                float angle=0.0f, glm::vec3 rotateAxis=glm::vec3(0.0f,1.0f,0.0f));
void renderGround(Shader &shader, unsigned int nWidth, unsigned int nHeight);

int main_loadingmodel()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    #endif
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "model loading", NULL, NULL);
    if(window == NULL)
    {
        std::cout<<"create window failed"<<std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouseCallback_model);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout<<"glad load failed"<<std::endl;
        glfwTerminate();
        return -1;
    }

    // Setup Dear ImGui context
    const char* glsl_version = "#version 150";
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    ImVec4 light_color = ImVec4(1.00f, 1.00f, 0.60f, 1.00f);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    
    float lightVertices[] = {
        //back
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f, 
         0.5f,  0.5f, -0.5f, 
         0.5f,  0.5f, -0.5f, 
        -0.5f,  0.5f, -0.5f, 
        -0.5f, -0.5f, -0.5f, 
        //front
        -0.5f, -0.5f,  0.5f, 
         0.5f, -0.5f,  0.5f, 
         0.5f,  0.5f,  0.5f, 
         0.5f,  0.5f,  0.5f, 
        -0.5f,  0.5f,  0.5f, 
        -0.5f, -0.5f,  0.5f, 
        //left
        -0.5f,  0.5f,  0.5f, 
        -0.5f,  0.5f, -0.5f, 
        -0.5f, -0.5f, -0.5f, 
        -0.5f, -0.5f, -0.5f, 
        -0.5f, -0.5f,  0.5f, 
        -0.5f,  0.5f,  0.5f, 
        //right
         0.5f,  0.5f,  0.5f, 
         0.5f,  0.5f, -0.5f, 
         0.5f, -0.5f, -0.5f, 
         0.5f, -0.5f, -0.5f, 
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f, 
        //bottom
        -0.5f, -0.5f, -0.5f, 
         0.5f, -0.5f, -0.5f, 
         0.5f, -0.5f,  0.5f, 
         0.5f, -0.5f,  0.5f, 
        -0.5f, -0.5f,  0.5f, 
        -0.5f, -0.5f, -0.5f, 
        //top
        -0.5f,  0.5f, -0.5f, 
         0.5f,  0.5f, -0.5f, 
         0.5f,  0.5f,  0.5f, 
         0.5f,  0.5f,  0.5f, 
        -0.5f,  0.5f,  0.5f, 
        -0.5f,  0.5f, -0.5f
    };

    unsigned int lightVAO,lightVBO;
    glGenVertexArrays(1, &lightVAO);
    glGenBuffers(1, &lightVBO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lightVertices), lightVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    unsigned int texture_container2;
    glGenTextures(1, &texture_container2);  
    glBindTexture(GL_TEXTURE_2D,texture_container2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    unsigned char *data;
    int width,height,nChannel;
    stbi_set_flip_vertically_on_load(1);
    data = stbi_load("/Users/cabin/Documents/cpp/learngl/textures/container2.png", &width, &height, &nChannel, 0);
    if(data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout<<"texture load failed"<<std::endl;
    }

    unsigned int texture_wall = loadTexture("/Users/cabin/Documents/cpp/learngl/textures/wall.jpg");
    unsigned int texture_container = loadTexture("/Users/cabin/Documents/cpp/learngl/textures/container.jpg");

    Shader shader("/Users/cabin/Documents/cpp/learngl/shaders/model.vert",
                  "/Users/cabin/Documents/cpp/learngl/shaders/model.frag");
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 100.0f);
    shader.use();
    shader.setMat4("projection", projection);

    Shader lightShader("/Users/cabin/Documents/cpp/learngl/shaders/color.vert",
                       "/Users/cabin/Documents/cpp/learngl/shaders/color.frag");
    lightShader.use();
    lightShader.setVec3("color", glm::vec3(1.0f,1.0f,1.0f));
    lightShader.setMat4("projection", projection);

    Shader outlineShader("/Users/cabin/Documents/cpp/learngl/shaders/outline.vert",
                       "/Users/cabin/Documents/cpp/learngl/shaders/outline.frag");
    outlineShader.use();
    outlineShader.setMat4("projection", projection);

    float lightSpeed = 0.5f;
    float lightAngle = 0.0f;
    float ambientFactor = 0.2f;
    Model myModel("/Users/cabin/Documents/cpp/learngl/models/cyborg/cyborg.obj");

    while(!glfwWindowShouldClose(window))
    {
        float currentTime = glfwGetTime();
        deltaTime_model = currentTime - lastFrameTime_model;
        lastFrameTime_model = currentTime;

        processInput_model(window);
        glClearColor(0.3f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        
        glStencilMask(0x00);
        glm::mat4 view = glm::lookAt(cameraPos_model, cameraPos_model+cameraFront_model, cameraUp_model); 
        glm::mat4 model(1.0f);
        float lightR = 2.0f;
        lightAngle += lightSpeed;
        double angle = glm::radians(lightAngle);
        float px = cos(angle) * lightR;
        float pz = sin(angle) * lightR;
        glm::vec3 lightPos = glm::vec3(px, 1.0f, pz);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f));
        glm::vec3 color = glm::vec3(light_color.x, light_color.y, light_color.z);
        lightShader.use();
        lightShader.setMat4("model", model);
        lightShader.setMat4("view", view);
        lightShader.setVec3("color", color);
        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    
        shader.use();
        shader.setVec3("light.ambient",  glm::vec3(ambientFactor));
        shader.setVec3("light.diffuse",  glm::vec3(0.6f)*color);
        shader.setVec3("light.specular", glm::vec3(0.8f)*color);
        shader.setMat4("view", view);
        shader.setVec3("viewPos", cameraPos_model);
        shader.setVec3("light.position", lightPos);
        shader.setFloat("material.shininess",64.0f);
        renderCube(shader,glm::vec3(0.0f, 1.5f, -4.0),texture_container,glm::vec3(0.5f));
        renderCube(shader,glm::vec3(2.0f, 0.0f, 1.0),texture_container2, glm::vec3(0.5f));
        renderCube(shader,glm::vec3(-1.0f, -1.0f, 2.0),texture_container2,glm::vec3(1.0f), 60.0f, glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
        renderCube(shader,glm::vec3(0.0f, 2.7f, 4.0),texture_container2,glm::vec3(1.25),23.0f,glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
        renderCube(shader,glm::vec3(-2.0f, 1.0f, -3.0), texture_container2, glm::vec3(1.0f), 124.0f, glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
        renderCube(shader,glm::vec3(-3.0f, 0.0f, 0.0), texture_container, glm::vec3(0.5f));

        renderGround(shader, 10, 10);
        
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);
        model = glm::mat4(1.0);
        model = glm::scale(model, glm::vec3(0.5f));
        shader.setMat4("model", model);
        myModel.Draw(shader);

        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);
        glDisable(GL_DEPTH_TEST);
        model = glm::mat4(1.0);
        model = glm::scale(model, glm::vec3(0.5f));
        outlineShader.use();
        outlineShader.setMat4("model", model);
        outlineShader.setMat4("view", view);
        outlineShader.setFloat("scale", 0.01f);
        outlineShader.setVec3("color", glm::vec3(1.0f,1.0f,0.0f));
        myModel.Draw(outlineShader);
        glStencilMask(0xFF);
        glStencilFunc(GL_ALWAYS, 0, 0xFF);
        glEnable(GL_DEPTH_TEST);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();      
        {
            static float f = 0.0f;
            glm::vec3 pos = cameraPos_model;
            static int counter = 0;
            ImGui::Begin("scene panel"); 
            ImGui::Text("camera position x:%.2f y:%.2f z:%2.f", pos.x, pos.y, pos.z);
            ImGui::Text("light  position x:%.2f y:%.2f z:%2.f", lightPos.x, lightPos.y, lightPos.z);
            
            ImGui::SliderFloat("ambient factor", &ambientFactor, 0.0f, 1.0f); 
            ImGui::ColorEdit3("light color", (float*)&light_color);
            if (ImGui::Button("Button")) 
                counter++;

            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);
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

    return 1;
}

void mouseCallback_model(GLFWwindow *window, double xPos, double yPos)
{
    float deltaX,deltaY;
    if(firstMouse_model)
    {
        deltaX = 0.0f;
        deltaY = 0.0f;
        firstMouse_model = false;
    }
    else
    {
        deltaX = xPos - lastPosx_model;
        deltaY = lastPosy_model - yPos;
    }
    lastPosx_model = xPos;
    lastPosy_model = yPos;

    float sensitivity = 0.1f;
    float xOffset = deltaX * sensitivity;
    float yOffset = deltaY * sensitivity;

    pitch_model += yOffset;
    if(pitch_model > 89.0f)
        pitch_model = 89.0f;
    if(pitch_model < -89.0f)
        pitch_model = -89.0f;

    yaw_model += xOffset;

    glm::vec3 front;
    front.x = cos(glm::radians(pitch_model)) * cos(glm::radians(yaw_model));
    front.y = sin(glm::radians(pitch_model));
    front.z = cos(glm::radians(pitch_model)) * sin(glm::radians(yaw_model));
    cameraFront_model = glm::normalize(front);
    cameraRight_model = glm::normalize(glm::cross(cameraFront_model, worldUp_model));
    cameraUp_model = glm::normalize(glm::cross(cameraRight_model, cameraFront_model));
}

void processInput_model(GLFWwindow *window)
{
    float moveSpeed = 2.5f * deltaTime_model;
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        cameraPos_model -= cameraRight_model * moveSpeed;
    }
    else  if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        cameraPos_model += cameraRight_model* moveSpeed;
    }
    else  if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        cameraPos_model +=  cameraFront_model * moveSpeed;
    }
    else  if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        cameraPos_model -=  cameraFront_model * moveSpeed;
    }
}

unsigned int VAO_model,VBO_model;
void renderCube(Shader &shader, glm::vec3 position, unsigned int texture,
                glm::vec3 scal, float angle, glm::vec3 rotateAxis)
{
    if(VAO_model == 0)
    {
        float vertices[] = 
        {
            //back
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
             0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
             0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,
             0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,
            -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
            //front
            -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 0.0f,  1.0f,
            0.5f,  -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f,  1.0f,
            0.5f,   0.5f,  0.5f, 1.0f, 1.0f, 0.0f, 0.0f,  1.0f,
            0.5f,   0.5f,  0.5f, 1.0f, 1.0f, 0.0f, 0.0f,  1.0f,
            -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f,  1.0f,
            -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 0.0f,  1.0f,
            //left
            -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
            -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
            -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
            //right
             0.5f,  0.5f,  0.5f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f,
             0.5f,  0.5f, -0.5f, 1.0f, 1.0f,  1.0f, 0.0f, 0.0f,
             0.5f, -0.5f, -0.5f, 1.0f, 0.0f,  1.0f, 0.0f, 0.0f,
             0.5f, -0.5f, -0.5f, 1.0f, 0.0f,  1.0f, 0.0f, 0.0f,
             0.5f, -0.5f,  0.5f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
             0.5f,  0.5f,  0.5f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f,
            //bottom
            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
             0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f,
             0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
             0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
            -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
            //top
            -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
             0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 0.0f,  1.0f, 0.0f,
             0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
             0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
            -0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 0.0f,  1.0f, 0.0f,
            -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  1.0f, 0.0f, 
        };
        
        glGenVertexArrays(1, &VAO_model);
        glGenBuffers(1, &VBO_model);

        glBindVertexArray(VAO_model);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_model);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5*sizeof(float)));
        glEnableVertexAttribArray(2);
    }
    glm::mat4 model(1.0f);
    model = glm::translate(model,position);
    model = glm::scale(model, scal);
    model = glm::rotate(model, glm::radians(angle), rotateAxis);
    shader.use();
    shader.setMat4("model", model);
    shader.setInt("material.texture_diffuse1",0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(VAO_model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

unsigned int groundVAO = 0, groundVBO = 0;
unsigned int groundTexture = 0;
void renderGround(Shader &shader, unsigned int nWidth, unsigned int nHeight)
{
    if(groundVAO == 0)
    {
        std::vector<Vertex> vertices;
        groundTexture = loadTexture("/Users/cabin/Documents/cpp/learngl/textures/wood.png");
        int startX =  -(int)nWidth;
        int startY =  -(int)nHeight;
        float posY = -1.0f;
        for(int i = startX; i < (int)nWidth;i++)
        {
            for(int j = startY; j < (int)nHeight; j++)
            {
                Vertex vert;
                vert.Position  = glm::vec3(i, posY, j);
                vert.TexCoords = glm::vec2(0.0f, 1.0f);
                vert.Normal    = glm::vec3(0.0f, 1.0f, 0.0f);
                vertices.push_back(vert);

                vert.Position  = glm::vec3(i+1, posY, j);
                vert.TexCoords = glm::vec2(1.0f, 1.0f);
                vert.Normal    = glm::vec3(0.0f, 1.0f, 0.0f);
                vertices.push_back(vert);

                vert.Position  = glm::vec3(i+1, posY, j+1);
                vert.TexCoords = glm::vec2(1.0f, 0.0f);
                vert.Normal    = glm::vec3(0.0f, 1.0f, 0.0f);
                vertices.push_back(vert);

                vert.Position  = glm::vec3(i+1, posY, j+1);
                vert.TexCoords = glm::vec2(1.0f, 0.0f);
                vert.Normal    = glm::vec3(0.0f, 1.0f, 0.0f);
                vertices.push_back(vert);

                vert.Position  = glm::vec3(i, posY, j+1);
                vert.TexCoords = glm::vec2(0.0f, 0.0f);
                vert.Normal    = glm::vec3(0.0f, 1.0f, 0.0f);
                vertices.push_back(vert);

                vert.Position  = glm::vec3(i, posY, j);
                vert.TexCoords = glm::vec2(0.0f, 1.0f);
                vert.Normal    = glm::vec3(0.0f, 1.0f, 0.0f);
                vertices.push_back(vert);
            }
        }
        
        glGenVertexArrays(1, &groundVAO);
        glGenBuffers(1, &groundVBO);
        
        glBindVertexArray(groundVAO);
        glBindBuffer(GL_ARRAY_BUFFER, groundVBO);

        glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,TexCoords));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex,Normal));
        glEnableVertexAttribArray(2);
        glBindVertexArray(0);
    }
    shader.use();
    glBindVertexArray(groundVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, groundTexture);
    glm::mat4 model(1.0f);
    shader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 4 * nWidth * nHeight * 6);
}