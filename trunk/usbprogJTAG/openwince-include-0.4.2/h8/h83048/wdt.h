/*
 * $Id: wdt.h,v 1.4 2005/09/29 11:13:32 telka Exp $
 *
 * H8/3048 WDT Registers
 * Copyright (C) 2005 Elcom s.r.o.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Written by Branislav Petrovsky <brano111@szm.sk>, 2005.
 *
 * Documentation:
 * [1] Renesas Technology Corp., "Hitachi Single-Chip Microcomputer
 *     H8/3048 Series, H8/3048F-ZTAT Hardware Manual",
 *     Rev. 6.0, 9/3/2002, Order Number: ADE-602-073E
 *
 */

#ifndef H83048_WDT_H
#define H83048_WDT_H

#include <openwince.h>

#ifndef __ASSEMBLY__
#include <stdint.h>
#endif

/* WDT registers */

#define WDT_BASE	0xffffa8

#ifndef __ASSEMBLY__
typedef volatile struct WDT_registers {
	union {
		union {
			uint16_t tcsr;
			uint16_t tcnt;
		} _write;
		struct {
			uint8_t tcsr;
			uint8_t tcnt;
		} _read;
	} _timer;
	union {
		union {
			uint16_t rstcsr;
		} _write;
		struct {
			uint8_t __reserved;
			uint8_t rstcsr;
		} _read;
	} _rstcsr;
} WDT_registers_t;

#define WDT_pointer	((WDT_registers_t*) WDT_BASE)

#define TCSR_r		WDT_pointer->_timer._read.tcsr
#define TCNT_r		WDT_pointer->_timer._read.tcnt
#define RSTCSR_r	WDT_pointer->_rstcsr._read.rstcsr
#define TCSR_w		WDT_pointer->_timer._write.tcsr
#define TCNT_w		WDT_pointer->_timer._write.tcnt
#define RSTCSR_w	WDT_pointer->_rstcsr._write.rstcsr
#endif /* __ASSEMBLY__ */

#define TCSR_OFFSET	0x00
#define TCNT_OFFSET_w	0x00
#define TCNT_OFFSET_r	0x01
#define RSTCSR_OFFSET_w	0x02
#define RSTCSR_OFFSET_r	0x03

/* TCSR bits */
#define TCSR_OVF	bit(7)
#define TCSR_WTIT	bit(6)
#define TCSR_TME	bit(5)
#define TCSR_CKS_MASK	bits(2,0)
#define TCSR_CKS(x)	bits_val(2,0,x)
#define get_TCSR_CKS(x)	bits_get(2,0,x)

/* RSTCSR bits */
#define RSTCSR_WRST	bit(7)
#define RSTCSR_RSTOE	bit(6)

#endif /* H83048_WDT_H */
