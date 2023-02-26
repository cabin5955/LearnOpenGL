#include "shader.h"
#include "common.hpp"
#include "stb_image.h"

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

void mouseCallback_lighting(GLFWwindow *window, double xPos, double yPos);
void processInput_lighting(GLFWwindow *window);

glm::vec3 cameraFront_lighting = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraPos_lighting = glm::vec3(0.0f, 0.0f, 3.0f);
float deltaPos_x, deltaPos_y;
float lastPos_x, lastPos_y;
float pitch_lighting = 0.0f;
float yaw_lighting = -90.0f;
bool firstMouse_lighting = true;

int main_lighting()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_REFRESH_RATE, 60);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    #endif
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "lighting", NULL, NULL);
    if(window == NULL)
    {
        std::cout<<"create window failed"<<std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouseCallback_lighting);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout<<"glad load failed"<<std::endl;
        glfwTerminate();
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5*sizeof(float)));
    glEnableVertexAttribArray(2);

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

    unsigned int texture_diff;
    glGenTextures(1, &texture_diff);
    glBindTexture(GL_TEXTURE_2D, texture_diff);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    int width,height,nChannel;
    unsigned char *data;
    data = stbi_load("/Users/cabin/Documents/cpp/learngl/textures/container2.png", &width, &height, &nChannel, 0);
    if(data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout<<"load texture failed"<<std::endl;
    }
    stbi_image_free(data);

    unsigned int texture_spec;
    glGenTextures(1, &texture_spec);
    glBindTexture(GL_TEXTURE_2D, texture_spec);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    data = stbi_load("/Users/cabin/Documents/cpp/learngl/textures/container2_specular.png", &width, &height, &nChannel, 0);
    if(data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout<<"load texture failed"<<std::endl;
    }
    stbi_image_free(data);

    unsigned int texture_emission;
    glGenTextures(1, &texture_emission);
    glBindTexture(GL_TEXTURE_2D, texture_emission);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    data = stbi_load("/Users/cabin/Documents/cpp/learngl/textures/matrix.jpg", &height, &width, &nChannel, 0);
    if(data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout<<"load texture failed"<<std::endl;
    }
    stbi_image_free(data);

    Shader shader("/Users/cabin/Documents/cpp/learngl/shaders/lighting.vert",
                  "/Users/cabin/Documents/cpp/learngl/shaders/lighting.frag");

    glm::mat4 model(1.0f);
    float scal= 0.5f;
    model = glm::scale(model,glm::vec3(scal,scal,scal));
    glm::vec3 up = glm::vec3(0.0f,1.0f,0.0f);
    glm::mat4 view = glm::lookAt(cameraPos_lighting, cameraPos_lighting + cameraFront_lighting, up);
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH/(float)SCR_HEIGHT,0.1f,100.0f);

    Shader lightShader("/Users/cabin/Documents/cpp/learngl/shaders/color.vert",
                       "/Users/cabin/Documents/cpp/learngl/shaders/color.frag");


    float lightR = 1.0f;
    float lightMoveSpeed = 0.5f;
    float lightAngle = 0.0f;
    glm::mat4 lightModel(1.0f);
    glm::vec3 lightPos = glm::vec3(0.0f);
    float angle = glm::radians(lightAngle);
    lightPos = glm::vec3(lightR * cos(angle),1.0f,lightR * sin(angle));
    lightModel = glm::translate(lightModel, lightPos);
    lightModel = glm::scale(lightModel,glm::vec3(0.05f,0.05f,0.05f));

    while(!glfwWindowShouldClose(window))
    {
        processInput_lighting(window);
        glClearColor(0.3f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        lightAngle += lightMoveSpeed;
        float a = glm::radians(lightAngle);
        lightPos = glm::vec3(lightR * cos(a), 1.0f, lightR * sin(a));
        lightModel = glm::mat4(1.0);
        lightModel = glm::translate(lightModel, lightPos);
        lightModel = glm::scale(lightModel, glm::vec3(0.05f,0.05f,0.05f));

        view = glm::lookAt(cameraPos_lighting, cameraPos_lighting + cameraFront_lighting, up);
        glm::vec3 lightColor = glm::vec3(1.0,1.0f,0.0f);
        shader.use();
        shader.setMat4("model", model);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        shader.setVec3("light.position", lightPos);
        shader.setVec3("light.ambient", glm::vec3(0.2f,0.2f,0.2f));
        shader.setVec3("light.diffuse", glm::vec3(0.5f,0.5f,0.5f));
        shader.setVec3("light.specular", glm::vec3(1.0f,1.0f,1.0f));
        shader.setInt("material.texture_diffuse1", 0);
        shader.setInt("material.texture_specular1", 1);
        shader.setInt("material.texture_emission1",2);
        shader.setFloat("material.shininess", 64.0f);
        shader.setVec3("viewPos", cameraPos_lighting);
        shader.setFloat("matrixMove", glfwGetTime());
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_diff);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture_spec);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, texture_emission);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        lightShader.use();
        lightShader.setMat4("model",lightModel);
        lightShader.setMat4("view", view);
        lightShader.setMat4("projection", projection);
        lightShader.setVec3("color",lightColor);
        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();    
    return 0;
}

void mouseCallback_lighting(GLFWwindow *window, double xPos, double yPos)
{
    if(firstMouse_lighting)
    {
        deltaPos_x = 0.0f;
        deltaPos_y = 0.0f;
        firstMouse_lighting = false;
    }
    else
    {
        deltaPos_x = xPos - lastPos_x;
        deltaPos_y = yPos - lastPos_y;
    }
    lastPos_x = xPos;
    lastPos_y = yPos;

    float sensitivity = 0.05f;
    float xoffset = deltaPos_x * sensitivity;
    float yoffset = deltaPos_y * sensitivity;

    pitch_lighting += yoffset;
    yaw_lighting += xoffset;

    if(pitch_lighting > 89.0f)
        pitch_lighting = 89.0f;

    if(pitch_lighting < -89.0f)
        pitch_lighting = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(pitch_lighting)) * cos(glm::radians(yaw_lighting));
    front.y = sin(glm::radians(pitch_lighting));
    front.z = cos(glm::radians(pitch_lighting)) * sin(glm::radians(yaw_lighting));

    cameraFront_lighting = glm::normalize(front);
        
}

void processInput_lighting(GLFWwindow *window)
{
    float cameraSpeed = 0.05f;
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        cameraPos_lighting -= glm::normalize(glm::cross(cameraFront_lighting,up))*cameraSpeed;
    }
    else if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        cameraPos_lighting += glm::normalize(glm::cross(cameraFront_lighting,up))*cameraSpeed;
    }
    else if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        cameraPos_lighting += cameraFront_lighting*cameraSpeed;
    }
    else if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        cameraPos_lighting -= cameraFront_lighting*cameraSpeed;
    }
    else if(glfwGetKey(window,GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        
    }
}