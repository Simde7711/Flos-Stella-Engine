#pragma once 

#include "lve_frame_info.hpp"
#include "ECS/coordinator.hpp"

namespace lve
{
    class PointLightSystem
    {
        public:
            void Update(FrameInfo &frameInfo, GlobalUbo &globalUbo);
    };
}