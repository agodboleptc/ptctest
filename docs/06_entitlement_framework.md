# PIM Entitlement Framework

> Synthesizes and cross-references `docs/classes/pimEntitlement.md` (Phase 5),
> `docs/04_installation_flow.md` (Phase 7), and `docs/05_prerequisite_framework.md`
> (Phase 8) into a lifecycle/state-oriented view of `pimEntitlement`, plus new
> evidence on install-state message catalog. No source was modified.

## 1. What an Entitlement Is

One `pimEntitlement` = one installable product, backed by one product-definition XML
document (Xerces DOM, via the inherited `pimLoop::xmlPtr`). It is simultaneously:

- **A data model** — identity (`ID`/`Tag`/`Family`/`PrimaryLicensePrefix`/`ARP`),
  size/version/shipcode accessors, license prefix/source lists for its PSF entry.
- **A dependency-graph node** — both a potential prerequisite of others
  (`IsPrerequisite`) and a holder of its own prerequisites
  (`PreRequisitesToHandle`, see `docs/05_prerequisite_framework.md`), plus a
  parent/child relationship via `ParentEntitlement`/`IsParentOnly()`.
- **A threaded execution unit** (`pimLoop` subclass) — its entire install, update,
  reconfigure, or uninstall runs as one cancelable/pausable background thread.

## 2. Lifecycle

```mermaid
stateDiagram-v2
    [*] --> Constructed: pimSessionInfo::AddEntitlement()
    Constructed --> Initialized: Init(xmlFile) / Init(xml, urls, mediaId)
    Initialized --> SelectionPending: registered in EntitlementArray;\nSetInstallMe()/SetReconfigureMe() from UI or defaults
    SelectionPending --> Running: StartOrRestartInstall() / Reconfigure() / Uninstall()\n(spawns thread via inherited pimLoop::Execute())
    Running --> OnExecuteGate: OnExecute() prerequisite gate
    OnExecuteGate --> WaitingOnPrerequisite: HasPrerequisite() && !HasPrerequisitesInstallSucceeded()
    WaitingOnPrerequisite --> Running: re-invoked once prerequisite completes (external caller's responsibility)
    OnExecuteGate --> Reconfiguring: in_reconfigure
    OnExecuteGate --> Updating: UpdateOfProduct != NULL
    OnExecuteGate --> Installing: fresh install (no update)
    Updating --> Installing: upgrade pre-cleanup done, falls into OnInstall()
    Installing --> Done: pipeline completed (see docs/04_installation_flow.md)
    Installing --> Cancelling: IsCancelFlagSet() observed mid-pipeline
    Installing --> RollingBack: do_rollback / do_install_rollback set
    Cancelling --> RollingBack: only if ROLLBACK_CANCELLED_INSTALL is defined (NOT found in this archive -- see docs/04_installation_flow.md §5)
    Cancelling --> Done: otherwise, stops in place
    RollingBack --> Done: OnRollback() completes
    Reconfiguring --> Done
    Done --> [*]: entitlement object persists in pimSessionInfo's arrays until DropEntitlement()/DropInstalledEntitlement() or session end
```

Notes on transitions confirmed from source:

- **Re-entry into `Running`** after `WaitingOnPrerequisite` is not automatic within
  this class — `OnExecute()` simply `return`s when the gate fails; something external
  (silent mode: `InstallPreReqSilent` in `pim/pim_src/pimTop.cxx`; UI mode: presumably
  `pim_ui`, not traced in this pass) must call `StartOrRestartInstall()` again once the
  prerequisite is satisfied.
- **`Cancelling` → `RollingBack`** is conditionally compiled and — per
  `docs/04_installation_flow.md` §5 — the guarding macro
  (`ROLLBACK_CANCELLED_INSTALL`) is not defined anywhere in this archive, so this
  transition is likely **dead in the current build** unless supplied externally.
- The entitlement object itself is **not destroyed** at the end of a run — it remains
  in `pimSessionInfo`'s `EntitlementArray`/`InstalledArray` so the UI can continue to
  query its final status (`GetInstallStatus`, `GetErrors`, `GetWarnings`).

## 3. Dependency Management

Two independent relationship types exist on `pimEntitlement`, and they are **not the
same mechanism**:

1. **Prerequisite graph** (`PreRequisitesToHandle`/`SoftPreRequisites`) — see
   `docs/05_prerequisite_framework.md` for the full mechanism. This is an
   install-ordering/gating relationship: a dependent entitlement will not proceed
   past `OnExecute()`'s gate until its prerequisites report satisfied (or, for soft
   ones, simply done).
