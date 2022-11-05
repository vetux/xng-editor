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

#ifndef XEDITOR_ENTITYWIDGET_HPP
#define XEDITOR_ENTITYWIDGET_HPP

#include <QWidget>
#include <QPushButton>
#include <QScrollArea>

#include "xng/ecs/entity.hpp"

#include "widgets/components/componentwidget.hpp"
#include "widgets/components/canvascomponentwidget.hpp"
#include "widgets/components/canvastransformcomponentwidget.hpp"
#include "widgets/components/rigidbodycomponentwidget.hpp"
#include "widgets/components/spriteanimationcomponentwidget.hpp"
#include "widgets/components/spritecomponentwidget.hpp"
#include "widgets/components/transformcomponentwidget.hpp"

class EntityEditWidget : public QWidget {
Q_OBJECT
public:
    explicit EntityEditWidget(QWidget *parent)
            : QWidget(parent) {
        setEnabled(false);
        setLayout(new QVBoxLayout);

        spacerWidget = new QWidget(this);

        auto *widget = new QWidget(this);
        widget->setLayout(new QHBoxLayout);

        headerText = new QLineEdit(this);

        destroyEntityButton = new QPushButton(this);
        destroyEntityButton->setText("Destroy");

        widget->layout()->addWidget(headerText);
        widget->layout()->addWidget(destroyEntityButton);

        componentParent = new QWidget();
        componentScroll = new QScrollArea(this);
        componentScroll->setFrameShape(QFrame::NoFrame);
        componentScroll->setWidget(componentParent);
        componentScroll->setWidgetResizable(true);

        addComponentContainer = new QWidget();

        addComponentButton = new QPushButton();
        addComponentButton->setText("Add Component");

        auto *addCompLayout = new QHBoxLayout;

        addCompLayout->addWidget(new QWidget, 1);
        addCompLayout->addWidget(addComponentButton, 1);
        addCompLayout->addWidget(new QWidget, 1);

        addComponentContainer->setLayout(addCompLayout);

        auto *compLayout = new QVBoxLayout;
        compLayout->setSpacing(0);

        compLayout->addWidget(addComponentContainer);
        compLayout->addWidget(spacerWidget, 1);

        componentParent->setLayout(compLayout);

        layout()->addWidget(widget);
        layout()->addWidget(componentScroll);

        layout()->setAlignment(Qt::AlignTop);

        connect(destroyEntityButton, SIGNAL(pressed()), this, SLOT(destroyEntityPressed()));
        connect(addComponentButton, SIGNAL(pressed()), this, SIGNAL(addComponent()));
 }

    void setEntity(xng::Entity value) {
        setEnabled(true);

        entity = value;
        for (auto &pair: components) {
            layout()->removeWidget(pair.second);
            pair.second->deleteLater();
        }
        components.clear();

        componentParent->layout()->removeWidget(spacerWidget);
        componentParent->layout()->removeWidget(addComponentContainer);

        if (value.hasName())
            headerText->setText(value.getName().c_str());

        if (entity.checkComponent<AudioListenerComponent>()){

        }

        if (entity.checkComponent<AudioSourceComponent>()) {
            auto *widget = new ComponentWidget(this);
            widget->setTitle("Audio Source");
            addComponentWidget(widget);
            components[typeid(AudioSourceComponent)] = widget;
        }

        if (entity.checkComponent<TransformComponent>()) {
            auto *widget = new TransformComponentWidget(this);
            widget->set(value.getComponent<TransformComponent>());
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
            widget->set(value.getComponent<CanvasTransformComponent>());
            connect(widget,
                    SIGNAL(destroyPressed()),
                    this,
                    SLOT(destroyPressed()));
            connect(widget, SIGNAL(valueChanged(const CanvasTransformComponent &)), this,
                    SLOT(valueChanged(const CanvasTransformComponent &)));
            addComponentWidget(widget);
            components[typeid(CanvasTransformComponent)] = widget;
        }

        componentParent->layout()->addWidget(addComponentContainer);

        auto *lay = (QVBoxLayout *) componentParent->layout();
        lay->addWidget(spacerWidget, 1);

        componentScroll->update();
    }

    void clearEntity() {
        entity = {};
        for (auto &pair: components) {
            layout()->removeWidget(pair.second);
            pair.second->deleteLater();
        }
        components.clear();
        setEnabled(false);
    }

    const Entity &getEntity() const {
        return entity;
    }

signals:

    void addComponent();

    void updateComponent(const Component &component);

    void destroyComponent(std::type_index componentType);

    void updateEntityName(const std::string &name);

    void destroyEntity();

private slots:

    void destroyEntityPressed() {
        emit destroyEntity();
    }

    void destroyPressed() {
        auto *sen = dynamic_cast<ComponentWidget *>(sender());
        auto type = sen->getType();
        if (type == typeid(ComponentWidget)) {
            // User defined component type
        } else if (type == typeid(TransformComponentWidget)) {
            emit destroyComponent(typeid(TransformComponent));
        } else if (type == typeid(CanvasTransformComponentWidget)) {
            emit destroyComponent(typeid(CanvasTransformComponent));
        }
    }

    void valueChanged(const AudioSourceComponent &value) {
        emit updateComponent(value);
    }

    void valueChanged(const CanvasComponent &value) {
        emit updateComponent(value);
    }

    void valueChanged(const CanvasTransformComponent &value) {
        emit updateComponent(value);
    }

    void valueChanged(const RigidBodyComponent &value) {
        emit updateComponent(value);
    }

    void valueChanged(const SpriteAnimationComponent &value) {
        emit updateComponent(value);
    }

    void valueChanged(const SpriteComponent &value) {
        emit updateComponent(value);
    }

    void valueChanged(const TransformComponent &value) {
        emit updateComponent(value);
    }

private:
    void addComponentWidget(ComponentWidget *widget) {
        componentParent->layout()->addWidget(widget);
    }

    xng::Entity entity;

    QLineEdit *headerText;

    std::map<std::type_index, ComponentWidget *> components;

    QWidget *addComponentContainer;
    QPushButton *addComponentButton;

    QScrollArea *componentScroll;

    QWidget *componentParent;

    QWidget *spacerWidget;

    QPushButton *destroyEntityButton;
};

#endif //XEDITOR_ENTITYWIDGET_HPP
