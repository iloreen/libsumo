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
#ifndef SUMO_IMAGE_H
#define SUMO_IMAGE_H

#include "basics.h"

namespace sumo {

class Image : public MessageQueue, public StoppableThread
{
public:
	Image() : MessageQueue(), StoppableThread()
	{ }
	virtual ~Image() { }

	void process();

	virtual void handleImage(const struct image *image, const uint8_t *buffer, size_t size) = 0;
};

class ImageMplayerPopen : public Image
{
	FILE *_p;

public:
	ImageMplayerPopen();
	~ImageMplayerPopen();

	void handleImage(const struct image *image, const uint8_t *buffer, size_t size);

};

}
#endif