2. **Parent/child** (`ParentEntitlement` string, `IsParentOnly()`,
   `GetRequiredParentEntitlement()`) — a *grouping* relationship (e.g. "Common Files"
   as a parent-only entitlement shared by several real products, seen in
   `pim/pim_src/pimTop.cxx`'s WGM/MKS mode default-install-me logic:
   `!SessionInfo.GetEntitlement(i)->IsParentOnly()` gates whether an entitlement is
   defaulted to install). A parent-only entitlement is not independently selected for
   install by the end user; it exists to be shared/referenced by its children. Its
   exact linkage mechanism (how `ParentEntitlement` is resolved to an actual
   `pimEntitlement*`) was not traced in this pass.
3. **Update relationship** (`UpdateOfProduct`) — a third, distinct kind of reference:
   points at the *previously-installed* `pimEntitlement` this one supersedes. Unlike
   prerequisites, this relationship drives *cleanup* of the old product (uninstall
   registry entry, PTC install record, shortcuts, service — see
   `docs/04_installation_flow.md` §3) rather than gating whether the new install can
   proceed.

## 4. Installation State

State is tracked through **two parallel channels**, not a single enum:

### 4a. In-memory progress (`PctMutex`-guarded, queried by the UI while running)

- `total_pct_done` (int, 0–100) — read via `GetProgress(int&)`, clamped to 99 if
  somehow reported above 100.
