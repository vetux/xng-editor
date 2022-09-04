# xng-tools
Editor and other applications for creating games with xEngine

# Building
- Clone and build the [xng](https://github.com/xenotux/xng)
- Either :
    - Copy the include directory of the engine to the lib/include directory and the built engine library to lib/bin
    - Add the path of the engine include directory to the include path by editing [config.cmake](cmake/config.cmake)
- Load [CMakeLists.txt](CMakeLists.txt) and build the editor target
