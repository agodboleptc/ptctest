# `pimMSILoop`

`pim_core/includes/pimMSILoop.h` (103 lines) / `pim_core/pim_core_src/pimMSILoop.cxx` (1106 lines, full read)

## Purpose

`pimMSILoop` is the install-step (`pimLoop` subclass) that installs Windows
Installer (MSI) packages declared in product XML. It is unique among the 9
Loop subclasses in supporting **two entirely different execution strategies**
for the same job, selected by the `use_msiexec` flag (default `true`):

1. **`pimMSIExec()`** — launches `msiexec.exe` as an external child process
   via `btkProcess`, the same general shape as `pimSFXLoop`.
2. **`OnInstall()`** — calls the MSI API directly in-process via
   `pimMSIInstallFullUI()`/`pimMSIInstallBasicUI()`/`pimMSIInstallSilent()`
   (`pim_util/includes/pimMSI.h`), the "enhanced" install path referenced in
   this file's own `$$7 "Chg for enhanced MSI install"` revision entry —
   offering a genuine progress-tick callback and mid-install cancellation
   that the `msiexec.exe`-launching path cannot provide.

This completes the full-depth documentation of all 9 Loop subclasses.

## Responsibilities

- Iterate `<MSI>` elements (gated by package-level and node-level
  `install="Y"` checks — see `IsEligibleForInstall()`) and install each
  eligible one via whichever of the two strategies above is active.
- Resolve the MSI's source path (`from`/`orig_value` attribute, relative to
  `Source` if not already absolute), locale transform file (`.mst`), and any
  `<PROPERTY_LIST>`/`<MSIARGUMENT>`/`<INSTALL>` command-line content.
- Interpret the resulting exit code against the same well-known Windows
  Installer vocabulary documented for `pimSFXLoop`
  (`docs/classes/pimSFXLoop.md`) — `0`/`1602`/`1259`/`1618`/`1637`/`1633`/
  `1632`/`1625`/`1619`/`1613`/`1641`/`1620`/`1603`/`3010` — treating `1618`
  as **cancellation** here (the confirmed intentional divergence from
  `pimSFXLoop`'s "1618 = failure" documented in that class's own Risk
  Analysis).
- On a genuine failure (not cancel, not the 3010/1641 reboot cases), also
  deselects the MSI's parent `<PACKAGE>` (`OnFailure()` sets `install="N"`)
  — the same "persistently deselect the failed package" business rule
  documented in `ai-context/business_rules.yaml`'s `msi_success_codes`.
- (`pimMSIExec()` path only) Append a successful package's uninstall command
  line to a shared, per-product-mode `Uninstall_all_utilities.bat` file
  (`CreateBatchFileUninstallEntry()`), skipping specific MSI names
  (`prime_msi`, `eng_notebook_msi`, `qualityagent_msi` unless Schematics
  mode) and skipping entirely when the product mode is `PIM_MKS_MODE`.

## Dependencies

