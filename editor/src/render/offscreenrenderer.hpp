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

#include "xng/driver/glfw/glfwdisplaydriver.hpp"
#include "xng/driver/opengl/oglgpudriver.hpp"
#include "xng/driver/freetype/ftfontdriver.hpp"
#include "xng/driver/spirv-cross/spirvcrossdecompiler.hpp"
#include "xng/driver/glslang/glslangcompiler.hpp"

using namespace xng;

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
              frameSize(std::move(frameSize)) {
        thread = std::thread([this]() {
            displayDriver = std::make_unique<glfw::GLFWDisplayDriver>();
            gpuDriver = std::make_unique<opengl::OGLGpuDriver> ();
            shaderCompiler = std::make_unique<glslang::GLSLangCompiler>();
            shaderDecompiler = std::make_unique<spirv_cross::SpirvCrossDecompiler>();
            fontDriver = std::make_unique<freetype::FtFontDriver>();

            window = displayDriver->createWindow(xng::OPENGL_4_6,
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
            target->setAttachments({RenderTargetAttachment::texture(*texture)});

            ren2d = std::make_unique<Renderer2D>(*device, *shaderCompiler, *shaderDecompiler);

            frameGraphRenderer = std::make_unique<FrameGraphRenderer>(std::make_unique<FrameGraphRuntimeSimple>(
                    *target,
                    *device,
                    *shaderCompiler,
                    *shaderDecompiler));

            canvasRenderSystem = std::make_shared<CanvasRenderSystem>(*ren2d, *target, *fontDriver);
            meshRenderSystem = std::make_shared<MeshRenderSystem>(*frameGraphRenderer);
            canvasRenderSystem->setDrawDebugGeometry(true);
            runtime.setPipelines({
                                         SystemPipeline({canvasRenderSystem, meshRenderSystem})
                                 });
            runtime.setScene(std::make_shared<EntityScene>());
            runtime.start();
            loop();
            runtime.stop();
        });
    }

    ~OffscreenRenderer() {
        if (!shutdown) {
            shutdown = true;
            thread.join();
        }
        if (exception) {
            std::rethrow_exception(exception);
        }
    }

    void setScene(const EntityScene &value) {
        std::lock_guard<std::mutex> guard(mutex);
        scene = std::make_shared<EntityScene>(value);
        sceneChanged = true;
    }

    void setFrameGraphPipeline(const FrameGraphPipeline &value) {
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

    void shutdownThread() {
        shutdown = true;
        thread.join();
        runtime = SystemRuntime();
        scene = {};
        if (exception) {
            std::rethrow_exception(exception);
        }
    }

private:
    void loop() {
        auto frameStart = std::chrono::high_resolution_clock::now();
        auto lastFrame = std::chrono::high_resolution_clock::now();
        DeltaTime deltaTime;
        while (!shutdown) {
#ifndef XEDITOR_DEBUGGING
            try {
#endif
            ren2d->renderClear(*target, ColorRGBA::black(), {}, frameSize);
            std::lock_guard<std::mutex> guard(mutex);
            if (layoutChanged) {
                frameGraphRenderer->setPipeline(layout);
                layoutChanged = false;
            }
            if (sceneChanged) {
                runtime.setScene(scene);
                sceneChanged = false;
            }
            if (frameSizeChanged) {
                target->clearAttachments();
                RenderTargetDesc rdesc;
                rdesc.size = frameSize;
                rdesc.numberOfColorAttachments = 1;
                target = device->createRenderTarget(rdesc);
                TextureBufferDesc desc;
                desc.size = frameSize;
                desc.bufferType = HOST_VISIBLE;
                texture = device->createTextureBuffer(desc);
                target->setAttachments({RenderTargetAttachment::texture(*texture)});
                frameSizeChanged = false;
                runtime.stop();
                runtime.setPipelines({});
                frameGraphRenderer = std::make_unique<FrameGraphRenderer>(std::make_unique<FrameGraphRuntimeSimple>(
                        *target,
                        *device,
                        *shaderCompiler,
                        *shaderDecompiler));

                canvasRenderSystem = std::make_unique<CanvasRenderSystem>(*ren2d, *target, *fontDriver);
                meshRenderSystem = std::make_unique<MeshRenderSystem>(*frameGraphRenderer);
                canvasRenderSystem->setDrawDebugGeometry(true);
                runtime.setPipelines({
                                             SystemPipeline({canvasRenderSystem, meshRenderSystem})
                                     });
                runtime.start();
            }

            runtime.update(deltaTime);

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

    std::unique_ptr<glfw::GLFWDisplayDriver> displayDriver;
    std::unique_ptr<opengl::OGLGpuDriver> gpuDriver;
    std::unique_ptr<glslang::GLSLangCompiler> shaderCompiler;
    std::unique_ptr<spirv_cross::SpirvCrossDecompiler> shaderDecompiler;
    std::unique_ptr<freetype::FtFontDriver> fontDriver;

    std::unique_ptr<Window> window;

    std::unique_ptr<RenderDevice> device;
    std::unique_ptr<RenderTarget> target;
    std::unique_ptr<TextureBuffer> texture;

    std::unique_ptr<Renderer2D> ren2d;

    FrameGraphPipeline layout;
    bool layoutChanged = false;

    std::unique_ptr<FrameGraphRenderer> frameGraphRenderer;

    std::shared_ptr<CanvasRenderSystem> canvasRenderSystem;
    std::shared_ptr<MeshRenderSystem> meshRenderSystem;

    SystemRuntime runtime;

    std::shared_ptr<ImageRGBA> frame;

    std::exception_ptr exception = nullptr;

    Listener callback;
};

#endif //XENGINE_OFFSCREENRENDERER_HPP
