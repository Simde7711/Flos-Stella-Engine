#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm.hpp"

namespace lve
{
    class LveCamera
    {
        public: 

            void SetOrthographicProjection(float left, float right, float top, float bottom, float near, float far);

            void SetPerspectiveProjection(float fovy, float aspect, float near, float far);
            
            const glm::mat4 &GetProjection() const { return projectionMatrix; }
        private:
            glm::mat4 projectionMatrix{1.f};
    };
}