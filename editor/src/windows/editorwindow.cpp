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

#include "windows/editorwindow.hpp"

#include <QVBoxLayout>
#include <QKeyEvent>
#include <QMenuBar>
#include <QSizePolicy>
#include <QMessageBox>
#include <QFileDialog>
#include <QApplication>
#include <QStatusBar>

#include <fstream>

#include "widgets/entityeditwidget.hpp"
#include "windows/builddialog.hpp"

#include "io/paths.hpp"

using namespace xng;

EditorWindow::Actions::Actions(QWidget *parent) {
    settingsAction = new QAction("Settings...", parent);
    exitAction = new QAction("Exit", parent);
    projectCreateAction = new QAction("New Project...", parent);
    projectOpenAction = new QAction("Open Project...", parent);
    projectOpenRecentMenu = new QMenu("Open Recent Project...", parent);
    projectCloseAction = new QAction("Close Project", parent);
    projectSaveAction = new QAction("Save All", parent);
    projectSettingsAction = new QAction("Project Settings...", parent);

    fileMenu = new QMenu("File", parent);
    fileMenu->addAction(projectCreateAction);
    fileMenu->addAction(projectOpenAction);
    fileMenu->addMenu(projectOpenRecentMenu);
    fileMenu->addAction(projectCloseAction);
    fileMenu->addSeparator();
    fileMenu->addAction(settingsAction);
    fileMenu->addSeparator();
    fileMenu->addAction(projectSaveAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    buildProjectAction = new QAction("Build Project...", parent);

    buildMenu = new QMenu("Build", parent);
    buildMenu->addAction(buildProjectAction);

    sceneNewAction = new QAction("New Scene...", parent);
    sceneOpenAction = new QAction("Open Scene...", parent);
    sceneSaveAsAction = new QAction("Save Scene As...", parent);
    sceneSaveAction = new QAction("Save Scene", parent);
    sceneCloseAction = new QAction("Close Scene", parent);

    sceneMenu = new QMenu("Scene", parent);
    sceneMenu->addAction(sceneNewAction);
    sceneMenu->addAction(sceneOpenAction);
    sceneMenu->addAction(sceneSaveAsAction);
    sceneMenu->addAction(sceneSaveAction);
    sceneMenu->addAction(sceneCloseAction);

    projectSaveAction->setShortcut(QKeySequence::Save);
    buildProjectAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_B));
}

