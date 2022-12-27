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
#ifndef XEDITOR_TEXTCOMPONENTWIDGET_HPP
#define XEDITOR_TEXTCOMPONENTWIDGET_HPP

#include <QComboBox>
#include <QStringListModel>

#include "widgets/components/componentwidget.hpp"
#include "widgets/vector3widget.hpp"

#include "xng/xng.hpp"

#include "widgets/vector2widget.hpp"
#include "widgets/colorwidget.hpp"
#include "widgets/uriwidget.hpp"

using namespace xng;

class TextComponentWidget : public ComponentWidget {
Q_OBJECT
public:
    explicit TextComponentWidget(QWidget *parent = nullptr)
            : ComponentWidget(parent) {
        textWidget = new QLineEdit;
        pixelSizeWidget = new Vector2Widget;
        lineHeightWidget = new QSpinBox;
        lineWidthWidget = new QSpinBox;
        lineSpacingWidget = new QSpinBox;
        alignmentWidget = new QComboBox;
        fontWidget = new UriWidget;
        textAnchorWidget = new QComboBox;
        textScrollWidget = new Vector2Widget;
        textColorWidget = new ColorWidget;
        layerWidget = new QSpinBox;

        alignmentMapping[0] = xng::ALIGN_LEFT;
        alignmentMapping[1] = xng::ALIGN_CENTER;
        alignmentMapping[2] = xng::ALIGN_RIGHT;

        for (auto &pair: alignmentMapping)
            alignmentMappingReverse[pair.second] = pair.first;

        for (auto i = (int)CanvasTransformComponent::TOP_LEFT; i <= CanvasTransformComponent::BOTTOM_RIGHT; i++) {
            anchorMapping[i] = (CanvasTransformComponent::Anchor)i;
        }

        for (auto &pair: anchorMapping)
            anchorMappingReverse[pair.second] = pair.first;

        alignmentWidget->setModel(new QStringListModel({
                                                               "ALIGN_LEFT",
                                                               "ALIGN_CENTER",
                                                               "ALIGN_RIGHT"
                                                       }));
        textAnchorWidget->setModel(new QStringListModel({
                                                                "TOP_LEFT",
                                                                "TOP_CENTER",
                                                                "TOP_RIGHT",
                                                                "LEFT",
                                                                "CENTER",
                                                                "RIGHT",
                                                                "BOTTOM_LEFT",
                                                                "BOTTOM_CENTER",
                                                                "BOTTOM_RIGHT"
                                                        }));

        layout()->addWidget(new QLabel("Text"));
        layout()->addWidget(textWidget);
        layout()->addWidget(new QLabel("Font Pixel Size"));
        layout()->addWidget(pixelSizeWidget);
        layout()->addWidget(new QLabel("Line height"));
        layout()->addWidget(lineHeightWidget);
        layout()->addWidget(new QLabel("Line width"));
        layout()->addWidget(lineWidthWidget);
        layout()->addWidget(new QLabel("Line spacing"));
        layout()->addWidget(lineSpacingWidget);
        layout()->addWidget(new QLabel("Alignment"));
        layout()->addWidget(alignmentWidget);
        layout()->addWidget(new QLabel("Font"));
        layout()->addWidget(fontWidget);
        layout()->addWidget(new QLabel("Text Anchor"));
        layout()->addWidget(textAnchorWidget);
        layout()->addWidget(new QLabel("Text Color"));
        layout()->addWidget(textColorWidget);
        layout()->addWidget(new QLabel("Layer"));
        layout()->addWidget(layerWidget);

        connect(textWidget,
                SIGNAL(textEdited(const QString &)),
                this,
                SLOT(textChanged(const QString &)));
        connect(pixelSizeWidget,
                SIGNAL(valueChanged(const Vec2f &)),
                this,
                SLOT(pixelSizeChanged(const Vec2f &)));
        connect(lineHeightWidget,
                SIGNAL(valueChanged(int)),
                this,
                SLOT(lineHeightChanged(int)));
        connect(lineWidthWidget,
                SIGNAL(valueChanged(int)),
                this,
                SLOT(lineWidthChanged(int)));
        connect(lineSpacingWidget,
                SIGNAL(valueChanged(int)),
                this,
                SLOT(lineSpacingChanged(int)));
        connect(alignmentWidget,
                SIGNAL(currentIndexChanged(int)),
                this,
                SLOT(alignmentChanged(int)));
        connect(fontWidget,
                SIGNAL(valueChanged(const QString &)),
                this,
                SLOT(fontChanged(const QString &)));
        connect(textAnchorWidget,
                SIGNAL(currentIndexChanged(int)),
                this,
                SLOT(textAnchorChanged(int)));
        connect(textScrollWidget,
                SIGNAL(valueChanged(const Vec2f &)),
                this,
                SLOT(textScrollChanged(const Vec2f &)));
        connect(textColorWidget,
                SIGNAL(valueChanged(const ColorRGBA &)),
                this,
                SLOT(textColorChanged(const ColorRGBA &)));
        connect(layerWidget,
                SIGNAL(valueChanged(int)),
                this,
                SLOT(layerChanged(int)));

        headerText->setText("Text");
    }

