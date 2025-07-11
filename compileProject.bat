@echo off
rem ---- clean & recreate build folder ---------------------------------
@REM rmdir /s /q build
@REM mkdir build
cd build

rem ---- create extra sub‑folder for your compiled shaders --------------
mkdir shaders
mkdir scripts

rem ---- remove cache --------------------------------------------------
del CMakeCache.txt
del cmake_install.cmake
del Makefile

rem ---- configure project ---------------------------------------------
cmake -G "Ninja" ..

rem ---- copy compile_commands.json to project root ---------------------
copy compile_commands.json ..

rem ---- build (Ninja automatically uses all available cores) ----------
ninja -j6
@REM ninja -v  - for verbose output

rem ---- run executable -------------------------------------------------
start FlosStellaEngine.exe