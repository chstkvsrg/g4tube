# AGENTS.md

## Overview

Geant4-based Monte Carlo X-ray tube simulation. Two independent C++17/CMake projects in one repo — they share no code.

| Project | Dir | Executable | Extra deps |
|---------|-----|-----------|------------|
| xtube | root | `xtube` | Geant4 only |
| corect | `CoreCT/` | `corect` | Geant4 + SQLite3 + gengetopt |

## Build

Both projects follow the standard CMake pattern, but each in its own build directory:

```bash
# xtube
mkdir build && cd build && cmake .. && make -j

# corect
cd CoreCT && mkdir build && cd build && cmake .. && make -j
```

**Geant4 11.4.2** is installed at `/opt/Geant4/Geant4-v11.4.2` (Arch `geant4-full` package; Qt6, MT, OpenGL). Both `CMakeLists.txt` use `find_package(Geant4 HINTS /opt/Geant4/Geant4-v11.4.2/lib/cmake/Geant4)`, so a bare `cmake ..` finds it — no `-DGeant4_DIR` needed. Geant4 11 requires C++17 (`CMAKE_CXX_STANDARD 17` is set in both projects).

**Datasets** are *not* in `share/Geant4/data` — the Arch package keeps them in `/opt/Geant4/Libraries/<Dataset><version>/` (~3.3 GB: `G4NDL4.7.1`, `G4EMLOW8.8`, `G4TENDL1.4`, `G4URRPT1.1`, `RealSurface2.2`, `G4NUDEXLIB1.0`, `G4CHANNELING2.0`, plus level-gamma/radioactive/particle-HP/etc.). `geant4.sh` only sets `GEANT4_DATA_DIR` and leaves every `G4*DATA` export commented out, so `~/.bashrc` sets all 15 of them explicitly. New terminals work out of the box; a stale env (e.g. a leftover 10.7.3) breaks init with `G4Exception em0003` / `G4AugerData::LoadData` failures — fix by opening a new terminal or:

```bash
source /opt/Geant4/Geant4-v11.4.2/bin/geant4.sh
```

Neither project's current physics list reads external data (xtube uses Livermore EM, which is self-contained), so missing `G4*DATA` shows up only when switching to `option4`/Auger/hadronic models. To sanity-check a dataset env, build and run Geant4's own `TestEm11` (option4 + Auger + EMLOW) with `-DGeant4_DIR=/opt/Geant4/Geant4-v11.4.2/lib/cmake/Geant4`; a bogus `G4LEDATA` should abort with `em0003`.

**CoreCT requires `gengetopt`** — it generates CLI parser code from `CoreCT/corect.ggo` at build time. It is **not currently installed** on this machine, so the CoreCT build fails at configure; install with `pacman -S gengetopt` (this is Arch/Omarchy, not apt).

## Running simulations

- **xtube**: `./xtube` (interactive) or `./xtube run.mac` (batch). Outputs timestamped `h1_*.csv` histograms.
- **corect**: `./corect -m runct.mac -a <angle>` for single angle. Full CT scan: `CoreCT/runct.sh` (runs 0–359°).
- **corect CLI options**: `-t CorePhantom|CoreVoxel`, `-o <output-dir>`, `-n <cores>`, `-h` (holder), `-d` (distributed mode).

Run xtube from the **repo root**: `run.mac` does `/control/execute detectors.mac` with a relative path, and histograms land in the CWD. `h1_*.csv` is not in `.gitignore`, so runs dirty the worktree — delete them or ignore them.

Macro files (`.mac`) are Geant4 command scripts, not Makefiles.

## Repository structure

```
xtube.cpp          — xtube main()
src/               — xtube sources (.cpp)
include/           — xtube headers (.hh)
CoreCT/
  corect.cc        — corect main()
  corect.ggo       — gengetopt CLI definition (generates code at build)
  src/             — corect sources (.cc)
  include/         — corect headers (.hh)
    targets/       — CorePhantom, CoreVoxel geometry
    distributed/   — TCP/UDP compute cluster layer (port 1532)
  INPUT/           — binary phantom data (.ml2g4) + Spectrum.txt
  runct.sh         — full CT angle-sweep driver (0–359°)
  cmake/           — FindGengetopt.cmake, FindSQLite3.cmake
```

## Code conventions

- **Headers**: `.hh` (both projects). **Sources**: `.cpp` (xtube), `.cc` (CoreCT).
- **Include guards**: `#pragma once` in CoreCT; classic `#ifndef` in xtube.
- **Member prefix**: `f` (Geant4 convention), e.g. `fParticleGun`, `fMother`.
- **Singletons**: static `p_` pointer + `instance()` / `free()` pattern.
- **Physics list**: custom `G4VUserPhysicsList` with Livermore EM models, cut `0.0001*mm`.
- **MT**: xtube uses `G4MTRunManager` with **24 threads hardcoded**.
- **RNG**: `CLHEP::RanecuEngine` seeded from `time(NULL)` — results are non-reproducible.

## Gotchas

- CoreCT's `TargetsManager::setTarget()` maps `CoreVoxel` to `CorePhantomHist` — appears to be a bug.
- CoreCT's `CoreVoxel` target **segfaults** (pre-existing, not a Geant4 11.x regression): the base `DetectorConstruction::Construct()` has `ConstructCore(...)` commented out (`src/DetectorConstruction.cc:85`), so the `VoxelsContainer` geometry is never built, and `CoreVoxelPrimaryGeneratorAction::ComputeDirection()` dereferences a NULL store lookup. The target also hardcodes `/home/xinady/data/...` file paths from another machine (`CoreVoxelDetectorConstruction.cc:29-31`) instead of the `INPUT/` convention. CorePhantom (default target) is verified working.
- CoreCT has extensive dead code behind `#if 0` blocks in `corect.cc` (SQLite-backed dispatcher variant).
- Comments are a mix of Russian and English.
- No tests, no CI, no linter, no formatter — all verification is manual.
- `CoreCT/INPUT/*.ml2g4` are raw binary voxel files read with `std::ifstream` — platform/endian sensitive.
- Distributed mode (`-d`) uses raw TCP sockets with length-prefixed framing; not production hardened.
