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

#include "gui/widgets/scenerenderwidget.hpp"
#include "gui/widgets/sceneeditwidget.hpp"
#include "gui/widgets/entityeditwidget.hpp"
#include "gui/tool/filebrowser.hpp"
#include "gui/dialog/projectbuilddialog.hpp"

#include "project/project.hpp"

class MainWindow : public QMainWindow {
Q_OBJECT
public:
    MainWindow();

    ~MainWindow() override;

protected slots:

    void onTimeout();

    void createEntity(const std::string &name);

    void setEntityName(Entity entity, const std::string &name);

    void createComponent(Entity entity, std::type_index componentType);

    void updateComponent(Entity entity, const Component &value);

    void destroyComponent(Entity entity, std::type_index type);

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

    QTimer timer;
};

#endif //XEDITOR_MAINWINDOW_HPP
