#pragma once 
#include "C.hpp"


#include "gameScripts/fs.engineApi.hpp"

class B: public fs::ScriptBase 
{
    void Init() override;
    void Update() override;
    void allo();
};