- `pimLoop` (base class).
- `pim_util/pimMSI.h` — both the MSI-API functions
  (`pimMSIInstallFullUI`/`BasicUI`/`Silent`, `pimMSISetProgressInterface`,
  `pimMSISetLogFile`) and, via `pimOKToRunMsiexec()`/`pimFreeMsiexec()`
  (called by this class's owner `pimMSICopier`, not by this class directly),
  the shared MSI/SFX serialization gate documented in
  `docs/classes/pimMSICopier.md`.
- `btkProcess` (external btk) for the `msiexec.exe` child-process path.
- `pimConvert` for property-placeholder resolution.
- `pimSessionInfo` (`MEDIA_PROPERTY`, `VERSION_PROPERTY`, session property
  lookups used throughout both install paths).
- `pimShortcutMgr` (`GetShortcutDesktopState()`, used only for the
  `prime_msi` special case to suppress a desktop shortcut).
- `pimEntitlement.h` is included; no direct call into it was observed in this
  file's body beyond shared type/constant usage — not confirmed further in
  this pass, same caveat noted for `pimRegEditLoop`'s identical inclusion.

## Members

| Member | Type | Purpose |
|---|---|---|
| `Source` | `btkString` | Base source directory for resolving a relative MSI path. |
| `Dest` | `btkFSEntry` | Destination path (declared for parity with sibling Loop types; not observed read in the traced install paths). |
| `use_msiexec` | `bool` | Selects `pimMSIExec()` (true, default) vs. `OnInstall()` (false) in `OnExecute()`. Set via `pimSetUseMsiExec()`, driven by `pimMSICopier`'s `MSIVERSION_PROPERTY`/`USE_MSIEXEC` logic (`docs/classes/pimMSICopier.md`). |
| `kMutex` | `thrMutex` | Guards `kill_msiexec`. |
| `kill_msiexec` | `bool` | Cooperative-termination flag for the `msiexec.exe` child process, polled every 2 seconds inside `pimMSIExec()`'s wait loop. |
| `SizeMutex` | `thrMutex` | Guards `installed_size`/`expected_installed_size`. |
| `installed_size`, `expected_installed_size` | `double` | Progress-bar tick counters — per the header's own comment, these are **not** real byte sizes for the MSI-API path ("we don't get sizes out of the MSI api, just a X position on the progress bar"); only for the `pimMSIExec()` path are they populated from the XML's own declared `<MSI size="...">` attribute. |

## Public/Protected APIs

- `pimMSILoop(pimXmlFile*)` — constructor; `use_msiexec = true`,
  `kill_msiexec = false`.
- `bool IsInitialized()` — always returns `true`; declared for interface
  parity, not observed meaningfully used in this pass.
- `void Cancel()` — **only forwards to `pimLoop::Cancel()` when
  `!use_msiexec`** (`pimMSILoop.h:60-67`). When `use_msiexec` is `true` (the
  default `msiexec.exe` path), `Cancel()` is a **silent no-op** — matches
  `pimCanCancel()` (`return !use_msiexec`), which callers are expected to
  check before offering a Cancel UI action. This is the same
  "some Loop types can't truly cancel a running child process" shape
  documented for `pimSFXLoop`, but here it is exposed as an explicit,
  queryable capability flag rather than left implicit.
- `void Kill()` (`pimMSILoop.h:69-86`) — **only does anything when
  `use_msiexec` is `true`**; when `false` (API-install path), `Kill()` is
  also a silent no-op — there is no way to force-terminate an in-progress
  MSI-API install from this class (see Risk Analysis). When `use_msiexec` is
  `true`, sets `kill_msiexec = true` under `kMutex`, then **busy-spins the
  calling thread** in a tight `do { lock; read flag; unlock; } while (flag)`
  loop with **no sleep at all** between iterations, waiting for
  `pimMSIExec()`'s own worker-thread loop to notice the flag (on its next
  2-second `WaitForExit` cycle), terminate the child process, and clear the
  flag back to `false` (see Risk Analysis for the CPU-burn implication).
- `void pimSetSource(btkString&)` / `void pimSetDestPath(btkFSEntry&)` —
  configure the base source directory / destination path.
- `void pimSetUseMsiExec(bool tf = true)` — selects the execution strategy
  (see Members).
- `bool pimCanCancel()` — `!use_msiexec`; the queryable capability flag
  matching `Cancel()`'s actual behavior.
- `double pimGetInstalledSize()` / `double pimGetExpectedInstalledSize()` —
  thread-safe (via `SizeMutex`) accessors for the progress-tick counters,
  read by `pimMSICopier::Status()` (`docs/classes/pimMSICopier.md`).
- `void OnExecute()` (protected) — dispatches to `pimMSIExec()` or
  `OnInstall()` based on `use_msiexec`.
- `void pimMSIExec()` (protected) — the `msiexec.exe`-launching strategy; see
  Walkthrough below.
- `void OnInstall()` (protected) — the MSI-API strategy; see Walkthrough and
  the **confirmed bugs** in Risk Analysis.
- `void OnTerminate()` (protected) — standard thread teardown.

## Private Utilities

- `void pimSetExpectedInstalledSize()` — sums the `<MSI size="...">`
  attributes of every eligible node into `expected_installed_size`; called
  only from `pimMSIExec()`, not from `OnInstall()` (which computes its own
  per-node size from `attribSize` directly at success/1641 time instead).
- `void pimSetInstalledSize(DOMNamedNodeMap*)` — adds one node's declared
  size to the running `installed_size` total; `pimMSIExec()`-path only.
