#pragma once

#include "ECS/components/fs.components.hpp"
#include "fs.window.hpp"

namespace FS
{
    class KeyboardMovementController
    {
        public:
            struct KeyMappings 
            {
                int moveLeft = GLFW_KEY_A;
                int moveRight = GLFW_KEY_D;
                int moveForward = GLFW_KEY_W;
                int moveBackward = GLFW_KEY_S;
                int lookLeft = GLFW_KEY_LEFT;
                int lookRight = GLFW_KEY_RIGHT;
                int lookUp = GLFW_KEY_UP;
                int lookDown = GLFW_KEY_DOWN;
            };

            KeyMappings keys{};
            float moveSpeed{3.f};
            float lookSpeed{1.5f};

            void MoveInPlaneXZ(GLFWwindow *window, float deltaTime, Transform &transform);
    };
}