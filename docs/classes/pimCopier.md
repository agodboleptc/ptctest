# `pimCopier`

`pim_core/includes/pimCopier.h` (104 lines) / `pim_core/pim_core_src/pimCopier.cxx` (223 lines)

## Purpose

`pimCopier` is the process-wide singleton "owner" wrapper around `pimCopyLoop`
(see `docs/classes/pimCopyLoop.md`). It is the gatekeeper `pimEntitlement` calls
to start a file-copy install step, and the object callers poll to track/cancel
that copy once started. It is one of 7 near-identical "Loop-owner wrapper"
singletons in `pim_core` (`pimCopier`, `pimMSICopier`, `pimSFXCopier`,
`pimShortcuts`, `pimRegEdit`, `pimServices`, `pimDownloader`); see
`docs/02_architecture_overview.md` §5 for the shared pattern across all 7.

Uniquely among the 7, `pimCopier` has **two independent singleton instances**:
`OnlyCopier` (`GetInstance()`) for install-time copying, and
`OnlyUninstallCopier` (`GetUninstallInstance()`) for uninstall/rollback copying
(`pimCopier.cxx:19-20, 30-42`). These are fully separate objects with their own
mutexes and their own `pimCopyLoop` — an install-copy and an uninstall-copy CAN
run concurrently; two install-copies (or two uninstall-copies) cannot.

## Responsibilities

- Gate entry into a copy operation via a non-blocking `TryLock()` on an
  internal `thrMutex xmlMutex` (`pimCopier.cxx:46`), so at most one copy is
  in flight per instance at a time.
- On successful lock, construct a fresh `pimCopyLoop`, configure it (rollback/
  uninstall/ignore-already-installed flags, dest/source paths resolved via
  `pimConvert`), and start it asynchronously via `CopyLoop->Execute()`
  (`pimCopier.cxx:56-92`) — `Copy_low()` returns immediately; it does not wait
  for the copy to finish.
- Translate the underlying `pimCopyLoop`'s state (`HasErrors()`/`IsPaused()`/
  `IsDone()`) plus its own `cancel_flag`/`pause_flag` into the `CopyStatus`
  enum for callers polling `Status()`.
- Hold the `xmlMutex` lock for the **entire duration** of the copy (not just
  setup): it is released only when `Status()` observes `IsDone()`
  (`pimCopier.cxx:165`), or when a caller calls `Kill()`
  (`pimCopier.cxx:205`) or `Release()` (`pimCopier.cxx:212`).

## Dependencies

- `pimCopyLoop` (the Loop subclass it owns; see `docs/classes/pimCopyLoop.md`).
- `pimConvert` (`SetupConverter()`, property placeholder resolution for the
  `[LP]`/`cachedir`/`[SOURCE]` paths).
- `pimXmlFile` (the XML buffer describing what to copy).
- `threadlibcxx` (`thrMutex`, `thrRWLock`) for its own internal locking.

## Members

| Member | Type | Purpose |
|---|---|---|
| `OnlyCopier` | `static pimCopier*` | Install-copy singleton instance pointer. |
| `OnlyUninstallCopier` | `static pimCopier*` | Uninstall/rollback-copy singleton instance pointer. |
| `xmlMutex` | `thrMutex` | Non-reentrant "one copy at a time" gate; `TryLock()`'d in `Copy_low()`, `Unlock()`'d in `Status()`/`Kill()`/`Release()`. |
| `statusMutex` | `thrRWLock` | Guards `CopyLoop` pointer swaps and `pause_flag`/`cancel_flag` reads/writes. |
| `pctMutex` | `thrRWLock` | Declared but not referenced anywhere in `pimCopier.cxx` — dead member (see Risk Analysis). |
| `copy_total` | `double` | Cached final installed size, set from `CopyLoop->pimGetInstalledSize()` when done (`pimCopier.cxx:159`); read via `GetCopiedSize()`. |
| `pause_flag`, `cancel_flag` | `bool` | Caller-facing intent flags, reported back through `Status()`. |
| `InMemoryXML` | `pimXmlFile*` | Declared but not assigned anywhere in `pimCopier.cxx` — dead member (see Risk Analysis). |
| `CopyLoop` | `pimCopyLoop*` | The transiently-owned Loop instance for the current copy; `NULL` when idle. |

## Public/Protected APIs

- `static pimCopier& GetInstance()` / `static pimCopier& GetUninstallInstance()`
  — lazily construct-once-and-return the two singletons (`XNew`, never freed;
  a deliberate process-lifetime leak common to all 7 wrapper classes).
- `int Copy(pimXmlFile*, bool ignore_already_installed = false)` — install-copy
  entry point; delegates to `Copy_low(xmlPtr, false, false, ignore_already_installed)`.
