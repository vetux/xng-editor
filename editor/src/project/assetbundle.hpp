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
#include <utility>

#include "xng/crypto/aes.hpp"
#include "xng/io/messageable.hpp"

using namespace xng;

struct AssetBundle : public Messageable {
    std::string name{};
    std::string scheme{}; // The scheme under which the bundle is made accessible

    bool compress = true;
    bool encrypt = false;
    xng::AES::Key key{};

    std::string directory; // The name of the directory which is bundled into name.pak relative to the project directory

    Messageable &operator<<(const Message &message) override {
        name = message.value("name", std::string());
        scheme = message.value("scheme", std::string());

        compress = message.value("compress", true);
        encrypt = message.value("encrypt", false);
        key = message.value("key", std::string());

        directory = message.value("directory", std::string());

        return *this;
    }

    Message &operator>>(Message &message) const override {
        message = Message(Message::DICTIONARY);
        message["name"] = name;
        message["scheme"] = scheme;

        message["compress"] = compress;
        message["encrypt"] = encrypt;
        message["key"] = key;

        message["directory"] = directory;

        return message;
    }
};

#endif //XEDITOR_ASSETBUNDLE_HPP
