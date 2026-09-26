# Class: `pimLoop`

**File:** `pim_core/includes/pimLoop.h` (79 lines) / `pim_core/pim_core_src/pimLoop.cxx` (248 lines)
**Module:** `pim_core`

## Purpose

Abstract base class giving every install "step" (and the top-level product unit,
`pimEntitlement`) a uniform, thread-backed, cancelable/pausable execution contract.
This is the single mechanism through which all install/uninstall/rollback work in PIM
actually runs.

## Responsibilities

- Own the threaded execution contract (`Execute`, `Cancel`, `Pause`/`Resume`, `Wait`,
  `Kill`).
- Own progress/completion state (`mInProgress`, `mDone`, `mCancel`, `mPause`) behind a
  mutex.
- Accumulate and expose error/warning text (`Errors`, `Warnings` — plain
  newline-joined `btkString` buffers, not structured lists).
- Hold a `pimXmlFile*` (`xmlPtr`) — every Loop operates against one XML document
  (the product/entitlement definition it's executing steps from).
- Provide a hook for unhandled exceptions in the worker thread (`OnUnhandled`),
  which crashes the process via `btkCrash` rather than trying to recover.

## Dependencies

- `thrThread` (base class, external `btk` toolkit) — supplies the actual OS thread
  mechanics (`Execute(thrAttached)`, `Terminate()`, `Wait()`).
- `thrMutex`, `thrSemaphore`, `thrRecursive` (external `btk` threading primitives).
- `pimXmlFile` (composition — holds a pointer, does not own/delete it based on visible
  code; ownership of the pointed-to `pimXmlFile` lies with the caller/subclass
  constructor argument).
- `btkString` (error/warning buffers).
- `msgID_sput_buffer` (external, `imp_pim_dll.h`) — used by the message-ID overloads of
  `AppendError`/`AppendWarning` to resolve a localized string before appending.

## Members

| Member | Type | Access | Purpose |
|---|---|---|---|
| `mCancel`, `mDone`, `mPause`, `mInProgress` | `bool` | protected | Lifecycle flags, all mutex-guarded on read/write |
| `xmlPtr` | `pimXmlFile*` | protected | The XML document this Loop instance operates on |
| `pausePtr` | `thrSemaphore*` | protected | Lazily allocated on first `Pause()`; used to block the worker thread until `Resume()` increments it |
| `Errors`, `Warnings` | `btkString` | protected | Accumulated diagnostic text, newline-joined |
| `Mutex` | `thrMutex` (recursive) | protected | Guards all the above |

## Public APIs

| Method | Signature | Behavior |
|---|---|---|
| `pimLoop(pimXmlFile*)` | ctor | Initializes all flags false, stores `xmlPtr` |
| `SetXml(pimXmlFile*)` | `void` | Repoints `xmlPtr` |
| `Execute()` | `virtual void` | Resets state flags + `Errors`/`Warnings`, then calls `thrThread::Execute(thrAttached)` to spawn the worker thread, which invokes the subclass's `OnExecute()` |
| `Cancel()` | `virtual void` | `Resume()`s first (to unblock if paused), then sets `mCancel = true` under the mutex — cooperative cancellation; the subclass's `OnExecute()`/`OnInstall()` etc. must poll `IsCancelFlagSet()` |
| `IsCancelFlagSet()`, `IsInProgress()`, `IsDone()`, `IsPaused()` | `virtual bool` | Mutex-guarded flag reads |
| `Pause()` | `virtual void` | Sets `mPause = true`, lazily creates `pausePtr` semaphore (actual blocking/`Decrement()` call happens in the subclass's main loop, not here — not fully traceable in `pimLoop.cxx` alone) |
| `Resume()` | `virtual void` | If paused, clears `mPause` and `pausePtr->Increment()` to unblock |
| `HasErrors()`/`GetErrors(btkString&)`, `HasWarnings()`/`GetWarnings(btkString&, bool remove_hyperlinks=false)` | `virtual` | Mutex-guarded reads; `GetWarnings` optionally strips `<a href="...">` hyperlink markup via `std::regex` |
| `Kill()` | `virtual void` | Calls `thrThread::Terminate()` (hard stop — external), then if `xmlPtr->WriteLockEngaged()`, calls `xmlPtr->PostWrite()` to release the XML write lock the thread may have held |
| `Wait()` | `virtual void` | Delegates to `thrThread::Wait()` — blocks the calling thread until this Loop's thread finishes |

## Protected APIs

| Method | Purpose |
|---|---|
| `OnUnhandled(char*)` | Called by the `thrThread` framework (external) on an unhandled exception in the worker thread; logs via `LG_ERROR`, sets `mInProgress = false`, then calls `btkCrash(...)` — **this terminates the process**, it is not a recoverable path |
| `AppendError`/`AppendWarning` (both string and message-ID overloads) | Mutex-guarded appends to `Errors`/`Warnings`, with an optional `prefix_label` prepended (`"<label> - <text>"`) |
| `ClearInProgress()` | Sets `mInProgress = false` under the mutex |

## Private Utilities

None declared in the header beyond what's listed above — `pimLoop` has no private
section; everything is `protected` or `public`.

## Called By

Every concrete Loop subclass's public API is exercised by its owner (see
`docs/02_architecture_overview.md` §5): `pimEntitlement` (which is itself a
`pimLoop`) is driven directly by `pim/pim_src/pimTop.cxx` (`Wait()` is called on every
entitlement at shutdown); the 7 wrapper classes (`pimCopier`, `pimMSICopier`, etc.)
drive their respective `*Loop` instances from within `pimEntitlement.cxx`'s install
pipeline (`StartCopy`, `InstallMSI`, `ApplyRegistryChanges`, etc. — see
`docs/classes/pimEntitlement.md`).

## Calls Into

`thrThread` (base class methods `Execute`, `Terminate`, `Wait` — external),
`thrSemaphore`/`thrMutex` (external), `btkCrash` (external), `msgID_sput_buffer`
(external), `std::regex` (standard library, for hyperlink stripping in
`GetWarnings`).

## Lifetime

A `pimLoop`-derived object's lifetime is owned by whichever class constructs it —
either directly on the stack/heap by `pimEntitlement.cxx` (for `pimScriptLoop`/
`pimPsfLoop`, created and destroyed per call) or held as a member pointer inside one
of the 7 owner-wrapper classes (`CopyLoop`, `MSILoop`, `SFXLoop`, `ShortcutLoop`,
`RegEditLoop`, `ServiceLoop`, `DownloadLoop`), lazily constructed on first use
(`XxxLoop = XNew pimXxxLoop(xmlPtr);`) and presumably destroyed with the owner
(destructor bodies not verified in this pass).

## Ownership Model

`pimLoop` does not own its `xmlPtr` — it is handed one by the constructor and never
frees it. The worker thread itself is owned by the external `thrThread` base class
machinery.

## Thread Safety

All mutable shared state (`mCancel`/`mDone`/`mPause`/`mInProgress`, `Errors`,
`Warnings`) is accessed only under `Mutex` (a recursive mutex, `thrRecursive`),
consistently in both `pimLoop.cxx` and (per the pattern) subclasses. `xmlPtr` itself
is **not** mutex-protected at this level — `pimXmlFile` has its own internal
`thrRWLock`/`PreRead`/`PreWrite` locking (see `docs/classes/pimXmlFile.md`), which is
what actually protects concurrent access to the underlying DOM document across
threads.

## Extension Points

Any new install-step type is created by subclassing `pimLoop` and overriding (by
convention, not enforced by virtual methods on `pimLoop` itself — `OnExecute` is not
declared virtual on `pimLoop`, meaning each subclass independently defines its own
`OnExecute`/`OnInstall`/`OnRollback`/`OnTerminate` methods that `Execute()`'s call into
`thrThread::Execute` presumably reaches via a `thrThread`-level virtual dispatch,
**not verified in this pass** — `thrThread`'s interface is external/not included).

## Risk Analysis

- **`OnUnhandled` crashes the process** on any uncaught exception in a worker
  thread — there is no per-step error isolation; a bug in, say, `pimScriptLoop`'s
  `OnExecute()` can take down the entire installer, not just that one step.
- **`Errors`/`Warnings` are unstructured strings**, not structured error objects —
  callers can only display/log them, not programmatically inspect what failed (no
  error codes, no per-item detail beyond string concatenation).
- **`Kill()`'s XML-lock release is a targeted workaround** ("if
  `xmlPtr->WriteLockEngaged()` call `PostWrite()`"), suggesting a known historical bug
  class (killing a thread mid-write could otherwise leave the XML write lock stuck) —
  any new subclass that acquires additional locks around `xmlPtr` outside the
  `PreWrite`/`PostWrite` pattern would not be covered by this safeguard.
- `Pause()`'s actual blocking (`pausePtr->Decrement()`) is **not in this class** — it's
  presumably done by convention inside each subclass's execution loop; a new subclass
  author must remember to poll `IsPaused()` and decrement the semaphore themselves, or
  `Pause()` will silently have no effect for that step type.

## Usage Example (as evidenced by call sites, not authored for this doc)

```cxx
// pim_core/pim_core_src/pimMSICopier.cxx (pattern, not full listing)
MSILoop = XNew pimMSILoop(xmlPtr);
MSILoop->Execute();       // spawns worker thread -> pimMSILoop::OnExecute()
...
MSILoop->Wait();          // block until the MSI install thread finishes
if (MSILoop->HasErrors()) { btkString errs; MSILoop->GetErrors(errs); /* log/report */ }
```

---
*Phase 5 of the requested 20-phase documentation set.*
