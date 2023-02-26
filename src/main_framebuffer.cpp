#include "common.hpp"
#include "shader.h"
#include "texutils.h"
#include "model.h"

const int SCR_WIDTH = 1280;
const int SCR_HEIGHT = 960;
void mouseCallback_framebuffer(GLFWwindow *window, double xPos, double yPos);
void processInput_framebuffer(GLFWwindow *window);

float lastFrameTime = 0.0f;
float deltaTime = 0.0f;

unsigned int VAO, VBO;
unsigned int planeVAO, planeVBO;
unsigned int texture, texture_plane;

int main_framebuffer()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    #endif

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "framebuffer window", NULL, NULL);
    if(window == NULL)
    {
        std::cout<<"create window failed"<<std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouseCallback_framebuffer);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout<<"glad load failed"<<std::endl;
        glfwTerminate();
        return -1;
    }
    
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

    float planeVertices[] = 
    {
        -10.5f,  0.5f, -10.5f, 0.0f,  10.0f, 0.0f,1.0f, 0.0f,
         10.5f,  0.5f, -10.5f, 10.0f, 10.0f, 0.0f,1.0f, 0.0f,
         10.5f,  0.5f,  10.5f, 10.0f, 0.0f,  0.0f,1.0f, 0.0f,
         10.5f,  0.5f,  10.5f, 10.0f, 0.0f,  0.0f,1.0f, 0.0f,
        -10.5f,  0.5f,  10.5f, 0.0f,  0.0f,  0.0f,1.0f, 0.0f,
        -10.5f,  0.5f, -10.5f, 0.0f,  10.0f, 0.0f,1.0f, 0.0f,
    };
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);

    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5*sizeof(float)));
    glEnableVertexAttribArray(2);

    float quadVertices[] =
    {
        -1.0f,  1.0f, 0.0f, 1.0f,
         1.0f,  1.0f, 1.0f, 1.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f, 0.0f,
        -1.0f,  1.0f, 0.0f, 1.0f
    };
    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);

    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2*sizeof(float)));

    unsigned int framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    unsigned int textureColorbuffer;
    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH*2, SCR_HEIGHT*2, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH*2, SCR_HEIGHT*2);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout<<"framebuffer is not complete"<<std::endl;
    }
    else
    {
        std::cout<<"framebuffer is complete"<<std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    texture = loadTexture("/Users/cabin/Documents/cpp/learngl/textures/container2.png");
    texture_plane = loadTexture("/Users/cabin/Documents/cpp/learngl/textures/wood.png");

    glm::mat4 model(1.0f);
    glm::vec3 eye = glm::vec3(0.0f,0.0f,6.0f);
    glm::vec3 center = glm::vec3(0.0f,0.0f,0.0f);       
    glm::vec3 up =  glm::vec3(0.0f,1.0f,0.0f);
    glm::mat4 view = glm::lookAt(eye, center, up);
    glm::mat4 projection = glm::perspective(glm::radians(45.0f),(float)SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 100.0f);

    Shader shader("/Users/cabin/Documents/cpp/learngl/shaders/framebuffer.vert",
                  "/Users/cabin/Documents/cpp/learngl/shaders/framebuffer.frag");
    shader.use();
    shader.setMat4("model", model);
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
    shader.setInt("ourTexture", 0);

    Shader outlineShader("/Users/cabin/Documents/cpp/learngl/shaders/outline2.vert",
                         "/Users/cabin/Documents/cpp/learngl/shaders/outline2.frag");
    outlineShader.use();
    outlineShader.setMat4("view", view);
    outlineShader.setMat4("projection", projection);

    Shader screenShader("/Users/cabin/Documents/cpp/learngl/shaders/screen.vert",
                        //"/Users/cabin/Documents/cpp/learngl/shaders/kernel.frag");
                        "/Users/cabin/Documents/cpp/learngl/shaders/grayscale.frag");
                        //"/Users/cabin/Documents/cpp/learngl/shaders/inversion.frag");
                        //"/Users/cabin/Documents/cpp/learngl/shaders/screen.frag");
    screenShader.setInt("ourTexture", 0);

    while(!glfwWindowShouldClose(window))
    {
        deltaTime = glfwGetTime() - lastFrameTime;
        lastFrameTime = glfwGetTime();

        processInput_framebuffer(window);

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glEnable(GL_DEPTH_TEST);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

        glClearColor(0.3f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);
        glBindVertexArray(VAO);
        shader.use();
        float angle = glm::radians(glfwGetTime() * 20);
        glm::mat4 model(1.0f);
        model = glm::rotate(model, angle, glm::vec3(0.0f,1.0f, 0.0f));
        shader.setMat4("model", model);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);
        glDisable(GL_DEPTH_TEST);
        glBindVertexArray(VAO);
        outlineShader.use();
        outlineShader.setMat4("view", view);
        model = glm::scale(model, glm::vec3(1.1f));
        outlineShader.setMat4("model", model);
        outlineShader.setVec3("color", glm::vec3(1.0f,1.0f,0.0f));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glEnable(GL_DEPTH_TEST);    

        glBindVertexArray(VAO);
        glm::mat4 model2(1.0f);
        model2 = glm::translate(model2,glm::vec3(2.0f,1.0f, -1.0f));
        model2 = glm::rotate(model2, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
        shader.use();
        shader.setMat4("model", model2);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindVertexArray(VAO);
        glm::mat4 model3(1.0f);
        model3 = glm::translate(model3,glm::vec3(-2.0f,-0.5f, -1.0f));
        model3 = glm::rotate(model3, glm::radians(124.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
        model3 = glm::scale(model3, glm::vec3(1.0f));
        shader.use();
        shader.setMat4("model", model3);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindVertexArray(planeVAO);
        glBindTexture(GL_TEXTURE_2D, texture_plane);
        shader.use();
        glm::mat4 planeModel(1.0f);
        planeModel = glm::translate(planeModel,glm::vec3(0.0f,-1.0f, 0.0f));
        planeModel = glm::scale(planeModel, glm::vec3(0.5f));
        shader.setMat4("model", planeModel);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST);

        glClearColor(0.3f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        screenShader.use();
        glBindVertexArray(quadVAO);
        glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &planeVAO);
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &planeVBO);
    glDeleteBuffers(1, &quadVBO);

    return 0;
}

void mouseCallback_framebuffer(GLFWwindow *window, double xPos, double yPos)
{

}

void processInput_framebuffer(GLFWwindow *window)
{

}
