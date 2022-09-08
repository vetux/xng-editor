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

#include "gui/mainwindow.hpp"

#include <QVBoxLayout>
#include <QKeyEvent>
#include <QMenuBar>
#include <QSizePolicy>
#include <QMessageBox>

#include <fstream>

#include "xengine.hpp"

#include "gui/widgets/entitywidget.hpp"

using namespace xng;

MainWindow::MainWindow() {
    scene = std::make_shared<EntityScene>();

    menuBar()->addMenu("File");

    rootWidget = new QWidget(this);

    setCentralWidget(rootWidget);

    rootLayout = new QHBoxLayout();

    sceneEditWidget = new EntitySceneWidget(this);
    fileBrowserWidget = new FileBrowserWidget(this);

    middleSplitter = new QSplitter(this);
    leftSplitter = new QSplitter(this);
    rightSplitter = new QSplitter(this);

    tabWidget = new QTabWidget(this);

    leftSplitter->setOrientation(Qt::Vertical);
    rightSplitter->setOrientation(Qt::Vertical);

    leftSplitter->addWidget(tabWidget);

    rightSplitter->addWidget(sceneEditWidget);
    rightSplitter->addWidget(fileBrowserWidget);

    middleSplitter->addWidget(leftSplitter);
    middleSplitter->addWidget(rightSplitter);

    rootLayout->addWidget(middleSplitter);

    rootWidget->setLayout(rootLayout);

    connect(&timer, SIGNAL(timeout()), this, SLOT(onTimeout()));

    timer.start(1000 / 60);

    loadStateFile();

    sceneEditWidget->setScene(scene);

    connect(sceneEditWidget,
            SIGNAL(createEntity(const std::string &)),
            this,
            SLOT(createEntity(const std::string &)));
    connect(sceneEditWidget,
            SIGNAL(setEntityName(EntityHandle, const std::string &)),
            this,
            SLOT(setEntityName(EntityHandle, const std::string &)));
    connect(sceneEditWidget,
            SIGNAL(createComponent(EntityHandle, std::type_index)),
            this,
            SLOT(createComponent(EntityHandle, std::type_index)));
    connect(sceneEditWidget,
            SIGNAL(updateComponent(EntityHandle, const std::any &, std::type_index)),
            this,
            SLOT(updateComponent(EntityHandle, const std::any &, std::type_index)));
    connect(sceneEditWidget,
            SIGNAL(destroyComponent(EntityHandle, std::type_index)),
            this,
            SLOT(destroyComponent(EntityHandle, std::type_index)));
}

MainWindow::~MainWindow() {
    saveStateFile();
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    QWidget::keyPressEvent(event);
}

void MainWindow::keyReleaseEvent(QKeyEvent *event) {
    QWidget::keyReleaseEvent(event);
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
    QWidget::mousePressEvent(event);
}

void MainWindow::onTimeout() {

}

void MainWindow::createEntity(const std::string &name) {
    if (scene->entityNameExists(name)) {
        QMessageBox::warning(this, "Cannot Create Entity", ("Entity with name " + name + " already exists").c_str());
    }
    scene->createEntity(name);
}

void MainWindow::setEntityName(Entity entity, const std::string &name) {
    if (scene->entityNameExists(name)) {
        QMessageBox::warning(this, "Cannot Set Entity Name", ("Entity with name " + name + " already exists").c_str());
    }
    scene->setEntityName(entity.getHandle(), name);
}

void MainWindow::createComponent(Entity entity, std::type_index componentType) {
    if (componentType == typeid(TransformComponent)) {
        if (scene->check<TransformComponent>(entity.getHandle())) {
            QMessageBox::warning(this, "Cannot Create Component", ("TransformComponent already exists on " + entity.toString()).c_str());
        } else {
            scene->createComponent(entity, TransformComponent());
        }
    } else if (componentType == typeid(CanvasTransformComponent)) {
        scene->createComponent(entity, CanvasTransformComponent());
    }
}

void MainWindow::updateComponent(Entity entity, const std::any &value, std::type_index type) {
    if (type == typeid(TransformComponent)) {
        scene->updateComponent(entity, std::any_cast<TransformComponent>(value));
    } else if (type == typeid(CanvasTransformComponent)) {
        scene->updateComponent(entity, std::any_cast<CanvasTransformComponent>(value));
    }
}

void MainWindow::destroyComponent(Entity entity, std::type_index type) {
    if (type == typeid(TransformComponent)) {
        scene->destroyComponent<TransformComponent>(entity);
    } else if (type == typeid(CanvasTransformComponent)) {
        scene->destroyComponent<CanvasTransformComponent>(entity);
    }
}

void MainWindow::loadStateFile() {
    try {
        std::ifstream fs("state.json");
        if (fs.good()) {
            JsonProtocol jsonProtocol;
            auto msg = jsonProtocol.deserialize(fs);

            std::string dec;
            dec = msg.at("leftSplitter").asString();
            leftSplitter->restoreState(QByteArray::fromBase64(QByteArray::fromStdString(dec)));
            dec = msg.at("rightSplitter").asString();
            rightSplitter->restoreState(QByteArray::fromBase64(QByteArray::fromStdString(dec)));
            dec = msg.at("middleSplitter").asString();
            middleSplitter->restoreState(QByteArray::fromBase64(QByteArray::fromStdString(dec)));
            dec = msg.at("sceneEditSplitter").asString();
            sceneEditWidget->restoreSplitterState(QByteArray::fromBase64(QByteArray::fromStdString(dec)));
        }
    } catch (const std::exception &e) {}
}

void MainWindow::saveStateFile() {
    Message msg(xng::Message::DICTIONARY);
    msg["leftSplitter"] = leftSplitter->saveState().toBase64().toStdString();
    msg["rightSplitter"] = rightSplitter->saveState().toBase64().toStdString();
    msg["middleSplitter"] = middleSplitter->saveState().toBase64().toStdString();
    msg["sceneEditSplitter"] = sceneEditWidget->saveSplitterState().toBase64().toStdString();

    std::ofstream fs("state.json");
    JsonProtocol jsonProtocol;
    jsonProtocol.serialize(fs, msg);
}

void MainWindow::loadSceneFile(const std::string &path) {

}

void MainWindow::saveSceneFile(const std::string &path) {

}