- `int Rollback(pimXmlFile*)` — delegates to `Copy_low(xmlPtr, true, true)`
  (uninstall=true, rollback=true).
- `int Uninstall(pimXmlFile*)` — delegates to `Copy_low(xmlPtr, true)`.
- All three return `1`(true)/`0`(false)/negative per the class-wide TryLock
  contract (see `docs/ai-context/ownership.yaml`'s
  `concurrency_implication_summary`): `true` if the lock was acquired and the
  copy started, `false` if another copy is already in progress on this
  instance and the caller should retry after a sleep.
- `CopyStatus Status(int &percent_Copied, btkTimeval &estimate_remaining)` —
  poll for progress; also performs cleanup (deletes `CopyLoop`, unlocks
  `xmlMutex`) the first time it observes `IsDone()`.
- `double GetCopiedSize()` — valid only after `Status()` has returned `Idle`.
- `void Pause()` / `void Resume()` — set/clear `pause_flag` only; **do not**
  call anything on `CopyLoop` (see Risk Analysis — `pimCopyLoop` has no
  `Pause`/`Resume` method to call in the first place; these flags are
  reported through `Status()` but are not observed to be read by
  `pimCopyLoop` itself).
- `void Cancel()` — sets `cancel_flag` **and** calls `CopyLoop->Cancel()`
  directly (`pimCopier.cxx:190`), propagating cancellation into the running
  Loop, unlike 4 of the other 6 wrapper classes (see Risk Analysis for the
  null-pointer implication).
- `void Kill()` — if a copy is in progress, calls `CopyLoop->Kill()`, deletes
  the Loop, and unlocks `xmlMutex`, freeing the instance for a new caller
  immediately (does not wait for the killed thread to finish tearing down).
- `void Release()` — unconditionally unlocks `xmlMutex` with no other
  cleanup; documented for the case where a caller acquired the lock's *intent*
  but decided not to call `Execute()` — in this class's actual `Copy_low()`,
  `Execute()` is always called once the lock is acquired, so this method's
  documented use case does not correspond to any call path within `pimCopier`
  itself; it exists for callers holding a reference across a decision point.
- `void GetErrors(btkString &out)` — proxies to `CopyLoop->GetErrors()` under
  a read lock; returns nothing (leaves `out` untouched) if `CopyLoop` is
  `NULL`.

## Private/Protected Utilities

- `pimCopier()` (protected ctor) — initializes `statusMutex`/`pctMutex` with
  `thrrwLockPriorityWrite`; `InMemoryXML`/`CopyLoop` to `NULL`. Protected (not
  private) but combined with no public constructor, still enforces
  singleton-only construction from `GetInstance()`/`GetUninstallInstance()`.
- `int Copy_low(pimXmlFile*, bool uninstall, bool rollback, bool ignore_already_installed)`
  (`pimCopier.cxx:44-99`) — the shared implementation behind `Copy()`/
  `Rollback()`/`Uninstall()`. Resolves destination via `[LP]`; for install
  (non-uninstall) resolves source via `cachedir` (or `[SOURCE]` if
  `NoDownloadSupport` is set) plus a hardcoded `ptcsh0` subpath
  (`pimCopier.cxx:83-89`) — the uninstall path does not set a copy source at
  all (nothing to copy *from* on uninstall; `pimCopyLoop` presumably only
  deletes on that path).

## Called By

- `pimEntitlement::StartCopy()` (`pimEntitlement.cxx:5479`) — `GetInstance()`,
  install-copy. Confirmed poll-and-retry usage:
  `while (cp_ret == false) { cp_ret = Copier.Copy(...); if (!cp_ret) Sleep(5); check-cancel }`
  (`pimEntitlement.cxx:5500-5519`).
- `pimEntitlement`'s uninstall/rollback path
  (`pimEntitlement.cxx:7452`) — `GetUninstallInstance()`.
- No other call sites of `pimCopier::GetInstance`/`GetUninstallInstance` exist
  in this archive — `pimEntitlement.cxx` is the sole caller, consistent with
  all 7 wrapper classes (see `docs/ai-context/ownership.yaml`
  `correction_history`).

## Calls Into

- `pimCopyLoop` (constructs, configures, `Execute()`s, `Cancel()`s, `Kill()`s,
  queries `HasErrors()`/`IsPaused()`/`IsDone()`/`pimGetInstalledSize()`/
  `pimGetExpectedInstalledSize()`/`GetErrors()`/`Wait()` on it).
- `pimConvert` / `SetupConverter()` for property resolution.

## Lifetime

