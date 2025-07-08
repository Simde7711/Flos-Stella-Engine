#pragma once

// libs
#include <glm/glm.hpp>

namespace fs
{
    struct PointLight
    {
        bool active = true;

        float lightIntensity = 1.0f;
        glm::vec3 color{};
    };
}
