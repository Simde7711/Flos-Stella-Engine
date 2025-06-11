#pragma once

#include "fs.shaderManager.hpp"

namespace FS
{
    struct Shader
    {
        PipelineKey pipelineKey;
        SinglePushConstantData pushConstant;
    };
}