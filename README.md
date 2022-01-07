# Editor
Editor and tools for creating games.

# Building
- Clone and build the [engine](https://github.com/xenotux/engine)
- Either :
    - Copy the include directory of the engine to the lib/include directory and the built engine library to lib/bin
    - Add the path of the engine include directory to the include path by editing [config.cmake](cmake/config.cmake)
- Clone the [assets](submodules/assets) submodule
- Load [CMakeLists.txt](CMakeLists.txt) and build the editor target