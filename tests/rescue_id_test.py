#!/usr/bin/env python3
# SPDX-License-Identifier: MPL-2.0
"""tools/fm1_rescue.py's chip gate: both 1 MiB flash parts of the FM-1 (856014, B36014) pass it and the whole
flash is read next; any other id or chip key stops before a read or a write."""
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
import fm1_rescue as R  # noqa: E402


class Passed(Exception):
    pass


class Dev:
    def __init__(self, key, typ, fid):
        self.id = (key, typ, fid)
        self.reads = 0

    def inquiry(self):
        return "WL82", "UBOOT"

    def start_loader(self, loader):
        pass

    def chip(self):
        return self.id

    def read(self, addr, n):
        self.reads += 1
        raise Passed()                      # the gate let it through: the backup read is next


fails = 0
for fid, ok in ((0x856014, True), (0xB36014, True), (0xEF4014, False), (0x856015, False), (0, False),
                (0xFFFFFF, False)):
    for key, typ in ((0x980F, 3), (0x1234, 3), (0x980F, 2)):
        want = ok and (key, typ) == (0x980F, 3)
        d = Dev(key, typ, fid)
        try:
            R.rescue(d, b"", b"", write=False, backup_dir="/nonexistent", say=lambda *_: None)
            got = "returned"
        except Passed:
            got = "read"
        except R.Fail:
            got = "refused"
        if (got == "read") != want or (not want and d.reads):
            print(f"FAIL key {key:04X} type {typ} id {fid:06X}: {got}")
            fails += 1
print("rescue_id_test: ok" if not fails else f"rescue_id_test: {fails} failed")
sys.exit(fails != 0)
