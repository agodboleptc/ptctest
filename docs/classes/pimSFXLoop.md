# Class: `pimSFXLoop`

**File:** `pim_core/includes/pimSFXLoop.h` (56 lines) / `pim_core/pim_core_src/pimSFXLoop.cxx` (416 lines)
**Module:** `pim_core`
**Inherits:** `pimLoop`

## Purpose

Install-step class that runs self-extracting-executable (SFX) payloads as child
processes — PTC's alternative to MSI for products/utilities packaged as a plain
`.exe` installer (e.g. Creo View Express, dotNetFx redistributables, platform
agents).

## Responsibilities

- Iterate `<SFX install="Y">` elements in the entitlement's XML and launch each
  referenced executable as a child process (`OnInstall`).
- Interpret the child process's exit code using the same well-known Windows
  Installer exit-code vocabulary as `pimMSILoop` (many SFX payloads are themselves
  MSI-based under the hood) — see §"Exit Code Handling" below.
- Support cooperative termination of the running child process via a
  mutex-guarded `kill_exec` flag, polled every 2 seconds while the process runs.
- Append a successful package's uninstall command line to a shared
  `Uninstall_all_utilities.bat` file, keyed by product mode (WGM/Schematics/MKS/
  Creo path conventions), so a single batch script can later uninstall every
  SFX-installed utility.

## Dependencies

- `pimLoop` (base class).
- `btkProcess` (external `btk`) — child-process creation/monitoring
  (`btkProcess::Interruptable`, `WaitForExit`, `GetCurrentState`, `Terminate`).
- `pimConvert` (property-placeholder resolution for the SFX path, working
  directory, and command-line arguments).
- `pimSessionInfo` / `pimGetSessionInfo()` (reads `VERSION_PROPERTY`,
  `[INSTALLBASE]` for building the shared uninstall-batch path).
- `pimGetTranslateSpaceChar`/`pimGetProductMode` (`pim/pim_src/pimGeneralInit.cxx`)
  for path construction conventions.

## Members

| Member | Type | Access | Purpose |
|---|---|---|---|
| `Source` | `btkString` | protected | Base source directory for relative SFX paths |
| `Dest` | `btkFSEntry` | protected | (declared; not observed read in `OnInstall`'s traced path — SFX paths are resolved via the XML's `orig_value`/`from` attribute plus `Source`, not `Dest`) |
| `kMutex` | `thrMutex` | protected | Guards `kill_exec` |
| `kill_exec` | `bool` | protected | Set by `Kill()`; polled every 2s while the child process runs to terminate it early |

## Public APIs

| Method | Purpose |
|---|---|
| `pimSFXLoop(pimXmlFile*)` | Ctor; `kill_exec = false` |
| `Cancel()` (override) | Calls `pimLoop::Cancel()` only — unlike `pimCopyLoop`, does **not** additionally signal any process-wide cancel flag; cooperative cancellation here relies solely on the per-element `mCancel` check in `OnInstall`'s loop, which only takes effect **between** SFX executions, not while one is actually running (only `Kill()` can stop a running child process) |
| `Kill()` (override) | Sets `kill_exec = true` under `kMutex`; the running child process is terminated on the next 2-second poll inside `OnInstall`'s wait loop |
| `pimSetSource(btkString&)` / `pimSetDestPath(btkFSEntry&)` | Configure `Source`/`Dest` before `Execute()` |

## Protected APIs

| Method | Purpose |
|---|---|
| `OnExecute()` | Thread entry point; resets `kill_exec` under `kMutex`, calls `OnInstall()` |
| `OnInstall()` | For each `<SFX install="Y">` node: resolves the executable path (`orig_value` or `from` attribute, relative to `Source` if not already absolute), builds a command line from an optional `<COMMAND_ARGUMENTS>` child, launches it via `btkProcess::Create(..., btkProcess::Interruptable)`, polls every 2 seconds (checking `kill_exec` each poll), then dispatches on the exit code |
| `OnTerminate()` | Standard thread teardown (release per-thread logger, `mDone = true`, `ClearInProgress()`) |

## Private Utilities

None beyond the members above — no private methods declared.

## Exit Code Handling (confirmed from source, `OnInstall`)