- `bool IsEligibleForInstall(DOMNamedNodeMap*)` — checks the parent
  `<PACKAGE>`'s own `install="Y"` (if a `package` attribute links one) AND
  this node's own `install="Y"`. **Does not check the separate `installed`
  (past-tense) attribute at all** — a commented-out block
  (`pimMSILoop.cxx:953-958`) shows a stricter check (skip if already
  `installed="Y"`) was once present or considered, but is dead in the
  compiled code (see Risk Analysis for why this matters).
- `bool GetPackageLabel(DOMNamedNodeMap*, btkString&)` — resolves a
  human-readable label from the linked `<PACKAGE>`'s `label` attribute, used
  to prefix error messages (`$$6 "Added prefix support for errors"`).
- `void SetAttributeInstalled(DOMElement*, const char*)` — sets the
  `installed` attribute under a `PushReadToWrite()`/`PopWriteToRead()`
  bracket and flags the XML content as changed.
- `void CreateBatchFileUninstallEntry(btkString&, pimConvert&)` — builds a
  per-product-mode path to a shared `Uninstall_all_utilities.bat`
  (`/wgm <ver>`, `/Creo <ver>/Schematics`, `/PTC Portmapper` for MKS, or
  `/Creo <ver>` default), de-duplicates against existing file content before
  appending, and silently gives up (logs only) if the target path isn't
  writable. **Called only from `pimMSIExec()`** — the `OnInstall()` API path
  never writes to this file (see Risk Analysis).
