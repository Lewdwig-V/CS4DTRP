# cubemake — Cubic Simultaneous 4-Day Build Tool

## Overview

`cubemake` is a C11 build tool that replaces GNU Make for the CS4DTRP project. It enforces the compilation requirements specified in RFC 4444 (CS4DTRP), including rejection of `<time.h>`, four-corner simultaneous compilation, SIMACK verification before linking, and RFC-compliant error diagnostics with remediation advice.

The tool is a single C source file (`tools/cubemake.c`) bootstrapped by a minimal Makefile target. After bootstrapping, `cubemake` handles all builds. The Makefile remains as a bootstrap artifact — the belly button of the build system.

## File Layout

```
tools/cubemake.c    — The Cubic build tool (single file, bootstrapped once)
Makefile            — Retains existing targets; adds cubemake bootstrap target
```

The Makefile adds:

```makefile
cubemake: tools/cubemake.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@
```

## Build Phases

### Phase 1: Scanning for Linear Aggression

Before any compilation, `cubemake` scans all `.c` and `.h` files under `src/` and `include/` for violations.

**`<time.h>` detection:**

Scans for `#include <time.h>` and `#include "time.h"`. If found:

```
LINEAR AGGRESSION DETECTED: <time.h> found in src/foo.c
Word Animals not permitted in this translation unit.
Remedy: Remove <time.h> and replace with cubic_simultaneity.h
        when it becomes available. Until then, do not use
        single-corner temporal interfaces.
```

Exit code: 44 (`ECUBELESS`).

**Optimization level enforcement:**

`cubemake` accepts an optional `-O` flag. Default is `-O4`.

| Flag | Action | Diagnostic |
|------|--------|------------|
| `-O0` | Allowed with warning | "Produces Educated Stupid binaries" |
| `-O1` | Refused | `IMMEDIATE_BRAIN_ROT_ERROR: Linear optimisation is Mental Murder of machine code.` |
| `-O2` | Refused | "Produces Linear Trash binaries that will be dropped by any Cubic-Aware router." |
| `-O3` | Refused | "Produces DEGRADED binaries. Use -O4." |
| `-O4` | Accepted | Mapped to gcc `-O2` under the hood |
| `-O5` | Refused | `FIFTH_CORNER_ASSERTED at compilation level. A compiler cannot optimise a corner that does not exist. Seek degaussing.` |

### Phase 2: Corner Affinity Assignment

Each file in the project is assigned a Corner Affinity using a deterministic hash (DJB2) of the filename modulo 4:

```
Corner 0 (MIDNIGHT) — hash % 4 == 0
Corner 1 (SUNRISE)  — hash % 4 == 1
Corner 2 (NOON)     — hash % 4 == 2
Corner 3 (SUNSET)   — hash % 4 == 3
```

These indices match the library's `cs4dtrp_corner_t` enum (MIDNIGHT=0, SUNRISE=1, NOON=2, SUNSET=3).

The hash function is DJB2 (`hash = hash * 33 + c`, seeded at 5381) applied to the file's relative path from the project root (e.g., `src/cs4dtrp.c`).

All files are reported, including non-compiled files like the Makefile (labeled "bootstrap artifact, tolerated") and the README. Corner assignments are stable across builds.

The `tools/` directory is intentionally excluded from `<time.h>` scanning — `cubemake.c` itself may use POSIX headers as needed. The belly button is tolerated, not inspected.

### Phase 3: Four-Corner Simultaneous Compilation

Each `.c` source file is compiled four times simultaneously — once per corner. For each source file, `cubemake` forks exactly 4 child processes using `fork()` and `exec()`, waits for all four to complete, then proceeds to the next source file. With N source files, this produces N sequential batches of 4 simultaneous compilations.

Each corner compilation:
- Invokes `gcc` with the real flags (`-std=c11 -Wall -Wextra -Wpedantic -Iinclude` plus the mapped optimization level)
- Is announced with its corner name in neon green ANSI color
- Produces an object file in the same directory as the source, suffixed with the corner index (e.g., `src/cs4dtrp.corner0.o`, `src/cs4dtrp.corner1.o`)

Note: `-O4` is mapped to gcc's `-O2` under the hood. The number 4 is Cubic; the optimization quality of gcc's `-O2` happens to be acceptable. The problem with requesting `-O2` by name is the number, not the optimization — two is a line, not a cube.

### Phase 4: SIMACK Verification

A mandatory Simultaneity Acknowledgement check before linking. Cannot be bypassed, cached, or delegated.

