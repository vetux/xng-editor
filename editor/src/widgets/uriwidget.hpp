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
#ifndef XEDITOR_URIWIDGET_HPP
#define XEDITOR_URIWIDGET_HPP

#include <QLineEdit>
#include <QHBoxLayout>
#include <QLabel>

#include "xng/xng.hpp"

using namespace xng;

class UriWidget : public QWidget {
Q_OBJECT
public:
    explicit UriWidget(QWidget *parent = nullptr)
            : QWidget(parent) {
        lineEdit = new QLineEdit(this);
        setLayout(new QHBoxLayout());
        layout()->setMargin(0);
        layout()->addWidget(new QLabel("Uri"));
        layout()->addWidget(lineEdit);

        connect(lineEdit, SIGNAL(textEdited(const QString &)), this, SIGNAL(valueChanged(const QString &)));
    }

    void setValue(const Uri &uri) {
        lineEdit->setText(uri.toString().c_str());
    }

signals:

    void valueChanged(const QString &uri);

private:
    QLineEdit *lineEdit;
};

#endif //XEDITOR_URIWIDGET_HPP
