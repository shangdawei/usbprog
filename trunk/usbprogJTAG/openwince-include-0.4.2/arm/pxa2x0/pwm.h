/*
 * $Id: pwm.h,v 1.12 2005/09/29 08:51:07 telka Exp $
 *
 * XScale PXA26x/PXA255/PXA250/PXA210 PWM0 and PWM1 Registers
 * Copyright (C) 2002, 2003 ETC s.r.o.
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
 * 3. Neither the name of the ETC s.r.o. nor the names of its contributors
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
 * Written by Marcel Telka <marcel@telka.sk>, 2002, 2003.
 *
 * Documentation:
 * [1] Intel Corporation, "Intel PXA250 and PXA210 Application Processors
 *     Developer's Manual", February 2002, Order Number: 278522-001
 * [2] Intel Corporation, "Intel PXA26x Processor Family Developer's Manual",
 *     March 2003, Order Number: 278638-002
 * [3] Intel Corporation, "Intel PXA255 Processor Developer's Manual"
 *     March 2003, Order Number: 278693-001
 *
 */

#ifndef	PXA2X0_PWM_H
#define	PXA2X0_PWM_H

#include <openwince.h>

#if LANGUAGE == C
#include <stdint.h>
#endif

#if defined(PXA2X0_NOPXA250) && !defined(PXA2X0_NOPXA255)
#define PXA2X0_NOPXA255
#endif

#if defined(PXA2X0_NOPXA255) && !defined(PXA2X0_NOPXA260)
#define PXA2X0_NOPXA260
#endif

/* PWM0 and PWM1 Registers */

#define	PWM0_BASE	0x40B00000
#define	PWM1_BASE	0x40C00000

#if LANGUAGE == C
typedef volatile struct PWM_registers {
	uint32_t pwm_ctrl;
	uint32_t pwm_pwduty;
	uint32_t pwm_perval;
} PWM_registers_t;

#ifdef PXA2X0_UNMAPPED
#define	PWM0_pointer	((PWM_registers_t*) PWM0_BASE)
#define	PWM1_pointer	((PWM_registers_t*) PWM1_BASE)
#endif

#define	PWM_CTRL	PWM_pointer->pwm_ctrl
#define	PWM_PWDUTY	PWM_pointer->pwm_pwduty
#define	PWM_PERVAL	PWM_pointer->pwm_perval

#define	PWM_CTRL0	PWM0_pointer->pwm_ctrl
#define	PWM_PWDUTY0	PWM0_pointer->pwm_pwduty
#define	PWM_PERVAL0	PWM0_pointer->pwm_perval

#define	PWM_CTRL1	PWM1_pointer->pwm_ctrl
#define	PWM_PWDUTY1	PWM1_pointer->pwm_pwduty
#define	PWM_PERVAL1	PWM1_pointer->pwm_perval
#endif /* LANGUAGE == C */

#define	PWM_CTRL_OFFSET		0x00
#define	PWM_PWDUTY_OFFSET	0x04
#define	PWM_PERVAL_OFFSET	0x08

/* PWM_CTRL bits - see Table 4-49 in [1], Table 4-50 in [2], Table 4-46 in [3] */

#define	PWM_CTRL_PWM_SD			bit(6)
#define	PWM_CTRL_PRESCALE_MASK		bits(5,0)
#define	PWM_CTRL_PRESCALE(x)		bits_val(5,0,x)
#define	get_PWM_CTRL_PRESCALE(x)	bits_get(5,0,x)

/* PWM_PWDUTY bits - see Table 4-50 in [1], Table 4-51 in [2], Table 4-47 in [3] */

#define	PWM_PWDUTY_FDCYCLE		bit(10)
#define	PWM_PWDUTY_DCYCLE_MASK		bits(9,0)
#define	PWM_PWDUTY_DCYCLE(x)		bits_val(9,0,x)
#define	get_PWM_PWDUTY_DCYCLE(x)	bits_get(9,0,x)

/* PWM_PERVAL bits - see Table 4-51 in [1], Table 4-52 in [2], Table 4-48 in [3] */

#define	PWM_PERVAL_PV_MASK		bits(9,0)
#define	PWM_PERVAL_PV(x)		bits_val(9,0,x)
#define	get_PWM_PERVAL_PV(x)		bits_get(9,0,x)

#endif /* PXA2X0_PWM_H */
