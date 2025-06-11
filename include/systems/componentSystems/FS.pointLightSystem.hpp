#pragma once 

#include "fs.frameInfo.hpp"
#include "ECS/fs.coordinator.hpp"

namespace FS
{
    class FsPointLightSystem
    {
        public:
            void Update(FrameInfo &frameInfo, GlobalUbo &globalUbo);
    };
}