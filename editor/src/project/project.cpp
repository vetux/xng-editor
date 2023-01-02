/**
 *  xEditor - Editor and tools for creating games
 *  Copyright (C) 2021  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "project/project.hpp"

#include "xng/asset/scene.hpp"
#include "xng/io/protocol/jsonprotocol.hpp"
#include "xng/io/archive/directoryarchive.hpp"

#include <filesystem>
#include <fstream>

#include "io/paths.hpp"

static const char *TEMPLATE_GAME_CLASS = R"###(
#ifndef NEWPROJECT_GAME_HPP
#define NEWPROJECT_GAME_HPP

#include "xng/xng.hpp"

using namespace xng;

class Game : public Application {
public:
    Game(int argc, char *argv[])
            : Application(argc, argv),
              dataArchive(std::make_unique<DirectoryArchive>(std::filesystem::current_path() / "data/")),
              assetPakStream(dataArchive->open("assets.pak")),
              cryptoDriver(DriverRegistry::load<CryptoDriver>("cryptopp")),
              zip(cryptoDriver->createGzip()),
              sha(cryptoDriver->createSHA()),
              assetPakArchive(std::make_shared<PakArchive>(
                      std::vector<std::reference_wrapper<std::istream>>({*assetPakStream}),
                      false,
                      *zip,
                      *sha)){
        ResourceRegistry::getDefaultRegistry().addArchive("assets", assetPakArchive); // The default project settings set the assets/ directory as an asset bundle. It is made available under the "assets" scheme.
    }

    void start() override {
        // Create the system pipelines
        // Load your scene and update the scene of the systemRuntime, for multi-scene games you would extend this class to handle switching between scenes (Loading screen etc.)
        systemRuntime.start();
    }

    void stop() override {
        systemRuntime.stop();
        // Cleanup the pipelines
    }

    void update(float deltaTime) override {
        systemRuntime.update(deltaTime);
    }

private:
    std::unique_ptr<CryptoDriver> cryptoDriver;

    SystemRuntime systemRuntime;

    std::unique_ptr<GZip> zip;
    std::unique_ptr<SHA> sha;

    std::unique_ptr<DirectoryArchive> dataArchive;

    std::unique_ptr<std::istream> assetPakStream;
    std::shared_ptr<PakArchive> assetPakArchive;
};

#endif //NEWPROJECT_GAME_HPP
)###";

static const char *TEMPLATE_MAIN = R"###(
#include "game.hpp"

int main(int argc, char *argv[]) {
    return Game(argc, argv).loop();
}
)###";

static const char *TEMPLATE_PLUGIN_MAIN = R"###(
/**
    This file is compiled by the editor to a shared library and loaded at the users request into the editor process.
    When the editor calls the load function on the shared library the plugin can register component and systems types
    which will allow them to be used in the editor play mode.

    For creating components in the scene it is not required to register components.
**/

void load() {
    // Register your systems and components like so
    // REGISTER_COMPONENT(MyComponentType)
    // REGISTER_COMPONENT(AnotherComponentType)
    // REGISTER_SYSTEM(MySystem)
}

void unload() {
    // Cleanup the registrations like so
    // UNREGISTER_COMPONENT(MyComponentType)
    // UNREGISTER_COMPONENT(AnotherComponentType)
    // UNREGISTER_SYSTEM(MySystem)
}
)###";

static const char *TEMPLATE_CMAKE_LISTS = R"###(
cmake_minimum_required(VERSION 3.0.0)

project(NewProject)

# These variables are set by the editor application when building the project
if (NOT DEFINED EXE_NAME)
    set(EXE_NAME game) # The name of the target
endif ()
if (NOT DEFINED PLUGIN_NAME)
    set(PLUGIN_NAME game_plugin) # The name of the plugin target
endif ()
if (NOT DEFINED PLUGIN_DIR)
    set(PLUGIN_DIR plugin/) # The path to the directory containing plugin sources
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

