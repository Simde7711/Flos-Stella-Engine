#pragma once

#include "FS.shaderManager.hpp"

namespace FS
{
    struct Shader
    {
        PipelineKey pipelineKey;
        SinglePushConstantData pushConstant;
    };
}