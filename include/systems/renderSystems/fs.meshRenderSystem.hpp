#pragma once 

#include "fs.camera.hpp"
#include "fs.pipeline.hpp"
#include "fs.device.hpp"
#include "fs.frameInfo.hpp"
#include "ECS/fs.coordinator.hpp"
#include "fs.shaderManager.hpp"

// std
#include <vector>
#include <memory>

namespace FS
{
    class FsMeshRenderSystem
    {
        public:
            FsMeshRenderSystem(FsDevice &device);
            ~FsMeshRenderSystem();

            FsMeshRenderSystem(const FsWindow &) = delete;
            FsMeshRenderSystem &operator = (const FsWindow &) = delete;

            void RenderGameObjects(FrameInfo &frameInfo);
        private:
            FsDevice &device;
    };
}