@echo off
set GLSLC="C:\VulkanSDK\1.4.309.0\Bin\glslc.exe"

%GLSLC% -DVERTEX_SHADER -fshader-stage=vert assets\shaders\simple_shader.glsl -o assets\shaders\simple_shader.vert.spv

%GLSLC% -DFRAGMENT_SHADER -fshader-stage=frag assets\shaders\simple_shader.glsl -o assets\shaders\simple_shader.frag.spv

echo Compilation done.
pause