- `void ProcessErrorCode(int errorCode, DOMNamedNodeMap*)` /
  `void ProcessErrorCode(ptc_intptr msg_id, DOMNamedNodeMap*)` — the two
  overloads driving exit-code-to-message dispatch; the `int` overload maps
  every numeric code to a message ID and, for real failures (not
  3010/1641/1602/1259/1618's cancel-classified codes), calls `OnFailure()`
  to deselect the parent package. The `ptc_intptr` overload is used for
  non-numeric failure paths (missing MSI, process-launch failure) and always
  calls `OnFailure()`.
- `void OnFailure(DOMNamedNodeMap*)` — sets the linked `<PACKAGE>`'s
  `install="N"`, the shared "persistently deselect" mechanism.

## `pimMSIExec()` Walkthrough (msiexec.exe path, `use_msiexec == true`, the default)

1. `pimSetExpectedInstalledSize()` sums declared sizes across all eligible
   `<MSI>` nodes up front.
2. Iterates `<MSI>` nodes (checking `mCancel`/`mPause` each iteration,
   correctly single-incremented — no double-increment bug here, unlike
   `pimRegEditLoop::OnRollback()`).
3. For each `IsEligibleForInstall()` node: resolves the MSI path, locale
   `.mst` transform, `<MSIARGUMENT>` content, and (for the special
   `msiname == "prime_msi"` case) a WS/WOS install-command variant selected
   by `pimGetPrimeInWSMode()` and the `same_msihybrid` XML property.
4. Builds the full command line (optionally appending `VFS_ROOT=...` when
   `VFS_READ_ENV` is set, reading the scrambled env var
   `PTC_WFS_DEFAULT_LOCATION`; optionally appending `TRANSFORMS=:xx.mst` when
   `USE_TRANSFORM` is set and the locale isn't English).
5. Launches via `btkProcess::Create(Cmd, workdir, btkProcess::Interruptable)`,
   then polls `WaitForExit(2 seconds)` in a loop, checking `kill_msiexec`
   each cycle and calling `MyProcess.Terminate()` if set (clearing the flag
   afterward so the busy-spinning `Kill()` caller unblocks).
6. Dispatches the exit code via `ProcessErrorCode(int, map)`; on
   0/3010/1641, marks `installed="Y"` and (with the name/mode exemptions
   above) appends to the uninstall batch file.

## `OnInstall()` Walkthrough (MSI-API path, `use_msiexec == false`)

1. **A first pass over every `<MSI>` node** (`pimMSILoop.cxx:436-460`) that
   checks `install="Y"` and, if `same_msihybrid` is set, logs a debug line
   ("`same_msihybrid detected so NO MSI called again !!`") — **and then does
   nothing else.** This loop has no side effect beyond that log line; it
   does not set any flag, populate any accumulator, or otherwise influence
   the second (real) loop below. See Risk Analysis: this is confirmed dead/
   vestigial code, and — more importantly — the `same_msihybrid` skip it
   appears to implement is **not actually applied anywhere in this file**.
2. **The real per-node loop** (`pimMSILoop.cxx:461-834`): for each
   `install="Y"` node (note: this loop does **not** call
   `IsEligibleForInstall()` — it checks only the node's own `install`
   attribute, not the parent package's), resolves the MSI path/transform/
   property-list exactly as in `pimMSIExec()`, then dispatches to one of 4
   sub-strategies based on the node's `format` attribute:
   - `format="full"` → `pimMSIInstallFullUI()` (interactive wizard).
   - `format="basic"` (or any `attribFormat` present with a non-empty
     property list, per the slightly odd `(attribFormat || !msi_property_list.IsEmpty())`
     guard combined with a value-equality check against `"basic"` two lines
     later — `pimMSILoop.cxx:603`) → `pimMSIInstallBasicUI()`.
   - `attribFormat` present (any other value) or a non-empty property list →
     `pimMSIInstallSilent()`.
   - Neither present → a fallback that scans the node's `<INSTALL>` child
     text for a `*/I "[MSI]"*` pattern, extracts what follows as the
     property list, **persists `format="silent"` back onto the XML node**
     (a self-healing mutation for legacy XML), and calls
     `pimMSIInstallSilent()`. If that pattern isn't found either, sets
     `fallback_to_msiexec = true` and `continue`s to the next node.
3. Reads any MSI error/warning log file the API call wrote
   (`pimReadWindowsUnicodeFile`), then dispatches the returned exit code
   inline (not via `ProcessErrorCode()` — `OnInstall()` has its own,
   separate, near-duplicate exit-code `if`/`else if` chain covering the same
   numeric codes) to append errors/warnings and mark installed/deselected
   state accordingly.
4. **At the end of every iteration** (`pimMSILoop.cxx:831-832`):
   `if (fallback_to_msiexec) pimMSIExec();` — see Risk Analysis for the
   confirmed repeated-invocation bug this produces.

## Called By

- `pimMSICopier::MSICopy_low()` (`pim_core/pim_core_src/pimMSICopier.cxx:53`)
  — the sole constructor/owner of this class, confirmed in
  `docs/classes/pimMSICopier.md`. `pimSetUseMsiExec()` is called there,
  driven by the XML's `MSIVERSION_PROPERTY` (>= "4.0" → API path) and an
  explicit `USE_MSIEXEC` property (always forces the `msiexec.exe` path,
  overriding the version-based default).

## Calls Into

- `btkProcess` (`msiexec.exe` path only).
- `pimMSIInstallFullUI`/`pimMSIInstallBasicUI`/`pimMSIInstallSilent`/
  `pimMSISetProgressInterface`/`pimMSISetLogFile` (`pim_util/pimMSI.h`, API
  path only) — `pimMSISetProgressInterface()`'s internal mechanism for
  actually driving `installed_size`/`expected_installed_size` during an
  API-based install was not independently traced beyond this call site in
  this pass; treat the exact tick-update mechanism as UNKNOWN/architecture-
  level rather than confirmed line-by-line.
- `pimConvert` / `SetupConverter()`.
- `pimShortcutMgr` (`prime_msi` desktop-shortcut suppression only).
- Base `pimLoop` members: `Mutex`/`mCancel`/`mPause`, `pausePtr`,
  `AppendError()`/`AppendWarning()`, `pimLoop::Cancel()`, `ClearInProgress()`,
  `xmlPtr`'s `PreRead`/`PostRead`/`PushReadToWrite`/`PopWriteToRead`
  protocol.

## Lifetime

Transient: constructed fresh by `pimMSICopier::MSICopy_low()` on each
gate-acquired operation (see `docs/classes/pimMSICopier.md`'s Thread Safety
for the shared MSI/SFX gate mechanism), `Execute()`d asynchronously, and
deleted only by `pimMSICopier::Release()` or `Kill()` — **not** automatically
by `Status()`, per the caller-responsibility-cleanup gap already documented
for `pimMSICopier`.

## Ownership Model

Owned exclusively by `pimMSICopier` (`docs/classes/pimMSICopier.md`), a
process-wide singleton whose serialization is additionally coupled to
`pimSFXCopier` through a shared external gate — not by `pimEntitlement`
directly, despite this file's `#include <pimEntitlement.h>`.

## Thread Safety

- `kill_msiexec` and the `installed_size`/`expected_installed_size` pair
  each have their own dedicated mutex (`kMutex`, `SizeMutex`), correctly
  scoped and consistently used across both execution strategies.
- The `Cancel()`/`Kill()` capability split by `use_msiexec` (see Public APIs)
  means **a caller must check `pimCanCancel()` before assuming `Cancel()`
  does anything**, and must be aware that `Kill()` is a no-op entirely for
  the API-install path — there is no documented way to forcibly abort an
  in-progress `pimMSIInstallFullUI`/`BasicUI`/`Silent()` call from outside
  this class once started.
- `Kill()`'s busy-spin (no `Sleep()` between polls of `kill_msiexec`) burns
  CPU on the calling thread for up to the ~2-second span of
  `pimMSIExec()`'s own `WaitForExit()` polling interval — bounded, but a
  real, confirmed inefficiency (see Risk Analysis).
- `pimMSIExec()` calls `xmlPtr->PreRead()`/`PostRead()` internally
  (`pimMSILoop.cxx:156, 421`). When invoked from `OnInstall()`'s
  `fallback_to_msiexec` path (itself already inside `OnInstall()`'s own
  outer `PreRead()`/`PostRead()` bracket, `pimMSILoop.cxx:429, 835`), this
  produces a **nested `PreRead()`/`PostRead()` pair on the same thread and
  the same `pimXmlFile`**. Whether `pimXmlFile`'s read-lock is safe under
  same-thread re-entrancy was not independently re-verified against
  `pimXmlFile.cxx`'s own body in this pass — flagged as an open question
  given that file's own confirmed prior lock-bug history
  (`docs/classes/pimXmlFile.md`).

## Extension Points

- Any change to the exit-code table must be applied in **three** places to
  stay consistent: `ProcessErrorCode(int, DOMNamedNodeMap*)` (used by
  `pimMSIExec()`), and the separate, hand-duplicated `if`/`else if` chain
  inside `OnInstall()` itself (`pimMSILoop.cxx:667-801`) — these are two
  independently maintained copies of essentially the same mapping, a
  confirmed duplication risk (see Risk Analysis).
- A fix for the `fallback_to_msiexec` repeated-invocation bug (see Risk
  Analysis) should reset the flag after handling it, or restructure the
  fallback to process only the triggering node via `pimMSIExec()` rather
  than re-scanning the whole node list every remaining iteration.
- Re-enabling `IsEligibleForInstall()`'s commented-out `installed="Y"` skip
  (`pimMSILoop.cxx:953-958`) would need product-XML confirmation that
  `installed` and `install` are never expected to both be `"Y"` for a
  package mid-reinstall — do this in tandem with fixing the
  `fallback_to_msiexec` bug, since re-enabling this check would also close
  most of that bug's re-install exposure as a side effect.

## Risk Analysis

- **CONFIRMED HIGH-SEVERITY BUG: `fallback_to_msiexec` triggers repeated,
  compounding re-invocation of `pimMSIExec()`.** The flag is declared once
  before `OnInstall()`'s per-node loop, set `true` (never reset) the first
  time any node's `<INSTALL>` command line can't be pattern-matched for a
  property list, and checked at the **end of every loop iteration**
  (`pimMSILoop.cxx:831-832`) rather than once after the loop. Once set, every
  remaining iteration of `OnInstall()`'s own loop triggers a full,
  independent re-scan-and-install pass over **every** eligible `<MSI>` node
  in the document via `pimMSIExec()` — and because `pimMSIExec()`'s own
  `IsEligibleForInstall()` gate checks only `install="Y"`, **not** the
  separate `installed` (past-tense) attribute, packages already
  successfully installed earlier in the same `OnInstall()` call would be
  silently reinstalled again on each of these repeated `pimMSIExec()`
  passes. Worst case (the very first `<MSI>` node triggers the fallback):
  every one of the remaining N-1 nodes' iterations each launches a full
  `pimMSIExec()` pass over all N nodes, an O(N²) blowup in redundant
  `msiexec.exe` launches. This reproduces only when `use_msiexec` is
  `false` (the API-install path is active) and at least one `<MSI>` node
  lacks a `format` attribute, has an empty property list, and its
  `<INSTALL>` command text doesn't match `*/I "[MSI]"*` — the code's own
  comment describes this as an "older XML" scenario, so it is a real,
  reachable condition for legacy product definitions, not purely
  hypothetical.
- **CONFIRMED: the `same_msihybrid` skip is dead — it has no effect on
  which MSIs actually get processed.** `OnInstall()`'s first loop
  (`pimMSILoop.cxx:436-460`) checks `same_msihybrid` and logs an
  intent-to-skip message, but sets no flag and has no other effect; the
  second (real) loop that actually performs installs never checks
  `same_msihybrid` at all, and neither does `pimMSIExec()`. A product XML
  relying on `same_msihybrid` to suppress a redundant MSI install (the
  property's own name strongly implies exactly that intent) gets no such
  suppression from this class in the traced code paths — the MSI is
  processed anyway. This is a confirmed gap between the code's evident
  intent (per its own debug-log wording) and its actual behavior; treat
  `same_msihybrid` as inert here rather than assuming it works, and confirm
  with the original team whether a different mechanism now supersedes it
  (e.g. `pimEntitlement`-level gating) before either "fixing" this
  vestigial loop or relying on the property continuing to be honored.
- **`Kill()`'s busy-spin has no sleep** (`pimMSILoop.h:79-84`) — a confirmed,
  though bounded (≤ ~2 seconds), 100%-CPU spin on the calling thread while
  waiting for the `msiexec.exe`-monitoring thread's next poll cycle to
  notice `kill_msiexec`. Contrast with `pimSFXLoop::Kill()`, which sets its
  flag and returns immediately without blocking the caller at all. A
  straightforward fix would add a short `Sleep()` inside this loop.
  Independent of severity, this is a real, reproducible inefficiency, not a
  hypothetical one.
- **`Kill()` is a total no-op for the MSI-API install path**
  (`use_msiexec == false`) — there is no mechanism in this class to abort an
  in-progress `pimMSIInstallFullUI`/`BasicUI`/`Silent()` call once started;
  a caller can only hope the API call itself checks for cancellation
  internally (not confirmed one way or the other in this pass, since
  `pimMSIInstallSilent()`'s own implementation is in `pim_util`, outside
  this file). Combined with `Cancel()` also being a no-op unless
  `!use_msiexec` (i.e., `Cancel()` DOES forward when the API path is active
  — so `Cancel()` is the only lever available for that path, and `Kill()`
  provides no escalation beyond it).
- **Uninstall-batch-file registration is asymmetric between the two
  strategies**: `CreateBatchFileUninstallEntry()` is called only from
  `pimMSIExec()`; `OnInstall()`'s API-installed packages never get an entry
  in the shared `Uninstall_all_utilities.bat`, regardless of product mode.
  Any downstream tooling or support process that relies on that batch file
  listing every installed MSI-based utility would silently miss entries
  installed via the API path.
- **Duplicated exit-code-handling logic**: `ProcessErrorCode(int, ...)` and
  `OnInstall()`'s own inline `if`/`else if` chain independently reimplement
  overlapping (but not textually identical — e.g. `OnInstall()`'s chain
  additionally handles `1603` inline where `ProcessErrorCode(int,...)` also
  has a `1603` branch, so these are close but not proven byte-identical)
  mappings from exit code to message/behavior. A future change to one
  without the other would silently desynchronize how the two install
  strategies report the same underlying MSI failure.
- Nested `PreRead()`/`PostRead()` risk via the `fallback_to_msiexec` path —
  see Thread Safety; flagged as an open question rather than a confirmed
  defect.

## Usage Example

```cpp
// Traced from pimMSICopier::MSICopy_low(), pim_core/pim_core_src/pimMSICopier.cxx:41-94
MSILoop = XNew pimMSILoop(xmlPtr);
MSILoop->pimSetDestPath(path);
MSILoop->pimSetSource(str);
if (pimCmpDottedVersions(msiVersionStr, "4.0") != -1)
    MSILoop->pimSetUseMsiExec(false);   // prefer the MSI-API path for MSI >= 4.0...
if (xmlPtr->GetProperty("USE_MSIEXEC", str))
    MSILoop->pimSetUseMsiExec(true);    // ...unless USE_MSIEXEC explicitly overrides it
MSILoop->Execute(); // asynchronous; caller polls pimMSICopier::Status()
```
