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

#include "gui/mainwindow.hpp"

#include <QVBoxLayout>
#include <QKeyEvent>
#include <QMenuBar>
#include <QSizePolicy>

#include <fstream>

#include "xengine.hpp"

#include "gui/widgets/entitywidget.hpp"

using namespace xng;

MainWindow::MainWindow() {
    menuBar()->addMenu("File");

    rootWidget = new QWidget(this);

    setCentralWidget(rootWidget);

    rootLayout = new QHBoxLayout();

    sceneEditWidget = new EntitySceneWidget(this);
    fileBrowser = new FileBrowser(this);

    middleSplitter = new QSplitter(this);
    leftSplitter = new QSplitter(this);
    rightSplitter = new QSplitter(this);

    tabWidget = new QTabWidget(this);

    leftSplitter->setOrientation(Qt::Vertical);
    rightSplitter->setOrientation(Qt::Vertical);

    leftSplitter->addWidget(tabWidget);

    rightSplitter->addWidget(sceneEditWidget);
    rightSplitter->addWidget(fileBrowser);

    middleSplitter->addWidget(leftSplitter);
    middleSplitter->addWidget(rightSplitter);

    rootLayout->addWidget(middleSplitter);

    rootWidget->setLayout(rootLayout);

    connect(&timer, SIGNAL(timeout()), this, SLOT(onTimeout()));

    timer.start(1000 / 60);

    loadStateFile();
}

MainWindow::~MainWindow() {
    saveStateFile();
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    QWidget::keyPressEvent(event);
}

void MainWindow::keyReleaseEvent(QKeyEvent *event) {
    QWidget::keyReleaseEvent(event);
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
    QWidget::mousePressEvent(event);
}

void MainWindow::onTimeout() {

}

void MainWindow::loadStateFile() {
    try {
        std::ifstream fs("state.json");
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
    } catch (const std::exception &e) {}
}

void MainWindow::saveStateFile() {
    Message msg(xng::Message::DICTIONARY);
    msg["leftSplitter"] = leftSplitter->saveState().toBase64().toStdString();
    msg["rightSplitter"] = rightSplitter->saveState().toBase64().toStdString();
    msg["middleSplitter"] = middleSplitter->saveState().toBase64().toStdString();
    msg["sceneEditSplitter"] = sceneEditWidget->saveSplitterState().toBase64().toStdString();

    std::ofstream fs("state.json");
    JsonProtocol jsonProtocol;
    jsonProtocol.serialize(fs, msg);
}

void MainWindow::loadSceneFile(const std::string &path) {

}

void MainWindow::saveSceneFile(const std::string &path) {

}

