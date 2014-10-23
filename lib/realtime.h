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
#ifndef SUMO_REALTIME_H
#define SUMO_REALTIME_H

#include "basics.h"

#include <cstring>

namespace sumo {

class Control;

class RealTime : public StoppableThread
{
	MessageQueue _in;
	MessageQueue _out;

	Control *_dp;

	int8_t _speed, _turn;

	bool _sendHeartBeatOut;
	bool _sendControl;

public:
	RealTime(Control *d) : StoppableThread(), _in(), _out(), _dp(d), _speed(0), _turn(0), _sendHeartBeatOut(false), _sendControl(false)
	{ }

	void heartBeatIn();
	void heartBeatOut();

	void setSpeed(int8_t s) { _speed = s; }
	void setTurn(int8_t t)  { _turn = t; }

	void activateHeartBeatOut(bool v) { _sendHeartBeatOut = v; }
	void activateControl(bool v)      { _sendControl = v; }

	void stop()
	{
		StoppableThread::stop();
		activateHeartBeatOut(false);
		activateControl(false);
	}

	MessageQueue & inMsg() { return _in; }
	MessageQueue & outMsg() { return _out; }
};
}

#endif
