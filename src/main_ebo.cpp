#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "common.hpp"
#include "shader.h"

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main_ebo()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        std::cout<<"apple window hint"<<std::endl;
    #endif

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "ebo triangle", NULL, NULL);
    if(window == NULL)
    {
        std::cout << "create window failed "<<std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout<<"glad load failed "<<std::endl;
        glfwTerminate();
        return -1;
    }

    Shader shader("/Users/cabin/Documents/cpp/learngl/shaders/simple.vert", 
                  "/Users/cabin/Documents/cpp/learngl/shaders/simple.frag");

    float vertices[] = {
        0.25f,  0.25f, 0.0f,
        0.25f, -0.25f, 0.0f,
       -0.25f, -0.25f, 0.0f,
       -0.25f,  0.25f, 0.0f 
    };
    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int VAO,VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices),indices,GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glm::mat4 model(1.0f);
    float timeValue = 0.0f;
    
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        glClearColor(0.3f,0.2f,0.2f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.use();
        timeValue += 0.1f;
        if(timeValue > 360.0f)
            timeValue = 0.0f;
        model = glm::rotate(model, glm::radians(timeValue), glm::vec3(0.0f,0.0f,1.0f));
        shader.setMat4("model", model);
        glBindVertexArray(VAO);
        //glDrawArrays(GL_TRIANGLES, 0 , 3);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glfwTerminate();
    return 0;    
}