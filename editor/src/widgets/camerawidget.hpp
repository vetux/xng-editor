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
#ifndef XEDITOR_CAMERAWIDGET_HPP
#define XEDITOR_CAMERAWIDGET_HPP

#include <QLineEdit>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QStringListModel>

#include "xng/xng.hpp"

class CameraWidget : public QWidget {
Q_OBJECT
public:
    explicit CameraWidget(QWidget *parent = nullptr)
            : QWidget(parent) {
        typeWidget = new QComboBox(this);

        nearClipWidget = new QDoubleSpinBox(this);
        farClipWidget = new QDoubleSpinBox(this);

        fovWidget = new QDoubleSpinBox(this);
        aspectRatioWidget = new QDoubleSpinBox(this);

        leftWidget = new QDoubleSpinBox(this);
        topWidget = new QDoubleSpinBox(this);
        rightWidget = new QDoubleSpinBox(this);
        bottomWidget = new QDoubleSpinBox(this);

        nearClipWidget->setRange(0, std::numeric_limits<double>().max());
        farClipWidget->setRange(0, std::numeric_limits<double>().max());

        fovWidget->setRange(0, std::numeric_limits<double>().max());
        aspectRatioWidget->setRange(0, std::numeric_limits<double>().max());

        leftWidget->setRange(std::numeric_limits<double>().min(), std::numeric_limits<double>().max());
        topWidget->setRange(std::numeric_limits<double>().min(), std::numeric_limits<double>().max());
        rightWidget->setRange(std::numeric_limits<double>().min(), std::numeric_limits<double>().max());
        bottomWidget->setRange(std::numeric_limits<double>().min(), std::numeric_limits<double>().max());

        nearClipWidget->setMinimumWidth(10);
        nearClipWidget->setMinimumWidth(10);

        fovWidget->setMinimumWidth(10);
        aspectRatioWidget->setMinimumWidth(10);

        leftWidget->setMinimumWidth(10);
        topWidget->setMinimumWidth(10);
        rightWidget->setMinimumWidth(10);
        bottomWidget->setMinimumWidth(10);

        setLayout(new QVBoxLayout());
        layout()->addWidget(new QLabel("Type"));
        layout()->addWidget(typeWidget);

        layout()->addWidget(new QLabel("Near Clip"));
        layout()->addWidget(nearClipWidget);
        layout()->addWidget(new QLabel("Far Clip"));
        layout()->addWidget(farClipWidget);

        fovLabel = new QLabel("Field Of View");
        layout()->addWidget(fovLabel);
        layout()->addWidget(fovWidget);
        aspectRatioLabel = new QLabel("Aspect Ratio");
        layout()->addWidget(aspectRatioLabel);
        layout()->addWidget(aspectRatioWidget);

        leftWidgetLabel = new QLabel("Left");
        layout()->addWidget(leftWidgetLabel);
        layout()->addWidget(leftWidget);
        topWidgetLabel = new QLabel("Top");
        layout()->addWidget(topWidgetLabel);
        layout()->addWidget(topWidget);
        rightWidgetLabel = new QLabel("Right");
        layout()->addWidget(rightWidgetLabel);
        layout()->addWidget(rightWidget);
        bottomWidgetLabel = new QLabel("Bottom");
        layout()->addWidget(bottomWidgetLabel);
        layout()->addWidget(bottomWidget);

        layout()->setMargin(0);

        typeMapping[0] = xng::PERSPECTIVE;
        typeMapping[1] = xng::ORTHOGRAPHIC;
        typeMappingReverse[PERSPECTIVE] = 0;
        typeMappingReverse[ORTHOGRAPHIC] = 1;
        typeWidget->setModel(new QStringListModel({"Perspective", "Orthographic"}));
        connect(typeWidget, SIGNAL(currentIndexChanged(int)), this, SLOT(currentTypeChanged(int)));
        connect(nearClipWidget,
                SIGNAL(valueChanged(double)),
                this,
                SLOT(nearClipChanged(double)));
        connect(farClipWidget,
                SIGNAL(valueChanged(double)),
                this,
                SLOT(farClipChanged(double)));
        connect(fovWidget,
                SIGNAL(valueChanged(double)),
                this,
                SLOT(fovChanged(double)));
        connect(aspectRatioWidget,
                SIGNAL(valueChanged(double)),
                this,
                SLOT(aspectRatioChanged(double)));
        connect(leftWidget,
                SIGNAL(valueChanged(double)),
                this,
                SLOT(leftChanged(double)));
        connect(topWidget,
                SIGNAL(valueChanged(double)),
                this,
                SLOT(topChanged(double)));
        connect(rightWidget,
                SIGNAL(valueChanged(double)),
                this,
                SLOT(rightChanged(double)));
        connect(bottomWidget,
                SIGNAL(valueChanged(double)),
                this,
                SLOT(bottomChanged(double)));
    }

