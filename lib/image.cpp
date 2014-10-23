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
	FILE *p = popen("mplayer -cache 32 -demuxer lavf -lavfdopts format=mjpeg - >/dev/null 2>&1", "w");
	while (!_stop) {
		uint8_t *b = getMessage();
		if (!b)
			break;
		auto *i = reinterpret_cast<struct image *>(b);

		//printf("size: %d, fno: %d\n", i->head.size, i->frame_number);
		if (p)
			fwrite(b + sizeof(*i), 1, i->head.size - sizeof(*i), p);

		delete[] b;
	}
	pclose(p);
}

}
