/*
 * $Id: initbus.c,v 1.1 2003/09/05 21:09:14 telka Exp $
 *
 * Copyright (C) 2003 ETC s.r.o.
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
 * Written by Marcel Telka <marcel@telka.sk>, 2003.
 *
 */

#include "sysdep.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cmd.h"
#include "jtag.h"

static int
cmd_initbus_run( char *params[] )
{
	int i;

	if (cmd_params( params ) != 2)
		return -1;

	if (!cmd_test_cable())
		return 1;

	if (!chain->parts) {
		printf( _("Run \"detect\" first.\n") );
		return 1;
	}

	if (chain->active_part >= chain->parts->len) {
		printf( _("%s: no active part\n"), "initbus" );
		return 1;
	}

	for (i = 0; bus_drivers[i] != NULL; i++) {
		if (strcmp( bus_drivers[i]->name, params[1] ) == 0) {
			bus_t *bus = bus_drivers[i]->new_bus();
			if (bus == NULL) {
				printf( _("bus initialization failed!\n") );
				return 1;
			}
			buses_add( bus );
			return 1;
		}
	}

	printf( _("Unknown bus: %s\n"), params[1] );

	return 1;
}

static void
cmd_initbus_help( void )
{
	int i;

	printf( _(
		"Usage: %s BUSNAME\n"
		"Initialize new bus driver for active part.\n"
		"\n"
		"BUSNAME       Name of the bus\n"
		"\n"
		"List of available buses:\n"
	), "initbus" );

	for (i = 0; bus_drivers[i] != NULL; i++)
		printf( _("%-10s %s\n"), bus_drivers[i]->name, bus_drivers[i]->description );
}

const cmd_t cmd_initbus = {
	"initbus",
	N_("initialize bus driver for active part"),
	cmd_initbus_help,
	cmd_initbus_run
};
