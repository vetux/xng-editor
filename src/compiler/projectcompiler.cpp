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

#include "compiler/projectcompiler.hpp"

#include <fstream>

ProjectCompiler &ProjectCompiler::setSettings(const BuildSettings &settings) {
    buildSettings = settings;
    return *this;
}

void ProjectCompiler::compile() {
    // Package assets

    // Copy Packaged assets to output dir

    // Create the CMakeLists.txt file
    auto cmSrc = getCMakeSource();
    std::ofstream fs(buildSettings.outputDir + "/CMakeLists.txt");
    fs << cmSrc;
    fs.close();

    // Run Cmake

    // Copy outputs from cmake

    // Copy library binaries
}

std::string ProjectCompiler::getCMakeSource() {
    throw std::runtime_error("Not Implemented");
}
