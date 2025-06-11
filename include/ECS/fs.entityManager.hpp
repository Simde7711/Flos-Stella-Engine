#pragma once

#include "ECS/fs.types.hpp"

// std
#include <array>
#include <cassert>
#include <queue>

namespace fs
{
    class FsEntityManager
    {
        public: 
        FsEntityManager();

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
