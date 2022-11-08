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
#ifndef XEDITOR_RECTANGLEWIDGET_HPP
#define XEDITOR_RECTANGLEWIDGET_HPP

#include <QLabel>

#include "vector2widget.hpp"

class RectangleWidget : public QWidget {
Q_OBJECT
public:
    explicit RectangleWidget(QWidget *parent = nullptr) : QWidget(parent) {
        positionLabel = new QLabel(this);
        positionWidget = new Vector2Widget(this);
        dimensionLabel = new QLabel(this);
        dimensionWidget = new Vector2Widget(this);

        positionLabel->setText("Position:");
        dimensionLabel->setText("Dimensions:");

        auto *line0 = new QWidget(this);
        line0->setLayout(new QHBoxLayout);
        line0->layout()->setMargin(0);
        line0->layout()->addWidget(positionLabel);
        line0->layout()->addWidget(positionWidget);
        auto *line1 = new QWidget(this);
        line1->setLayout(new QHBoxLayout);
        line1->layout()->setMargin(0);
        line1->layout()->addWidget(dimensionLabel);
        line1->layout()->addWidget(dimensionWidget);

        dimensionWidget->setRange(1);

        setLayout(new QVBoxLayout);
        layout()->addWidget(line0);
        layout()->addWidget(line1);
    }

    void set(const Rectf &value) {
        rectangle = value;
        positionWidget->set(value.position);
        dimensionWidget->set(value.dimensions);
    }

    const Rectf &get() const {
        return rectangle;
    }

signals:

    void valueChanged(const Rectf &value);

private slots:

    void valueChanged(const Vec2f &value) {
        auto *s = QObject::sender();
        if (s == positionWidget) {
            rectangle.position = value;
        } else {
            rectangle.dimensions = value;
        }
        emit valueChanged(rectangle);
    };
private:
    QLabel *positionLabel;
    Vector2Widget *positionWidget;
    QLabel *dimensionLabel;
    Vector2Widget *dimensionWidget;

    Rectf rectangle;
};

#endif //XEDITOR_RECTANGLEWIDGET_HPP