| Exit code | Treatment |
|---|---|
| `0` | Success — marks `installed="Y"` on the `<SFX>` node, records the uninstall command line into the shared batch file (unless in `PIM_MATHCAD_MODE` or the uninstall string is trivially short) |
| `1602`, `1259` | User cancel / non-continue — calls `pimLoop::Cancel()` (propagates the cancel flag) and logs `pimInstallStateCancelled` |
| `1637`, `1633`, `1632`, `1625`, `1619`, `1613` | Specific MSI-style error messages appended (`pimUIInstallStatusErrorMsi_16xx`) |
| `1618` | **Diverges from `pimMSILoop`'s treatment of the same code**: `pimMSILoop` treats 1618 as a cancellation; here it is logged as a full failure (`AppendError(pimUIInstallStatusErrorMsi_1618)`), with the `pimLoop::Cancel()` call explicitly commented out — the code comment reads "MSI busy - shouldn't really happen... show it as complete failure and not as cancelled." This is a confirmed, intentional divergence between the two Loop types for the identical exit code |
| `1641` | Reboot-initiated success — marks `installed="Y"` |
| `1638` | "A newer version is installed already" — treated as success, marks `installed="Y"` |
| `3010` | Reboot-required success, **except** when `sfxname == "dotNetFx40_Full"`, in which case it is instead treated as an error (`AppendError`, not `AppendWarning`) — a specific carve-out for that one payload found in this codebase |
| anything else | Generic `pimUIInstallStatusError_User_msg` |

## Called By

`pimSFXCopier::SFXCopy_low()` (see Ownership Model below).

## Calls Into

`btkProcess` (child process management), `pimConvert`, `pimMessage`/`LG_*`
(logging), the shared `Uninstall_all_utilities.bat` file (direct file I/O via
`btkFileStream`/`btkOFileStream`).

## Lifetime

Created on demand by `pimSFXCopier` (see Ownership Model), lives for one
SFX-install operation.

## Ownership Model

**Correction relative to this documentation set's earlier assumption**: `pimSFXCopier`
(`pim_core/includes/pimSFXCopier.h`) is a **process-wide singleton**
(`static pimSFXCopier *OnlySFXCopier`, `pimSFXCopier& GetInstance()`), exactly like
`pimRegEdit` (documented in `docs/04_installation_flow.md` §7) — not a
per-entitlement owned object. Its own header comment explains why: *"in theory the
self extracting executable is possibly running an MSI installer, so we ensure we
are not also running an MSI installer"* (referencing `pimOKToRunMsiexec()`/
`pimFreeMsiexec()` from `pim_util`). The singleton holds exactly one `pimSFXLoop*`
at a time, created fresh per `SFXInstall()` call and guarded by a `TryLock`-style
mutex so **only one SFX install can be in flight across the entire process** at
any moment — see `docs/classes/pimCopyLoop.md`'s Ownership Model for the identical
mechanism confirmed directly against `pimCopier.cxx`, which this class mirrors
structurally (not independently re-verified against `pimSFXCopier.cxx`'s own body
in this pass, but the header's `GetInstance()`/`TryLock`-contract comments are
identical wording to the verified `pimCopier` case).

## Thread Safety

Inherits `pimLoop`'s mutex-guarded lifecycle flags. Adds its own `kMutex` for the
`kill_exec` flag, checked on a 2-second polling cadence rather than being pushed to
the child process immediately — a kill request can take up to ~2 seconds to take
effect.

## Extension Points

- New SFX-specific exit code: extend the `if/else if` chain in `OnInstall()`.
- The `dotNetFx40_Full` special case demonstrates the precedent for adding further
  per-payload-name exit-code overrides if a future SFX package needs one.

## Risk Analysis

- **`Cancel()` cannot interrupt a running child process** — only `Kill()` can (via
  the 2-second poll). A caller that only calls `Cancel()` expecting prompt
  termination of an in-progress SFX install will instead wait for that SFX to
  finish naturally before the next loop iteration's cancel check fires.
- **Exit code 1618 is handled inconsistently with `pimMSILoop`** for the same
  numeric code — anyone extending or unifying MSI/SFX error handling should treat
  this as a deliberate divergence to preserve, not a bug to "fix" by making them
  match, unless product behavior confirms otherwise.
- The shared `Uninstall_all_utilities.bat` file is appended to with a
  read-before-append duplicate check, but is **not protected by any mutex/lock
  visible in this class** against concurrent writers — if two SFX installs from
  different entitlement threads complete around the same time (both going through
  their own singleton instance sequentially, but the file write itself isn't gated
  by the same lock protecting `pimSFXCopier`'s `TryLock`), a race is structurally
  possible. Not confirmed as an actual bug in this pass — the outer
  `pimSFXCopier` singleton's serialization likely prevents concurrent
  `pimSFXLoop::OnInstall()` bodies from running at all, which would make this safe
  in practice; worth verifying if this file is ever written from any other code path.

## Usage Example (as evidenced by call sites)

```cxx
// pattern inside pimEntitlement / pimSFXCopier
pimSFXCopier &Copier = pimSFXCopier::GetInstance();
int ret = Copier.SFXInstall(xmlPtr);   // returns 0 if another SFX install is already in progress process-wide
if (ret == 1) {
    pimSFXCopier::CopyStatus st;
    do { st = Copier.Status(); Sleep(100); } while (st == pimSFXCopier::InProgess);
}
```

---
*Extends Phase 5 of the requested 20-phase documentation set (remaining Loop subclasses, done at the same depth as `pimCopyLoop`).*
