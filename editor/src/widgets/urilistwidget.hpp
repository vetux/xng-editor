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
#ifndef XEDITOR_VECTOREDITWIDGET_HPP
#define XEDITOR_VECTOREDITWIDGET_HPP

#include <QListWidget>
#include <QListWidgetItem>
#include <QSpinBox>

#include "xng/xng.hpp"

#include "widgets/uriwidget.hpp"

class UriListWidget : public QWidget {
Q_OBJECT
public:
    explicit UriListWidget(QWidget *parent = nullptr)
            : QWidget(parent) {
        sizeWidget = new QSpinBox;
        listWidget = new QListWidget;

        auto *l = new QHBoxLayout;
        l->setMargin(0);
        l->addWidget(new QLabel("Size:"));
        l->addWidget(sizeWidget);

        auto *vl = new QVBoxLayout;
        vl->addLayout(l);
        vl->addWidget(listWidget);
        setLayout(vl);

        connect(sizeWidget,
                SIGNAL(valueChanged(int)),
                this,
                SLOT(sizeChanged(int)));
    }

    void set(const std::vector<Uri> &v) {
        uris = v;
        for (auto *ptr: uriItems) {
            listWidget->removeItemWidget(ptr);
        }
        uriWidgets.clear();
        uriItems.clear();
        for (auto &uri: uris) {
            auto *widget = new UriWidget;
            auto *item = new QListWidgetItem;
            widget->setValue(uri);
            item->setSizeHint(widget->sizeHint());
            listWidget->addItem(item);
            listWidget->setItemWidget(item, widget);
            uriIndexMapping[widget] = uriWidgets.size();
            uriWidgets.emplace_back(widget);
            uriItems.emplace_back(item);
            connect(widget, SIGNAL(valueChanged(const QString &)), this, SLOT(uriChanged(const QString &)));
        }
        sizeWidget->setValue(uris.size());
    }

signals:

    void valueChanged(const std::vector<Uri> &v);

private slots:

    void sizeChanged(int size) {
        if (uriWidgets.size() < size) {
            for (int i = uriWidgets.size(); i < size; i++) {
                auto *widget = new UriWidget;
                auto *item = new QListWidgetItem;
                item->setSizeHint(widget->sizeHint());
                listWidget->addItem(item);
                listWidget->setItemWidget(item, widget);
                uriIndexMapping[widget] = uriWidgets.size();
                uriWidgets.emplace_back(widget);
                uriItems.emplace_back(item);
                uris.emplace_back("");
                connect(widget, SIGNAL(valueChanged(const QString &)), this, SLOT(uriChanged(const QString &)));
            }
        } else {
            for (int i = size; i < uriWidgets.size(); i++) {
                listWidget->removeItemWidget(uriItems.at(i));
            }
            uriWidgets.resize(size);
            uriItems.resize(size);
            uris.resize(size);
        }
        emit valueChanged(uris);
    }

    void uriChanged(const QString &uri) {
        auto *sen = dynamic_cast<UriWidget *>(sender());
        auto index = uriIndexMapping.at(sen);
        uris.at(index) = Uri(uri.toStdString());
        uriItems.at(index)->setSizeHint(uriWidgets.at(index)->sizeHint());
        emit valueChanged(uris);
    }

protected:
    std::vector<Uri> uris;

    QSpinBox *sizeWidget;
    QListWidget *listWidget;
    std::vector<UriWidget *> uriWidgets;
    std::vector<QListWidgetItem *> uriItems;
    std::map<UriWidget *, int> uriIndexMapping;
};

#endif //XEDITOR_VECTOREDITWIDGET_HPP
