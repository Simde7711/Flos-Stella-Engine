#pragma once

#include "ECS/types.hpp"
#include <array>
#include <cassert>
#include <queue>

namespace lve
{
    class EntityManager
    {
        public: 
        EntityManager();

        Entity CreateEntity();

        void DestroyEntity(Entity entity);

        void SetSignature(Entity entity, Signature signature);

        Signature GetSignature(Entity entity);

        private:

        std::queue<Entity> mAvailableEntities{};
	    std::array<Signature, MAX_ENTITIES> mSignatures{};
	    uint32_t mLivingEntityCount{};
    };
}
