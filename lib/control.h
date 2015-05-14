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
#ifndef SUMO_CONTROL_H
#define SUMO_CONTROL_H

#include <cstdint>

#include "basics.h"

namespace sumo {

struct packet;

class RealTime;
class Image;
class ControlIn;

class Control : public StoppableThread, public MessageQueue
{
	int _udp;
	std::mutex _send_lock;

	RealTime *_rt;
	Image *_image;
	ControlIn *_ctrl_in;

	std::thread       _rt_thread_in;
	std::thread       _rt_thread_out;
	std::thread       _dispatch_thread;
	std::thread       _ctrl_in_thread;
	std::thread       _image_thread;

	std::atomic<uint8_t> _seqno;

	void dispatch();
	bool waitForAck(const struct ioctl_packet &h);
	void sendDate();
	void sendTime();
	void getInfo();
	void enableStuff();

	bool blockingSend(const struct ioctl_packet &b);

public:
	Control(Image *image) : _udp(-1), _send_lock(), _rt(0), _image(image), _rt_thread_in(), _rt_thread_out(), _image_thread(), _seqno(0)
	{ }
	~Control();

	bool open();
	void close();

	void move(int8_t, int8_t);

	void highJump();
	void longJump();

	void quickTurn(float angle);
	void flipUpsideDown();
	void flipDownsideUp();
	void handstandBalance();

	void quickTurnRight();
	void lookLeftAndRight();
	void tap();
	void swing();
	void quickTurnRightLeft();
	void turnAndJump();
	void turnToBalance();
	void slalom();
	void growingCircles();

	int batteryLevel();

	bool send(const struct packet &b);
};

}

#endif

