#include <glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include "camera.h"
#include <iostream>
#include <string>

using namespace glm;
using namespace std;

Camera::Camera(GLFWwindow* window) : window(window)
{
    position = glm::vec3(0, 0, 1);
    horizontalAngle = 3.14f;
    verticalAngle = 0.0f;
    FoV = 45.0f;
    speed = 8.0f;
    mouseSpeed = 0.0005f;
    fovSpeed = 2.0f;
}

void Camera::update()
{
    // glfwGetTime is called only once, the first time this function is called
    static double lastTime = glfwGetTime();

    // Compute time difference between current and last frame
    double currentTime = glfwGetTime();
    float deltaTime = float(currentTime - lastTime);

    // Get mouse position
    double xPos, yPos;
    glfwGetCursorPos(window, &xPos, &yPos);

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    // Reset mouse position for next frame
    glfwSetCursorPos(window, width / 2, height / 2);

    // Task 5.3: Compute new horizontal and vertical angles, given windows size
    //*/
    // and cursor position
    horizontalAngle += mouseSpeed * (width/2 - xPos);
    verticalAngle += mouseSpeed * (height/2 - yPos);

    // Task 5.4: right and up vectors of the camera coordinate system
    // use spherical coordinates
    vec3 direction(
        cos(verticalAngle) * sin(horizontalAngle),
        sin(verticalAngle),
        cos(verticalAngle) * cos(horizontalAngle)
    );

    // Right vector
    vec3 right(
        -cos(horizontalAngle),
        0,
        sin(horizontalAngle)
    );

    // Up vector
    vec3 up = cross(right, direction);
    
    if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS) {
        speed += 0.1f;
    }
    
    if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS) {
        speed -= 0.1f;
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        position += direction * speed * deltaTime;
    }
    // Move backward
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        position -= direction * speed * deltaTime;
    }
    // Strafe right
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        position += right * speed * deltaTime;
    }
    // Strafe left
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        position -= right * speed * deltaTime;
    }

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        position += up * deltaTime * speed;
    }
    // Strafe left
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        position -= up * deltaTime * speed;
    }
    

    // Task 5.6: handle zoom in/out effects
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        FoV -= fovSpeed;
        if (FoV < 1.0f)
            FoV = 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        FoV += fovSpeed;
        if (FoV > 45.0f)
            FoV = 45.0f;
    }

    // Task 5.7: construct projection and view matrices
    projectionMatrix = perspective(radians(FoV), 4.0f/3.0f, 0.1f, 100.0f);
    viewMatrix = lookAt(
        position,
        position + direction,
        up
    );
    //*/

    // For the next frame, the "last time" will be "now"
    lastTime = currentTime;
}
glm::vec3 Camera::info(){
    return position;
}
