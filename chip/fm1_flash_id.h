/* SPDX-License-Identifier: MPL-2.0 */
/* The FM-1's flash, 1 MiB: Puya's P25Q80 (JEDEC 85 60 14) in the first units, a UC25HQ80 (B3 60 14) in newer ones.
 * Both take the plain SPI NOR commands of fm1_flash.h (9F 05 35 06 20 02 0B), and every store write is read back.
 * Any other id: RAM only, nothing written. Pure logic, no registers (tests/flash_id_test.c). */
#pragma once
#include <stdint.h>

#define FL_JEDEC_PUYA 0x856014u
#define FL_JEDEC_UC   0xB36014u
#define FL_JEDEC_OK(id) ((uint32_t)(id) == FL_JEDEC_PUYA || (uint32_t)(id) == FL_JEDEC_UC)
