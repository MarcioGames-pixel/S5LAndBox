# Multi-device bring-up: iPhone1,2 and iPod4,1

This is the first step toward a multi-device architecture, not a claim that both
devices boot today. The native machine core in this repository is currently an
S5L8900 model, and firmware import/boot policy is tailored to iPhone1,2.

## Initial hardware catalog

| Identifier | Board | SoC family | CPU family | Logical display | Physical display | Boot status |
| --- | --- | --- | --- | --- | --- | --- |
| `iPhone1,2` | `N82AP` | Samsung S5L8900 | ARMv6 | 320×480 | 320×480 | Existing machine |
| `iPod4,1` | `N81AP` | Apple A4 / S5L8930 | ARMv7 | 320×480 | 640×960 | Not implemented |

The registry is in `core/include/device_profile.h` and
`core/src/soc/device_profile.c`. The bring-up request now accepts an optional
`device_identifier`. When supplied, the core resolves it against this registry
before touching guest memory. Unknown targets and known-but-unimplemented SoCs
are refused with `device-unsupported`; `iPod4,1` therefore cannot accidentally
enter the S5L8900/ARMv6 boot path. The capability predicate requires both
S5L8900 and ARMv6, so a profile that accidentally combines S5L8900 with ARMv7
is rejected too. Regression tests cover the supported iPhone identity, the
unsupported iPod identity, an unknown identifier, and the mismatched CPU family. A null identifier preserves the existing
legacy callers while the Android UI is migrated to explicit target selection.

The core now also has an explicit machine-selection boundary in
`core/include/machine_factory.h` and `core/src/soc/machine_factory.c`.
Callers can resolve a selected identifier before constructing a machine; the
API distinguishes invalid input, unknown devices, and known profiles whose SoC
backend is not implemented. The `s5l_machine_init_for_device()` entry point now
performs that gate and invokes `s5l8900_init()` only for a supported profile,
so a frontend can no longer accidentally send an iPod A4 target through the
S5L8900 initializer. It returns no profile on refusal. The CTest target covers
the selection outcomes and invalid initialization arguments.

## Implementation sequence

1. **Profile registry and boot guard** — catalog the targets and reject an
   explicitly selected unsupported SoC before guest memory is modified. The
   iPhone1,2 path remains the legacy default until the frontend passes an ID.
2. **Firmware-aware machine selection** — detect the product/board identity
   from a user-imported IPSW manifest and reject mismatches with a clear error.
   Do not silently feed iPod firmware to the S5L8900 core.
3. **Display/input abstraction** — separate logical touch coordinates from
   physical panel dimensions and keep the existing 320×480 iPhone path unchanged.
4. **SoC boundary** — isolate machine creation, MMIO map, interrupt controller,
   clocks, RAM layout, and boot-ROM expectations behind a SoC-specific interface.
5. **S5L8930/A4 implementation** — add the ARMv7 CPU/system-control behavior and
   the A4 peripheral map required by actual iPod4,1 firmware. Reuse devices only
   after register-level compatibility is demonstrated.
6. **Boot milestones and CI** — add deterministic tests for firmware identity,
   CPU reset state, memory map, interrupt delivery, display output, and finally
   a captured boot-to-SpringBoard test using legally obtained firmware supplied
   outside the repository.

## Current limitations

- This change does not add S5L8930 emulation, ARMv7 boot support, or a working
  iPod4,1 firmware importer. The boot guard is a safety check, not a substitute
  for implementing the A4 machine.
- It does not change the current emulator engine or UI selection flow.
- A successful build or profile lookup is not evidence of an iPod boot.
- No Apple firmware, keys, or copyrighted system files are added to the repo.
