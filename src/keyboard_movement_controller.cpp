#include "keyboard_movement_controller.hpp"
#include <glm/gtc/quaternion.hpp>

namespace fs
{
    void KeyboardMovementController::MoveInPlaneXZ(GLFWwindow *window, float deltaTime, Transform &transform)
    {
        glm::vec3 rotate{0.f};
        if (glfwGetKey(window, keys.lookRight) == GLFW_PRESS)
        {
            rotate.y += 1.f;
        }
        if (glfwGetKey(window, keys.lookLeft) == GLFW_PRESS)
        {
            rotate.y -= 1.f;
        }
        if (glfwGetKey(window, keys.lookUp) == GLFW_PRESS)
        {
            rotate.x += 1.f;
        }
        if (glfwGetKey(window, keys.lookDown) == GLFW_PRESS)
        {
            rotate.x -= 1.f;
        }

        if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon())
        {
            transform.rotation += lookSpeed * deltaTime * glm::normalize(rotate);
        }

        transform.rotation.x = glm::clamp(transform.rotation.x, -1.5f, 1.5f);
        transform.rotation.y = glm::mod(transform.rotation.y, glm::two_pi<float>());

        // Base local directions
        const glm::vec3 forwardDir = glm::vec3(0.f, 0.f, 1.f);
        const glm::vec3 rightDir   = glm::vec3(1.f, 0.f,  0.f);
        const glm::vec3 upDir      = glm::vec3(0.f, 1.f,  0.f);

        glm::vec3 moveDir{0.f};
        if (glfwGetKey(window, keys.moveForward) == GLFW_PRESS)
        {
            moveDir += forwardDir;
        }
        if (glfwGetKey(window, keys.moveBackward) == GLFW_PRESS)
        {
            moveDir -= forwardDir;
        }
        if (glfwGetKey(window, keys.moveRight) == GLFW_PRESS)
        {
            moveDir += rightDir;
        }
        if (glfwGetKey(window, keys.moveLeft) == GLFW_PRESS)
        {
            moveDir -= rightDir;
        }

        if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon())
        {
            moveDir = glm::normalize(moveDir);

            glm::quat rotQuat = glm::quat(transform.rotation); // if in radians
            glm::vec3 worldDir = rotQuat * moveDir;

            transform.translation += moveSpeed * deltaTime * worldDir;
        }
    }
}