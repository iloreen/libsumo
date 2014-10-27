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
#include "sumo-widget.h"

#include <QKeyEvent>
#include <QSlider>

#include <control.h>

SumoWidget::SumoWidget() : accel(0), turn(0), sumo(0)
{
	setupUi(this);

	setFocusPolicy(Qt::StrongFocus);
}

void SumoWidget::keyPressEvent(QKeyEvent *e)
{
	if (!e->isAutoRepeat()) {
		keys[e->key()] = true;
	}
	QWidget::keyPressEvent(e);
}

void SumoWidget::keyReleaseEvent(QKeyEvent *e)
{
	if (!e->isAutoRepeat()) {
		keys[e->key()] = false;
	}
	QWidget::keyReleaseEvent(e);
}

void SumoWidget::timerEvent(QTimerEvent *)
{
	int mod = 0;

#define ACCELERATION_CONSTANT 6
	/* auto reduce speed if neither accelerating nor breaking */
	if (!keys[Qt::Key_Down] && !keys[Qt::Key_Up]) {
		mod = -(accel/ACCELERATION_CONSTANT); /* the faster we go the more we reduce speed */
		if (mod == 0 && accel)
			mod = 1 * (accel < 0 ? 1 : -1);
	}

	if (keys[Qt::Key_Up]) {
		if (accel >= 0)
			mod = ACCELERATION_CONSTANT;
		else /* breaking - we are going reverse */
			mod = ACCELERATION_CONSTANT * 2;
	}

	if (keys[Qt::Key_Down]) {
		if (accel <= 0)
			mod = -ACCELERATION_CONSTANT;
		else /* breaking */
			mod = -ACCELERATION_CONSTANT * 2;
	}

	accel += mod;

	if (accel > 127)
		accel = 127;
	if (accel < -127)
		accel = -127;

	/* turning */
#define TURN_CONSTANT 5
	mod = 0;
	if (!keys[Qt::Key_Left] && !keys[Qt::Key_Right]) {
		mod = -turn/TURN_CONSTANT * 3;


		if (abs(turn) < TURN_CONSTANT && turn)
			mod = -turn; //TURN_CONSTANT * (turn < 0 ? 1 : -1);
	}

	if (keys[Qt::Key_Left])
		mod = -TURN_CONSTANT;

	if (keys[Qt::Key_Right])
		mod = TURN_CONSTANT;

	turn += mod;
	if (turn > 32)
		turn = 32;
	if (turn < -32)
		turn = -32;

	_speed->setValue(accel);
	_turning->setValue(turn);

	sumo->move(accel, turn);
	_batteryLevel->setValue(sumo->batteryLevel());
//	fprintf(stderr, "a: %4d, t: %4d\n", accel, turn);
}

void SumoWidget::on__open_close_clicked(bool)
{
	if (sumo) {
		killTimer(timer_id);
		sumo->close();
		delete sumo; sumo = 0;
		_open_close->setText("Open");
	} else {
		sumo = new sumo::Control;
		if (!sumo->open()) {
			delete sumo;
			sumo = 0;
			return;
		}

		timer_id = startTimer(75);
		_open_close->setText("Close");
	}
}

void SumoWidget::on__flip_clicked(bool)
{
	if (sumo)
		sumo->flipUpsideDown();
}

void SumoWidget::on__flipDown_clicked(bool)
{
	if (sumo)
		sumo->flipDownsideUp();
}

void SumoWidget::on__balance_clicked(bool)
{
	if (sumo)
		sumo->handstandBalance();
}

void SumoWidget::on__hijump_clicked(bool)
{
	if (sumo)
		sumo->highJump();
}

void SumoWidget::on__longjump_clicked(bool)
{
	if (sumo)
		sumo->longJump();
}

void SumoWidget::on__swing_clicked(bool)
{
	if (sumo)
		sumo->swing();
}

void SumoWidget::on__growingCircles_clicked(bool)
{
	if (sumo)
		sumo->growingCircles();
}

void SumoWidget::on__slalom_clicked(bool)
{
	if (sumo)
		sumo->slalom();
}

void SumoWidget::on__tap_clicked(bool)
{
	if (sumo)
		sumo->tap();
}

void SumoWidget::on__turn_clicked(bool)
{
	if (sumo)
		sumo->quickTurnRight();
}

void SumoWidget::on__turnLeftRight_clicked(bool)
{
	if (sumo)
		sumo->quickTurnRightLeft();
}

void SumoWidget::on__turnToBalance_clicked(bool)
{	if (sumo)
		sumo->turnToBalance();
}

void SumoWidget::on__watchLeftRight_clicked(bool)
{
	if (sumo)
		sumo->lookLeftAndRight();
}

void SumoWidget::on__turnAndJump_clicked(bool)
{
	if (sumo)
		sumo->turnAndJump();
}
