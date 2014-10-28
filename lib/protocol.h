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
#ifndef SUMO_PROTOCOL
#define SUMO_PROTOCOL

#include <cstdint>

#define ACK   1
#define SYNC  2
#define IMAGE 3
#define IOCTL 4

namespace sumo
{

struct header
{
	uint8_t type;  /* the main protocol type/category: ACK, SYNC, IMAGE, IOCTL */
	uint8_t ext;   /* extension of this type*/
	uint8_t seqno; /* sequence number for this type */
	uint16_t size; /* packet-size including header */
	uint16_t unk;  /* unknown 16bit always 0 in the analyzed data */
} __attribute__((packed));

/* packet base - has a header */
struct packet
{
	struct header head;

	packet(uint8_t t, uint8_t e, uint8_t s, uint16_t size) : head( { t, e, s, size, 0 } )
	{ }
} __attribute__((packed));

/* synchronoziation timestamp since device has booted in nanoseconds
 * bi-directional and acknowledged from the other side acknowledge is
 * done with setting head.ext = 1
 *
 * Seems to be send by the device at least before another transaction
 * is done, otherwise with a delay of 500msecs
 */
struct sync : public packet { uint32_t seconds; uint32_t nanoseconds;

	sync(uint8_t seq, uint32_t sec, uint32_t nsec) :
		packet(SYNC, 0, seq, sizeof(*this)),
		seconds(sec), nanoseconds(nsec)
	{ }
} __attribute__((packed));

/* a packet of type sync used to move forward or backward and to turn the sumo */
struct move : public packet
{
	uint8_t b[4];
	uint8_t active; /* if speed or turn is non-zero, this field goes to one */
	int8_t  speed; /* -127 - 127 */
	int8_t  turn;  /* -64 - 64 */

	move(uint8_t seq, uint8_t a, int8_t s, int8_t t) :
		packet(SYNC, 10, seq, sizeof(*this)),
		b{0x03,0,0,0}, active(a), speed(s), turn(t)
	{ }
} __attribute__((packed));

/* acknowledge an ioctl packet */
struct ack : public packet
{
	uint8_t seqno; /* ioctl seqno to be acknowledged */

	ack(uint8_t ext, uint8_t seq, uint8_t confirm) :
		packet(ACK, ext, seq, sizeof(*this)),
		seqno(confirm)
	{}
} __attribute__((packed));

/* base class of an ioctl-packet, containing the common fields */
struct ioctl_packet : public packet
{
	uint8_t flags; /* I used flags to name this field as I saw bitfields being used for some exchanges */
	uint8_t type;  /* type and func are used to indicate what to tell or instruct with this packet and indicate which data-payload is send */
	uint8_t func;  /* func or index */
	uint8_t unk;   /* unknown */

	ioctl_packet(uint8_t seq, uint16_t size, uint8_t t, uint8_t fu, uint8_t flags = 0) :
		packet(IOCTL, 11, seq, size),  /* ext is always set to 11 for outgoing packets */
		flags(flags), type(t), func(fu), unk(0) /* unknown always set to 0 */
	{}

} __attribute__((packed));

template<typename T>
struct ioctl : public ioctl_packet /* template to create customized ioctl-payload packets */
{
	T param;

	ioctl(uint8_t seq, uint8_t t, uint8_t fu, uint8_t flags = 0) :
		ioctl_packet(seq, sizeof(*this), t, fu, flags),
		param()
	{ }
} __attribute__((packed));

struct date : public ioctl<char[11]>
{
	date(uint8_t seq) :
		ioctl(seq, 4, 1)
	{ }
} __attribute__((packed));

/* used to set the current timestamp */
struct time : public ioctl<char[13]>
{
	time(uint8_t seq) :
		ioctl(seq, 4, 2)
	{ }
} __attribute__((packed));

/* quick to turn in param = [ -PI,  PI ]; */
struct turn : public ioctl<float>
{
	turn(uint8_t seq, float an) :
		ioctl(seq, 3, 1, 0x83)
	{
		param = an;
	}
} __attribute__((packed));

/* instruct a jump - uses a uint32 for signaling the kind of jump */
struct jump : public ioctl<uint32_t>
{
	enum type {
		Long,
		High,
	};
	jump(uint8_t seq, enum type type) : ioctl(seq, 2, 3, 0x03)
	{
		if (type == Long)
			param = 0;
		else if (type == High)
			param = 1;
	}
};

struct special : public ioctl<uint32_t>
{
	enum type
	{
		QuickTurnRight,
		LookLeftAndRight,
		Tap,
		Swing,
		QuickTurnRightLeft,
		TurnAndJump,
		TurnToBalance,
		Slalom,
		GrowingCircles,
	};

	special(uint8_t seq, enum type type) : ioctl(seq, 2, 4, 0x03)
	{
		/* tried 1 - does nothing */
		switch (type) {
		case QuickTurnRight:
			param = 1;
			break;

		case Tap:
			param = 2;
			break;

		case LookLeftAndRight:
			param = 3;
			break;

		case QuickTurnRightLeft:
			param = 4;
			break;

		case Swing:
			param = 5;
			break;

		case TurnAndJump:
			param = 6;
			break;

		case TurnToBalance:
			param = 7;
			break;

		case GrowingCircles:
			param = 8;
			break;

		case Slalom:
			param = 9;
			break;
		}
	}
};

struct flip : public ioctl<uint32_t>
{
	enum type {
		Balance,
		UpsideDown,
		DownsideUp,
	};

	flip(uint8_t seq, enum type type) : ioctl(seq, 0, 1, 0x03)
	{
		switch (type) {
		case Balance:
			param = 0;
			break;
		case UpsideDown:
			param = 1;
			break;
		case DownsideUp:
			param = 2;
			break;
		}
	}
};

struct image : public packet
{
    uint16_t frame_number; /* absolute frame-name */
	uint16_t unk0;         /* unknown */
	uint8_t unk1;          /* unknown */
	/* uint8_t jpeg[head.size - sizeof(head) - 5)] */ /* image data JPEG */
} __attribute__((packed));

}

#endif
