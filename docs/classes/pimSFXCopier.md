# `pimSFXCopier`

`pim_core/includes/pimSFXCopier.h` (105 lines) / `pim_core/pim_core_src/pimSFXCopier.cxx` (215 lines)

## Purpose

`pimSFXCopier` is the process-wide singleton "owner" wrapper around
`pimSFXLoop` (see `docs/classes/pimSFXLoop.md`), the Loop subclass that runs
self-extracting-executable (SFX) installer payloads. It is structurally
almost identical to `pimMSICopier` (`docs/classes/pimMSICopier.md`) — the
header's own comment explains why: "in theory the self extracting executable
is possibly running an MSI installer, so we ensure we are not also running an
MSI installer" (`pimSFXCopier.h:19-23`). It shares `pimMSICopier`'s external
gate mechanism exactly (see Thread Safety) rather than having an independent
lock of its own.

## Responsibilities

- Gate entry into an SFX install via the same `pimOKToRunMsiexec()` external
  function `pimMSICopier` uses (`pimSFXCopier.cxx:33`) — not a `TryLock()` on
  a member mutex (this class also has no `xmlMutex` member).
- On success, construct a fresh `pimSFXLoop`, configure its dest/source paths
  via `pimConvert`, and start it asynchronously via `SFXLoop->Execute()`
  (`pimSFXCopier.cxx:31-76`). Unlike `pimMSICopier`, there is no
  version-comparison or `USE_MSIEXEC`-style branch here — the source/dest
  setup is a straight pass-through.
- Translate `pimSFXLoop`'s state into `CopyStatus` for `Status()` — this
  class's `Status()` takes **no output parameters at all**
  (`pimSFXCopier.h:71`, unlike every other wrapper's `Status(int&, ...)` or
  `Status(int&)`) — callers get only the enum, no percent-complete or
  time-remaining estimate for SFX installs.
- Release the shared MSI/SFX gate via `pimFreeMsiexec()` on `Kill()`,
  `ForceRelease()`, and `Release()` (`pimSFXCopier.cxx:154, 161, 170`) — same
  "Status() does not clean up" shape as `pimMSICopier` (see Risk Analysis).

## Dependencies

- `pimSFXLoop` (the Loop subclass it owns; full class doc at
  `docs/classes/pimSFXLoop.md`).
- `pimMSI.h`/`pimMSI.cxx` (`pim_util`) — `pimOKToRunMsiexec()`/
  `pimFreeMsiexec()`, shared with `pimMSICopier`.
- `pimConvert` (`SetupConverter()`, property resolution).

## Members

| Member | Type | Purpose |
|---|---|---|
| `OnlySFXCopier` | `static pimSFXCopier*` | The single singleton instance pointer. |
| `statusMutex` | `thrRWLock` | Guards `SFXLoop` pointer swaps and `pause_flag`/`cancel_flag`. Note: unlike `pimCopier`/`pimMSICopier`, this class has **no `pctMutex` member at all** (confirmed absent from `pimSFXCopier.h`) — there is no percent-tracking state to guard, consistent with `Status()` not reporting a percentage. |
| `pause_flag`, `cancel_flag` | `bool` | Caller-facing intent flags. |
| `InMemoryXML` | `pimXmlFile*` | Declared; not assigned in `pimSFXCopier.cxx` — dead member, same pattern as its 6 siblings. |
| `SFXLoop` | `pimSFXLoop*` | The transiently-owned Loop instance; `NULL` when idle. |

## Public/Protected APIs

- `static pimSFXCopier& GetInstance()` — lazy singleton construction.
- `int SFXInstall(pimXmlFile*)` — delegates to `SFXCopy_low()`; gated by
  `pimOKToRunMsiexec()`, shared with `pimMSICopier`.
- `CopyStatus Status()` — no output parameters (see Responsibilities); in its
  `IsDone()` branch, additionally distinguishes a cancelled finish
  (`SFXLoop->IsCancelFlagSet()`, returning `CancelFlagSet` rather than a
  dedicated `CancelIdle` value the way `pimMSICopier` does) from a clean
  finish (`Idle`) (`pimSFXCopier.cxx:105-118`). Like `pimMSICopier::Status()`,
  this method does **not** delete `SFXLoop` or call `pimFreeMsiexec()` in
  either branch — cleanup is caller-driven via `Release()`/`Kill()` (see Risk
  Analysis).
- `void Pause()` / `void Resume()` — flag-only, same caveat as the other 6
  wrappers (no corresponding call into `SFXLoop`).
- `void Cancel()` — sets `cancel_flag` **and** calls `SFXLoop->Cancel()`
  directly (`pimSFXCopier.cxx:139`) — same unguarded-null-dereference shape
  as `pimCopier`/`pimMSICopier`'s `Cancel()` (no `if (SFXLoop)` check). Per
  `docs/classes/pimSFXLoop.md`, `pimSFXLoop::Cancel()` itself cannot interrupt
  an already-running child process — only `Kill()` can (2-second poll).
- `void Kill()` — if in progress, calls `SFXLoop->Kill()`, deletes it, calls
  `pimFreeMsiexec()`.
- `void ForceRelease()` — calls only `pimFreeMsiexec()`, does not touch
  `SFXLoop`.
- `void Release()` — deletes `SFXLoop` unconditionally and calls
  `pimFreeMsiexec()`; the only path that frees both after a normal
  completion, identical in shape to `pimMSICopier::Release()`.
- `bool HasErrors()` / `void GetErrors(btkString&)` / `bool HasWarnings()` /
  `void GetWarnings(btkString&, bool remove_hyperlinks = false)` — read-lock
  guarded proxies to `SFXLoop`, identical surface to `pimMSICopier`.

## Private/Protected Utilities

