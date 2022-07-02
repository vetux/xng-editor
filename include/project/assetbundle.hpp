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
#ifndef XEDITOR_ASSETBUNDLE_HPP
#define XEDITOR_ASSETBUNDLE_HPP

#include <set>
#include <filesystem>

#include "crypto/aes.hpp"

struct AssetBundle {
    AssetBundle() = default;

    AssetBundle(std::string name,
                std::set<std::filesystem::path> directories)
            : name(std::move(name)), directories(std::move(directories)) {}

    AssetBundle(std::string name,
                xng::AES::Key key,
                std::set<std::filesystem::path> directories)
            : name(std::move(name)), key(std::move(key)), directories(std::move(directories)) {}

    AssetBundle(std::string name,
                bool compress,
                bool encrypt,
                xng::AES::Key key,
                std::set<std::filesystem::path> directories)
            : name(std::move(name)),
              compress(compress),
              encrypt(encrypt),
              key(std::move(key)),
              directories(std::move(directories)) {}

    std::string name{}; // The bundle is made accessible as the "name" scheme

    bool compress = true;
    bool encrypt = false;
    xng::AES::Key key{};

    std::set<std::filesystem::path> directories{}; // The set of directory paths (relative to the project path) which are bundled into name.pak
};

#endif //XEDITOR_ASSETBUNDLE_HPP
