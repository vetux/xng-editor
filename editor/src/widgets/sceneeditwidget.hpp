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

#include "widgets/entityeditwidget.hpp"

#include "xng/ecs/entityscene.hpp"
#include "xng/ecs/components/transformcomponent.hpp"

class SceneEditWidget : public QWidget, public EntityScene::Listener {
Q_OBJECT
public:
    explicit SceneEditWidget(QWidget *parent)
            : QWidget(parent) {
        splitter = new QSplitter(this);
        sceneTree = new QTreeWidget(this);
        sceneTree->setSelectionMode(QAbstractItemView::ExtendedSelection);
        entityEditWidget = new EntityEditWidget(this);

        setLayout(new QHBoxLayout);

        splitter->addWidget(sceneTree);
        splitter->addWidget(entityEditWidget);
        layout()->addWidget(splitter);

        sceneTree->headerItem()->setHidden(true);
        layout()->setMargin(0);

        sceneTree->setContextMenuPolicy(Qt::CustomContextMenu);
        sceneTree->viewport()->installEventFilter(this);

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
                SIGNAL(destroyGenericComponent(const std::string &)),
                this,
                SLOT(destroy(const std::string &)));
        connect(entityEditWidget,
                SIGNAL(updateComponent(const std::string &, const Message &)),
                this,
                SLOT(update(const std::string &, const Message &)));
        connect(entityEditWidget,
                SIGNAL(destroyEntity()),
                this,
                SLOT(destroyEntity()));
        connect(entityEditWidget,
                SIGNAL(updateEntityName(const QString &)),
                this,
                SLOT(updateEntityName(const QString &)));
        connect(sceneTree,
                SIGNAL(currentItemChanged(QTreeWidgetItem * , QTreeWidgetItem * )),
                this,
                SLOT(currentItemChanged(QTreeWidgetItem * , QTreeWidgetItem * )));
        connect(sceneTree,
                SIGNAL(customContextMenuRequested(const QPoint &)),
                this,
                SLOT(contextMenuRequested(const QPoint &)));
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

        if (scene) {
            scene->addListener(*this);
        }
    }

    void setAvailableComponentMetadata(const std::map<std::string, ComponentMetadata> &metadata) {
        availableMetadata = metadata;
    }

    const Entity &getSelectedEntity() const {
        return selectedEntity;
    }

    QByteArray saveSplitterState() const {
        return splitter->saveState();
    }

    void restoreSplitterState(const QByteArray &state) const {
        splitter->restoreState(state);
    }

signals:

    void createEntity();

    void createEntity(const std::string &name);

    void destroyEntity(const Entity &entity);

    void setEntityName(const Entity &entity, const std::string &name);

    void createComponent(const Entity &entity, std::type_index componentType);

    void createComponent(const Entity &entity, const std::string &typeName);

    void updateComponent(const Entity &entity, const Component &value);

    void updateComponent(const Entity &entity, const GenericComponent &value);

    void destroyComponent(const Entity &entity, std::type_index type);

    void destroyComponent(const Entity &entity, const std::string &typeName);

