find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)

set(Editor.Dir.INCLUDE source/editor/include/)
set(Editor.Dir.SRC source/editor/src/)

file(GLOB_RECURSE Editor.File.Qt.GUI_HDR source/editor/include/gui/*.hpp)

file(GLOB_RECURSE Editor.File.Qt.SRC ${Editor.Dir.SRC}*.cpp ${Editor.Dir.SRC}.c)

qt5_wrap_cpp(Editor.File.Qt.WRAP_CPP ${Editor.File.Qt.GUI_HDR})

if (WIN32)
    add_executable(mana-editor WIN32 ${Editor.File.Qt.SRC} ${Editor.File.Qt.WRAP_CPP})
else ()
    add_executable(mana-editor ${Editor.File.Qt.SRC} ${Editor.File.Qt.WRAP_CPP})
endif ()

target_include_directories(mana-editor PUBLIC ${Engine.Dir.INCLUDE} ${Editor.Dir.INCLUDE} ${Editor.Dir.SRC} ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(mana-editor mana-engine Qt5::Core Qt5::Widgets)

file(GLOB_RECURSE Sample.AssetPack assets/sample/assets*.pak)
file(COPY ${Sample.AssetPack} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})