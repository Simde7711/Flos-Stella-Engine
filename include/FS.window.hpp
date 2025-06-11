#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

namespace FS
{
    class FsWindow
    {
        public:
            FsWindow(int w, int h, std::string name);
            ~FsWindow();

            FsWindow(const FsWindow &) = delete;
            FsWindow &operator = (const FsWindow &) = delete;

            bool ShouldClose(){ return glfwWindowShouldClose(window);}
            VkExtent2D GetExtent() { return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};}
            bool WasWindowResized() { return framebufferResized; }
            void ResetWindowResizedFlag() { framebufferResized = false; }
            void CreateWindowSurface(VkInstance instance, VkSurfaceKHR *surface);
            GLFWwindow *GetGLFWwindow() const { return window; };    
        private:
            int width;
            int height;
            bool framebufferResized = false;

            GLFWwindow *window;
            std::string windowName;

            void InitWindow();
            static void FramebufferResizeCallback(GLFWwindow *window, int width, int height);
    };
}