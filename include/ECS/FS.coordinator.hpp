#pragma once

#include "ECS/fs.componentManager.hpp"
#include "ECS/fs.entityManager.hpp"
#include "ECS/fs.types.hpp"

// std
#include <memory>
#include <unordered_set>
#include <iostream>

namespace FS
{
    class FsCoordinator
    {
        public: 
            std::unordered_set<Entity> mEntities;

            void Init()
            {
                mComponentManager = std::make_unique<FsComponentManager>();
                mEntityManager = std::make_unique<FsEntityManager>();

                RegisterComponents();
            }

            Entity CreateEntity()
            {
                Entity entity = mEntityManager->CreateEntity();
                mEntities.insert(entity);
                
                // ajoute les components universels
                AddComponent(entity, Transform{});

                return entity;
            }

            void DestroyEntity(Entity entity)
            {
                std::cout << "[FsCoordinator] Destroying entity: " << entity << std::endl;

                mComponentManager->EntityDestroyed(entity);
                mEntityManager->DestroyEntity(entity);
            }

            template<typename T>
	        void RegisterComponent()
            {
                mComponentManager->RegisterComponent<T>();
            }

            template<typename T>
	        void AddComponent(Entity entity, T &&component)
            { 
                mComponentManager->AddComponent<T>(entity, std::forward<T>(component));

                auto signature = mEntityManager->GetSignature(entity);
                signature.set(mComponentManager->GetComponentType<T>(), true);
                mEntityManager->SetSignature(entity, signature);
            }

            template<typename T>
	        void RemoveComponent(Entity entity)
            {
                mComponentManager->RemoveComponent<T>(entity);

                auto signature = mEntityManager->GetSignature(entity);
                signature.set(mComponentManager->GetComponentType<T>(), false);
                mEntityManager->SetSignature(entity, signature);
            }

            template<typename T>
	        T& GetComponent(Entity entity)
            {
                if (!HasComponent<T>(entity)) 
                {
                    std::cerr << "Error: Tried to get component of type " << typeid(T).name() << " from entity " << entity << " but it doesn't exist.\n";
                    throw std::runtime_error("Missing component");
                }
                else
                {
                    return mComponentManager->GetComponent<T>(entity);
                }
            }

            template<typename T>
	        ComponentType GetComponentType()
            {
                return mComponentManager->GetComponentType<T>();
            }

            template<typename T>
            bool HasComponent(Entity entity) const
            {
                return mComponentManager->HasComponent<T>(entity);
            }

            void DestroyAllEntity()
            {
                for (Entity entity : mEntities)     
                {
                    DestroyEntity(entity);
                }
                mEntities.clear();
            }

        private:

	        std::unique_ptr<FsComponentManager> mComponentManager;
	        std::unique_ptr<FsEntityManager> mEntityManager;

            void RegisterComponents()
            {
                // Register all engine components
                RegisterComponent<Transform>();
                RegisterComponent<Mesh>();
                RegisterComponent<PointLight>();
                RegisterComponent<Shader>();
            }
    };

    // pour faire une instance unique dans le cpp
    extern FsCoordinator gCoordinator;
}