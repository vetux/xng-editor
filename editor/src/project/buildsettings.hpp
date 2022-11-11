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

#include "xng/io/messageable.hpp"

using namespace xng;

struct BuildSettings : public Messageable {
    std::string name{};

    BuildPlatform targetPlatform{};
    BuildOptimization optimization{};

    std::string cmakeCommand{}; // The command to use when invoking cmake

    std::filesystem::path buildDir{}; // The output directory relative to the project directory

    std::string gameTargetName{}; // The name of the game cmake target
    std::string pluginTargetName{}; // The name of the plugin cmake target
    std::set<std::filesystem::path> srcDirs{};
    std::set<std::filesystem::path> incDirs{};
    std::set<std::filesystem::path> lnkDirs{};
    std::set<std::filesystem::path> pluginDirs{};
    std::set<std::string> linkedLibraries{};

    static QString combinePaths(const std::set<std::filesystem::path> &paths) {
        QString ret;
        for (auto &p: paths) {
            ret += p.string().c_str();
            ret += " ";
        }
        ret.remove(ret.size() - 1, 1);
        return ret;
    }

    static QString combineLibraryNames(const std::set<std::string> &names) {
        QString ret;
        for (auto &str: names) {
            ret += str.c_str();
            ret += " ";
        }
        ret.remove(ret.size() - 1, 1);
        return ret;
    }

    std::filesystem::path getBuildDirectory(const std::filesystem::path &projectDir) const {
        return std::filesystem::path(projectDir).append(buildDir.string().c_str());
    }

    void initialize(const std::filesystem::path &projectDir, std::string &output, std::string &error) const {
        if (buildDir.is_absolute()) {
            throw std::runtime_error("Build directory path cannot be absolute");
        }
        auto dir = getBuildDirectory(projectDir);

        std::filesystem::remove_all(dir); // Erase previous build contents

        std::filesystem::create_directories(dir);

        QProcess process;
        process.setWorkingDirectory(dir.string().c_str());
        process.setProgram(cmakeCommand.c_str());
        process.setArguments({"..",
                              "-DEXE_NAME=" + QString(gameTargetName.c_str()),
                              "-DPLUGIN_NAME=" + QString(pluginTargetName.c_str()),
                              "-DSRC_DIR=" + combinePaths(srcDirs),
                              "-DINC_DIR=" + combinePaths(incDirs),
                              "-DPLUGIN_DIR=" + combinePaths(pluginDirs),
                              "-DLNK_DIR=" + combinePaths(lnkDirs),
                              "-DLINK=" + combineLibraryNames(linkedLibraries)});
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
                              "--target " + QString(gameTargetName.c_str())});
        process.start();
        process.waitForFinished();
        output = process.readAllStandardOutput().toStdString();
        error = process.readAllStandardError().toStdString();
    }

    void buildPlugin(const std::filesystem::path &projectDir, std::string &output, std::string &error) const {
        auto dir = getBuildDirectory(projectDir);
        QProcess process;
        process.setWorkingDirectory(dir.string().c_str());
        process.setProgram(cmakeCommand.c_str());
        process.setArguments({"--build",
                              "--target " + QString(pluginTargetName.c_str())});
        process.start();
        process.waitForFinished();
        output = process.readAllStandardOutput().toStdString();
        error = process.readAllStandardError().toStdString();
    }

    std::filesystem::path getPluginLibraryFilePath(const std::filesystem::path &projectDir) const {
        return getBuildDirectory(projectDir)
                .append(Library::getPlatformFilePrefix() + pluginTargetName + Library::getPlatformFileExtension());
    }

    Messageable &operator<<(const Message &message) override {
        name = message.value("name", std::string());
        targetPlatform = (BuildPlatform) message.value("targetPlatform", (int) LINUX_64);
        optimization = (BuildOptimization) message.value("optimization", (int) NO_OPTIMIZATION);
        cmakeCommand = message.value("cmakeCommand", std::string());
        buildDir = message.value("buildDir", std::string());
        gameTargetName = message.value("gameTargetName", std::string());
        pluginTargetName = message.value("pluginTargetName", std::string());
        for (auto &v: message.value("srcDirs").asList()) {
            srcDirs.insert(v.asString());
        }
        for (auto &v: message.value("incDirs").asList()) {
            incDirs.insert(v.asString());
        }
        for (auto &v: message.value("lnkDirs").asList()) {
            lnkDirs.insert(v.asString());
        }
        for (auto &v: message.value("pluginDirs").asList()) {
            pluginDirs.insert(v.asString());
        }
        for (auto &v: message.value("linkedLibraries").asList()) {
            linkedLibraries.insert(v.asString());
        }
        return *this;
    }

    Message &operator>>(Message &message) const override {
        message = Message(xng::Message::DICTIONARY);

        message["name"] = name;

        message["targetPlatform"] = (int) targetPlatform;
        message["optimization"] = (int) optimization;

        message["cmakeCommand"] = cmakeCommand;

        message["buildDir"] = buildDir.string();

        message["gameTargetName"] = gameTargetName;
        message["pluginTargetName"] = pluginTargetName;

        auto vec = std::vector<Message>();
        for (auto &v: srcDirs) {
            vec.emplace_back(v.string());
        }
        message["srcDirs"] = vec;

        vec.clear();
        for (auto &v: incDirs) {
            vec.emplace_back(v.string());
        }
        message["incDirs"] = vec;

        vec.clear();
        for (auto &v: lnkDirs) {
            vec.emplace_back(v.string());
        }
        message["lnkDirs"] = vec;

        vec.clear();
        for (auto &v: pluginDirs) {
            vec.emplace_back(v.string());
        }
        message["pluginDirs"] = vec;

        vec.clear();
        for (auto &v: linkedLibraries) {
            vec.emplace_back(v);
        }
        message["linkedLibraries"] = vec;

        return message;
    }
};

#endif //XEDITOR_BUILDSETTINGS_HPP
