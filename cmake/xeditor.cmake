find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)

set(XEditor.Dir.INCLUDE include/)
set(XEditor.Dir.SRC src/)

file(GLOB_RECURSE XEditor.File.Qt.GUI_HDR include/gui/*.hpp)

file(GLOB_RECURSE XEditor.File.Qt.SRC ${XEditor.Dir.SRC}*.cpp ${XEditor.Dir.SRC}.c)

qt5_wrap_cpp(XEditor.File.Qt.WRAP_CPP ${XEditor.File.Qt.GUI_HDR})

if (WIN32)
    add_executable(xeditor WIN32 ${XEditor.File.Qt.SRC} ${XEditor.File.Qt.WRAP_CPP})
else ()
    add_executable(xeditor ${XEditor.File.Qt.SRC} ${XEditor.File.Qt.WRAP_CPP})
endif ()

target_include_directories(xeditor PUBLIC ${Engine.Dir.INCLUDE} ${XEditor.Dir.INCLUDE} ${XEditor.Dir.SRC} ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(xeditor xengine Qt5::Core Qt5::Widgets)

set(Assets submodules/assets)
file(COPY ${Assets} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})