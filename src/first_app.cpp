#include "First_app.hpp"

#include "simple_render_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm.hpp"
#include "gtc/constants.hpp"

// std
#include <stdexcept>
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

        while (!lveWindow.ShouldClose())
        {
            glfwPollEvents();

            if (auto commandBuffer = lveRenderer.BeginFrame())
            {
                lveRenderer.BeginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.RenderGameObjects(commandBuffer, gameObjects);
                lveRenderer.EndSwapChainRenderPass(commandBuffer);
                lveRenderer.EndFrame();
            }
        }

        vkDeviceWaitIdle(lveDevice.device());
    }

    void FirstApp::LoadGameObjects()
    {
        std::vector<LveModel::Vertex> vertices{
            {{0.0, -0.5}, {1.0f, 0.0f, 0.0f}},
            {{0.5, 0.5}, {0.0f, 1.0f, 0.0f}},
            {{-0.5, 0.5}, {0.0f, 0.0f, 1.0f}}
        };

        auto lveModel = std::make_shared<LveModel>(lveDevice, vertices);

        auto triangle = LveGameObject::CreateGameObject();
        triangle.model = lveModel;
        triangle.color = {.1f, .8f, .1f};
        triangle.transform2d.translation.x = .2f;
        triangle.transform2d.scale = {2.f, .5f};
        triangle.transform2d.rotation = .25f * glm::two_pi<float>();

        gameObjects.push_back(std::move(triangle));
    }
}