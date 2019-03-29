#include <glad/glad.h>
#include <GLFW/glfw3.h>

//#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//#include "mesh.h"
#include "model.h"

#include "shader_s.h"
#include "camera.h"
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);

// settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 7.0f, 37.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//light Model
int directLight = 0;
int pointLight = 0;
int spotLight = 0;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif
    
    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "ModelShow", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    
    // build and compile our shader zprogram
    // ------------------------------------
    Shader lightingShader("lightShader.vs", "lightShader.fs");
    Shader lampShader("lampShader.vs", "lampShader.fs");
    
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
        
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
        
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
        
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };
    // positions all containers
    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };
	//position of c++
	glm::vec3 cubePositionsCpp[] = {
		glm::vec3(0.0f, 0.0f, -10.0f),
		glm::vec3(0.0f, 1.0f, -10.0f),
		glm::vec3(0.0f, 2.0f, -10.0f),
		glm::vec3(1.0f, 3.0f, -10.0f),
		glm::vec3(2.0f, 3.0f, -10.0f),
		glm::vec3(3.0f, 2.0f, -10.0f),
		glm::vec3(0.0f, -1.0f, -10.0f),
		glm::vec3(0.0f, -2.0f, -10.0f),
		glm::vec3(1.0f, -3.0f, -10.0f),
		glm::vec3(2.0f, -3.0f, -10.0f),
		glm::vec3(3.0f, -2.0f, -10.0f),
		glm::vec3(5.0f, 0.0f, -10.0f),
		glm::vec3(6.0f, 0.0f, -10.0f),
		glm::vec3(7.0f, 0.0f, -10.0f),
		glm::vec3(6.0f, 1.0f, -10.0f),
		glm::vec3(6.0f, -1.0f, -10.0f),
		glm::vec3(9.0f, 0.0f, -10.0f),
		glm::vec3(10.0f, 0.0f, -10.0f),
		glm::vec3(11.0f, 0.0f, -10.0f),
		glm::vec3(10.0f, 1.0f, -10.0f),
		glm::vec3(10.0f, -1.0f, -10.0f)
	};
	//position of floor
	glm::vec3 cubePositionsFloor[] = {
		glm::vec3(0.0f,1.0f,0.0f),
		glm::vec3(1.0f,1.0f,0.0f),
		glm::vec3(2.0f,1.0f,0.0f),
		glm::vec3(3.0f,1.0f,0.0f),
		glm::vec3(4.0f,1.0f,0.0f),
		glm::vec3(5.0f,1.0f,0.0f),
		glm::vec3(6.0f,1.0f,0.0f),
		glm::vec3(7.0f,1.0f,0.0f),
		glm::vec3(8.0f,1.0f,0.0f),
		glm::vec3(9.0f,1.0f,0.0f),
		glm::vec3(10.0f,1.0f,0.0f),
		glm::vec3(11.0f,1.0f,0.0f),
		glm::vec3(12.0f,1.0f,0.0f),
		glm::vec3(13.0f,1.0f,0.0f),
		glm::vec3(14.0f,1.0f,0.0f),
		glm::vec3(15.0f,1.0f,0.0f),
		glm::vec3(16.0f,1.0f,0.0f),
		glm::vec3(17.0f,1.0f,0.0f),
		glm::vec3(18.0f,1.0f,0.0f),
		glm::vec3(0.0f,1.0f,1.0f),
		glm::vec3(1.0f,1.0f,1.0f),
		glm::vec3(2.0f,1.0f,1.0f),
		glm::vec3(3.0f,1.0f,1.0f),
		glm::vec3(4.0f,1.0f,1.0f),
		glm::vec3(5.0f,1.0f,1.0f),
		glm::vec3(6.0f,1.0f,1.0f),
		glm::vec3(7.0f,1.0f,1.0f),
		glm::vec3(8.0f,1.0f,1.0f),
		glm::vec3(9.0f,1.0f,1.0f),
		glm::vec3(10.0f,1.0f,1.0f),
		glm::vec3(11.0f,1.0f,1.0f),
		glm::vec3(12.0f,1.0f,1.0f),
		glm::vec3(13.0f,1.0f,1.0f),
		glm::vec3(14.0f,1.0f,1.0f),
		glm::vec3(15.0f,1.0f,1.0f),
		glm::vec3(16.0f,1.0f,1.0f),
		glm::vec3(17.0f,1.0f,1.0f),
		glm::vec3(18.0f,1.0f,1.0f),
		glm::vec3(0.0f,1.0f,2.0f),
		glm::vec3(1.0f,1.0f,2.0f),
		glm::vec3(2.0f,1.0f,2.0f),
		glm::vec3(3.0f,1.0f,2.0f),
		glm::vec3(4.0f,1.0f,2.0f),
		glm::vec3(5.0f,1.0f,2.0f),
		glm::vec3(6.0f,1.0f,2.0f),
		glm::vec3(7.0f,1.0f,2.0f),
		glm::vec3(8.0f,1.0f,2.0f),
		glm::vec3(9.0f,1.0f,2.0f),
		glm::vec3(10.0f,1.0f,2.0f),
		glm::vec3(11.0f,1.0f,2.0f),
		glm::vec3(12.0f,1.0f,2.0f),
		glm::vec3(13.0f,1.0f,2.0f),
		glm::vec3(14.0f,1.0f,2.0f),
		glm::vec3(15.0f,1.0f,2.0f),
		glm::vec3(16.0f,1.0f,2.0f),
		glm::vec3(17.0f,1.0f,2.0f),
		glm::vec3(18.0f,1.0f,2.0f),
		glm::vec3(0.0f,1.0f,3.0f),
		glm::vec3(1.0f,1.0f,3.0f),
		glm::vec3(2.0f,1.0f,3.0f),
		glm::vec3(3.0f,1.0f,3.0f),
		glm::vec3(4.0f,1.0f,3.0f),
		glm::vec3(5.0f,1.0f,3.0f),
		glm::vec3(6.0f,1.0f,3.0f),
		glm::vec3(7.0f,1.0f,3.0f),
		glm::vec3(8.0f,1.0f,3.0f),
		glm::vec3(9.0f,1.0f,3.0f),
		glm::vec3(10.0f,1.0f,3.0f),
		glm::vec3(11.0f,1.0f,3.0f),
		glm::vec3(12.0f,1.0f,3.0f),
		glm::vec3(13.0f,1.0f,3.0f),
		glm::vec3(14.0f,1.0f,3.0f),
		glm::vec3(15.0f,1.0f,3.0f),
		glm::vec3(16.0f,1.0f,3.0f),
		glm::vec3(17.0f,1.0f,3.0f),
		glm::vec3(18.0f,1.0f,3.0f),
		glm::vec3(0.0f,1.0f,4.0f),
		glm::vec3(1.0f,1.0f,4.0f),
		glm::vec3(2.0f,1.0f,4.0f),
		glm::vec3(3.0f,1.0f,4.0f),
		glm::vec3(4.0f,1.0f,4.0f),
		glm::vec3(5.0f,1.0f,4.0f),
		glm::vec3(6.0f,1.0f,4.0f),
		glm::vec3(7.0f,1.0f,4.0f),
		glm::vec3(8.0f,1.0f,4.0f),
		glm::vec3(9.0f,1.0f,4.0f),
		glm::vec3(10.0f,1.0f,4.0f),
		glm::vec3(11.0f,1.0f,4.0f),
		glm::vec3(12.0f,1.0f,4.0f),
		glm::vec3(13.0f,1.0f,4.0f),
		glm::vec3(14.0f,1.0f,4.0f),
		glm::vec3(15.0f,1.0f,4.0f),
		glm::vec3(16.0f,1.0f,4.0f),
		glm::vec3(17.0f,1.0f,4.0f),
		glm::vec3(18.0f,1.0f,4.0f),
		glm::vec3(0.0f,1.0f,5.0f),
		glm::vec3(1.0f,1.0f,5.0f),
		glm::vec3(2.0f,1.0f,5.0f),
		glm::vec3(3.0f,1.0f,5.0f),
		glm::vec3(4.0f,1.0f,5.0f),
		glm::vec3(5.0f,1.0f,5.0f),
		glm::vec3(6.0f,1.0f,5.0f),
		glm::vec3(7.0f,1.0f,5.0f),
		glm::vec3(8.0f,1.0f,5.0f),
		glm::vec3(9.0f,1.0f,5.0f),
		glm::vec3(10.0f,1.0f,5.0f),
		glm::vec3(11.0f,1.0f,5.0f),
		glm::vec3(12.0f,1.0f,5.0f),
		glm::vec3(13.0f,1.0f,5.0f),
		glm::vec3(14.0f,1.0f,5.0f),
		glm::vec3(15.0f,1.0f,5.0f),
		glm::vec3(16.0f,1.0f,5.0f),
		glm::vec3(17.0f,1.0f,5.0f),
		glm::vec3(18.0f,1.0f,5.0f),
		glm::vec3(0.0f,1.0f,6.0f),
		glm::vec3(1.0f,1.0f,6.0f),
		glm::vec3(2.0f,1.0f,6.0f),
		glm::vec3(3.0f,1.0f,6.0f),
		glm::vec3(4.0f,1.0f,6.0f),
		glm::vec3(5.0f,1.0f,6.0f),
		glm::vec3(6.0f,1.0f,6.0f),
		glm::vec3(7.0f,1.0f,6.0f),
		glm::vec3(8.0f,1.0f,6.0f),
		glm::vec3(9.0f,1.0f,6.0f),
		glm::vec3(10.0f,1.0f,6.0f),
		glm::vec3(11.0f,1.0f,6.0f),
		glm::vec3(12.0f,1.0f,6.0f),
		glm::vec3(13.0f,1.0f,6.0f),
		glm::vec3(14.0f,1.0f,6.0f),
		glm::vec3(15.0f,1.0f,6.0f),
		glm::vec3(16.0f,1.0f,6.0f),
		glm::vec3(17.0f,1.0f,6.0f),
		glm::vec3(18.0f,1.0f,6.0f),
		glm::vec3(0.0f,1.0f,7.0f),
		glm::vec3(1.0f,1.0f,7.0f),
		glm::vec3(2.0f,1.0f,7.0f),
		glm::vec3(3.0f,1.0f,7.0f),
		glm::vec3(4.0f,1.0f,7.0f),
		glm::vec3(5.0f,1.0f,7.0f),
		glm::vec3(6.0f,1.0f,7.0f),
		glm::vec3(7.0f,1.0f,7.0f),
		glm::vec3(8.0f,1.0f,7.0f),
		glm::vec3(9.0f,1.0f,7.0f),
		glm::vec3(10.0f,1.0f,7.0f),
		glm::vec3(11.0f,1.0f,7.0f),
		glm::vec3(12.0f,1.0f,7.0f),
		glm::vec3(13.0f,1.0f,7.0f),
		glm::vec3(14.0f,1.0f,7.0f),
		glm::vec3(15.0f,1.0f,7.0f),
		glm::vec3(16.0f,1.0f,7.0f),
		glm::vec3(17.0f,1.0f,7.0f),
		glm::vec3(18.0f,1.0f,7.0f),
		glm::vec3(0.0f,1.0f,8.0f),
		glm::vec3(1.0f,1.0f,8.0f),
		glm::vec3(2.0f,1.0f,8.0f),
		glm::vec3(3.0f,1.0f,8.0f),
		glm::vec3(4.0f,1.0f,8.0f),
		glm::vec3(5.0f,1.0f,8.0f),
		glm::vec3(6.0f,1.0f,8.0f),
		glm::vec3(7.0f,1.0f,8.0f),
		glm::vec3(8.0f,1.0f,8.0f),
		glm::vec3(9.0f,1.0f,8.0f),
		glm::vec3(10.0f,1.0f,8.0f),
		glm::vec3(11.0f,1.0f,8.0f),
		glm::vec3(12.0f,1.0f,8.0f),
		glm::vec3(13.0f,1.0f,8.0f),
		glm::vec3(14.0f,1.0f,8.0f),
		glm::vec3(15.0f,1.0f,8.0f),
		glm::vec3(16.0f,1.0f,8.0f),
		glm::vec3(17.0f,1.0f,8.0f),
		glm::vec3(18.0f,1.0f,8.0f),
		glm::vec3(0.0f,1.0f,9.0f),
		glm::vec3(1.0f,1.0f,9.0f),
		glm::vec3(2.0f,1.0f,9.0f),
		glm::vec3(3.0f,1.0f,9.0f),
		glm::vec3(4.0f,1.0f,9.0f),
		glm::vec3(5.0f,1.0f,9.0f),
		glm::vec3(6.0f,1.0f,9.0f),
		glm::vec3(7.0f,1.0f,9.0f),
		glm::vec3(8.0f,1.0f,9.0f),
		glm::vec3(9.0f,1.0f,9.0f),
		glm::vec3(10.0f,1.0f,9.0f),
		glm::vec3(11.0f,1.0f,9.0f),
		glm::vec3(12.0f,1.0f,9.0f),
		glm::vec3(13.0f,1.0f,9.0f),
		glm::vec3(14.0f,1.0f,9.0f),
		glm::vec3(15.0f,1.0f,9.0f),
		glm::vec3(16.0f,1.0f,9.0f),
		glm::vec3(17.0f,1.0f,9.0f),
		glm::vec3(18.0f,1.0f,9.0f),
		glm::vec3(0.0f,1.0f,10.0f),
		glm::vec3(1.0f,1.0f,10.0f),
		glm::vec3(2.0f,1.0f,10.0f),
		glm::vec3(3.0f,1.0f,10.0f),
		glm::vec3(4.0f,1.0f,10.0f),
		glm::vec3(5.0f,1.0f,10.0f),
		glm::vec3(6.0f,1.0f,10.0f),
		glm::vec3(7.0f,1.0f,10.0f),
		glm::vec3(8.0f,1.0f,10.0f),
		glm::vec3(9.0f,1.0f,10.0f),
		glm::vec3(10.0f,1.0f,10.0f),
		glm::vec3(11.0f,1.0f,10.0f),
		glm::vec3(12.0f,1.0f,10.0f),
		glm::vec3(13.0f,1.0f,10.0f),
		glm::vec3(14.0f,1.0f,10.0f),
		glm::vec3(15.0f,1.0f,10.0f),
		glm::vec3(16.0f,1.0f,10.0f),
		glm::vec3(17.0f,1.0f,10.0f),
		glm::vec3(18.0f,1.0f,10.0f),
		glm::vec3(0.0f,1.0f,11.0f),
		glm::vec3(1.0f,1.0f,11.0f),
		glm::vec3(2.0f,1.0f,11.0f),
		glm::vec3(3.0f,1.0f,11.0f),
		glm::vec3(4.0f,1.0f,11.0f),
		glm::vec3(5.0f,1.0f,11.0f),
		glm::vec3(6.0f,1.0f,11.0f),
		glm::vec3(7.0f,1.0f,11.0f),
		glm::vec3(8.0f,1.0f,11.0f),
		glm::vec3(9.0f,1.0f,11.0f),
		glm::vec3(10.0f,1.0f,11.0f),
		glm::vec3(11.0f,1.0f,11.0f),
		glm::vec3(12.0f,1.0f,11.0f),
		glm::vec3(13.0f,1.0f,11.0f),
		glm::vec3(14.0f,1.0f,11.0f),
		glm::vec3(15.0f,1.0f,11.0f),
		glm::vec3(16.0f,1.0f,11.0f),
		glm::vec3(17.0f,1.0f,11.0f),
		glm::vec3(18.0f,1.0f,11.0f),
		glm::vec3(0.0f,1.0f,12.0f),
		glm::vec3(1.0f,1.0f,12.0f),
		glm::vec3(2.0f,1.0f,12.0f),
		glm::vec3(3.0f,1.0f,12.0f),
		glm::vec3(4.0f,1.0f,12.0f),
		glm::vec3(5.0f,1.0f,12.0f),
		glm::vec3(6.0f,1.0f,12.0f),
		glm::vec3(7.0f,1.0f,12.0f),
		glm::vec3(8.0f,1.0f,12.0f),
		glm::vec3(9.0f,1.0f,12.0f),
		glm::vec3(10.0f,1.0f,12.0f),
		glm::vec3(11.0f,1.0f,12.0f),
		glm::vec3(12.0f,1.0f,12.0f),
		glm::vec3(13.0f,1.0f,12.0f),
		glm::vec3(14.0f,1.0f,12.0f),
		glm::vec3(15.0f,1.0f,12.0f),
		glm::vec3(16.0f,1.0f,12.0f),
		glm::vec3(17.0f,1.0f,12.0f),
		glm::vec3(18.0f,1.0f,12.0f),
		glm::vec3(0.0f,1.0f,13.0f),
		glm::vec3(1.0f,1.0f,13.0f),
		glm::vec3(2.0f,1.0f,13.0f),
		glm::vec3(3.0f,1.0f,13.0f),
		glm::vec3(4.0f,1.0f,13.0f),
		glm::vec3(5.0f,1.0f,13.0f),
		glm::vec3(6.0f,1.0f,13.0f),
		glm::vec3(7.0f,1.0f,13.0f),
		glm::vec3(8.0f,1.0f,13.0f),
		glm::vec3(9.0f,1.0f,13.0f),
		glm::vec3(10.0f,1.0f,13.0f),
		glm::vec3(11.0f,1.0f,13.0f),
		glm::vec3(12.0f,1.0f,13.0f),
		glm::vec3(13.0f,1.0f,13.0f),
		glm::vec3(14.0f,1.0f,13.0f),
		glm::vec3(15.0f,1.0f,13.0f),
		glm::vec3(16.0f,1.0f,13.0f),
		glm::vec3(17.0f,1.0f,13.0f),
		glm::vec3(18.0f,1.0f,13.0f),
		glm::vec3(0.0f,1.0f,14.0f),
		glm::vec3(1.0f,1.0f,14.0f),
		glm::vec3(2.0f,1.0f,14.0f),
		glm::vec3(3.0f,1.0f,14.0f),
		glm::vec3(4.0f,1.0f,14.0f),
		glm::vec3(5.0f,1.0f,14.0f),
		glm::vec3(6.0f,1.0f,14.0f),
		glm::vec3(7.0f,1.0f,14.0f),
		glm::vec3(8.0f,1.0f,14.0f),
		glm::vec3(9.0f,1.0f,14.0f),
		glm::vec3(10.0f,1.0f,14.0f),
		glm::vec3(11.0f,1.0f,14.0f),
		glm::vec3(12.0f,1.0f,14.0f),
		glm::vec3(13.0f,1.0f,14.0f),
		glm::vec3(14.0f,1.0f,14.0f),
		glm::vec3(15.0f,1.0f,14.0f),
		glm::vec3(16.0f,1.0f,14.0f),
		glm::vec3(17.0f,1.0f,14.0f),
		glm::vec3(18.0f,1.0f,14.0f),
		glm::vec3(0.0f,1.0f,15.0f),
		glm::vec3(1.0f,1.0f,15.0f),
		glm::vec3(2.0f,1.0f,15.0f),
		glm::vec3(3.0f,1.0f,15.0f),
		glm::vec3(4.0f,1.0f,15.0f),
		glm::vec3(5.0f,1.0f,15.0f),
		glm::vec3(6.0f,1.0f,15.0f),
		glm::vec3(7.0f,1.0f,15.0f),
		glm::vec3(8.0f,1.0f,15.0f),
		glm::vec3(9.0f,1.0f,15.0f),
		glm::vec3(10.0f,1.0f,15.0f),
		glm::vec3(11.0f,1.0f,15.0f),
		glm::vec3(12.0f,1.0f,15.0f),
		glm::vec3(13.0f,1.0f,15.0f),
		glm::vec3(14.0f,1.0f,15.0f),
		glm::vec3(15.0f,1.0f,15.0f),
		glm::vec3(16.0f,1.0f,15.0f),
		glm::vec3(17.0f,1.0f,15.0f),
		glm::vec3(18.0f,1.0f,15.0f),
		glm::vec3(0.0f,1.0f,16.0f),
		glm::vec3(1.0f,1.0f,16.0f),
		glm::vec3(2.0f,1.0f,16.0f),
		glm::vec3(3.0f,1.0f,16.0f),
		glm::vec3(4.0f,1.0f,16.0f),
		glm::vec3(5.0f,1.0f,16.0f),
		glm::vec3(6.0f,1.0f,16.0f),
		glm::vec3(7.0f,1.0f,16.0f),
		glm::vec3(8.0f,1.0f,16.0f),
		glm::vec3(9.0f,1.0f,16.0f),
		glm::vec3(10.0f,1.0f,16.0f),
		glm::vec3(11.0f,1.0f,16.0f),
		glm::vec3(12.0f,1.0f,16.0f),
		glm::vec3(13.0f,1.0f,16.0f),
		glm::vec3(14.0f,1.0f,16.0f),
		glm::vec3(15.0f,1.0f,16.0f),
		glm::vec3(16.0f,1.0f,16.0f),
		glm::vec3(17.0f,1.0f,16.0f),
		glm::vec3(18.0f,1.0f,16.0f),
		glm::vec3(0.0f,1.0f,17.0f),
		glm::vec3(1.0f,1.0f,17.0f),
		glm::vec3(2.0f,1.0f,17.0f),
		glm::vec3(3.0f,1.0f,17.0f),
		glm::vec3(4.0f,1.0f,17.0f),
		glm::vec3(5.0f,1.0f,17.0f),
		glm::vec3(6.0f,1.0f,17.0f),
		glm::vec3(7.0f,1.0f,17.0f),
		glm::vec3(8.0f,1.0f,17.0f),
		glm::vec3(9.0f,1.0f,17.0f),
		glm::vec3(10.0f,1.0f,17.0f),
		glm::vec3(11.0f,1.0f,17.0f),
		glm::vec3(12.0f,1.0f,17.0f),
		glm::vec3(13.0f,1.0f,17.0f),
		glm::vec3(14.0f,1.0f,17.0f),
		glm::vec3(15.0f,1.0f,17.0f),
		glm::vec3(16.0f,1.0f,17.0f),
		glm::vec3(17.0f,1.0f,17.0f),
		glm::vec3(18.0f,1.0f,17.0f),
		glm::vec3(0.0f,1.0f,18.0f),
		glm::vec3(1.0f,1.0f,18.0f),
		glm::vec3(2.0f,1.0f,18.0f),
		glm::vec3(3.0f,1.0f,18.0f),
		glm::vec3(4.0f,1.0f,18.0f),
		glm::vec3(5.0f,1.0f,18.0f),
		glm::vec3(6.0f,1.0f,18.0f),
		glm::vec3(7.0f,1.0f,18.0f),
		glm::vec3(8.0f,1.0f,18.0f),
		glm::vec3(9.0f,1.0f,18.0f),
		glm::vec3(10.0f,1.0f,18.0f),
		glm::vec3(11.0f,1.0f,18.0f),
		glm::vec3(12.0f,1.0f,18.0f),
		glm::vec3(13.0f,1.0f,18.0f),
		glm::vec3(14.0f,1.0f,18.0f),
		glm::vec3(15.0f,1.0f,18.0f),
		glm::vec3(16.0f,1.0f,18.0f),
		glm::vec3(17.0f,1.0f,18.0f),
		glm::vec3(18.0f,1.0f,18.0f),
	};
	//positon of wall
	glm::vec3 cubePositionsWall[]=
	{
		//second floor
		glm::vec3(0.0f, 2.0f, 0.0f),
		glm::vec3(1.0f, 2.0f, 0.0f),
		glm::vec3(2.0f, 2.0f, 0.0f),
		glm::vec3(3.0f, 2.0f, 0.0f),
		glm::vec3(4.0f, 2.0f, 0.0f),
		glm::vec3(5.0f, 2.0f, 0.0f),
		glm::vec3(6.0f, 2.0f, 0.0f),
		glm::vec3(7.0f, 2.0f, 0.0f),
		glm::vec3(8.0f, 2.0f, 0.0f),
		glm::vec3(9.0f, 2.0f, 0.0f),
		glm::vec3(10.0f, 2.0f, 0.0f),
		glm::vec3(11.0f, 2.0f, 0.0f),
		glm::vec3(12.0f, 2.0f, 0.0f),
		glm::vec3(13.0f, 2.0f, 0.0f),
		glm::vec3(14.0f, 2.0f, 0.0f),
		glm::vec3(15.0f, 2.0f, 0.0f),
		glm::vec3(16.0f, 2.0f, 0.0f),
		glm::vec3(17.0f, 2.0f, 0.0f),
		glm::vec3(18.0f, 2.0f, 0.0f),
		glm::vec3(0.0f, 2.0f, 1.0f),
		glm::vec3(1.0f, 2.0f, 1.0f),
		glm::vec3(2.0f, 2.0f, 1.0f),
		glm::vec3(3.0f, 2.0f, 1.0f),
		glm::vec3(4.0f, 2.0f, 1.0f),
		glm::vec3(5.0f, 2.0f, 1.0f),
		glm::vec3(9.0f, 2.0f, 1.0f),
		glm::vec3(10.0f, 2.0f, 1.0f),
		glm::vec3(15.0f, 2.0f, 1.0f),
		glm::vec3(16.0f, 2.0f, 1.0f),
		glm::vec3(18.0f, 2.0f, 1.0f),
		glm::vec3(0.0f, 2.0f, 2.0f),
		glm::vec3(5.0f, 2.0f, 2.0f),
		glm::vec3(6.0f, 2.0f, 2.0f),
		glm::vec3(7.0f, 2.0f, 2.0f),
		glm::vec3(9.0f, 2.0f, 2.0f),
		glm::vec3(10.0f, 2.0f, 2.0f),
		glm::vec3(11.0f, 2.0f, 2.0f),
		glm::vec3(13.0f, 2.0f, 2.0f),
		glm::vec3(15.0f, 2.0f, 2.0f),
		glm::vec3(18.0f, 2.0f, 2.0f),
		glm::vec3(0.0f, 2.0f, 3.0f),
		glm::vec3(2.0f, 2.0f, 3.0f),
		glm::vec3(3.0f, 2.0f, 3.0f),
		glm::vec3(10.0f, 2.0f, 3.0f),
		glm::vec3(11.0f, 2.0f, 3.0f),
		glm::vec3(13.0f, 2.0f, 3.0f),
		glm::vec3(15.0f, 2.0f, 3.0f),
		glm::vec3(17.0f, 2.0f, 3.0f),
		glm::vec3(18.0f, 2.0f, 3.0f),
		glm::vec3(0.0f, 2.0f, 4.0f),
		glm::vec3(2.0f, 2.0f, 4.0f),
		glm::vec3(6.0f, 2.0f, 4.0f),
		glm::vec3(7.0f, 2.0f, 4.0f),
		glm::vec3(8.0f, 2.0f, 4.0f),
		glm::vec3(11.0f, 2.0f, 4.0f),
		glm::vec3(13.0f, 2.0f, 4.0f),
		glm::vec3(15.0f, 2.0f, 4.0f),
		glm::vec3(18.0f, 2.0f, 4.0f),
		glm::vec3(0.0f, 2.0f, 5.0f),
		glm::vec3(2.0f, 2.0f, 5.0f),
		glm::vec3(4.0f, 2.0f, 5.0f),
		glm::vec3(5.0f, 2.0f, 5.0f),
		glm::vec3(6.0f, 2.0f, 5.0f),
		glm::vec3(7.0f, 2.0f, 5.0f),
		glm::vec3(8.0f, 2.0f, 5.0f),
		glm::vec3(9.0f, 2.0f, 5.0f),
		glm::vec3(13.0f, 2.0f, 5.0f),
		glm::vec3(15.0f, 2.0f, 5.0f),
		glm::vec3(16.0f, 2.0f, 5.0f),
		glm::vec3(18.0f, 2.0f, 5.0f),
		glm::vec3(0.0f, 2.0f, 6.0f),
		glm::vec3(2.0f, 2.0f, 6.0f),
		glm::vec3(9.0f, 2.0f, 6.0f),
		glm::vec3(10.0f, 2.0f, 6.0f),
		glm::vec3(11.0f, 2.0f, 6.0f),
		glm::vec3(12.0f, 2.0f, 6.0f),
		glm::vec3(13.0f, 2.0f, 6.0f),
		glm::vec3(16.0f, 2.0f, 6.0f),
		glm::vec3(18.0f, 2.0f, 6.0f),
		glm::vec3(0.0f, 2.0f, 7.0f),
		glm::vec3(2.0f, 2.0f, 7.0f),
		glm::vec3(3.0f, 2.0f, 7.0f),
		glm::vec3(4.0f, 2.0f, 7.0f),
		glm::vec3(6.0f, 2.0f, 7.0f),
		glm::vec3(7.0f, 2.0f, 7.0f),
		glm::vec3(9.0f, 2.0f, 7.0f),
		glm::vec3(10.0f, 2.0f, 7.0f),
		glm::vec3(11.0f, 2.0f, 7.0f),
		glm::vec3(12.0f, 2.0f, 7.0f),
		glm::vec3(13.0f, 2.0f, 7.0f),
		glm::vec3(14.0f, 2.0f, 7.0f),
		glm::vec3(16.0f, 2.0f, 7.0f),
		glm::vec3(18.0f, 2.0f, 7.0f),
		glm::vec3(0.0f, 2.0f, 8.0f),
		glm::vec3(3.0f, 2.0f, 8.0f),
		glm::vec3(4.0f, 2.0f, 8.0f),
		glm::vec3(9.0f, 2.0f, 8.0f),
		glm::vec3(16.0f, 2.0f, 8.0f),
		glm::vec3(17.0f, 2.0f, 8.0f),
		glm::vec3(18.0f, 2.0f, 8.0f),
		glm::vec3(0.0f, 2.0f, 9.0f),
		glm::vec3(1.0f, 2.0f, 9.0f),
		glm::vec3(2.0f, 2.0f, 9.0f),
		glm::vec3(3.0f, 2.0f, 9.0f),
		glm::vec3(6.0f, 2.0f, 9.0f),
		glm::vec3(7.0f, 2.0f, 9.0f),
		glm::vec3(8.0f, 2.0f, 9.0f),
		glm::vec3(9.0f, 2.0f, 9.0f),
		glm::vec3(11.0f, 2.0f, 9.0f),
		glm::vec3(12.0f, 2.0f, 9.0f),
		glm::vec3(13.0f, 2.0f, 9.0f),
		glm::vec3(14.0f, 2.0f, 9.0f),
		glm::vec3(15.0f, 2.0f, 9.0f),
		glm::vec3(16.0f, 2.0f, 9.0f),
		glm::vec3(18.0f, 2.0f, 9.0f),
		glm::vec3(0.0f, 2.0f, 10.0f),
		glm::vec3(5.0f, 2.0f, 10.0f),
		glm::vec3(6.0f, 2.0f, 10.0f),
		glm::vec3(7.0f, 2.0f, 10.0f),
		glm::vec3(11.0f, 2.0f, 10.0f),
		glm::vec3(12.0f, 2.0f, 10.0f),
		glm::vec3(13.0f, 2.0f, 10.0f),
		glm::vec3(18.0f, 2.0f, 10.0f),
		glm::vec3(0.0f, 2.0f, 11.0f),
		glm::vec3(1.0f, 2.0f, 11.0f),
		glm::vec3(2.0f, 2.0f, 11.0f),
		glm::vec3(4.0f, 2.0f, 11.0f),
		glm::vec3(5.0f, 2.0f, 11.0f),
		glm::vec3(9.0f, 2.0f, 11.0f),
		glm::vec3(10.0f, 2.0f, 11.0f),
		glm::vec3(11.0f, 2.0f, 11.0f),
		glm::vec3(12.0f, 2.0f, 11.0f),
		glm::vec3(13.0f, 2.0f, 11.0f),
		glm::vec3(15.0f, 2.0f, 11.0f),
		glm::vec3(16.0f, 2.0f, 11.0f),
		glm::vec3(18.0f, 2.0f, 11.0f),
		glm::vec3(0.0f, 2.0f, 12.0f),
		glm::vec3(5.0f, 2.0f, 12.0f),
		glm::vec3(6.0f, 2.0f, 12.0f),
		glm::vec3(7.0f, 2.0f, 12.0f),
		glm::vec3(9.0f, 2.0f, 12.0f),
		glm::vec3(15.0f, 2.0f, 12.0f),
		glm::vec3(16.0f, 2.0f, 12.0f),
		glm::vec3(18.0f, 2.0f, 12.0f),
		glm::vec3(0.0f, 2.0f, 13.0f),
		glm::vec3(2.0f, 2.0f, 13.0f),
		glm::vec3(3.0f, 2.0f, 13.0f),
		glm::vec3(6.0f, 2.0f, 13.0f),
		glm::vec3(7.0f, 2.0f, 13.0f),
		glm::vec3(9.0f, 2.0f, 13.0f),
		glm::vec3(11.0f, 2.0f, 13.0f),
		glm::vec3(12.0f, 2.0f, 13.0f),
		glm::vec3(13.0f, 2.0f, 13.0f),
		glm::vec3(14.0f, 2.0f, 13.0f),
		glm::vec3(15.0f, 2.0f, 13.0f),
		glm::vec3(18.0f, 2.0f, 13.0f),
		glm::vec3(0.0f, 2.0f, 14.0f),
		glm::vec3(3.0f, 2.0f, 14.0f),
		glm::vec3(4.0f, 2.0f, 14.0f),
		glm::vec3(6.0f, 2.0f, 14.0f),
		glm::vec3(7.0f, 2.0f, 14.0f),
		glm::vec3(9.0f, 2.0f, 14.0f),
		glm::vec3(12.0f, 2.0f, 14.0f),
		glm::vec3(17.0f, 2.0f, 14.0f),
		glm::vec3(18.0f, 2.0f, 14.0f),
		glm::vec3(0.0f, 2.0f, 15.0f),
		glm::vec3(1.0f, 2.0f, 15.0f),
		glm::vec3(2.0f, 2.0f, 15.0f),
		glm::vec3(3.0f, 2.0f, 15.0f),
		glm::vec3(6.0f, 2.0f, 15.0f),
		glm::vec3(7.0f, 2.0f, 15.0f),
		glm::vec3(9.0f, 2.0f, 15.0f),
		glm::vec3(10.0f, 2.0f, 15.0f),
		glm::vec3(12.0f, 2.0f, 15.0f),
		glm::vec3(13.0f, 2.0f, 15.0f),
		glm::vec3(14.0f, 2.0f, 15.0f),
		glm::vec3(15.0f, 2.0f, 15.0f),
		glm::vec3(17.0f, 2.0f, 15.0f),
		glm::vec3(18.0f, 2.0f, 15.0f),
		glm::vec3(0.0f, 2.0f, 16.0f),
		glm::vec3(5.0f, 2.0f, 16.0f),
		glm::vec3(6.0f, 2.0f, 16.0f),
		glm::vec3(12.0f, 2.0f, 16.0f),
		glm::vec3(13.0f, 2.0f, 16.0f),
		glm::vec3(14.0f, 2.0f, 16.0f),
		glm::vec3(17.0f, 2.0f, 16.0f),
		glm::vec3(18.0f, 2.0f, 16.0f),
		glm::vec3(0.0f, 2.0f, 17.0f),
		glm::vec3(2.0f, 2.0f, 17.0f),
		glm::vec3(3.0f, 2.0f, 17.0f),
		glm::vec3(4.0f, 2.0f, 17.0f),
		glm::vec3(5.0f, 2.0f, 17.0f),
		glm::vec3(8.0f, 2.0f, 17.0f),
		glm::vec3(9.0f, 2.0f, 17.0f),
		glm::vec3(11.0f, 2.0f, 17.0f),
		glm::vec3(12.0f, 2.0f, 17.0f),
		glm::vec3(16.0f, 2.0f, 17.0f),
		glm::vec3(17.0f, 2.0f, 17.0f),
		glm::vec3(18.0f, 2.0f, 17.0f),
		glm::vec3(0.0f, 2.0f, 18.0f),
		glm::vec3(2.0f, 2.0f, 18.0f),
		glm::vec3(3.0f, 2.0f, 18.0f),
		glm::vec3(4.0f, 2.0f, 18.0f),
		glm::vec3(5.0f, 2.0f, 18.0f),
		glm::vec3(7.0f, 2.0f, 18.0f),
		glm::vec3(8.0f, 2.0f, 18.0f),
		glm::vec3(9.0f, 2.0f, 18.0f),
		glm::vec3(10.0f, 2.0f, 18.0f),
		glm::vec3(11.0f, 2.0f, 18.0f),
		glm::vec3(12.0f, 2.0f, 18.0f),
		glm::vec3(13.0f, 2.0f, 18.0f),
		glm::vec3(14.0f, 2.0f, 18.0f),
		glm::vec3(15.0f, 2.0f, 18.0f),
		glm::vec3(16.0f, 2.0f, 18.0f),
		glm::vec3(17.0f, 2.0f, 18.0f),
		glm::vec3(18.0f, 2.0f, 18.0f),
		//third floor
		glm::vec3(0.0f, 3.0f, 0.0f),
		glm::vec3(1.0f, 3.0f, 0.0f),
		glm::vec3(2.0f, 3.0f, 0.0f),
		glm::vec3(3.0f, 3.0f, 0.0f),
		glm::vec3(4.0f, 3.0f, 0.0f),
		glm::vec3(5.0f, 3.0f, 0.0f),
		glm::vec3(6.0f, 3.0f, 0.0f),
		glm::vec3(7.0f, 3.0f, 0.0f),
		glm::vec3(8.0f, 3.0f, 0.0f),
		glm::vec3(9.0f, 3.0f, 0.0f),
		glm::vec3(10.0f, 3.0f, 0.0f),
		glm::vec3(11.0f, 3.0f, 0.0f),
		glm::vec3(12.0f, 3.0f, 0.0f),
		glm::vec3(13.0f, 3.0f, 0.0f),
		glm::vec3(14.0f, 3.0f, 0.0f),
		glm::vec3(15.0f, 3.0f, 0.0f),
		glm::vec3(16.0f, 3.0f, 0.0f),
		glm::vec3(17.0f, 3.0f, 0.0f),
		glm::vec3(18.0f, 3.0f, 0.0f),
		glm::vec3(0.0f, 3.0f, 1.0f),
		glm::vec3(1.0f, 3.0f, 1.0f),
		glm::vec3(2.0f, 3.0f, 1.0f),
		glm::vec3(3.0f, 3.0f, 1.0f),
		glm::vec3(4.0f, 3.0f, 1.0f),
		glm::vec3(5.0f, 3.0f, 1.0f),
		glm::vec3(9.0f, 3.0f, 1.0f),
		glm::vec3(10.0f, 3.0f, 1.0f),
		glm::vec3(15.0f, 3.0f, 1.0f),
		glm::vec3(16.0f, 3.0f, 1.0f),
		glm::vec3(18.0f, 3.0f, 1.0f),
		glm::vec3(0.0f, 3.0f, 2.0f),
		glm::vec3(5.0f, 3.0f, 2.0f),
		glm::vec3(6.0f, 3.0f, 2.0f),
		glm::vec3(7.0f, 3.0f, 2.0f),
		glm::vec3(9.0f, 3.0f, 2.0f),
		glm::vec3(10.0f, 3.0f, 2.0f),
		glm::vec3(11.0f, 3.0f, 2.0f),
		glm::vec3(13.0f, 3.0f, 2.0f),
		glm::vec3(15.0f, 3.0f, 2.0f),
		glm::vec3(18.0f, 3.0f, 2.0f),
		glm::vec3(0.0f, 3.0f, 3.0f),
		glm::vec3(2.0f, 3.0f, 3.0f),
		glm::vec3(3.0f, 3.0f, 3.0f),
		glm::vec3(10.0f, 3.0f, 3.0f),
		glm::vec3(11.0f, 3.0f, 3.0f),
		glm::vec3(13.0f, 3.0f, 3.0f),
		glm::vec3(15.0f, 3.0f, 3.0f),
		glm::vec3(17.0f, 3.0f, 3.0f),
		glm::vec3(18.0f, 3.0f, 3.0f),
		glm::vec3(0.0f, 3.0f, 4.0f),
		glm::vec3(2.0f, 3.0f, 4.0f),
		glm::vec3(6.0f, 3.0f, 4.0f),
		glm::vec3(7.0f, 3.0f, 4.0f),
		glm::vec3(8.0f, 3.0f, 4.0f),
		glm::vec3(11.0f, 3.0f, 4.0f),
		glm::vec3(13.0f, 3.0f, 4.0f),
		glm::vec3(15.0f, 3.0f, 4.0f),
		glm::vec3(18.0f, 3.0f, 4.0f),
		glm::vec3(0.0f, 3.0f, 5.0f),
		glm::vec3(2.0f, 3.0f, 5.0f),
		glm::vec3(4.0f, 3.0f, 5.0f),
		glm::vec3(5.0f, 3.0f, 5.0f),
		glm::vec3(6.0f, 3.0f, 5.0f),
		glm::vec3(7.0f, 3.0f, 5.0f),
		glm::vec3(8.0f, 3.0f, 5.0f),
		glm::vec3(9.0f, 3.0f, 5.0f),
		glm::vec3(13.0f, 3.0f, 5.0f),
		glm::vec3(15.0f, 3.0f, 5.0f),
		glm::vec3(16.0f, 3.0f, 5.0f),
		glm::vec3(18.0f, 3.0f, 5.0f),
		glm::vec3(0.0f, 3.0f, 6.0f),
		glm::vec3(2.0f, 3.0f, 6.0f),
		glm::vec3(9.0f, 3.0f, 6.0f),
		glm::vec3(10.0f, 3.0f, 6.0f),
		glm::vec3(11.0f, 3.0f, 6.0f),
		glm::vec3(12.0f, 3.0f, 6.0f),
		glm::vec3(13.0f, 3.0f, 6.0f),
		glm::vec3(16.0f, 3.0f, 6.0f),
		glm::vec3(18.0f, 3.0f, 6.0f),
		glm::vec3(0.0f, 3.0f, 7.0f),
		glm::vec3(2.0f, 3.0f, 7.0f),
		glm::vec3(3.0f, 3.0f, 7.0f),
		glm::vec3(4.0f, 3.0f, 7.0f),
		glm::vec3(6.0f, 3.0f, 7.0f),
		glm::vec3(7.0f, 3.0f, 7.0f),
		glm::vec3(9.0f, 3.0f, 7.0f),
		glm::vec3(10.0f, 3.0f, 7.0f),
		glm::vec3(11.0f, 3.0f, 7.0f),
		glm::vec3(12.0f, 3.0f, 7.0f),
		glm::vec3(13.0f, 3.0f, 7.0f),
		glm::vec3(14.0f, 3.0f, 7.0f),
		glm::vec3(16.0f, 3.0f, 7.0f),
		glm::vec3(18.0f, 3.0f, 7.0f),
		glm::vec3(0.0f, 3.0f, 8.0f),
		glm::vec3(3.0f, 3.0f, 8.0f),
		glm::vec3(4.0f, 3.0f, 8.0f),
		glm::vec3(9.0f, 3.0f, 8.0f),
		glm::vec3(16.0f, 3.0f, 8.0f),
		glm::vec3(17.0f, 3.0f, 8.0f),
		glm::vec3(18.0f, 3.0f, 8.0f),
		glm::vec3(0.0f, 3.0f, 9.0f),
		glm::vec3(1.0f, 3.0f, 9.0f),
		glm::vec3(2.0f, 3.0f, 9.0f),
		glm::vec3(3.0f, 3.0f, 9.0f),
		glm::vec3(6.0f, 3.0f, 9.0f),
		glm::vec3(7.0f, 3.0f, 9.0f),
		glm::vec3(8.0f, 3.0f, 9.0f),
		glm::vec3(9.0f, 3.0f, 9.0f),
		glm::vec3(11.0f, 3.0f, 9.0f),
		glm::vec3(12.0f, 3.0f, 9.0f),
		glm::vec3(13.0f, 3.0f, 9.0f),
		glm::vec3(14.0f, 3.0f, 9.0f),
		glm::vec3(15.0f, 3.0f, 9.0f),
		glm::vec3(16.0f, 3.0f, 9.0f),
		glm::vec3(18.0f, 3.0f, 9.0f),
		glm::vec3(0.0f, 3.0f, 10.0f),
		glm::vec3(5.0f, 3.0f, 10.0f),
		glm::vec3(6.0f, 3.0f, 10.0f),
		glm::vec3(7.0f, 3.0f, 10.0f),
		glm::vec3(11.0f, 3.0f, 10.0f),
		glm::vec3(12.0f, 3.0f, 10.0f),
		glm::vec3(13.0f, 3.0f, 10.0f),
		glm::vec3(18.0f, 3.0f, 10.0f),
		glm::vec3(0.0f, 3.0f, 11.0f),
		glm::vec3(1.0f, 3.0f, 11.0f),
		glm::vec3(2.0f, 3.0f, 11.0f),
		glm::vec3(4.0f, 3.0f, 11.0f),
		glm::vec3(5.0f, 3.0f, 11.0f),
		glm::vec3(9.0f, 3.0f, 11.0f),
		glm::vec3(10.0f, 3.0f, 11.0f),
		glm::vec3(11.0f, 3.0f, 11.0f),
		glm::vec3(12.0f, 3.0f, 11.0f),
		glm::vec3(13.0f, 3.0f, 11.0f),
		glm::vec3(15.0f, 3.0f, 11.0f),
		glm::vec3(16.0f, 3.0f, 11.0f),
		glm::vec3(18.0f, 3.0f, 11.0f),
		glm::vec3(0.0f, 3.0f, 12.0f),
		glm::vec3(5.0f, 3.0f, 12.0f),
		glm::vec3(6.0f, 3.0f, 12.0f),
		glm::vec3(7.0f, 3.0f, 12.0f),
		glm::vec3(9.0f, 3.0f, 12.0f),
		glm::vec3(15.0f, 3.0f, 12.0f),
		glm::vec3(16.0f, 3.0f, 12.0f),
		glm::vec3(18.0f, 3.0f, 12.0f),
		glm::vec3(0.0f, 3.0f, 13.0f),
		glm::vec3(2.0f, 3.0f, 13.0f),
		glm::vec3(3.0f, 3.0f, 13.0f),
		glm::vec3(6.0f, 3.0f, 13.0f),
		glm::vec3(7.0f, 3.0f, 13.0f),
		glm::vec3(9.0f, 3.0f, 13.0f),
		glm::vec3(11.0f, 3.0f, 13.0f),
		glm::vec3(12.0f, 3.0f, 13.0f),
		glm::vec3(13.0f, 3.0f, 13.0f),
		glm::vec3(14.0f, 3.0f, 13.0f),
		glm::vec3(15.0f, 3.0f, 13.0f),
		glm::vec3(18.0f, 3.0f, 13.0f),
		glm::vec3(0.0f, 3.0f, 14.0f),
		glm::vec3(3.0f, 3.0f, 14.0f),
		glm::vec3(4.0f, 3.0f, 14.0f),
		glm::vec3(6.0f, 3.0f, 14.0f),
		glm::vec3(7.0f, 3.0f, 14.0f),
		glm::vec3(9.0f, 3.0f, 14.0f),
		glm::vec3(12.0f, 3.0f, 14.0f),
		glm::vec3(17.0f, 3.0f, 14.0f),
		glm::vec3(18.0f, 3.0f, 14.0f),
		glm::vec3(0.0f, 3.0f, 15.0f),
		glm::vec3(1.0f, 3.0f, 15.0f),
		glm::vec3(2.0f, 3.0f, 15.0f),
		glm::vec3(3.0f, 3.0f, 15.0f),
		glm::vec3(6.0f, 3.0f, 15.0f),
		glm::vec3(7.0f, 3.0f, 15.0f),
		glm::vec3(9.0f, 3.0f, 15.0f),
		glm::vec3(10.0f, 3.0f, 15.0f),
		glm::vec3(12.0f, 3.0f, 15.0f),
		glm::vec3(13.0f, 3.0f, 15.0f),
		glm::vec3(14.0f, 3.0f, 15.0f),
		glm::vec3(15.0f, 3.0f, 15.0f),
		glm::vec3(17.0f, 3.0f, 15.0f),
		glm::vec3(18.0f, 3.0f, 15.0f),
		glm::vec3(0.0f, 3.0f, 16.0f),
		glm::vec3(5.0f, 3.0f, 16.0f),
		glm::vec3(6.0f, 3.0f, 16.0f),
		glm::vec3(12.0f, 3.0f, 16.0f),
		glm::vec3(13.0f, 3.0f, 16.0f),
		glm::vec3(14.0f, 3.0f, 16.0f),
		glm::vec3(17.0f, 3.0f, 16.0f),
		glm::vec3(18.0f, 3.0f, 16.0f),
		glm::vec3(0.0f, 3.0f, 17.0f),
		glm::vec3(2.0f, 3.0f, 17.0f),
		glm::vec3(3.0f, 3.0f, 17.0f),
		glm::vec3(4.0f, 3.0f, 17.0f),
		glm::vec3(5.0f, 3.0f, 17.0f),
		glm::vec3(8.0f, 3.0f, 17.0f),
		glm::vec3(9.0f, 3.0f, 17.0f),
		glm::vec3(11.0f, 3.0f, 17.0f),
		glm::vec3(12.0f, 3.0f, 17.0f),
		glm::vec3(16.0f, 3.0f, 17.0f),
		glm::vec3(17.0f, 3.0f, 17.0f),
		glm::vec3(18.0f, 3.0f, 17.0f),
		glm::vec3(0.0f, 3.0f, 18.0f),
		glm::vec3(2.0f, 3.0f, 18.0f),
		glm::vec3(3.0f, 3.0f, 18.0f),
		glm::vec3(4.0f, 3.0f, 18.0f),
		glm::vec3(5.0f, 3.0f, 18.0f),
		glm::vec3(7.0f, 3.0f, 18.0f),
		glm::vec3(8.0f, 3.0f, 18.0f),
		glm::vec3(9.0f, 3.0f, 18.0f),
		glm::vec3(10.0f, 3.0f, 18.0f),
		glm::vec3(11.0f, 3.0f, 18.0f),
		glm::vec3(12.0f, 3.0f, 18.0f),
		glm::vec3(13.0f, 3.0f, 18.0f),
		glm::vec3(14.0f, 3.0f, 18.0f),
		glm::vec3(15.0f, 3.0f, 18.0f),
		glm::vec3(16.0f, 3.0f, 18.0f),
		glm::vec3(17.0f, 3.0f, 18.0f),
		glm::vec3(18.0f, 3.0f, 18.0f)
	};
    // positions of the point lights
    glm::vec3 pointLightPositions[] = {
        glm::vec3( 0.0f,  4.0f,  0.0f),
        glm::vec3( 0.0f, 4.0f, 18.0f),
        glm::vec3(18.0f,  4.0f, 0.0f),
        glm::vec3( 18.0f,  4.0f, 18.0f),
		glm::vec3(0.0f,  3.0f, 19.0f),
		glm::vec3(2.0f,  3.0f, 19.0f),
		glm::vec3(5.0f,  3.0f, 19.0f),
		glm::vec3(7.0f,  3.0f, 19.0f),

    };
    // first, configure the cube's VAO (and VBO)
    unsigned int VBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindVertexArray(cubeVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    //Another cube VAO
    unsigned int cubeVAO2;
    glGenVertexArrays(1,&cubeVAO2);
    glBindVertexArray(cubeVAO2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    
    
    // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // load textures (we now use a utility function to keep the code more organized)
    // -----------------------------------------------------------------------------
    unsigned int diffuseMap = loadTexture("container2.png");
    unsigned int specularMap = loadTexture("container2_specular.png");
    unsigned int diffuseMap1 = loadTexture("container.jpg");
	unsigned int specularMap1 = loadTexture("container2_specular2.png");
	unsigned int diffuseMap2 = loadTexture("awesomeface.png");
	unsigned int specularMap2 = loadTexture("shabiface.png");

	
   
    
    // shader configuration
    // --------------------
    lightingShader.use();
    lightingShader.setInt("material.diffuse", 0);
    lightingShader.setInt("material.specular", 1);
    
    
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // input
        // -----
        processInput(window);
        
        // render
        // ------
        glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // be sure to activate shader when setting uniforms/drawing objects
        lightingShader.use();
        lightingShader.setVec3("viewPos", camera.Position);
        lightingShader.setFloat("material.shininess", 32.0f);
        
        /*
         Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index
         the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
         by defining light types as classes and set their values in there, or by using a more efficient uniform approach
         by using 'Uniform buffer objects', but that is something we'll discuss in the 'Advanced GLSL' tutorial.
         */
		// light model
		lightingShader.setInt("ambientSwitch", directLight);
		lightingShader.setInt("diffuseSwitch", pointLight);
		lightingShader.setInt("specularSwitch", spotLight);
        // directional light
        lightingShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        lightingShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        lightingShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
        // point light 1
        lightingShader.setVec3("pointLights[0].position", pointLightPositions[0]);
        lightingShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("pointLights[0].constant", 1.0f);
        lightingShader.setFloat("pointLights[0].linear", 0.09);
        lightingShader.setFloat("pointLights[0].quadratic", 0.032);
        // point light 2
        lightingShader.setVec3("pointLights[1].position", pointLightPositions[1]);
        lightingShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("pointLights[1].constant", 1.0f);
        lightingShader.setFloat("pointLights[1].linear", 0.09);
        lightingShader.setFloat("pointLights[1].quadratic", 0.032);
        // point light 3
        lightingShader.setVec3("pointLights[2].position", pointLightPositions[2]);
        lightingShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("pointLights[2].constant", 1.0f);
        lightingShader.setFloat("pointLights[2].linear", 0.09);
        lightingShader.setFloat("pointLights[2].quadratic", 0.032);
        // point light 4
        lightingShader.setVec3("pointLights[3].position", pointLightPositions[3]);
        lightingShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("pointLights[3].constant", 1.0f);
        lightingShader.setFloat("pointLights[3].linear", 0.09);
        lightingShader.setFloat("pointLights[3].quadratic", 0.032);
		// point light 5
		lightingShader.setVec3("pointLights[4].position", pointLightPositions[4]);
		lightingShader.setVec3("pointLights[4].ambient", 0.01f, 0.01f, 0.05f);
		lightingShader.setVec3("pointLights[4].diffuse", 0.1f, 0.1f, 0.8f);
		lightingShader.setVec3("pointLights[4].specular", 0.2f, 0.2f, 1.0f);
		lightingShader.setFloat("pointLights[4].constant", 1.0f);
		lightingShader.setFloat("pointLights[4].linear", 0.09);
		lightingShader.setFloat("pointLights[4].quadratic", 0.032);
		// point light 6
		lightingShader.setVec3("pointLights[5].position", pointLightPositions[5]);
		lightingShader.setVec3("pointLights[5].ambient", 0.01f, 0.01f, 0.05f);
		lightingShader.setVec3("pointLights[5].diffuse", 0.1f, 0.1f, 0.8f);
		lightingShader.setVec3("pointLights[5].specular", 0.2f, 0.2f, 1.0f);
		lightingShader.setFloat("pointLights[5].constant", 1.0f);
		lightingShader.setFloat("pointLights[5].linear", 0.09);
		lightingShader.setFloat("pointLights[5].quadratic", 0.032);
		// point light 7
		lightingShader.setVec3("pointLights[6].position", pointLightPositions[6]);
		lightingShader.setVec3("pointLights[6].ambient", 0.01f, 0.01f, 0.05f);
		lightingShader.setVec3("pointLights[6].diffuse", 0.1f, 0.1f, 0.8f);
		lightingShader.setVec3("pointLights[6].specular", 0.2f, 0.2f, 1.0f);
		lightingShader.setFloat("pointLights[6].constant", 1.0f);
		lightingShader.setFloat("pointLights[6].linear", 0.09);
		lightingShader.setFloat("pointLights[6].quadratic", 0.032);
		// point light 8
		lightingShader.setVec3("pointLights[7].position", pointLightPositions[7]);
		lightingShader.setVec3("pointLights[7].ambient", 0.01f, 0.01f, 0.05f);
		lightingShader.setVec3("pointLights[7].diffuse", 0.1f, 0.1f, 0.8f);
		lightingShader.setVec3("pointLights[7].specular", 0.2f, 0.2f, 1.0f);
		lightingShader.setFloat("pointLights[7].constant", 1.0f);
		lightingShader.setFloat("pointLights[7].linear", 0.09);
		lightingShader.setFloat("pointLights[7].quadratic", 0.032);
        // spotLight
        lightingShader.setVec3("spotLight.position", camera.Position);
        lightingShader.setVec3("spotLight.direction", camera.Front);
        lightingShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        lightingShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("spotLight.constant", 1.0f);
        lightingShader.setFloat("spotLight.linear", 0.09);
        lightingShader.setFloat("spotLight.quadratic", 0.032);
        lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
        
        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);
        
        // world transformation
        glm::mat4 model;
        lightingShader.setMat4("model", model);
        
        // bind diffuse map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap1);
        // bind specular map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);
        
        // render containers
        glBindVertexArray(cubeVAO);
        for (unsigned int i = 1; i < 9; i++)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model;
			float radius = 15.0f;
			if (i == 2||i==3||i==4)
			{
				
				model = glm::translate(model, glm::vec3(9.0f + radius * cos(glfwGetTime()+20.0f*i), 5.0f, 9.0f + radius * sin(glfwGetTime()+20.0f*i)));
			}
			else if (i == 5||i==6)
			{
				float phi = glfwGetTime()+15.0f*i;
				float theta = 45.0f;
				model = glm::translate(model, glm::vec3(radius*sin(phi)*sin(theta)+9.0f, radius*sin(phi)*cos(theta)+1.5f, radius*cos(phi)+9.0f));
			}
			else if (i == 7 || i == 8)
			{
				float phi = glfwGetTime() + 15.0f*i;
				float theta = -45.0f;
				model = glm::translate(model, glm::vec3(radius*sin(phi)*sin(theta) + 9.0f, radius*sin(phi)*cos(theta) + 1.5f, radius*cos(phi) + 9.0f));
			}
			else
				model = glm::translate(model, glm::vec3(9.0f,10.0f,9.0f));
			float angle = 20.0f * i;
			if (i==1)  // every 3rd iteration (including the first) we set the angle using GLFW's time function.
				angle = glfwGetTime() * 75.0f;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            lightingShader.setMat4("model", model);
            
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        
        
        //draw another cubes
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, specularMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);
        
        glBindVertexArray(cubeVAO2);
        for (unsigned int i = 0; i < 361; i++)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model;
            model = glm::translate(model, cubePositionsFloor[i]);
            //float angle = 25.0f * i;
            //model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.7f, 0.5f));
            lightingShader.setMat4("model", model);
            
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

		//second floor
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, specularMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);
		glBindVertexArray(cubeVAO2);
		for (unsigned int i = 0; i < 434; i++)
		{
			// calculate the model matrix for each object and pass it to shader before drawing
			glm::mat4 model;
			model = glm::translate(model, cubePositionsWall[i]);
			//float angle = 25.0f * i;
			//model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.7f, 0.5f));
			lightingShader.setMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

       
        // also draw the lamp object(s)
		if (pointLight)
		{
			lampShader.use();
			lampShader.setMat4("projection", projection);
			lampShader.setMat4("view", view);

			// we now draw as many light bulbs as we have point lights.
			glBindVertexArray(lightVAO);
			for (unsigned int i = 0; i < 8; i++)
			{
				model = glm::mat4();
				model = glm::translate(model, pointLightPositions[i]);
				model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
				lampShader.setMat4("model", model);
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
		}
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);
    
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

	//light control(3 model)
	if (glfwGetKey(window, GLFW_KEY_KP_1) == GLFW_PRESS)
		directLight = 1;
	if (glfwGetKey(window, GLFW_KEY_KP_2) == GLFW_PRESS)
		pointLight = 1;
	if (glfwGetKey(window, GLFW_KEY_KP_3) == GLFW_PRESS)
		spotLight = 1;
	if (glfwGetKey(window, GLFW_KEY_KP_4) == GLFW_PRESS)
		directLight = 0;
	if (glfwGetKey(window, GLFW_KEY_KP_5) == GLFW_PRESS)
		pointLight = 0;
	if (glfwGetKey(window, GLFW_KEY_KP_6) == GLFW_PRESS)
		spotLight = 0;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    
    lastX = xpos;
    lastY = ypos;
    
    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
	glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;
        
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }
    
    return textureID;
}