EditorWindow::EditorWindow() : QMainWindow(),
                               actions(this) {
    buildDialog = new BuildDialog(this);
    buildDialog->setProject(project);

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

    rootLayout->setContentsMargins(6, 6, 6, 0);
    rootWidget->setLayout(rootLayout);

    loadStateFile();
    loadRecentProjects();

    sceneEditWidget->setScene(scene);

    actions.buildProjectAction->setEnabled(false);

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
            SIGNAL(createComponent(const Entity &, const std::string &)),
            this,
            SLOT(createComponent(const Entity &, const std::string &)));
    connect(sceneEditWidget,
            SIGNAL(updateComponent(const Entity &, const Component &)),
            this,
            SLOT(updateComponent(const Entity &, const Component &)));
    connect(sceneEditWidget,
            SIGNAL(updateComponent(const Entity &, const GenericComponent &)),
            this,
            SLOT(updateComponent(const Entity &, const GenericComponent &)));
    connect(sceneEditWidget,
            SIGNAL(destroyComponent(const Entity &, std::type_index)),
            this,
            SLOT(destroyComponent(const Entity &, std::type_index)));
    connect(sceneEditWidget,
            SIGNAL(destroyComponent(const Entity &, const std::string&)),
            this,
            SLOT(destroyComponent(const Entity &, const std::string&)));

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

    connect(fileBrowserWidget,
            SIGNAL(openPath(const std::filesystem::path &)),
            this,
            SLOT(openPath(const std::filesystem::path &)));
    connect(fileBrowserWidget,
            SIGNAL(deletePath(const std::filesystem::path &)),
            this,
            SLOT(deletePath(const std::filesystem::path &)));
    connect(fileBrowserWidget,
            SIGNAL(createPath(const std::filesystem::path &)),
            this,
            SLOT(createPath(const std::filesystem::path &)));

    connect(buildDialog,
            SIGNAL(projectChanged(const Project &)),
            this,
            SLOT(buildDialogChanged(const Project &)));
    connect(buildDialog,
            SIGNAL(pluginChanged(const std::filesystem::path &)),
            this,
            SLOT(loadPlugin(const std::filesystem::path &)));

    menuBar()->addMenu(actions.fileMenu);
    menuBar()->addMenu(actions.buildMenu);
    menuBar()->addMenu(actions.sceneMenu);

    updateActions();
    std::vector<std::unique_ptr<ResourceParser>> parsers;
    parsers.emplace_back(std::unique_ptr<ResourceParser>(new StbiParser));
    parsers.emplace_back(std::unique_ptr<ResourceParser>(new JsonParser));
    parsers.emplace_back(std::unique_ptr<ResourceParser>(ResourceParser::load(xng::ASSIMP).release()));
    parsers.emplace_back(std::unique_ptr<ResourceParser>(ResourceParser::load(xng::LIBSNDFILE).release()));
    ResourceRegistry::getDefaultRegistry().setImporter(ResourceImporter(std::move(parsers)));

    statusBar()->show();

    connect(QGuiApplication::instance(),
            SIGNAL(applicationStateChanged(Qt::ApplicationState)),
            this,
            SLOT(applicationStateChanged(Qt::ApplicationState)));
}

EditorWindow::~EditorWindow() {
    // Wait for scene render widget shutdown and unset scene because there might be components in the current scene which's destructors are defined in the loaded plugin library and will be called after the library is unloaded.
    sceneRenderWidget->shutdown();
    scene = std::make_shared<EntityScene>();
    sceneRenderWidget->setScene(*scene);
    sceneEditWidget->setScene(scene);
    unloadPlugin();
}

void EditorWindow::keyPressEvent(QKeyEvent *event) {
    auto &r = ResourceRegistry::getDefaultRegistry();
    QWidget::keyPressEvent(event);
}

void EditorWindow::keyReleaseEvent(QKeyEvent *event) {
    QWidget::keyReleaseEvent(event);
}

void EditorWindow::mousePressEvent(QMouseEvent *event) {
    QWidget::mousePressEvent(event);
}

void EditorWindow::createEntity() {
    auto selectedEntity = sceneEditWidget->getSelectedEntity();
    auto ent = scene->createEntity();
    TransformComponent comp;
    if (selectedEntity) {
        if (selectedEntity.hasName()) {
            comp.parent = selectedEntity.getName();
        }
    }
    ent.createComponent<>(comp);
}

