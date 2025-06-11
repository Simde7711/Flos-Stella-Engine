#pragma once 

#include "FS.frameInfo.hpp"
#include "ECS/FS.coordinator.hpp"

namespace FS
{
    class PointLightSystem
    {
        public:
            void Update(FrameInfo &frameInfo, GlobalUbo &globalUbo);
    };
}