file(GLOB_RECURSE PLUGIN_SRC ${PLUGIN_DIR}*.c ${PLUGIN_DIR}*.cpp)

add_library(${PLUGIN_NAME} SHARED ${PLUGIN_SRC})
target_include_directories(${PLUGIN_NAME} PUBLIC ${INC_DIR})
target_link_directories(${PLUGIN_NAME} PUBLIC ${LNK_DIR})
target_link_libraries(${PLUGIN_NAME} ${LINK})
)###";

void Project::createNewProject(const std::filesystem::path &outputDir) {
    if (!std::filesystem::is_empty(outputDir)) {
        throw std::runtime_error("Output directory is not empty.");
    }

    BuildSettings buildLinux64;
    buildLinux64.name = "Linux x64";
    buildLinux64.cmakeCommand = "cmake";
    buildLinux64.buildDir = "build/linux_x64/";
    buildLinux64.targetPlatform = LINUX_64;
    buildLinux64.optimization = SAFE_OPTIMIZATIONS;
    buildLinux64.gameTargetName = "game";
    buildLinux64.pluginTargetName = "game_plugin";
    buildLinux64.sourceDirectories.insert("source/");
    buildLinux64.includeDirectories.insert("source/");
    buildLinux64.linkedLibraries.insert("xengine");

    BuildSettings buildWin32;
    buildWin32.name = "Windows x64";
    buildWin32.cmakeCommand = "cmake";
    buildWin32.buildDir = "build/windows_x64/";
    buildWin32.targetPlatform = WIN32_64;
    buildWin32.optimization = SAFE_OPTIMIZATIONS;
    buildWin32.gameTargetName = "game";
    buildWin32.pluginTargetName = "game_plugin";
    buildWin32.sourceDirectories.insert("source/");
    buildWin32.includeDirectories.insert("source/");
    buildWin32.linkedLibraries.insert("xengine");

    AssetBundle assetBundle;
    assetBundle.name = "Assets";
    assetBundle.scheme = "assets";
    assetBundle.directory = "assets/";

    ProjectSettings templateProjectSettings;
    templateProjectSettings.name = "New Project";
    templateProjectSettings.assetBundles.emplace_back(assetBundle);
    templateProjectSettings.buildSettings.emplace_back(buildLinux64);
    templateProjectSettings.buildSettings.emplace_back(buildWin32);
    templateProjectSettings.componentDirectories.insert("source/");

    Message msg;
    templateProjectSettings >> msg;

    std::stringstream stream;
    JsonProtocol().serialize(stream, msg);

    auto projectSettingsStr = stream.str();

    auto projectSettingsPath = outputDir;
    projectSettingsPath.append(Paths::projectSettingsFilename().toStdString());

    auto sourceDirectoryPath = outputDir;
    sourceDirectoryPath.append("source/");

    auto assetDirectoryPath = outputDir;
    assetDirectoryPath.append("assets/");

    auto pluginDirectoryPath = outputDir;
    pluginDirectoryPath.append("plugin/");

    auto cmakelistsPath = outputDir;
    cmakelistsPath.append("CMakeLists.txt");

    auto mainPath = sourceDirectoryPath;
    mainPath.append("main.cpp");

    auto gameHeaderPath = sourceDirectoryPath;
    gameHeaderPath.append("game.hpp");

    auto pluginMainPath = pluginDirectoryPath;
    pluginMainPath.append("main.cpp");

    if (!std::filesystem::create_directories(sourceDirectoryPath)) {
        throw std::runtime_error("Failed to create directory: " + sourceDirectoryPath.string());
    }

    if (!std::filesystem::create_directories(assetDirectoryPath)) {
        throw std::runtime_error("Failed to create directory: " + assetDirectoryPath.string());
    }

    if (!std::filesystem::create_directories(pluginDirectoryPath)) {
        throw std::runtime_error("Failed to create directory: " + pluginDirectoryPath.string());
    }

    std::fstream fs;
    fs.exceptions(std::fstream::badbit);

    fs.open(projectSettingsPath, std::fstream::out);
    fs << projectSettingsStr.c_str();
    fs.flush();
    fs.close();

    fs.open(cmakelistsPath.string(), std::fstream::out);
    fs << TEMPLATE_CMAKE_LISTS;
    fs.flush();
    fs.close();

    fs.open(mainPath, std::fstream::out);
    fs << TEMPLATE_MAIN;
    fs.flush();
    fs.close();

    fs.open(gameHeaderPath, std::fstream::out);
    fs << TEMPLATE_GAME_CLASS;
    fs.flush();
    fs.close();

    fs.open(pluginMainPath, std::fstream::out);
    fs << TEMPLATE_PLUGIN_MAIN;
    fs.flush();
    fs.close();
}

