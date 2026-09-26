# Class: `pimServiceLoop`

**File:** `pim_core/includes/pimServiceLoop.h` (52 lines) / `pim_core/pim_core_src/pimServiceLoop.cxx` (596 lines)
**Module:** `pim_core`
**Inherits:** `pimLoop`

## Purpose

Install-step class that creates, starts, stops, and removes Windows services
declared in an entitlement's XML.

## Responsibilities

- Dispatch on a **externally-set, class-static action** (`pimServices::getServiceAction()`)
  to decide what `OnInstall()` actually does: `InstallAction` (create + start),
  `StartAction` (start only), or `StopAction` (stop only).
- Create a service either via the Windows-default mechanism (`ginst_install_service`,
  an external PTC-platform import from `imp_pim_dll.h`) when no explicit
  `<INSTALL_COMMAND>` is given, or by running an arbitrary shell command
  (`pimSystemCall`) when one is.
- Symmetrically support removal via `ginst_remove_service` or an explicit
  `<UNINSTALL_COMMAND>`.
- Query service presence/running-state via `ginst_is_service_installed`/
  `ginst_is_service_running`.
- On rollback: stop a running service (via `<STOP_COMMAND>` if present, else the
  default mechanism), then uninstall it (via `<UNINSTALL_COMMAND>` if present,
  else the default mechanism), marking `installed="N"` on success. Skips services
  already marked `installed="N"`.

## Dependencies

- `pimLoop` (base class).
- `pim/includes/imp_pim_dll.h`'s imported `ginst_*` functions (external PTC
  platform DLL, identity unknown — see `docs/01_repository_inventory.md` §5) —
  **this class is the confirmed, concrete consumer of those imports**, closing a
  gap left open in earlier phases about what actually calls them.
- `pimSystemCall` (`pim_util`) — generic shell-command execution, used as the
  alternative to the `ginst_*` default mechanism whenever the XML supplies an
  explicit command.
- `pimConvert` (property resolution in install/uninstall/start/stop commands).
- `pimServices` (friend class — see Ownership Model).

## Members

| Member | Type | Access | Purpose |
|---|---|---|---|
| `do_rollback` | `int` | private | Selects install vs. rollback in `OnExecute` |

## Public APIs

| Method | Purpose |
|---|---|
| `pimServiceLoop(pimXmlFile*)` | Ctor; `do_rollback = false` |
| `SetRollback(bool)` | Marks this Loop for rollback behavior |

`friend class pimServices;` — grants the owning singleton direct access to this
class's otherwise-protected members (consistent with `pimServices` being the sole
driver of this Loop type).

## Protected APIs

| Method | Purpose |
|---|---|
| `OnExecute()` | Dispatches to `OnInstall()` or `OnRollback()` |
| `OnInstall()` | Switches on `pimServices::getServiceAction()`: `InstallAction` → `CreateServicesFromXml()` then `StartServicesFromXml()`; `StartAction` → `StartServicesFromXml()` only; `StopAction` → `StopServicesFromXml()` only |
| `OnRollback()` | Per `<SERVICE installed!="N">` node: stop (if running) then uninstall, using XML-declared commands where present, else the `ginst_*` defaults |
| `OnTerminate()` | Standard thread teardown |
| `CreateServicesFromXml()` / `StartServicesFromXml()` / `StopServicesFromXml()` | Iterate `<SERVICE>` nodes and dispatch to the corresponding per-service method below |
| `CreateService(name, install_cmd, exe, Converter)` | `ginst_install_service(name, exe)` if no `install_cmd`; else `pimSystemCall(install_cmd, ...)` |
| `RemoveService(name, uninstall_cmd, Converter)` | Symmetric to `CreateService` |
| `HasService(name)` | `ginst_is_service_installed(name)` |
| `IsRunning(name)` | `ginst_is_service_running(name)` |
| `StartService(name, start_cmd, Converter)` / `StopService(name, stop_cmd, Converter)` | Same default-vs-explicit-command pattern as Create/Remove |

## Private Utilities

None beyond the protected helpers above.

## Called By

`pimServices::Create_low()` via the action dispatch (see Ownership Model).

## Calls Into

`ginst_install_service`, `ginst_remove_service`, `ginst_is_service_installed`,
`ginst_is_service_running` (all external), `pimSystemCall` (`pim_util`),
`pimConvert`, `pimXmlFile`.

## Lifetime

Created on demand by `pimServices`, lives for one service operation.

## Ownership Model

**Correction relative to this documentation set's earlier assumption**: `pimServices`
(`pim_core/includes/pimServices.h`) is a **process-wide singleton**
(`static pimServices *OnlyServices`, `GetInstance()`), following the same pattern
confirmed against `pimCopier.cxx` — not per-entitlement as originally documented.

**Additional wrinkle unique to this pair**: the "current action" a
`pimServiceLoop::OnInstall()` will perform is communicated via a **class-static
member on `pimServices`** — `static ServiceAction serviceAction;` — read through
`pimServices::getServiceAction()` and set via `setServiceAction(ServiceAction)`,
rather than being passed as a constructor or method argument the way `pimCopyLoop`
receives its uninstall/rollback flags. Since `pimServices` is already a singleton,
this doesn't introduce a *new* sharing concern in practice, but it is a
structurally different (and less discoverable) way of passing that one piece of
state than every other Loop-owner pair in the codebase uses, and it means the
action must be set **before** `Execute()` is called on a given `pimServiceLoop`
instance, with no compiler-enforced ordering guarantee.

## Thread Safety

Inherits `pimLoop`'s mutex-guarded lifecycle flags. The `pimServices::serviceAction`
static member itself has no visible mutex guarding it in this class — its safety
depends entirely on the owning singleton's own `TryLock` serialization preventing
more than one `Create()`/action sequence from running at a time.

## Extension Points

- New service action: extend the `ServiceAction` enum on `pimServices` and the
  `switch` in `pimServiceLoop::OnInstall()`.
- New service XML element (beyond `<INSTALL_COMMAND>`/`<UNINSTALL_COMMAND>`/
  `<START_COMMAND>`/`<STOP_COMMAND>`): extend the child-node search pattern used
  in `OnRollback()`/the `CreateServicesFromXml`-family methods.

## Risk Analysis

- **`ServiceAction` communicated via a static member, not a parameter** — a future
  refactor that makes `pimServices` no longer a strict singleton (e.g. to allow
  concurrent service operations) would need to also refactor this action-passing
  mechanism, since a class-static field cannot safely carry per-call state across
  concurrent instances.
- **`OnRollback()`'s stop-then-uninstall sequence uses fixed `Sleep(2)`/`Sleep(5)`
  waits** ("give the OS a moment") rather than polling for actual state
  transitions — a slow-to-stop service could still be transitioning when the
  uninstall command runs, risking a failed removal that looks like a timing issue
  rather than a real error.
- This class is the confirmed real consumer of `ginst_*` functions from the
  unidentified external PTC platform DLL (`imp_pim_dll.h`) — any investigation
  into what that DLL actually is should start from these exact call sites.

## Usage Example (as evidenced by call sites)

```cxx
// pattern inside pimEntitlement::PerformServiceAction(sa)
pimServices &Svc = pimServices::GetInstance();
Svc.setServiceAction(sa);   // e.g. pimServices::InstallAction
int ret = Svc.Create(xmlPtr);   // 0 if another service op is in progress process-wide
```

---
*Extends Phase 5 of the requested 20-phase documentation set (remaining Loop subclasses, done at the same depth as `pimCopyLoop`).*
