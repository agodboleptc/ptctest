# `pimShortcuts`

`pim_core/includes/pimShortcuts.h` (88 lines) / `pim_core/pim_core_src/pimShortcuts.cxx` (148 lines)

## Purpose

`pimShortcuts` is the process-wide singleton "owner" wrapper around
`pimShortcutLoop` (see `docs/classes/pimShortcutLoop.md`), the Loop subclass
that creates/removes Windows `.lnk` shortcuts. It follows the same
`xmlMutex.TryLock()`-gated pattern as `pimCopier`/`pimRegEdit`/`pimServices`/
`pimDownloader` (an internal mutex, not an external shared gate like
`pimMSICopier`/`pimSFXCopier`), but with a confirmed behavioral gap in its
`Cancel()` (see Thread Safety).

## Responsibilities

- Gate entry into a shortcut create/remove/reconfigure-cleanup operation via
  `xmlMutex.TryLock()` (`pimShortcuts.cxx:30`).
- On success, construct a fresh `pimShortcutLoop`, set its rollback/
  reconfigure flags, and start it asynchronously via `Execute()`
  (`pimShortcuts.cxx:28-51`). Unlike `pimCopier`/`pimMSICopier`/`pimSFXCopier`,
  `Create_low()` does no path resolution of its own — `pimShortcutLoop`
  resolves its own destinations internally per shortcut location type (see
  `docs/classes/pimShortcutLoop.md`).
- Distinguish three entry points that all funnel through `Create_low()`:
  `Create()` (plain create), `Uninstall()` (rollback=true), and
  `ReconfigureCleanup()` (rollback=true **and** reconfigure=true) — the third
  is unique to this wrapper among the 7 (no sibling class exposes a distinct
  "reconfigure" entry point; `pimRegEdit`/`pimServices` expose only
  `Create()`/`Uninstall()`).
- Hold `xmlMutex` for the full operation duration, released only by
  `Status()` on `IsDone()` (`pimShortcuts.cxx:106`) or by `Kill()`
  (`pimShortcuts.cxx:145`) — same duration-holding discipline as `pimCopier`.

## Dependencies

- `pimShortcutLoop` (the Loop subclass it owns; full class doc at
  `docs/classes/pimShortcutLoop.md`).
- `threadlibcxx` (`thrMutex`, `thrRWLock`).
- No `pimConvert`/`pimXmlFile` property resolution performed directly in this
  class's `.cxx` (contrast with `pimCopier`/`pimMSICopier`/`pimSFXCopier`,
  which all resolve `[LP]`/`cachedir`/`[SOURCE]` themselves before
  constructing their Loop) — `pimShortcutLoop` is simply handed the raw XML
  and does its own resolution per-shortcut.

## Members

| Member | Type | Purpose |
|---|---|---|
| `OnlyShortcuts` | `static pimShortcuts*` | The singleton instance pointer. |
| `xmlMutex` | `thrMutex` | "One shortcut operation at a time" gate. |
| `statusMutex` | `thrRWLock` | Guards `ShortcutLoop` pointer swaps and `pause_flag`/`cancel_flag`. |
| `pctMutex` | `thrRWLock` | Declared; not referenced in `pimShortcuts.cxx` — dead member, same pattern as its siblings. |
| `pause_flag`, `cancel_flag` | `bool` | Caller-facing intent flags. |
| `InMemoryXML` | `pimXmlFile*` | Declared; not assigned in `pimShortcuts.cxx` — dead member. |
| `ShortcutLoop` | `pimShortcutLoop*` | The transiently-owned Loop instance; `NULL` when idle. |

## Public/Protected APIs

- `static pimShortcuts& GetInstance()` — lazy singleton construction.
- `int Create(pimXmlFile*)` — delegates to `Create_low(xmlPtr, false)`.
- `int Uninstall(pimXmlFile*)` — delegates to `Create_low(xmlPtr, true)`.
- `int ReconfigureCleanup(pimXmlFile*)` — delegates to
  `Create_low(xmlPtr, true, true)`.
- `ShortcutStatus Status(int &percent_done)` — despite the output parameter,
  `percent_done` is **never written** anywhere in `Status()`'s body
  (`pimShortcuts.cxx:68-111`) — it is accepted but left untouched; a caller
  relying on it for a progress bar would read uninitialized/stale memory
  (see Risk Analysis). Also performs cleanup (deletes `ShortcutLoop`, unlocks
  `xmlMutex`) on first observing `IsDone()`.
- `void Pause()` / `void Resume()` — flag-only; no corresponding call into
  `ShortcutLoop`.
- `void Cancel()` — sets `cancel_flag` **only** (`pimShortcuts.cxx:127-132`).
  Does **not** call anything on `ShortcutLoop` — confirmed divergence from
  `pimCopier`/`pimMSICopier`/`pimSFXCopier`'s `Cancel()`, which all propagate
  into their Loop. A cancelled `pimShortcutLoop` therefore keeps running to
  natural completion; only `Kill()` actually stops it (see Thread Safety for
  how the confirmed caller compensates for this).
- `void Kill()` — if in progress, calls `ShortcutLoop->Kill()`, deletes it,
  unlocks `xmlMutex`.

## Private/Protected Utilities

