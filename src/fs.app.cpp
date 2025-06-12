#include "fs.app.hpp"

#include "keyboard_movement_controller.hpp"
#include "systems/renderSystems/fs.meshRenderSystem.hpp"
#include "systems/componentSystems/fs.pointLightSystem.hpp"
#include "fs.camera.hpp"
#include "fs.buffer.hpp"
#include "fs.frameInfo.hpp"
#include "fs.renderPassManager.hpp"
#include "fs.shaderManager.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm.hpp"
#include "gtc/constants.hpp"

// std
#include <stdexcept>
#include <chrono>
#include <array>
#include <cassert>

namespace fs
{
    FsApp::FsApp()
    {
        // entity component system
        gCoordinator.Init();
        
        // TODO: temporaire pour faire marcher le init
        globalSetLayout = FsDescriptorSetLayout::Builder(device)   
            .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
            .build();
        
        // shader manager
        shaderManager.Init(&device, &renderer, globalSetLayout->getDescriptorSetLayout());

        globalPool = FsDescriptorPool::Builder(device)
            .setMaxSets(FsSwapChain::MAX_FRAMES_IN_FLIGHT)
            .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, FsSwapChain::MAX_FRAMES_IN_FLIGHT)
            .build();

        LoadGameObjects();
    }

    FsApp::~FsApp()
    {
        
    }

    void FsApp::run()
    {
        int modelCount = 0;
        for (Entity entity : gCoordinator.mEntities) 
        {
            if (gCoordinator.HasComponent<Mesh>(entity)) {
                // std::cout << "[DEBUG] Entity " << entity << " has Model component\n";
                ++modelCount;
            }
        }
        // std::cout << "[DEBUG] Total Model components: " << modelCount << "\n";

        std::vector<std::unique_ptr<FsBuffer>> uboBuffers(FsSwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < uboBuffers.size(); i++)
        {
            uboBuffers[i] = std::make_unique<FsBuffer>
            (
                device,
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

        std::vector<VkDescriptorSet> globalDescriptorSets(FsSwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < globalDescriptorSets.size(); i++)
        {
            auto bufferInfo = uboBuffers[i]->descriptorInfo();
            FsDescriptorWriter(*globalSetLayout, *globalPool)
                .writeBuffer(0, &bufferInfo)
                .build(globalDescriptorSets[i]);
        }
        
        // render systems
        FsMeshRenderSystem meshRenderSystem{device};
       
        // component system
        FsPointLightSystem pointLightSystem{};
        
        FsCamera camera{}; 
        // camera.SetViewDirection(glm::vec3(0.f), glm::vec3(0.5f, 0.f, 1.f));
        camera.SetViewTarget(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5f));

        // créer l'entity de la caméra
        Entity entityCamera = gCoordinator.CreateEntity();
        gCoordinator.GetComponent<Transform>(entityCamera).translation.z = -2.5f;

        KeyboardMovementController cameraController{};

        auto currentTime = std::chrono::high_resolution_clock::now();

        while (!window.ShouldClose())
        {
            glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            cameraController.MoveInPlaneXZ(window.GetGLFWwindow(), frameTime, gCoordinator.GetComponent<Transform>(entityCamera));
            camera.SetViewYXZ(gCoordinator.GetComponent<Transform>(entityCamera).translation, gCoordinator.GetComponent<Transform>(entityCamera).rotation);

            float aspect = renderer.GetAspectRatio();
            // camera.SetOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
            camera.SetPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 80.f);

            if (auto commandBuffer = renderer.BeginFrame())
            {
                int frameIndex = renderer.GetFrameIndex();
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
                renderer.BeginSwapChainRenderPass(commandBuffer);

                // order here matters
                meshRenderSystem.RenderGameObjects(frameInfo);

                renderer.EndSwapChainRenderPass(commandBuffer);
                renderer.EndFrame();
            }
        }

        vkDeviceWaitIdle(device.device());
        
        // final cleanup
        shaderManager.Cleanup();
        gCoordinator.DestroyAllEntity();
    }

    void FsApp::LoadGameObjects()
    {   
        {
            // entity
            Entity flatVase = gCoordinator.CreateEntity();
            
            // ajout du model
            gCoordinator.AddComponent(flatVase, Mesh{FsModel::CreateModelFromFile(device, "../assets/models/flat_vase.obj")});

            // set le transform
            Transform &transform1 = gCoordinator.GetComponent<Transform>(flatVase);
            transform1.translation = {-.5f, .5f, 0.f};
            transform1.rotation.z = 0.f;
            transform1.scale = {3.f, 1.5, 3.f};

            // set le shader
            PipelineKey pipelineKey1{};
            pipelineKey1.vertShaderPath = "../assets/shaders/simple_shader.vert.spv";
            pipelineKey1.fragShaderPath = "../assets/shaders/simple_shader.frag.spv";
            PipelineConfigKey pipelineConfigKey1{};
            pipelineKey1.config = pipelineConfigKey1;
            gCoordinator.AddComponent(flatVase, Shader{shaderManager.GetOrCreatePipelineKey(pipelineKey1)});
        }

        {
            // entity
            Entity smoothVase = gCoordinator.CreateEntity();
            
            // ajout du model
            gCoordinator.AddComponent(smoothVase, Mesh{FsModel::CreateModelFromFile(device, "../assets/models/smooth_vase.obj")});

            // set le transform
            Transform &transform2 = gCoordinator.GetComponent<Transform>(smoothVase);
            transform2.translation = {.5f, .5f, 0.f};
            transform2.rotation.z = 0.f;
            transform2.scale = glm::vec3(3.f);
        
            // set le shader
            PipelineKey pipelineKey2{};
            pipelineKey2.vertShaderPath = "../assets/shaders/simple_shader.vert.spv";
            pipelineKey2.fragShaderPath = "../assets/shaders/simple_shader.frag.spv";
            PipelineConfigKey pipelineConfigKey2{};
            pipelineKey2.config = pipelineConfigKey2;
            gCoordinator.AddComponent(smoothVase, Shader{shaderManager.GetOrCreatePipelineKey(pipelineKey2)});
        }

        {
            // entity
            Entity floor = gCoordinator.CreateEntity();
            
            // ajout du model
            gCoordinator.AddComponent(floor, Mesh{FsModel::CreateModelFromFile(device, "../assets/models/quad.obj")});

            // set le transform
            Transform &transform3 = gCoordinator.GetComponent<Transform>(floor);
            transform3.translation = {0.f, .5f, 0.f};
            transform3.scale = {3.f, 1.f, 3.f};

            // set le shader
            PipelineKey pipelineKey3{};
            pipelineKey3.vertShaderPath = "../assets/shaders/simple_shader.vert.spv";
            pipelineKey3.fragShaderPath = "../assets/shaders/simple_shader.frag.spv";
            PipelineConfigKey pipelineConfigKey3{};
            pipelineKey3.config = pipelineConfigKey3;
            gCoordinator.AddComponent(floor, Shader{shaderManager.GetOrCreatePipelineKey(pipelineKey3)});
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