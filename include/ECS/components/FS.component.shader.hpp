#pragma once

#include "FS.shaderManager.hpp"

namespace lve
{
    struct Shader
    {
        PipelineKey pipelineKey;
        SinglePushConstantData pushConstant;
    };
}