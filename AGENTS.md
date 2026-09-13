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

**Geant4 11.4.2** is installed at `/home/user/geant4/11.4.2` (Qt6, MT, datasets in `share/Geant4/data`). Both `CMakeLists.txt` use `find_package(Geant4 HINTS /home/user/geant4/11.4.2/lib/cmake/Geant4)`. Geant4 11 requires C++17 (`CMAKE_CXX_STANDARD 17` is set in both projects).

**Before running**, source the env script so the `G4*DATA` variables point to the 11.4.2 datasets (a stale system 10.7.3 install exports old values that break init with `G4AugerData::LoadData` failures):

```bash
source /home/user/geant4/11.4.2/bin/geant4.sh
```

**CoreCT requires `gengetopt`** — it generates CLI parser code from `CoreCT/corect.ggo` at build time. If missing, install it (`apt install gengetopt`) or the build fails with a CMake error.

## Running simulations

- **xtube**: `./xtube` (interactive) or `./xtube run.mac` (batch). Outputs timestamped `h1_*.csv` histograms.
- **corect**: `./corect -m runct.mac -a <angle>` for single angle. Full CT scan: `CoreCT/runct.sh` (runs 0–359°).
- **corect CLI options**: `-t CorePhantom|CoreVoxel`, `-o <output-dir>`, `-n <cores>`, `-h` (holder), `-d` (distributed mode).

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
