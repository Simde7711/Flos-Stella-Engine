#pragma once

// #define GLFW_INCLUDE_VULKAN
#include "libs/glfw-3.4.bin.WIN64/include/GLFW/glfw3.h"
#include <string>

namespace lve
{
    class LveWindow
    {
        public:
            LveWindow(int w, int h, std::string name);
            ~LveWindow();
        private:
            GLFWwindow *window;
            const int width;
            const int height;
            std::string windowName;

            void InitWindow();
    };
}