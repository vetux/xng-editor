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
#ifndef XEDITOR_SPRITECOMPONENTWIDGET_HPP
#define XEDITOR_SPRITECOMPONENTWIDGET_HPP

#include "widgets/components/componentwidget.hpp"
#include "widgets/vector3widget.hpp"

#include "xng/xng.hpp"

#include "widgets/uriwidget.hpp"
#include "widgets/colorwidget.hpp"

class SpriteComponentWidget : public ComponentWidget {
Q_OBJECT
public:
    explicit SpriteComponentWidget(QWidget *parent = nullptr)
            : ComponentWidget(parent) {
        spriteWidget = new UriWidget;
        mixSpriteWidget = new UriWidget;
        layerWidget = new QSpinBox;
        flipSpriteXWidget = new QCheckBox;
        flipSpriteYWidget = new QCheckBox;
        mixWidget = new QDoubleSpinBox;
        mixColorWidget = new ColorWidget;

        mixWidget->setRange(0, 1);

        flipSpriteXWidget->setText("x");
        flipSpriteYWidget->setText("y");

        auto *w = new QWidget;
        auto *l = new QHBoxLayout;
        l->addWidget(flipSpriteXWidget);
        l->addWidget(flipSpriteYWidget);
        w->setLayout(l);

        layout()->addWidget(new QLabel("Sprite"));
        layout()->addWidget(spriteWidget);
        layout()->addWidget(new QLabel("Layer"));
        layout()->addWidget(layerWidget);
        layout()->addWidget(new QLabel("Flip"));
        layout()->addWidget(w);
        layout()->addWidget(new QLabel("Mix"));
        layout()->addWidget(mixWidget);
        layout()->addWidget(new QLabel("Mix Sprite"));
        layout()->addWidget(mixSpriteWidget);
        layout()->addWidget(new QLabel("Mix Color"));
        layout()->addWidget(mixColorWidget);

        connect(spriteWidget,
                SIGNAL(valueChanged(const QString &)),
                this,
                SLOT(spriteChanged(const QString &)));
        connect(mixSpriteWidget,
                SIGNAL(valueChanged(const QString &)),
                this,
                SLOT(spriteBChanged(const QString &)));
        connect(layerWidget,
                SIGNAL(valueChanged(int)),
                this,
                SLOT(layerChanged(int)));
        connect(flipSpriteXWidget,
                SIGNAL(stateChanged(int)),
                this,
                SLOT(flipSpriteXChanged(int)));
        connect(flipSpriteYWidget,
                SIGNAL(stateChanged(int)),
                this,
                SLOT(flipSpriteYChanged(int)));
        connect(mixWidget,
                SIGNAL(valueChanged(double)),
                this,
                SLOT(mixChanged(double)));
        connect(mixColorWidget,
                SIGNAL(valueChanged(const ColorRGBA &)),
                this,
                SLOT(mixColorChanged(const ColorRGBA &)));

        headerText->setText("Sprite");
    }

    void set(const SpriteComponent &value) {
        component = value;
        spriteWidget->setValue(value.sprite.getUri());
     /*   mixSpriteWidget->setValue(value.mixSprite.getUri());
        layerWidget->setValue(value.layer);
        flipSpriteXWidget->setChecked(value.flipSprite.x);
        flipSpriteYWidget->setChecked(value.flipSprite.y);*/
        mixWidget->setValue(value.mix);
        mixColorWidget->setColor(ColorRGBA(value.mixColor.r(), value.mixColor.g(), value.mixColor.b(), 0));
        headerCheckBox->setChecked(component.enabled);
    }

    const SpriteComponent &get() const {
        return component;
    }

    virtual std::type_index getType() override {
        return component.getType();
    }

    std::type_index getComponentType() override {
        return typeid(SpriteComponent);
    }

signals:

    void valueChanged(const SpriteComponent &value);

protected:
    void checkBoxStateChange(int state) override {
        component.enabled = state == Qt::Checked;
        emit valueChanged(component);
    }

private slots:

    void spriteChanged(const QString &uri) {
        component.sprite = ResourceHandle<Sprite>(Uri(uri.toStdString().c_str()));
        emit valueChanged(component);
    }

    void spriteBChanged(const QString &uri) {
     //   component.mixSprite = ResourceHandle<Sprite>(Uri(uri.toStdString().c_str()));
        emit valueChanged(component);
    }

    void layerChanged(int layer) {
     //   component.layer = layer;
        emit valueChanged(component);
    }

    void flipSpriteXChanged(int state) {
    //    component.flipSprite.x = state == Qt::Checked;
        emit valueChanged(component);
    }

    void flipSpriteYChanged(int state) {
   //     component.flipSprite.y = state == Qt::Checked;
        emit valueChanged(component);
    }

    void mixChanged(double value) {
        component.mix = value;
        emit valueChanged(component);
    }

    void mixColorChanged(const ColorRGBA &value){
   //     component.mixColor = ColorRGB(value.r(), value.g(), value.b());
        emit valueChanged(component);
    }

private:
    SpriteComponent component;
    UriWidget *spriteWidget;
    QSpinBox *layerWidget;
    QCheckBox *flipSpriteXWidget;
    QCheckBox *flipSpriteYWidget;
    QDoubleSpinBox *mixWidget;
    ColorWidget *mixColorWidget;
    UriWidget *mixSpriteWidget;

};

#endif //XEDITOR_SPRITECOMPONENTWIDGET_HPP
