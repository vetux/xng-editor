/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2022  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef XENGINE_OFFSCREENRENDERER_HPP
#define XENGINE_OFFSCREENRENDERER_HPP

#include <thread>
#include <utility>

#include "xng/xng.hpp"

namespace xng {
    /**
     * Starts a separate thread, creates a invisible window and runs a ECS with render systems.
     * Results can be retrieved by calling getFrame.
     */
    class OffscreenRenderer {
    public:
        typedef std::function<void(float, const ImageRGBA &)> Listener;

        explicit OffscreenRenderer(float frameRate,
                                   Vec2i frameSize)
                : frameRate(frameRate),
                  frameSize(std::move(frameSize)),
                  displayDriver(DriverLoader::load<DisplayDriver>("glfw")),
                  gpuDriver(DriverLoader::load<GpuDriver>("opengl")),
                  shaderCompiler(DriverLoader::load<SPIRVCompiler>("shaderc")),
                  shaderDecompiler(DriverLoader::load<SPIRVDecompiler>("spirv-cross")),
                  fontDriver(DriverLoader::load<FontDriver>("freetype")) {
            thread = std::thread([this]() {
                window = displayDriver->createWindow("opengl",
                                                     "Render Window",
                                                     {1, 1},
                                                     {.visible = false});
                gpu = DriverLoader::load<GpuDriver>("opengl");
                device = gpu->createRenderDevice();
                target = device->createRenderTarget(RenderTargetDesc{.size = this->frameSize,
                        .multisample = false,
                        .numberOfColorAttachments = 1});
                TextureBufferDesc desc;
                desc.size = this->frameSize;
                desc.bufferType = HOST_VISIBLE;
                texture = device->createTextureBuffer(desc);
                target->setColorAttachments({*texture});

                ren2d = std::make_unique<Renderer2D>(*device, *shaderCompiler, *shaderDecompiler);

                frameGraphRenderer = std::make_unique<FrameGraphRenderer>(*target,
                                                                          std::make_unique<FrameGraphPoolAllocator>(
                                                                                  *device,
                                                                                  *shaderCompiler,
                                                                                  *shaderDecompiler));

                canvasRenderSystem = std::make_unique<CanvasRenderSystem>(*ren2d, *target, *fontDriver);
                meshRenderSystem = std::make_unique<MeshRenderSystem>(*frameGraphRenderer);
                canvasRenderSystem->setDrawDebugGeometry(true);
                ecs.setSystems({*canvasRenderSystem, *meshRenderSystem});
                ecs.setScene(std::make_shared<EntityScene>());
                ecs.start();
                loop();
                ecs.stop();
            });
        }

        ~OffscreenRenderer() {
            shutdown = true;
            thread.join();
            if (exception) {
                std::rethrow_exception(exception);
            }
        }

        void setScene(const std::shared_ptr<EntityScene> &value, const std::shared_ptr<std::mutex> &mValue) {
            std::lock_guard<std::mutex> guard(mutex);
            scene = value;
            sceneChanged = true;
            sceneMutex = mValue;
        }

        void setFrameGraphLayout(const FrameGraphLayout &value) {
            std::lock_guard<std::mutex> guard(mutex);
            layout = value;
            layoutChanged = true;
        }

        ImageRGBA getFrame() {
            std::lock_guard<std::mutex> guard(mutex);
            return frame;
        }

        void setFrameSize(const Vec2i &size) {
            std::lock_guard<std::mutex> guard(mutex);
            frameSize = size;
            frameSizeChanged = true;
        }

        std::exception_ptr getException() {
            std::lock_guard<std::mutex> guard(mutex);
            return exception;
        }

        bool isShutdown() const {
            return shutdown;
        }

        void setListener(const Listener &v) {
            std::lock_guard<std::mutex> guard(mutex);
            callback = v;
        }

    private:
        void loop() {
            auto frameStart = std::chrono::high_resolution_clock::now();
            auto lastFrame = std::chrono::high_resolution_clock::now();
            DeltaTime deltaTime = 0;
            while (!shutdown) {
             //   try {
                    ren2d->renderClear(*target, ColorRGBA::black(), {}, frameSize);
                    std::lock_guard<std::mutex> guard(mutex);
                    std::lock_guard<std::mutex> sGuard(*sceneMutex);
                    if (layoutChanged) {
                        frameGraphRenderer->setLayout(layout);
                        layoutChanged = false;
                    }
                    if (sceneChanged) {
                        ecs.setScene(scene);
                        sceneChanged = false;
                    }
                    if (frameSizeChanged) {
                        target->setColorAttachments({});
                        TextureBufferDesc desc;
                        desc.size = frameSize;
                        desc.bufferType = HOST_VISIBLE;
                        texture = device->createTextureBuffer(desc);
                        target->setColorAttachments({*texture});
                        frameSizeChanged = false;
                    }
                    ecs.update(deltaTime);
                    frame = texture->download();
             /*   } catch (...) {
                    {
                        std::lock_guard<std::mutex> guard(mutex);
                        exception = std::current_exception();
                    }
                    shutdown = true;
                    std::rethrow_exception(exception);
                }*/

                auto frameEnd = std::chrono::high_resolution_clock::now();
                auto frameDelta = frameEnd - frameStart;
                frameStart = frameEnd;
                deltaTime = static_cast<DeltaTime>(frameDelta.count()) / 1000000000.0f;

                if (callback) {
                    callback(deltaTime, frame);
                }

                auto frameTime = 1.0f / frameRate;
                if (deltaTime < frameTime) {
                    std::this_thread::sleep_for(std::chrono::milliseconds((size_t) ((frameTime - deltaTime) * 1000)));
                }
            }
        }

        std::thread thread;
        std::mutex mutex;

        bool shutdown = false;

        float frameRate = 30;

        Vec2i frameSize = {10, 10};
        bool frameSizeChanged = false;

        std::shared_ptr<std::mutex> sceneMutex;
        std::shared_ptr<EntityScene> scene;
        bool sceneChanged = false;

        std::unique_ptr<DisplayDriver> displayDriver;
        std::unique_ptr<GpuDriver> gpuDriver;
        std::unique_ptr<SPIRVCompiler> shaderCompiler;
        std::unique_ptr<SPIRVDecompiler> shaderDecompiler;

        std::unique_ptr<FontDriver> fontDriver;

        std::unique_ptr<Window> window;
        std::unique_ptr<GpuDriver> gpu;

        std::unique_ptr<RenderDevice> device;
        std::unique_ptr<RenderTarget> target;
        std::unique_ptr<TextureBuffer> texture;

        std::unique_ptr<Renderer2D> ren2d;

        FrameGraphLayout layout;
        bool layoutChanged = false;

        std::unique_ptr<FrameGraphPoolAllocator> frameGraphAllocator;
        std::unique_ptr<FrameGraphRenderer> frameGraphRenderer;

        std::unique_ptr<CanvasRenderSystem> canvasRenderSystem;
        std::unique_ptr<MeshRenderSystem> meshRenderSystem;

        ECS ecs;

        ImageRGBA frame;

        std::exception_ptr exception = nullptr;

        Listener callback;
    };
}
#endif //XENGINE_OFFSCREENRENDERER_HPP
