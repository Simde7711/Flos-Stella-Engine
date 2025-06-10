#pragma once

#include "shaderManager.hpp"

namespace lve
{
    struct Shader
    {
        PipelineKey pipelineKey;
        SinglePushConstantData pushConstant;
    };
}