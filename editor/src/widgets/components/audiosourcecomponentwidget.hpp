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
#ifndef XEDITOR_AUDIOSOURCECOMPONENTWIDGET_HPP
#define XEDITOR_AUDIOSOURCECOMPONENTWIDGET_HPP

#include "widgets/components/componentwidget.hpp"
#include "widgets/vector3widget.hpp"
#include "widgets/uriwidget.hpp"

#include "xng/xng.hpp"

class AudioSourceComponentWidget : public ComponentWidget {
Q_OBJECT
public:
    explicit AudioSourceComponentWidget(QWidget *parent = nullptr)
            : ComponentWidget(parent) {
        audioWidget = new UriWidget(this);
        playWidget = new QCheckBox(this);
        loopWidget = new QCheckBox(this);
        velocityWidget = new Vector3Widget(this);
        playWidget->setText("Play");
        loopWidget->setText("Loop");
        layout()->addWidget(new QLabel("Audio Resource:"));
        layout()->addWidget(audioWidget);
        layout()->addWidget(playWidget);
        layout()->addWidget(loopWidget);
        layout()->addWidget(new QLabel("Velocity:"));
        layout()->addWidget(velocityWidget);
        connect(audioWidget,
                SIGNAL(valueChanged(const QString &)),
                this,
                SLOT(onUriChanged(const QString &)));
        connect(playWidget,
                SIGNAL(stateChanged(int)),
                this,
                SLOT(onPlayStateChanged(int)));
        connect(loopWidget,
                SIGNAL(stateChanged(int)),
                this,
                SLOT(onLoopStateChanged(int)));
        connect(velocityWidget,
                SIGNAL(valueChanged(const Vec3f &)),
                this,
                SLOT(onVelocityChanged(const Vec3f &)));
        headerText->setText("Audio Source");
    }

    void set(const AudioSourceComponent &value) {
        component = value;
        audioWidget->setValue(component.audio.getUri());
        playWidget->setChecked(component.play);
        loopWidget->setChecked(component.loop);
        velocityWidget->set(component.velocity);
        headerCheckBox->setChecked(component.enabled);
    }

    const AudioSourceComponent &get() const {
        return component;
    }

    virtual std::type_index getType() override {
        return component.getType();
    }

    std::type_index getComponentType() override {
        return typeid(AudioSourceComponent);
    }

signals:

    void valueChanged(const AudioSourceComponent &value);

protected:
    void checkBoxStateChange(int state) override {
        component.enabled = state == Qt::Checked;
        emit valueChanged(component);
    }

private slots:

    void onUriChanged(const QString &uri) {
        component.audio = ResourceHandle<AudioData>(Uri(uri.toStdString().c_str()));
        emit valueChanged(component);
    }

    void onPlayStateChanged(int state) {
        component.play = state == Qt::Checked;
        emit valueChanged(component);
    }

    void onLoopStateChanged(int state) {
        component.loop = state == Qt::Checked;
        emit valueChanged(component);
    }

    void onVelocityChanged(const Vec3f &vel){
        component.velocity = vel;
        emit valueChanged(component);
    }

private:
    AudioSourceComponent component;

    UriWidget *audioWidget;
    QCheckBox *playWidget;
    QCheckBox *loopWidget;
    Vector3Widget *velocityWidget;
};

#endif //XEDITOR_AUDIOSOURCECOMPONENTWIDGET_HPP