void Project::load(const std::filesystem::path &dir) {
    unload();

    // Read and deserialize the settings object
    auto settingsFile = dir;
    settingsFile.append(Paths::projectSettingsFilename().toStdString());
    if (!std::filesystem::exists(settingsFile)) {
        throw std::runtime_error("Settings file " + settingsFile.string() + " not found.");
    }

    std::ifstream fs(settingsFile.string());
    auto msg = xng::JsonProtocol().deserialize(fs);
    settings = {};
    settings << msg;

    // Create and register asset bundle archives
    for (auto &bundle: settings.assetBundles) {
        auto bundleDir = dir;
        bundleDir.append(bundle.directory);
        ResourceRegistry::getDefaultRegistry().addArchive(bundle.scheme,
                                                          std::make_shared<xng::DirectoryArchive>(bundleDir));
    }

    directory = dir;
}

bool Project::unload() {
    if (directory.empty()) {
        return false;
    } else {
        // Remove asset bundle archives
        for (auto &bundle: settings.assetBundles) {
            ResourceRegistry::getDefaultRegistry().removeArchive(bundle.scheme);
        }
        directory = std::filesystem::path();
        settings = {};
        return true;
    }
}

bool Project::isLoaded() {
    return !directory.empty();
}

void Project::compile(const BuildSettings &settings) const {
    // Package asset bundles

    // Copy pak files to output dir

    // Run cmake

    // Copy outputs from cmake to output dir

    // Copy library binaries
}

void Project::save() const {
    auto settingsFile = std::filesystem::path(directory).append(Paths::projectSettingsFilename().toStdString().c_str());
    std::ofstream fs(settingsFile.string());
    Message msg;
    settings >> msg;
    xng::JsonProtocol().serialize(fs, msg);
}

const ProjectSettings &Project::getSettings() const {
    return settings;
}

ProjectSettings &Project::getSettings() {
    return settings;
}

void Project::setSettings(const ProjectSettings &value) {
    settings = value;
}

std::filesystem::path Project::getProjectDirectory() const {
    return directory;
}

std::filesystem::path Project::getPluginDirectory() const {
    return std::filesystem::path(directory).append(Paths::pluginDirectory().toStdString().c_str());
}

std::filesystem::path Project::getPluginLibraryFilePath() const {
    return getPluginDirectory().append(Paths::pluginLibraryFileName().toStdString().c_str());
}

std::set<std::filesystem::path> Project::getSourceDirectories() const {
    std::set<std::filesystem::path> ret;
    for (auto &buildSettings: settings.buildSettings) {
        for (auto &path: buildSettings.sourceDirectories) {
            ret.insert(std::filesystem::path(path));
        }
        for (auto &path: buildSettings.includeDirectories) {
            ret.insert(std::filesystem::path(path));
        }
    }
    return ret;
}

std::set<std::filesystem::path> Project::getAssetDirectories() const {
    std::set<std::filesystem::path> ret;
    for (auto &bundle: settings.assetBundles) {
        ret.insert(getProjectDirectory().append(bundle.directory));
    }
    return ret;
}

