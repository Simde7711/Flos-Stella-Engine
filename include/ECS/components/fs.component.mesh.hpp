#pragma once

#include "fs.model.hpp"

// std
#include <memory>
#include <iostream>

namespace fs 
{
    enum class RenderType 
    {
        Mesh,
        Billboard,
        // Possibly more in the future.
    };

    struct Mesh
    {
        bool active = true;

        std::unique_ptr<FsModel> model;
        RenderType renderType;

        Mesh() = default;
        Mesh(std::unique_ptr<FsModel> _model) : model(std::move(_model))
        {
        
        }

        ~Mesh()
        {
            std::cout << "[Mesh] Destroyed (ptr=" << model.get() << ") \n";
        }

        // move support
        Mesh(Mesh&&) noexcept = default;
        Mesh& operator=(Mesh&&) noexcept = default;

        // disallow copies
        Mesh(const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;
    };
}
