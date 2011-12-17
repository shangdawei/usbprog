/*
 * $Id: flash.h,v 1.4 2003/08/27 09:13:16 telka Exp $
 *
 * Flash Memory interface
 * Copyright (C) 2003 AH
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
 * Written by August Hörandl <august.hoerandl@gmx.at>
 *
 */

#ifndef FLASH_H
#define	FLASH_H

#include <brux/flash.h>

extern flash_driver_t *flash_driver;
extern flash_driver_t *flash_drivers[];

#endif /* FLASH_H */
