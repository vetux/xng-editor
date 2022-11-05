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
#ifndef XEDITOR_EDITORAPPLICATION_HPP
#define XEDITOR_EDITORAPPLICATION_HPP

#include "xng/xng.hpp"

#include "systems/examplesystem.hpp"

using namespace xng;

class Game : public Application {
public:
    Game(int argc, char *argv[])
            : Application(argc, argv),
              dataArchive(std::make_unique<DirectoryArchive>(std::filesystem::current_path() / "data/")),
              assetPakStream(dataArchive->open("assets.pak")),
              cryptoDriver(DriverRegistry::load<CryptoDriver>("cryptopp")),
              zip(cryptoDriver->createGzip()),
              sha(cryptoDriver->createSHA()),
              assetPakArchive(std::make_shared<PakArchive>(
                      std::vector<std::reference_wrapper<std::istream>>({*assetPakStream}),
                      false,
                      *zip,
                      *sha)){
        ResourceRegistry::getDefaultRegistry().addArchive("assets", assetPakArchive);
    }

    void start() override {
        ecs.start();
    }

    void stop() override {
        ecs.stop();
    }

    void update(float deltaTime) override {
        ecs.update(deltaTime);
    }

private:
    std::unique_ptr<CryptoDriver> cryptoDriver;

    ECS ecs;

    std::unique_ptr<GZip> zip;
    std::unique_ptr<SHA> sha;

    std::unique_ptr<DirectoryArchive> dataArchive;

    std::unique_ptr<std::istream> assetPakStream;
    std::shared_ptr<PakArchive> assetPakArchive;
};

#endif //XEDITOR_EDITORAPPLICATION_HPP
