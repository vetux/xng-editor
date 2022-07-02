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

#include "xengine.hpp"

using namespace xng;

class Game : public Application {
public:
    Game(int argc, char *argv[])
            : Application(argc, argv),
              dataArchive(std::make_unique<DirectoryArchive>(std::filesystem::current_path() / "data/")),
              frameGraphRenderer(*renderDevice),
              renderSystem(window->getRenderTarget(), frameGraphRenderer),
              audioSystem(*audioDevice, ResourceRegistry::getDefaultRegistry()),
              assetPakStream(dataArchive->open("assets.pak")),
              assetPakArchive(std::make_shared<PakArchive>(
                      std::vector<std::reference_wrapper<std::istream>>({*assetPakStream}))) {
        ResourceRegistry::getDefaultRegistry().addArchive("assets", assetPakArchive);
    }

    void start() override {
        EntityContainer c;
        ecs.setEntityContainer(c << JsonProtocol().deserialize(*assetPakArchive->open("/defaultScene.json")));
        ecs.setSystems({
                               audioSystem,
                               renderSystem
                       });
        ecs.start();
    }

    void stop() override {
        ecs.stop();
    }

    void update(float deltaTime) override {
        ecs.update(deltaTime);
    }

private:
    ECS ecs;

    std::unique_ptr<DirectoryArchive> dataArchive;

    std::unique_ptr<std::istream> assetPakStream;
    std::shared_ptr<PakArchive> assetPakArchive;

    FrameGraphRenderer frameGraphRenderer;

    RenderSystem renderSystem;
    AudioSystem audioSystem;
};

#endif //XEDITOR_EDITORAPPLICATION_HPP
