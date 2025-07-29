#pragma once

#include "ECS/fs.componentManager.hpp"
#include "ECS/fs.entityManager.hpp"
#include "ECS/fs.types.hpp"
#include "fs.logger.hpp"

// std
#include <memory>
#include <string>
#include <unordered_set>

namespace fs
{
    class FsCoordinator
    {
        public: 
            std::unordered_set<Entity> mEntities;

            static FsCoordinator &GetInstance()
            {
                return instance;
            }

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
                AddComponent<Transform>(entity, Transform{});
                AddComponent<Active>(entity, Active{});

                return entity;
            }

            void DestroyEntity(Entity entity)
            {
                FsLogger::GetInstance().Log(LogType::System, "[FsCoordinator] Destroying entity: " + std::to_string(entity));
                
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
	        T *GetComponent(Entity entity)
            {
                if (!HasComponent<T>(entity)) 
                {
                    FsLogger::GetInstance().Log(LogType::Warning, "[FsCoordinator] Tried to get component of type " + std::string(typeid(T).name()) + " from entity " + std::to_string(entity) + " but it doesn't exist.");
                    return nullptr;
                }
                else
                {
                    return &(mComponentManager->GetComponent<T>(entity));
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

            inline void SetActive(Entity entity, bool active)
            {
                if (active)  
                {    
                    AddComponent<Active>(entity, Active{});
                }
                else
                {
                    RemoveComponent<Active>(entity);
                }
            }

            inline bool IsEntityActive(Entity entity)
            {
                return HasComponent<Active>(entity);
            }

        private:
            static FsCoordinator instance;
	        std::unique_ptr<FsComponentManager> mComponentManager;
	        std::unique_ptr<FsEntityManager> mEntityManager;

            void RegisterComponents()
            {
                // Register all engine components
                RegisterComponent<Transform>();
                RegisterComponent<Active>();
                RegisterComponent<Mesh>();
                RegisterComponent<PointLight>();
                RegisterComponent<Shader>();
            }
    };
}