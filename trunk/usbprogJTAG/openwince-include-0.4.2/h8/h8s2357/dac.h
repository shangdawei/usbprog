/*
 * $Id: dac.h,v 1.3 2005/09/29 08:51:07 telka Exp $
 *
 * H8S/2357 DAC Registers
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
 * [1] Renesas Technology Corp., "Hitachi 16-Bit Single-chip Microcomputer
 *     H8S/2357 Series, H8S/2357F-ZTAT, H8S/2398F-ZTAT Hardware Manual",
 *     Rev. 5.0, 11/22/02, Order Number: ADE-602-146D
 *
 */

#ifndef H8S2357_DAC_H
#define H8S2357_DAC_H

#include <openwince.h>

#if LANGUAGE == C
#include <stdint.h>
#endif

/* DAC registers */

#define DAC_BASE	0xffffffa4

#if LANGUAGE == C
typedef volatile struct DAC_registers {
	uint8_t dadr0;
	uint8_t dadr1;
	uint8_t dacr;
} DAC_registers_t;

#define DAC_pointer	((DAC_registers_t*) DAC_BASE)

#define DADR0		DAC_pointer->dadr0
#define DADR1		DAC_pointer->dadr1
#define DACR		DAC_pointer->dacr
#endif /* LANGUAGE == C */

#define DADR0_OFFSET	0x00
#define DADR1_OFFSET	0x01
#define DACR_OFFSET	0x02

/* DACR bits */
#define DACR_DAOE1	bit(7)
#define DACR_DAOE0	bit(6)
#define DACR_DAE	bit(5)

#endif /* H8S2357_DAC_H */
