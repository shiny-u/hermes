# Hermes

Hermes : Heterogeneous Messaging C++ library.

## Compilation

### Linux

Execute the following commands in a terminal:
```bash
mkdir build
cd build
cmake ..
make
```

### Windows using Visual Studio

Execute the following commands in git bash:
```bash
mkdir build
cd build
# Replace "Visual Studio 15 2017" by your version, for example "Visual Studio 16 2019".
# Replace "x64" by "Win32" if you want to build 32 bits binaries.
cmake .. -G"Visual Studio 15 2017" -A x64
```
It will generate a `.sln` file in `build` folder that you can open with Visual Studio.

Compilation and execution of any code should be done using Visual Studio.

### Windows using MinGW

Execute the following commands in git bash:
```bash
mkdir build
cd build
cmake .. -G"MinGW Makefiles" -DCMAKE_SH="CMAKE_SH-NOTFOUND"
mingw32-make -j $(nproc)
```
