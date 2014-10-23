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
#include <cstdint>
#include <cstring>

#include "common.h"

#include <cassert>
#include <cmath>

#include <protocol.h>

using namespace sumo;

static int prefix;
static int count;

void udpIn(const uint8_t *b, uint32_t len)
{
	while (len) {
		struct header *head = (struct header *) b;
		printf(" IN: head %d: size: %5d: ext: %3d, seqno: %3d, unk: %3d\t",
				head->type, head->size, head->ext, head->seqno, head->unk);
		assert(head->unk == 0);

		switch (head->type) {
		case SYNC: {
			struct sync *s = (struct sync *) b;
			printf("%s %d.%09d secs\n", head->ext ? "RECV" : "SEND", s->seconds, s->nanoseconds);
			break;
		}
		case ACK: {
			struct ack *a  = (struct ack *) b;
			printf("ACK for OUT-IOCTL seqno: %d\n", a->seqno);
			break;
		}

		case IOCTL: {
			auto *a = reinterpret_cast<const struct ioctl_packet *>(b);
			const uint8_t *payload = b + sizeof(*a);
			printf("ioctl: flags %02x, type: %d, func: %d; unk: %d\t", a->flags, a->type, a->func, a->unk);

			//assert(a->flags == 0);
			//assert(a->unk == 0);

			switch (a->type) {
			case 5: /* confirm */
				switch (a->func) {
				case 1:
					printf("battery level: %d\n", *payload);
					break;
				case 2:
					printf("info ??: %s\n", payload + 1);
					break;
				case 4:
					printf("confirm date: %s\n", payload);
					break;
				case 5:
					printf("confirm time: %s\n", payload);
					break;

				default:
					printf("unh func 5/%d\n", a->func);
					//dumpPayload(b + sizeof(*head), head->size - sizeof(*head));
					break;
				}
				break;

			case 3: /* id */
				switch(a->func) {
				case 2:
					printf("name: %s\n", payload);
					break;
				case 3:
					printf("swver: %s HW: %s\n", payload, payload + strlen((const char *)payload) + 1);
					break;
				case 4:
					printf("PI: %s\n", payload);
					break;
				case 5:
					printf("S/N: %s\n", payload);
					break;
				case 6:
					printf("LANG: %s\n", payload);
					break;
				case 7:
					printf("???: %d\n", *payload);
					break;

				default:
					printf("unh func 3/%d\n", a->func);
					//dumpPayload(b + sizeof(*head), head->size - sizeof(*head));
					break;
				}
				break;

			case 9:
				printf("type 9 ???\n");
				break;

			default:
				//dumpPayload(b + sizeof(*head), head->size - sizeof(*head));
				printf("unh type %d\n", a->type);
				break;
			}


		} break;

		case IMAGE: { /* JPEG */
			printf("image\n");
#if 0
			dumpPayload(b + sizeof(struct header), 12);

			//write | mplayer -demuxer lavf -lavfdopts format=mjpeg  -
			char s[FILENAME_MAX];
			snprintf(s, sizeof(s), "img/%03d-image-%05d.jpg", prefix, count);
			//snprintf(s, sizeof(s), "img/image.jpg", count);
			count++;

			int file = open(s, O_WRONLY | O_CREAT | O_TRUNC, 0644);
			write(file, b + sizeof(struct header) + 5, len - sizeof(struct header) - 5);
			close(file);
#endif
		} break;
		default:
			handle_error("unknown IN %d\n", *b);
			break;
		}

		len -= head->size;
		b += head->size;
	}
}

void udpOut(const uint8_t *b, uint32_t len)
{
	while (len) {
		struct header *head = (struct header *) b;
		printf("OUT: head %d: size: %5d: ext: %3d, seqno: %3d, unk: %3d\t", head->type, head->size, head->ext, head->seqno, head->unk);
		switch (head->type) {
		case SYNC: {

			switch (head->ext) {
			case 0:
			case 1: {
				struct sync *s = (struct sync *) b;
				printf("%s %d.%09d secs\n", head->ext ? "RECV" : "SEND", s->seconds, s->nanoseconds);
				assert(s->nanoseconds < 1000000000);
			} break;

			case 10: {
				struct move *s = (struct move *) b;
				//uint8_t b[4];
				//uint8_t accel;
				//int8_t  speed;
				//int8_t unk;
				assert(s->b[0] == 3);
				//usleep(240000);
				printf("b[0-4] ... set speed: %d, turn: %d\n",s->speed, s->turn);
				dumpPayload(s->b, 7);
			} break;
			default:
				printf("unknown ext: %d\n", head->ext);
				dumpPayload(b + sizeof(*head), head->size - sizeof(*head), false);
				assert( 0 );
				break;
			}
			break;
		}
		case ACK:
			printf("ACK for IN-IOCTL seqno: %d\n", * (b + sizeof(*head)));
			break;
		case IOCTL: {
			prefix++;
			count = 0;

			auto *a = reinterpret_cast<const struct ioctl_packet *>(b);
			printf("ioctl: flags: %02x, type: %d, func: %d; unk: %d, prefix: %d\t", a->flags, a->type, a->func, a->unk, prefix);
			switch (a->type) {
			case 0:
				printf("unhandled rotate X?\n");
				break;
			case 2:
				switch (a->func) {
				case 0:
					printf("get_id\n");
					break;
				case 3: {
					auto *j = reinterpret_cast<const struct ioctl<uint32_t> *>(b);
					printf("jump: %x\n", j->param);
				} break;
				case 4: {
					auto *j = reinterpret_cast<const struct ioctl<uint32_t> *>(b);
					printf("????: %x\n", j->param);
				} break;
				default:
					printf("4/ %d no additional data: %lu?\n", a->func, head->size - sizeof(*a));
					assert((head->size - sizeof(*a)) == 0);
					break;
				}
				break;
			case 3: {
				auto *t = reinterpret_cast<const struct turn *>(b);
				printf("turn: %f deg\n", t->param / M_PI * 180);
			} break;
			case 4:
				switch (a->func) {
				case 0:
					printf("unknown func - no add. data video starts here\n");
					break;
				case 1:
					printf("set date: %s\n", b + sizeof(*a) );
					break;
				case 2:
					printf("set time: %s\n", b + sizeof(*a) );
					break;
				default:
					printf("func-type unhandled\n");
					dumpPayload(b + sizeof(*head), head->size - sizeof(*head));
					assert(0);
					break;
				}
				break;
			case 8: {
				auto *c = reinterpret_cast<const struct ioctl<uint8_t> *>(b);
				printf("enable 8 ???: %d\n", c->param);
			} break;

			case 18: {
				auto *c = reinterpret_cast<const struct ioctl<uint8_t> *>(b);
				printf("enable 18 ???: %d\n", c->param);
			} break;

			default:
				printf("four unhandled\n");
				dumpPayload(b + sizeof(*head), head->size - sizeof(*head));
				assert(0);
				break;
			}

		} break;
		default:
			handle_error("unknown OUT %d\n", *b);
		}
		b += head->size;
		len -= head->size;
	}
}

