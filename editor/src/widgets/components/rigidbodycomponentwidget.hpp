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
#ifndef XEDITOR_RIGIDBODYCOMPONENTWIDGET_HPP
#define XEDITOR_RIGIDBODYCOMPONENTWIDGET_HPP

#include "widgets/components/componentwidget.hpp"
#include "widgets/vector3widget.hpp"
#include "widgets/colliderwidget.hpp"

#include "xng/xng.hpp"

#include <QListWidget>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QStringListModel>

#include "widgets/vector3widget.hpp"
#include "widgets/urilistwidget.hpp"

//TODO: Rewrite physics component widgets

class RigidBodyComponentWidget : public ComponentWidget {
Q_OBJECT
public:
    explicit RigidBodyComponentWidget(QWidget *parent = nullptr)
            : ComponentWidget(parent) {
        typeWidget = new QComboBox;

        lockedAxeXWidget = new QCheckBox;
        lockedAxeYWidget = new QCheckBox;
        lockedAxeZWidget = new QCheckBox;

        velocityWidget = new Vector3Widget;
        angularVelocityWidget = new Vector3Widget;

        forceWidget = new Vector3Widget;
        forcePointWidget = new Vector3Widget;
        torqueWidget = new Vector3Widget;

        impulseWidget = new Vector3Widget;
        impulsePointWidget = new Vector3Widget;
        angularImpulseWidget = new Vector3Widget;

        massWidget = new QDoubleSpinBox;
        massCenterWidget = new Vector3Widget;
        rotationalInertiaWidget = new Vector3Widget;

        gravityScaleWidget = new QDoubleSpinBox;

        collidersWidget = new UriListWidget;

        auto *colBtn = new QPushButton;

        colBtn->setText("Add Collider");

        typeMapping[0] = xng::RigidBody::STATIC;
        typeMapping[1] = xng::RigidBody::KINEMATIC;
        typeMapping[2] = xng::RigidBody::DYNAMIC;
        typeMappingReverse[RigidBody::STATIC] = 0;
        typeMappingReverse[RigidBody::KINEMATIC] = 1;
        typeMappingReverse[RigidBody::DYNAMIC] = 2;
        typeWidget->setModel(new QStringListModel({"Static", "Kinematic", "Dynamic"}));

        massWidget->setRange(-1, std::numeric_limits<double>::max());
        gravityScaleWidget->setRange(0.0000001, std::numeric_limits<double>::max());

        massWidget->setMinimumWidth(10);
        gravityScaleWidget->setMinimumWidth(10);

        layout()->addWidget(new QLabel("Type"));
        layout()->addWidget(typeWidget);

        auto *widget = new QWidget;
        auto *l = new QHBoxLayout;
        l->addWidget(new QLabel("x"));
        l->addWidget(lockedAxeXWidget, 1);
        l->addWidget(new QLabel("y"));
        l->addWidget(lockedAxeYWidget, 1);
        l->addWidget(new QLabel("z"));
        l->addWidget(lockedAxeZWidget, 1);
        widget->setLayout(l);

        layout()->addWidget(new QLabel("Locked Axes"));
        layout()->addWidget(widget);

        layout()->addWidget(new QLabel("Velocity"));
        layout()->addWidget(velocityWidget);

        layout()->addWidget(new QLabel("Angular Velocity"));
        layout()->addWidget(angularVelocityWidget);

        layout()->addWidget(new QLabel("Force"));
        layout()->addWidget(forceWidget);
        layout()->addWidget(new QLabel("Force Point"));
        layout()->addWidget(forcePointWidget);
        layout()->addWidget(new QLabel("Torque"));
        layout()->addWidget(torqueWidget);

        layout()->addWidget(new QLabel("Impulse"));
        layout()->addWidget(impulseWidget);
        layout()->addWidget(new QLabel("Impulse Point"));
        layout()->addWidget(impulsePointWidget);
        layout()->addWidget(new QLabel("Angular Impulse"));
        layout()->addWidget(angularImpulseWidget);

        layout()->addWidget(new QLabel("Mass"));
        layout()->addWidget(massWidget);
        layout()->addWidget(new QLabel("Center of Mass"));
        layout()->addWidget(massCenterWidget);
        layout()->addWidget(new QLabel("Rotational Inertia"));
        layout()->addWidget(rotationalInertiaWidget);

        layout()->addWidget(new QLabel("Gravity Scale"));
        layout()->addWidget(gravityScaleWidget);

        layout()->addWidget(new QLabel("Colliders"));
        layout()->addWidget(collidersWidget);

        connect(typeWidget,
                SIGNAL(currentIndexChanged(int)),
                this,
                SLOT(typeChanged(int)));

        connect(lockedAxeXWidget,
                SIGNAL(stateChanged(int)),
                this,
                SLOT(lockedAxisXChanged(int)));
        connect(lockedAxeYWidget,
                SIGNAL(stateChanged(int)),
                this,
                SLOT(lockedAxisYChanged(int)));
        connect(lockedAxeZWidget,
                SIGNAL(stateChanged(int)),
                this,
                SLOT(lockedAxisZChanged(int)));

        connect(velocityWidget,
                SIGNAL(valueChanged(const Vec3f &)),
                this,
                SLOT(velocityChanged(const Vec3f &)));
        connect(angularVelocityWidget,
                SIGNAL(valueChanged(const Vec3f &)),
                this,
                SLOT(angularVelocityChanged(const Vec3f &)));

        connect(forceWidget,
                SIGNAL(valueChanged(const Vec3f &)),
                this,
                SLOT(forceChanged(const Vec3f &)));
        connect(forcePointWidget,
                SIGNAL(valueChanged(const Vec3f &)),
                this,
                SLOT(forcePointChanged(const Vec3f &)));
        connect(torqueWidget,
                SIGNAL(valueChanged(const Vec3f &)),
                this,
                SLOT(torqueChanged(const Vec3f &)));

        connect(impulseWidget,
                SIGNAL(valueChanged(const Vec3f &)),
                this,
                SLOT(impulseChanged(const Vec3f &)));
        connect(impulsePointWidget,
                SIGNAL(valueChanged(const Vec3f &)),
                this,
                SLOT(impulsePointChanged(const Vec3f &)));
        connect(angularImpulseWidget,
                SIGNAL(valueChanged(const Vec3f &)),
                this,
                SLOT(angularImpulseChanged(const Vec3f &)));

        connect(massWidget,
                SIGNAL(valueChanged(double)),
                this,
                SLOT(massChanged(double)));
        connect(massCenterWidget,
                SIGNAL(valueChanged(const Vec3f &)),
                this,
                SLOT(massCenterChanged(const Vec3f &)));
        connect(rotationalInertiaWidget,
                SIGNAL(valueChanged(const Vec3f &)),
                this,
                SLOT(rotationalInertiaChanged(const Vec3f &)));

        connect(gravityScaleWidget,
                SIGNAL(valueChanged(double)),
                this,
                SLOT(gravityScaleChanged(double)));

        connect(collidersWidget,
                SIGNAL(valueChanged(const std::vector<Uri> &)),
                this,
                SLOT(collidersChanged(const std::vector<Uri> &)));

        headerText->setText("Rigidbody");
    }

