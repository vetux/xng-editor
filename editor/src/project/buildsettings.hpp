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

#include "project/buildoptimization.hpp"
#include "project/buildplatform.hpp"

#include "xng/io/messageable.hpp"

using namespace xng;

struct BuildSettings : public Messageable {
    std::string name{};

    BuildPlatform targetPlatform{};
    BuildOptimization optimization{};

    std::filesystem::path outputDir{}; // The output directory

    // CMake build configuration definitions
    std::string targetName{};
    std::set<std::filesystem::path> srcDirs{};
    std::set<std::filesystem::path> incDirs{};
    std::set<std::filesystem::path> lnkDirs{};
    std::set<std::string> linkedLibraries{};

    Messageable &operator<<(const Message &message) override {
        name = message.value("name", std::string());
        targetPlatform = (BuildPlatform) message.value("targetPlatform", (int) LINUX_64);
        for (auto &v: message.value("srcDirs").asList()) {
            srcDirs.insert(v.asString());
        }
        for (auto &v: message.value("incDirs").asList()) {
            incDirs.insert(v.asString());
        }
        for (auto &v: message.value("lnkDirs").asList()) {
            lnkDirs.insert(v.asString());
        }
        for (auto &v: message.value("linkedLibraries").asList()) {
            linkedLibraries.insert(v.asString());
        }
        return *this;
    }

    Message &operator>>(Message &message) const override {
        message = Message(xng::Message::DICTIONARY);
        message["name"] = name;
        message[targetPlatform] = (int) targetPlatform;
        message[optimization] = (int) optimization;
        message["outputDir"] = outputDir.string();
        message["targetName"] = targetName;

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
        for (auto &v: linkedLibraries) {
            vec.emplace_back(v);
        }
        message["linkedLibraries"] = vec;

        return message;
    }
};

#endif //XEDITOR_BUILDSETTINGS_HPP