void EditorWindow::createEntity(const std::string &name) {
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
    setSceneSaved(false);
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

void EditorWindow::destroyEntity(const Entity &entity) {
    if (QMessageBox::question(this, "Destroy Entity",
                              entity.hasName()
                              ? "Do you want to destroy " + QString(entity.getName().c_str()) + " ?"
                              : "Do you want to destroy the entity: " + QString(entity.getHandle().toString().c_str()) +
                                " ?")
        != QMessageBox::Yes) {
        return;
    }
    if (entity.hasName()) {
        auto vec = destroyRecursive(entity.getHandle(), entity.getName(), *scene);
        for (auto &v: vec) {
            scene->destroy(v);
        }
    } else {
        scene->destroy(entity.getHandle());
    }
    setSceneSaved(false);
}

void EditorWindow::setEntityName(const Entity &entity, const std::string &name) {
    if (scene->entityNameExists(name)) {
        QMessageBox::warning(this, "Cannot Set Entity Name", ("Entity with name " + name + " already exists").c_str());
    } else {
        scene->setEntityName(entity.getHandle(), name);
        setSceneSaved(false);
    }
}

void EditorWindow::createComponent(const Entity &entity, std::type_index componentType) {
    if (scene->checkComponent(entity.getHandle(), componentType)) {
        QMessageBox::warning(this, "Cannot Create Component",
                             (std::string(componentType.name()) + " already exists on " + entity.toString()).c_str());
    } else {
        scene->createComponent(entity.getHandle(), componentType);
    }
}

void EditorWindow::createComponent(Entity entity, const std::string &typeName) {
    if (!entity.checkComponent<GenericComponent>()) {
        entity.createComponent<GenericComponent>();
    }

    auto comp = entity.getComponent<GenericComponent>();
    comp.components[typeName] = Message();
    entity.updateComponent(comp);

    sceneEditWidget->setScene(scene);
}

void EditorWindow::updateComponent(const Entity &entity, const Component &value) {
    scene->updateComponent(entity.getHandle(), value);
}

void EditorWindow::updateComponent(const Entity &entity, const GenericComponent &value) {
    scene->updateComponent(entity.getHandle(), value);
}

void EditorWindow::destroyComponent(const Entity &entity, std::type_index type) {
    if (QMessageBox::question(this, "Destroy Component",
                              ("Do you want to destroy " + ComponentRegistry::instance().getNameFromType(type)).c_str())
        == QMessageBox::Yes) {
        scene->destroyComponent(entity.getHandle(), type);
    }
}

void EditorWindow::destroyComponent(Entity entity, const std::string &typeName) {
    if (QMessageBox::question(this, "Destroy Component",
                              ("Do you want to destroy " + typeName).c_str())
        == QMessageBox::Yes) {
        auto comp = entity.getComponent<GenericComponent>();
        comp.components.erase(typeName);
        entity.updateComponent(comp);
        sceneEditWidget->setScene(scene);
    }
}

void EditorWindow::openSettings() {

}

void EditorWindow::newProject() {
    if (!checkUnsavedSceneChanges()) {
        QMessageBox::information(this, "Aborted", "The operation was cancelled, no project created.");
        return;
    }

    QFileDialog dialog;
    dialog.setWindowTitle("Select output directory for new project...");
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setFileMode(QFileDialog::Directory);
    while (dialog.exec() == QFileDialog::Accepted) {
        auto dir = dialog.selectedFiles().at(0);
        auto path = std::filesystem::path(dir.toStdString());
        if (!std::filesystem::is_empty(path)) {
            QMessageBox::warning(this, "Invalid Directory", ("Directory "
                                                             + QString(path.string().c_str())
                                                             +
                                                             " has existing contents, please select a writeable empty directory."));
        } else {
            Project::createNewProject(std::filesystem::path(dir.toStdString()));
            path.append(Paths::projectSettingsFilename().toStdString().c_str());
            loadProject(path);
            break;
        }
    }
}

void EditorWindow::openProject() {
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

void EditorWindow::openRecentProject() {
    auto *ptr = sender();
    loadProject(recentProjectActions.at(dynamic_cast<QAction *>(ptr)));
}

void EditorWindow::saveProject() {
    if (!scenePath.empty())
        saveScene();
    project.save();
    setProjectSaved(true);
}

void EditorWindow::openProjectSettings() {

}

void EditorWindow::newScene() {
    if (!checkUnsavedSceneChanges()) {
        QMessageBox::information(this, "Aborted", "The operation was cancelled.");
        return;
    }

    scenePath = "";
    scene->clear();

    setSceneSaved(true);

    sceneRenderWidget->setScene(*scene);
}

void EditorWindow::openScene() {
    if (!checkUnsavedSceneChanges()) {
        QMessageBox::information(this, "Aborted", "The operation was cancelled.");
        return;
    }

    QFileDialog dialog;
    dialog.setWindowTitle("Select scene file to open...");
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setDirectory(project.getProjectDirectory().string().c_str());
    dialog.setMimeTypeFilters({"application/json"});

    if (dialog.exec() == QFileDialog::Accepted) {
        auto &file = dialog.selectedFiles().at(0);
        loadScene(file.toStdString());
    }
}

bool EditorWindow::saveScene() {
    if (scenePath.empty()) {
        QFileDialog dialog;
        dialog.setWindowTitle("Select scene output file...");
        dialog.setAcceptMode(QFileDialog::AcceptSave);
        dialog.setFileMode(QFileDialog::AnyFile);
        dialog.setDirectory(project.getProjectDirectory().string().c_str());
        dialog.setMimeTypeFilters({"application/json"});

        if (dialog.exec() == QFileDialog::Accepted) {
            auto &file = dialog.selectedFiles().at(0);
            scenePath = std::filesystem::path(file.toStdString());
        } else {
            return false;
        }
    }
    if (!scenePath.has_extension()) {
        scenePath = scenePath.parent_path().append(scenePath.filename().string() + JsonParser::EXTENSION_SCENE);
    }
    Message msg;
    *scene >> msg;
    auto p = JsonProtocol();
    std::ofstream fs(scenePath.string());
    p.serialize(fs, msg);
    setSceneSaved(true);
    return true;
}

void EditorWindow::saveSceneAs() {
    QFileDialog dialog;
    dialog.setWindowTitle("Select scene output file...");
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setDirectory(project.getProjectDirectory().string().c_str());
    dialog.setMimeTypeFilters({"application/json"});

    if (dialog.exec() == QFileDialog::Accepted) {
        auto &file = dialog.selectedFiles().at(0);
        scenePath = std::filesystem::path(file.toStdString());
        saveScene();
    }
}

void EditorWindow::buildProject() {
    buildDialog->show();
}

void EditorWindow::shutdown() {
    checkUnsavedSceneChanges();
    saveStateFile();
    close();
}

void EditorWindow::loadStateFile() {
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

void EditorWindow::saveStateFile() {
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

void EditorWindow::loadScene(const std::filesystem::path &path) {
#ifndef XEDITOR_DEBUGGING
    try {
#endif
    statusBar()->showMessage("Opening scene at " + QString(path.string().c_str()));
    QApplication::processEvents();
    auto prot = JsonProtocol();
    std::ifstream fs(path.string());
    scene->clear();
    *scene << prot.deserialize(fs);
    scenePath = path;
    setSceneSaved(true);
    sceneRenderWidget->setScene(*scene);
    statusBar()->showMessage("Opened \"" + QString(scene->getName().c_str()) + "\"");
#ifndef XEDITOR_DEBUGGING
    } catch (const std::exception &e) {
        QMessageBox::warning(this,
                             "Scene load failed",
                             ("Failed to load scene at " + QString(path.string().c_str()) + " Error: " + e.what()));
    }
#endif
}

void EditorWindow::loadProject(const std::filesystem::path &path) {
    if (!checkUnsavedSceneChanges()) {
        QMessageBox::information(this, "Aborted", "The operation was cancelled.");
        return;
    }
    scene->clear();
    setSceneSaved(true);
    try {
        project.load(path.parent_path());
        setWindowTitle(QString(project.getSettings().name.c_str()) + " - " + QString(path.string().c_str()));
        buildDialog->setProject(project);
        setProjectSaved(true);
        addRecentProject(path.string());
        fileBrowserWidget->setCurrentPath(path.parent_path());
        auto pluginFile = project.getPluginLibraryFilePath();
        if (std::filesystem::exists(pluginFile)) {
            loadPlugin(pluginFile);
        }
        actions.buildProjectAction->setEnabled(true);
        scanComponentHeaders();
        statusBar()->showMessage(("Opened project at " + path.string()).c_str());
    } catch (const std::exception &e) {
        QMessageBox::warning(this,
                             "Project load failed",
                             ("Failed to load project at " + QString(path.string().c_str()) + " Error: " +
                              e.what()));
    }
}

bool EditorWindow::checkUnsavedSceneChanges() {
    if (!sceneSaved) {
        if (QMessageBox::question(this,
                                  "Unsaved Scene Changes",
                                  "Your scene contains unsaved changes that are going to be discarded, do you want to save them now?")
            == QMessageBox::Yes) {
            return saveScene();
        }
    }
    return true;
}

void EditorWindow::addRecentProject(const std::string &path) {
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

void EditorWindow::loadRecentProjects() {
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

void EditorWindow::saveRecentProjects() {
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

void EditorWindow::setProjectSaved(bool saved) {
    projectSaved = saved;
    updateActions();
    updateTitle();
}

void EditorWindow::setSceneSaved(bool saved) {
    sceneSaved = saved;
    updateActions();
    updateTitle();
}

void EditorWindow::updateTitle() {
    if (project.isLoaded()) {
        if (projectSaved && sceneSaved)
            setWindowTitle(QString(project.getSettings().name.c_str())
                           + " - "
                           + QString(project.getProjectDirectory().string().c_str()));
        else
            setWindowTitle(QString(project.getSettings().name.c_str())
                           + "* - "
                           + QString(project.getProjectDirectory().string().c_str()));
    } else {
        if (projectSaved && sceneSaved)
            setWindowTitle(QApplication::applicationDisplayName());
        else
            setWindowTitle(QApplication::applicationDisplayName() + "*");
    }
}

void EditorWindow::updateActions() {
    actions.projectSaveAction->setEnabled(project.isLoaded() && (!sceneSaved || !projectSaved));
    actions.sceneSaveAction->setEnabled(!sceneSaved && !scenePath.empty());
}

void EditorWindow::scanComponentHeaders() {
    if (project.isLoaded()) {
        for (auto &dir: project.getSourceDirectories()) {
            auto scanCount = 0;
            auto compCount = 0;
            try {
                for (auto &dirEntry: std::filesystem::recursive_directory_iterator(dir)) {
                    if (dirEntry.is_directory())
                        continue;
                    scanCount++;
                    statusBar()->showMessage(("Scanning " + dirEntry.path().string() + " for components...").c_str());
                    xng::Tokenizer tokenizer;
                    xng::HeaderParser parser;
                    xng::HeaderGenerator generator;
                    try {
                        std::fstream stream;
                        stream.exceptions(std::fstream::badbit);
                        stream.open(dirEntry.path());
                        auto tokens = tokenizer.tokenize(stream);
                        auto metadataList = parser.parseTokens(
                                dirEntry.path().filename().string(),
                                tokens);
                        if (!metadataList.empty()) {
                            for (auto &metadata: metadataList) {
                                availableMetadata[metadata.typeName] = metadata;
                                auto generatedHeader = generator.generateHeader(metadata);
                                if (!generatedHeader.empty()) {
                                    auto generatedPath = dirEntry.path().parent_path()
                                            .append(Paths::generatedHeaderFileName(
                                                    dirEntry.path().filename()).string());
                                    std::fstream ofs;
                                    ofs.exceptions(std::fstream::badbit);
                                    ofs.open(generatedPath, std::fstream::out);
                                    ofs << generatedHeader.c_str();
                                    ofs.flush();
                                    ofs.close();
                                }
                                compCount++;
                            }
                            sceneEditWidget->setAvailableComponentMetadata(availableMetadata);
                        }
                    } catch (const std::exception &e) {
                        QMessageBox::warning(this, "Failed to scan file", ("Failed to scan "
                                                                           + dirEntry.path().string()
                                                                           + " Error:\n"
                                                                           + e.what()).c_str());
                    }
                }
                statusBar()->showMessage(("Scanned "
                                          + std::to_string(scanCount)
                                          + " files for components, Found "
                                          + std::to_string(compCount)
                                          + " Components").c_str());
            } catch (const std::exception &e) {}
        }
    }
}

void EditorWindow::closeEvent(QCloseEvent *event) {
    shutdown();
}

void EditorWindow::openPath(const std::filesystem::path &path) {
    if (path.filename().string().c_str() == Paths::projectSettingsFilename()) {
        // Open project
        loadProject(path);
    } else if (path.extension().string() == JsonParser::EXTENSION_BUNDLE) {
        // Open / Edit resource bundle
        std::ifstream fs(path);
        auto str = std::string(std::istream_iterator<char>(fs),
                               std::istream_iterator<char>());
        std::vector<char> vec;
        vec.insert(vec.begin(), str.begin(), str.end());
        auto bundle = JsonParser().read(vec, path.extension(), nullptr);
    } else if (path.extension().string() == JsonParser::EXTENSION_SCENE) {
        if (QMessageBox::question(this, "Open Scene",
                                  "Do you want to open the scene at: " + QString(path.string().c_str()) + " ?")
            == QMessageBox::Yes) {
            loadScene(path);
        }
    } else {
        QMessageBox::information(this, "Unrecognized file",
                                 "Could not determine how to open the file at " + QString(path.string().c_str()));
    }
}

void EditorWindow::deletePath(const std::filesystem::path &path) {
    if (QMessageBox::question(this,
                              "Delete Path",
                              "Do you want to move the file/files at " + QString(path.string().c_str()) +
                              " to the trash?")
        == QMessageBox::Yes) {
        // Move files/files to trash
    }
}

void EditorWindow::createPath(const std::filesystem::path &parentPath) {
    // Create resource / scene dialog
}

void EditorWindow::loadPlugin(const std::filesystem::path &pluginFile) {
    unloadPlugin();
    if (QMessageBox::question(this,
                              "Load Plugin",
                              "Do you want to load the plugin library at " + QString(pluginFile.string().c_str()) + "?")
        == QMessageBox::Yes) {
        pluginLibrary = Library::load(pluginFile.string());
        void (*loadFunc)() = pluginLibrary->getSymbol<void()>("load");
        if (loadFunc) {
            loadFunc();
        } else {
            QMessageBox::warning(this,
                                 "Function not found",
                                 "load() function was not found in the plugin library.");
        }
    }
}

void EditorWindow::unloadPlugin() {
    if (!pluginLibrary)
        return;
    void (*unloadFunc)() = pluginLibrary->getSymbol<void()>("unload");
    if (unloadFunc) {
        unloadFunc();
    } else {
        QMessageBox::warning(this,
                             "Function not found",
                             "unload() function was not found in the plugin library.");
    }
}

void EditorWindow::buildDialogChanged(const Project &value) {
    setProjectSaved(false);
    project = value;
}

void EditorWindow::applicationStateChanged(Qt::ApplicationState state) {
    switch (state) {
        case Qt::ApplicationSuspended:
            break;
        case Qt::ApplicationHidden:
            break;
        case Qt::ApplicationInactive:
            break;
        case Qt::ApplicationActive:
            scanComponentHeaders();
            break;
    }
}

void EditorWindow::onEntityCreate(const EntityHandle &entity) {
    setSceneSaved(false);
    sceneRenderWidget->setScene(*scene);
}

void EditorWindow::onEntityDestroy(const EntityHandle &entity) {
    setSceneSaved(false);
    sceneRenderWidget->setScene(*scene);
}

void EditorWindow::onEntityNameChanged(const EntityHandle &entity,
                                       const std::string &newName,
                                       const std::string &oldName) {
    setSceneSaved(false);
    sceneRenderWidget->setScene(*scene);
}

void EditorWindow::onComponentCreate(const EntityHandle &entity,
                                     const Component &component) {
    setSceneSaved(false);
    sceneRenderWidget->setScene(*scene);
}

void EditorWindow::onComponentDestroy(const EntityHandle &entity, const Component &component) {
    setSceneSaved(false);
    sceneRenderWidget->setScene(*scene);
}

void EditorWindow::onComponentUpdate(const EntityHandle &entity,
                                     const Component &oldComponent,
                                     const Component &newComponent) {
    setSceneSaved(false);
    sceneRenderWidget->setScene(*scene);
}
