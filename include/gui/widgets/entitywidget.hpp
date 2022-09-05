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

#include "ecs/entity.hpp"

#include "gui/widgets/components/componentwidget.hpp"
#include "gui/widgets/components/canvascomponentwidget.hpp"
#include "gui/widgets/components/canvastransformcomponentwidget.hpp"
#include "gui/widgets/components/rigidbodycomponentwidget.hpp"
#include "gui/widgets/components/spriteanimationcomponentwidget.hpp"
#include "gui/widgets/components/spritecomponentwidget.hpp"
#include "gui/widgets/components/transformcomponentwidget.hpp"

class EntityWidget : public QWidget {
Q_OBJECT
public:
    explicit EntityWidget(QWidget *parent)
            : QWidget(parent) {
        setLayout(new QVBoxLayout);
        auto * widget = new QWidget(this);
        widget->setLayout(new QHBoxLayout);

        auto *label = new QLabel(this);
        label->setText("Entity Name:");

        headerText = new QLineEdit(this);

        widget->layout()->addWidget(label);
        widget->layout()->addWidget(headerText);

        layout()->addWidget(widget);

        addComponentButton = new QPushButton(this);
        widget = new QWidget(this);
        widget->setLayout(new QHBoxLayout);
        widget->layout()->addWidget(addComponentButton);

        layout()->addWidget(addComponentButton);
        connect(addComponentButton, SIGNAL(pressed()), this, SIGNAL(addComponent()));

        layout()->setAlignment(Qt::AlignTop);

        addComponentButton->setText("Add Component");
    }

    void setEntity(xng::Entity value) {
        entity = value;
        for (auto &pair: components) {
            pair.second->deleteLater();
        }
        components.clear();

        if (entity.checkComponent<TransformComponent>()) {
            auto *widget = new TransformComponentWidget(this);
            connect(widget, SIGNAL(valueChanged(const TransformComponent &)), this,
                    SLOT(valueChanged(const TransformComponent &)));
            auto *lay = (QVBoxLayout*)layout();
            lay->insertWidget(lay->count() - 1, widget);
        }
    }

    const Entity &getEntity() const {
        return entity;
    }

signals:
    void addComponent();

    void componentChanged(std::any component, std::type_index componentType);

    void nameChanged(const std::string &name);

private slots:
    void valueChanged(const AudioSourceComponent &value) {
        emit componentChanged(value, typeid(AudioSourceComponent));
    }

    void valueChanged(const CanvasComponent &value) {
        emit componentChanged(value, typeid(CanvasComponent));
    }

    void valueChanged(const CanvasTransformComponent &value) {
        emit componentChanged(value, typeid(CanvasTransformComponent));
    }

    void valueChanged(const RigidBodyComponent &value) {
        emit componentChanged(value, typeid(RigidBodyComponent));
    }

    void valueChanged(const SpriteAnimationComponent &value) {
        emit componentChanged(value, typeid(SpriteAnimationComponent));
    }

    void valueChanged(const SpriteComponent &value) {
        emit componentChanged(value, typeid(SpriteComponent));
    }

    void valueChanged(const TransformComponent &value) {
        emit componentChanged(value, typeid(TransformComponent));
    }

private:
    xng::Entity entity;

    QLineEdit *headerText;

    std::map<std::type_index, ComponentWidget *> components;

    QPushButton *addComponentButton;
};

#endif //XEDITOR_ENTITYWIDGET_HPP
