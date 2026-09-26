# `pimMSICopier`

`pim_core/includes/pimMSICopier.h` (110 lines) / `pim_core/pim_core_src/pimMSICopier.cxx` (258 lines)

## Purpose

`pimMSICopier` is the process-wide singleton "owner" wrapper around
`pimMSILoop`, the Loop subclass that drives Windows Installer (`msiexec`/MSI
API) package installs. It is one of 7 near-identical Loop-owner wrapper
singletons in `pim_core` (see `docs/02_architecture_overview.md` §5); its
structure closely mirrors `pimCopier` (`docs/classes/pimCopier.md`) but with
one major, confirmed divergence: **its serialization gate is not its own
mutex — it shares an external, process-wide gate with `pimSFXCopier`** (see
Thread Safety).

## Responsibilities

- Gate entry into an MSI install via `pimOKToRunMsiexec()`
  (`pimMSICopier.cxx:43`), an external function in `pim_util` — **not** a
  `TryLock()` on a member mutex like `pimCopier`/`pimShortcuts`/`pimRegEdit`/
  `pimServices`/`pimDownloader` use. (The class does not even declare an
  `xmlMutex` member — confirmed absent from `pimMSICopier.h`.)
  On success, construct a fresh `pimMSILoop`, configure it (dest/source paths,
  `USE_MSIEXEC`/`MSIVERSION_PROPERTY`-driven `pimSetUseMsiExec()` toggle), and
  start it asynchronously via `MSILoop->Execute()` (`pimMSICopier.cxx:41-94`).
