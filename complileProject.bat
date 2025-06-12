rmdir /s /q .\build\
mkdir build
cd .\build\
mkdir shaders
cmake -G "MinGW Makefiles" ..
mingw32-make
start FLosStellaEngine.exe