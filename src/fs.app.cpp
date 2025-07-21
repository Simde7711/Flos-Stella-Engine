#include "fs.app.hpp"

#include "keyboard_movement_controller.hpp"
#include "systems/renderSystems/fs.meshRenderSystem.hpp"
#include "systems/componentSystems/fs.pointLightSystem.hpp"
#include "fs.camera.hpp"
#include "fs.buffer.hpp"
#include "ECS/fs.coordinator.hpp"
#include "fs.frameInfo.hpp"
#include "fs.renderPassManager.hpp"
#include "fs.shaderManager.hpp"
#include "fs.logger.hpp"
#include "compilers/fs.compilerManager.hpp"
#include <string>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <memory>
#include <stdexcept>
#include <chrono>
#include <array>
#include <vector>
#include <cassert>

namespace fs
{
    FsApp::FsApp(mINI::INIStructure _config)
    {
        FsLogger::GetInstance().Log(LogType::System, "Ouverture de l'application");

        // set la config dans une variable.
        config = _config;

        // window
        window = std::make_unique<FsWindow>(DEFAULT_WIDTH, DEFAULT_HEIGHT, "Flōs Stella Engine V0.002");

        // device
        device = std::make_unique<FsDevice>(*window);

        // renderer
        renderer = std::make_unique<FsRenderer>(*window, *device);

        // entity component system
        FsCoordinator::GetInstance().Init();

        // compilerManager
        compilerManager = std::make_unique<FsCompilerManager>(device.get(), config["Compilers"]["assetsPath"], config["Compilers"]["outputShadersPath"], config["Compilers"]["outputScriptsPath"]);

        // TODO: temporaire pour faire marcher le init
        globalSetLayout = FsDescriptorSetLayout::Builder(*device)   
            .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
            .build();
        
        // shader manager
        FsShaderManager::GetInstance().Init(device.get(), renderer.get(), globalSetLayout->getDescriptorSetLayout());

        globalPool = FsDescriptorPool::Builder(*device)
            .setMaxSets(FsSwapChain::MAX_FRAMES_IN_FLIGHT)
            .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, FsSwapChain::MAX_FRAMES_IN_FLIGHT)
            .build();

