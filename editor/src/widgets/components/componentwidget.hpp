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
#ifndef XEDITOR_COMPONENTWIDGET_HPP
#define XEDITOR_COMPONENTWIDGET_HPP

#include <QFrame>
#include <QLabel>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>

#include "xengine.hpp"

using namespace xng;

class ComponentWidget : public QFrame {
Q_OBJECT
public:
    ComponentWidget(QWidget *parent = nullptr) : QFrame(parent) {
        setFrameShape(StyledPanel);
        setFrameShadow(Raised);

        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

        header = new QWidget(this);
        headerText = new QLabel(this);
        headerCheckBox = new QCheckBox(this);
        headerDestroyButton = new QPushButton(this);

        headerDestroyButton->setText("Destroy");

        auto *lay = new QHBoxLayout;
        header->setLayout(lay);
        lay->addWidget(headerText, 1);
        header->layout()->addWidget(headerDestroyButton);
        header->layout()->addWidget(headerCheckBox);

        setLayout(new QVBoxLayout);

        layout()->addWidget(header);

        connect(headerCheckBox, SIGNAL(stateChanged(int)), this, SLOT(stateChanged(int)));
        connect(headerDestroyButton, SIGNAL(pressed()), this, SIGNAL(destroyPressed()));
    }

    virtual void setTitle(const QString &title) {
        headerText->setText(title);
    }

    virtual void setChecked(bool enabled) {
        headerCheckBox->setCheckState(enabled ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    }

    virtual bool getChecked() {
        return headerCheckBox->checkState() == Qt::Checked;
    }

    virtual std::type_index getType() {
        return typeid(ComponentWidget);
    }

signals:

    void checkedChanged(bool enabled);

    void destroyPressed();

protected slots:

    void stateChanged(int state) {
        emit checkedChanged(state == Qt::Checked);
    };

protected:
    QWidget *header;
    QLabel *headerText;
    QCheckBox *headerCheckBox;
    QPushButton *headerDestroyButton;
};


#endif //XEDITOR_COMPONENTWIDGET_HPP
