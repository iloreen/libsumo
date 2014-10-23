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
#ifndef ANALYZE_H
#define ANALYZE_H

#include <pcap.h>

#include <cstdint>
#include <cstring>
#include <map>

#include <arpa/inet.h>

#include "common.h"


struct assemble {
	uint16_t total;
	uint16_t size;
	uint8_t buf[65536];
public:
	assemble() : total(0), size(0), buf() { memset(buf, 0, sizeof(buf)); }
};

class networkSnooperPcap
{
private:
	char _errbuf[PCAP_ERRBUF_SIZE];
	pcap_t *_pcap_ctx;

	uint32_t _id;

	std::string _capture_filename;

	static void callback(u_char *args, const struct pcap_pkthdr *pkthdr, const u_char *packet);

	bool capture();

	void handleIP(const uint8_t *packet, uint32_t length);

	//	bool captureLive();
	bool captureFromFile();

	struct in_addr _remote_addr;

	std::map<uint16_t, struct assemble> _asm;

public:
	networkSnooperPcap();

	void start(const std::string &);
	void stop();

	void process();
};

#endif
