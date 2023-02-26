#include "shader.h"
#include "camera.h"
#include "common.hpp"
#include "stb_image.h"
#include <iostream>
#include <vector>

void key_callback_cpuheight(GLFWwindow* window, int key, int scancode, int action, int modifiers);
void mouse_callback_cpuheight(GLFWwindow* window, double xpos, double ypos);
void scroll_callback_cpuheight(GLFWwindow* window, double xoffset, double yoffset);
void processInput_cpuheight(GLFWwindow *window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int useWireframe = 0;
int displayGrayscale = 0;

Camera camera_cpuheight(glm::vec3(67.0f, 627.5f, 169.9f),
                        glm::vec3(0.0f, 1.0f, 0.0f),
                        -128.1f, -42.4f);

float lastX_cpuheight = SCR_WIDTH / 2.0f;
float lastY_cpuheight = SCR_HEIGHT / 2.0f;
bool firstMouse_cpuheight = true;

// timing
float deltaTime_cpuheight = 0.0f;
float lastFrame_cpuheight = 0.0f;

int main_cpuheight()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL: Terrain CPU", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback_cpuheight);
    glfwSetCursorPosCallback(window, mouse_callback_cpuheight);
    glfwSetScrollCallback(window, scroll_callback_cpuheight);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout<<"Failed to load glad loader"<<std::endl;
        glfwTerminate();
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    Shader heightMapShader("/Users/cabin/Documents/cpp/learngl/shaders/cpuheight.vert",
                           "/Users/cabin/Documents/cpp/learngl/shaders/cpuheight.frag");
    stbi_set_flip_vertically_on_load(true); 
    int width, height, nrChannels;
    unsigned char *data = stbi_load("/Users/cabin/Documents/cpp/learngl/textures/heightmaps/shenzhen_cpuheight.png",
                                    &width, &height, &nrChannels, 0);
    if (data)
    {
        std::cout << "Loaded heightmap of size " << height << " x " << width << std::endl;
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    std::vector<float> vertices;
    float yScale = 64.0f/256.0f, yShift = 16.0f;
    int rez = 1;
    unsigned bytePerpixel = nrChannels;
    for(int i = 0; i < height; i++)
    {
        for(int j = 0; j < width; j++)
        {
            unsigned char* pixelOffset = data + (j + width * i) * bytePerpixel;
            unsigned char y = pixelOffset[0];

            vertices.push_back( -height/2.0f + height*i/(float)height );
            vertices.push_back( (int) y * yScale - yShift );
            vertices.push_back( -width/2.0f + width*j/(float)width );
        }
    }
    std::cout << "Loaded " << vertices.size() / 3 << " vertices" << std::endl;
    stbi_image_free(data);

    std::vector<unsigned> indices;
    for(unsigned i = 0; i < height-1; i += rez)
    {
        for(unsigned j = 0; j < width; j += rez)
        {
            for(unsigned k = 0; k < 2; k++)
            {
                indices.push_back(j + width * (i + k*rez));
            }
        }
    }
    std::cout << "Loaded " << indices.size() << " indices" << std::endl;

    const int numStrips = (height-1)/rez;
    const int numTrisPerStrip = (width/rez)*2-2;
    std::cout << "Created lattice of " << numStrips << " strips with " << numTrisPerStrip << " triangles each" << std::endl;
    std::cout << "Created " << numStrips * numTrisPerStrip << " triangles total" << std::endl;

    unsigned int terrainVAO, terrainVBO, terrainIBO;
    glGenVertexArrays(1, &terrainVAO);
    glBindVertexArray(terrainVAO);

    glGenBuffers(1, &terrainVBO);
    glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &terrainIBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned), &indices[0], GL_STATIC_DRAW);

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime_cpuheight = currentFrame - lastFrame_cpuheight;
        lastFrame_cpuheight = currentFrame;
        processInput_cpuheight(window);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.3f, 0.2f, 0.2f, 1.0f);

        heightMapShader.use();
        
        glm::mat4 projection = glm::perspective(glm::radians(camera_cpuheight.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100000.0f);
        glm::mat4 view = camera_cpuheight.GetViewMatrix();
        heightMapShader.setMat4("projection", projection);
        heightMapShader.setMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f);
        heightMapShader.setMat4("model", model);

        glBindVertexArray(terrainVAO);
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        for(unsigned strip = 0; strip < numStrips; strip++)
        {
            glDrawElements(GL_TRIANGLE_STRIP,   // primitive type
                           numTrisPerStrip+2,   // number of indices to render
                           GL_UNSIGNED_INT,     // index data type
                           (void*)(sizeof(unsigned) * (numTrisPerStrip+2) * strip)); // offset to starting index
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();    

    return 0;
}

void key_callback_cpuheight(GLFWwindow* window, int key, int scancode, int action, int modifiers)
{
    if(action == GLFW_PRESS)
    {
        switch(key)
        {
            case GLFW_KEY_SPACE:
                useWireframe = 1 - useWireframe;
                break;
            case GLFW_KEY_G:
                displayGrayscale = 1 - displayGrayscale;
                break;
            default:
                break;
        }
    }
}

void mouse_callback_cpuheight(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse_cpuheight)
    {
        lastX_cpuheight = xpos;
        lastY_cpuheight = ypos;
        firstMouse_cpuheight = false;
    }

    float xoffset = xpos - lastX_cpuheight;
    float yoffset = lastY_cpuheight - ypos; // reversed since y-coordinates go from bottom to top

    lastX_cpuheight = xpos;
    lastY_cpuheight = ypos;

    camera_cpuheight.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback_cpuheight(GLFWwindow* window, double xoffset, double yoffset)
{
    camera_cpuheight.ProcessMouseScroll(yoffset);
}

void processInput_cpuheight(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera_cpuheight.ProcessKeyboard(FORWARD, deltaTime_cpuheight);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera_cpuheight.ProcessKeyboard(BACKWARD, deltaTime_cpuheight);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera_cpuheight.ProcessKeyboard(LEFT, deltaTime_cpuheight);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera_cpuheight.ProcessKeyboard(RIGHT, deltaTime_cpuheight);
}