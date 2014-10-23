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
#include "realtime.h"

#include "protocol.h"

#include "control.h"

namespace sumo {


void RealTime::heartBeatIn()
{
	while (!_stop) {
		uint8_t *b = _in.getMessage();
		if (!b)
			break;

		auto *s = reinterpret_cast<struct sync *>(b);
		//printf("HEARTBEAT: %d %d %d.%09d\n", s->head.ext, s->head.seqno, s->seconds, s->nanoseconds);
		s->head.ext = 1;
		_dp->send(*s);
		delete[] b;
	}
}

void RealTime::heartBeatOut()
{
	bool waiting_for_response = false;
	uint8_t seqno = 1;
	while (!_stop) {

		if (_sendHeartBeatOut) {
			if (waiting_for_response) {
				uint8_t *b = _out.getMessage(5);
				if (b != 0)
					waiting_for_response = false;
				delete[] b;
			} else {
				struct timespec tp;
				if (clock_gettime(CLOCK_MONOTONIC, &tp) != 0) {
					msleep(10);
					continue;
				}

				struct sync sync(seqno++, (uint32_t) tp.tv_sec, (uint32_t) tp.tv_nsec);
				if (!_dp->send(sync)) {
					msleep(100);
					continue;
				}
				waiting_for_response = true;
			}
		}

		if (_sendControl) {
			struct move m(seqno++, _speed != 0 || _turn != 0, _speed, _turn);
			_dp->send(m);

			msleep(20);
		}

		if (!_sendHeartBeatOut && !_sendControl)
			msleep(50);
	}
}

};
