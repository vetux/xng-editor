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

#ifndef XEDITOR_BUILDSETTINGS_HPP
#define XEDITOR_BUILDSETTINGS_HPP

#include <set>
#include <filesystem>

#include <QProcess>

#include "project/buildoptimization.hpp"
#include "project/buildplatform.hpp"

#include "xng/io/message.hpp"
#include "xng/io/messageable.hpp"
#include "xng/io/library.hpp"

#include "io/paths.hpp"

using namespace xng;

struct BuildSettings : public Messageable {
    std::string name{}; // The name of this build configuration

    std::string cmakeCommand{}; // The command to use when invoking cmake
    std::string buildDir{}; // The output directory relative to the project directory

    // CMake Configuration variables that are passed as command line arguments when invoking cmake
    BuildPlatform targetPlatform{};
    BuildOptimization optimization{};

    std::string gameTargetName{}; // The name of the game cmake target
    std::string pluginTargetName{}; // The name of the plugin cmake target

    std::set<std::string> sourceDirectories{}; // The list of source directories
    std::set<std::string> includeDirectories{}; // The list of include directories
    std::set<std::string> linkDirectories{}; // The list of link directories
    std::set<std::string> linkedLibraries{}; // The list of linked library names

    static std::string concatCmakeList(const std::set<std::string> &dirs) {
        std::string ret;
        for (auto &dir: dirs)
            ret += dir + " ";
        if (!ret.empty())
            ret.pop_back();
        return ret;
    }

    static std::set<std::string> splitCmakeList(const std::string &dirs) {
        std::set<std::string> ret;
        size_t it = 0;
        size_t last = 0;
        for (it = dirs.find(' '); it != std::string::npos; last = it, it = dirs.find(' ', it + 1)) {
            auto begin = last;
            if (begin > 0)
                begin += 1;
            auto count = it - begin;
            if (begin > 0)
                count -= 1;
            auto str = dirs.substr(begin, count);
            ret.insert(str);
        }
        if (last > 0)
            last += 1;
        if (last < dirs.size()) {
            ret.insert(dirs.substr(last));
        }
        return ret;
    }

    std::filesystem::path getBuildDirectory(const std::filesystem::path &projectDir) const {
        return std::filesystem::path(projectDir).append(buildDir.c_str());
    }

    void initialize(const std::filesystem::path &projectDir, std::string &output, std::string &error) const {
        if (std::filesystem::path(buildDir).is_absolute()) {
            throw std::runtime_error("Build directory path cannot be absolute");
        }

        auto dir = getBuildDirectory(projectDir);

        std::filesystem::remove_all(dir); // Erase previous build contents

        std::filesystem::create_directories(dir);

        QProcess process;
        process.setWorkingDirectory(dir.string().c_str());
        process.setProgram(cmakeCommand.c_str());
        process.setArguments({projectDir.string().c_str(),
                              "-DEXE_NAME=" + QString(gameTargetName.c_str()),
                              "-DPLUGIN_NAME=" + QString(pluginTargetName.c_str()),
                              "-DSRC_DIR=" + QString(concatCmakeList(sourceDirectories).c_str()),
                              "-DINC_DIR=" + QString(concatCmakeList(includeDirectories).c_str()),
                              "-DLNK_DIR=" + QString(concatCmakeList(linkDirectories).c_str()),
                              "-DLINK=" + QString(concatCmakeList(linkedLibraries).c_str())});
        process.start();
        process.waitForFinished();
        output = process.readAllStandardOutput().toStdString();
        error = process.readAllStandardError().toStdString();
    }

    void buildTarget(const std::filesystem::path &projectDir, std::string &output, std::string &error) const {
        auto dir = getBuildDirectory(projectDir);
        QProcess process;
        process.setWorkingDirectory(dir.string().c_str());
        process.setProgram(cmakeCommand.c_str());
        process.setArguments({"--build",
                              ".",
                              "--target",
                              QString(gameTargetName.c_str())});
        process.start();
        process.waitForFinished();
        output = process.readAllStandardOutput().toStdString();
        error = process.readAllStandardError().toStdString();
    }

    void buildPlugin(const std::filesystem::path &projectDir,
                     std::string &output,
                     std::string &error) const {
        {
            auto dir = getBuildDirectory(projectDir);
            QProcess process;
            process.setWorkingDirectory(dir.string().c_str());
            process.setProgram(cmakeCommand.c_str());
            process.setArguments({"--build",
                                  ".",
                                  "--target",
                                  QString(pluginTargetName.c_str())});
            process.start();
            process.waitForFinished();
            output = process.readAllStandardOutput().toStdString();
            error = process.readAllStandardError().toStdString();
        }
    }

    std::filesystem::path getBuiltPluginLibraryFilePath(const std::filesystem::path &projectDir) const {
        return getBuildDirectory(projectDir)
                .append(Library::getPlatformFilePrefix() + pluginTargetName + Library::getPlatformFileExtension());
    }

    Messageable &operator<<(const Message &message) override {
        message.value("name", name, std::string());
        message.value("targetPlatform", (int &) targetPlatform, (int) LINUX_64);
        message.value("optimization", (int &) optimization, (int) NO_OPTIMIZATION);
        message.value("cmakeCommand", cmakeCommand, std::string());
        message.value("buildDir", buildDir, std::string());
        message.value("gameTargetName", gameTargetName, std::string());
        message.value("pluginTargetName", pluginTargetName, std::string());
        message.value("sourceDirectories", sourceDirectories, {});
        message.value("includeDirectories", includeDirectories, {});
        message.value("linkDirectories", linkDirectories, {});
        message.value("linkedLibraries", linkedLibraries, {});
        return *this;
    }

    Message &operator>>(Message &message) const override {
        message = Message(xng::Message::DICTIONARY);

        name >> message["name"];

        targetPlatform >> message["targetPlatform"];
        optimization >> message["optimization"];

        cmakeCommand >> message["cmakeCommand"];

        buildDir >> message["buildDir"];

        gameTargetName >> message["gameTargetName"];
        pluginTargetName >> message["pluginTargetName"];

        sourceDirectories >> message["sourceDirectories"];
        includeDirectories >> message["includeDirectories"];
        linkDirectories >> message["linkDirectories"];
        linkedLibraries >> message["linkedLibraries"];

        return message;
    }
};

#endif //XEDITOR_BUILDSETTINGS_HPP
