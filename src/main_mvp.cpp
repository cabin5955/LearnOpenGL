#include "shader.h"
#include "common.hpp"

#include "stb_image.h"

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

void cursorPositionCallback(GLFWwindow *window, double xPos, double yPos);
void cursorEnterCallback(GLFWwindow *window, int entered);
void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
void scrollCallback(GLFWwindow *window, double xOffset, double yOffset);

void processInput_mvp(GLFWwindow *window);

glm::vec2 currMousePos;
glm::vec2 prevMousePos;
glm::vec2 deltaMousePos = {0.0f,0.0f};

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float pitch = 0.0f, yaw = -90.0f;

int main_mvp()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_REFRESH_RATE, 60);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    #endif

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "mvp demo", NULL, NULL);
    if(window == NULL)
    {
        std::cout<<"create window failed"<<std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    glfwSetCursorPosCallback(window, cursorPositionCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetCursorEnterCallback(window, cursorEnterCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout<<"glad load failed"<<std::endl;
        glfwTerminate();
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    float vertices[] = 
    {
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
         0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
         0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
         0.5f,  0.5f, 0.5f, 1.0f, 1.0f,
         0.5f,  0.5f, 0.5f, 1.0f, 1.0f,
        -0.5f,  0.5f, 0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 

        -0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, 1.0f, 0.0f,

        0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
        0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f,  0.5f, 0.0f, 0.0f,
        0.5f,  0.5f,  0.5f, 1.0f, 0.0f,

        -0.5f,-0.5f, -0.5f, 0.0f, 1.0f,
         0.5f,-0.5f, -0.5f, 1.0f, 1.0f,
         0.5f,-0.5f,  0.5f, 1.0f, 0.0f,
         0.5f,-0.5f,  0.5f, 1.0f, 0.0f,
        -0.5f,-0.5f,  0.5f, 0.0f, 0.0f,
        -0.5f,-0.5f, -0.5f, 0.0f, 1.0f, 

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
         0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
         0.5f, 0.5f,  0.5f, 1.0f, 0.0f,
         0.5f, 0.5f,  0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f,  0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,       
    };

    unsigned int VAO,VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width,height,nChannels;
    unsigned char* data = stbi_load("/Users/cabin/Documents/cpp/learngl/textures/wall.jpg",&width,&height,&nChannels,0);
    if(data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0 , GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout<<"stb load image failed"<<std::endl;
    }
    stbi_image_free(data);

    Shader shader("/Users/cabin/Documents/cpp/learngl/shaders/mvp.vert",
                  "/Users/cabin/Documents/cpp/learngl/shaders/mvp.frag");

    glm::mat4 model(1.0f);
    glm::mat4 view(1.0f);
    glm::mat4 projection(1.0f);

    //view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    while(!glfwWindowShouldClose(window))
    {
        processInput_mvp(window);
        glClearColor(0.3f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        //model = glm::rotate(model, (float)deltaMousePos.x*0.01f, glm::vec3(0.0f,1.0f,0.0f));
        //model = glm::rotate(model, (float)deltaMousePos.y*0.01f, glm::vec3(1.0f, 0.0f, 0.0f));
        view = glm::lookAt(cameraPos, cameraPos+cameraFront, cameraUp);
        shader.setMat4("model", model);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);

        glBindVertexArray(VAO);
        glBindTexture(GL_TEXTURE_2D, texture);;
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 1;
}

void processInput_mvp(GLFWwindow *window)
{
    float cameraSpeed = 0.05f;
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }
    else if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }
    else if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        cameraPos += cameraFront * cameraSpeed;
    }
    else if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        cameraPos -= cameraFront * cameraSpeed;   
    }
}

void cursorPositionCallback(GLFWwindow *window, double xPos, double yPos)
{
    prevMousePos = currMousePos;
    currMousePos.x = xPos;
    currMousePos.y = yPos;
    if(firstMouse)
    {
        prevMousePos = currMousePos;
        deltaMousePos.x = 0.0f;
        deltaMousePos.y = 0.0f;
        firstMouse = false;
        return;
    }
    deltaMousePos.x = currMousePos.x - prevMousePos.x;
    deltaMousePos.y = currMousePos.y - prevMousePos.y;

    float sensitivity = 0.05f;
    float xoffset = deltaMousePos.x * sensitivity;
    float yoffset = deltaMousePos.y * sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if(pitch > 89.0f)
        pitch = 89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
    front.y = sin(glm::radians(pitch));
    front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));  

    cameraFront = glm::normalize(front);
}

void cursorEnterCallback(GLFWwindow *window, int entered)
{
    if(entered)
    {
        std::cout<<"cursor entered window"<<std::endl;
    }
    else
    {
        std::cout<<"cursor left window"<<std::endl;
    }
}

void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    if(button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        std::cout<<"right button pressed"<<std::endl;
    }
    else if(button ==  GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    {
        std::cout<<"right button release"<<std::endl;
    }
}

void scrollCallback(GLFWwindow *window, double xOffset, double yOffset)
{

}