private slots:

    void selectEntity(Entity entity) {
        selectedEntity = entity;
        entityEditWidget->setEntity(selectedEntity, availableMetadata);
    }

    void destroyEntity() {
        emit destroyEntity(selectedEntity);
    }

    void triggered(QAction *action) {
        auto *act = dynamic_cast<ComponentAddAction *>(action);
        if (act) {
            emit createComponent(selectedEntity, act->getType());
        } else {
            auto *gAct = dynamic_cast<GenericComponentAddAction *>(action);
            if (gAct) {
                emit createComponent(selectedEntity, gAct->getType());
            }
        }
    }

    void addComponent() {
        auto *widget = dynamic_cast<EntityEditWidget *>(sender());
        auto cursorPos = QCursor::pos();

        auto *menu = new QMenu(this);
        menu->addAction(new ComponentAddAction("Transform", typeid(TransformComponent), this));
        menu->addAction(new ComponentAddAction("Rect Transform", typeid(RectTransformComponent), this));

        auto *menuSound = new QMenu("Sound", this);
        menuSound->addAction(new ComponentAddAction("Audio Source", typeid(AudioSourceComponent), this));
        menuSound->addAction(new ComponentAddAction("Audio Listener", typeid(AudioListenerComponent), this));

        auto *menuPhysics = new QMenu("Physics", this);
        menuPhysics->addAction(new ComponentAddAction("Rigidbody", typeid(RigidBodyComponent), this));

        auto *menu3D = new QMenu("3D", this);
        menu3D->addAction(new ComponentAddAction("Camera", typeid(CameraComponent), this));
        menu3D->addAction(new ComponentAddAction("Light", typeid(LightComponent), this));
        menu3D->addAction(new ComponentAddAction("Skinned Mesh", typeid(SkinnedMeshComponent), this));
        menu3D->addAction(new ComponentAddAction("Skybox", typeid(SkyboxComponent), this));

        auto *menu2D = new QMenu("2D", this);
        menu2D->addAction(new ComponentAddAction("Canvas", typeid(CanvasComponent), this));
        menu2D->addAction(new ComponentAddAction("Sprite", typeid(SpriteComponent), this));
        menu2D->addAction(new ComponentAddAction("Sprite Animation", typeid(SpriteAnimationComponent), this));
        menu2D->addAction(new ComponentAddAction("Text", typeid(TextComponent), this));
        menu2D->addAction(new ComponentAddAction("Button", typeid(ButtonComponent), this));

        menu->addMenu(menu3D);
        menu->addMenu(menu2D);
        menu->addMenu(menuPhysics);
        menu->addMenu(menuSound);

        std::map<std::string, QMenu *> userMenus;

        // TODO: Create separate menus for subdirectories
        for (auto &metadata: availableMetadata) {
            std::string category = "User Components";
            if (!metadata.second.category.empty()) {
                category = metadata.second.category;
            }
            if (userMenus.find(category) == userMenus.end()) {
                userMenus[category] = new QMenu(category.c_str(), this);;
            }
            auto m = userMenus.at(category);
            m->addAction(new GenericComponentAddAction(metadata.second.typeName, this));
        }

        for (auto &pair: userMenus) {
            menu->addMenu(pair.second);
        }

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

    void update(const std::string &typeName, const Message &value) {
        auto *sen = dynamic_cast<EntityEditWidget *>(sender());
        auto comp = sen->getEntity().getComponent<GenericComponent>();
        comp.components.at(typeName) = value;
        emit updateComponent(sen->getEntity(), comp);
    }

    void destroy(const std::string &typeName) {
        auto *sen = dynamic_cast<EntityEditWidget *>(sender());
        emit destroyComponent(sen->getEntity(), typeName);
    }

    void currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous) {
        if (current == nullptr) {
            selectedEntity = {};
        } else {
            auto &ent = entityItemsReverse.at(current);
            selectedEntity = xng::Entity(ent, *scene);
        }
        entityEditWidget->setEntity(selectedEntity, availableMetadata);
    }

    void contextMenuRequested(const QPoint &pos) {
        QMenu contextMenu(this);
        auto *action = new QAction("Create Entity", this);
        connect(action, SIGNAL(triggered()), this, SIGNAL(createEntity()));
        contextMenu.addAction(action);

        if (selectedEntity) {
            action = new QAction(selectedEntity.hasName()
                                 ? "Destroy " + QString(selectedEntity.getName().c_str())
                                 : "Destroy Entity",
                                 this);
            connect(action, SIGNAL(triggered()), this, SLOT(destroyEntitySlot()));
            contextMenu.addAction(action);
        }

        contextMenu.exec(mapToGlobal(pos));
    }

    void destroyEntitySlot() {
        emit destroyEntity(selectedEntity);
    }

    void updateEntityName(const QString &name) {
        emit setEntityName(entityEditWidget->getEntity(), name.toStdString().c_str());
    }

public:
    bool eventFilter(QObject *watched, QEvent *event) override {
        if (event->type() == QEvent::Type::MouseButtonPress) {
            auto *ev = dynamic_cast<QMouseEvent *>(event);
            auto index = sceneTree->indexAt(ev->pos());
            if (index.isValid()) {
                return QWidget::eventFilter(watched, event);
            } else {
                if (selectedEntity) {
                    sceneTree->selectionModel()->clear();
                    selectedEntity = {};
                    entityEditWidget->setEntity(selectedEntity, availableMetadata);
                }
                return true;
            }
        } else {
            return QWidget::eventFilter(watched, event);
        }
    }

