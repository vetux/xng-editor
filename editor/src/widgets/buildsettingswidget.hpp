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
#ifndef XEDITOR_BUILDSETTINGSWIDGET_HPP
#define XEDITOR_BUILDSETTINGSWIDGET_HPP

#include <QComboBox>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QStringListModel>

#include "project/buildsettings.hpp"

class BuildSettingsWidget : public QWidget {
Q_OBJECT
public:
    BuildSettingsWidget(QWidget *parent = nullptr) : QWidget(parent) {
        deleteButton = new QPushButton;
        name = new QLineEdit;
        targetPlatform = new QComboBox;
        optimization = new QComboBox;
        cmakeCommand = new QLineEdit;
        buildDir = new QLineEdit;
        gameTargetName = new QLineEdit;
        pluginTargetName = new QLineEdit;
        srcDirs = new QLineEdit;
        incDirs = new QLineEdit;
        lnkDirs = new QLineEdit;
        linkedLibraries = new QLineEdit;

        deleteButton->setText("Delete Configuration");

        platformMapping = {
                {WIN32_86, 0},
                {WIN32_64, 1},
                {LINUX_86, 2},
                {LINUX_64, 3}
        };

        optimizationMapping = {
                {NO_OPTIMIZATION,    0},
                {SAFE_OPTIMIZATIONS, 1},
                {FULL_OPTIMIZATIONS, 2}
        };

        for (auto &p: platformMapping) {
            platformMappingReverse[p.second] = p.first;
        }

        for (auto &p: optimizationMapping) {
            optimizationMappingReverse[p.second] = p.first;
        }

        targetPlatform->setModel(new QStringListModel({
                                                              "Windows 32bit",
                                                              "Windows 64bit",
                                                              "Linux 32bit",
                                                              "Linux 64bit"
                                                      }));

        optimization->setModel(new QStringListModel({
                                                            "Disable Optimizations",
                                                            "Safe Optimizations",
                                                            "All Optimizations"
                                                    }));

        auto *hLayout = new QHBoxLayout;
        hLayout->addWidget(name);
        hLayout->addWidget(deleteButton);

        auto *vLayout = new QVBoxLayout;
        vLayout->addLayout(hLayout);
        vLayout->addWidget(new QLabel("Target Platform"));
        vLayout->addWidget(targetPlatform);
        vLayout->addWidget(new QLabel("Optimization Level"));
        vLayout->addWidget(optimization);
        vLayout->addWidget(new QLabel("CMake Command"));
        vLayout->addWidget(cmakeCommand);
        vLayout->addWidget(new QLabel("Output Directory"));
        vLayout->addWidget(buildDir);
        vLayout->addWidget(new QLabel("Game CMake Target Name"));
        vLayout->addWidget(gameTargetName);
        vLayout->addWidget(new QLabel("Plugin CMake Target Name"));
        vLayout->addWidget(pluginTargetName);
        vLayout->addWidget(new QLabel("Source Directories"));
        vLayout->addWidget(srcDirs);
        vLayout->addWidget(new QLabel("Include Directories"));
        vLayout->addWidget(incDirs);
        vLayout->addWidget(new QLabel("Link Directories"));
        vLayout->addWidget(lnkDirs);
        vLayout->addWidget(new QLabel("Linked Libraries"));
        vLayout->addWidget(linkedLibraries);

        setLayout(vLayout);

        connect(deleteButton, SIGNAL(clicked(bool)), this, SIGNAL(deleteClicked()));

        connect(name, SIGNAL(textEdited(const QString &)), this, SLOT(changedValue()));
        connect(targetPlatform, SIGNAL(currentIndexChanged(int)), this, SLOT(changedValue()));
        connect(optimization, SIGNAL(currentIndexChanged(int)), this, SLOT(changedValue()));
        connect(cmakeCommand, SIGNAL(textEdited(const QString &)), this, SLOT(changedValue()));
        connect(buildDir, SIGNAL(textEdited(const QString &)), this, SLOT(changedValue()));
        connect(gameTargetName, SIGNAL(textEdited(const QString &)), this, SLOT(changedValue()));
        connect(pluginTargetName, SIGNAL(textEdited(const QString &)), this, SLOT(changedValue()));
        connect(srcDirs, SIGNAL(textEdited(const QString &)), this, SLOT(changedValue()));
        connect(incDirs, SIGNAL(textEdited(const QString &)), this, SLOT(changedValue()));
        connect(lnkDirs, SIGNAL(textEdited(const QString &)), this, SLOT(changedValue()));
        connect(linkedLibraries, SIGNAL(textEdited(const QString &)), this, SLOT(changedValue()));
    }