- `pimSFXCopier()` (protected ctor) — initializes `statusMutex` only (no
  `pctMutex` to initialize); `InMemoryXML`/`SFXLoop` to `NULL`.
- `int SFXCopy_low(pimXmlFile*)` (`pimSFXCopier.cxx:31-76`) — resolves dest
  via `[LP]`, source via `cachedir` (or `[SOURCE]` if `NoDownloadSupport`).
  No MSI-version or `USE_MSIEXEC` handling — those concerns are specific to
  `pimMSICopier`'s configuration surface, not shared here.

## Called By

- `pimEntitlement`, exactly one call site: `pimEntitlement.cxx:6148`
  (`pimSFXCopier& SFXCopier = pimSFXCopier::GetInstance();`), inside a block
  guarded by `IsSFXInstall()` — consistent with the "sole caller is
  `pimEntitlement`" pattern shared by all 7 wrappers. Confirmed full
  lifecycle at this call site: `SFXInstall()` (6157) → poll `Status()` in a
  loop (6182), calling `Cancel()` (6187) then `Kill()` (6190) if the
  entitlement is itself cancelled → `Release()` is called on every exit path
  (6201, 6227, 6237), including after inspecting `HasErrors()`/`GetErrors()`/
  `HasWarnings()`/`GetWarnings()` — so the "caller must remember to call
  `Release()`" risk (see Risk Analysis) is NOT observed to manifest on this
  traced path.

## Calls Into

- `pimSFXLoop` (constructs, configures, `Execute()`s, `Cancel()`s, `Kill()`s,
  queries `IsDone()`/`IsCancelFlagSet()`/`HasErrors()`/`GetErrors()`/
  `HasWarnings()`/`GetWarnings()`/`Wait()` on it).
- `pimOKToRunMsiexec()` / `pimFreeMsiexec()` (`pim_util`, shared with
  `pimMSICopier` — see `docs/classes/pimMSICopier.md`'s Thread Safety section
  for the full mechanism: an in-process `thrMutex` plus a registry probe of
  Windows Installer's `...\Installer\InProgress` key).
- `pimConvert` / `SetupConverter()`.

## Lifetime

Singleton constructed on first `GetInstance()` call, lives for process
lifetime. `SFXLoop` is transient per install attempt; cleanup (delete +
gate release) is caller-driven via `Release()`/`Kill()`, not automatic from
`Status()` — identical shape to `pimMSICopier`.

## Ownership Model

Free-standing process-wide singleton; no owner object. Owns one `pimSFXLoop`
instance at a time for the duration of one SFX install, gated **jointly with
`pimMSICopier`** through the shared `pimOKToRunMsiexec()`/`pimFreeMsiexec()`
functions — not independently serialized the way `pimCopier`/`pimShortcuts`/
`pimRegEdit`/`pimServices`/`pimDownloader` are.

## Thread Safety

- Identical shared-gate mechanism and consequence as documented in
  `docs/classes/pimMSICopier.md`'s Thread Safety section: at most one of {an
  MSI install, an SFX install} can run process-wide at any moment, and the
  gate additionally fails closed if Windows Installer is busy anywhere on
  the machine, not just within PIM.
- Because many SFX payloads are themselves MSI-based under the hood (per the
  class-level rationale comment in `pimSFXCopier.h` and
  `docs/classes/pimSFXLoop.md`'s shared exit-code vocabulary with
  `pimMSILoop`), this shared gate is very likely an intentional design
  decision to prevent two Windows Installer transactions (one direct, one
  nested inside an SFX) from colliding — not an accidental oversight, unlike
  some of the other divergences documented across these 7 wrapper classes.
- `Cancel()`'s unguarded `SFXLoop->Cancel()` carries the same structural
  null-dereference risk as its siblings (see `docs/classes/pimCopier.md` Risk
  Analysis); not observed triggered by the single confirmed call site.

## Extension Points

- Same caveat as `pimMSICopier`: new code assuming SFX and MSI installs can
  run in parallel (because they are different singleton classes) will be
  surprised to find them serialized against each other.
- A future caller wanting percent-complete/time-remaining for an SFX install
  would need to add output parameters to `Status()` (or read `pimSFXLoop`
  state directly) — the current signature has none.

## Risk Analysis

- **Same "cleanup is caller-responsibility" gap as `pimMSICopier`**:
  `Status()`'s `IsDone()` branches never delete `SFXLoop` or call
  `pimFreeMsiexec()` themselves; a caller that polls to a terminal state and
  then never calls `Release()`/`Kill()` leaves both `SFXLoop` and the shared
  MSI/SFX gate held indefinitely, blocking all future MSI **and** SFX
  installs process-wide. The one confirmed call site (`pimEntitlement.cxx`)
  does call `Release()` on every exit path (see Called By), so this is a
  latent structural risk for any *future* caller, not an observed defect in
  the traced path.
- **No progress reporting**: `Status()` returning no percent/time-remaining
  means any UI wanting to show SFX install progress must either poll
  `pimSFXLoop` directly (bypassing the wrapper's intended encapsulation) or
  show an indeterminate/spinner state.
- Shares every structural risk already documented for `pimMSICopier`
  (dead `InMemoryXML` member, unguarded `Cancel()`).

## Usage Example

```cpp
// Traced from pimEntitlement.cxx:6148 area (SFX install path, IsSFXInstall() guarded)
pimSFXCopier& SFXCopier = pimSFXCopier::GetInstance();
int ret = false;
while (ret == false)
{
    ret = SFXCopier.SFXInstall(xmlPtr);
    if (!ret) Sleep(5); // busy: shared MSI/SFX gate, or external msiexec
}
// poll SFXCopier.Status() (no percent/time-remaining) until Idle/CancelFlagSet
SFXCopier.Release(); // frees SFXLoop and the shared gate
```
