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

#include <QApplication>

#include "windows/editorwindow.hpp"

int main(int argc, char *argv[]) {
    QApplication::setApplicationName("xeditor");
    QApplication::setApplicationDisplayName("xEditor");
    QApplication::setApplicationVersion("v0.0.1");

    QApplication app(argc, argv);

#ifndef XEDITOR_DEBUGGING
    try {
#endif
        EditorWindow win;
        win.show();
        return app.exec();
#ifndef XEDITOR_DEBUGGING
    } catch (const std::exception &e) {
        QMessageBox::warning(nullptr, "Uncaught Exception", e.what());
    }
#endif
}