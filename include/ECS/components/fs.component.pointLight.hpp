#pragma once

// libs
#include <glm/glm.hpp>

namespace fs
{
    struct PointLight
    {
        float lightIntensity = 1.0f;
        glm::vec3 color{};
    };
}
