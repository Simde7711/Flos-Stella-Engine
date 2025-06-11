#pragma once 

#include "fs.frameInfo.hpp"
#include "ECS/fs.coordinator.hpp"

namespace fs
{
    class FsPointLightSystem
    {
        public:
            void Update(FrameInfo &frameInfo, GlobalUbo &globalUbo);
    };
}