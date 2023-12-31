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
#ifndef XEDITOR_COLORWIDGET_HPP
#define XEDITOR_COLORWIDGET_HPP

#include <QWidget>
#include <QColorDialog>
#include <QPushButton>

#include "vector4widget.hpp"

#include "xng/xng.hpp"

class ColorWidget : public QWidget {
Q_OBJECT
public:
    explicit ColorWidget(QWidget *parent = nullptr)
            : QWidget(parent) {
        colorWidget = new Vector4Widget(this);
        button = new QPushButton(this);
        button->setText("Choose");
        auto *layout = new QHBoxLayout;
        layout->addWidget(colorWidget, 1);
        layout->addWidget(button);
        setLayout(layout);
        connect(colorWidget,
                SIGNAL(valueChanged(const Vec4f &)),
                this,
                SLOT(colorChanged(const Vec4f &)));
        connect(button,
                SIGNAL(clicked()),
                this,
                SLOT(buttonPressed()));
    }

    void setColor(const ColorRGBA &value) {
        color = value;
        colorWidget->set(value.divide(1));
    }

signals:

    void valueChanged(const ColorRGBA &value);

private slots:

    void buttonPressed() {
        QColorDialog dialog;
        dialog.setCurrentColor(QColor(color.r(),
                                      color.g(),
                                      color.b(),
                                      color.a()));
        if (dialog.exec() == QColorDialog::Accepted) {
            auto c = dialog.currentColor();
            color.r() = c.red();
            color.g() = c.green();
            color.b() = c.blue();
            color.a() = c.alpha();
            emit valueChanged(color);
        }
    }

    void colorChanged(const Vec4f &value) {
        color.r() = (int)value.x;
        color.g() = (int)value.y;
        color.b() = (int)value.z;
        color.a() = (int)value.w;
        emit valueChanged(color);
    }

private:
    ColorRGBA color;
    Vector4Widget *colorWidget;
    QPushButton *button;
};

#endif //XEDITOR_COLORWIDGET_HPP
