/**
 *  Mana - 3D Game Engine
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

#ifndef MANA_MAINWINDOW_HPP
#define MANA_MAINWINDOW_HPP

#include <QMainWindow>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>
#include <QTimer>
#include <QTabWidget>

#include "engine.hpp"

#include "gui/scene/sceneeditwidget.hpp"
#include "gui/scene/entityeditwidget.hpp"
#include "gui/tool/filebrowser.hpp"
#include "gui/dialog/projectbuilddialog.hpp"

class MainWindow : public QMainWindow {
Q_OBJECT
public:
    MainWindow();

    ~MainWindow() override;

protected slots:

    void onTimeout();

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

    engine::RenderWidgetQt *renderWidget;
    SceneEditWidget *sceneEditWidget;
    FileBrowser *fileBrowser;

    QTabWidget *tabWidget;

    engine::EntityManager entityManager;
    engine::Scene renderScene;

    std::unique_ptr<engine::Archive> archive;
    std::unique_ptr<engine::AssetManager> assetManager;
    std::unique_ptr<engine::AssetRenderManager> assetRenderManager;

    QTimer timer;
};

#endif //MANA_MAINWINDOW_HPP
