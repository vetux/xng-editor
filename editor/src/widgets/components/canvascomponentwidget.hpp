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
#ifndef XEDITOR_CANVASCOMPONENTWIDGET_HPP
#define XEDITOR_CANVASCOMPONENTWIDGET_HPP

#include "widgets/components/componentwidget.hpp"
#include "widgets/vector2widget.hpp"
#include "widgets/colorwidget.hpp"

#include "xng/xng.hpp"

class CanvasComponentWidget : public ComponentWidget {
Q_OBJECT
public:
    explicit CanvasComponentWidget(QWidget *parent = nullptr)
            : ComponentWidget(parent) {
        cameraPositionWidget = new Vector2Widget;

        viewportSizeWidget = new Vector2Widget;
        viewportOffsetWidget = new Vector2Widget;

        projectionSizeWidget = new Vector2Widget;

        lockViewportWidget = new QCheckBox;
        stretchViewportWidget = new QCheckBox;
        lockAspectRatioWidget = new QCheckBox;

        clearWidget = new QCheckBox;
        clearColorWidget = new ColorWidget;
        layerWidget = new QSpinBox;

        lockViewportWidget->setText("Lock Viewport");
        stretchViewportWidget->setText("Stretch Viewport");
        lockAspectRatioWidget->setText("Lock Aspect Ratio");

        clearWidget->setText("Clear");

        viewportSizeWidget->setRange(1);
        projectionSizeWidget->setRange(1);

        layerWidget->setRange(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());

        layout()->addWidget(new QLabel("Camera Position:"));
        layout()->addWidget(cameraPositionWidget);

        layout()->addWidget(new QLabel("Viewport Size:"));
        layout()->addWidget(viewportSizeWidget);
        layout()->addWidget(new QLabel("Viewport Offset:"));
        layout()->addWidget(viewportOffsetWidget);

        layout()->addWidget(new QLabel("Projection Size:"));
        layout()->addWidget(projectionSizeWidget);

        layout()->addWidget(lockViewportWidget);
        layout()->addWidget(stretchViewportWidget);
        layout()->addWidget(lockAspectRatioWidget);

        layout()->addWidget(clearWidget);
        layout()->addWidget(new QLabel("Clear Color:"));
        layout()->addWidget(clearColorWidget);
        layout()->addWidget(new QLabel("Layer:"));
        layout()->addWidget(layerWidget);

        connect(cameraPositionWidget,
                SIGNAL(valueChanged(const Vec2f &)),
                this,
                SLOT(cameraPositionChanged(const Vec2f &)));

        connect(viewportSizeWidget,
                SIGNAL(valueChanged(const Vec2f &)),
                this,
                SLOT(viewportSizeChanged(const Vec2f &)));
        connect(viewportOffsetWidget,
                SIGNAL(valueChanged(const Vec2f &)),
                this,
                SLOT(viewportOffsetChanged(const Vec2f &)));

        connect(projectionSizeWidget,
                SIGNAL(valueChanged(const Vec2f &)),
                this,
                SLOT(projectionSizeChanged(const Vec2f &)));

        connect(lockViewportWidget,
                SIGNAL(stateChanged(int)),
                this,
                SLOT(lockViewportChanged(int)));
        connect(stretchViewportWidget,
                SIGNAL(stateChanged(int)),
                this,
                SLOT(stretchViewportChanged(int)));
        connect(lockAspectRatioWidget,
                SIGNAL(stateChanged(int)),
                this,
                SLOT(lockAspectRatioChanged(int)));

        connect(clearWidget,
                SIGNAL(stateChanged(int)),
                this,
                SLOT(clearChanged(int)));
        connect(clearColorWidget,
                SIGNAL(valueChanged(const ColorRGBA &)),
                this,
                SLOT(clearColorChanged(const ColorRGBA &)));
        connect(layerWidget,
                SIGNAL(valueChanged(int)),
                this,
                SLOT(layerChanged(int)));

        headerText->setText("Canvas");
    }

    void set(const CanvasComponent &value) {
        component = value;
        cameraPositionWidget->set(value.cameraPosition);
        viewportSizeWidget->set(value.viewportSize.convert<float>());
        viewportOffsetWidget->set(value.viewportOffset.convert<float>());
        projectionSizeWidget->set(value.projectionSize.convert<float>());
        lockViewportWidget->setChecked(value.lockViewport);
        stretchViewportWidget->setChecked(value.stretchViewport);
        lockAspectRatioWidget->setChecked(value.lockAspectRatio);
        clearWidget->setChecked(value.clear);
        clearColorWidget->setColor(value.clearColor);
        layerWidget->setValue(value.layer);
        headerCheckBox->setChecked(component.enabled);
    }

    const CanvasComponent &get() const {
        return component;
    }

    virtual std::type_index getType() override {
        return component.getType();
    }

    std::type_index getComponentType() override {
        return typeid(CanvasComponent);
    }

signals:

    void valueChanged(const CanvasComponent &value);

protected:
    void checkBoxStateChange(int state) override {
        component.enabled = state == Qt::Checked;
        emit valueChanged(component);
    }

private slots:

    void cameraPositionChanged(const Vec2f &value) {
        component.cameraPosition = value;
        emit valueChanged(component);
    }

    void viewportSizeChanged(const Vec2f &value) {
        component.viewportSize = Vec2i(value.x, value.y);
        emit valueChanged(component);
    }

    void viewportOffsetChanged(const Vec2f &value) {
        component.viewportOffset = Vec2i(value.x, value.y);
        emit valueChanged(component);
    }

    void projectionSizeChanged(const Vec2f &value) {
        component.projectionSize = value;
        emit valueChanged(component);
    }

    void lockViewportChanged(int state) {
        component.lockViewport = state == Qt::Checked;
        emit valueChanged(component);
    }

    void stretchViewportChanged(int state) {
        component.stretchViewport = state == Qt::Checked;
        emit valueChanged(component);
    }

    void lockAspectRatioChanged(int state) {
        component.lockAspectRatio = state == Qt::Checked;
        emit valueChanged(component);
    }

    void clearChanged(int state) {
        component.clear = state == Qt::Checked;
        emit valueChanged(component);
    }

    void clearColorChanged(const ColorRGBA &color) {
        component.clearColor = color;
        emit valueChanged(component);
    }

    void layerChanged(int layer) {
        component.layer = layer;
        emit valueChanged(component);
    }

private:
    CanvasComponent component;

    Vector2Widget *cameraPositionWidget;
    Vector2Widget *viewportSizeWidget;
    Vector2Widget *viewportOffsetWidget;
    Vector2Widget *projectionSizeWidget;

    QCheckBox *lockViewportWidget;
    QCheckBox *stretchViewportWidget;
    QCheckBox *lockAspectRatioWidget;

    QCheckBox *clearWidget;
    ColorWidget *clearColorWidget;
    QSpinBox *layerWidget;
};

#endif //XEDITOR_CANVASCOMPONENTWIDGET_HPP
