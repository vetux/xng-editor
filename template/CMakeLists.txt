cmake_minimum_required(VERSION 3.0.0)

project(NewProject)

# These variables are set by the editor application when building the project
set(EXE_NAME game) # The name of the target
set(SRC_DIR template/source/ template/plugin/) # Source directory
set(INC_DIR template/source/ template/plugin/) # Include directory
set(LNK_DIR) # Link directory
set(LINK xengine) # Link libraries

file(GLOB_RECURSE SRC ${SRC_DIR}*.c ${SRC_DIR}*.cpp)

if (WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
endif ()

add_executable(${EXE_NAME} ${SRC})

target_include_directories(${EXE_NAME} PUBLIC ${INC_DIR})
target_link_directories(${EXE_NAME} PUBLIC ${LNK_DIR})
target_link_libraries(${EXE_NAME} ${LINK})
