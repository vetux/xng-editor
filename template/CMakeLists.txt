cmake_minimum_required(VERSION 3.0.0)

project(NewProject)

# These variables are set by the editor application when building the project
if (NOT DEFINED EXE_NAME)
    set(EXE_NAME game) # The name of the target
endif ()
if (NOT DEFINED PLUGIN_NAME)
    set(PLUGIN_NAME game_plugin) # The name of the plugin target
endif ()
if (NOT DEFINED SRC_DIR)
    set(SRC_DIR source/) # Source directories
endif ()
if (NOT DEFINED INC_DIR)
    set(INC_DIR source/) # Include directories
endif ()
if (NOT DEFINED LNK_DIR)
    set(LNK_DIR) # Link directories
endif ()
if (NOT DEFINED LINK)
    set(LINK xengine) # Link libraries
endif ()

file(GLOB_RECURSE SRC ${SRC_DIR}*.c ${SRC_DIR}*.cpp)

if (WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
elseif (UNIX)
    set(CMAKE_EXE_LINKER_FLAGS -rdynamic)
endif ()

if (UNIX)
    add_compile_definitions(EXPORT=__attribute__\(\(visibility\("default"\)\)\))
elseif (WIN32)
    add_compile_definitions(EXPORT=__declspec\(dllexport\))
else ()
    add_compile_definitions(EXPORT=)
endif ()

add_executable(${EXE_NAME} ${SRC})

target_include_directories(${EXE_NAME} PUBLIC ${INC_DIR})
target_link_directories(${EXE_NAME} PUBLIC ${LNK_DIR})
target_link_libraries(${EXE_NAME} ${LINK})

file(GLOB_RECURSE PLUGIN_SRC plugin/*.c plugin/*.cpp)

add_library(${PLUGIN_NAME} SHARED ${PLUGIN_SRC})
target_include_directories(${PLUGIN_NAME} PUBLIC ${INC_DIR})
target_link_directories(${PLUGIN_NAME} PUBLIC ${LNK_DIR})
target_link_libraries(${PLUGIN_NAME} ${LINK})
