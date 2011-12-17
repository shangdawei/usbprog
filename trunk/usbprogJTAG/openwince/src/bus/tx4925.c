/*
 * $Id: tx4925.c,v 1.1 2004/08/19 04:13:29 telka Exp $
 *
 * Copyright (C) 2003 RightHand Technologies, Inc.
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
 * Christian Pellegrin <chri@ascensit.com>, 2003.
 * Modified by Marcel Telka <marcel@telka.sk>, 2003.
 * Modified by Andrew Dyer <adyer@righthandtech.com>, 2003.
 *
 */

#include "sysdep.h"

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "part.h"
#include "bus.h"
#include "chain.h"
#include "bssignal.h"
#include "jtag.h"
#include "buses.h"

// FIXME board level write protect is ignored here
//  should be okay because pin isn't implemented
//  on 29LV200 we use now.

typedef struct {
	chain_t *chain;
	part_t *part;
    signal_t *oe;
    signal_t *swe;
    signal_t *romce[4];
    signal_t *sdcs[4];
    signal_t *addr[20];
    signal_t *data[16];
} bus_params_t;

#define	CHAIN	((bus_params_t *) bus->params)->chain
#define	PART	((bus_params_t *) bus->params)->part
#define	OE	    ((bus_params_t *) bus->params)->oe
#define	SWE  	((bus_params_t *) bus->params)->swe
#define	ROMCE	((bus_params_t *) bus->params)->romce
#define SDCS    ((bus_params_t *) bus->params)->sdcs
#define	ADDR	((bus_params_t *) bus->params)->addr
#define	DATA	((bus_params_t *) bus->params)->data



static void 
select_flash( bus_t *bus )
{
	part_t *p = PART;

	part_set_signal( p, ROMCE[0], 1, 0 );
	part_set_signal( p, ROMCE[1], 1, 1 );
	part_set_signal( p, ROMCE[2], 1, 1 );
	part_set_signal( p, ROMCE[3], 1, 1 );
	part_set_signal( p, SDCS[0],  1, 1 );
	part_set_signal( p, SDCS[1],  1, 1 );
	part_set_signal( p, SDCS[2],  1, 1 );
	part_set_signal( p, SDCS[3],  1, 1 );
}

static void 
unselect_flash( bus_t *bus )
{
	part_t *p = PART;

	part_set_signal( p, ROMCE[0], 1, 1 );
	part_set_signal( p, ROMCE[1], 1, 1 );
	part_set_signal( p, ROMCE[2], 1, 1 );
	part_set_signal( p, ROMCE[3], 1, 1 );
	part_set_signal( p, SDCS[0],  1, 1 );
	part_set_signal( p, SDCS[1],  1, 1 );
	part_set_signal( p, SDCS[2],  1, 1 );
	part_set_signal( p, SDCS[3],  1, 1 );
}


// the number of bytes wide that the TX4925
// CS0 signal is set to by the external
// config resistors on A13/A12 at reset
// 1, 2, or 4 are legal values

#define TX4925_FLASH_CS_WIDTH 2

static void
setup_address( bus_t *bus, uint32_t a )
{
	int i;
	part_t *p = PART;
	int addr_shift = (TX4925_FLASH_CS_WIDTH / 2);

	for (i = 0; i < 20; i++)
	  part_set_signal( p, ADDR[i], 1, (a >> (i+addr_shift)) & 1 );
}

static void
set_data_in( bus_t *bus )
{
	int i;
	part_t *p = PART;

	for (i = 0; i < 16; i++)
		part_set_signal( p, DATA[i], 0, 0 );
}

static void
setup_data( bus_t *bus, uint32_t d )
{
	int i;
	part_t *p = PART;

	for (i = 0; i < 16; i++)
		part_set_signal( p, DATA[i], 1, (d >> i) & 1 );
}

static void
tx4925_bus_printinfo( bus_t *bus )
{
	int i;

	for (i = 0; i < CHAIN->parts->len; i++)
		if (PART == CHAIN->parts->parts[i])
			break;
	printf( _("Toshiba TX4925 compatible bus driver via BSR (JTAG part No. %d)\n"), i );
}

static void
tx4925_bus_prepare( bus_t *bus )
{
	part_set_instruction( PART, "EXTEST" );
	chain_shift_instructions( CHAIN );
}

static void
tx4925_bus_read_start( bus_t *bus, uint32_t adr )
{
	part_t *p = PART;
	chain_t *chain = CHAIN;

	select_flash( bus );
	setup_address( bus, adr );
	part_set_signal( p, OE, 1, 0 );
	part_set_signal( p, SWE, 1, 1 );

	set_data_in( bus );

	chain_shift_data_registers( chain, 0 );
}

