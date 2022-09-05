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

#ifndef XEDITOR_ENTITYSCENEWIDGET_HPP
#define XEDITOR_ENTITYSCENEWIDGET_HPP

#include <QWidget>
#include <QTreeWidget>
#include <QHBoxLayout>
#include <QSplitter>
#include <QHeaderView>
#include <utility>

#include "entitywidget.hpp"

#include "ecs/entityscene.hpp"
#include "ecs/components/transformcomponent.hpp"

class EntitySceneWidget : public QWidget, public xng::EntityScene::Listener {
Q_OBJECT
public:
    //TODO: QTreeWidget entities display
    explicit EntitySceneWidget(QWidget *parent)
            : QWidget(parent) {
        splitter = new QSplitter(this);
        sceneTree = new QTreeWidget(this);
        entityEditWidget = new EntityWidget(this);

        setLayout(new QHBoxLayout);

        splitter->addWidget(sceneTree);
        splitter->addWidget(entityEditWidget);
        layout()->addWidget(splitter);

        sceneTree->headerItem()->setHidden(true);
        layout()->setMargin(0);
    }

    ~EntitySceneWidget() override {
        if (scene)
            scene->removeListener(*this);
    }

    void setScene(std::shared_ptr<xng::EntityScene> value) {
        if (scene)
            scene->removeListener(*this);
        scene = std::move(value);
        scene->addListener(*this);
    }

    QByteArray saveSplitterState() const {
        return splitter->saveState();
    }

    void restoreSplitterState(const QByteArray &state) const {
        splitter->restoreState(state);
    }

signals:
    void entityCreated(const std::string &name);

    void componentAdded(EntityHandle entity, std::any component, std::type_index componentType);

private:
    std::shared_ptr<xng::EntityScene> scene = nullptr;

    QSplitter *splitter;
    QTreeWidget *sceneTree;
    EntityWidget *entityEditWidget;

    std::map<xng::Entity, QTreeWidgetItem *> entityItems;
};

#endif //XEDITOR_ENTITYSCENEWIDGET_HPP
