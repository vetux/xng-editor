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
#ifndef XEDITOR_CAMERACOMPONENTWIDGET_HPP
#define XEDITOR_CAMERACOMPONENTWIDGET_HPP

#include "widgets/components/componentwidget.hpp"
#include "widgets/vector3widget.hpp"
#include "widgets/camerawidget.hpp"

#include "xng/xng.hpp"

class CameraComponentWidget : public ComponentWidget {
Q_OBJECT
public:
    explicit CameraComponentWidget(QWidget *parent = nullptr)
            : ComponentWidget(parent) {
        cameraWidget = new CameraWidget(this);
        layout()->addWidget(cameraWidget);
        connect(cameraWidget,
                SIGNAL(valueChanged(const Camera &)),
                this,
                SLOT(cameraChanged(const Camera &)));
        headerText->setText("Camera");
    }

    void set(const CameraComponent &value) {
        component = value;
        cameraWidget->setValue(component.camera);
        headerCheckBox->setChecked(component.enabled);
    }

    const CameraComponent &get() const {
        return component;
    }

    virtual std::type_index getType() override {
        return component.getType();
    }

    std::type_index getComponentType() override {
        return typeid(CameraComponent);
    }

signals:

    void valueChanged(const CameraComponent &value);

protected:
    void checkBoxStateChange(int state) override {
        component.enabled = state == Qt::Checked;
        emit valueChanged(component);
    }

private slots:

    void cameraChanged(const Camera &camera) {
        component.camera = camera;
        emit valueChanged(component);
    }

private:
    CameraComponent component;
    CameraWidget *cameraWidget;
};

#endif //XEDITOR_CAMERACOMPONENTWIDGET_HPP
