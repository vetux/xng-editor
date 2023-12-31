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
#ifndef XEDITOR_TRANSFORMWIDGET_HPP
#define XEDITOR_TRANSFORMWIDGET_HPP

#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "xng/xng.hpp"

#include "vector3widget.hpp"

using namespace xng;

class TransformWidget : public QWidget {
Q_OBJECT
public:
    TransformWidget(QWidget *parent = nullptr) : QWidget(parent) {
        positionLabel = new QLabel(this);
        positionWidget = new Vector3Widget(this);
        rotationLabel = new QLabel(this);
        rotationWidget = new Vector3Widget(this);
        scaleLabel = new QLabel(this);
        scaleWidget = new Vector3Widget(this);

        positionLabel->setText("Position:");
        rotationLabel->setText("Rotation:");
        scaleLabel->setText("Scale:");

        setLayout(new QVBoxLayout);

        layout()->addWidget(positionLabel);
        layout()->addWidget(positionWidget);

        layout()->addWidget(rotationLabel);
        layout()->addWidget(rotationWidget);

        layout()->addWidget(scaleLabel);
        layout()->addWidget(scaleWidget);

        connect(positionWidget, SIGNAL(valueChanged(const Vec3f &)), this, SLOT(valueChanged(const Vec3f &)));
        connect(rotationWidget, SIGNAL(valueChanged(const Vec3f &)), this, SLOT(valueChanged(const Vec3f &)));
        connect(scaleWidget, SIGNAL(valueChanged(const Vec3f &)), this, SLOT(valueChanged(const Vec3f &)));
    }

    void set(const Transform &value) {
        transform = value;
        positionWidget->set(transform.getPosition());
        rotationWidget->set(transform.getRotation().getEulerAngles());
        scaleWidget->set(transform.getScale());
    }

    const Transform &get() const {
        return transform;
    }

signals:

    void valueChanged(const Transform &value);

private slots:

    void valueChanged(const Vec3f &value) {
        auto *sen = sender();
        if (sen == positionWidget) {
            transform.setPosition(value);
        } else if (sen == rotationWidget) {
            transform.setRotation(Quaternion(value));
        } else {
            transform.setScale(value);
        }
        emit valueChanged(transform);
    };

private:
    QLabel *positionLabel;
    Vector3Widget *positionWidget;
    QLabel *rotationLabel;
    Vector3Widget *rotationWidget;
    QLabel *scaleLabel;
    Vector3Widget *scaleWidget;

    Transform transform;
};

#endif //XEDITOR_TRANSFORMWIDGET_HPP