public:
    void onEntityCreate(const EntityHandle &entity) override {
        auto *item = new QTreeWidgetItem(sceneTree,
                                         {scene->entityHasName(entity)
                                          ? scene->getEntityName(entity).c_str()
                                          : "Unnamed Entity"});
        if (scene->checkComponent<TransformComponent>(entity)) {
            auto &t = scene->getComponent<TransformComponent>(entity);
            if (scene->entityNameExists(t.parent)) {
                auto parent = scene->getEntity(t.parent);
                auto *parentItem = entityItems.at(parent.getHandle());
                parentItem->addChild(item);
            }
        } else {
            sceneTree->addTopLevelItem(item);
        }
        entityItems[entity] = item;
        entityItemsReverse[item] = entity;
        entityEditWidget->setEntity(selectedEntity, availableMetadata);
    }

    void onEntityDestroy(const EntityHandle &entity) override {
        if (selectedEntity && entity == selectedEntity.getHandle()) {
            selectedEntity = {};
            entityEditWidget->setEntity(selectedEntity, availableMetadata);
        }
        auto *item = entityItems.at(entity);
        auto *parent = entityItems.at(entity)->parent();
        if (parent == nullptr) {
            sceneTree->takeTopLevelItem(sceneTree->indexOfTopLevelItem(item));
        } else {
            parent->removeChild(item);
        }
        entityItems.erase(entity);
        entityItemsReverse.erase(item);
        entityEditWidget->setEntity(selectedEntity, availableMetadata);
    }

    void onEntityNameChanged(const EntityHandle &entity,
                             const std::string &newName,
                             const std::string &oldName) override {
        // Update child item parents
        if (!oldName.empty()) {
            std::map<EntityHandle, TransformComponent> comps;
            for (auto &pair: scene->getPool<TransformComponent>()) {
                if (pair.second.parent == oldName) {
                    auto tcomp = pair.second;
                    tcomp.parent = newName;
                    comps[pair.first] = tcomp;
                }
            }
            for (auto &pair: comps) {
                scene->updateComponent<>(pair.first, pair.second);
            }
        }

        // Update text
        entityItems.at(entity)->setText(0, newName.c_str());
        entityEditWidget->setEntity(selectedEntity, availableMetadata);
    }

    void onComponentCreate(const EntityHandle &entity, const Component &component) override {
        if (component.getType() == typeid(TransformComponent)) {
            auto &tcomp = dynamic_cast<const TransformComponent &>(component);

            auto currentItem = sceneTree->currentItem();

            // Re-Add item to appropriate parent
            auto *item = entityItems.at(entity);
            if (item->parent() != nullptr) {
                item->parent()->removeChild(item);
            } else {
                sceneTree->takeTopLevelItem(sceneTree->indexOfTopLevelItem(item));
            }

            if (!tcomp.parent.empty() && scene->entityNameExists(tcomp.parent)) {
                auto parentEnt = scene->getEntity(tcomp.parent);
                auto *parentItem = entityItems.at(parentEnt.getHandle());
                parentItem->addChild(item);
            } else {
                sceneTree->addTopLevelItem(item);
            }

            sceneTree->setCurrentItem(currentItem);
        }
        entityEditWidget->setEntity(selectedEntity, availableMetadata);
    }

    void onComponentDestroy(const EntityHandle &entity, const Component &component) override {
        if (component.getType() == typeid(TransformComponent)) {
            auto currentItem = sceneTree->currentItem();
            // Re-Add item to top level
            auto *item = entityItems.at(entity);
            if (item->parent() != nullptr) {
                item->parent()->removeChild(item);
            } else {
                sceneTree->takeTopLevelItem(sceneTree->indexOfTopLevelItem(item));
            }
            sceneTree->addTopLevelItem(item);
            sceneTree->setCurrentItem(currentItem);
        }
        entityEditWidget->setEntity(selectedEntity, availableMetadata);
    }

    void onComponentUpdate(const EntityHandle &entity,
                           const Component &oldComponent,
                           const Component &newComponent) override {
        if (newComponent.getType() == typeid(GenericComponent)) {
            auto &oldComp = dynamic_cast<const GenericComponent &>(oldComponent);
            auto &newComp = dynamic_cast<const GenericComponent &>(newComponent);
            if (oldComp.components.size() != newComp.components.size()){
                entityEditWidget->setEntity(selectedEntity, availableMetadata);
            }
        } else if (newComponent.getType() == typeid(TransformComponent)) {
            auto &oldComp = dynamic_cast<const TransformComponent &>(oldComponent);
            auto &newComp = dynamic_cast<const TransformComponent &>(newComponent);

            if (oldComp.parent == newComp.parent) {
                // Parent unchanged
                return;
            }

            auto oldTopLevel = !oldComp.parent.empty() && scene->entityNameExists(oldComp.parent);
            auto newTopLevel = !newComp.parent.empty() && scene->entityNameExists(newComp.parent);

            if (!oldTopLevel
                && oldTopLevel == newTopLevel) {
                // Still top level
                return;
            }

            auto currentItem = sceneTree->currentItem();

            // Re-Add item to appropriate parent
            auto *item = entityItems.at(entity);
            if (item->parent() != nullptr) {
                item->parent()->removeChild(item);
            } else {
                sceneTree->takeTopLevelItem(sceneTree->indexOfTopLevelItem(item));
            }
            if (!newComp.parent.empty() && scene->entityNameExists(newComp.parent)) {
                auto parentEnt = scene->getEntity(newComp.parent);
                auto *parentItem = entityItems.at(parentEnt.getHandle());
                parentItem->addChild(item);
            } else {
                sceneTree->addTopLevelItem(item);
            }

            sceneTree->setCurrentItem(currentItem);
        }
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

    class GenericComponentAddAction : public QAction {
    public:
        GenericComponentAddAction(std::string type, QWidget *parent)
                : QAction(type.c_str(), parent),
                  type(std::move(type)) {}

        std::string getType() const {
            return type;
        }

    private:
        std::string type;
    };

    std::shared_ptr<xng::EntityScene> scene = nullptr;

    QSplitter *splitter;
    QTreeWidget *sceneTree;
    EntityEditWidget *entityEditWidget;

    std::map<xng::EntityHandle, QTreeWidgetItem *> entityItems;
    std::map<QTreeWidgetItem *, xng::EntityHandle> entityItemsReverse;

    Entity selectedEntity;

    std::map<std::string, ComponentMetadata> availableMetadata;
};

#endif //XEDITOR_ENTITYSCENEWIDGET_HPP
