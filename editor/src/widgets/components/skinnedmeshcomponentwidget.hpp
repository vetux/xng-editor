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
#ifndef XEDITOR_SKINNEDMESHCOMPONENTWIDGET_HPP
#define XEDITOR_SKINNEDMESHCOMPONENTWIDGET_HPP

#include "widgets/components/componentwidget.hpp"
#include "widgets/vector3widget.hpp"

#include "xng/xng.hpp"

class SkinnedMeshComponentWidget : public ComponentWidget {
Q_OBJECT
public:
    explicit SkinnedMeshComponentWidget(QWidget *parent = nullptr)
            : ComponentWidget(parent) {
        headerText->setText("Mesh");
    }

    void set(const SkinnedMeshComponent &value) {
        component = value;
        headerCheckBox->setChecked(component.enabled);
    }

    const SkinnedMeshComponent &get() const {
        return component;
    }

    virtual std::type_index getType() override {
        return component.getType();
    }

    std::type_index getComponentType() override {
        return typeid(SkinnedMeshComponent);
    }

signals:

    void valueChanged(const SkinnedMeshComponent &value);

protected:
    void checkBoxStateChange(int state) override {
        component.enabled = state == Qt::Checked;
        emit valueChanged(component);
    }

private:
    SkinnedMeshComponent component;
};

#endif //XEDITOR_SKINNEDMESHCOMPONENTWIDGET_HPP
