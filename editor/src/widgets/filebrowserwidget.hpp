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

#ifndef XEDITOR_FILEBROWSERWIDGET_HPP
#define XEDITOR_FILEBROWSERWIDGET_HPP

#include <QWidget>
#include <QTreeView>
#include <QFileSystemModel>
#include <QFileIconProvider>
#include <QVBoxLayout>
#include <QLabel>
#include <QAction>

#include <filesystem>

class FileBrowserWidget : public QWidget {
Q_OBJECT
public:
    //TODO: Drag and Drop, Context menu
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
        tree->setContextMenuPolicy(Qt::ActionsContextMenu);
        openAction = new QAction("Open...");
        editAction = new QAction("Edit...");
        deleteAction = new QAction("Delete...");
        createAction = new QAction("Create New File...");
        tree->setSelectionMode(QAbstractItemView::SingleSelection);
        connect(openAction,
                SIGNAL(triggered(bool)),
                this,
                SLOT(onOpenPath()));
        connect(editAction,
                SIGNAL(triggered(bool)),
                this,
                SLOT(onEditPath()));
        connect(deleteAction,
                SIGNAL(triggered(bool)),
                this,
                SLOT(onDeletePath()));
        connect(createAction,
                SIGNAL(triggered(bool)),
                this,
                SLOT(onCreatePath()));
        tree->addAction(openAction);
        tree->addAction(editAction);
        tree->addAction(deleteAction);
        tree->addAction(createAction);

        tree->setColumnWidth(0, 200);

        connect(tree,
                SIGNAL(doubleClicked(const QModelIndex &)),
                this,
                SLOT(treeDoubleClicked(const QModelIndex &)));
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

    std::filesystem::path getSelectedPath() {
        auto indices = tree->selectionModel()->selectedIndexes();
        if (indices.empty()) {
            return "";
        } else {
            return model.filePath(indices.at(0)).toStdString().c_str();
        }
    }

signals:

    void openPath(const std::filesystem::path &path);

    void editPath(const std::filesystem::path &path);

    void deletePath(const std::filesystem::path &path);

    void createPath(const std::filesystem::path &parentPath);

private slots:

    void onOpenPath() {
        emit openPath(getSelectedPath());
    }

    void onEditPath() {
        emit editPath(getSelectedPath());
    }

    void onDeletePath() {
        emit deletePath(getSelectedPath());
    }

    void onCreatePath() {
        emit createPath(getSelectedPath());
    }

    void treeDoubleClicked(const QModelIndex &index) {
        std::filesystem::path p(model.filePath(index).toStdString().c_str());
        if (is_regular_file(p)) {
            emit openPath(p);
        }
    }

private:
    std::filesystem::path currentPath;
    QTreeView *tree;
    QFileSystemModel model;
    QFileIconProvider iconProvider;
    QAction *openAction;
    QAction *editAction;
    QAction *deleteAction;
    QAction *createAction;
};

#endif //XEDITOR_FILEBROWSERWIDGET_HPP
