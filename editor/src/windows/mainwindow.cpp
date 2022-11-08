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
    sceneSaveAsAction = new QAction("Save As...", parent);
    sceneSaveAction = new QAction("Save", parent);

    sceneMenu->addAction(sceneNewAction);
    sceneMenu->addAction(sceneOpenAction);
    sceneMenu->addAction(sceneSaveAsAction);
    sceneMenu->addAction(sceneSaveAction);

    buildMenu = new QMenu("Build", parent);
    buildProjectAction = new QAction("Build Project...", parent);
    buildSettingsAction = new QAction("Open Build Settings...", parent);

    buildMenu->addAction(buildProjectAction);
    buildMenu->addAction(buildSettingsAction);

    projectSaveAction->setShortcut(QKeySequence::Save);
    sceneSaveAction->setShortcut(QKeySequence::Save);
}

MainWindow::MainWindow() : QMainWindow(),
                           actions(this),
                           sceneMutex(std::make_unique<std::mutex>()) {
    scene = std::make_shared<EntityScene>();

    scene->addListener(*this);

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
    loadRecentProjects();

    sceneEditWidget->setScene(scene);
    sceneRenderWidget->setScene(scene, sceneMutex);

    connect(sceneEditWidget,
            SIGNAL(createEntity()),
            this,
            SLOT(createEntity()));
    connect(sceneEditWidget,
            SIGNAL(createEntity(const std::string &)),
            this,
            SLOT(createEntity(const std::string &)));
    connect(sceneEditWidget,
            SIGNAL(destroyEntity(const Entity &)),
            this,
            SLOT(destroyEntity(const Entity &)));
    connect(sceneEditWidget,
            SIGNAL(setEntityName(const Entity &, const std::string &)),
            this,
            SLOT(setEntityName(const Entity &, const std::string &)));
    connect(sceneEditWidget,
            SIGNAL(createComponent(const Entity &, std::type_index)),
            this,
            SLOT(createComponent(const Entity &, std::type_index)));
    connect(sceneEditWidget,
            SIGNAL(updateComponent(const Entity &, const Component &)),
            this,
            SLOT(updateComponent(const Entity &, const Component &)));
    connect(sceneEditWidget,
            SIGNAL(destroyComponent(const Entity &, std::type_index)),
            this,
            SLOT(destroyComponent(const Entity &, std::type_index)));

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
    connect(actions.sceneSaveAction,
            SIGNAL(triggered(bool)),
            this,
            SLOT(saveScene()));
    connect(actions.sceneSaveAsAction,
            SIGNAL(triggered(bool)),
            this,
            SLOT(saveSceneAs()));


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

MainWindow::~MainWindow() {}

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

void MainWindow::createEntity() {
    std::lock_guard<std::mutex> guard(*sceneMutex);
    auto selectedEntity = sceneEditWidget->getSelectedEntity();
    auto ent = scene->createEntity();
    TransformComponent comp;
    if (selectedEntity) {
        if (selectedEntity.hasName()) {
            comp.parent = selectedEntity.getName();
        }
    }
    ent.createComponent<>(comp);
    sceneSaved = false;
    updateActions();
}

void MainWindow::createEntity(const std::string &name) {
    std::lock_guard<std::mutex> guard(*sceneMutex);
    auto selectedEntity = sceneEditWidget->getSelectedEntity();
    if (scene->entityNameExists(name)) {
        QMessageBox::warning(this, "Cannot Create Entity", ("Entity with name " + name + " already exists").c_str());
    }
    auto ent = scene->createEntity(name);
    TransformComponent comp;
    if (selectedEntity) {
        if (selectedEntity.hasName()) {
            comp.parent = selectedEntity.getName();
        }
    }
    ent.createComponent<>(comp);
    sceneSaved = false;
    updateActions();
}

static std::vector<EntityHandle>
destroyRecursive(const EntityHandle &ent, const std::string &name, EntityScene &scene) {
    std::vector<EntityHandle> ret;
    for (auto &pair: scene.getPool<TransformComponent>()) {
        if (pair.first == ent)
            continue;
        if (pair.second.parent == name) {
            if (scene.entityHasName(pair.first)) {
                auto v = destroyRecursive(pair.first, scene.getEntityName(pair.first), scene);
                ret.insert(ret.end(), v.begin(), v.end());
            } else {
                ret.emplace_back(pair.first);
            }
        }
    }
    ret.emplace_back(ent);
    return ret;
}

void MainWindow::destroyEntity(const Entity &entity) {
    if (QMessageBox::question(this, "Destroy Entity",
                              entity.hasName()
                              ? "Do you want to destroy " + QString(entity.getName().c_str()) + " ?"
                              : "Do you want to destroy the entity: " + QString(entity.getHandle().toString().c_str()) +
                                " ?")
        != QMessageBox::Yes) {
        return;
    }
    std::lock_guard<std::mutex> guard(*sceneMutex);
    if (entity.hasName()) {
        auto vec = destroyRecursive(entity.getHandle(), entity.getName(), *scene);
        for (auto &v: vec) {
            scene->destroy(v);
        }
    } else {
        scene->destroy(entity.getHandle());
    }
    sceneSaved = false;
    updateActions();
}

void MainWindow::setEntityName(const Entity &entity, const std::string &name) {
    std::lock_guard<std::mutex> guard(*sceneMutex);
    if (scene->entityNameExists(name)) {
        QMessageBox::warning(this, "Cannot Set Entity Name", ("Entity with name " + name + " already exists").c_str());
    } else {
        scene->setEntityName(entity.getHandle(), name);
        sceneSaved = false;
        updateActions();
    }
}

void MainWindow::createComponent(const Entity &entity, std::type_index componentType) {
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

void MainWindow::updateComponent(const Entity &entity, const Component &value) {
    std::lock_guard<std::mutex> guard(*sceneMutex);
    scene->updateComponent(entity.getHandle(), value);
    sceneSaved = false;
}

void MainWindow::destroyComponent(const Entity &entity, std::type_index type) {
    std::lock_guard<std::mutex> guard(*sceneMutex);
    scene->destroyComponent(entity.getHandle(), type);
    sceneSaved = false;
    updateActions();
}

void MainWindow::openSettings() {

}

void MainWindow::newProject() {
    if (!checkUnsavedSceneChanges()) {
        QMessageBox::information(this, "Aborted", "The operation was cancelled, no project created.");
        return;
    }

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
        path.append(Paths::projectSettingsFilename().toStdString().c_str());
        loadProject(path);
    }
}

void MainWindow::openProject() {
    if(!checkUnsavedSceneChanges()){
        QMessageBox::information(this, "Aborted", "The operation was cancelled, no project opened.");
        return;
    }

    QFileDialog dialog;
    dialog.setWindowTitle("Select project to open...");
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setNameFilter(Paths::projectSettingsFilename());
    if (dialog.exec() == QFileDialog::Accepted) {
        auto &file = dialog.selectedFiles().at(0);
        auto path = std::filesystem::path(file.toStdString().c_str());
        loadProject(path);
    }
}

void MainWindow::openRecentProject() {
    if (!checkUnsavedSceneChanges()){
        QMessageBox::information(this, "Aborted", "The operation was cancelled, no project opened.");
        return;
    }
    auto *ptr = sender();
    loadProject(recentProjectActions.at(dynamic_cast<QAction *>(ptr)));
}

void MainWindow::saveProject() {
    project.save();
    projectSaved = true;
    updateActions();
}

void MainWindow::openProjectSettings() {

}

void MainWindow::newScene() {
    if (!checkUnsavedSceneChanges()){
        QMessageBox::information(this, "Aborted", "The operation was cancelled, no scene created.");
        return;
    }

    scenePath = "";
    scene->clear();

    sceneSaved = true;
}

void MainWindow::openScene() {
    if (!checkUnsavedSceneChanges()){
        QMessageBox::information(this, "Aborted", "The operation was cancelled, no scene opened.");
        return;
    }

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

bool MainWindow::saveScene() {
    if (!sceneSaved) {
        if (scenePath.empty()) {
            QFileDialog dialog;
            dialog.setWindowTitle("Select scene output file...");
            dialog.setAcceptMode(QFileDialog::AcceptOpen);
            dialog.setFileMode(QFileDialog::AnyFile);
            dialog.setDirectory(project.getDirectory().c_str());
            dialog.setMimeTypeFilters({"application/json"});

            if (dialog.exec() == QFileDialog::Accepted) {
                auto &file = dialog.selectedFiles().at(0);
                scenePath = std::filesystem::path(file.toStdString());
            } else {
                return false;
            }
        }
        Message msg;
        *scene >> msg;
        auto p = JsonProtocol();
        std::ofstream fs(scenePath.string());
        p.serialize(fs, msg);
        sceneSaved = true;
        updateActions();
    }
    return true;
}

void MainWindow::saveSceneAs() {
    QFileDialog dialog;
    dialog.setWindowTitle("Select scene output file...");
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setDirectory(project.getDirectory().c_str());
    dialog.setMimeTypeFilters({"application/json"});

    if (dialog.exec() == QFileDialog::Accepted) {
        auto &file = dialog.selectedFiles().at(0);
        sceneSaved = false;
        scenePath = std::filesystem::path(file.toStdString());
        saveScene();
    }
}

void MainWindow::buildProject() {

}

void MainWindow::openBuildSettings() {

}

void MainWindow::shutdown() {
    checkUnsavedSceneChanges();
    saveStateFile();
    close();
}

void MainWindow::loadStateFile() {
    auto path = Paths::stateFilePath();
    if (std::filesystem::exists(path)) {
        try {
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
        } catch (const std::exception &e) {
            QMessageBox::warning(this,
                                 "Failed to load state",
                                 QString(e.what()));
        }
    }
}

void MainWindow::saveStateFile() {
    Message msg(xng::Message::DICTIONARY);
    msg["leftSplitter"] = leftSplitter->saveState().toBase64().toStdString();
    msg["rightSplitter"] = rightSplitter->saveState().toBase64().toStdString();
    msg["middleSplitter"] = middleSplitter->saveState().toBase64().toStdString();
    msg["sceneEditSplitter"] = sceneEditWidget->saveSplitterState().toBase64().toStdString();

    try {
        std::ofstream fs(Paths::stateFilePath().string());
        JsonProtocol jsonProtocol;
        jsonProtocol.serialize(fs, msg);
    } catch (const std::exception &e) {
        QMessageBox::warning(this,
                             "Failed to save state",
                             QString(e.what()));
    }
}

void MainWindow::loadScene(const std::filesystem::path &path) {
    try {
        std::lock_guard<std::mutex> guard(*sceneMutex);
        auto prot = JsonProtocol();
        std::ifstream fs(path.string());
        *scene << prot.deserialize(fs);
        sceneSaved = true;
    } catch (const std::exception &e) {
        QMessageBox::warning(this,
                             "Scene load failed",
                             ("Failed to load scene at " + QString(path.string().c_str()) + " Error: " + e.what()));
    }
}

void MainWindow::loadProject(const std::filesystem::path &path) {
    scene->clear();
    try {
        project.load(path.parent_path());
        setWindowTitle(project.getSettings().name.c_str());
        QMessageBox::information(this, "Project opened",
                                 ("Successfully opened " + project.getSettings().name).c_str());
        projectSaved = true;
        updateActions();
        addRecentProject(path.string());
    } catch (const std::exception &e) {
        QMessageBox::warning(this,
                             "Project load failed",
                             ("Failed to load project at " + QString(path.string().c_str()) + " Error: " +
                              e.what()));
    }
}

bool MainWindow::checkUnsavedSceneChanges() {
    if (!sceneSaved) {
        if (QMessageBox::question(this,
                                  "Unsaved Scene Changes",
                                  "Your scene contains unsaved changes that are going to be overwritten, do you want to save them now?")
            == QMessageBox::Yes) {
            return saveScene();
        }
    }
    return true;
}

void MainWindow::addRecentProject(const std::string &path) {
    auto it = std::find(recentProjects.begin(), recentProjects.end(), path);
    if (it != recentProjects.end()) {
        auto *action = recentProjectActionsReverse.at(path);
        disconnect(action);
        actions.projectOpenRecentMenu->removeAction(action);
        recentProjects.erase(it);
        recentProjectActionsReverse.erase(path);
    }
    recentProjects.insert(recentProjects.begin(), path);
    auto *action = new QAction(path.c_str(), this);
    actions.projectOpenRecentMenu->insertAction(actions.projectOpenRecentMenu->actionAt({0, 0}), action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(openRecentProject()));
    recentProjectActions[action] = path;
    recentProjectActionsReverse[path] = action;
    saveRecentProjects();
}

void MainWindow::loadRecentProjects() {
    for (auto &pair: recentProjectActions) {
        disconnect(pair.first);
    }
    recentProjectActions.clear();
    recentProjects.clear();
    auto path = Paths::recentProjectsPath();
    if (std::filesystem::exists(path)) {
        auto prot = xng::JsonProtocol();
        try {
            std::ifstream fs(path.string());
            auto msg = prot.deserialize(fs);
            for (auto &v: msg.asList()) {
                auto p = std::filesystem::path(v.asString());
                if (std::filesystem::exists(p)) {
                    recentProjects.emplace_back(v.asString());
                }
            }
        } catch (const std::exception &e) {
            QMessageBox::warning(this,
                                 "Failed to load recent projects",
                                 QString(e.what()));
        }
    }
    for (auto &p: recentProjects) {
        auto *action = new QAction(p.c_str(), this);
        actions.projectOpenRecentMenu->addAction(action);
        connect(action, SIGNAL(triggered(bool)), this, SLOT(openRecentProject()));
        recentProjectActions[action] = p;
        recentProjectActionsReverse[p] = action;
    }
}

void MainWindow::saveRecentProjects() {
    std::vector<Message> vec;
    for (auto &path: recentProjects) {
        Message m;
        m = path;
        vec.emplace_back(m);
    }

    Message msg = Message(vec);

    auto path = Paths::recentProjectsPath();

    auto prot = xng::JsonProtocol();
    try {
        std::ofstream fs(path.string());
        prot.serialize(fs, msg);
    } catch (const std::exception &e) {
        QMessageBox::warning(this,
                             "Failed to save recent projects",
                             QString(e.what()));
    }
}

void MainWindow::updateActions() {
    actions.projectSaveAction->setEnabled(project.isLoaded() && (!sceneSaved || !projectSaved));
    actions.sceneSaveAction->setEnabled(!sceneSaved && !scenePath.empty());
}

void MainWindow::closeEvent(QCloseEvent *event) {
    shutdown();
}

void MainWindow::onEntityCreate(const EntityHandle &entity) {
    sceneSaved = false;
}

void MainWindow::onEntityDestroy(const EntityHandle &entity) {
    sceneSaved = false;
}

void
MainWindow::onEntityNameChanged(const EntityHandle &entity, const std::string &newName, const std::string &oldName) {
    sceneSaved = false;
}

void MainWindow::onComponentCreate(const EntityHandle &entity, const Component &component) {
    sceneSaved = false;
}

void MainWindow::onComponentDestroy(const EntityHandle &entity, const Component &component) {
    sceneSaved = false;
}

void MainWindow::onComponentUpdate(const EntityHandle &entity, const Component &oldComponent,
                                   const Component &newComponent) {
    sceneSaved = false;
}