- `execute_status_string` (btkString) — a localized, human-readable label read via
  `GetInstallStep(btkString&)`, defaulting to the "pending" message if empty or if
  `msiexec_return` is `"3010"` (the standard Windows Installer "success, reboot
  required" code — treated here as still-pending rather than complete).

### 4b. Persistent XML state (`ENTITLEMENT_STATUS_STRING` = `"status_message"` property
  on the entitlement's own XML, `xmlPtr->DoSave()`d after nearly every pipeline stage)

Every value ever assigned to either channel across the file was cataloged directly
from source (`pim_core/pim_core_src/pimEntitlement.cxx`) — this is the closest thing to
an authoritative state enum for entitlement install status:

| Message ID | When set (evidence) |
|---|---|
| `pimInstallStateInitialize` | Right after generating the Add/Remove Programs uninstall entry, before the main copy/MSI pipeline |
| `pimInstallStateDownloading` | During a network/web-download-driven install (exact trigger not individually re-traced this pass) |
| `pimInstallStateInstalling` | During the main install pipeline (exact trigger not individually re-traced this pass) |
| `pimInstallStateCancelled` | Every `IsCancelFlagSet()` trip point in `OnInstall`/`ApplyRegistryChanges`/`HasPrerequisitesInstallSucceeded` |
| `pimUIInstallStatusPending` | Default/fallback in `GetInstallStep()`; also used while `ApplyRegistryChanges()` waits for the `pimRegEdit` singleton to become available |
| `pimUIInstallStatusPendingPrerequisite` | While `HasPrerequisitesInstallSucceeded()`'s wait loop is still polling |
| `pimUIInstallStatusInstall` | Set right when the main copy step begins (`total_pct_done = 0`) |
| `pimUIInstallStatusDownload` / `pimUIInstallStatusDownloadStatus` | Download-related progress (not individually re-traced this pass) |
| `pimUIInstallStatusErrorMsi` | Set alongside a hard prerequisite-check failure in `HasPrerequisitesInstallSucceeded()` (despite the "Msi" in the name, this is the message used for the general prerequisite-check-failed case too) |
| `pimUIInstallStatusIncompleteMsi` | MSI-specific incomplete state (not individually re-traced this pass) |
| `pimUIInstallStatusComplete` | Set at `total_pct_done = 100` when neither `CA_WARNINGS` nor `CA_WARNINGS_WITH_ERROR` XML properties are present |
| `pimUIInstallStatusCompleteWithWarnings` | Same point, when `CA_WARNINGS` is present |
| `pimUIInstallStatusCompleteWithErrors` | Same point, when `CA_WARNINGS_WITH_ERROR` is present |

Separately, the persistent **"has this ever been installed" marker** is the
`installed="Y"` XML attribute referenced in `pim_core/includes/pimCopyLoop.h`'s
comment ("content that has attrib `installed=Y` will be removed" — consumed by
`pimCopyLoop::SetUninstall()`'s mode) and set on individual CDSECTION nodes after a
successful `pimInstallCab`/copy step (seen in `pim/pim_src/pimTop.cxx`'s pattern of
`((DOMElement*)node)->setAttribute(piminstalled, x_Yes.unicodeForm());` for a
different but structurally identical case). This is a **per-content-item** flag
within the XML, distinct from the whole-entitlement status message above.

## 5. Verification State

As established in `docs/04_installation_flow.md` §8, **no distinctly-named
post-install verification stage** was found in `pimEntitlement.cxx`. What exists
instead are targeted *pre-install* "is this already satisfied" checks that double as
implicit verification when re-run:

- `IsMSISameVersionInstalled()` / `IsSFXSameVersionInstalled()` — same-version check,
  used to skip redundant reinstalls.
- `IsPrerequisiteSatisfied(true)` — as shown in `HasPrerequisitesInstallSucceeded()`
  (`docs/05_prerequisite_framework.md` §5), this same function serves as both the
  pre-install gate *and* the closest thing to post-install verification: it's called
  again after a prerequisite reports `IsDone()`, specifically to catch the case where
  installation "finished" without actually satisfying the check.
- No equivalent re-verification was found for the entitlement's *own* completion
  (i.e., nothing calls something like `IsPrerequisiteSatisfied`-for-self after
  `OnInstall()` finishes) — completion status is entirely inferred from pipeline
  return values and `CA_WARNINGS[_WITH_ERROR]` properties (§4b), not from an
  independent check of resulting machine state.

## 6. Failure State

- **Textual accumulation** (inherited from `pimLoop`): `Errors`/`Warnings` buffers,
  appended to via `AppendError`/`AppendWarning`, exposed via `GetErrors`/`GetWarnings`.
  These are newline-joined strings, not structured error objects (see
  `docs/classes/pimLoop.md` Risk Analysis).
- **Global error-code stack** (from `pim/pim_src/pimExit.cxx`, module `pim`):
  `pimHitError(code)` pushes onto a process-wide stack independent of any one
  entitlement; `pimGetLastError()` reads the most recent. `PIM_PREQUISITE_NOT_SATISFIED`
  is the specific code pushed by `HasPrerequisitesInstallSucceeded()`'s hard-failure
  path (`docs/05_prerequisite_framework.md` §5). This is a **separate failure channel**
  from the per-entitlement `Errors` buffer — a caller needing full failure context must
  consult both.
- **XML-flagged warnings that escalate status**: `CA_WARNINGS`/`CA_WARNINGS_WITH_ERROR`
  properties (set by custom-action processing, not traced to their setter in this
  pass) directly determine which of the three "Complete*" status messages is chosen at
  the end of `OnInstall()` (§4b) — i.e., warnings recorded during custom actions can
  downgrade an otherwise-successful install's reported status without the pipeline
  itself having returned `false` anywhere.

## 7. Rollback State

- **Trigger flags**: `do_rollback` (explicit uninstall/rollback request) and
  `do_install_rollback` (set only under the `ROLLBACK_CANCELLED_INSTALL` compile
  guard — see `docs/04_installation_flow.md` §5 for why this may be inert in the
  current build). Both are checked identically in `OnExecute()`:
  `if (do_rollback || do_install_rollback) OnRollback(do_install_rollback);` — the
  `from_during_install` parameter it's called with is exactly the
  `do_install_rollback` flag's value, letting `OnRollback()` distinguish "user asked
  to uninstall" from "this failed mid-install and we're unwinding."
- **Rollback sequence**: `OnUninstallCustomActions("PreUninstall")` →
  `UninstallShortcuts()` → (untraced middle section, presumed to mirror the install
  pipeline in reverse) → registry rollback via the **`pimRegEdit` singleton**
  (`docs/04_installation_flow.md` §7 — this is a correction to the ownership model
  assumed in earlier phases, since `pimRegEdit` is *not* a per-entitlement owned
  wrapper like the other 6 Loop-owner classes).
- **No independent "rollback state" enum** was found distinct from the general
  install-state messages in §4b (`pimInstallStateCancelled` is reused rather than a
  dedicated "RollingBack" message existing).

## 8. Summary Table: State Touchpoints by Concern

| Concern | Mechanism | Where |
|---|---|---|
| Am I a prerequisite / do I have prerequisites | `IsPrerequisite`, `HasPrerequisite`, `PreRequisitesToHandle` | `docs/05_prerequisite_framework.md` |
| Should I install right now | `GetInstallMe`/`SetInstallMe`, gated by `HasPrerequisitesInstallSucceeded()` | `docs/04_installation_flow.md` §1 |
| What am I doing right now (UI) | `execute_status_string` / `total_pct_done` via `GetInstallStep`/`GetProgress` | §4a above |
| What happened, persistently | `ENTITLEMENT_STATUS_STRING` XML property, `installed="Y"` attributes | §4b above |
| Did anything go wrong | `Errors`/`Warnings` (per-entitlement), `pimHitError`/`pimGetLastError` (process-wide) | §6 above |
| Should I undo what I did | `do_rollback`/`do_install_rollback`, `OnRollback` | §7 above |

---
*Phase 9 of the requested 20-phase documentation set. No source was modified.*
