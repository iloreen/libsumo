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
#include <control.h>

int main(void)
{
	sumo::Control c;

	if (!c.open())
		return EXIT_FAILURE;

#if 0
	for (int i = 0; i < 4; i++) {
		dispatcher.quickTurn(-M_PI/2);
		usleep(1000000);
	}

	for (int i = 0 ; i < 6; i++) {
		dispatcher.quickTurn(M_PI/3);
		usleep(1500000);
	}
#endif

	c.move(-20, 10);

	getchar(); //dispatcher.wait();

	c.close();

	return EXIT_SUCCESS;
}