    void set(const RigidBodyComponent &value) {
        component = value;

        typeWidget->setCurrentIndex(typeMappingReverse.at(component.type));

        lockedAxeXWidget->setChecked(component.angularFactor.x);
        lockedAxeYWidget->setChecked(component.angularFactor.y);
        lockedAxeZWidget->setChecked(component.angularFactor.z);

        velocityWidget->set(component.velocity);
        angularVelocityWidget->set(component.angularVelocity);

        forceWidget->set(component.force);
        forcePointWidget->set(component.forcePoint);
        torqueWidget->set(component.torque);

        impulseWidget->set(component.impulse);
        impulsePointWidget->set(component.impulsePoint);
        angularImpulseWidget->set(component.angularImpulse);

        massWidget->setValue(component.mass);
        massCenterWidget->set(component.massCenter);
        rotationalInertiaWidget->set(component.rotationalInertia);

        gravityScaleWidget->setValue(component.gravityScale);

      /*  std::vector<Uri> uris;
        for (auto &v: component.colliders) {
            uris.emplace_back(v.getUri());
        }
        collidersWidget->set(uris);*/

        headerCheckBox->setChecked(component.enabled);
    }

    const RigidBodyComponent &get() const {
        return component;
    }

    virtual std::type_index getType() override {
        return component.getType();
    }

