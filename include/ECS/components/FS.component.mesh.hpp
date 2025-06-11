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

    struct Mesh
    {
        std::unique_ptr<LveModel> model;
        RenderType renderType;

        Mesh() = default;
        Mesh(std::unique_ptr<LveModel> _model) : model(std::move(_model))
        {
        
        }

        ~Mesh()
        {
            std::cout << "[Model] Destroyed (ptr=" << model.get() << ") \n";
        }

        // move support
        Mesh(Mesh&&) noexcept = default;
        Mesh& operator=(Mesh&&) noexcept = default;

        // disallow copies
        Mesh(const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;
    };
}
