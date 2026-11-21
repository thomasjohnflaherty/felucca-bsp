/* SPDX-License-Identifier: MPL-2.0 */
/* SOFT0: the software interrupt of ILAT bit 0 (IRQ 120, FM1_IRQ_SOFT0) as a priority level of its own between
 * the audio render and TIMER5 (fm1-hapax u309, trail 40 D133: the 1 kHz app tick's body runs there, TIMER5 only
 * requests it). Not included by default: an app that asks for it (fm1-base APP_SOFT0) includes it once.
 *
 *   fm1_soft0_raise()   latch the request (ILAT_SET bit 0); the level-4 entry runs when nothing at 4 or above runs
 *   fm1_soft0_clear()   drop the latch (ILAT_CLR bit 0): the hardware does not clear it on entry (the latch stays
 *                       set until software clears it; the stock RTOS's context switch uses the same latch, bit 7 - cnum)
 *   FM1_SOFT0_ENTRY(name, body)   the entry wrapper, as fm1_isr.S's FM1_ISR: it saves psr, rets and reti (pushing
 *                       reti opens nesting, so a higher level, TIMER5, enters), r0-r3, calls body, restores, rti
 *
 * Unverified on silicon (u309: modeled by fm1-emu, both the 0.3.0 release and the upstream pin): that ILAT bit 0
 * raises vector 120, that the latch stays until cleared, and the 5 > 4 > 3 preemption. The device trial checks them. */
#pragma once
#include "fm1_irq.h"
static inline void fm1_soft0_raise(void) { FM1_ILAT_SET = 1u; }
static inline void fm1_soft0_clear(void) { FM1_ILAT_CLR = 1u; }
#define FM1_SOFT0_ENTRY(name, body)                                                                              \
    void body(void);                                                                                             \
    __asm__(".section .text." #name ",\"ax\",@progbits\n\t.globl " #name "\n" #name ":\n"                       \
            "\t[--sp] = {psr, rets, reti}\n\t[--sp] = {r3-r0}\n\tcall " #body "\n\t{r3-r0} = [sp++]\n"            \
            "\t{psr, rets, reti} = [sp++]\n\tcsync\n\trti\n\t.text\n");                                          \
    extern void name(void)
