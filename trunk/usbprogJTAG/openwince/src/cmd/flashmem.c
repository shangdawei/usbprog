/*
 * $Id: flashmem.c,v 1.6 2003/09/15 17:31:46 telka Exp $
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
#include <stdint.h>
#include <string.h>

#include "jtag.h"
#include "flash.h"

#include "cmd.h"

static int
cmd_flashmem_run( char *params[] )
{
	int msbin;
	uint32_t adr = 0;
	FILE *f;

	if (cmd_params( params ) != 3)
		return -1;

	if (!bus) {
		printf( _("Error: Bus driver missing.\n") );
		return 1;
	}

	msbin = strcmp( "msbin", params[1] ) == 0;
	if (!msbin && cmd_get_number( params[1], &adr ))
			return -1;

	f = fopen( params[2], "r" );
	if (!f) {
		printf( _("Unable to open file `%s'!\n"), params[2] );
		return 1;
	}
	if (msbin)
		flashmsbin( bus, f );
	else
		flashmem( bus, f, adr );
	fclose( f );

	return 1;
}

static void
cmd_flashmem_help( void )
{
	int i;

	printf( _(
		"Usage: %s ADDR FILENAME\n"
		"Usage: %s FILENAME\n"
		"Program FILENAME content to flash memory.\n"
		"\n"
		"ADDR       target address for raw binary image\n"
		"FILENAME   name of the input file\n"
		"%-10s FILENAME is in MS .bin format (for WinCE)\n"
		"\n"
		"ADDR could be in decimal or hexadecimal (prefixed with 0x) form.\n"
		"\n"
		"Supported Flash Memories:\n"
	), "flashmem", "flashmem msbin", "msbin" );

	for (i = 0; flash_drivers[i]; i++)
		printf( _("%s\n     %s\n"), _(flash_drivers[i]->name), _(flash_drivers[i]->description) );
}

cmd_t cmd_flashmem = {
	"flashmem",
	N_("burn flash memory with data from a file"),
	cmd_flashmem_help,
	cmd_flashmem_run
};
