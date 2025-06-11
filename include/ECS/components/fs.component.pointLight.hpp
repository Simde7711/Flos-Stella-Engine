#pragma once

// libs
#include "glm.hpp"

namespace FS
{
    struct PointLight
    {
        float lightIntensity = 1.0f;
        glm::vec3 color{};
    };
}
