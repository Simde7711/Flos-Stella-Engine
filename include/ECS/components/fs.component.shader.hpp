#pragma once

#include "fs.shaderManager.hpp"

namespace fs
{
    struct Shader
    {
        PipelineKey pipelineKey;
        SinglePushConstantData pushConstant;
    };
}