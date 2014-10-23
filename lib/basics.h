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
#ifndef SUMO_BASICS_H
#define SUMO_BASICS_H

#include <cstdint>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <atomic>
#include <thread>

#include <cstring>

namespace sumo {

class MessageQueue
{
	std::queue<uint8_t *>   _queue;
	std::mutex              _queue_mutex;
	std::condition_variable _queue_cv;

public:
	MessageQueue() : _queue(), _queue_mutex(), _queue_cv()
	{}

	void sendMessage(const uint8_t *b, size_t len)
	{
		std::lock_guard<std::mutex> __g(_queue_mutex);
		uint8_t *buf = new uint8_t[len];
		memcpy(buf, b, len);
		_queue.push(buf);
		_queue_cv.notify_one();
	}

	uint8_t * getMessage(uint32_t timeout_ms = 0)
	{
		std::unique_lock<std::mutex> lock(_queue_mutex);
		if (_queue.size() == 0) {
			if (timeout_ms == 0)
				_queue_cv.wait(lock);
			else
				if (_queue_cv.wait_for(lock, std::chrono::milliseconds(timeout_ms) ) == std::cv_status::timeout)
					return 0;
		}

		uint8_t *b = _queue.front();
		_queue.pop();

		return b;
	}
};

class StoppableThread
{
protected:
	std::atomic<bool> _stop;

public:
	virtual ~StoppableThread() {}

	virtual void stop() { _stop = true; }
	void reset() { _stop = false; }

	void msleep(uint32_t dur) { std::this_thread::sleep_for( std::chrono::milliseconds( dur ) ); }
};

}

#endif
