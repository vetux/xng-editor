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
#ifndef XEDITOR_BUILDDIALOG_HPP
#define XEDITOR_BUILDDIALOG_HPP

#include <QDialog>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>
#include <QMessageBox>
#include <QApplication>

#include "widgets/buildsettingswidget.hpp"

#include "project/project.hpp"

//TODO: Display cmake output / error
class BuildDialog : public QDialog {
Q_OBJECT
public:
    explicit BuildDialog(QWidget *parent = nullptr) : QDialog(parent) {
        listWidget = new QListWidget();
        settingsWidget = new BuildSettingsWidget();
        buildPluginButton = new QPushButton();
        buildGameButton = new QPushButton();
        addSettingsButton = new QPushButton();
        initializeButton = new QPushButton();

        addSettingsButton->setText("Add Build Configuration");
        initializeButton->setText("Initialize CMake Build");
        buildPluginButton->setText("Build Plugin");
        buildGameButton->setText("Build Game");

        settingsWidget->setEnabled(false);

        auto *vlayout = new QVBoxLayout;
        auto *hlayout = new QHBoxLayout;

        auto *leftLayout = new QVBoxLayout;
        leftLayout->addWidget(addSettingsButton);
        leftLayout->addWidget(listWidget);

        hlayout->addWidget(buildPluginButton);
        hlayout->addWidget(buildGameButton);

        vlayout->addWidget(settingsWidget);
        vlayout->addWidget(initializeButton);
        vlayout->addLayout(hlayout);

        auto *widget = new QWidget();
        widget->setLayout(vlayout);

        auto *lWidget = new QWidget();
        lWidget->setLayout(leftLayout);

        auto *splitter = new QSplitter();
        splitter->setOrientation(Qt::Horizontal);
        splitter->addWidget(lWidget);
        splitter->addWidget(widget);

        setLayout(new QHBoxLayout);
        layout()->addWidget(splitter);

        connect(listWidget, SIGNAL(currentRowChanged(int)), this, SLOT(listWidgetRowChanged(int)));
        connect(settingsWidget,
                SIGNAL(valueChanged(const BuildSettings &)),
                this,
                SLOT(settingsChanged(const BuildSettings &)));
        connect(settingsWidget, SIGNAL(deleteClicked()), this, SLOT(settingsDeleteClicked()));
        connect(buildPluginButton, SIGNAL(clicked(bool)), this, SLOT(buildPluginClicked()));
        connect(buildGameButton, SIGNAL(clicked(bool)), this, SLOT(buildGameClicked()));
        connect(addSettingsButton, SIGNAL(clicked(bool)), this, SLOT(settingsAddClicked()));
        connect(initializeButton, SIGNAL(clicked(bool)), this, SLOT(initializeClicked()));
    }

    void setProject(const Project &value) {
        auto *focus = dynamic_cast<QWidget *>(QApplication::focusObject());
        auto &buildSettings = value.getSettings().buildSettings;
        auto selected = listWidget->currentRow();
        listWidget->clear();
        for (auto &s: buildSettings) {
            listWidget->addItem(s.name.c_str());
        }
        listWidget->setCurrentRow(selected);
        project = value;
        if (focus)
            focus->setFocus();
    }

    const Project &getProject() {
        return project;
    }

signals:

    void pluginChanged(const std::filesystem::path &pluginFile);

    void projectChanged(const Project &project);

private slots:

    void listWidgetRowChanged(int row) {
        currentSettingsIndex = row;
        settingsWidget->setEnabled(currentSettingsIndex >= 0);
        if (currentSettingsIndex >= 0) {
            settingsWidget->setSettings(getCurrentSettings());
        }
    }

    void initializeClicked() {
        std::string output;
        std::string error;
        if (currentSettingsIndex < 0) {
            QMessageBox::warning(this, "No configuration selected", "No build configuration selected");
            return;
        }
        getCurrentSettings().initialize(project.getProjectDirectory(), output, error);
        if (!error.empty()) {
            QMessageBox::warning(this, "Failed to initialize build", error.c_str());
        } else {
            QMessageBox::information(this, "Build initialization success", "Successfully initialized the build");
        }
    }

    void buildPluginClicked() {
        std::string output;
        std::string error;
        if (currentSettingsIndex < 0) {
            QMessageBox::warning(this, "No configuration selected", "No build configuration selected");
            return;
        }
        getCurrentSettings().buildPlugin(project.getProjectDirectory(), output, error);
        if (!error.empty()) {
            QMessageBox::warning(this, "Failed to build plugin", error.c_str());
        } else {
            QMessageBox::information(this, "Build successful", "Successfully built the plugin");
            copyPluginFile(getCurrentSettings(), project.getProjectDirectory());
            emit pluginChanged(project.getPluginLibraryFilePath());
        }
    }

    void buildGameClicked() {
        std::string output;
        std::string error;
        if (currentSettingsIndex < 0) {
            QMessageBox::warning(this, "No configuration selected", "No build configuration selected");
            return;
        }
        getCurrentSettings().buildTarget(project.getProjectDirectory(), output, error);
        if (!error.empty()) {
            QMessageBox::warning(this, "Failed to build game", error.c_str());
        } else {
            QMessageBox::information(this, "Build successful", "Successfully built the game");
        }
    }

    void settingsChanged(const BuildSettings &settings) {
        if (currentSettingsIndex < 0) {
            QMessageBox::warning(this, "No configuration selected", "No build configuration selected");
            return;
        }
        getCurrentSettings() = settings;
        emit projectChanged(project);
    }

    void settingsAddClicked() {
        BuildSettings conf;
        conf.name = "New Build Configuration";
        project.getSettings().buildSettings.emplace_back(conf);
        emit projectChanged(project);
    }

    void settingsDeleteClicked() {
        auto buildSettings = project.getSettings().buildSettings.at(currentSettingsIndex);
        if (QMessageBox::question(this,
                                  "Delete Build Configuration",
                                  ("Do you want to delete " + buildSettings.name + " ?").c_str())
            == QMessageBox::Yes) {
            auto &vec = project.getSettings().buildSettings;
            vec.erase(vec.begin() + currentSettingsIndex);
            emit projectChanged(project);
        }
    }

private:
    void copyPluginFile(const BuildSettings &settings, const std::filesystem::path &projectDir) {
        std::filesystem::path pluginDir = project.getPluginDirectory();
        auto pluginFileTarget = project.getPluginLibraryFilePath();
        auto pluginFile = settings.getBuiltPluginLibraryFilePath(projectDir);
        std::filesystem::remove(pluginFileTarget);
        std::filesystem::copy_file(pluginFile, pluginFileTarget);
    }

    BuildSettings &getCurrentSettings() {
        return project.getSettings().buildSettings.at(currentSettingsIndex);
    }

    QListWidget *listWidget;
    BuildSettingsWidget *settingsWidget;
    QPushButton *addSettingsButton;
    QPushButton *initializeButton;
    QPushButton *buildPluginButton;
    QPushButton *buildGameButton;

    Project project;
    int currentSettingsIndex;
};

#endif //XEDITOR_BUILDDIALOG_HPP