    void setSettings(const BuildSettings &settings) {
        blockComboBox = true;

        if (name->text().toStdString() != settings.name)
            name->setText(settings.name.c_str());
        targetPlatform->setCurrentIndex(platformMapping.at(settings.targetPlatform));
        optimization->setCurrentIndex(optimizationMapping.at(settings.optimization));
        if (cmakeCommand->text().toStdString() != settings.cmakeCommand)
            cmakeCommand->setText(settings.cmakeCommand.c_str());
        if (buildDir->text().toStdString() != settings.buildDir)
            buildDir->setText(settings.buildDir.c_str());
        if (gameTargetName->text().toStdString() != settings.gameTargetName)
            gameTargetName->setText(settings.gameTargetName.c_str());
        if (pluginTargetName->text().toStdString() != settings.pluginTargetName)
            pluginTargetName->setText(settings.pluginTargetName.c_str());
        auto str = BuildSettings::concatCmakeList(settings.sourceDirectories);
        if (srcDirs->text().toStdString() != str)
            srcDirs->setText(str.c_str());
        str = BuildSettings::concatCmakeList(settings.includeDirectories);
        if (incDirs->text().toStdString() != str)
            incDirs->setText(str.c_str());
        str = BuildSettings::concatCmakeList(settings.linkDirectories);
        if (lnkDirs->text().toStdString() != str)
            lnkDirs->setText(str.c_str());
        str = BuildSettings::concatCmakeList(settings.linkedLibraries);
        if (linkedLibraries->text().toStdString() != str)
            linkedLibraries->setText(str.c_str());

        blockComboBox = false;
    }

    BuildSettings getSettings() const {
        BuildSettings ret;
        ret.name = name->text().toStdString().c_str();
        ret.targetPlatform = platformMappingReverse.at(targetPlatform->currentIndex());
        ret.optimization = optimizationMappingReverse.at(optimization->currentIndex());
        ret.cmakeCommand = cmakeCommand->text().toStdString().c_str();
        ret.buildDir = buildDir->text().toStdString().c_str();
        ret.gameTargetName = gameTargetName->text().toStdString().c_str();
        ret.pluginTargetName = pluginTargetName->text().toStdString().c_str();
        ret.sourceDirectories = BuildSettings::splitCmakeList(srcDirs->text().toStdString());
        ret.includeDirectories = BuildSettings::splitCmakeList(incDirs->text().toStdString());
        ret.linkDirectories = BuildSettings::splitCmakeList(lnkDirs->text().toStdString());
        ret.linkedLibraries = BuildSettings::splitCmakeList(linkedLibraries->text().toStdString());
        return ret;
    }

signals:

    void valueChanged(const BuildSettings &value);

    void deleteClicked();

private slots:

    void changedValue() {
        if (!blockComboBox) {
            emit valueChanged(getSettings());
        }
    }

private:
    QPushButton *deleteButton;
    QLineEdit *name;
    QComboBox *targetPlatform;
    QComboBox *optimization;
    QLineEdit *cmakeCommand;
    QLineEdit *buildDir;
    QLineEdit *gameTargetName;
    QLineEdit *pluginTargetName;
    QLineEdit *srcDirs;
    QLineEdit *incDirs;
    QLineEdit *lnkDirs;
    QLineEdit *linkedLibraries;

    std::map<BuildPlatform, int> platformMapping;
    std::map<int, BuildPlatform> platformMappingReverse;

    std::map<BuildOptimization, int> optimizationMapping;
    std::map<int, BuildOptimization> optimizationMappingReverse;

    bool blockComboBox = false;
};

#endif //XEDITOR_BUILDSETTINGSWIDGET_HPP
