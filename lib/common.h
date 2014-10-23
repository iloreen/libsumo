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
#ifndef _COMMON_H
#define _COMMON_H

#include <cstdint>
#include <cstdio>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstdarg>
#include <vector>
#include <list>
#include <string>
#include <unistd.h>
#include <sys/time.h>

using namespace std;
enum severity {
    SEV_ERROR,
    SEV_WARNING,
    SEV_INFO,
};

#define handle_error(msg, ...)   _handle_error(SEV_ERROR, __FUNCTION__, __FILE__, __LINE__, msg, ##__VA_ARGS__)
#define handle_warning(msg, ...) _handle_error(SEV_WARNING, __FUNCTION__, __FILE__, __LINE__, msg, ##__VA_ARGS__)
#define handle_info(msg, ...)    _handle_error(SEV_INFO, __FUNCTION__, __FILE__, __LINE__, msg, ##__VA_ARGS__)

#define verb(args...) do { printf(args); } while (0)

#define dump(args...) do { fprintf(stdout, args); } while (0)

extern void _handle_error (enum severity s, const char *func, const char *filename, const unsigned int line, const char *fmt ...);
extern void dumpPayload(const uint8_t *packet, uint32_t l, bool floats = true);

#endif
