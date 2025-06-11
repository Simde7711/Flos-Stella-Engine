#pragma once

#include "ECS/FS.types.hpp"

// std
#include <array>
#include <cassert>
#include <unordered_map>
#include <optional>
#include <iostream>

namespace lve
{
    class IComponentArray
    {
        public:
            virtual ~IComponentArray() = default;
            virtual void EntityDestroyed(Entity entity) = 0;
    };

    template<typename T>
    class ComponentArray : public IComponentArray
    {
        public:

        void InsertData(Entity entity, T &&component)
        {
            assert(mEntityToIndexMap.find(entity) == mEntityToIndexMap.end() && "Component added to same entity more than once.");

            // Put new entry at end
            size_t newIndex = mSize;
            mEntityToIndexMap[entity] = newIndex;
            mIndexToEntityMap[newIndex] = entity;
            mComponentArray[newIndex] = std::move(component);
            ++mSize;
        }

        void RemoveData(Entity entity)
        {
            std::cout << "[ComponentArray] Removed component for entity: " << entity << std::endl;

            assert(mEntityToIndexMap.find(entity) != mEntityToIndexMap.end() && "Removing non-existent component.");

            // Copy element at end into deleted element's place to maintain density
            size_t indexOfRemovedEntity = mEntityToIndexMap[entity];
            size_t indexOfLastElement = mSize - 1;
            mComponentArray[indexOfRemovedEntity] = std::move(mComponentArray[indexOfLastElement]);
            mComponentArray[indexOfLastElement].reset();

            // Update map to point to moved spot
            Entity entityOfLastElement = mIndexToEntityMap[indexOfLastElement];
            mEntityToIndexMap[entityOfLastElement] = indexOfRemovedEntity;
            mIndexToEntityMap[indexOfRemovedEntity] = entityOfLastElement;

            mEntityToIndexMap.erase(entity);
            mIndexToEntityMap.erase(indexOfLastElement);

            --mSize;
        }

        T& GetData(Entity entity)
        {
            assert(mEntityToIndexMap.find(entity) != mEntityToIndexMap.end() && "Retrieving non-existent component.");

            auto &opt = mComponentArray[mEntityToIndexMap[entity]];
            assert(opt.has_value() && "Optional component is empty");

            return *opt;
        }

        void EntityDestroyed(Entity entity) override
        {
            if (mEntityToIndexMap.find(entity) != mEntityToIndexMap.end())
            {
                RemoveData(entity);
            }
        }

        bool HasEntity(Entity entity) const
        {
            return mEntityToIndexMap.find(entity) != mEntityToIndexMap.end();
        }

        private:

            std::array<std::optional<T>, MAX_ENTITIES> mComponentArray;
            std::unordered_map<Entity, size_t> mEntityToIndexMap{};
            std::unordered_map<size_t, Entity> mIndexToEntityMap{};
            size_t mSize{};
    };
}