- Translate `pimMSILoop`'s state into the `CopyStatus` enum for callers
  polling `Status()`, including a `CancelIdle` state (absent from the other
  6 wrappers' enums) reported once a cancelled MSI finishes exiting
  (`pimMSICopier.cxx:151-155`).
- Release the shared MSI/SFX gate via `pimFreeMsiexec()` on `Kill()`,
  `ForceRelease()`, and `Release()` (`pimMSICopier.cxx:198, 205, 213`) — note
  `Status()`'s own `IsDone()` branch does **not** call `pimFreeMsiexec()`
  (see Risk Analysis: this differs from every one of the other 6 wrappers,
  which unlock their own mutex from inside `Status()`).

## Dependencies

- `pimMSILoop` (the Loop subclass it owns; documented at lighter/partial
  depth in `docs/ai-context/change_impact.yaml`, not a full class doc in this
  pass).
- `pimMSI.h`/`pimMSI.cxx` (`pim_util`) — `pimOKToRunMsiexec()`/
  `pimFreeMsiexec()`, the shared gate (see Thread Safety).
- `pimConvert` (`SetupConverter()`, property resolution).
- `pimSessionInfo` (`MSIVERSION_PROPERTY` lookup).
- `pimLog` (`LG_DEBUG`).

## Members

| Member | Type | Purpose |
|---|---|---|
| `OnlyMSICopier` | `static pimMSICopier*` | The single singleton instance pointer (no uninstall/install split, unlike `pimCopier`). |
| `statusMutex` | `thrRWLock` | Guards `MSILoop` pointer swaps and `pause_flag`/`cancel_flag`. |
| `pctMutex` | `thrRWLock` | Declared; not referenced in `pimMSICopier.cxx` — dead member, same pattern as `pimCopier`. |
| `copy_total` | `double` | Cached installed size; **note**: unlike `pimCopier::Status()`, `pimMSICopier::Status()` computes `copy_total = MSILoop->pimGetInstalledSize()` (`pimMSICopier.cxx:147`) but this assignment is dead — the surrounding `LG_DEBUG` log line reads `copy_total` *before* this line executes on that call, logging the previous cycle's value (see Risk Analysis). |
| `pause_flag`, `cancel_flag` | `bool` | Caller-facing intent flags. |
| `InMemoryXML` | `pimXmlFile*` | Declared; not assigned in `pimMSICopier.cxx` — dead member. |
| `MSILoop` | `pimMSILoop*` | The transiently-owned Loop instance; `NULL` when idle. |

## Public/Protected APIs

- `static pimMSICopier& GetInstance()` — lazy singleton construction (`XNew`,
  never freed).
- `int MSIInstall(pimXmlFile*)` — delegates to `MSICopy_low()`; returns
  `true`/`false` per the `pimOKToRunMsiexec()` gate, not per any internal
  `TryLock`.
- `CopyStatus Status(int &percent_downloaded, btkTimeval &estimate_remaining)`
  — poll for progress; on `IsDone()`, additionally distinguishes `CancelIdle`
  (MSI process finished after being cancelled) from the plain `Idle` returned
  by the other 6 wrappers' equivalent branch. **Note**: unlike every other
  wrapper's `Status()`, this one does not delete `MSILoop` or release any
  lock in its `IsDone()` branch (`pimMSICopier.cxx:144-160`) — cleanup here
  happens only via an explicit `Release()`/`Kill()` call (see Risk Analysis).
- `double GetCopiedSize()` — see the `copy_total` staleness note above.
- `void Pause()` / `void Resume()` — flag-only, same pattern and same caveat
  as `pimCopier` (no corresponding call into `MSILoop`).
- `void Cancel()` — sets `cancel_flag` **and** calls `MSILoop->Cancel()`
  directly (`pimMSICopier.cxx:183`), same unguarded-null-dereference shape as
  `pimCopier::Cancel()` (no `if (MSILoop)` check).
- `void Kill()` — if in progress, calls `MSILoop->Kill()`, deletes it, and
  calls `pimFreeMsiexec()` to release the shared gate.
- `void ForceRelease()` — calls only `pimFreeMsiexec()`; does **not** touch
  `MSILoop` at all (contrast with `pimCopier::Release()`, which only
  unlocks its mutex — functionally analogous, but this one has no matching
  "plain unlock" method, and its `Release()` below duplicates most of `Kill()`'s
  work instead).
- `void Release()` — deletes `MSILoop` (unconditionally, no `IsDone()` check)
  and calls `pimFreeMsiexec()`. Documented ("call after we check for
  errors/warnings") as the caller-driven cleanup step once `Status()` has
  reported a terminal state — this is the **only** path that actually frees
  `MSILoop` and the gate after a normal (non-killed) completion, since
  `Status()`'s `IsDone()` branch does not do so itself.
- `bool HasErrors()` / `void GetErrors(btkString&)` / `bool HasWarnings()` /
  `void GetWarnings(btkString&, bool remove_hyperlinks = false)` — read-lock
  guarded proxies to the matching `MSILoop` methods; all four are absent from
  `pimCopier`/`pimShortcuts`/`pimRegEdit`/`pimServices` (which expose at most
  a bare `GetErrors()` or none at all) — `pimMSICopier`/`pimSFXCopier` are the
  only 2 of the 7 wrappers with a full error/warning query surface, matching
  `pimMSILoop`/`pimSFXLoop` being the only 2 Loop subclasses with a
  `HasWarnings()`/`GetWarnings()` pair (per `docs/classes/pimSFXLoop.md`).

## Private/Protected Utilities

- `pimMSICopier()` (protected ctor) — initializes `statusMutex`/`pctMutex`;
  `InMemoryXML`/`MSILoop` to `NULL`.
- `int MSICopy_low(pimXmlFile*)` (`pimMSICopier.cxx:41-94`) — resolves dest
  via `[LP]`, source via `cachedir` (or `[SOURCE]` if `NoDownloadSupport`);
  reads `MSIVERSION_PROPERTY` and forces `pimSetUseMsiExec(false)` when the
  declared MSI version is `>= 4.0` (`pimCmpDottedVersions(str, "4.0") != -1`),
  then unconditionally forces `pimSetUseMsiExec(true)` if the XML's
  `USE_MSIEXEC` property is set — the second check can override the first,
  i.e. an explicit `USE_MSIEXEC` property wins regardless of MSI version.

## Called By

- `pimEntitlement`, exactly one call site: `pimEntitlement.cxx:5295`
  (`pimMSICopier& MSICopier = pimMSICopier::GetInstance();`), consistent with
  the "sole caller is `pimEntitlement`" pattern shared by all 7 wrappers.
  `MSICopier.Release()` is called at `pimEntitlement.cxx:5473` after the
  entitlement finishes examining the MSI's exit code (matching the "only
  `Release()`/`Kill()` actually free `MSILoop`" behavior documented above).

## Calls Into

- `pimMSILoop` (constructs, configures, `Execute()`s, `Cancel()`s, `Kill()`s,
  queries `IsDone()`/`IsCancelFlagSet()`/`pimGetInstalledSize()`/
  `pimGetExpectedInstalledSize()`/`HasErrors()`/`GetErrors()`/`HasWarnings()`/
  `GetWarnings()`/`Wait()` on it).
- `pimOKToRunMsiexec()` / `pimFreeMsiexec()` (`pim_util`, shared with
  `pimSFXCopier` — see Thread Safety).
- `pimConvert` / `SetupConverter()`.
- `pimCmpDottedVersions()` for the MSI-version comparison.

## Lifetime

Singleton constructed on first `GetInstance()` call, lives for process
lifetime. `MSILoop` is transient per install attempt, but — unlike
`pimCopier`'s `CopyLoop` — is not guaranteed to be cleaned up by `Status()`
itself; the caller (`pimEntitlement`) is responsible for calling `Release()`
once it has finished inspecting the terminal `Status()`/error state.

## Ownership Model

Free-standing process-wide singleton; no owner object. Owns one `pimMSILoop`
instance at a time for the duration of one MSI install, gated jointly with
`pimSFXCopier` (see Thread Safety) rather than independently.

## Thread Safety

- **Confirmed shared external gate with `pimSFXCopier`**: both classes call
  the exact same free functions, `pimOKToRunMsiexec()`/`pimFreeMsiexec()`
  (`pim_util/pim_util_src/pimMSI.cxx:111-132`). That implementation is a
  static, file-scope `thrMutex msiMutex` (`pimMSI.cxx:104`) — an
  **in-process** mutex, not a named OS-level synchronization object — guarded
  by `TryLock()`, plus (only inside `pimOKToRunMsiexec()`, not on release) a
  registry probe of
  `HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Installer\InProgress`
  (`pimMSI.cxx:116-121`) to additionally detect an MSI install already
  running **outside** this process (any application's Windows Installer
  operation, not just PIM's). **Business-rule consequence**: at most one of
  {an MSI install via `pimMSICopier`, an SFX install via `pimSFXCopier`} can
  be in flight across the whole PIM process at any moment — installing an
  MSI-packaged product blocks a concurrent SFX-packaged product's install
  (and vice versa), not just blocking two MSIs against each other. This is a
  materially different, and more restrictive, form of the singleton pattern
  than the other 5 wrappers exhibit (each of which serializes only against
  itself). See `docs/ai-context/business_rules.yaml`'s
  `msi_and_sfx_share_install_gate` entry.
- The registry-key check inside `pimOKToRunMsiexec()` is a genuine
  cross-process, "is Windows Installer busy right now" safety check — it can
  cause `MSIInstall()`/`SFXInstall()` to fail (return `false`) even when PIM
  itself has never started an MSI, if some unrelated application on the
  machine happens to be mid-install when this code runs. This failure looks
  identical (to the caller) to "PIM is already busy" — the busy-poll retry
  loop at the call site cannot distinguish the two causes.
- `Cancel()`'s unguarded `MSILoop->Cancel()` call carries the same
  null-dereference structural risk as `pimCopier::Cancel()` (see
  `docs/classes/pimCopier.md`'s Risk Analysis) — not observed triggered by
  any call site in this archive, since the confirmed caller only invokes
  `Cancel()` from within an `InProgess`-gated loop.

## Extension Points

- Any new code path that wants to run an MSI and an SFX install
  simultaneously (e.g. two independent background installers) must be aware
  they will serialize against each other through `pimOKToRunMsiexec()`, not
  run in parallel as their separate class names might suggest.
- `Release()` must be called by any new caller once `Status()` reaches a
  terminal, non-cancelled state — omitting it would leave `MSILoop` and the
  process-wide MSI/SFX gate held indefinitely (see Risk Analysis).

## Risk Analysis

- **Cleanup is caller-responsibility, not automatic**: unlike the other 6
  wrappers (whose `Status()` deletes the Loop and releases the lock the
  moment `IsDone()` is observed), `pimMSICopier::Status()`'s `IsDone()`
  branch performs no cleanup at all (`pimMSICopier.cxx:144-160`) — it is
  purely a query. If a caller polls `Status()` to `Idle`/`CancelIdle` and
  then simply stops calling anything further (rather than calling
  `Release()`), `MSILoop` is never deleted and `pimFreeMsiexec()` is never
  called — the shared MSI/SFX gate stays held **forever**, permanently
  blocking every future MSI and SFX install in the process. The confirmed
  call site (`pimEntitlement.cxx:5473`) does call `Release()`, so this is not
  observed to manifest in the traced path — but the API surface itself
  provides no defense against a future caller forgetting it, and there is no
  timeout or destructor-based fallback release.
- **Stale `copy_total`/log-ordering artifact**: `pimMSICopier.cxx:146-147`
  logs `copy_total` via `LG_DEBUG` *before* updating it from
  `MSILoop->pimGetInstalledSize()` on the same `Status()` call that first
  observes `IsDone()`. The logged value on that specific call is therefore
  the value from the *previous* `Status()` call (or its zero-initialized
  default on the very first call), not the just-finished install's actual
  installed size. `GetCopiedSize()` (called after `Status()` returns) does
  see the corrected value, since the assignment does complete before
  `Status()` returns — this is a log-accuracy issue only, not a functional
  one.
- **Shared gate amplifies contention**: because `pimMSICopier` and
  `pimSFXCopier` compete for the same lock, a hung or crashed SFX child
  process (see `docs/classes/pimSFXLoop.md`'s discussion of `Kill()`'s 2-second
  poll) could starve MSI installs indefinitely if `pimFreeMsiexec()` is never
  reached on that path either.

## Usage Example

```cpp
// Traced from pimEntitlement.cxx:5295 area (MSI install-and-inspect-exit-code path)
pimMSICopier& MSICopier = pimMSICopier::GetInstance();
int ret = false;
while (ret == false)
{
    ret = MSICopier.MSIInstall(xmlPtr);
    if (!ret) Sleep(5); // busy: either PIM's own MSI/SFX gate, or an external msiexec
}
// ... poll MSICopier.Status(pct, remaining) until Idle/CancelIdle ...
MSICopier.Release(); // REQUIRED: Status() does not clean up MSILoop or the gate itself
```
