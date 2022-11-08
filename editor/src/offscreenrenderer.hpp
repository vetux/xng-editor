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
        typedef std::function<void(float, std::shared_ptr<ImageRGBA>)> Listener;

        explicit OffscreenRenderer(float frameRate,
                                   Vec2i frameSize)
                : frameRate(frameRate),
                  frameSize(std::move(frameSize)),
                  displayDriver(DisplayDriver::load(GLFW)),
                  gpuDriver(GpuDriver::load(OPENGL_4_6)),
                  shaderCompiler(SPIRVCompiler::load(SHADERC)),
                  shaderDecompiler(SPIRVDecompiler::load(SPIRV_CROSS)),
                  fontDriver(FontDriver::load(FREETYPE)) {
            thread = std::thread([this]() {
                window = displayDriver->createWindow("opengl",
                                                     "Render Window",
                                                     {1, 1},
                                                     {.visible = false});
                device = gpuDriver->createRenderDevice();
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

        void setScene(const EntityScene &value) {
            std::lock_guard<std::mutex> guard(mutex);
            scene = std::make_shared<EntityScene>(value);
            sceneChanged = true;
        }

        void setFrameGraphLayout(const FrameGraphLayout &value) {
            std::lock_guard<std::mutex> guard(mutex);
            layout = value;
            layoutChanged = true;
        }

        std::shared_ptr<ImageRGBA> getFrame() {
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
#ifndef XEDITOR_DEBUGGING
                try {
#endif
                    ren2d->renderClear(*target, ColorRGBA::black(), {}, frameSize);
                    std::lock_guard<std::mutex> guard(mutex);
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
                        RenderTargetDesc rdesc;
                        rdesc.size = frameSize;
                        rdesc.numberOfColorAttachments = 1;
                        target = device->createRenderTarget(rdesc);
                        TextureBufferDesc desc;
                        desc.size = frameSize;
                        desc.bufferType = HOST_VISIBLE;
                        texture = device->createTextureBuffer(desc);
                        target->setColorAttachments({*texture});
                        frameSizeChanged = false;
                        ecs.stop();
                        ecs.setSystems({});
                        frameGraphRenderer = std::make_unique<FrameGraphRenderer>(*target,
                                                                                  std::make_unique<FrameGraphPoolAllocator>(
                                                                                          *device,
                                                                                          *shaderCompiler,
                                                                                          *shaderDecompiler));

                        canvasRenderSystem = std::make_unique<CanvasRenderSystem>(*ren2d, *target, *fontDriver);
                        meshRenderSystem = std::make_unique<MeshRenderSystem>(*frameGraphRenderer);
                        canvasRenderSystem->setDrawDebugGeometry(true);
                        ecs.setSystems({*canvasRenderSystem, *meshRenderSystem});
                        ecs.start();
                    }
                    ecs.update(deltaTime);

                    frame = std::make_shared<ImageRGBA>(texture->download());
#ifndef XEDITOR_DEBUGGING
                } catch (...) {
                    {
                        std::lock_guard<std::mutex> guard(mutex);
                        exception = std::current_exception();
                    }
                    std::rethrow_exception(exception);
                }
#endif

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

        std::shared_ptr<EntityScene> scene;
        bool sceneChanged = false;

        std::unique_ptr<DisplayDriver> displayDriver;
        std::unique_ptr<GpuDriver> gpuDriver;
        std::unique_ptr<SPIRVCompiler> shaderCompiler;
        std::unique_ptr<SPIRVDecompiler> shaderDecompiler;

        std::unique_ptr<FontDriver> fontDriver;

        std::unique_ptr<Window> window;

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

        std::shared_ptr<ImageRGBA> frame;

        std::exception_ptr exception = nullptr;

        Listener callback;
    };
}
#endif //XENGINE_OFFSCREENRENDERER_HPP
