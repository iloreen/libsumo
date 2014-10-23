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
#include <net/ethernet.h>

#include <netinet/if_ether.h>
#include <netinet/ether.h>

#include <cstring>
#include <iostream>

#include "network.h"
#include "decode.h"
#include "common.h"

using namespace std;

#if 0
#define dump_net(args...) dump(args)
#else
#define dump_net(args...) do { } while (0)
#endif

#define ETHER_HDRLEN sizeof(struct ether_header)

#define PROTOCOL_TCP 6
#define PROTOCOL_UDP 17

struct ipv4Header {
	uint8_t	 vhl;         /* header length, version */
#define IP_VER(ip)  (((ip)->vhl & 0xf0) >> 4)
#define IP_HLEN(ip) ( (ip)->vhl & 0x0f)
	uint8_t	 tos;         /* type of service */
	uint16_t len;         /* total length */
	uint16_t id;          /* identification */
	uint16_t off;         /* fragment offset field */
#define	IP_DF 0x4000      /* dont fragment flag */
#define	IP_MF 0x2000      /* more fragments flag */
#define	IP_OFFMASK 0x1fff /* mask for fragmenting bits */
	uint8_t	 ttl;         /* time to live */
	uint8_t	 protocol;	  /* protocol */
	uint16_t sum;         /* checksum */
	struct	in_addr src, dst; /* source and dest address */

};

struct tcpHeader {
	union {
		struct {
			uint16_t sport;
			uint16_t dport;
			uint32_t seq;
			uint32_t ack;
			uint8_t  data_offset; /* 4:7, 0:3 ->unused */
			uint8_t  flags;
#define TCP_FIN  0x01
#define TCP_SYN  0x02
#define TCP_RST  0x04
#define TCP_PUSH 0x08
#define TCP_ACK  0x10
#define TCP_URG  0x20
			uint16_t win;
			uint16_t sum;
			uint16_t urp;
		} tcp;

		struct {
			uint16_t sport;
			uint16_t dport;
			uint16_t length;
			uint16_t check;
		} udp;
	} u;
};

void networkSnooperPcap::callback(u_char *args, const struct pcap_pkthdr *pkthdr, const u_char *packet)
{
	networkSnooperPcap *o = reinterpret_cast<networkSnooperPcap *>(args);

	struct ether_header *ether_head = (struct ether_header *) packet;  /* net/ethernet.h */
	u_short ether_type;

	if (pkthdr->caplen < ETHER_HDRLEN) {
		handle_error("Packet length less than ethernet header length");
		return;
	}

	ether_type = ntohs(ether_head->ether_type);

	switch (ether_type) {
	case ETHERTYPE_IP: /* handle IP packet */
		o->handleIP(packet + sizeof(struct ether_header), pkthdr->len - sizeof(struct ether_header));
		break;
	case 2054: /* MDNS */
		break;
	default:
		handle_error("BUG: correct the pcap -> non-IP stuff has arrived %d", ether_type);
		break;
	}
}

