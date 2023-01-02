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
#ifndef XNG_EDITOR_PATHS_HPP
#define XNG_EDITOR_PATHS_HPP

#include <QCoreApplication>
#include <QString>
#include <QStandardPaths>

#include <filesystem>

#include "xng/io/library.hpp"

namespace Paths {
    static inline std::filesystem::path dataDirPath() {
        auto ret = std::filesystem::path(
                QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdString());
        std::filesystem::create_directories(ret);
        return ret;
    }

    static inline std::filesystem::path configDirPath() {
        auto ret = std::filesystem::path(
                QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation).toStdString());
        std::filesystem::create_directories(ret);
        return ret;
    }

    static inline std::filesystem::path settingsPath() {
        return configDirPath().string() + "/settings.json";
    }

    static inline std::filesystem::path recentProjectsPath() {
        return dataDirPath().string() + "/recent-projects.json";
    }

    static inline std::filesystem::path stateFilePath() {
        return dataDirPath().string() + "/state.json";
    }

    static inline QString projectSettingsFilename() {
        return "project-settings.json";
    }

    static inline QString pluginDirectory() {
        return "plugin/";
    }

    static inline QString pluginLibraryFileName() {
        return xng::Library::getPlatformFilePrefix() + QString("plugin") + xng::Library::getPlatformFileExtension();
    }
}
#endif //XNG_EDITOR_PATHS_HPP
