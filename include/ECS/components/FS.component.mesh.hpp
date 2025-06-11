#pragma once

#include "FS.model.hpp"
#include <memory>
#include <iostream>

namespace FS 
{
    enum class RenderType 
    {
        Mesh,
        Billboard,
        // Possibly more in the future.
    };

    struct Model
    {
        std::unique_ptr<LveModel> model;
        RenderType renderType;

        Model() = default;
        Model(std::unique_ptr<LveModel> _model) : model(std::move(_model))
        {
        
        }

        ~Model()
        {
            std::cout << "[Model] Destroyed (ptr=" << model.get() << ") \n";
        }

        // move support
        Model(Model&&) noexcept = default;
        Model& operator=(Model&&) noexcept = default;

        // disallow copies
        Model(const Model&) = delete;
        Model& operator=(const Model&) = delete;
    };
}
