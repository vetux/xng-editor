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

#include "compiler/buildoptimization.hpp"
#include "compiler/buildplatform.hpp"

struct BuildSettings {
    std::string includeDir{}; // User include path
    std::string sourceDir{}; // User source files
    std::string assetDir{}; // Assets directory which is packed into dirName.pack
    std::string linkDir{}; // Directory containing the library binaries (Engine and dependencies) for the target platform.
    std::string outputDir{}; // The output directory

    BuildPlatform targetPlatform;
    BuildOptimization optimization{};
};

#endif //XEDITOR_BUILDSETTINGS_HPP
