#include "FS.app.hpp"

#include "keyboard_movement_controller.hpp"
#include "systems/renderSystems/FS.meshRenderSystem.hpp"
#include "systems/componentSystems/FS.pointLightSystem.hpp"
#include "FS.camera.hpp"
#include "FS.buffer.hpp"
#include "FS.frameInfo.hpp"
#include "FS.renderPassManager.hpp"
#include "FS.shaderManager.hpp"

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
        // entity component system
        gCoordinator.Init();
        
        // TODO: temporaire pour faire marcher le init
        globalSetLayout = LveDescriptorSetLayout::Builder(lveDevice)   
            .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
            .build();
        
        // shader manager
        shaderManager.Init(&lveDevice, &lveRenderer, globalSetLayout->getDescriptorSetLayout());

        globalPool = LveDescriptorPool::Builder(lveDevice)
            .setMaxSets(LveSwapChain::MAX_FRAMES_IN_FLIGHT)
            .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, LveSwapChain::MAX_FRAMES_IN_FLIGHT)
            .build();

        LoadGameObjects();
    }

    FirstApp::~FirstApp()
    {
        
    }

    void FirstApp::run()
    {
        int modelCount = 0;
        for (Entity entity : gCoordinator.mEntities) 
        {
            if (gCoordinator.HasComponent<Model>(entity)) {
                // std::cout << "[DEBUG] Entity " << entity << " has Model component\n";
                ++modelCount;
            }
        }
        // std::cout << "[DEBUG] Total Model components: " << modelCount << "\n";

        std::vector<std::unique_ptr<LveBuffer>> uboBuffers(LveSwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < uboBuffers.size(); i++)
        {
            uboBuffers[i] = std::make_unique<LveBuffer>
            (
                lveDevice,
                sizeof(GlobalUbo),
                1,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
            );
            uboBuffers[i]->map();
        }

        // TODO: TEMPORAIRE
        // globalSetLayout = LveDescriptorSetLayout::Builder(lveDevice)   
        //     .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
        //     .build();

        std::vector<VkDescriptorSet> globalDescriptorSets(LveSwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < globalDescriptorSets.size(); i++)
        {
            auto bufferInfo = uboBuffers[i]->descriptorInfo();
            LveDescriptorWriter(*globalSetLayout, *globalPool)
                .writeBuffer(0, &bufferInfo)
                .build(globalDescriptorSets[i]);
        }
        
        // render systems
        SimpleRenderSystem simpleRenderSystem{lveDevice};
       
        // component system
        PointLightSystem pointLightSystem{};
        
        LveCamera camera{}; 
        // camera.SetViewDirection(glm::vec3(0.f), glm::vec3(0.5f, 0.f, 1.f));
        camera.SetViewTarget(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5f));

        // créer l'entity de la caméra
        Entity entityCamera = gCoordinator.CreateEntity();
        gCoordinator.GetComponent<Transform>(entityCamera).translation.z = -2.5f;

        KeyboardMovementController cameraController{};

        auto currentTime = std::chrono::high_resolution_clock::now();

        while (!lveWindow.ShouldClose())
        {
            glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            cameraController.MoveInPlaneXZ(lveWindow.GetGLFWwindow(), frameTime, gCoordinator.GetComponent<Transform>(entityCamera));
            camera.SetViewYXZ(gCoordinator.GetComponent<Transform>(entityCamera).translation, gCoordinator.GetComponent<Transform>(entityCamera).rotation);

            float aspect = lveRenderer.GetAspectRatio();
            // camera.SetOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
            camera.SetPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 80.f);

            if (auto commandBuffer = lveRenderer.BeginFrame())
            {
                int frameIndex = lveRenderer.GetFrameIndex();
                FrameInfo frameInfo
                {
                    frameIndex,
                    frameTime,
                    commandBuffer,
                    camera,
                    globalDescriptorSets[frameIndex]
                };

                // update
                GlobalUbo ubo{};
                ubo.projection = camera.GetProjection();
                ubo.view = camera.GetView();
                ubo.inverseView = camera.GetInverseView();
                pointLightSystem.Update(frameInfo, ubo);
                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();

                // render
                lveRenderer.BeginSwapChainRenderPass(commandBuffer);

                // order here matters
                simpleRenderSystem.RenderGameObjects(frameInfo);

                lveRenderer.EndSwapChainRenderPass(commandBuffer);
                lveRenderer.EndFrame();
            }
        }

        vkDeviceWaitIdle(lveDevice.device());
        
        // final cleanup
        shaderManager.Cleanup();
        gCoordinator.DestroyAllEntity();
    }

    void FirstApp::LoadGameObjects()
    {   
        {
            // entity
            Entity flatVase = gCoordinator.CreateEntity();
            
            // ajout du model
            gCoordinator.AddComponent(flatVase, Model{LveModel::CreateModelFromFile(lveDevice, "../models/flat_vase.obj")});

            // set le transform
            Transform &transform1 = gCoordinator.GetComponent<Transform>(flatVase);
            transform1.translation = {-.5f, .5f, 0.f};
            transform1.rotation.z = 0.f;
            transform1.scale = {3.f, 1.5, 3.f};

            // set le shader
            PipelineKey pipelineKey1{};
            pipelineKey1.vertShaderPath = "../shaders/simple_shader.vert.spv";
            pipelineKey1.fragShaderPath = "../shaders/simple_shader.frag.spv";
            PipelineConfigKey pipelineConfigKey1{};
            pipelineKey1.config = pipelineConfigKey1;
            auto key1 = shaderManager.GetOrCreatePipelineKey(pipelineKey1);
            gCoordinator.AddComponent(flatVase, Shader{key1});
        }

        {
            // entity
            Entity smoothVase = gCoordinator.CreateEntity();
            
            // ajout du model
            gCoordinator.AddComponent(smoothVase, Model{LveModel::CreateModelFromFile(lveDevice, "../models/smooth_vase.obj")});

            // set le transform
            Transform &transform2 = gCoordinator.GetComponent<Transform>(smoothVase);
            transform2.translation = {.5f, .5f, 0.f};
            transform2.rotation.z = 0.f;
            transform2.scale = glm::vec3(3.f);
        
            // set le shader
            PipelineKey pipelineKey2{};
            pipelineKey2.vertShaderPath = "../shaders/simple_shader.vert.spv";
            pipelineKey2.fragShaderPath = "../shaders/simple_shader.frag.spv";
            PipelineConfigKey pipelineConfigKey2{};
            pipelineKey2.config = pipelineConfigKey2;
            auto key2 = shaderManager.GetOrCreatePipelineKey(pipelineKey2);
            gCoordinator.AddComponent(smoothVase, Shader{key2});
        }

        {
            // entity
            Entity floor = gCoordinator.CreateEntity();
            
            // ajout du model
            gCoordinator.AddComponent(floor, Model{LveModel::CreateModelFromFile(lveDevice, "../models/quad.obj")});

            // set le transform
            Transform &transform3 = gCoordinator.GetComponent<Transform>(floor);
            transform3.translation = {0.f, .5f, 0.f};
            transform3.scale = {3.f, 1.f, 3.f};

            // set le shader
            PipelineKey pipelineKey3{};
            pipelineKey3.vertShaderPath = "../shaders/simple_shader.vert.spv";
            pipelineKey3.fragShaderPath = "../shaders/simple_shader.frag.spv";
            PipelineConfigKey pipelineConfigKey3{};
            pipelineKey3.config = pipelineConfigKey3;
            auto key3 = shaderManager.GetOrCreatePipelineKey(pipelineKey3);
            gCoordinator.AddComponent(floor, Shader{key3});
        }
        
        std::vector<glm::vec3> lightColors{
            {1.f, .1f, .1f},
            {.1f, .1f, 1.f},
            {.1f, 1.f, .1f},
            {1.f, 1.f, .1f},
            {.1f, 1.f, 1.f},
            {1.f, 1.f, 1.f}  
        };

        for (int i = 0; i < lightColors.size(); i++)
        {
            // crée l'entity et ajoute le pointLight component
            Entity pointLightEntity = gCoordinator.CreateEntity();
            gCoordinator.AddComponent(pointLightEntity, PointLight{});
            
            // récupère les components
            Transform &transform = gCoordinator.GetComponent<Transform>(pointLightEntity);
            PointLight &pointLight = gCoordinator.GetComponent<PointLight>(pointLightEntity);
            
            pointLight.lightIntensity = 0.2f;
            pointLight.color = lightColors[i];

            auto rotateLight =  
                glm::rotate(glm::mat4(1.f), 
                (i * glm::two_pi<float>()) / lightColors.size(), 
                {0.f, -1.f, 0.f});

            transform.translation = glm::vec3(rotateLight * glm::vec4(-1.f, -1.f, -1.f, 1.f));
        }
    }
}