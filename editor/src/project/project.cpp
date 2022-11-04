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

#include "asset/scene.hpp"

void Project::create(const std::filesystem::path &outputDir, const std::filesystem::path &templateDir) {

}

Project::Project(const std::filesystem::path &dir) {
    // Read and deserialize the settings object
}

void Project::compile(const BuildSettings &settings) const {
    // Package asset bundles

    // Copy pak files to output dir

    // Run cmake

    // Copy outputs from cmake to output dir

    // Copy library binaries
}

void Project::save() const {
    // Serialize and write settings object.
}

ProjectSettings &Project::getSettings() {
    return settings;
}

const ProjectSettings &Project::getSettings() const {
    return settings;
}

const std::filesystem::path &Project::getDirectory() const {
    return directory;
}
