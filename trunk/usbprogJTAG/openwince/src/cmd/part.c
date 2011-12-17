/*
 * $Id: part.c,v 1.2 2003/08/19 08:42:20 telka Exp $
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
#include <string.h>

#include "jtag.h"

#include "cmd.h"

static int
cmd_part_run( char *params[] )
{
	unsigned int n;

	if (cmd_params( params ) != 2)
		return -1;

	if (!cmd_test_cable())
		return 1;

	if (!chain->parts) {
		printf( _("Run \"detect\" first.\n") );
		return 1;
	}

	if (cmd_get_number( params[1], &n ))
		return -1;

	if (n >= chain->parts->len) {
		printf( _("%s: invalid part number\n"), "part" );
		return 1;
	}

	chain->active_part = n;

	return 1;
}

static void
cmd_part_help( void )
{
	printf( _(
		"Usage: %s PART\n"
		"Change active part for current JTAG chain.\n"
		"\n"
		"PART          part number\n"
	), "part" );
}

cmd_t cmd_part = {
	"part",
	N_("change active part for current JTAG chain"),
	cmd_part_help,
	cmd_part_run
};
