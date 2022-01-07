find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)

set(Editor.Dir.INCLUDE include/)
set(Editor.Dir.SRC src/)

file(GLOB_RECURSE Editor.File.Qt.GUI_HDR include/gui/*.hpp)

file(GLOB_RECURSE Editor.File.Qt.SRC ${Editor.Dir.SRC}*.cpp ${Editor.Dir.SRC}.c)

qt5_wrap_cpp(Editor.File.Qt.WRAP_CPP ${Editor.File.Qt.GUI_HDR})

if (WIN32)
    add_executable(editor WIN32 ${Editor.File.Qt.SRC} ${Editor.File.Qt.WRAP_CPP})
else ()
    add_executable(editor ${Editor.File.Qt.SRC} ${Editor.File.Qt.WRAP_CPP})
endif ()

target_include_directories(editor PUBLIC ${Engine.Dir.INCLUDE} ${Editor.Dir.INCLUDE} ${Editor.Dir.SRC} ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(editor xengine Qt5::Core Qt5::Widgets)

set(Assets submodules/assets)
file(COPY ${Assets} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_compile_definitions(MANA_EXPORT=)