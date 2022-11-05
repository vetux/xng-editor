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

#ifndef XEDITOR_MAINWINDOW_HPP
#define XEDITOR_MAINWINDOW_HPP

#include <QMainWindow>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>
#include <QTimer>
#include <QTabWidget>

#include "xengine.hpp"

#include "widgets/scenerenderwidget.hpp"
#include "widgets/sceneeditwidget.hpp"
#include "widgets/entityeditwidget.hpp"
#include "widgets/filebrowser.hpp"

#include "windows/projectbuilddialog.hpp"

#include "project/project.hpp"

class MainWindow : public QMainWindow {
Q_OBJECT
public:
    struct Actions {
        QMenu *fileMenu;
        QAction *projectCreateAction;
        QAction *projectOpenAction;
        QAction *projectSaveAction;
        QAction *projectSettingsAction;
        QAction *exitAction;

        QMenu *sceneMenu;
        QAction *sceneNewAction;
        QAction *sceneOpenAction;

        QMenu *buildMenu;
        QAction *buildProjectAction;
        QAction *buildSettingsAction;

        explicit Actions(QWidget *parent = nullptr);
    };

    MainWindow();

    ~MainWindow() override;

protected slots:

    void createEntity(const std::string &name);

    void setEntityName(Entity entity, const std::string &name);

    void createComponent(Entity entity, std::type_index componentType);

    void updateComponent(Entity entity, const Component &value);

    void destroyComponent(Entity entity, std::type_index type);

    void newProject();

    void openProject();

    void saveProject();

    void openProjectSettings();

    void newScene();

    void openScene();

    void buildProject();

    void openBuildSettings();

private:
    void keyPressEvent(QKeyEvent *event) override;

    void keyReleaseEvent(QKeyEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;

    void loadStateFile();

    void saveStateFile();

    void loadSceneFile(const std::string &path);

    void saveSceneFile(const std::string &path);

    QWidget *rootWidget;
    QHBoxLayout *rootLayout;

    QSplitter *middleSplitter;
    QSplitter *leftSplitter;
    QSplitter *rightSplitter;

    SceneRenderWidget *sceneRenderWidget;
    SceneEditWidget *sceneEditWidget;
    FileBrowserWidget *fileBrowserWidget;

    QTabWidget *tabWidget;

    std::shared_ptr<xng::EntityScene> scene;

    Actions actions;
};

#endif //XEDITOR_MAINWINDOW_HPP