/**
 *  xEditor - Editor and tools for creating games
 *  Copyright (C) 2023  Julian Zampiccoli
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
#ifndef XEDITOR_PROJECT_HPP
#define XEDITOR_PROJECT_HPP

#include <string>
#include <vector>
#include <filesystem>

#include "project/buildsettings.hpp"
#include "project/projectsettings.hpp"

#include "xng/io/archive.hpp"

class Project {
public:
    /**
     * Create the files required for a new loadable project in the specified directory.
     * Output Dir must be empty and writeable.
     *
     * @param outputDir
     */
    static void createNewProject(const std::filesystem::path &outputDir);

    Project() = default;

    /**
     * Load the project in the specified directory.
     * Unsaved changes in the current project are discarded.
     *
     * Calls unload() before loading the new project.
     *
     * While the project is loaded the asset bundles are available on the resource registry of the editor.
     *
     * @param dir
     */
    void load(const std::filesystem::path &dir);

    /**
     * Unload the currently loaded project.
     *
     * @return True if a project was loaded and has been unloaded otherwise false
     */
    bool unload();

    bool isLoaded();

    /**
     * Compile the project using the specified settings
     *
     * @param settings
     */
    void compile(const BuildSettings &settings) const;

    /**
     * Save the project settings.
     */
    void save() const;

    const ProjectSettings &getSettings() const;

    ProjectSettings &getSettings();

    void setSettings(const ProjectSettings &settings);

    std::filesystem::path getProjectDirectory() const;

    std::filesystem::path getPluginDirectory() const;

    std::filesystem::path getPluginLibraryFilePath() const;

    /**
     * @return The sourceDirectories and includeDirectories entries of all build settings appended to the project directory.
     */
    std::set<std::filesystem::path> getSourceDirectories() const;

    /**
     * @return The asset bundle directories of all build settings appended to the project directory..
     */
    std::set<std::filesystem::path> getAssetDirectories() const;

    bool initialized() const {
        return !directory.empty();
    }

private:
    std::filesystem::path directory;
    ProjectSettings settings;
};

#endif //XEDITOR_PROJECT_HPP