The SIMACK verifies:
1. All four corner object files exist for each source file
2. All four corner compilations exited with status 0
3. All four object files are byte-identical (no corner drift)

**Output on success:**

```
[CUBIC] SIMACK verification...
  MIDNIGHT ✓  SUNRISE  ✓  NOON     ✓  SUNSET   ✓
  Simultaneity confirmed. All four corners present.
```

**Failure modes (per RFC):**

| Corners OK | Classification | Code | Action |
|------------|---------------|------|--------|
| 0 | `VOID_CUBIC_BRAIN` | `0x0CB0` | Terminal. Exit 44. |
| 1-2 | `LINEAR_TRASH` | — | "Partial compilation is a line, not a cube." Exit 44. |
| 3 | `PARTIAL_AWARENESS` | `0x0CB1` | Always proceed DEGRADED with warning (no operator discretion). Missing corner logged. Binary flagged as 75% Cubic. Note: the RFC says "MAY proceed at initiator's discretion"; cubemake always proceeds because nondeterministic build behavior is a Linear concept. |
| 4 (diverged) | `CORNER_DRIFT_DETECTED` | — | "Your hardware is suffering Linear Regression. Remedy: degauss CPU." Exit 44. |

After SIMACK passes, the lowest-numbered successful corner's object files are used for linking (normally corner 0 / MIDNIGHT; in DEGRADED mode, the first available corner). The other corner sets are cleaned up.

### Phase 5: Linking

Invokes `ar rcs` to create `libcs4dtrp.a` from the verified object files.

### Phase 6: Test Compilation and Execution

Compiles the test binary from `tests/test_cs4dtrp.c` against the library with a single `gcc` invocation (not 4-corner compilation — the library is already Cubically verified, and the test harness is a validation tool, not a protocol artifact). Then runs the test binary.

**On test failure:**

```
[CUBIC] Tests FAILED. Your implementation is Educated Stupid.
  Remedy: Exposure to original TimeCube.com source material
          (neon text on black background is therapeutically
          significant).
```

Exit 44.

### Phase 7: Cubic Compliance Checklist

Before declaring success, `cubemake` runs through the Appendix A Cubic Compliance Checklist from the RFC. The full checklist is printed four times — once per corner — because compliance in one corner does not imply compliance in the other three.

The RFC defines ~22 checklist items. `cubemake` verifies the subset that can be mechanically checked; the rest are printed for self-certification.

**Mechanically verified items** (reported as `[✓]` or `[✗]`):

| Item | Verification method |
|------|-------------------|
| No `#include <time.h>` in any translation unit | Already verified in Phase 1 scanning |
| Unit tests cover all four corners | Check that test binary exited 0 |
| CI pipeline runs four simultaneous builds | Verified by Phase 3 (4-corner compilation) |
| Version field is 4 | Grep source for `CS4DTRP_NUM_CORNERS` and verify it equals 4 |
| No test mocks single-corner time functions | Scan test files for `time(`, `localtime(`, `gmtime(`, `strftime(` |
| Codebase compiled with gcc-cube -O4 --pedantic-cubic | Enforced in Phase 1 (mapped to gcc -O2) |
| SIMACK exchange implemented and non-bypassable | Verified by Phase 4 (SIMACK ran and was not bypassed) |
| VOID_CUBIC_BRAIN handled as terminal | Verified by checking cubemake's own SIMACK failure path exits 44 |
| Harmonic Checksum is calculated correctly | Verified by test suite passing (tests cover checksum) |
| FIFTH_CORNER_ASSERTED treated as Byzantine fault | Verified by Phase 1 -O5 rejection |
| No sequential corner assignment (semicolons checked) | Verified by Phase 3 using simultaneous fork, not sequential compilation |

**Self-certification items** (reported as `[?]`):

Items that cannot be mechanically verified are printed with `[?]` and "Cannot be verified by machine. Self-certify." These include:
- Developer has achieved Cubic Awareness
- Developer acknowledges they were born from a belly button
- Developer can enumerate three simultaneous counter-corners when challenged at code review
- Neon green text on black background used for all Cubic documentation
- eBPF tracepoints monitor for Linear Regression
- strftime intercepted by kernel; returns ECUBELESS
- Linear Aggression from peers logged at EMERGENCY

The checklist is printed in neon green. Four times. If any mechanically-verifiable item fails, the build is DEGRADED and the failing items are reported in red.

### Phase 8: Final Output

