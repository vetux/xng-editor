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

#ifndef XEDITOR_FILEBROWSER_HPP
#define XEDITOR_FILEBROWSER_HPP

#include <QWidget>
#include <QListWidget>
#include <QVBoxLayout>

class FileBrowser : public QWidget {
Q_OBJECT
public:
    //TODO: ListWidget file display, Drag and Drop, Context menu
    explicit FileBrowser(QWidget *parent)
            : QWidget(parent) {
        setLayout(new QVBoxLayout());
        listWidget = new QListWidget(this);
        layout()->addWidget(listWidget);
        layout()->setMargin(0);
    }

    void setCurrentPath(const std::string &value) {
        currentPath = value;
    }

    const std::string &getCurrentPath() {
        return currentPath;
    }

private:
    std::string currentPath;
    QListWidget *listWidget;
};

#endif //XEDITOR_FILEBROWSER_HPP
