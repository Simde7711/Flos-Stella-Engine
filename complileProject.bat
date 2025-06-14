@REM rmdir /s /q .\build\
@REM mkdir build
cd .\build\
@REM mkdir shaders
cmake -G "Ninja" ..
ninja -j6
start FLosStellaEngine.exe