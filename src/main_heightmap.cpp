#include "shader.h"
#include "common.hpp"
#include "texutils.h"
#include "mesh.h"
#include "camera.h"
#include "stb_image.h"
#include <vector>

void key_callback_heightmap(GLFWwindow* window, int key, int scancode, int action, int modifiers);
void mouse_callback_heightmap(GLFWwindow* window, double xpos, double ypos);
void scroll_callback_heightmap(GLFWwindow* window, double xoffset, double yoffset);
void processInput_heightmap(GLFWwindow *window);
unsigned int loadHeightMapTexture(const char *path);

const int SCR_WIDTH = 1280;
const int SCR_HEIGHT = 960;

Camera camera_heightmap(glm::vec3(0.0f, 5.0f, 20.0f));

float lastX_heightmap = SCR_WIDTH / 2.0f;
float lastY_heightmap = SCR_HEIGHT / 2.0f;
bool firstMouse_heightmap = true;

// timing
float deltaTime_heightmap = 0.0f;
float lastFrame_heightmap = 0.0f;

int main_heightmap()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    #endif

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "WINDOW HEIGHTMAP", NULL, NULL);
    if(window == NULL)
    {
        std::cout<<"create window failed"<<std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback_heightmap);
    glfwSetCursorPosCallback(window, mouse_callback_heightmap);
    glfwSetScrollCallback(window, scroll_callback_heightmap);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout<<"glad load failed"<<std::endl;
        glfwTerminate();
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    int width = 200;
    int height = 200;

    std::vector<Vertex> vertices;
    float posY = -10.0f;
    float LEN = 0.1f;
    for(int i = -width; i < width;i++)
    {
        for(int j = -height; j < height; j++)
        {
            Vertex vert;
            vert.Position  = glm::vec3(i*LEN, posY, j*LEN);
            vert.Normal    = glm::vec3(0.0f, 1.0f, 0.0f);
            vert.TexCoords = glm::vec2((width+i)/(2.0f*width), 1.0f - (height+j)/(2.0f*height));
            vertices.push_back(vert);

            vert.Position  = glm::vec3((i+1)*LEN, posY, j*LEN);
            vert.Normal    = glm::vec3(0.0f, 1.0f, 0.0f);
            vert.TexCoords = glm::vec2((width+i+1)/(2.0f*width), 1.0f - (height+j)/(2.0f*height));
            vertices.push_back(vert);

            vert.Position  = glm::vec3((i+1)*LEN, posY, (j+1)*LEN);
            vert.Normal    = glm::vec3(0.0f, 1.0f, 0.0f);
            vert.TexCoords = glm::vec2((width+i+1)/(2.0f*width), 1.0f - (height+j+1)/(2.0f*height));
            vertices.push_back(vert);

            vert.Position  = glm::vec3((i+1)*LEN, posY, (j+1)*LEN);
            vert.Normal    = glm::vec3(0.0f, 1.0f, 0.0f);
            vert.TexCoords = glm::vec2((width+i+1)/(2.0f*width), 1.0f - (height+j+1)/(2.0f*height));
            vertices.push_back(vert);

            vert.Position  = glm::vec3(i*LEN, posY, (j+1)*LEN);
            vert.Normal    = glm::vec3(0.0f, 1.0f, 0.0f);
            vert.TexCoords = glm::vec2((width+i)/(2.0f*width), 1.0f - (height+j+1)/(2.0f*height));
            vertices.push_back(vert);

            vert.Position  = glm::vec3(i*LEN, posY, j*LEN);
            vert.Normal    = glm::vec3(0.0f, 1.0f, 0.0f);
            vert.TexCoords = glm::vec2((width+i)/(2.0f*width), 1.0f - (height+j)/(2.0f*height));
            vertices.push_back(vert);
        }
    }
    int verticesCount =  vertices.size();

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, verticesCount*sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

    stbi_set_flip_vertically_on_load(true);
    unsigned int dep =  loadHeightMapTexture("/Users/cabin/Documents/cpp/learngl/textures/heightmaps/heightmap.png");
    unsigned int tex1 = loadHeightMapTexture("/Users/cabin/Documents/cpp/learngl/textures/heightmaps/rock.jpg");
    unsigned int tex2 = loadHeightMapTexture("/Users/cabin/Documents/cpp/learngl/textures/heightmaps/water.jpg");

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 1000.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 5.0f, 25.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 model = glm::mat4(1.0f);
    Shader heightMapShader("/Users/cabin/Documents/cpp/learngl/shaders/heightmap.vert",
                           "/Users/cabin/Documents/cpp/learngl/shaders/heightmap.frag");
    heightMapShader.use();
    heightMapShader.setMat4("model", model);
    heightMapShader.setMat4("view", view);
    heightMapShader.setMat4("projection", projection);
    heightMapShader.setInt("dep", 0);
    heightMapShader.setInt("tex1", 1);
    heightMapShader.setInt("tex2", 2);

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime_heightmap = currentFrame - lastFrame_heightmap;
        lastFrame_heightmap = currentFrame;
        processInput_heightmap(window);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.3f, 0.2f, 0.2f, 1.0f);

        heightMapShader.use();
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100000.0f);
        glm::mat4 view = camera_heightmap.GetViewMatrix();
        heightMapShader.setMat4("projection", projection);
        heightMapShader.setMat4("view", view);
        glm::mat4 model = glm::mat4(1.0f);
        heightMapShader.setMat4("model", model);
        
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, dep);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, tex1);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, tex2);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, verticesCount);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;    
}

void key_callback_heightmap(GLFWwindow* window, int key, int scancode, int action, int modifiers)
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

void mouse_callback_heightmap(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse_heightmap)
    {
        lastX_heightmap = xpos;
        lastY_heightmap = ypos;
        firstMouse_heightmap = false;
    }

    float xoffset = xpos - lastX_heightmap;
    float yoffset = lastY_heightmap - ypos; // reversed since y-coordinates go from bottom to top

    lastX_heightmap = xpos;
    lastY_heightmap = ypos;

    camera_heightmap.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback_heightmap(GLFWwindow* window, double xoffset, double yoffset)
{
    camera_heightmap.ProcessMouseScroll(yoffset);
}

void processInput_heightmap(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera_heightmap.ProcessKeyboard(FORWARD, deltaTime_heightmap);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera_heightmap.ProcessKeyboard(BACKWARD, deltaTime_heightmap);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera_heightmap.ProcessKeyboard(LEFT, deltaTime_heightmap);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera_heightmap.ProcessKeyboard(RIGHT, deltaTime_heightmap);
}

unsigned int loadHeightMapTexture(const char *path)
{
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    int width,height,nChannel;
    unsigned char *data = stbi_load(path, &width, &height, &nChannel, 0);
    if(data != NULL)
    {
        int format;
        if(nChannel == 4)
            format = GL_RGBA;
        else if(nChannel == 3)
            format = GL_RGB;
        else
            format = GL_RED;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
    else
    {
        std::cout<<"load height map texture failed"<<std::endl;
    }
    stbi_image_free(data);
    return texture;
}