void networkSnooperPcap::handleIP(const uint8_t *packet, uint32_t length)
{
	const struct ipv4Header *head = (const struct ipv4Header *) packet;
	/* check to see we have a packet of valid length */
	if (length < (sizeof(struct ipv4Header))) {
		handle_error("IPv4 header is bad %d vs %d", length, sizeof(struct ipv4Header));
		return;
	}

	if (IP_VER(head) != 4) {
		handle_error("only handling IPv4");
		return;
	}

	if (IP_HLEN(head) < 5) {
		handle_error("header-length is bad");
		return;
	}

	if (head->protocol != PROTOCOL_TCP &&
			head->protocol != PROTOCOL_UDP) {
		handle_error("unexpect protocol");
		return;
	}

	uint16_t len = ntohs(head->len);
	uint16_t off = ntohs(head->off);

	_id++;
	dump_net("%8d: ", _id); //dump_net(strTime());
	dump_net("%s: %s -> %s, HLEN: %d, VER: %d, len: %d, DF: %d, MF: %d, off: %d, id: %d\n",
			head->protocol == PROTOCOL_TCP ? "TCP" : "UDP",
			inet_ntoa(head->src), inet_ntoa(head->dst),
			IP_HLEN(head), IP_VER(head), len, !!(off & IP_DF), !!(off & IP_MF), off & 0x1fff, htons(head->id));

	packet += (IP_HLEN(head) * 4);
	len    -= (IP_HLEN(head) * 4);

	/* reassembling */
	bool reassembled_packet_completed = false;
	if ((off & IP_MF) || (off & 0x1fff)) {
		struct assemble &a = _asm[htons(head->id)];
		memcpy(&a.buf[(off&0x1fff) * 8], packet, len);
		a.size += len;

		if ((off & 0x1fff) == 0)
			a.total = ntohs( *(uint16_t *) &a.buf[4]);

		/* packet has been fully re-assembled */
		reassembled_packet_completed = a.total != 0 && a.total == a.size;
		if (reassembled_packet_completed) {
			packet = a.buf;
			len = a.size;
		} else /* or not yet */
			return;
	}
	struct tcpHeader *tcp = (struct tcpHeader *) packet;

	if (head->protocol == PROTOCOL_TCP) {
#if 0
		uint32_t seq = 0, ack = 0;
		seq = ntohl(tcp->u.tcp.seq);
		ack = ntohl(tcp->u.tcp.ack);
		dump_net("TCP: tcp_flags: %02x (%c%c%c%c%c%c) %u %u\n", tcp->u.tcp.flags,
				tcp->u.tcp.flags & TCP_URG  ? 'U' : '-',
				tcp->u.tcp.flags & TCP_ACK  ? 'A' : '-',
				tcp->u.tcp.flags & TCP_PUSH ? 'P' : '-',
				tcp->u.tcp.flags & TCP_RST  ? 'R' : '-',
				tcp->u.tcp.flags & TCP_SYN  ? 'S' : '-',
				tcp->u.tcp.flags & TCP_FIN  ? 'F' : '-',
				seq, ack);
#endif
	}


	switch (head->protocol) {
	case PROTOCOL_TCP:
		len    -= ((tcp->u.tcp.data_offset >> 4) & 0xf) * 4;
		packet += ((tcp->u.tcp.data_offset >> 4) & 0xf) * 4;
		break;
	case PROTOCOL_UDP:
		len    -= sizeof(tcp->u.udp);
		packet += sizeof(tcp->u.udp);
		break;
	default:
		break;
	}

	/* determine the data flow direction */
	bool in = false;
	if (head->src.s_addr == _remote_addr.s_addr)
		in = true;
	else
		in = false;

	if (head->protocol == PROTOCOL_UDP) {
		if (in)
			udpIn(packet, len);
		else
			udpOut(packet, len);
	} else { /* TCP */

		switch (ntohs(in ? tcp->u.tcp.sport : tcp->u.tcp.dport)) {
		case 44444: {
			printf("initial protocol %s\n", in ? "IN" : "OUT");
			printf("%s", packet);
			break;
		}

		default:
			break;
		}
	}

	if (reassembled_packet_completed)
		_asm.erase(_asm.find(htons(head->id)));
}

networkSnooperPcap::networkSnooperPcap() :
	_errbuf(), _pcap_ctx(NULL), _id(0),
	//	_udp_in(new gameServerUdpIn(t)), _udp_out(new gameServerUdpOut(t)),
	_capture_filename()
{
	inet_aton("192.168.2.1", &_remote_addr);
}

bool networkSnooperPcap::capture()
{
	struct bpf_program fp;      /* hold compiled program     */

	if (_pcap_ctx == NULL) {
		handle_error("capturing could not be started. pcap error: %s\n", _errbuf);
		return false;
	}

	if (pcap_compile(_pcap_ctx, &fp, "(dst net 192.168.2.0/24) and (src net 192.168.2.0/24)", 1, 0) == -1) {
		handle_error("pcap_compile: error\n");
		return false;
	}

	if (pcap_setfilter(_pcap_ctx, &fp) == -1) {
		handle_error("pcap_setfilter: error\n");
		return false;
	}

	/* start all the producer threads */

	pcap_loop(_pcap_ctx, -1, networkSnooperPcap::callback, reinterpret_cast<u_char *>(this));
	handle_info("pcap_loop ended\n");

	pcap_close(_pcap_ctx);
	_pcap_ctx = NULL;

	return true;
}

#if 0
bool networkSnooperPcap::captureLive()
{
	bpf_u_int32 maskp;          /* subnet mask               */
	bpf_u_int32 netp;           /* ip                        */
	char *dev;

	/* grab a device to peak into... */
	dev = pcap_lookupdev(_errbuf);
	if(dev == NULL) {
		handle_error("pcap_lookupdev: %s\n", _errbuf);
		return false;
	}
	cout << "using dev '" << dev << "'" << endl;

	/* ask pcap for the network address and mask of the device */
	pcap_lookupnet(dev, &netp, &maskp, _errbuf);

	_pcap_ctx = pcap_open_live(dev, 65535, 0, 0, _errbuf);

	return capture();
}
#endif

bool networkSnooperPcap::captureFromFile()
{
	_pcap_ctx = pcap_open_offline(_capture_filename.c_str(), _errbuf);
	handle_info("filename: %s\n", _capture_filename.c_str());

	return capture();
}

void networkSnooperPcap::process()
{
	if (_capture_filename.length())
		captureFromFile();
#if 0
	else
		captureLive();
#endif
}

void networkSnooperPcap::start(const string &filename)
{
	_capture_filename = filename;
}

void networkSnooperPcap::stop()
{
	printf("received %lu unfinished packets\n", _asm.size());
	if (_pcap_ctx) {
		struct pcap_stat ps;

		pcap_breakloop(_pcap_ctx);
		if (pcap_stats(_pcap_ctx, &ps) == 0)
			cout << "PCAP stats: received: " << ps.ps_recv << ", dropped: " << ps.ps_drop << endl;
	}
}

