#include "fs.window.hpp"

// std
#include <stdexcept>

namespace fs
{
    FsWindow::FsWindow(int _w, int _h, std::string _windowName) : width(_w), height(_h), windowName(_windowName)
    {
        InitWindow();
    }

    FsWindow::~FsWindow()
    {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void FsWindow::InitWindow()
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, FramebufferResizeCallback);
    }

    void FsWindow::CreateWindowSurface(VkInstance instance, VkSurfaceKHR *surface)
    {
        if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create window surface");
        }
    }

    void FsWindow::FramebufferResizeCallback(GLFWwindow *window, int width, int height)
    {
        auto newWindow = reinterpret_cast<FsWindow *>(glfwGetWindowUserPointer(window));
        newWindow->framebufferResized = true;
        newWindow->width = width;
        newWindow->height = height;
    }

}