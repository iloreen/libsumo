/*
 * This file is part of the libsumo - a reverse-engineered library for
 * controlling Parrot's connected toy: Jumping Sumo
 *
 * Copyright (C) 2014 I. Loreen
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */
#ifndef SUMO_WIDGET_H
#define SUMO_WIDGET_H

#include <QMainWindow>
#include <QMap>

#include "ui_widget.h"

class QSlider;

namespace sumo {
	class Control;
}

class SumoWidget : public QMainWindow, private Ui_SumoMain
{
    Q_OBJECT

    QMap<int, bool> keys;

	int accel;
	int turn;

	sumo::Control *sumo;

	int timer_id;

public:
    SumoWidget();
    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);
    void timerEvent(QTimerEvent *);
private slots:

	void on__open_close_clicked(bool);
};

#endif
