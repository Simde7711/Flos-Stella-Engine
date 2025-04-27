#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

namespace lve
{
    class LveWindow
    {
        public:
            LveWindow(int w, int h, std::string name);
            ~LveWindow();

            LveWindow(const LveWindow &) = delete;
            LveWindow &operator = (const LveWindow &) = delete;

            bool ShouldClose(){ return glfwWindowShouldClose(window);}
            VkExtent2D GetExtent() { return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};}
            void CreateWindowSurface(VkInstance instance, VkSurfaceKHR *surface);
        private:
            GLFWwindow *window;
            const int width;
            const int height;
            std::string windowName;

            void InitWindow();
    };
}