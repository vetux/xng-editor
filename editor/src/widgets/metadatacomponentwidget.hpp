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
#ifndef XEDITOR_COMPONENTMETADATAWIDGET_HPP
#define XEDITOR_COMPONENTMETADATAWIDGET_HPP

#include "widgets/components/componentwidget.hpp"

#include <utility>

#include <QSpinBox>

#include "headertool/componentmetadata.hpp"

/**
* The list of supported types for XVARIABLE invocations
*/
enum VariableType {
    // Fundamental Types
    TYPE_CHAR,
    TYPE_SHORT,
    TYPE_INT,
    TYPE_LONG,
    TYPE_BOOL,
    TYPE_FLOAT,
    TYPE_DOUBLE,

    // Standard Types
    TYPE_STRING,

    // Standard Containers
    TYPE_VECTOR,
    TYPE_MAP,

    // xEngine Types
    TYPE_RESOURCE_HANDLE,
    TYPE_URI,
    TYPE_ENTITY_NAME,

    //      - Assets -
    TYPE_AUDIO,
    TYPE_COLOR,
    TYPE_IMAGE,
    TYPE_LIGHT,
    TYPE_MATERIAL,
    TYPE_MESH,
    TYPE_SHADER,
    TYPE_SKYBOX,
    TYPE_SPRITE,
    TYPE_TEXTURE,
    TYPE_SPRITE_ANIMATION,

    //      - Math -
    TYPE_GRID,
    TYPE_MATRIX,
    TYPE_RECTANGLE,
    TYPE_TRANSFORM,
    TYPE_VECTOR2,
    TYPE_VECTOR3,
    TYPE_VECTOR4,
};

/**
 * The corresponding names for the supported types.
 * Only single word fundamental types are supported.
 * For standard types the full name space must be specified,
 * for xEngine types the xng:: namespace specification is optional.
 */
const std::map<std::string, VariableType> typeNameMapping = {
        {"char",                 TYPE_CHAR},
        {"short",                TYPE_SHORT},
        {"int",                  TYPE_INT},
        {"long",                 TYPE_LONG},
        {"bool",                 TYPE_BOOL},
        {"float",                TYPE_FLOAT},
        {"double",               TYPE_FLOAT},

        {"std::string",          TYPE_STRING},

        {"std::vector",          TYPE_VECTOR},
        {"std::map",             TYPE_MAP},

        {"ResourceHandle",       TYPE_RESOURCE_HANDLE},
        {"xng::ResourceHandle",  TYPE_RESOURCE_HANDLE},
        {"Uri",                  TYPE_URI},
        {"xng::Uri",             TYPE_URI},
        {"EntityName",           TYPE_ENTITY_NAME},
        {"xng::EntityName",      TYPE_ENTITY_NAME},

        {"Audio",                TYPE_AUDIO},
        {"xng::Audio",           TYPE_AUDIO},
        {"ColorRGBA",            TYPE_COLOR},
        {"xng::ColorRGBA",       TYPE_COLOR},
        {"ColorRGB",             TYPE_COLOR},
        {"xng::ColorRGB",        TYPE_COLOR},
        {"ImageRGBA",            TYPE_IMAGE},
        {"xng::ImageRGBA",       TYPE_IMAGE},
        {"Light",                TYPE_LIGHT},
        {"xng::Light",           TYPE_LIGHT},
        {"Material",             TYPE_MATERIAL},
        {"xng::Material",        TYPE_MATERIAL},
        {"Mesh",                 TYPE_MESH},
        {"xng::Mesh",            TYPE_MESH},
        {"Shader",               TYPE_SHADER},
        {"xng::Shader",          TYPE_SHADER},
        {"Skybox",               TYPE_SKYBOX},
        {"xng::Skybox",          TYPE_SKYBOX},
        {"Sprite",               TYPE_SPRITE},
        {"xng::Sprite",          TYPE_SPRITE},
        {"Texture",              TYPE_TEXTURE},
        {"xng::Texture",         TYPE_TEXTURE},
        {"SpriteAnimation",      TYPE_SPRITE_ANIMATION},
        {"xng::SpriteAnimation", TYPE_SPRITE_ANIMATION},
};

