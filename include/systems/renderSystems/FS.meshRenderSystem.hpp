#pragma once 

#include "FS.camera.hpp"
#include "FS.pipeline.hpp"
#include "FS.device.hpp"
#include "FS.frameInfo.hpp"
#include "ECS/FS.coordinator.hpp"
#include "FS.shaderManager.hpp"

// std
#include <vector>
#include <memory>

namespace FS
{
    class MeshRenderSystem
    {
        public:
            MeshRenderSystem(LveDevice &device);
            ~MeshRenderSystem();

            MeshRenderSystem(const LveWindow &) = delete;
            MeshRenderSystem &operator = (const LveWindow &) = delete;

            void RenderGameObjects(FrameInfo &frameInfo);
        private:
            LveDevice &lveDevice;
    };
}