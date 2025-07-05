@echo off
rem ---- clean & recreate build folder ---------------------------------
rmdir /s /q build
mkdir build
cd build

rem ---- create extra sub‑folder for your compiled shaders --------------
mkdir shaders

rem ---- configure project ---------------------------------------------
cmake -G "MinGW Makefiles"  ..

rem ---- copy compile_commands.json to project root ---------------------
copy compile_commands.json ..

rem ---- build (6 parallel jobs) ---------------------------------------
mingw32-make -j6

rem ---- run executable -------------------------------------------------

start FlosStellaEngine.exe