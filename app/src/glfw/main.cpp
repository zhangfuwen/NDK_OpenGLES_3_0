//#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "Components/Renderer/PointRenderer.h"
#include "Components/Renderer/TexturedMeshRenderer.h"
#include "System/Scene.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float x = 0, y = 0, z = 0.8;
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    FUN_INFO("scroll %f, %f", xoffset, yoffset );
    z -= yoffset*0.005;
}
double oldX, oldY;
double dragX = 0, dragY = 0;
bool lbutton_down = false;
bool rbutton_down = false;

void mouse_cursor_callback( GLFWwindow * window, double xpos, double ypos)
{

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
    {
        return;
    }
    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        dragX = xpos - oldX;
        dragY = ypos - oldY;
        FUN_INFO("drag %f %f", dragX, dragY);
    }

}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_W && action == GLFW_PRESS) {
        z -= 0.001;

    }
    if (key == GLFW_KEY_S& action == GLFW_PRESS) {
        z += 0.001;

    }
    if (key == GLFW_KEY_A && action == GLFW_PRESS) {

    }
    if (key == GLFW_KEY_D && action == GLFW_PRESS) {

    }
}

static void mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if(GLFW_PRESS == action) {
            lbutton_down = true;
            glfwGetCursorPos(window, &oldX, &oldY);
            FUN_INFO("lbutton_down, %f, %f", oldX, oldY);
        }
        else if(GLFW_RELEASE == action) {
            oldX = 0.0f;
            oldY = 0.0f;
            FUN_INFO("lbutton_up, %f, %f", oldX, oldY);
            lbutton_down = false;
            x += dragX;
            y +=  dragY;

        }
    }
}

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
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetCursorPosCallback(window, mouse_cursor_callback);
    glfwSetMouseButtonCallback(window, mouse_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
//    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
//    {
//        std::cout << "Failed to initialize GLAD" << std::endl;
//        return -1;
//    }

    // render loop
    // -----------
    auto scene = std::make_unique<Scene>();
    scene->Init();
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
        static float rotX = 0.0f;
        rotX += 10;
//        if(lbutton_down) {
//            scene->CameraMove((x +dragX)* 0.005 *z, (y+dragY) * 0.005 *z, z);
//        } else {
//            scene->CameraMove(x*0.005 * z,y * 0.005 *z,z);
//        }
//        scene->CameraRotate(0, rotX, 0);
        scene->UpdateTransformMatrix(0.0f, 0.0f, 0.2f, 0.2f);
        scene->Draw();

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
