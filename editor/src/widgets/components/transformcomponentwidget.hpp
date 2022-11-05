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
#ifndef XEDITOR_TRANSFORMCOMPONENTWIDGET_HPP
#define XEDITOR_TRANSFORMCOMPONENTWIDGET_HPP

#include <QLineEdit>

#include "xng/xng.hpp"

#include "componentwidget.hpp"

#include "widgets/math/transformwidget.hpp"

using namespace xng;

class TransformComponentWidget : public ComponentWidget {
Q_OBJECT
public:
    TransformComponentWidget(QWidget *parent = nullptr) : ComponentWidget(parent) {
        parentLabel = new QLabel(this);
        parentWidget = new QLineEdit(this);
        transformLabel = new QLabel(this);
        transformWidget = new TransformWidget(this);

        headerText->setText("Transform");

        parentLabel->setText("Parent:");
        transformLabel->setText("Transform:");

        layout()->addWidget(parentLabel);
        layout()->addWidget(parentWidget);
        layout()->addWidget(transformLabel);
        layout()->addWidget(transformWidget);

        connect(parentWidget, SIGNAL(textEdited(const QString &)), this, SLOT(textEdited(const QString &)));
        connect(transformWidget, SIGNAL(valueChanged(const Transform &)), this, SLOT(valueChanged(const Transform &)));
    }

    void set(const TransformComponent &value) {
        transform = value;
        parentWidget->setText(transform.parent.c_str());
        transformWidget->set(transform.transform);
    }

    const TransformComponent &get() const {
        return transform;
    }

    virtual std::type_index getType() override {
        return typeid(TransformComponentWidget);
    }

signals:

    void valueChanged(const TransformComponent &value);

private slots:

    void textEdited(const QString &text) {
        transform.parent = text.toStdString();
        emit valueChanged(transform);
    }

    void valueChanged(const Transform &t) {
        transform.transform = t;
        emit valueChanged(transform);
    }

private:
    QLabel *parentLabel;
    QLineEdit *parentWidget;

    QLabel *transformLabel;
    TransformWidget *transformWidget;

    TransformComponent transform;
};

#endif //XEDITOR_TRANSFORMCOMPONENTWIDGET_HPP
