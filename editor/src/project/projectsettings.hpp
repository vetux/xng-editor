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
#ifndef XEDITOR_PROJECTSETTINGS_HPP
#define XEDITOR_PROJECTSETTINGS_HPP

#include <vector>
#include <string>
#include <filesystem>

#include "project/buildsettings.hpp"
#include "project/assetbundle.hpp"

#include "xng/io/messageable.hpp"

using namespace xng;

struct ProjectSettings : public Messageable {
    std::string name; // The name of the project
    std::vector<AssetBundle> assetBundles{}; // The asset bundles
    std::vector<BuildSettings> buildSettings; // The user created build settings.

    Messageable &operator<<(const Message &message) override {
        name = message.value("name", std::string());
        for (auto &v: message.value("assetBundles").asList()) {
            AssetBundle bundle;
            bundle << v;
            assetBundles.emplace_back(bundle);
        }
        for (auto &v: message.value("buildSettings").asList()) {
            BuildSettings settings;
            settings << v;
            buildSettings.emplace_back(settings);
        }
        return *this;
    }

    Message &operator>>(Message &message) const override {
        message = Message(xng::Message::DICTIONARY);

        message["name"] = name;

        auto vec = std::vector<Message>();
        for (auto &v: assetBundles) {
            Message msg;
            v >> msg;
            vec.emplace_back(msg);
        }
        message["assetBundles"] = vec;

        vec.clear();
        for (auto &v: buildSettings) {
            Message msg;
            v >> msg;
            vec.emplace_back(msg);
        }
        message["buildSettings"] = vec;

        return message;
    }
};

#endif //XEDITOR_PROJECTSETTINGS_HPP
