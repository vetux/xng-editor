/**
 *  xEditor - Editor and tools for creating games
 *  Copyright (C) 2023  Julian Zampiccoli
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
#ifndef XEDITOR_BUTTONCOMPONENTWIDGET_HPP
#define XEDITOR_BUTTONCOMPONENTWIDGET_HPP

#include "xng/xng.hpp"

#include "widgets/components/componentwidget.hpp"
#include "widgets/vector3widget.hpp"
#include "widgets/uriwidget.hpp"

class ButtonComponentWidget : public ComponentWidget {
Q_OBJECT
public:
    explicit ButtonComponentWidget(QWidget *parent = nullptr)
            : ComponentWidget(parent) {
        idWidget = new QLineEdit;
        layerWidget = new QSpinBox;

        spriteWidget = new UriWidget;
        spriteHoverWidget = new UriWidget;
        spritePressedWidget = new UriWidget;

        layout()->addWidget(new QLabel("ID"));
        layout()->addWidget(idWidget);
        layout()->addWidget(new QLabel("Layer"));
        layout()->addWidget(layerWidget);
        layout()->addWidget(new QLabel("Sprite"));
        layout()->addWidget(spriteWidget);
        layout()->addWidget(new QLabel("Sprite Hover"));
        layout()->addWidget(spriteHoverWidget);
        layout()->addWidget(new QLabel("Sprite Pressed"));
        layout()->addWidget(spritePressedWidget);

        layerWidget->setRange(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());

        connect(idWidget,
                SIGNAL(textEdited(const QString &)),
                this,
                SLOT(onIdChanged(const QString &)));
        connect(layerWidget,
                SIGNAL(valueChanged(int)),
                this,
                SLOT(onLayerChanged(int)));
        connect(spriteWidget,
                SIGNAL(valueChanged(const QString &)),
                this,
                SLOT(onSpriteChanged(const QString &)));
        connect(spriteHoverWidget,
                SIGNAL(valueChanged(const QString &)),
                this,
                SLOT(onSpriteHoverChanged(const QString &)));
        connect(spritePressedWidget,
                SIGNAL(valueChanged(const QString &)),
                this,
                SLOT(onSpritePressedChanged(const QString &)));

        headerText->setText("Button");
    }

    void set(const ButtonComponent &value) {
        component = value;
        idWidget->setText(value.id.c_str());
        layerWidget->setValue(value.layer);
        spriteWidget->setValue(value.sprite.getUri());
        spriteHoverWidget->setValue(value.spriteHover.getUri());
        spritePressedWidget->setValue(value.spritePressed.getUri());
        headerCheckBox->setChecked(component.enabled);
    }

    const ButtonComponent &get() const {
        return component;
    }

    virtual std::type_index getType() override {
        return component.getType();
    }

    std::type_index getComponentType() override {
        return typeid(ButtonComponent);
    }

signals:

    void valueChanged(const ButtonComponent &value);

protected:
    void checkBoxStateChange(int state) override {
        component.enabled = state == Qt::Checked;
        emit valueChanged(component);
    }

private slots:

    void onIdChanged(const QString &value) {
        component.id = value.toStdString().c_str();
        emit valueChanged(component);
    }

    void onLayerChanged(int layer) {
        component.layer = layer;
        emit valueChanged(component);
    }

    void onSpriteChanged(const QString &uri) {
        component.sprite = ResourceHandle<Sprite>(Uri(uri.toStdString().c_str()));
        emit valueChanged(component);
    }

    void onSpriteHoverChanged(const QString &uri) {
        component.spriteHover = ResourceHandle<Sprite>(Uri(uri.toStdString().c_str()));
        emit valueChanged(component);
    }

    void onSpritePressedChanged(const QString &uri) {
        component.spritePressed = ResourceHandle<Sprite>(Uri(uri.toStdString().c_str()));
        emit valueChanged(component);
    }

private:
    ButtonComponent component;

    QLineEdit *idWidget;
    QSpinBox *layerWidget;
    UriWidget *spriteWidget;
    UriWidget *spriteHoverWidget;
    UriWidget *spritePressedWidget;
};

#endif //XEDITOR_BUTTONCOMPONENTWIDGET_HPP
