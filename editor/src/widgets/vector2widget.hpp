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
#ifndef XEDITOR_VECTOR2WIDGET_HPP
#define XEDITOR_VECTOR2WIDGET_HPP

#include <QWidget>
#include <QDoubleSpinBox>

#include <QHBoxLayout>
#include <QVBoxLayout>

#include "xng/xng.hpp"

using namespace xng;

class Vector2Widget : public QWidget {
Q_OBJECT
public:
    explicit Vector2Widget(QWidget *parent = nullptr) : QWidget(parent) {
        spinX = new QDoubleSpinBox(this);
        spinY = new QDoubleSpinBox(this);

        connect(spinX, SIGNAL(valueChanged(double)), this, SLOT(valueChanged(double)));
        connect(spinY, SIGNAL(valueChanged(double)), this, SLOT(valueChanged(double)));

        setLayout(new QHBoxLayout());

        layout()->addWidget(spinX);
        layout()->addWidget(spinY);
        layout()->setMargin(0);

        spinX->setRange(std::numeric_limits<double>::min(), std::numeric_limits<double>::max());
        spinY->setRange(std::numeric_limits<double>::min(), std::numeric_limits<double>::max());

        spinX->setMinimumWidth(10);
        spinY->setMinimumWidth(10);
    };

    void set(const Vec2f &vec) {
        value = vec;
        spinX->setValue(value.x);
        spinY->setValue(value.y);
    }

    Vec2f get() {
        return value;
    }

    void setRange(double min = std::numeric_limits<double>::min(),
                  double max = std::numeric_limits<double>::max()) {
        spinX->setRange(min, max);
        spinY->setRange(min, max);
    }

signals:

    void valueChanged(const Vec2f &value);

private slots:

    void valueChanged(double val) {
        auto *sender = QObject::sender();
        if (sender == spinX) {
            value.x = val;
        } else {
            value.y = val;
        }
        emit valueChanged(value);
    };

private:
    QDoubleSpinBox *spinX;
    QDoubleSpinBox *spinY;

    Vec2f value;
};

#endif //XEDITOR_VECTOR2WIDGET_HPP
