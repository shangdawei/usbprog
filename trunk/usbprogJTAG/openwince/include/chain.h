/*
 * $Id: chain.h,v 1.4 2003/08/11 09:15:07 telka Exp $
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

#ifndef CHAIN_H
#define	CHAIN_H

#include "part.h"

typedef struct chain_t chain_t;

#include "cable.h"

struct chain_t {
	int state;
	parts_t *parts;
	int active_part;
	cable_t *cable;
};

chain_t *chain_alloc( void );
void chain_free( chain_t *chain );
void chain_disconnect( chain_t *chain );
void chain_clock( chain_t *chain, int tms, int tdi );
int chain_set_trst( chain_t *chain, int trst );
int chain_get_trst( chain_t *chain );
void chain_shift_instructions( chain_t *chain );
void chain_shift_data_registers( chain_t *chain, int capture_output );

typedef struct {
	chain_t **chains;
	int size;			/* allocated chains array size */
} chains_t;

#endif /* CHAIN_H */