        LoadGameObjects();
    }

    FsApp::~FsApp()
    {
        // cleanup du shader manager.
        FsShaderManager::GetInstance().Cleanup();

        // detruit tout les entities et cleanup du coordinator.
        FsCoordinator::GetInstance().DestroyAllEntity();

        FsLogger::GetInstance().Log(LogType::System, "Fermeture de l'application");
    }

    void FsApp::run()
    {
        int modelCount = 0;
        for (Entity entity : FsCoordinator::GetInstance().mEntities) 
        {
            if (FsCoordinator::GetInstance().HasComponent<Mesh>(entity)) {
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
                *device,
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
        FsMeshRenderSystem meshRenderSystem{*device};
       
        // component system
        FsPointLightSystem pointLightSystem{};
        
        FsCamera camera{}; 
        // camera.SetViewDirection(glm::vec3(0.f), glm::vec3(0.5f, 0.f, 1.f));
        camera.SetViewTarget(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5f));

        // créer l'entity de la caméra
        Entity entityCamera = FsCoordinator::GetInstance().CreateEntity();
        FsCoordinator::GetInstance().GetComponent<Transform>(entityCamera).translation.z = -2.5f;

        KeyboardMovementController cameraController{};

        auto currentTime = std::chrono::high_resolution_clock::now();

        float compilerTimer = 0.0f;

        while (!window->ShouldClose())
        {
            glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            cameraController.MoveInPlaneXZ(window->GetGLFWwindow(), frameTime, FsCoordinator::GetInstance().GetComponent<Transform>(entityCamera));
            camera.SetViewYXZ(FsCoordinator::GetInstance().GetComponent<Transform>(entityCamera).translation, FsCoordinator::GetInstance().GetComponent<Transform>(entityCamera).rotation);

            float aspect = renderer->GetAspectRatio();
            // camera.SetOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
            camera.SetPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 80.f);

            if (auto commandBuffer = renderer->BeginFrame())
            {
                int frameIndex = renderer->GetFrameIndex();
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
                renderer->BeginSwapChainRenderPass(commandBuffer);

                // order here matters
                meshRenderSystem.RenderGameObjects(frameInfo);

                renderer->EndSwapChainRenderPass(commandBuffer);
                renderer->EndFrame();

                if (glfwGetWindowAttrib(window->GetGLFWwindow(), GLFW_FOCUSED)) 
                {
                    compilerTimer += frameTime;
                    if (compilerTimer >= 2.0f) 
                    {
                        compilerManager->WatchForChanges();
                        compilerTimer = 0.0f;
                    }
                } 
                else 
                {
                    compilerTimer = 0.0f; 
                }

            }

            FsLogger::GetInstance().FlushFile();
        }

        vkDeviceWaitIdle(device->device());
    }

    void FsApp::LoadGameObjects()
    {   
        {
            // entity
            Entity flatVase = FsCoordinator::GetInstance().CreateEntity();
            
            // ajout du model
            FsCoordinator::GetInstance().AddComponent<Mesh>(flatVase, Mesh{FsModel::CreateModelFromFile(*device, "../assets/models/flat_vase.obj")});

            // set le transform
            Transform &transform1 = FsCoordinator::GetInstance().GetComponent<Transform>(flatVase);
            transform1.translation = {-.5f, .5f, 0.f};
            transform1.rotation.z = 0.f;
            transform1.scale = {3.f, 1.5, 3.f};

            // set le shader
            PipelineKey pipelineKey1{};
            pipelineKey1.vertShaderPath = "shaders/simple_shader.vert.spv";
            pipelineKey1.fragShaderPath = "shaders/simple_shader.frag.spv";
            PipelineConfigKey pipelineConfigKey1{};
            pipelineKey1.config = pipelineConfigKey1;
            FsCoordinator::GetInstance().AddComponent<Shader>(flatVase, Shader{FsShaderManager::GetInstance().GetOrCreatePipelineKey(pipelineKey1)});
        }

        {
            // entity
            Entity smoothVase = FsCoordinator::GetInstance().CreateEntity();
            
            // ajout du model
            FsCoordinator::GetInstance().AddComponent<Mesh>(smoothVase, Mesh{FsModel::CreateModelFromFile(*device, "../assets/models/smooth_vase.obj")});

            // set le transform
            Transform &transform2 = FsCoordinator::GetInstance().GetComponent<Transform>(smoothVase);
            transform2.translation = {.5f, .5f, 0.f};
            transform2.rotation.z = 0.f;
            transform2.scale = glm::vec3(3.f);
        
            // set le shader
            PipelineKey pipelineKey2{};
            pipelineKey2.vertShaderPath = "shaders/simple_shader.vert.spv";
            pipelineKey2.fragShaderPath = "shaders/simple_shader.frag.spv";
            PipelineConfigKey pipelineConfigKey2{};
            pipelineKey2.config = pipelineConfigKey2;
            FsCoordinator::GetInstance().AddComponent<Shader>(smoothVase, Shader{FsShaderManager::GetInstance().GetOrCreatePipelineKey(pipelineKey2)});
        }

        {
            // entity
            Entity floor = FsCoordinator::GetInstance().CreateEntity();
            
            // ajout du model
            FsCoordinator::GetInstance().AddComponent<Mesh>(floor, Mesh{FsModel::CreateModelFromFile(*device, "../assets/models/quad.obj")});

            // set le transform
            Transform &transform3 = FsCoordinator::GetInstance().GetComponent<Transform>(floor);
            transform3.translation = {0.f, .5f, 0.f};
            transform3.scale = {3.f, 1.f, 3.f};

            // set le shader
            PipelineKey pipelineKey3{};
            pipelineKey3.vertShaderPath = "shaders/simple_shader.vert.spv";
            pipelineKey3.fragShaderPath = "shaders/simple_shader.frag.spv";
            PipelineConfigKey pipelineConfigKey3{};
            pipelineKey3.config = pipelineConfigKey3;
            FsCoordinator::GetInstance().AddComponent<Shader>(floor, Shader{FsShaderManager::GetInstance().GetOrCreatePipelineKey(pipelineKey3)});
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
            Entity pointLightEntity = FsCoordinator::GetInstance().CreateEntity();
            FsCoordinator::GetInstance().AddComponent<PointLight>(pointLightEntity, PointLight{});
            
            // récupère les components
            Transform &transform = FsCoordinator::GetInstance().GetComponent<Transform>(pointLightEntity);
            PointLight &pointLight = FsCoordinator::GetInstance().GetComponent<PointLight>(pointLightEntity);
            
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