class MemberWidget : public QWidget {
Q_OBJECT
public:
    MemberWidget(QWidget *parent,
                 xng::ComponentMetadata::MemberMetadata m,
                 Message message)
            : QWidget(parent), metadata(std::move(m)), value(std::move(message)) {
        auto *layout = new QHBoxLayout;

        layout->addWidget(new QLabel(metadata.displayName.empty()
                                     ? metadata.instanceName.c_str()
                                     : metadata.displayName.c_str()));

        //TODO: Implement widget constructors
        if (typeNameMapping.find(metadata.type.typeName) != typeNameMapping.end()) {
            switch (typeNameMapping.at(metadata.type.typeName)) {
                default:
                    layout->addWidget(new QLabel("Unsupported Type"), 1);
                    break;
                case TYPE_INT: {
                    auto *widget = new QSpinBox(this);
                    QObject::connect(widget,
                                     QOverload<int>::of(&QSpinBox::valueChanged),
                                     this,
                                     [this](int v) {
                                         value = Message(v);
                                         emit valueChanged(value);
                                     },
                                     Qt::ConnectionType::AutoConnection);
                    int defaultValue = 0;
                    try {
                        defaultValue = std::stoi(metadata.defaultValue);
                    } catch (const std::exception &e) {}
                    int val = defaultValue;
                    if (value.getType() == xng::Message::SIGNED_INTEGER) {
                        val = value.asInt();
                    }
                    int min = std::numeric_limits<int>::min();
                    int max = std::numeric_limits<int>::max();
                    try {
                        if (metadata.minimum.type == xng::Token::LITERAL_NUMERIC) {
                            min = std::stoi(metadata.minimum.value);
                        }
                        if (metadata.maximum.type == xng::Token::LITERAL_NUMERIC) {
                            max = std::stoi(metadata.maximum.value);
                        }
                    } catch (const std::exception &e) {}
                    widget->setMinimum(min);
                    widget->setMaximum(max);
                    widget->setValue(val);
                    layout->addWidget(widget);
                }
                    break;
            }
        } else {
            layout->addWidget(new QLabel("Unsupported Type"), 1);
        }

        setLayout(layout);
    }


    const Message &getValue() const {
        return value;
    }

signals:

    void valueChanged(const Message &msg);

private:
    xng::ComponentMetadata::MemberMetadata metadata;
    Message value;
};

class MetadataComponentWidget : public ComponentWidget {
Q_OBJECT
public:
    MetadataComponentWidget(QWidget *parent,
                            const ComponentMetadata &componentMetadata,
                            const Message &value)
            : ComponentWidget(parent), metadata(componentMetadata) {
        headerText->setText(componentMetadata.typeName.c_str());
        parseMessage(value);
    }

    std::type_index getComponentType() override {
        throw std::runtime_error("getComponentType() called on MetadataComponentWidget");
    }

    const std::string &getComponentTypeName() const {
        return metadata.typeName;
    }

    Message getMessage() {
        std::map<std::string, Message> ret;
        for (auto &comp: memberWidgets) {
            ret.insert({comp.first, comp.second->getValue()});
        }
        ret.insert({"enabled", headerCheckBox->isChecked()});
        return {ret};
    }

signals:

    void dataChanged(const std::string &typeName, const Message &value);

protected:
    void checkBoxStateChange(int state) override {}

private slots:

    void valueChanged(const Message &memberValue) {
        emit dataChanged(metadata.typeName, getMessage());
    }

private:
    void parseMessage(const Message &value) {
        for (auto &member: metadata.members) {
            Message memberMsg;
            if (value.getType() == xng::Message::DICTIONARY
                && value.has(member.instanceName.c_str())) {
                memberMsg = value[member.instanceName.c_str()];
            }
            auto *widget = new MemberWidget(this, member, memberMsg);
            layout()->addWidget(widget);
            memberWidgets[member.instanceName] = widget;
            connect(widget, SIGNAL(valueChanged(const Message &)), this, SLOT(valueChanged(const Message &)));
        }

        bool val;
        value.value("enabled", val, true);
        headerCheckBox->setChecked(val);
    }

    ComponentMetadata metadata;
    std::map<std::string, MemberWidget *> memberWidgets;
};

#endif //XEDITOR_COMPONENTMETADATAWIDGET_HPP
