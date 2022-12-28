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

#ifndef XEDITOR_EDITORWINDOW_HPP
#define XEDITOR_EDITORWINDOW_HPP

#include <QMainWindow>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>
#include <QTimer>
#include <QTabWidget>

#include "xng/xng.hpp"

#include "widgets/scenerenderwidget.hpp"
#include "widgets/sceneeditwidget.hpp"
#include "widgets/entityeditwidget.hpp"
#include "widgets/filebrowserwidget.hpp"

#include "windows/builddialog.hpp"

#include "project/project.hpp"

class EditorWindow : public QMainWindow, EntityScene::Listener {
Q_OBJECT
public:
    struct Actions {
        QMenu *fileMenu;
        QAction *buildProjectAction;
        QAction *settingsAction;
        QAction *projectCreateAction;
        QAction *projectOpenAction;
        QMenu *projectOpenRecentMenu;
        QAction *projectSaveAction;
        QAction *projectSettingsAction;
        QAction *exitAction;

        QMenu *sceneMenu;
        QAction *sceneNewAction;
        QAction *sceneOpenAction;
        QAction *sceneSaveAsAction;
        QAction *sceneSaveAction;

        explicit Actions(QWidget *parent = nullptr);
    };

    EditorWindow();

    ~EditorWindow() override;

protected slots:

    void createEntity();

    void createEntity(const std::string &name);

    void destroyEntity(const Entity &entity);

    void setEntityName(const Entity &entity, const std::string &name);

    void createComponent(const Entity &entity, std::type_index componentType);

    void updateComponent(const Entity &entity, const Component &value);

    void updateComponent(const Entity &entity, const GenericComponent &value);

    void destroyComponent(const Entity &entity, std::type_index type);

    void openSettings();

    void newProject();

    void openProject();

    void openRecentProject();

    void saveProject();

    void openProjectSettings();

    void newScene();

    void openScene();

    bool saveScene();

    void saveSceneAs();

    void buildProject();

    void shutdown();

    void closeEvent(QCloseEvent *event) override;

    void openPath(const std::filesystem::path &path);

    void deletePath(const std::filesystem::path &path);

    void createPath(const std::filesystem::path &parentPath);

    void loadPlugin(const std::filesystem::path &pluginFile);

    void unloadPlugin();

    void projectChanged(const Project &project);

private:
    void onEntityCreate(const EntityHandle &entity) override;

    void onEntityDestroy(const EntityHandle &entity) override;

    void onEntityNameChanged(const EntityHandle &entity,
                             const std::string &newName,
                             const std::string &oldName) override;

    void onComponentCreate(const EntityHandle &entity, const Component &component) override;

    void onComponentDestroy(const EntityHandle &entity, const Component &component) override;

    void onComponentUpdate(const EntityHandle &entity, const Component &oldComponent,
                           const Component &newComponent) override;

    void keyPressEvent(QKeyEvent *event) override;

    void keyReleaseEvent(QKeyEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;

    void loadStateFile();

    void saveStateFile();

    void loadScene(const std::filesystem::path &path);

    void loadProject(const std::filesystem::path &path);

    bool checkUnsavedSceneChanges();

    void addRecentProject(const std::string &path);

    void loadRecentProjects();

    void saveRecentProjects();

    void updateActions();

    void scanComponentHeaders();

    QWidget *rootWidget;
    QHBoxLayout *rootLayout;

    QSplitter *middleSplitter;
    QSplitter *leftSplitter;
    QSplitter *rightSplitter;

    SceneRenderWidget *sceneRenderWidget;
    SceneEditWidget *sceneEditWidget;
    FileBrowserWidget *fileBrowserWidget;

    QTabWidget *tabWidget;

    std::filesystem::path scenePath;

    std::shared_ptr<xng::EntityScene> scene;

    bool sceneSaved = true;
    bool projectSaved = true;

    Actions actions;

    Project project;

    std::vector<std::string> recentProjects;
    std::map<QAction *, std::string> recentProjectActions;
    std::map<std::string, QAction *> recentProjectActionsReverse;

    ResourceImporter importer;

    std::unique_ptr<Library> pluginLibrary;

    BuildDialog *buildDialog;
};

#endif //XEDITOR_EDITORWINDOW_HPP