Both singletons are constructed on first `GetInstance()`/`GetUninstallInstance()`
call (typically the first install-copy or first uninstall-copy of the process)
and live until process exit — never explicitly destroyed (destructor is a
trivial empty protected `~pimCopier(){}`, never invoked). The owned
`CopyLoop` is transient: newly `XNew`'d at the start of each `Copy_low()`
call (with any prior instance `delete`d first, `pimCopier.cxx:54-55`) and
`delete`d again once `Status()` observes completion or `Kill()` is called.

## Ownership Model

`pimCopier` does not itself have an owner object — it is a free-standing
process-wide singleton, reached only through its static accessors. It owns
(in the sense of exclusive, serialized use) exactly one `pimCopyLoop`
instance at a time, for the duration of one copy operation, per singleton
instance (`OnlyCopier` and `OnlyUninstallCopier` each own their own `CopyLoop`
independently). `pimEntitlement` calls `GetInstance()`/`GetUninstallInstance()`
but does not own either singleton or the `pimCopyLoop` it wraps.

## Thread Safety

- `xmlMutex.TryLock()` is the sole re-entrancy gate; it is intentionally
  non-blocking so busy callers get an immediate `false` rather than blocking
  the calling thread (`pimEntitlement`'s worker thread stays responsive to
  its own cancellation checks between retries).
- `statusMutex` (a `thrRWLock`) protects `CopyLoop` pointer swaps/reads and
  `pause_flag`/`cancel_flag`, using write locks for mutation and read locks
  for `Status()`'s flag checks — correctly split for concurrent readers.
- **Risk**: `Cancel()` (`pimCopier.cxx:186-192`) calls `CopyLoop->Cancel()`
  unconditionally, without checking `CopyLoop != NULL` first. If a caller
  calls `Cancel()` when no copy is in progress (`CopyLoop` is `NULL`), this
  is a null-pointer dereference. No caller in this archive is observed to do
  this (the confirmed call site in `pimEntitlement.cxx` only calls `Cancel()`
  from inside the `Status() == InProgess` polling loop, i.e., only while a
  copy is known to be running), but the method itself has no defensive
  `if (CopyLoop)` guard the way `Kill()` does.

## Extension Points

- A new caller of `Copy()`/`Rollback()`/`Uninstall()` must follow the
  established busy-poll idiom (`while (ret==false) { ret = Copier.X(...); if
  (!ret) Sleep(N); check cancellation }`) — calling once and assuming success
  will silently no-op if another copy is in progress.
- Anyone wanting per-file or finer-grained progress must go through
  `pimCopyLoop` directly (not exposed by `pimCopier`); `pimCopier::Status()`
  only reports coarse percent-by-size and the enum state.

## Risk Analysis

- **`Cancel()` null-dereference risk** (see Thread Safety) — low real-world
  risk given current call-site discipline, but structurally fragile; a future
  caller invoking `Cancel()` outside the established `InProgess`-guarded loop
  would crash the process (uncaught null dereference has no recovery path in
  this codebase's crash-on-unhandled-exception model; see
  `docs/classes/pimLoop.md`'s `OnUnhandled()`).
- **Dead members** `pctMutex` and `InMemoryXML` are declared but never used
  in `pimCopier.cxx` — harmless, but indicate either incomplete cleanup from
  an earlier design or a field intended for use that was never wired up. Not
  a functional risk, but worth flagging before any refactor that assumes
  these fields are load-bearing.
- **`Pause()`/`Resume()` are flag-only** — they set/clear `pause_flag` (read
  back through `Status()`) but do not call any pause/resume method on
  `CopyLoop` (which, per `docs/classes/pimCopyLoop.md`, does inherit
  `pimLoop::Pause()`/`Resume()` — those are simply never invoked here). It is
  UNCONFIRMED whether any caller in this archive actually calls
  `pimCopier::Pause()`/`Resume()` at all; a grep of `pimEntitlement.cxx`
  around the `StartCopy()`/rollback call sites shows none. If a caller ever
  does, the flag would be visible via `Status()` but the underlying copy
  thread would keep running unpaused — a functional gap.
- **Two independent singletons risk of confusion**: code that mistakenly
  calls `GetInstance()` when it means `GetUninstallInstance()` (or vice
  versa) would silently operate on the wrong lock/Loop pair with no
  compiler or runtime check to catch the mistake — both are valid,
  differently-behaving singletons of the identical type.

## Usage Example

```cpp
// Traced from pimEntitlement::StartCopy(), pimEntitlement.cxx:5477-5519
pimCopier& Copier = pimCopier::GetInstance();
int cp_ret = false;
while (cp_ret == false)
{
    cp_ret = Copier.Copy(xmlPtr, ignore_already_installed_flag);
    if (cp_ret == false)
    {
        Sleep(5);
        if (IsCancelFlagSet())
        {
            // ... mark cancelled, optionally schedule rollback, return
        }
    }
}
// then poll Copier.Status(pct, remaining) until Idle/Error
```
