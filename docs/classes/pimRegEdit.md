# `pimRegEdit`

`pim_core/includes/pimRegEdit.h` (80 lines) / `pim_core/pim_core_src/pimRegEdit.cxx` (143 lines)

## Purpose

`pimRegEdit` is the process-wide singleton "owner" wrapper around
`pimRegEditLoop`, the Loop subclass that applies/rolls back Windows registry
changes declared in product XML. It was the **first** of the 7 Loop-owner
wrapper classes whose singleton nature was confirmed directly from source in
this documentation effort (see `docs/04_installation_flow.md` §7 and
`docs/ai-context/ownership.yaml`'s `correction_history`), and it remains, with
`pimCopier`, one of only 2 of the 7 confirmed by a full read of both header
and `.cxx` (the other 5 were confirmed from header text alone until this
pass). Structurally it is nearly identical to `pimShortcuts` (see
`docs/classes/pimShortcuts.md`) — same `xmlMutex.TryLock()` gate, same
flag-only `Cancel()` — but with one additional, confirmed **comment/behavior
mismatch** (see Extension Points).

## Responsibilities

- Gate entry into a registry-change operation via `xmlMutex.TryLock()`
  (`pimRegEdit.cxx:32`).
- On success, construct a fresh `pimRegEditLoop`, set its rollback flag if
  uninstalling, and start it asynchronously via `Execute()`
  (`pimRegEdit.cxx:30-51`). Like `pimShortcuts`, no path/property resolution
  is performed here — `pimRegEditLoop` reads directly from the XML it is
  handed.
- Hold `xmlMutex` for the full operation duration, released only by
  `Status()` on `IsDone()` (`pimRegEdit.cxx:101`) or by `Kill()`
  (`pimRegEdit.cxx:140`).

## Dependencies

- `pimRegEditLoop` (the Loop subclass it owns; not given a full class doc in
  this pass — see `docs/01_repository_inventory.md`/`docs/07_registry_usage.md`
  for its behavior, documented at the flow level rather than the class level).
- `threadlibcxx` (`thrMutex`, `thrRWLock`).

## Members

| Member | Type | Purpose |
|---|---|---|
| `OnlyRegEdit` | `static pimRegEdit*` | The singleton instance pointer. |
| `xmlMutex` | `thrMutex` | "One registry operation at a time" gate. |
| `statusMutex` | `thrRWLock` | Guards `RegEditLoop` pointer swaps and `pause_flag`/`cancel_flag`. |
| `pctMutex` | `thrRWLock` | Declared; not referenced in `pimRegEdit.cxx` — dead member, same pattern as its siblings. |
| `pause_flag`, `cancel_flag` | `bool` | Caller-facing intent flags. |
| `InMemoryXML` | `pimXmlFile*` | Declared; not assigned in `pimRegEdit.cxx` — dead member. |
| `RegEditLoop` | `pimRegEditLoop*` | The transiently-owned Loop instance; `NULL` when idle. |
| `host` | `btkString` | Declared with the comment "reserved for future use" (`pimRegEdit.h:30`); never read or written anywhere in `pimRegEdit.cxx` — confirmed unused. |

## Public/Protected APIs

- `static pimRegEdit& GetInstance()` — lazy singleton construction.
- `int Create(pimXmlFile*)` — delegates to `Create_low(xmlPtr, false)`.
- `int Uninstall(pimXmlFile*)` — delegates to `Create_low(xmlPtr, true)`.
- `RegEditStatus Status(int &percent_done)` — like `pimShortcuts::Status()`,
  `percent_done` is accepted but **never written** anywhere in the method
  body (`pimRegEdit.cxx:63-106`) — the same dead-output-parameter pattern
  documented for `pimShortcuts`. Also performs cleanup on first observing
  `IsDone()`.
- `void Pause()` / `void Resume()` — flag-only; no corresponding call into
  `RegEditLoop`.
- `void Cancel()` — sets `cancel_flag` **only** (`pimRegEdit.cxx:122-127`),
  same flag-only shape as `pimShortcuts`/`pimServices`/`pimDownloader`; does
  not call anything on `RegEditLoop`.
- `void Kill()` — if in progress, calls `RegEditLoop->Kill()`, deletes it,
  unlocks `xmlMutex`.

## Private/Protected Utilities

- `pimRegEdit()` (protected ctor) — initializes `statusMutex`/`pctMutex`;
  `InMemoryXML`/`RegEditLoop` to `NULL`.
- `int Create_low(pimXmlFile*, bool uninstall)` (`pimRegEdit.cxx:30-51`) —
  thin setup-and-launch, with the same dead `if (!uninstall) {}` empty stub
  block seen in `pimShortcuts::Create_low()` and `pimServices::Create_low()`
  (`pimRegEdit.cxx:41-43`) — a copy-pasted pattern across all three sibling
  classes.

## Called By

- `pimEntitlement::ApplyRegistryChanges()` (`pimEntitlement.cxx:5574`) —
  `Create()`. Confirmed poll-and-retry usage:
  `while (re_ret == false) { re_ret = RegEdit.Create(xmlPtr); if (!re_ret) Sleep(5); check-cancel }`
  (`pimEntitlement.cxx:5579-5598`).
- A second call site at `pimEntitlement.cxx:7421` (`Uninstall()`, in the
  rollback/uninstall path) — consistent with the "sole caller is
  `pimEntitlement`" pattern shared by all 7 wrappers.

## Calls Into

- `pimRegEditLoop` (constructs, sets `SetRollback()`, `Execute()`s, `Kill()`s,
  queries `HasErrors()`/`IsPaused()`/`IsDone()`/`Wait()` on it — not
  `Cancel()`, same gap as `pimShortcuts`).

## Lifetime

Singleton constructed on first `GetInstance()` call, lives for process
lifetime. `RegEditLoop` is transient per operation; cleaned up by `Status()`
on completion or by `Kill()`.

## Ownership Model

Free-standing process-wide singleton; no owner object. Owns one
`pimRegEditLoop` instance at a time for the duration of one registry-change
operation. This was the original finding (prior to the extension pass
documented in `docs/classes/pimCopyLoop.md`'s Ownership Model correction)
that first established the singleton pattern for this family of classes —
`pimRegEdit` is not a per-entitlement object, and two entitlements applying
registry changes concurrently will serialize through this one instance.

## Thread Safety

- `xmlMutex.TryLock()`/full-duration-hold pattern identical to `pimCopier`/
  `pimShortcuts`.
- **Confirmed `Cancel()` gap** — same as `pimShortcuts`: `Cancel()` sets a
  flag only, `RegEditLoop` keeps running until it finishes naturally or
  `Kill()` is called. UNCONFIRMED in this pass whether the
  `pimEntitlement::ApplyRegistryChanges()` call site implements the same
  grace-period-then-`Kill()` compensating idiom documented for
  `pimShortcuts`/`pimServices`/`pimDownloader` — the code immediately
  surrounding `pimEntitlement.cxx:5574-5598` was read and shows only the
  initial acquire-and-retry loop, not the subsequent completion-polling loop;
  the completion-polling loop was not re-read in this pass to confirm whether
  it contains the same `KILL_COUNT` pattern. Treat as likely-but-unconfirmed
  given the pattern's consistency across the other three flag-only wrappers.

## Extension Points

- **Confirmed comment/behavior mismatch**: `pimRegEdit.h:54-56` comments
  `Create()`/`Uninstall()` as "these WAIT until they are able to start a
  download and then they return immediately" — but `Create_low()`'s actual
  implementation is `xmlMutex.TryLock()` (`pimRegEdit.cxx:32`), which is
  **non-blocking**: it returns `false` immediately if the lock is held,
  exactly like every other wrapper's TryLock-gated methods, and the
  confirmed caller (`pimEntitlement::ApplyRegistryChanges()`) explicitly
  busy-polls with `Sleep(5)` on a `false` return (`pimEntitlement.cxx:5579-5598`)
  — behavior that would be unnecessary if the method actually blocked until
  able to proceed. The header comment describes a design that was either
  never implemented or was changed without updating the comment; **the
  implementation and the confirmed call-site usage are the source of truth,
  not this comment** (per this documentation effort's standing rule to
  prefer implementation over comments/naming). Any future maintainer reading
  only the header should not assume `Create()`/`Uninstall()` block.
- The dead `host` member ("reserved for future use") suggests a
  once-planned, never-implemented feature (perhaps remote-registry editing
  on another host, given the name) — worth checking with the original team
  before removing, but confirmed inert in the current codebase.

## Risk Analysis

- The header/implementation comment mismatch above is a genuine
  documentation-trust hazard: a developer skimming only `pimRegEdit.h` (as
  opposed to tracing `Create_low()` and a real call site) would draw the
  wrong conclusion about this API's blocking behavior.
- Shares the flag-only `Cancel()` risk documented for `pimShortcuts` — same
  caveat applies: do not assume `Cancel()` alone stops an in-flight registry
  operation.
- Dead members `pctMutex`/`InMemoryXML`/`host`, same pattern as its siblings.

## Usage Example

```cpp
// Traced from pimEntitlement::ApplyRegistryChanges(), pimEntitlement.cxx:5572-5598
pimRegEdit& RegEdit = pimRegEdit::GetInstance();
int re_ret = false;
while (re_ret == false)
{
    re_ret = RegEdit.Create(xmlPtr); // NON-BLOCKING despite the header's "WAIT" comment
    if (re_ret == false)
    {
        Sleep(5);
        if (IsCancelFlagSet()) { /* mark cancelled, return false */ }
    }
}
// then poll RegEdit.Status(pct) until Idle/Error
```
