/*
 * $Id: au1500.c,v 1.1 2003/12/20 15:19:27 telka Exp $
 *
 * Copyright (C) 2003 BLXCPU co. Ltd.
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
 * Written by ZHANG WEI <zwblue@sohu.com>, 2003
 *
 * Documentation:
 * [1] AMD, "AMD Alchemy Solutions AU1500 Processor Data Book - 
 *     Preliminary Information", June 2003, Publication ID: 30361B
 *
 */


#include "sysdep.h"

#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include "part.h"
#include "bus.h"
#include "chain.h"
#include "bssignal.h"
#include "jtag.h"
#include "buses.h"

typedef struct{
	chain_t *chain;
	part_t *part;
	signal_t *rad[32];
	signal_t *nrcs[4];
	signal_t *nrwe;
	signal_t *nroe;
	signal_t *rd[32];
} bus_params_t;

#define CHAIN   ((bus_params_t *) bus->params)->chain
#define PART    ((bus_params_t *) bus->params)->part

#define RAD ((bus_params_t *) bus->params)->rad
#define nRCS ((bus_params_t *) bus->params)->nrcs
#define nRWE ((bus_params_t *) bus->params)->nrwe
#define nROE ((bus_params_t *) bus->params)->nroe
#define RD ((bus_params_t *) bus->params)->rd

static void
setup_address( bus_t *bus, uint32_t a)
{
	int i;
	part_t *p = PART;

	for( i = 0; i < 32; i++)
		part_set_signal( p, RAD[i], 1, (a >>i) & 1);
}

static int au1500_bus_area(bus_t * bus, uint32_t addr, bus_area_t * area);

static void
set_data_in( bus_t *bus )
{
	int i;
	part_t *p = PART;
	bus_area_t area;

	au1500_bus_area( bus, 0, &area);

	for( i = 0; i < area.width; i++ )
		part_set_signal( p, RD[i], 0, 0 );
	
}

static uint32_t
get_data_out( bus_t *bus )
{
	int i;
	part_t *p = PART;
	bus_area_t area;
	uint32_t d = 0;

	au1500_bus_area( bus, 0, &area);

	for( i = 0; i < area.width; i++ )
		d |= (uint32_t)(part_get_signal( p, RD[i] ) << i);

	return d;	
}

static void
setup_data( bus_t *bus, uint32_t d)
{
	int i;
	part_t *p = PART;
	bus_area_t area;

	au1500_bus_area( bus, 0, &area);

	for( i = 0; i < area.width; i++ )
		part_set_signal( p, RD[i], 1, ( d>>i ) & 1 );
}

static bus_t *au1500_bus_new(void)
{
	bus_t *bus;
	char buff[10];
	int i;
	int failed = 0;

	
	if (!chain || !chain->parts || chain->parts->len <= chain->active_part || chain->active_part < 0)
		return NULL;

	bus = malloc( sizeof (bus_t) );
	if (!bus)
		return NULL;
		
	bus->driver  = &au1500_bus;
	bus->params = calloc( 1, sizeof(bus_params_t) );
	if (!bus->params){
		free(bus);
		return NULL;
	}

	CHAIN = chain;
	PART = chain->parts->parts[chain->active_part];

	for(i=0; i<32; i++){
		sprintf( buff, "RAD%d", i);
		RAD[i] = part_find_signal( PART, buff);
		if(!RAD[i]){
			printf( _("signal '%s' not found\n"), buff );
			failed =1;
			break;
		}
	}

	for(i=0; i<4; i++){
		sprintf( buff, "RCE_N%d", i);
		nRCS[i] = part_find_signal( PART, buff);
		if(!nRCS[i]){
			printf( _("signal '%s' not found\n"), buff );
			failed =1;
			break;
		}
	}


	nRWE = part_find_signal( PART, "RWE_N" );
	if (!nRWE) {
		printf( _("signal '%s' not found\n"), "RWE_N" );
		failed = 1;
	}

	nROE = part_find_signal( PART, "ROE_N" );
	if (!nROE) {
		printf( _("signal '%s' not found\n"), "ROE_N" );
		failed = 1;
	}

	for(i=0; i<32; i++){
		sprintf( buff, "RD%d", i);
		RD[i] = part_find_signal( PART, buff);
		if(!RD[i]){
			printf( _("signal '%s' not found\n"), buff);
			failed =1;
			break;
		}
	}

	if (failed) {
		free( bus->params );
		free ( bus );
		return NULL;
	}

	return bus;
 		
}

