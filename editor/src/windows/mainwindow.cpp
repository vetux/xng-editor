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

#include "windows/mainwindow.hpp"

#include <QVBoxLayout>
#include <QKeyEvent>
#include <QMenuBar>
#include <QSizePolicy>
#include <QMessageBox>
#include <QFileDialog>

#include <fstream>

#include "widgets/entityeditwidget.hpp"

#include "io/paths.hpp"

using namespace xng;

MainWindow::Actions::Actions(QWidget *parent) {
    fileMenu = new QMenu("File", parent);
    settingsAction = new QAction("Settings...", parent);
    projectCreateAction = new QAction("New Project...", parent);
    projectOpenAction = new QAction("Open Project...", parent);
    projectOpenRecentMenu = new QMenu("Open Recent Project...", parent);
    projectSaveAction = new QAction("Save Project", parent);
    projectSettingsAction = new QAction("Project Settings...", parent);
    exitAction = new QAction("Exit", parent);

    fileMenu->addAction(projectCreateAction);
    fileMenu->addAction(projectOpenAction);
    fileMenu->addMenu(projectOpenRecentMenu);
    fileMenu->addAction(projectSaveAction);
    fileMenu->addAction(projectSettingsAction);
    fileMenu->addSeparator();
    fileMenu->addAction(settingsAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    sceneMenu = new QMenu("Scene", parent);
    sceneNewAction = new QAction("New...", parent);
    sceneOpenAction = new QAction("Open...", parent);

    sceneMenu->addAction(sceneNewAction);
    sceneMenu->addAction(sceneOpenAction);

    buildMenu = new QMenu("Build", parent);
    buildProjectAction = new QAction("Build Project...", parent);
    buildSettingsAction = new QAction("Open Build Settings...", parent);

    buildMenu->addAction(buildProjectAction);
    buildMenu->addAction(buildSettingsAction);
}

MainWindow::MainWindow() : QMainWindow(),
                           actions(this),
                           sceneMutex(std::make_unique<std::mutex>()) {
    scene = std::make_shared<EntityScene>();

    rootWidget = new QWidget(this);

    setCentralWidget(rootWidget);

    rootLayout = new QHBoxLayout();

    sceneRenderWidget = new SceneRenderWidget(this);
    sceneEditWidget = new SceneEditWidget(this);
    fileBrowserWidget = new FileBrowserWidget(this);

    middleSplitter = new QSplitter(this);
    leftSplitter = new QSplitter(this);
    rightSplitter = new QSplitter(this);

    tabWidget = new QTabWidget(this);

    tabWidget->addTab(sceneRenderWidget, "Viewport");

    leftSplitter->setOrientation(Qt::Vertical);
    rightSplitter->setOrientation(Qt::Vertical);

    leftSplitter->addWidget(tabWidget);

    rightSplitter->addWidget(sceneEditWidget);
    rightSplitter->addWidget(fileBrowserWidget);

    middleSplitter->addWidget(leftSplitter);
    middleSplitter->addWidget(rightSplitter);

    rootLayout->addWidget(middleSplitter);

    rootWidget->setLayout(rootLayout);

    loadStateFile();

    sceneEditWidget->setScene(scene);
    sceneRenderWidget->setScene(scene, sceneMutex);

    connect(sceneEditWidget,
            SIGNAL(createEntity(const std::string &)),
            this,
            SLOT(createEntity(const std::string &)));
    connect(sceneEditWidget,
            SIGNAL(setEntityName(Entity, const std::string &)),
            this,
            SLOT(setEntityName(Entity, const std::string &)));
    connect(sceneEditWidget,
            SIGNAL(createComponent(Entity, std::type_index)),
            this,
            SLOT(createComponent(Entity, std::type_index)));
    connect(sceneEditWidget,
            SIGNAL(updateComponent(Entity, const Component &)),
            this,
            SLOT(updateComponent(Entity, const Component &)));
    connect(sceneEditWidget,
            SIGNAL(destroyComponent(Entity, std::type_index)),
            this,
            SLOT(destroyComponent(Entity, std::type_index)));

    connect(actions.settingsAction,
            SIGNAL(triggered(bool)),
            this,
            SLOT(openSettings()));
    connect(actions.projectCreateAction,
            SIGNAL(triggered(bool)),
            this,
            SLOT(newProject()));
    connect(actions.projectOpenAction,
            SIGNAL(triggered(bool)),
            this,
            SLOT(openProject()));
    connect(actions.projectSaveAction,
            SIGNAL(triggered(bool)),
            this,
            SLOT(saveProject()));
    connect(actions.projectSettingsAction,
            SIGNAL(triggered(bool)),
            this,
            SLOT(openProjectSettings()));
    connect(actions.exitAction,
            SIGNAL(triggered(bool)),
            this,
            SLOT(shutdown()));

    connect(actions.sceneNewAction,
            SIGNAL(triggered(bool)),
            this,
            SLOT(newScene()));
    connect(actions.sceneOpenAction,
            SIGNAL(triggered(bool)),
            this,
            SLOT(openScene()));

    connect(actions.buildProjectAction,
            SIGNAL(triggered(bool)),
            this,
            SLOT(buildProject()));
    connect(actions.buildSettingsAction,
            SIGNAL(triggered(bool)),
            this,
            SLOT(openBuildSettings()));

    menuBar()->addMenu(actions.fileMenu);
    menuBar()->addMenu(actions.buildMenu);
    menuBar()->addMenu(actions.sceneMenu);

    updateActions();
}

MainWindow::~MainWindow() {
    saveStateFile();
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    auto &r = ResourceRegistry::getDefaultRegistry();
    QWidget::keyPressEvent(event);
}

void MainWindow::keyReleaseEvent(QKeyEvent *event) {
    QWidget::keyReleaseEvent(event);
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
    QWidget::mousePressEvent(event);
}

void MainWindow::createEntity(const std::string &name) {
    std::lock_guard<std::mutex> guard(*sceneMutex);
    if (scene->entityNameExists(name)) {
        QMessageBox::warning(this, "Cannot Create Entity", ("Entity with name " + name + " already exists").c_str());
    }
    scene->createEntity(name);
    sceneSaved = false;
    updateActions();
}

void MainWindow::setEntityName(Entity entity, const std::string &name) {
    std::lock_guard<std::mutex> guard(*sceneMutex);
    if (scene->entityNameExists(name)) {
        QMessageBox::warning(this, "Cannot Set Entity Name", ("Entity with name " + name + " already exists").c_str());
    }
    scene->setEntityName(entity.getHandle(), name);
    sceneSaved = false;
    updateActions();
}

void MainWindow::createComponent(Entity entity, std::type_index componentType) {
    std::lock_guard<std::mutex> guard(*sceneMutex);
    if (scene->checkComponent(entity.getHandle(), componentType)) {
        QMessageBox::warning(this, "Cannot Create Component",
                             (std::string(componentType.name()) + " already exists on " + entity.toString()).c_str());
    } else {
        scene->createComponent(entity.getHandle(), componentType);
        sceneSaved = false;
        updateActions();
    }
}

void MainWindow::updateComponent(Entity entity, const Component &value) {
    std::lock_guard<std::mutex> guard(*sceneMutex);
    scene->updateComponent(entity.getHandle(), value);
    sceneSaved = false;
}

void MainWindow::destroyComponent(Entity entity, std::type_index type) {
    std::lock_guard<std::mutex> guard(*sceneMutex);
    scene->destroyComponent(entity.getHandle(), type);
    sceneSaved = false;
    updateActions();
}

void MainWindow::openSettings() {

}

void MainWindow::newProject() {
    checkUnsavedSceneChanges();

    QFileDialog dialog;
    dialog.setWindowTitle("Select output directory for new project...");
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setFileMode(QFileDialog::Directory);
    if (dialog.exec() == QFileDialog::Accepted) {
        auto dir = dialog.selectedFiles().at(0);
        auto path = std::filesystem::path(dir.toStdString());
        if (!std::filesystem::is_empty(path)) {
            if (QMessageBox::question(this, "Overwrite Existing files", ("Directory " + path.string() +
                                                                         " is not empty, do you wish to overwrite the existing contents?").c_str())
                != QMessageBox::Yes) {
                QMessageBox::information(this, "Cancelled", "The project creation was cancelled.");
                return;
            } else {
                for (const auto &entry: std::filesystem::directory_iterator(path))
                    std::filesystem::remove_all(entry.path());
            }
        }
        Project::create(std::filesystem::path(dir.toStdString()),
                        std::filesystem::path(Paths::projectTemplatePath().string()));
        path.append(Paths::projectSettingsFilename().toStdString());
        loadProject(path);
    }
}

void MainWindow::openProject() {
    checkUnsavedSceneChanges();

    QFileDialog dialog;
    dialog.setWindowTitle("Select project to open...");
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setNameFilter(Paths::projectSettingsFilename());
    if (dialog.exec() == QFileDialog::Accepted) {
        auto &file = dialog.selectedFiles().at(0);
        auto path = std::filesystem::path(file.toStdString());
        loadProject(path);
    }
}

void MainWindow::saveProject() {
    saveScene();
    project.save();
    projectSaved = true;
    updateActions();
}

void MainWindow::openProjectSettings() {

}

void MainWindow::newScene() {
    checkUnsavedSceneChanges();

    QFileDialog dialog;
    dialog.setWindowTitle("Select scene output file...");
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setDirectory(project.getDirectory().c_str());
    dialog.setMimeTypeFilters({"application/json"});

    if (dialog.exec() == QFileDialog::Accepted) {
        auto &file = dialog.selectedFiles().at(0);
        scenePath = std::filesystem::path(file.toStdString());
        scene = std::make_shared<EntityScene>();
        sceneEditWidget->setScene(scene);
        sceneRenderWidget->setScene(scene, sceneMutex);
    }
}

void MainWindow::openScene() {
    checkUnsavedSceneChanges();

    QFileDialog dialog;
    dialog.setWindowTitle("Select scene file...");
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setDirectory(project.getDirectory().c_str());
    dialog.setMimeTypeFilters({"application/json"});

    if (dialog.exec() == QFileDialog::Accepted) {
        auto &file = dialog.selectedFiles().at(0);
        loadScene(file.toStdString());
    }
}

void MainWindow::buildProject() {

}

void MainWindow::openBuildSettings() {

}

void MainWindow::shutdown() {
    checkUnsavedSceneChanges();
    close();
}

void MainWindow::loadStateFile() {
    auto path = Paths::stateFilePath();
    if (std::filesystem::exists(path)) {
        std::ifstream fs(path.string());
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
    }
}

void MainWindow::saveStateFile() {
    Message msg(xng::Message::DICTIONARY);
    msg["leftSplitter"] = leftSplitter->saveState().toBase64().toStdString();
    msg["rightSplitter"] = rightSplitter->saveState().toBase64().toStdString();
    msg["middleSplitter"] = middleSplitter->saveState().toBase64().toStdString();
    msg["sceneEditSplitter"] = sceneEditWidget->saveSplitterState().toBase64().toStdString();

    std::ofstream fs(Paths::stateFilePath().string());
    JsonProtocol jsonProtocol;
    jsonProtocol.serialize(fs, msg);
}

void MainWindow::loadScene(const std::filesystem::path &path) {
    try {
        std::lock_guard<std::mutex> guard(*sceneMutex);
        auto prot = JsonProtocol();
        std::ifstream fs(path.string());
        *scene << prot.deserialize(fs);
    } catch (const std::exception &e) {
        QMessageBox::warning(this,
                             "Scene load failed",
                             ("Failed to load scene at " + QString(path.string().c_str()) + " Error: " + e.what()));
    }
}

void MainWindow::saveScene() {
    if (!sceneSaved
        && !scenePath.empty()) {
        Message msg;
        *scene >> msg;
        auto p = JsonProtocol();
        std::ofstream fs(scenePath.string());
        p.serialize(fs, msg);
        sceneSaved = true;
        updateActions();
    }
}

void MainWindow::loadProject(const std::filesystem::path &path) {
    scene = std::make_shared<EntityScene>();
    sceneEditWidget->setScene(scene);
    sceneRenderWidget->setScene(scene, sceneMutex);
    try {
        project.load(path.parent_path());
        setWindowTitle(project.getSettings().name.c_str());
        QMessageBox::information(this, "Project opened", ("Successfully opened " + project.getSettings().name).c_str());
        projectSaved = true;
        updateActions();
    } catch (const std::exception &e) {
        QMessageBox::warning(this,
                             "Project load failed",
                             ("Failed to load project at " + QString(path.string().c_str()) + " Error: " + e.what()));
    }
}

void MainWindow::checkUnsavedSceneChanges() {
    if (!sceneSaved) {
        if (QMessageBox::question(this,
                                  "Unsaved Scene Changes",
                                  "Your scene has unsaved changes, do you want to save the scene?")
            == QMessageBox::Yes) {
            if (scenePath.empty()) {
                QFileDialog dialog;
                dialog.setAcceptMode(QFileDialog::AcceptSave);
                dialog.setFileMode(QFileDialog::AnyFile);
                dialog.setMimeTypeFilters({"application/json"});
                if (dialog.exec() == QFileDialog::Accepted) {
                    auto file = dialog.selectedFiles().at(0);
                    scenePath = file.toStdString();
                } else {
                    return;
                }
            }
            saveScene();
        }
    }
}

void MainWindow::loadRecentProjects() {

}

void MainWindow::updateActions() {
    actions.projectSaveAction->setEnabled(project.isLoaded() && (!sceneSaved || !projectSaved));
}
