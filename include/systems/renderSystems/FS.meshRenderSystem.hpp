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
    class SimpleRenderSystem
    {
        public:
            SimpleRenderSystem(LveDevice &device);
            ~SimpleRenderSystem();

            SimpleRenderSystem(const LveWindow &) = delete;
            SimpleRenderSystem &operator = (const LveWindow &) = delete;

            void RenderGameObjects(FrameInfo &frameInfo);
        private:
            LveDevice &lveDevice;
    };
}