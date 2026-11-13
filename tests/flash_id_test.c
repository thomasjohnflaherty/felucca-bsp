/* SPDX-License-Identifier: MPL-2.0 */
/* Host test of chip/fm1_flash_id.h: the boot gate (flash_ok) takes both 1 MiB parts of the FM-1 and nothing else. */
#include <stdio.h>
#include "fm1_flash_id.h"

static int fails;
#define CHECK(c) do { if (!(c)) { printf("FAIL line %d: %s\n", __LINE__, #c); fails++; } } while (0)

int main(void)
{
    CHECK(FL_JEDEC_OK(0x856014u));      /* Puya P25Q80, the first units */
    CHECK(FL_JEDEC_OK(0xB36014u));      /* UC25HQ80, newer units */
    CHECK(!FL_JEDEC_OK(0));             /* no answer */
    CHECK(!FL_JEDEC_OK(0xFFFFFFu));     /* bus floating */
    CHECK(!FL_JEDEC_OK(0x856015u));     /* a 2 MiB Puya: another layout, not this gate's job */
    CHECK(!FL_JEDEC_OK(0xB36015u));
    CHECK(!FL_JEDEC_OK(0xEF4014u));     /* a Winbond 1 MiB: not seen in an FM-1 */
    CHECK(!FL_JEDEC_OK(0x85601400u));   /* a 4-byte read is not a match */
    CHECK(!FL_JEDEC_OK(0x1B36014u));    /* a 25-bit value: only the low 24 bits are the id */
    for (unsigned id = 0; id < 0x1000000u; id++)    /* every 24-bit id: exactly two pass */
        if (FL_JEDEC_OK(id) != (id == 0x856014u || id == 0xB36014u)) { CHECK(0); break; }
    if (!fails) printf("flash_id_test: ok\n");
    return fails != 0;
}
