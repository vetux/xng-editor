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
#ifndef XEDITOR_VECTOR4WIDGET_HPP
#define XEDITOR_VECTOR4WIDGET_HPP

#include <QWidget>
#include <QDoubleSpinBox>

#include <QHBoxLayout>
#include <QVBoxLayout>

#include "xng/xng.hpp"

using namespace xng;

class Vector4Widget : public QWidget {
Q_OBJECT
public:
    explicit Vector4Widget(QWidget *parent = nullptr) : QWidget(parent) {
        spinX = new QDoubleSpinBox(this);
        spinY = new QDoubleSpinBox(this);
        spinZ = new QDoubleSpinBox(this);
        spinW = new QDoubleSpinBox(this);

        connect(spinX, SIGNAL(valueChanged(double)), this, SLOT(valueChanged(double)));
        connect(spinY, SIGNAL(valueChanged(double)), this, SLOT(valueChanged(double)));
        connect(spinZ, SIGNAL(valueChanged(double)), this, SLOT(valueChanged(double)));
        connect(spinW, SIGNAL(valueChanged(double)), this, SLOT(valueChanged(double)));

        setLayout(new QHBoxLayout());

        layout()->addWidget(spinX);
        layout()->addWidget(spinY);
        layout()->addWidget(spinZ);
        layout()->addWidget(spinW);
        layout()->setMargin(0);

        spinX->setRange(-std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
        spinY->setRange(-std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
        spinZ->setRange(-std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
        spinW->setRange(-std::numeric_limits<double>::max(), std::numeric_limits<double>::max());

        spinX->setMinimumWidth(10);
        spinY->setMinimumWidth(10);
        spinZ->setMinimumWidth(10);
        spinW->setMinimumWidth(10);
    };

    void set(const Vec4f &vec) {
        value = vec;
        spinX->setValue(value.x);
        spinY->setValue(value.y);
        spinZ->setValue(value.z);
        spinW->setValue(value.w);
    }

    Vec4f get() {
        return value;
    }

    void setRange(double min, double max){
        spinX->setRange(min, max);
        spinY->setRange(min, max);
        spinZ->setRange(min, max);
        spinW->setRange(min, max);
    }

signals:

    void valueChanged(const Vec4f &value);

private slots:

    void valueChanged(double val) {
        auto *sender = QObject::sender();
        if (sender == spinX) {
            value.x = val;
        } else if (sender == spinY) {
            value.y = val;
        } else if (sender == spinZ) {
            value.z = val;
        } else {
            value.w = val;
        }
        emit valueChanged(value);
    };

private:
    QDoubleSpinBox *spinX;
    QDoubleSpinBox *spinY;
    QDoubleSpinBox *spinZ;
    QDoubleSpinBox *spinW;

    Vec4f value;
};

#endif //XEDITOR_VECTOR4WIDGET_HPP