    void set(const TextComponent &value) {
        component = value;
        textWidget->setText(value.text.c_str());
        pixelSizeWidget->set(value.pixelSize.convert<float>());
        lineHeightWidget->setValue(value.lineHeight);
        lineWidthWidget->setValue(value.lineWidth);
        lineSpacingWidget->setValue(value.lineSpacing);
        alignmentWidget->setCurrentIndex(alignmentMappingReverse.at(value.alignment));
        fontWidget->setValue(value.font.getUri());
        textAnchorWidget->setCurrentIndex(anchorMappingReverse.at(value.textAnchor));
        layerWidget->setValue(value.layer);
        headerCheckBox->setChecked(component.enabled);
    }

    const TextComponent &get() const {
        return component;
    }

    virtual std::type_index getType() override {
        return component.getType();
    }

    std::type_index getComponentType() override {
        return typeid(TextComponent);
    }

signals:

    void valueChanged(const TextComponent &value);

protected:
    void checkBoxStateChange(int state) override {
        component.enabled = state == Qt::Checked;
        emit valueChanged(component);
    }

private slots:

    void textChanged(const QString &v) {
        component.text = v.toStdString().c_str();
        emit valueChanged(component);
    }

    void pixelSizeChanged(const Vec2f &v) {
        component.pixelSize = v.convert<int>();
        emit valueChanged(component);
    }

    void lineHeightChanged(int v) {
        component.lineHeight = v;
        emit valueChanged(component);
    }

    void lineWidthChanged(int v) {
        component.lineWidth = v;
        emit valueChanged(component);
    }

    void lineSpacingChanged(int v) {
        component.lineSpacing = v;
        emit valueChanged(component);
    }

    void alignmentChanged(int index) {
        component.alignment = alignmentMapping.at(index);
        emit valueChanged(component);
    }

    void fontChanged(const QString &uri) {
        component.font = ResourceHandle<RawResource>(Uri(uri.toStdString().c_str()));
        emit valueChanged(component);
    }

    void textAnchorChanged(int index) {
        component.textAnchor = anchorMapping.at(index);
        emit valueChanged(component);
    }

    void textScrollChanged(const Vec2f &v) {
        component.textScroll = v;
        emit valueChanged(component);
    }

    void textColorChanged(const ColorRGBA &v) {
        component.textColor = v;
        emit valueChanged(component);
    }

    void layerChanged(int v) {
        component.layer = v;
        emit valueChanged(component);
    }

private:
    TextComponent component;
    QLineEdit *textWidget;
    Vector2Widget *pixelSizeWidget;
    QSpinBox *lineHeightWidget;
    QSpinBox *lineWidthWidget;
    QSpinBox *lineSpacingWidget;
    QComboBox *alignmentWidget;
    UriWidget *fontWidget;
    QComboBox *textAnchorWidget;
    Vector2Widget *textScrollWidget;
    ColorWidget *textColorWidget;
    QSpinBox *layerWidget;

    std::map<int, Alignment> alignmentMapping;
    std::map<Alignment, int> alignmentMappingReverse;

    std::map<int, CanvasTransformComponent::Anchor> anchorMapping;
    std::map<CanvasTransformComponent::Anchor, int> anchorMappingReverse;
};

#endif //XEDITOR_TEXTCOMPONENTWIDGET_HPP
