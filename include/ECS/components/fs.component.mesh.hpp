#pragma once

#include "fs.model.hpp"
#include "fs.logger.hpp"

// std
#include <memory>
#include <sstream>

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
            std::stringstream ss;
            ss << "0x" << std::hex << reinterpret_cast<uintptr_t>(model.get());
            FsLogger::GetInstance().Log(LogType::System, "[Mesh] Destroyed (ptr=" + ss.str() + ")");
        }

        // move support
        Mesh(Mesh&&) noexcept = default;
        Mesh& operator=(Mesh&&) noexcept = default;

        // disallow copies
        Mesh(const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;
    };
}
