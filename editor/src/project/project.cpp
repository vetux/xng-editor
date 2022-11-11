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

void Project::create(const std::filesystem::path &outputDir, const std::filesystem::path &templateDir) {
    if (!std::filesystem::is_empty(outputDir)) {
        throw std::runtime_error("Output directory is not empty.");
    }
    std::filesystem::copy(templateDir, outputDir, std::filesystem::copy_options::recursive);
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

const std::string &Project::getDirectory() const {
    return directory;
}

