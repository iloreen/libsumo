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
#include "image.h"

#include "protocol.h"

namespace sumo
{

void Image::process()
{
	while (!_stop) {
		uint8_t *b = getMessage();
		if (!b)
			break;
		auto *i = reinterpret_cast<struct image *>(b);

		handleImage(i, b + sizeof(*i), i->head.size - sizeof(*i));

		//printf("size: %d, fno: %d\n", i->head.size, i->frame_number);

		delete[] b;
	}
}

ImageMplayerPopen::ImageMplayerPopen() : _p(0)
{
	_p = popen("mplayer -cache 32 -demuxer lavf -lavfdopts format=mjpeg - >/dev/null 2>&1", "w");
	if (!_p)
		fprintf(stderr, "could not start mplayer process via popen() - images will be dropped\n");
}

ImageMplayerPopen::~ImageMplayerPopen()
{
	if (_p)
		pclose(_p);
}

void ImageMplayerPopen::handleImage(const struct image *, const uint8_t *buffer, size_t size)
{
	if (_p)
		fwrite(buffer, 1, size, _p);
}

}
