#include <filesystem>
namespace fs = std::filesystem;

#include "common.hpp"
#include "shader.h"
#include "camera.h"
#include "texutils.h"
#include "vertex2.h"
#include "shader_class.h"
#include "texture.h"
#include "render_util.h"
#include <vector>

const unsigned int width = 800;
const unsigned int height = 800;

// Number of samples per pixel for MSAA
unsigned int bloom_samples3 = 8;

// Controls the gamma function
float bloom_gamma3 = 2.2f;

int main3()
{
    // Initialize GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

	// Create a GLFWwindow object of 800 by 800 pixels, naming it "YoutubeOpenGL"
	GLFWwindow* window = glfwCreateWindow(width, height, "OpenGL Bloom", NULL, NULL);
	// Error check if the window fails to create
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// Introduce the window into the current context
	glfwMakeContextCurrent(window);

	//Load GLAD so it configures OpenGL
	gladLoadGL();
	// Specify the viewport of OpenGL in the Window
	// In this case the viewport goes from x = 0, y = 0, to x = 800, y = 800
	glViewport(0, 0, width, height);

    unsigned int containerTexture = loadTexture("/Users/cabin/Documents/cpp/learngl/textures/container2.png");
    unsigned int woodTexture = loadTexture("/Users/cabin/Documents/cpp/learngl/textures/wood.png");

    unsigned int diffuseTexture = loadTexture("/Users/cabin/Documents/cpp/learngl/textures/toy_box_diffuse.png");
    unsigned int normalTexture =  loadTexture("/Users/cabin/Documents/cpp/learngl/textures/toy_box_normal.png");
    unsigned int displacementTexture = loadTexture("/Users/cabin/Documents/cpp/learngl/textures/toy_box_disp.png");

    Shader shader("/Users/cabin/Documents/cpp/learngl/shaders/bloom3.vert",
                  "/Users/cabin/Documents/cpp/learngl/shaders/bloom3.frag",
                  "/Users/cabin/Documents/cpp/learngl/shaders/bloom3.geom");

    Shader shaderFramebuffer("/Users/cabin/Documents/cpp/learngl/shaders/bloomframebuffer2.vert",
                             "/Users/cabin/Documents/cpp/learngl/shaders/bloomframebuffer2.frag");

	std::string currentDir = fs::current_path().parent_path().string();
	std::string defaultVert = "/shaders/bloom2.vert";
	std::string defaultFrag = "/shaders/bloom2.frag";
	std::string defaultGeom = "/shaders/bloom2.geom";
	std::string framebufferVert = "/shaders/bloomframebuffer2.vert";
	std::string framebufferFrag = "/shaders/bloomframebuffer2.frag";
	std::string blurFrag = "/shaders/bloomblur2.frag";

	//ShaderClass shader((currentDir+defaultVert).c_str(),(currentDir+defaultFrag).c_str(),(currentDir+defaultGeom).c_str());
	//ShaderClass shaderFramebuffer((currentDir+framebufferVert).c_str(),(currentDir+framebufferFrag).c_str());
	ShaderClass shaderBlur((currentDir+framebufferVert).c_str(),(currentDir+blurFrag).c_str());
    
	shader.use();
    glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
	shader.setVec4("lightColor", lightColor);
	shader.setVec3("lightPos", lightPos);

    shaderFramebuffer.use();
	shaderFramebuffer.setInt("screenTexture", 0);
	shaderFramebuffer.setInt("bloomTexture", 1);
	shaderFramebuffer.setInt("gamma", bloom_gamma3);

	shaderBlur.use();
	shaderBlur.setInt("screenTexture", 0);

    glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glFrontFace(GL_CCW);

	// Creates camera object
	//Camera camera(width, height, glm::vec3(0.0f, 0.0f, 2.0f));
    glm::mat4 model(1.0f);
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 12.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, up);
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width/(float)height,0.1f,100.0f);

    // Variables to create periodic event for FPS displaying
	double prevTime = 0.0;
	double crntTime = 0.0;
	double timeDiff;
	// Keeps track of the amount of frames in timeDiff
	unsigned int counter = 0;

    // Create Frame Buffer Object
	unsigned int postProcessingFBO;
	glGenFramebuffers(1, &postProcessingFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, postProcessingFBO);

	// Create Framebuffer Texture
	unsigned int postProcessingTexture;
	glGenTextures(1, &postProcessingTexture);
	glBindTexture(GL_TEXTURE_2D, postProcessingTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width*2, height*2, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProcessingTexture, 0);

	// Create Second Framebuffer Texture
	unsigned int bloomTexture;
	glGenTextures(1, &bloomTexture);
	glBindTexture(GL_TEXTURE_2D, bloomTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width*2, height*2, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, bloomTexture, 0);

	// Tell OpenGL we need to draw to both attachments
	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);

    // Error checking framebuffer
	auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Post-Processing Framebuffer error: " << fboStatus << std::endl;

	// Create Ping Pong Framebuffers for repetitive blurring
	unsigned int pingpongFBO[2];
	unsigned int pingpongBuffer[2];
	glGenFramebuffers(2, pingpongFBO);
	glGenTextures(2, pingpongBuffer);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, pingpongBuffer[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongBuffer[i], 0);

		fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Ping-Pong Framebuffer error: " << fboStatus << std::endl;
	}

    // Main while loop
	while (!glfwWindowShouldClose(window))
	{
		// Updates counter and times
		crntTime = glfwGetTime();
		timeDiff = crntTime - prevTime;
		counter++;

		if (timeDiff >= 1.0 / 30.0)
		{
			// Creates new title
			std::string FPS = std::to_string((1.0 / timeDiff) * counter);
			std::string ms = std::to_string((timeDiff / counter) * 1000);
			std::string newTitle = "YoutubeOpenGL - " + FPS + "FPS / " + ms + "ms";
			//glfwSetWindowTitle(window, newTitle.c_str());

			// Resets times and counter
			prevTime = crntTime;
			counter = 0;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, postProcessingFBO);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		shader.use();		
		shader.setInt("diffuse0", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseTexture);

		shader.setInt("normal0", 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normalTexture);

		shader.setInt("displacement0", 2);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, displacementTexture);
        
        glm::mat4 matrix = glm::mat4(1.0f);
        shader.setMat4("model", matrix);
            
        shader.setVec3("camPos", cameraPos);
        shader.setMat4("view", view);
		shader.setMat4("projection", projection);
        renderColorCube();

		// Bounce the image data around to blur multiple times
		bool horizontal = true, first_iteration = true;
		// Amount of time to bounce the blur
		int amount = 2;
		shaderBlur.Activate();
		for (unsigned int i = 0; i < amount; i++)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
			shaderBlur.setInt("horizontal", horizontal);

			// In the first bounc we want to get the data from the bloomTexture
			if (first_iteration)
			{
				glBindTexture(GL_TEXTURE_2D, bloomTexture);
				first_iteration = false;
			}
			// Move the data between the pingPong textures
			else
			{
				glBindTexture(GL_TEXTURE_2D, pingpongBuffer[!horizontal]);
			}

			// Render the image		
			glDisable(GL_DEPTH_TEST);
			renderColorQuad();

			// Switch between vertical and horizontal blurring
			horizontal = !horizontal;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		shaderFramebuffer.use();
		glDisable(GL_DEPTH_TEST); // prevents framebuffer rectangle from being discarded
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, postProcessingTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, bloomTexture);
		renderColorQuad();

		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
	}

	glDeleteFramebuffers(1, &postProcessingFBO);
	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}
