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

#ifndef XEDITOR_FILEBROWSERWIDGET_HPP
#define XEDITOR_FILEBROWSERWIDGET_HPP

#include <QWidget>
#include <QTreeView>
#include <QFileSystemModel>
#include <QFileIconProvider>
#include <QVBoxLayout>
#include <QLabel>

#include <filesystem>

class FileBrowserWidget : public QWidget {
Q_OBJECT
public:
    //TODO: ListWidget file display, Drag and Drop, Context menu
    explicit FileBrowserWidget(QWidget *parent)
            : QWidget(parent) {
        setLayout(new QVBoxLayout());
        tree = new QTreeView(this);
        layout()->addWidget(tree);
        layout()->setMargin(0);
        model.setIconProvider(&iconProvider);
        model.setRootPath("");
        model.setOption(QFileSystemModel::DontUseCustomDirectoryIcons);
        //model.setOption(QFileSystemModel::DontWatchForChanges);
        tree->setModel(&model);
    }

    void setCurrentPath(const std::filesystem::path &value) {
        currentPath = value;
        QString v = value.string().c_str();
        model.setRootPath(v);
        auto i = model.index(v);
        tree->setRootIndex(i);
    }

    std::filesystem::path getCurrentPath() {
        return model.rootPath().toStdString().c_str();
    }

signals:

    void openMaterial(const std::filesystem::path &path);

    void openShader(const std::filesystem::path &path);

    void openSkybox(const std::filesystem::path &path);

    void openTexture(const std::filesystem::path &path);

    void openSprite(const std::filesystem::path &path);

    void openSpriteAnimation(const std::filesystem::path &path);

private:
    std::filesystem::path currentPath;
    QTreeView *tree;
    QFileSystemModel model;
    QFileIconProvider iconProvider;
};

#endif //XEDITOR_FILEBROWSERWIDGET_HPP