- `pimShortcuts()` (protected ctor) — initializes `statusMutex`/`pctMutex`;
  `InMemoryXML`/`ShortcutLoop` to `NULL`.
- `int Create_low(pimXmlFile*, bool uninstall, bool reconfigure)`
  (`pimShortcuts.cxx:28-51`) — thin setup-and-launch; the `if (!uninstall) {}`
  empty block (`pimShortcuts.cxx:41-43`) is dead code, presumably a stub left
  for install-specific setup that was never needed.

## Called By

- `pimEntitlement::InstallShortcuts()` (`pimEntitlement.cxx:5780`) — `Create()`.
- `pimEntitlement::UninstallShortcuts()` (`pimEntitlement.cxx:5838`) — `Uninstall()`.
- Two further call sites at `pimEntitlement.cxx:6930` and `:7171` — both
  `GetInstance()` again, consistent with reconfigure/additional
  install-shortcut passes (not individually re-traced line-by-line in this
  pass, but confirmed to exist and to be within `pimEntitlement.cxx`,
  maintaining the "sole caller" pattern shared by all 7 wrappers).

## Calls Into

- `pimShortcutLoop` (constructs, sets `SetRollback()`/`SetReconfigure()`,
  `Execute()`s, `Kill()`s, queries `HasErrors()`/`IsPaused()`/`IsDone()`/
  `Wait()` on it — notably **not** `Cancel()`, see Thread Safety).

## Lifetime

Singleton constructed on first `GetInstance()` call, lives for process
lifetime. `ShortcutLoop` is transient per operation; cleaned up by `Status()`
on completion or by `Kill()`.

## Ownership Model

Free-standing process-wide singleton; no owner object. Owns one
`pimShortcutLoop` instance at a time for the duration of one shortcut
create/remove/reconfigure-cleanup operation.

## Thread Safety

- `xmlMutex.TryLock()`/full-duration-hold pattern is identical to
  `pimCopier` — no divergence there.
- **Confirmed `Cancel()` gap, with confirmed caller-side compensation**: since
  `pimShortcuts::Cancel()` only sets a flag and does not stop
  `ShortcutLoop`, the confirmed caller
  (`pimEntitlement::InstallShortcuts()`, `pimEntitlement.cxx:5806-5832`)
  compensates explicitly: on detecting the entitlement itself was cancelled,
  it calls `Shortcuts.Cancel()`, then polls `Status()` for up to `KILL_COUNT`
  iterations waiting for the (still-running) Loop to reach `CancelFlagSet`
  naturally, and only calls `Shortcuts.Kill()` if it is still not done after
  that grace period (`pimEntitlement.cxx:5811-5822`). This same
  cancel-then-grace-period-then-force-kill idiom is used identically for
  `pimServices`/`pimDownloader` (see their docs) — it is a deliberate,
  repeated design pattern in this codebase for wrapper classes whose
  `Cancel()` does not propagate, not an overlooked bug at the call sites
  that use it. It does mean a shortcut operation cancelled mid-flight can
  keep running for up to `KILL_COUNT` polling cycles before being force-killed.

## Extension Points

- A new caller must know that `Status(int &percent_done)`'s `percent_done`
  parameter is never populated — do not build a progress UI around it without
  first adding the missing assignment in `Status()` or reading
  `pimShortcutLoop` state directly.
- Any code adding a new "stop this shortcut operation now" path should call
  `Kill()`, not rely on `Cancel()` alone, or should replicate the
  grace-period-then-`Kill()` idiom used by the existing `pimEntitlement` call
  sites.

## Risk Analysis

- **`percent_done` is a dead output parameter** — confirmed never written in
  `Status()`. Low functional risk today since no traced caller reads it for
  display, but it is a foot-gun for any future caller assuming this API
  behaves like `pimCopier::Status()`'s equivalent (which *is* populated).
- **`Cancel()` does not stop the underlying operation** — by itself, a
  meaningful risk (a shortcut create/remove could keep running well past
  when the user believes they cancelled); mitigated in the one traced call
  path by the grace-period-then-`Kill()` idiom, but any new caller that
  simply calls `Cancel()` and assumes the operation stops promptly would be
  wrong.
- Dead members `pctMutex`/`InMemoryXML`, same as the other 6 wrappers — no
  functional impact, noted for completeness.

## Usage Example

```cpp
// Traced from pimEntitlement::InstallShortcuts(), pimEntitlement.cxx:5778-5834
pimShortcuts& Shortcuts = pimShortcuts::GetInstance();
int ret = false;
while (ret == false)
{
    ret = Shortcuts.Create(xmlPtr);
    if (!ret) { Sleep(5); /* check cancel */ }
}
while ((stat = Shortcuts.Status(pct)) == pimShortcuts::InProgess)
{
    Sleep(1);
    if (IsCancelFlagSet())
    {
        int ct = KILL_COUNT;
        Shortcuts.Cancel(); // flag only -- does not stop ShortcutLoop
        while (((stat = Shortcuts.Status(pct)) == pimShortcuts::CancelFlagSet) && ct > 0)
        {
            Sleep(1); ct--;
        }
        if ((stat = Shortcuts.Status(pct)) == pimShortcuts::CancelFlagSet)
            Shortcuts.Kill(); // actually stops it
        break;
    }
}
```
