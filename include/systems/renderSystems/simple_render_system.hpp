#pragma once 

#include "lve_camera.hpp"
#include "lve_pipeline.hpp"
#include "lve_device.hpp"
#include "lve_frame_info.hpp"
#include "ECS/coordinator.hpp"
#include "shaderManager.hpp"

// std
#include <vector>
#include <memory>

namespace lve
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