static int 
au1500_bus_area(bus_t *bus, uint32_t addr, bus_area_t *area)
{
	area->description = NULL;
	area->start = UINT32_C(0x00000000);
	area->length = UINT64_C(0x00100000000);
//	area->width = 16;
	area->width = part_get_signal( PART, part_find_signal( PART, "ROMSIZ" ) ) ? 16 : 32;


	return 0;
	
}

static void
au1500_bus_free( bus_t *bus )
{
	free( bus->params );
	free ( bus );
}

static void
au1500_bus_printinfo( bus_t *bus)
{
	int i;

	for (i = 0; i < CHAIN->parts->len; i++)
		if (PART == CHAIN->parts->parts[i])
			break;
	printf( _("AU1500 compatible bus driver via BSR (JTAG part No. %d)\n"), i );
}

static void
au1500_bus_prepare( bus_t *bus )
{
	part_set_instruction( PART, "EXTEST" );
	chain_shift_instructions( CHAIN );
}

static void 
au1500_bus_read_start( bus_t *bus, uint32_t adr )
{
	part_t *p = PART;
	chain_t *chain = CHAIN;

	part_set_signal( p, nRCS[0], 1, 0 );
	part_set_signal( p, nRCS[1], 1, 1 );
	part_set_signal( p, nRCS[2], 1, 1 );
	part_set_signal( p, nRCS[3], 1, 1 );
	part_set_signal( p, nRWE, 1, 1 );
	part_set_signal( p, nROE, 1, 0);

	setup_address( bus, adr);
	set_data_in( bus );

	chain_shift_data_registers( chain, 0 );
}

static uint32_t
au1500_bus_read_next( bus_t *bus, uint32_t adr )
{
	chain_t *chain = CHAIN;

	setup_address( bus, adr );
	chain_shift_data_registers( chain, 1 );

	return get_data_out( bus );
}

static uint32_t
au1500_bus_read_end( bus_t *bus )
{
	part_t *p = PART;
	chain_t *chain = CHAIN;

	part_set_signal( p, nRCS[0], 1, 1 );
	part_set_signal( p, nRCS[1], 1, 1 );
	part_set_signal( p, nRCS[2], 1, 1 );
	part_set_signal( p, nRCS[3], 1, 1 );
	part_set_signal( p, nRWE, 1, 1 );
	part_set_signal( p, nROE, 1, 1 );

	chain_shift_data_registers( chain, 1 );

	return get_data_out( bus );
}

static uint32_t
au1500_bus_read( bus_t *bus, uint32_t adr )
{
	au1500_bus_read_start( bus, adr );
	return au1500_bus_read_end( bus );
}

static void
au1500_bus_write( bus_t *bus, uint32_t adr, uint32_t data )
{
	part_t *p = PART;
	chain_t *chain = CHAIN;

	part_set_signal( p, nRCS[0], 1, 0 );
	part_set_signal( p, nRCS[1], 1, 1 );
	part_set_signal( p, nRCS[2], 1, 1 );
	part_set_signal( p, nRCS[3], 1, 1 );
	part_set_signal( p, nRWE, 1, 1 );
	part_set_signal( p, nROE, 1, 1 );

	setup_address( bus, adr );
	setup_data( bus, data );

	chain_shift_data_registers( chain, 0 );

	part_set_signal( p, nRWE, 1, 0 );
	chain_shift_data_registers( chain, 0 );

	part_set_signal( p, nRWE, 1, 1);
	part_set_signal( p, nROE, 1, 1);
	part_set_signal( p, nRCS[0], 1, 1);
	part_set_signal( p, nRCS[1], 1, 1 );
	part_set_signal( p, nRCS[2], 1, 1 );
	part_set_signal( p, nRCS[3], 1, 1 );

	chain_shift_data_registers( chain, 0);
}

const bus_driver_t au1500_bus = {
	"au1500",
	N_("AU1500 BUS Driver via BSR"),
	au1500_bus_new,
	au1500_bus_free,
	au1500_bus_printinfo,
	au1500_bus_prepare,
	au1500_bus_area,
	au1500_bus_read_start,
	au1500_bus_read_next,
	au1500_bus_read_end,
	au1500_bus_read,
	au1500_bus_write
};