    std::type_index getComponentType() override {
        return typeid(RigidBodyComponent);
    }

signals:

    void valueChanged(const RigidBodyComponent &value);

protected:
    void checkBoxStateChange(int state) override {
        component.enabled = state == Qt::Checked;
        emit valueChanged(component);
    }

private slots:

    void typeChanged(int index) {
        component.type = typeMapping.at(index);
        emit valueChanged(component);
    }

    void lockedAxisXChanged(int state) {
        component.angularFactor.x = state == Qt::Checked;
        emit valueChanged(component);
    }

    void lockedAxisYChanged(int state) {
        component.angularFactor.y = state == Qt::Checked;
        emit valueChanged(component);
    }

    void lockedAxisZChanged(int state) {
        component.angularFactor.z = state == Qt::Checked;
        emit valueChanged(component);
    }

    void velocityChanged(const Vec3f &velocity) {
        component.velocity = velocity;
        emit valueChanged(component);
    }

    void angularVelocityChanged(const Vec3f &angularVelocity) {
        component.angularVelocity = angularVelocity;
        emit valueChanged(component);
    }

    void forceChanged(const Vec3f &force) {
        component.force = force;
        emit valueChanged(component);
    }

    void forcePointChanged(const Vec3f &forcePoint) {
        component.forcePoint = forcePoint;
        emit valueChanged(component);
    }

    void torqueChanged(const Vec3f &torque) {
        component.torque = torque;
        emit valueChanged(component);
    }

    void impulseChanged(const Vec3f &impulse) {
        component.impulse = impulse;
        emit valueChanged(component);
    }

    void impulsePointChanged(const Vec3f &impulsePoint) {
        component.impulsePoint = impulsePoint;
        emit valueChanged(component);
    }

    void angularImpulseChanged(const Vec3f &angularImpulse) {
        component.angularImpulse = angularImpulse;
        emit valueChanged(component);
    }

    void massChanged(double mass) {
        component.mass = mass;
        emit valueChanged(component);
    }

    void massCenterChanged(const Vec3f &massCenter) {
        component.massCenter = massCenter;
        emit valueChanged(component);
    }

    void rotationalInertiaChanged(const Vec3f &rotationalInertia) {
        component.rotationalInertia = rotationalInertia;
        emit valueChanged(component);
    }

    void gravityScaleChanged(double gravityScale) {
        component.gravityScale = gravityScale;
        emit valueChanged(component);
    }

    void collidersChanged(const std::vector<Uri> &colliders) {
      /*  component.colliders.clear();
        for (auto &c: colliders) {
            component.colliders.emplace_back(ResourceHandle<ColliderDesc>(c));
        }*/
        emit valueChanged(component);
    }

private:
    RigidBodyComponent component;

    std::map<int, RigidBody::RigidBodyType> typeMapping;
    std::map<RigidBody::RigidBodyType, int> typeMappingReverse;

    QComboBox *typeWidget;
    QCheckBox *lockedAxeXWidget;
    QCheckBox *lockedAxeYWidget;
    QCheckBox *lockedAxeZWidget;

    Vector3Widget *velocityWidget;
    Vector3Widget *angularVelocityWidget;

    Vector3Widget *forceWidget;
    Vector3Widget *forcePointWidget;
    Vector3Widget *torqueWidget;

    Vector3Widget *impulseWidget;
    Vector3Widget *impulsePointWidget;
    Vector3Widget *angularImpulseWidget;

    QDoubleSpinBox *massWidget;
    Vector3Widget *massCenterWidget;
    Vector3Widget *rotationalInertiaWidget;

    QDoubleSpinBox *gravityScaleWidget;

    UriListWidget *collidersWidget;
};

#endif //XEDITOR_RIGIDBODYCOMPONENTWIDGET_HPP