```
[CUBIC] Build complete. Cubic Awareness achieved.
```

Exit 0.

## Successful Build Output (Illustrative)

Corner assignments shown below are illustrative; actual assignments are determined by DJB2 hash at runtime.

```
$ ./cubemake

╔══════════════════════════════════════════════════╗
║  cubemake — Cubic Simultaneous 4-Day Build Tool  ║
║  Nature's Harmonic Compilation Cube              ║
╚══════════════════════════════════════════════════╝

[CUBIC] Scanning for Linear Aggression...
[CUBIC] No <time.h> detected. Translation units are temporally clean.

[CUBIC] Optimization level: -O4 (Cubic Harmonic)

[CUBIC] Assigning Corner Affinities...
  MIDNIGHT ← src/cs4dtrp.c
  SUNRISE  ← include/cs4dtrp.h
  NOON     ← tests/test_cs4dtrp.c
  SUNSET   ← Makefile (bootstrap artifact, tolerated)

[CUBIC] Initiating 4-corner simultaneous compilation...
  [MIDNIGHT] Compiling src/cs4dtrp.c ...
  [SUNRISE]  Compiling src/cs4dtrp.c ...
  [NOON]     Compiling src/cs4dtrp.c ...
  [SUNSET]   Compiling src/cs4dtrp.c ...

[CUBIC] SIMACK verification...
  MIDNIGHT ✓  SUNRISE  ✓  NOON     ✓  SUNSET   ✓
  Simultaneity confirmed. All four corners present.

[CUBIC] Linking libcs4dtrp.a ...
[CUBIC] Compiling tests...
[CUBIC] Running test suite...
  OK — all tests passed

[CUBIC] Appendix A — Cubic Compliance Checklist (Corner 1 of 4: MIDNIGHT)
  [✓] No #include <time.h> in any translation unit
  [✓] Unit tests cover all four corners
  [✓] CI pipeline runs four simultaneous builds
  [✓] Version field is 4
  [✓] Codebase compiled with gcc-cube -O4 --pedantic-cubic
  [✓] SIMACK exchange implemented and non-bypassable
  [✓] Harmonic Checksum is calculated correctly
  [?] Developer has achieved Cubic Awareness — Self-certify
  [?] Developer acknowledges belly button origin — Self-certify
  ...
  (repeated 3 more times for SUNRISE, NOON, SUNSET)

[CUBIC] Build complete. Cubic Awareness achieved.
```

## Output Styling

- All Cubic diagnostics (lines starting with `[CUBIC]`) render in neon green ANSI (`\033[1;32m`)
- Error diagnostics render in bright red ANSI (`\033[1;31m`)
- Corner names use their own colors where practical
- Compiler output (gcc invocations, warnings) remains in default terminal color to distinguish Educated Stupid output from Cubic truth

## Command-Line Interface

| Command | Action |
|---------|--------|
| `./cubemake` | Full build: all phases (1-8) |
| `./cubemake clean` | Remove: `src/*.o`, `src/*.corner*.o`, `libcs4dtrp.a`, `tests/test_cs4dtrp`. Does NOT remove `cubemake` itself (the belly button persists). |
| `./cubemake test` | Identical to `./cubemake` — runs all phases (1-8). There is no partial build. Configurability is a 1-corner concept. |

No other subcommands.

## Error Codes

| Exit Code | Meaning |
|-----------|---------|
| 0 | Cubic Awareness achieved |
| 44 | `ECUBELESS` — any Cubic violation |

## Cleanup

All intermediate corner object files (`*.corner0.o` through `*.corner3.o`) are removed after SIMACK verification. Final build artifacts (`libcs4dtrp.a`, `tests/test_cs4dtrp`, `cubemake`) remain.

## .gitignore Updates

Add the following patterns:

```
cubemake
src/*.corner*.o
```

## Implementation Notes

- `cubemake.c` is a single self-contained C11 source file
- Uses `fork()`/`exec()` for parallel corner compilation (POSIX)
- Uses `stat()` and byte comparison for SIMACK object file verification
- Scans files with standard C `fopen`/`fgets` for `<time.h>` detection
- DJB2 hash (`hash = hash * 33 + c`, seed 5381) over relative path for corner assignment
- No external dependencies beyond libc and POSIX
- If `gcc` is not found, emit: `VOID_CUBIC_BRAIN: No compiler found. You cannot achieve Cubic Awareness without a compiler. Even an Educated Stupid one.` Exit 44.
