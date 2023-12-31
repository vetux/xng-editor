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
#ifndef XEDITOR_RECTTRANSFORMCOMPONENTWIDGET_HPP
#define XEDITOR_RECTTRANSFORMCOMPONENTWIDGET_HPP

#include <QLineEdit>
#include <QComboBox>

#include "xng/xng.hpp"

#include "componentwidget.hpp"

#include "widgets/rectanglewidget.hpp"

using namespace xng;

class AnchorWidget : public QWidget {
Q_OBJECT
public:
    explicit AnchorWidget(QWidget *parent = nullptr) : QWidget(parent) {
        setLayout(new QHBoxLayout);
        comboBox = new QComboBox(this);
        comboBox->addItems({"RECT_ALIGN_LEFT_TOP",
                            "RECT_ALIGN_CENTER_TOP",
                            "RECT_ALIGN_RIGHT_TOP",
                            "RECT_ALIGN_LEFT_CENTER",
                            "RECT_ALIGN_CENTER_CENTER",
                            "RECT_ALIGN_RIGHT_CENTER",
                            "RECT_ALIGN_LEFT_BOTTOM",
                            "RECT_ALIGN_CENTER_BOTTOM",
                            "RECT_ALIGN_RIGHT_BOTTOM"});
        layout()->addWidget(comboBox);
        connect(comboBox, SIGNAL(activated(int)), this, SLOT(activated(int)));
    }

    void setAnchor(RectTransform::Alignment anchor) {
        comboBox->setCurrentIndex((int) anchor);
    }

    RectTransform::Alignment getAnchor() {
        return (RectTransform::Alignment) comboBox->currentIndex();
    }

signals:

    void valueChanged(RectTransform::Alignment anchor);

private slots:

    void activated(int index) {
        emit valueChanged((RectTransform::Alignment) (RectTransform::Alignment::RECT_ALIGN_LEFT_TOP + index));
    }

private:
    QComboBox *comboBox;
};

class RectTransformComponentWidget : public ComponentWidget {
Q_OBJECT
public:
    RectTransformComponentWidget(QWidget *parent = nullptr) : ComponentWidget(parent) {
        headerText->setText("Canvas Transform");

        anchorLabel = new QLabel(this);
        anchorWidget = new AnchorWidget(this);
        canvasLabel = new QLabel(this);
        canvasWidget = new QLineEdit(this);
        rectangleLabel = new QLabel(this);
        rectangleWidget = new RectangleWidget(this);
        centerLabel = new QLabel(this);
        centerWidget = new Vector2Widget(this);
        rotationLabel = new QLabel(this);
        rotationWidget = new QDoubleSpinBox(this);

        anchorLabel->setText("Anchor:");
        canvasLabel->setText("Canvas:");
        rectangleLabel->setText("Rectangle:");
        centerLabel->setText("Center:");
        rotationLabel->setText("Rotation:");

        rotationWidget->setRange(std::numeric_limits<double>::min(), std::numeric_limits<double>::max());
        rotationWidget->setMinimumWidth(10);

        layout()->addWidget(anchorLabel);
        layout()->addWidget(anchorWidget);

        layout()->addWidget(canvasLabel);
        layout()->addWidget(canvasWidget);
        layout()->addWidget(rectangleLabel);
        layout()->addWidget(rectangleWidget);
        layout()->addWidget(centerLabel);
        layout()->addWidget(centerWidget);
        layout()->addWidget(rotationLabel);
        layout()->addWidget(rotationWidget);

        connect(anchorWidget,
                SIGNAL(valueChanged(CanvasTransformComponent::Anchor)),
                this,
                SLOT(valueChanged(CanvasTransformComponent::Anchor)));
        connect(canvasWidget, SIGNAL(textEdited(const QString &)), this, SLOT(textEdited(const QString &)));
        connect(rectangleWidget, SIGNAL(valueChanged(const Rectf &)), this, SLOT(valueChanged(const Rectf &)));
        connect(centerWidget, SIGNAL(valueChanged(const Vec2f &)), this, SLOT(valueChanged(const Vec2f &)));
        connect(rotationWidget, SIGNAL(valueChanged(double)), this, SLOT(valueChanged(double)));
    }

    void set(const RectTransformComponent &comp) {
        transform = comp;
        anchorWidget->setAnchor(transform.rectTransform.alignment);
        //canvasWidget->setText(transform.parent.c_str());
        rectangleWidget->set(Rectf(transform.rectTransform.position, transform.rectTransform.size));
        centerWidget->set(transform.rectTransform.center);
        rotationWidget->setValue(transform.rectTransform.rotation);
        headerCheckBox->setChecked(transform.enabled);
    }

    const RectTransformComponent &get() const {
        return transform;
    }

    virtual std::type_index getType() override {
        return typeid(RectTransformComponentWidget);
    }

    std::type_index getComponentType() override {
        return typeid(RectTransformComponent);
    }

protected:
    void checkBoxStateChange(int state) override {
        transform.enabled = state == Qt::Checked;
        emit valueChanged(transform);
    }

signals:

    void valueChanged(const RectTransformComponent &value);

private slots:

    void valueChanged(RectTransform::Alignment anchor) {
        transform.rectTransform.alignment = anchor;
        emit valueChanged(transform);
    }

    void textEdited(const QString &text) {
       // transform.rectTransform.parent = text.toStdString();
        emit valueChanged(transform);
    }

    void valueChanged(const Rectf &t) {
        transform.rectTransform.size = t.dimensions;
        transform.rectTransform.position = t.position;
        emit valueChanged(transform);
    }

    void valueChanged(const Vec2f &v) {
        transform.rectTransform.center = v;
        emit valueChanged(transform);
    }

    void valueChanged(double v) {
        transform.rectTransform.rotation = v;
        emit valueChanged(transform);
    }

private:
    QLabel *anchorLabel;
    AnchorWidget *anchorWidget;

    QLabel *canvasLabel;
    QLineEdit *canvasWidget;

    QLabel *rectangleLabel;
    RectangleWidget *rectangleWidget;

    QLabel *centerLabel;
    Vector2Widget *centerWidget;

    QLabel *rotationLabel;
    QDoubleSpinBox *rotationWidget;

    RectTransformComponent transform;
};

#endif //XEDITOR_RECTTRANSFORMCOMPONENTWIDGET_HPP
