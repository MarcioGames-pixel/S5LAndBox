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
   The first host-side mapping helper is now in `display_profile.h/.c`: it maps
   an in-bounds logical pixel to the corresponding top-left physical panel pixel
   using 64-bit intermediate arithmetic. A reverse mapping now converts physical
   touch coordinates back into logical guest coordinates, including the 2×
   iPod touch 4 panel. Tests cover both directions, 1× iPhone 3G, 2× iPod touch
   4, and invalid coordinates. This does not change guest CLCD scanout or claim
   that the A4 display controller is emulated.
4. **SoC boundary** — isolate machine creation, MMIO map, interrupt controller,
   clocks, RAM layout, and boot-ROM expectations behind a SoC-specific interface.
5. **S5L8930/A4 implementation** — add the ARMv7 CPU/system-control behavior and
   the A4 peripheral map required by actual iPod4,1 firmware. Reuse devices only
   after register-level compatibility is demonstrated. As CPU groundwork, the
   interpreter now distinguishes the existing ARM1176, the Apple A4's Cortex-A8,
   and the later Swift core without changing Swift's existing enum value. MOVW/
   MOVT are enabled for Cortex-A8, while SDIV/UDIV remain unavailable there (the
   A4 Cortex-A8 does not implement those divide instructions). Focused regression
   tests guard this distinction; this is not yet a complete ARMv7/Thumb-2 core.
6. **Boot milestones and CI** — add deterministic tests for firmware identity,
   CPU reset state, memory map, interrupt delivery, display output, and finally
   a captured boot-to-SpringBoard test using legally obtained firmware supplied
   outside the repository.

## First S5L8930 platform scaffold

`core/include/s5l8930.h` and `core/src/soc/s5l8930.c` now provide a separate A4 platform-state type and a provisional memory bus: configurable DRAM, an optional read-only boot-ROM image, little-endian 8/16/32-bit accesses, explicit unmapped/no-backing/read-only errors, overflow/bounds checks, and reset. Caller-owned backing buffers are never allocated or freed by the module. The `s5l8930_platform_map` test covers invalid ranges, boundary accesses, endianness, read-only ROM, unmapped access, overflow, and reset. The address constants remain provisional and must be verified against iPod4,1 device-tree/boot-ROM evidence before being treated as authoritative. This module is deliberately not dispatched as a bootable machine yet: it does not emulate A4 MMIO, interrupt delivery, clock/reset control, CPU reset state, or boot ROM execution.

## Current limitations

- This change does not add S5L8930 emulation, ARMv7 boot support, or a working
  iPod4,1 firmware importer. The boot guard is a safety check, not a substitute
  for implementing the A4 machine.
- It does not change the current emulator engine or UI selection flow.
- A successful build or profile lookup is not evidence of an iPod boot.
- No Apple firmware, keys, or copyrighted system files are added to the repo.


### Explicit S5L8930 scaffold selection

The machine factory now exposes `s5l_machine_init_s5l8930_scaffold_for_device()`
and a firmware-aware counterpart. These APIs initialize the separate A4 platform
state for a matching S5L8930/ARMv7 profile and validate the board ID when
firmware identity is supplied. They do not change the existing S5L8900 dispatch,
do not allocate guest RAM, and do not mark the profile bootable. They are an
integration seam for subsequent CPU/bus work, not an iOS boot implementation.
