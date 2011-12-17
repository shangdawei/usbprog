/*
 * $Id: dlc5.c,v 1.8 2003/08/19 08:42:20 telka Exp $
 *
 * Xilinx DLC5 JTAG Parallel Cable III Driver
 * Copyright (C) 2002, 2003 ETC s.r.o.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 *
 * Written by Marcel Telka <marcel@telka.sk>, 2002, 2003.
 *
 * Documentation:
 * [1] Xilinx, Inc., "JTAG Programmer Guide",
 *     http://toolbox.xilinx.com/docsan/3_1i/pdf/docs/jtg/jtg.pdf
 *
 */

#include "sysdep.h"

#include "cable.h"
#include "parport.h"
#include "chain.h"

#include "generic.h"

/* see Figure B-1 in [1] */

/*
 * data D[7:0] (pins 9:2)
 */
#define	TDI	0
#define	TCK	1
#define	TMS	2
#define	CTRL	3
#define	PROG	4

/*
 * 7 - BUSY (pin 11)
 * 6 - ACK (pin 10)
 * 5 - PE (pin 12)
 * 4 - SEL (pin 13)
 * 3 - ERROR (pin 15)
 */
#define	TDO	4

static int
dlc5_init( cable_t *cable )
{
	if (parport_open( cable->port ))
		return -1;
	PARAM_TRST(cable) = 1;

	return 0;
}

static void
dlc5_clock( cable_t *cable, int tms, int tdi )
{
	tms = tms ? 1 : 0;
	tdi = tdi ? 1 : 0;

	parport_set_data( cable->port, (1 << PROG) | (0 << TCK) | (tms << TMS) | (tdi << TDI) );
	cable_wait();
	parport_set_data( cable->port, (1 << PROG) | (1 << TCK) | (tms << TMS) | (tdi << TDI) );
	cable_wait();
}

static int
dlc5_get_tdo( cable_t *cable )
{
	parport_set_data( cable->port, (1 << PROG) | (0 << TCK) );
	cable_wait();
	return (parport_get_status( cable->port ) >> TDO) & 1;
}

static int
dlc5_set_trst( cable_t *cable, int trst )
{
	return 1;
}

cable_driver_t dlc5_cable_driver = {
	"DLC5",
	N_("Xilinx DLC5 JTAG Parallel Cable III"),
	generic_connect,
	generic_disconnect,
	generic_cable_free,
	dlc5_init,
	generic_done,
	dlc5_clock,
	dlc5_get_tdo,
	dlc5_set_trst,
	generic_get_trst
};