static uint32_t
tx4925_bus_read_next( bus_t *bus, uint32_t adr )
{
	part_t *p = PART;
	chain_t *chain = CHAIN;
	int i;
	uint32_t d = 0;

	setup_address( bus, adr );
	chain_shift_data_registers( chain, 1 );

	for (i = 0; i < 16; i++)
		d |= (uint32_t) (part_get_signal( p, DATA[i] ) << i);

	return d;
}

static uint32_t
tx4925_bus_read_end( bus_t *bus )
{
	part_t *p = PART;
	chain_t *chain = CHAIN;
	int i;
	uint32_t d = 0;

	unselect_flash( bus );
	part_set_signal( p, OE, 1, 1 );
	part_set_signal( p, SWE, 1, 1 );

	chain_shift_data_registers( chain, 1 );

	for (i = 0; i < 16; i++)
		d |= (uint32_t) (part_get_signal( p, DATA[i] ) << i);

	return d;
}

static uint32_t
tx4925_bus_read( bus_t *bus, uint32_t adr )
{
	tx4925_bus_read_start( bus, adr );
	return tx4925_bus_read_end( bus );
}

static void
tx4925_bus_write( bus_t *bus, uint32_t adr, uint32_t data )
{
	part_t *p = PART;
	chain_t *chain = CHAIN;

	select_flash( bus );
	part_set_signal( p, OE, 1, 1 );

	setup_address( bus, adr );
	setup_data( bus, data );

	chain_shift_data_registers( chain, 0 );

	part_set_signal( p, SWE, 1, 0 );
	chain_shift_data_registers( chain, 0 );
	part_set_signal( p, SWE, 1, 1 );
	unselect_flash( bus );
	chain_shift_data_registers( chain, 0 );
}

static int
tx4925_bus_area( bus_t *bus, uint32_t adr, bus_area_t *area )
{
	area->description = NULL;
	area->start = UINT32_C(0x00000000);
	area->length = UINT64_C(0x100000000);
	area->width = 16;

	return 0;
}

static void
tx4925_bus_free( bus_t *bus )
{
	free( bus->params );
	free( bus );
}

static bus_t *tx4925_bus_new( void );

const bus_driver_t tx4925_bus = {
	"tx4925",
	N_("Toshiba TX4925 compatible bus driver via BSR"),
	tx4925_bus_new,
	tx4925_bus_free,
	tx4925_bus_printinfo,
	tx4925_bus_prepare,
	tx4925_bus_area,
	tx4925_bus_read_start,
	tx4925_bus_read_next,
	tx4925_bus_read_end,
	tx4925_bus_read,
	tx4925_bus_write
};


static bus_t *
tx4925_bus_new( void )
{
	bus_t *bus;
	char buff[15];
	int i;
	int failed = 0;

	if (!chain || !chain->parts || chain->parts->len <= chain->active_part || chain->active_part < 0)
		return NULL;

	bus = malloc( sizeof (bus_t) );
	if (!bus)
		return NULL;

	bus->driver = &tx4925_bus;
	bus->params = malloc( sizeof (bus_params_t) );
	if (!bus->params) {
		free( bus );
		return NULL;
	}

	CHAIN = chain;
	PART = chain->parts->parts[chain->active_part];

	OE = part_find_signal( PART, "OE" );
	if (!OE) {
		printf( _("signal '%s' not found\n"), "OE" );
		failed = 1;
	}

	SWE = part_find_signal( PART, "SWE" );
	if (!SWE) {
		printf( _("signal '%s' not found\n"), "SWE" );
		failed = 1;
	}

	for (i = 0; i < 4; i++) {
		sprintf( buff, "ROMCE_%d", i );
		ROMCE[i] = part_find_signal( PART, buff );
		if (!ROMCE[i]) {
			printf( _("signal '%s' not found\n"), buff );
			failed = 1;
			break;
		}
	}

	for (i = 0; i < 4; i++) {
		sprintf( buff, "SDCS_%d", i );
		SDCS[i] = part_find_signal( PART, buff );
		if (!SDCS[i]) {
			printf( _("signal '%s' not found\n"), buff );
			failed = 1;
			break;
		}
	}

	for (i = 0; i < 20; i++) {
		sprintf( buff, "ADDR_%d", i );
		ADDR[i] = part_find_signal( PART, buff );
		if (!ADDR[i]) {
			printf( _("signal '%s' not found\n"), buff );
			failed = 1;
			break;
		}
	}

	for (i = 0; i < 16; i++) {
		sprintf( buff, "DATA_%d", i );
		DATA[i] = part_find_signal( PART, buff );
		if (!DATA[i]) {
			printf( _("signal '%s' not found\n"), buff );
			failed = 1;
			break;
		}
	}

	if (failed) {
		free( bus->params );
		free( bus );
		return NULL;
	}

	return bus;
}
