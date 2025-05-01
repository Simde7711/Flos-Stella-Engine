#include "First_app.hpp"

#include "keyboard_movement_controller.hpp"
#include "simple_render_system.hpp"
#include "lve_camera.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm.hpp"
#include "gtc/constants.hpp"

// std
#include <stdexcept>
#include <chrono>
#include <array>
#include <cassert>

namespace lve
{

    FirstApp::FirstApp()
    {
        LoadGameObjects();
    }

    FirstApp::~FirstApp()
    {
        
    }

    void FirstApp::run()
    {
        SimpleRenderSystem simpleRenderSystem{lveDevice, lveRenderer.GetSwapChainRenderPass()};
        LveCamera camera{}; 
        // camera.SetViewDirection(glm::vec3(0.f), glm::vec3(0.5f, 0.f, 1.f));
        camera.SetViewTarget(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5f));

        auto viewerObject = LveGameObject::CreateGameObject();
        KeyboardMovementController cameraController{};

        auto currentTime = std::chrono::high_resolution_clock::now();

        while (!lveWindow.ShouldClose())
        {
            glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            cameraController.MoveInPlaneXZ(lveWindow.GetGLFWwindow(), frameTime, viewerObject);
            camera.SetViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

            float aspect = lveRenderer.GetAspectRatio();
            // camera.SetOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
            camera.SetPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 80.f);

            if (auto commandBuffer = lveRenderer.BeginFrame())
            {
                lveRenderer.BeginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.RenderGameObjects(commandBuffer, gameObjects, camera);
                lveRenderer.EndSwapChainRenderPass(commandBuffer);
                lveRenderer.EndFrame();
            }
        }

        vkDeviceWaitIdle(lveDevice.device());
    }

    void FirstApp::LoadGameObjects()
    {
        std::shared_ptr<LveModel> lveModel = LveModel::CreateModelFromFile(lveDevice, "C:\\Users\\simde\\RepositoryGit\\vulkan\\models\\flat_vase.obj");
        auto flatVase = LveGameObject::CreateGameObject();
        flatVase.model = lveModel;
        flatVase.transform.translation = {-.5f, .5f, 2.5f};
        flatVase.transform.rotation.z = 0.f;
        flatVase.transform.scale = {3.f, 1.5, 3.f};
        gameObjects.push_back(std::move(flatVase));

        lveModel = LveModel::CreateModelFromFile(lveDevice, "C:\\Users\\simde\\RepositoryGit\\vulkan\\models\\smooth_vase.obj");
        auto smoothVase = LveGameObject::CreateGameObject();
        smoothVase.model = lveModel;
        smoothVase.transform.translation = {.5f, .5f, 2.5f};
        smoothVase.transform.rotation.z = 0.f;
        smoothVase.transform.scale = glm::vec3(3.f);
        gameObjects.push_back(std::move(smoothVase));
    }
}