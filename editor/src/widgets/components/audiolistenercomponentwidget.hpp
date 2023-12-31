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
#ifndef XEDITOR_AUDIOLISTENERCOMPONENTWIDGET_HPP
#define XEDITOR_AUDIOLISTENERCOMPONENTWIDGET_HPP

#include "widgets/components/componentwidget.hpp"
#include "widgets/vector3widget.hpp"

#include "xng/xng.hpp"

class AudioListenerComponentWidget : public ComponentWidget {
Q_OBJECT
public:
    explicit AudioListenerComponentWidget(QWidget *parent = nullptr)
            : ComponentWidget(parent) {
        velocityWidget = new Vector3Widget(this);
        layout()->addWidget(new QLabel("Velocity:"));
        layout()->addWidget(velocityWidget);
        connect(velocityWidget,
                SIGNAL(valueChanged(const Vec3f &)),
                this,
                SLOT(onVelocityChanged(const Vec3f &)));
        headerText->setText("Audio Listener");
    }

    void set(const AudioListenerComponent &value) {
        component = value;
        velocityWidget->set(component.velocity);
        headerCheckBox->setChecked(component.enabled);
    }

    const AudioListenerComponent &get() const {
        return component;
    }

    virtual std::type_index getType() override {
        return component.getType();
    }

    std::type_index getComponentType() override {
        return typeid(AudioListenerComponent);
    }

signals:

    void valueChanged(const AudioListenerComponent &value);

protected:
    void checkBoxStateChange(int state) override {
        component.enabled = state == Qt::Checked;
        emit valueChanged(component);
    }

private slots:
    void onVelocityChanged(const Vec3f &v){
        component.velocity = v;
        emit valueChanged(component);
    }

private:

    AudioListenerComponent component;
    Vector3Widget *velocityWidget;
};

#endif //XEDITOR_AUDIOLISTENERCOMPONENTWIDGET_HPP