    void setValue(const Camera &c) {
        camera = c;
        fovWidget->setVisible(camera.type == xng::PERSPECTIVE);
        aspectRatioWidget->setVisible(camera.type == xng::PERSPECTIVE);

        fovLabel->setVisible(camera.type == xng::PERSPECTIVE);
        aspectRatioLabel->setVisible(camera.type == xng::PERSPECTIVE);

        leftWidget->setVisible(camera.type == xng::ORTHOGRAPHIC);
        topWidget->setVisible(camera.type == xng::ORTHOGRAPHIC);
        rightWidget->setVisible(camera.type == xng::ORTHOGRAPHIC);
        bottomWidget->setVisible(camera.type == xng::ORTHOGRAPHIC);

        leftWidgetLabel->setVisible(camera.type == xng::ORTHOGRAPHIC);
        topWidgetLabel->setVisible(camera.type == xng::ORTHOGRAPHIC);
        rightWidgetLabel->setVisible(camera.type == xng::ORTHOGRAPHIC);
        bottomWidgetLabel->setVisible(camera.type == xng::ORTHOGRAPHIC);

        typeWidget->setCurrentIndex(typeMappingReverse.at(camera.type));
        nearClipWidget->setValue(camera.nearClip);
        farClipWidget->setValue(camera.farClip);
        fovWidget->setValue(camera.fov);
        aspectRatioWidget->setValue(camera.aspectRatio);
        leftWidget->setValue(camera.left);
        topWidget->setValue(camera.top);
        rightWidget->setValue(camera.right);
        bottomWidget->setValue(camera.bottom);
    }

signals:

    void valueChanged(const Camera &camera);

private slots:

    void currentTypeChanged(int index) {
        camera.type = typeMapping.at(index);
        emit valueChanged(camera);
    }

    void nearClipChanged(double value) {
        camera.nearClip = value;
        emit valueChanged(camera);
    }

    void farClipChanged(double value) {
        camera.farClip = value;
        emit valueChanged(camera);
    }

    void fovChanged(double value) {
        camera.fov = value;
        emit valueChanged(camera);
    }

    void aspectRatioChanged(double value) {
        camera.aspectRatio = value;
        emit valueChanged(camera);
    }

    void leftChanged(double value) {
        camera.left = value;
        emit valueChanged(camera);
    }

    void topChanged(double value) {
        camera.top = value;
        emit valueChanged(camera);
    }

    void rightChanged(double value) {
        camera.right = value;
        emit valueChanged(camera);
    }

    void bottomChanged(double value) {
        camera.bottom = value;
        emit valueChanged(camera);
    }

private:
    Camera camera;

    std::map<int, CameraType> typeMapping;
    std::map<CameraType, int> typeMappingReverse;
    QComboBox *typeWidget;
    QDoubleSpinBox *nearClipWidget;
    QDoubleSpinBox *farClipWidget;

    QLabel *fovLabel;
    QDoubleSpinBox *fovWidget;
    QLabel *aspectRatioLabel;
    QDoubleSpinBox *aspectRatioWidget;

    QLabel *leftWidgetLabel;
    QDoubleSpinBox *leftWidget;

    QLabel *topWidgetLabel;
    QDoubleSpinBox *topWidget;

    QLabel *rightWidgetLabel;
    QDoubleSpinBox *rightWidget;

    QLabel *bottomWidgetLabel;
    QDoubleSpinBox *bottomWidget;
};

#endif //XEDITOR_CAMERAWIDGET_HPP
