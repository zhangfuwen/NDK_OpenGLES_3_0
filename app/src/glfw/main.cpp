//#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "Components/Renderer/PointRenderer.h"
#include "Components/Renderer/TexturedMeshRenderer.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
//    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
//    {
//        std::cout << "Failed to initialize GLAD" << std::endl;
//        return -1;
//    }

    // render loop
    // -----------
    auto pointRenderer = new PointRenderer();
    pointRenderer->Init();
    pointRenderer->LoadPoints([&](std::vector<Point> &points, int & numElements) -> int {
        points.push_back(Point{glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), 30.0f});
        points.push_back(Point{glm::vec3{1.0f, 1.0f, 0.0f}, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), 30.0f});
        points.push_back(Point{glm::vec3{-1.0f, -1.0f, 0.0f}, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), 30.0f});
        numElements = points.size();
        return points.size();
    });
    ObjLoader *objLoader = new ObjLoader();
    objLoader->LoadObjFile("/home/zhangfuwen/Code/NDK_OpenGLES_3_0/app/src/main/assets/poly/Apricot_02_hi_poly.obj");
    auto renderer = new TexturedMeshRenderer();
    renderer->Init();
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        auto transform = new Transform();
        auto camera = new Camera();
        std::vector<Light> lights;
        pointRenderer->Draw(*transform, *camera, lights);
        renderer->Draw(*transform, *camera, lights);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
