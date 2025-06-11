#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm.hpp"

namespace fs
{
    class FsCamera
    {
        public: 

            void SetOrthographicProjection(float left, float right, float top, float bottom, float near, float far);

            void SetPerspectiveProjection(float fovy, float aspect, float near, float far);
            
            void SetViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up = glm::vec3{0.f, -1.f, 0.f});
            void SetViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up = glm::vec3{0.f, -1.f, 0.f});
            void SetViewYXZ(glm::vec3 position, glm::vec3 rotation);

            const glm::mat4 &GetProjection() const { return projectionMatrix; }
            const glm::mat4 &GetView() const { return viewMatrix; }
            const glm::mat4 &GetInverseView() const { return inverseViewMatrix; }
            const glm::vec3 GetPosition() const { return glm::vec3(inverseViewMatrix[3]); }

        private:
            glm::mat4 projectionMatrix{1.f};
            glm::mat4 viewMatrix{1.f};
            glm::mat4 inverseViewMatrix{1.f};
    };
}