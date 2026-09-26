# `pimServices`

`pim_core/includes/pimServices.h` (96 lines) / `pim_core/pim_core_src/pimServices.cxx` (154 lines)

## Purpose

`pimServices` is the process-wide singleton "owner" wrapper around
`pimServiceLoop` (see `docs/classes/pimServiceLoop.md`), the Loop subclass
that installs/removes/starts/stops Windows services. It also owns the
class-static `serviceAction` field that tells `pimServiceLoop::OnExecute()`
which of Install/Start/Stop to perform (documented in
`docs/classes/pimServiceLoop.md`'s "Structural oddity" note) — this class is
the field's actual owner and accessor. Structurally it looks like the
`xmlMutex.TryLock()` family (`pimCopier`/`pimShortcuts`/`pimRegEdit`), but a
close read of `Create_low()` reveals a **confirmed divergence that
undermines its own serialization guarantee** (see Thread Safety) — this is
the most significant finding of this documentation pass across all 7
wrapper classes.

## Responsibilities

- Gate entry into a service operation via `xmlMutex.TryLock()`
  (`pimServices.cxx:42`).
- On success, construct a fresh `pimServiceLoop`, set its rollback flag if
  uninstalling, and start it asynchronously via `Execute()`
  (`pimServices.cxx:40-61`).
- Own and expose `serviceAction` (`InstallAction`/`StopAction`/`StartAction`)
  via the class-static `getServiceAction()`/`setServiceAction()`
  (`pimServices.cxx:23-31`) — the confirmed mechanism `pimServiceLoop` uses
  to learn which action to perform, since neither `Create()` nor the
  constructor takes an action parameter.
- **Unlike every one of its 6 sibling wrappers**, release `xmlMutex`
  **immediately** after starting the thread, inside `Create_low()` itself
  (`pimServices.cxx:56`), rather than waiting for `Status()` to observe
  `IsDone()` (see Thread Safety for the full analysis and consequence).

## Dependencies

- `pimServiceLoop` (the Loop subclass it owns; full class doc at
  `docs/classes/pimServiceLoop.md`, including the `ginst_*` external
  platform-DLL calls it makes).
- `threadlibcxx` (`thrMutex`, `thrRWLock`).

## Members

| Member | Type | Purpose |
|---|---|---|
| `OnlyServices` | `static pimServices*` | The singleton instance pointer. |
| `xmlMutex` | `thrMutex` | Intended as the "one service operation at a time" gate — but see Thread Safety for why it does not actually serialize whole operations. |
| `statusMutex` | `thrRWLock` | Guards `ServiceLoop` pointer swaps and `pause_flag`/`cancel_flag`. |
| `pctMutex` | `thrRWLock` | Declared; not referenced in `pimServices.cxx` — dead member, same pattern as its siblings. |
| `pause_flag`, `cancel_flag` | `bool` | Caller-facing intent flags. |
| `InMemoryXML` | `pimXmlFile*` | Declared; not assigned in `pimServices.cxx` — dead member. |
| `ServiceLoop` | `pimServiceLoop*` | The transiently-owned Loop instance; `NULL` when idle. |
| `serviceAction` | `static ServiceAction` (private) | Process-wide current action, initialized to `InstallAction` (`pimServices.cxx:14`); read/written only through `getServiceAction()`/`setServiceAction()`. Because it is `static` (one value shared by the whole class, not per-instance and not per-call), setting it and then calling `Create()` is inherently a two-step, non-atomic sequence from the caller's perspective (see Thread Safety). |

## Public/Protected APIs

- `static pimServices& GetInstance()` — lazy singleton construction.
- `int Create(pimXmlFile*)` — delegates to `Create_low(xmlPtr, false)`.
- `int Uninstall(pimXmlFile*)` — delegates to `Create_low(xmlPtr, true)`.
- `static ServiceAction getServiceAction()` / `void setServiceAction(ServiceAction)`
  — the static action-passing mechanism (see Purpose).
- `ServiceStatus Status(int &percent_done)` — like `pimShortcuts`/`pimRegEdit`,
  `percent_done` is accepted but **never written** anywhere in the method
  body (`pimServices.cxx:73-116`) — same dead-output-parameter pattern.
  Cleans up `ServiceLoop` on first observing `IsDone()`, but (consistent with
  `xmlMutex` already having been released in `Create_low()`) does **not**
  call `xmlMutex.Unlock()` here — there would be nothing to unlock a second
  time without risking a double-unlock/undefined-state bug, so its absence
  here is actually the *correct* consequence of the early release, not an
  independent oversight.
- `void Pause()` / `void Resume()` — flag-only; no corresponding call into
  `ServiceLoop`.
- `void Cancel()` — sets `cancel_flag` **only** (`pimServices.cxx:132-137`),
  same flag-only shape as `pimShortcuts`/`pimRegEdit`/`pimDownloader`.
- `void Kill()` — if in progress, calls `ServiceLoop->Kill()`, deletes it;
  also does **not** call `xmlMutex.Unlock()` (same reasoning as `Status()`
  above — it was already released).

## Private/Protected Utilities

- `pimServices()` (protected ctor) — initializes `statusMutex`/`pctMutex`;
  `InMemoryXML`/`ServiceLoop` to `NULL`.
- `int Create_low(pimXmlFile*, bool uninstall)` (`pimServices.cxx:40-61`) —
  see Thread Safety for the critical `xmlMutex.Unlock()` placement finding.
  Contains the same dead `if (!uninstall) {}` empty stub block seen in
  `pimShortcuts`/`pimRegEdit`.

## Called By

- `pimEntitlement::PerformServiceAction(ServiceAction)`
  (`pimEntitlement.cxx:5662-5716`) — calls `Services.setServiceAction(service_action)`
  immediately before `Services.Create(xmlPtr)` inside the retry loop
  (`pimEntitlement.cxx:5671-5672`) — confirming the two-step
  set-then-create sequence described above. Also implements the confirmed
  cancel-then-`KILL_COUNT`-grace-period-then-`Kill()` idiom
  (`pimEntitlement.cxx:5694-5713`), same pattern documented for
  `pimShortcuts`/`pimDownloader`.
- `pimEntitlement::UninstallService()` (`pimEntitlement.cxx:5718-5739`) —
  `Uninstall()`, then polls `Status()` to completion (no cancel-handling
  branch observed in the read range for this particular method).
- A further call site at `pimEntitlement.cxx:7390` (`GetInstance()` again,
  in the rollback/uninstall path) — consistent with the "sole caller is
  `pimEntitlement`" pattern shared by all 7 wrappers.

## Calls Into

- `pimServiceLoop` (constructs, sets `SetRollback()`, `Execute()`s, `Kill()`s,
  queries `HasErrors()`/`IsPaused()`/`IsDone()`/`Wait()` on it — not
  `Cancel()`).

## Lifetime

Singleton constructed on first `GetInstance()` call, lives for process
lifetime. `ServiceLoop`'s intended lifetime is "one operation," but see
Thread Safety for why its actual deletion can race with the thread still
running.

## Ownership Model

Free-standing process-wide singleton; no owner object. **Intended** to own
one `pimServiceLoop` instance at a time for the duration of one service
operation, matching the pattern documented for the other 6 wrappers and
previously stated (before this pass) in `docs/ai-context/business_rules.yaml`
and `docs/classes/pimServiceLoop.md` — **this intent is not what the code
actually enforces** (see Thread Safety). This is a correction to those
earlier documents' characterization of `pimServices` specifically; the other
6 wrappers' "only one operation in flight" claim remains accurate.

## Thread Safety

- **CONFIRMED: `pimServices::Create_low()` releases `xmlMutex` immediately
  after starting the operation's thread, not after the operation
  completes** (`pimServices.cxx:40-61`):
  ```cpp
  int pimServices::Create_low(pimXmlFile *xmlPtr, bool uninstall)
  {
      if (xmlMutex.TryLock())
      {
          statusMutex.SetWriteLock();
          ...
          ServiceLoop = XNew pimServiceLoop(xmlPtr);
          ...
          ServiceLoop->Execute();
          statusMutex.ReleaseWriteLock();
          xmlMutex.Unlock();               // <-- released HERE, before the thread finishes
          // return true - success; xmlMutex lock will be released by Copy thread when done.
          return true;
      }
      return false;
  }
  ```
  The trailing comment — "xmlMutex lock will be released by Copy thread when
  done" — is a stale copy-paste from the sibling classes' `Create_low()`
  methods (`pimCopier.cxx:95`, `pimShortcuts.cxx:47`, `pimRegEdit.cxx:47` all
  carry the identical comment) and is **factually wrong for this class**: the
  lock is released synchronously, inline, immediately, not "when done."
- **Consequence**: because `xmlMutex` is unlocked well before the spawned
  `pimServiceLoop` thread finishes, a second call to `Create()` (or
  `Uninstall()`) made while the first service operation is still running
  will succeed at `TryLock()` (since it is already unlocked), then execute
  `if (ServiceLoop) delete ServiceLoop;` (`pimServices.cxx:46-47`) —
  **deleting the `pimServiceLoop` object that the first call's still-running
  thread is actively executing `OnExecute()` on.** This is a use-after-free /
  destroy-while-running race condition, not merely a missed serialization
  guarantee. Depending on timing this could manifest as a crash, corrupted
  service state, or (best case, if the first thread happens to finish just
  before the second `delete` runs) no visible symptom at all — which is
  likely why this has not been caught: `pimServices::Create()`/`Uninstall()`
  are, per the confirmed call sites, only ever invoked serially by a single
  entitlement's own sequential install/uninstall steps in the traced code
  (`pimEntitlement.cxx`'s `PerformServiceAction()`/`UninstallService()`), not
  from two concurrent threads calling `pimServices::GetInstance()`
  simultaneously. If a future change introduces any concurrent caller (e.g.
  two entitlements' service steps running on separate threads at once, or a
  retry path that re-enters `Create()` before the prior call's thread has
  actually finished), this becomes exploitable.
- **This is a documentation correction**: `docs/classes/pimServiceLoop.md`
  ("Owned by: pimServices ... Only one service operation can be in flight
  across the whole process at a time") and
  `docs/ai-context/business_rules.yaml`, `docs/ai-context/ownership.yaml`, and
  `docs/ai-context/knowledge_graph.yaml`'s prior characterization of all 7
  wrappers as uniformly enforcing "one operation at a time" is **not accurate
  for `pimServices`** specifically. See the correction notes added to those
  files as part of this pass.
- The `serviceAction` static-field hand-off (`setServiceAction()` then
  `Create()`) is itself a second, independent race window: since
  `serviceAction` is a single process-wide static (not per-call), any
  interleaving of two `PerformServiceAction()` calls from different threads
  — even ignoring the `xmlMutex` issue above — could have one thread's
  `setServiceAction()` overwritten by another's before the first's
  `Create()`/`ServiceLoop::OnExecute()` reads it. Combined with the
  `xmlMutex` early-release, there is no actual mutual exclusion protecting
  either the `ServiceLoop` object or the `serviceAction` value across two
  concurrent `pimServices` operations.
- Same flag-only `Cancel()` gap as `pimShortcuts`/`pimRegEdit`, compensated
  at the confirmed `PerformServiceAction()` call site via the same
  grace-period-then-`Kill()` idiom.

## Extension Points

- Before adding ANY concurrent caller of `pimServices` (e.g. parallelizing
  entitlement processing), the `xmlMutex`-early-release bug above must be
  fixed — moving the `xmlMutex.Unlock()` call out of `Create_low()` and into
  `Status()`'s `IsDone()` branch and `Kill()`, matching the other 6 wrappers'
  pattern, is the straightforward fix (not implemented here, per this
  documentation effort's "no source modification" rule — flagged for the
  code owner).
- Any new caller must continue the `setServiceAction()`-then-`Create()`
  sequencing discipline; there is no API that takes the action as a `Create()`
  parameter directly.

## Risk Analysis

- **This is the single highest-severity finding across all 7 wrapper class
  docs in this pass**: a confirmed use-after-free / destroy-while-running
  race condition, currently latent because no traced caller exercises it
  concurrently, but present in the shipped code and not gated behind any
  `#if`/dead-code marker the way some of this codebase's other latent issues
  are (e.g. `ROLLBACK_CANCELLED_INSTALL`, `ERROR_ON_UNSIGNED`) — this one is
  live, reachable code.
- Recommend flagging to the code owner as a priority fix candidate,
  independent of any planned refactor, since it requires only a one-line
  relocation of the `xmlMutex.Unlock()` call to close.
- Shares the flag-only `Cancel()` risk and dead-member pattern (`pctMutex`,
  `InMemoryXML`) documented for its siblings.

## Usage Example

```cpp
// Traced from pimEntitlement::PerformServiceAction(), pimEntitlement.cxx:5662-5716
pimServices& Services = pimServices::GetInstance();
int svc_ret = false;
while (svc_ret == false)
{
    Services.setServiceAction(service_action); // static field, set before every Create()
    svc_ret = Services.Create(xmlPtr);
    if (!svc_ret) { Sleep(5); /* check cancel */ }
}
// NOTE: by the time this poll loop below even starts, xmlMutex has ALREADY
// been released by Create_low() -- a second, concurrent Create() call from
// elsewhere would not be blocked here despite ServiceLoop still running.
while ((stat = Services.Status(pct)) == pimServices::InProgess)
{
    Sleep(1);
    if (IsCancelFlagSet())
    {
        int ct = KILL_COUNT;
        Services.Cancel(); // flag only
        while (((stat = Services.Status(pct)) == pimServices::CancelFlagSet) && ct > 0)
        { Sleep(1); ct--; }
        if ((stat = Services.Status(pct)) == pimServices::CancelFlagSet)
            Services.Kill();
        break;
    }
}
```
