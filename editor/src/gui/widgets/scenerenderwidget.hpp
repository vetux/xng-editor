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
#ifndef XNG_EDITOR_SCENERENDERWIDGET_HPP
#define XNG_EDITOR_SCENERENDERWIDGET_HPP

#include <QWidget>
#include <QTimer>
#include <QLabel>
#include <QVBoxLayout>
#include <QResizeEvent>
#include <QScrollArea>
#include <QScrollBar>

#include "offscreenrenderer.hpp"

class SceneRenderWidget : public QWidget {
Q_OBJECT
public:
    explicit SceneRenderWidget(QWidget *parent = nullptr) : QWidget(parent),
                                                            ren(30, getSize()) {
        label = new QLabel(this);
        timer = new QTimer(this);
        scroll = new QScrollArea(this);

        scroll->setWidget(label);
        scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        auto *l = new QVBoxLayout();
        l->setMargin(0);
        l->addWidget(scroll);
        setLayout(l);
        connect(timer, SIGNAL(timeout()), this, SLOT(timeout()));

        timer->setSingleShot(false);
        timer->start((int) (1000.0f / 60.0f));
    }

    void setScene(std::shared_ptr<EntityScene> scene) {
        if (scene) {
            ren.setScene(*scene);
        }
    }

protected:
    void resizeEvent(QResizeEvent *event) override {
        label->setFixedSize(event->size());
        ren.setFrameSize(convert(event->size()));
    }

private slots:

    void timeout() {
        label->setPixmap(getPixmap());
    }

private:
    Vec2i getSize() {
        return convert(size());
    }

    Vec2i convert(QSize v) {
        return {v.width(), v.height()};
    }

    QPixmap getPixmap() {
        auto img = ren.getFrame();
        QImage image((const uchar *) img.getData(),
                     img.getWidth(),
                     img.getHeight(),
                     (int) ((size_t) img.getWidth() * sizeof(ColorRGBA)),
                     QImage::Format::Format_RGBA8888);
        return QPixmap::fromImage(image);
    }

    QTimer *timer;
    OffscreenRenderer ren;
    QScrollArea *scroll;
    QLabel *label;
};

#endif //XNG_EDITOR_SCENERENDERWIDGET_HPP
