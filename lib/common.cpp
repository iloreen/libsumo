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
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <sstream>
#include <sys/time.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#include <cstring>
#include <cmath>

#include "common.h"

using namespace std;

void _handle_error (enum severity s, const char *func, const char *filename, const unsigned int line, const char *fmt ...)
{
	va_list ap;

	cout << endl;

	switch (s){
	case SEV_INFO : fprintf(stderr, "INFO("); break;
	case SEV_WARNING : fprintf(stderr, "WARNING("); break;
	default:
	case SEV_ERROR : fprintf(stderr, "ERROR("); break;
	};
	fprintf(stderr, "%s,%s:%i): ", func, filename, line);

	va_start(ap,fmt);
	vfprintf(stderr, fmt, ap);
	fflush(stderr);
	fprintf(stderr, "\n");
	fflush(stderr);
	va_end(ap);

	if (s==SEV_ERROR){
		exit(EXIT_FAILURE);
	}else{
		return;
	}
}

static struct timeval start_cap_time;
static uint32_t id;
static char timestamp[100];

static const char * strTime()
{
	double off;
	struct timeval t;
	gettimeofday(&t, NULL);

	off = t.tv_sec  + t.tv_usec/1000000.0;
	off -= start_cap_time.tv_sec + start_cap_time.tv_usec/1000000.0;

	snprintf(timestamp, 100, "%6d: %9.3f ", id, off);
	return timestamp;
}

static void printTime()
{
	dump(strTime());
}

void dumpPayload(const uint8_t *packet, uint32_t l, bool floats)
{
	uint32_t i;

	if (l < 3)
		return;

	printTime();
	dump("ascii: ");
	for (i = 0; i < l; i++)
		dump("%c", isprint(packet[i]) ? packet[i] : '.');
	dump("\n");

	printTime();
	dump("offs:  ");
	for (i = 0; i < l-3; i+=4)
		dump("%-11u     ", i);
	dump("\n");

	printTime();
	dump("hex  : ");
	for (i = 0; i < l; i++)
		dump("%02x  ", packet[i]);
	dump("\n");

	printTime();
	dump("ascii: ");
	for (i = 0; i < l; i++)
		dump("%c   ", isprint(packet[i]) ? packet[i] : '.');
	dump("\n");

	printTime();
	dump("8b:    ");
	for (i = 0; l && i < l; i++)
		dump("%-3u ", packet[i]);
	dump("\n");

	printTime();
	dump("16b 0: ");
	for (i = 0; l && i < l-1; i += 2)
		dump("%-5u   ", *((uint16_t *) &packet[i]) );
	dump("\n");

	printTime();
	dump("16b 1:     ");
	for (i = 1; l && i < l-1; i += 2)
		dump("%-5u   ", *((uint16_t *) &packet[i]) );
	dump("\n");

	printTime();
	dump("32b 0: ");
	for (i = 0; l && i < l-3; i += 4)
		dump("%-11u     ", *((uint32_t *) &packet[i]) );
	dump("\n");

	printTime();
	dump("32b 1:     ");
	for (i = 1; l && i < l-3; i += 4)
		dump("%-11u     ", *((uint32_t *) &packet[i]) );
	dump("\n");

	printTime();
	dump("32b 2:         ");
	for (i = 2; l && i < l-3; i += 4)
		dump("%-11u     ", *((uint32_t *) &packet[i]) );
	dump("\n");

	printTime();
	dump("32b 3:             ");
	for (i = 3; l && i < l-3; i += 4)
		dump("%-11u     ", *((uint32_t *) &packet[i]) );
	dump("\n");

	if (floats) {
		printTime();
		dump("32f 0: ");
		for (i = 0; l && i < l-3; i += 4)
			dump("%-11g     ", *((float *) &packet[i]) );
		dump("\n");

		printTime();
		dump("32f 1:     ");
		for (i = 1; l && i < l-3; i += 4)
			dump("%-11g     ", *((float *) &packet[i]) );
		dump("\n");

		printTime();
		dump("32f 2:         ");
		for (i = 2; l && i < l-3; i += 4)
			dump("%-11g     ", *((float *) &packet[i]) );
		dump("\n");

		printTime();
		dump("32f 3:             ");
		for (i = 3; l && i < l-3; i += 4)
			dump("%-11g     ", *((float *) &packet[i]) );
		dump("\n");

		if (l >= 8) {
			printTime();
			dump("64f 0: ");
			for (i = 0; l && i < l-7; i += 8)
				dump("%-27.4g     ", *((double *) &packet[i]) );
			dump("\n");

			printTime();
			dump("64f 1:     ");
			for (i = 1; l && i < l-7; i += 8)
				dump("%-27.4g     ", *((double *) &packet[i]) );
			dump("\n");

			printTime();
			dump("64g 2:         ");
			for (i = 2; l && i < l-7; i += 8)
				dump("%-27.4g     ", *((double *) &packet[i]) );
			dump("\n");

			printTime();
			dump("64g 3:             ");
			for (i = 3; l && i < l-7; i += 8)
				dump("%-27.4g     ", *((double *) &packet[i]) );
			dump("\n");

			printTime();
			dump("64g 4:                 ");
			for (i = 4; l && i < l-7; i += 8)
				dump("%-27.4g     ", *((double *) &packet[i]) );
			dump("\n");

			printTime();
			dump("64g 5:                     ");
			for (i = 5; l && i < l-7; i += 8)
				dump("%-27.4g     ", *((double *) &packet[i]) );
			dump("\n");

			printTime();
			dump("64g 6:                         ");
			for (i = 6; l && i < l-7; i += 8)
				dump("%-27.4g     ", *((double *) &packet[i]) );
			dump("\n");

			printTime();
			dump("64g 7:                             ");
			for (i = 7; l && i < l-7; i += 8)
				dump("%-27.4g     ", *((double *) &packet[i]) );
			dump("\n");
		}
	}
}
