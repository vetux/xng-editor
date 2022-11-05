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
#include <QMenu>
#include <QResizeEvent>

#include <utility>

#include "entityeditwidget.hpp"

#include "xng/ecs/entityscene.hpp"
#include "xng/ecs/components/transformcomponent.hpp"

class SceneEditWidget : public QWidget, public EntityScene::Listener {
Q_OBJECT
public:
    //TODO: QTreeWidget entities display
    explicit SceneEditWidget(QWidget *parent)
            : QWidget(parent) {
        splitter = new QSplitter(this);
        sceneTree = new QTreeWidget(this);
        entityEditWidget = new EntityEditWidget(this);

        setLayout(new QHBoxLayout);

        splitter->addWidget(sceneTree);
        splitter->addWidget(entityEditWidget);
        layout()->addWidget(splitter);

        sceneTree->headerItem()->setHidden(true);
        layout()->setMargin(0);

        connect(entityEditWidget, SIGNAL(addComponent()), this, SLOT(addComponent()));
        connect(entityEditWidget,
                SIGNAL(updateComponent(const Component&)),
                this,
                SLOT(update(const Component&)));
        connect(entityEditWidget,
                SIGNAL(destroyComponent(std::type_index)),
                this,
                SLOT(destroy(std::type_index)));
        connect(entityEditWidget,
                SIGNAL(destroyEntity()),
                this,
                SLOT(destroyEntity()));
    }

    ~SceneEditWidget() override {
        if (scene) {
            scene->removeListener(*this);
        }
    }

    void setScene(std::shared_ptr<xng::EntityScene> value) {
        if (scene) {
            scene->removeListener(*this);
        }
        scene = std::move(value);

        scene->addListener(*this);

        auto ent = scene->createEntity();
        selectEntity(ent);
    }

    QByteArray saveSplitterState() const {
        return splitter->saveState();
    }

    void restoreSplitterState(const QByteArray &state) const {
        splitter->restoreState(state);
    }

signals:

    void createEntity(const std::string &name);

    void destroyEntity(Entity entity);

    void setEntityName(Entity entity, const std::string &name);

    void createComponent(Entity entity, std::type_index componentType);

    void updateComponent(Entity entity, const Component &value);

    void destroyComponent(Entity entity, std::type_index type);

private slots:

    void selectEntity(Entity entity) {
        selectedEntity = entity;
        entityEditWidget->setEntity(selectedEntity);
    }

    void destroyEntity() {
        emit destroyEntity(selectedEntity);
    }

    void triggered(QAction *action) {
        auto *act = dynamic_cast<ComponentAddAction *>(action);
        emit createComponent(selectedEntity, act->getType());
    }

    void addComponent() {
        auto *widget = dynamic_cast<EntityEditWidget *>(sender());
        auto cursorPos = QCursor::pos();

        auto *menu = new QMenu(this);
        menu->addAction(new ComponentAddAction("Transform", typeid(TransformComponent), this));

        auto *menuSound = new QMenu("Sound", this);
        menuSound->addAction(new ComponentAddAction("Audio Source", typeid(TransformComponent), this));
        menuSound->addAction(new ComponentAddAction("Audio Listener", typeid(TransformComponent), this));

        auto *menuPhysics = new QMenu("Physics", this);
        menuPhysics->addAction(new ComponentAddAction("Rigidbody", typeid(RigidBodyComponent), this));

        auto *menuCanvas = new QMenu("Canvas Rendering", this);
        menuCanvas->addAction(new ComponentAddAction("Canvas", typeid(CanvasComponent), this));
        menuCanvas->addAction(new ComponentAddAction("Canvas Transform", typeid(CanvasTransformComponent), this));
        menuCanvas->addAction(new ComponentAddAction("Sprite", typeid(SpriteComponent), this));
        menuCanvas->addAction(new ComponentAddAction("Text", typeid(TextComponent), this));

        menu->addMenu(menuCanvas);
        menu->addMenu(menuPhysics);
        menu->addMenu(menuSound);

        menu->popup(cursorPos);

        connect(menu,
                SIGNAL(triggered(QAction * )),
                this,
                SLOT(triggered(QAction * )));
    }

    void update(const Component &value) {
        auto *sen = dynamic_cast<EntityEditWidget *>(sender());
        emit updateComponent(sen->getEntity(), value);
    }

    void destroy(std::type_index type) {
        auto *sen = dynamic_cast<EntityEditWidget *>(sender());
        emit destroyComponent(sen->getEntity(), type);
    }

public:
    void onEntityCreate(const EntityHandle &entity) override {
        Listener::onEntityCreate(entity);
    }

    void onEntityDestroy(const EntityHandle &entity) override {
        if (entity == selectedEntity.getHandle()) {
            selectedEntity = {};
            entityEditWidget->clearEntity();
        }
    }

    void onComponentCreate(const EntityHandle &entity, const Component &component) override {
        entityEditWidget->setEntity(Entity(entity, *scene));
    }

    void onComponentDestroy(const EntityHandle &entity, const Component &component) override {
        entityEditWidget->setEntity(Entity(entity, *scene));
    }

    void onComponentUpdate(const EntityHandle &entity, const Component &oldComponent,
                           const Component &newComponent) override {
        entityEditWidget->setEntity(Entity(entity, *scene));
    }

private:
    class ComponentAddAction : public QAction {
    public:
        ComponentAddAction(const QString &text, std::type_index type, QWidget *parent)
                : QAction(text, parent),
                  type(type) {}

        std::type_index getType() const {
            return type;
        }

    private:
        std::type_index type;
    };

    std::shared_ptr<xng::EntityScene> scene = nullptr;

    QSplitter *splitter;
    QTreeWidget *sceneTree;
    EntityEditWidget *entityEditWidget;

    std::map<xng::Entity, QTreeWidgetItem *> entityItems;

    Entity selectedEntity;
};

#endif //XEDITOR_ENTITYSCENEWIDGET_HPP
