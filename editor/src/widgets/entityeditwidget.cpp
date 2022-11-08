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

#include "widgets/entityeditwidget.hpp"

#include "widgets/components/audiolistenercomponentwidget.hpp"
#include "widgets/components/audiosourcecomponentwidget.hpp"
#include "widgets/components/buttoncomponentwidget.hpp"
#include "widgets/components/cameracomponentwidget.hpp"
#include "widgets/components/canvascomponentwidget.hpp"
#include "widgets/components/canvastransformcomponentwidget.hpp"
#include "widgets/components/lightcomponentwidget.hpp"
#include "widgets/components/meshrendercomponentwidget.hpp"
#include "widgets/components/rigidbodycomponentwidget.hpp"
#include "widgets/components/skyboxcomponentwidget.hpp"
#include "widgets/components/spriteanimationcomponentwidget.hpp"
#include "widgets/components/spritecomponentwidget.hpp"
#include "widgets/components/textcomponentwidget.hpp"
#include "widgets/components/transformcomponentwidget.hpp"

void EntityEditWidget::createComponentWidgets() {
    if (entity.checkComponent<TransformComponent>()) {
        auto *widget = new TransformComponentWidget(this);
        widget->set(entity.getComponent<TransformComponent>());
        connect(widget,
                SIGNAL(destroyPressed()),
                this,
                SLOT(destroyPressed()));
        connect(widget, SIGNAL(valueChanged(const TransformComponent &)), this,
                SLOT(valueChanged(const TransformComponent &)));
        addComponentWidget(widget);
        components[typeid(TransformComponent)] = widget;
    }

    if (entity.checkComponent<CanvasTransformComponent>()) {
        auto *widget = new CanvasTransformComponentWidget(this);
        widget->set(entity.getComponent<CanvasTransformComponent>());
        connect(widget,
                SIGNAL(destroyPressed()),
                this,
                SLOT(destroyPressed()));
        connect(widget, SIGNAL(valueChanged(const CanvasTransformComponent &)), this,
                SLOT(valueChanged(const CanvasTransformComponent &)));
        addComponentWidget(widget);
        components[typeid(CanvasTransformComponent)] = widget;
    }

    if (entity.checkComponent<AudioListenerComponent>()) {
        auto *widget = new AudioListenerComponentWidget(this);
        widget->set(entity.getComponent<AudioListenerComponent>());
        connect(widget,
                SIGNAL(destroyPressed()),
                this,
                SLOT(destroyPressed()));
        connect(widget, SIGNAL(valueChanged(const AudioListenerComponent &)), this,
                SLOT(valueChanged(const AudioListenerComponent &)));
        addComponentWidget(widget);
        components[typeid(AudioListenerComponent)] = widget;
    }

    if (entity.checkComponent<AudioSourceComponent>()) {
        auto *widget = new AudioSourceComponentWidget(this);
        widget->set(entity.getComponent<AudioSourceComponent>());
        connect(widget,
                SIGNAL(destroyPressed()),
                this,
                SLOT(destroyPressed()));
        connect(widget, SIGNAL(valueChanged(const AudioSourceComponent &)), this,
                SLOT(valueChanged(const AudioSourceComponent &)));
        addComponentWidget(widget);
        components[typeid(AudioSourceComponent)] = widget;
    }

    if (entity.checkComponent<ButtonComponent>()) {
        auto *widget = new ButtonComponentWidget(this);
        widget->set(entity.getComponent<ButtonComponent>());
        connect(widget,
                SIGNAL(destroyPressed()),
                this,
                SLOT(destroyPressed()));
        connect(widget, SIGNAL(valueChanged(const ButtonComponent &)), this,
                SLOT(valueChanged(const ButtonComponent &)));
        addComponentWidget(widget);
        components[typeid(ButtonComponent)] = widget;
    }

    if (entity.checkComponent<CameraComponent>()) {
        auto *widget = new CameraComponentWidget(this);
        widget->set(entity.getComponent<CameraComponent>());
        connect(widget,
                SIGNAL(destroyPressed()),
                this,
                SLOT(destroyPressed()));
        connect(widget, SIGNAL(valueChanged(const CameraComponent &)), this,
                SLOT(valueChanged(const CameraComponent &)));
        addComponentWidget(widget);
        components[typeid(CameraComponent)] = widget;
    }

    if (entity.checkComponent<CanvasComponent>()) {
        auto *widget = new CanvasComponentWidget(this);
        widget->set(entity.getComponent<CanvasComponent>());
        connect(widget,
                SIGNAL(destroyPressed()),
                this,
                SLOT(destroyPressed()));
        connect(widget, SIGNAL(valueChanged(const CanvasComponent &)), this,
                SLOT(valueChanged(const CanvasComponent &)));
        addComponentWidget(widget);
        components[typeid(CanvasComponent)] = widget;
    }

    if (entity.checkComponent<LightComponent>()) {
        auto *widget = new LightComponentWidget(this);
        widget->set(entity.getComponent<LightComponent>());
        connect(widget,
                SIGNAL(destroyPressed()),
                this,
                SLOT(destroyPressed()));
        connect(widget, SIGNAL(valueChanged(const LightComponent &)), this,
                SLOT(valueChanged(const LightComponent &)));
        addComponentWidget(widget);
        components[typeid(LightComponent)] = widget;
    }

    if (entity.checkComponent<MeshRenderComponent>()) {
        auto *widget = new MeshRenderComponentWidget(this);
        widget->set(entity.getComponent<MeshRenderComponent>());
        connect(widget,
                SIGNAL(destroyPressed()),
                this,
                SLOT(destroyPressed()));
        connect(widget, SIGNAL(valueChanged(const MeshRenderComponent &)), this,
                SLOT(valueChanged(const MeshRenderComponent &)));
        addComponentWidget(widget);
        components[typeid(MeshRenderComponent)] = widget;
    }

    if (entity.checkComponent<RigidBodyComponent>()) {
        auto *widget = new RigidBodyComponentWidget(this);
        widget->set(entity.getComponent<RigidBodyComponent>());
        connect(widget,
                SIGNAL(destroyPressed()),
                this,
                SLOT(destroyPressed()));
        connect(widget, SIGNAL(valueChanged(const RigidBodyComponent &)), this,
                SLOT(valueChanged(const RigidBodyComponent &)));
        addComponentWidget(widget);
        components[typeid(RigidBodyComponent)] = widget;
    }

    if (entity.checkComponent<SkyboxComponent>()) {
        auto *widget = new SkyboxComponentWidget(this);
        widget->set(entity.getComponent<SkyboxComponent>());
        connect(widget,
                SIGNAL(destroyPressed()),
                this,
                SLOT(destroyPressed()));
        connect(widget, SIGNAL(valueChanged(const SkyboxComponent &)), this,
                SLOT(valueChanged(const SkyboxComponent &)));
        addComponentWidget(widget);
        components[typeid(SkyboxComponent)] = widget;
    }

    if (entity.checkComponent<SpriteAnimationComponent>()) {
        auto *widget = new SpriteAnimationComponentWidget(this);
        widget->set(entity.getComponent<SpriteAnimationComponent>());
        connect(widget,
                SIGNAL(destroyPressed()),
                this,
                SLOT(destroyPressed()));
        connect(widget, SIGNAL(valueChanged(const SpriteAnimationComponent &)), this,
                SLOT(valueChanged(const SpriteAnimationComponent &)));
        addComponentWidget(widget);
        components[typeid(SpriteAnimationComponent)] = widget;
    }

    if (entity.checkComponent<SpriteComponent>()) {
        auto *widget = new SpriteComponentWidget(this);
        widget->set(entity.getComponent<SpriteComponent>());
        connect(widget,
                SIGNAL(destroyPressed()),
                this,
                SLOT(destroyPressed()));
        connect(widget, SIGNAL(valueChanged(const SpriteComponent &)), this,
                SLOT(valueChanged(const SpriteComponent &)));
        addComponentWidget(widget);
        components[typeid(SpriteComponent)] = widget;
    }


    if (entity.checkComponent<TextComponent>()) {
        auto *widget = new TextComponentWidget(this);
        widget->set(entity.getComponent<TextComponent>());
        connect(widget,
                SIGNAL(destroyPressed()),
                this,
                SLOT(destroyPressed()));
        connect(widget, SIGNAL(valueChanged(const TextComponent &)), this,
                SLOT(valueChanged(const TextComponent &)));
        addComponentWidget(widget);
        components[typeid(TextComponent)] = widget;
    }
}
