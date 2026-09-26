# Class: `pimEntitlement`

**File:** `pim_core/includes/pimEntitlement.h` (334 lines) / `pim_core/pim_core_src/pimEntitlement.cxx` (7,649 lines — largest file in the codebase)
**Module:** `pim_core`
**Inherits:** `pimLoop` (→ `thrThread`, external)

> **Further pass (`OnReconfigure()`/`OnRollback()` fully traced)**: this
> class's own doc had left `OnReconfigure()`'s body and the middle section
> of `OnRollback()` untraced (flagged in `docs/04_installation_flow.md` §4,
> §6, §9). Both are now read in full (`OnReconfigure()`:
> `pim_core_src/pimEntitlement.cxx:6866-7222`, 357 lines; `OnRollback()`:
> `:7287-7565`, 279 lines). Headline finding: a **CONFIRMED BUG, HIGH
> SEVERITY** in `OnReconfigure()` — an unconditional `Erase()` of the
> entitlement's own cached XML file, as a side effect of computing an
> unrelated path, with 2 confirmed early-return paths that leave the
> deleted file never recreated. See Risk Analysis.

## Purpose

Represents one installable "product" (an entitlement, backed by one product-definition
XML file) and is the central orchestrator for that product's entire lifecycle: install,
update, reconfigure, uninstall, and rollback. Because it derives from `pimLoop`, an
entire product install/uninstall runs as one cancelable, pausable, threaded unit —
`pimEntitlement` *is* the top-level install-step object; the CAB/MSI/SFX/registry/
shortcut/service/script/PSF operations underneath it are sub-steps it drives.

## Responsibilities

- Identify itself: `ID`, `Tag`, `Pref`, `PrimaryLicensePrefix`, `Family`,
  `ParentEntitlement`, `ARP` — all derived from the product XML during `Init()`.
- Drive the full install pipeline as an ordered sequence of protected `On*` methods
  (see Protected APIs) — copy/extract, MSI, registry, scripts, services, PSF,
  shortcuts.
- Own and lazily create the 7 wrapper objects that each own exactly one `pim*Loop`
  worker (`pimCopier`→`pimCopyLoop`, and by the same pattern in other `.cxx` files:
  `pimMSICopier`→`pimMSILoop`, `pimSFXCopier`→`pimSFXLoop`, `pimShortcuts`→
  `pimShortcutLoop`, `pimRegEdit`(wrapper)→`pimRegEditLoop`, `pimServices`→
  `pimServiceLoop`, `pimDownloader`→`pimDownloadLoop`) — only `IamInCopy`
  (`pimCopier*`) is a named member in the header; the others are presumably members
  too but weren't enumerated as private fields in the visible header (some may be
  local/static within `pimEntitlement.cxx`, **not fully verified in this pass**).
- Create and drive `pimScriptLoop`/`pimPsfLoop` directly and transiently (no
  persistent member/wrapper — confirmed via grep in
  `docs/02_architecture_overview.md` §5: 8 instantiation sites across
  `pimEntitlement.cxx`).
- Own the **prerequisite graph**: register other `pimEntitlement*` instances as hard
  or soft prerequisites (`PreRequisitesToHandle`, a `btkMap<char*, pimEntitlement*>`,
  plus `SoftPreRequisites`, a `StringXArray` of IDs), and answer whether they're
  satisfied.
- Track update/upgrade relationships to a previously-installed version
  (`UpdateOfProduct`, `update_from_version`, `update_from_shipcode`,
  `update_from_path`).
- Track web-download backup state for URL rewriting during network installs
  (`current_download_url_references`, `DownloadXmlBackups` — a
  `btkMap<char*, pimEntDlBackup*>`).
- Report progress/size/status to the UI (`GetProgress`, `GetInstallStep`, `GetSize`,
  `GetDownloadSize`, `GetLabel`, `GetInstallStatus`).
- Manage license-source/feature lists that get written into the product's PSF entry
  (`GetLicensePrefixes`, `GetLicenseSources`, `AppendLicenseSource`,
  `ReplaceLicenseSource`, `ClearLicenseSource[sNFeatures]`, `AppendLicenseFeatures`).
- Manage Quality Agent opt-in/requirement flags (`SetQualityAgent`,
  `SetQualityAgentRequired`, `IsQualityAgentEnabled`, `CanEnableQualityAgent`).
- Support the "Customize" UI dialogs via `SupportCustomize(CustomizeAttribs)` and a
  backup/restore pair (`BackupFile`/`RestoreFromBackup`) so a canceled customize
  dialog can revert the product XML.

## Dependencies

- `pimLoop` (base class — threading/cancel/progress contract).
- `pimCopier` (owns the copy/archive-extraction sub-step).
- `pimServices` (owns the Windows-service sub-step).
- `pimSessionInfo` (included; entitlements are held in and reference back to session
  state, e.g. license source lookups).
- `pimConvert` (`SetupConverter` utility function declared alongside this class —
  resolves XML property placeholders like `[INSTALLBASE]` against session state).
- `pimXmlFile` (every entitlement wraps one product-definition XML, inherited as
  `xmlPtr` from `pimLoop`).
- Transitively, everything the 7 sub-step wrapper classes depend on (MSI, CAB/ZIP,
  FlexNet, registry, Win32 services).

## Members (private, from header)

| Member | Type | Purpose |
|---|---|---|
| `ID`, `Tag`, `Pref`, `PrimaryLicensePrefix`, `Family`, `ParentEntitlement`, `ARP` | `btkString` | Identity fields parsed from the product XML |
| `parent_only`, `required`, `prerequisite`, `do_rollback`, `do_install_rollback`, `in_reconfigure`, `msi_same_version`, `sfx_same_version`, `msi_is_update` | `bool` | State flags controlling which pipeline branch runs |
| `IamInCopy` | `pimCopier*` | Owned copy/archive sub-step wrapper |
| `IamInMSI` | `bool` | Flag, not a pointer — actual MSI wrapper member not named in the visible header (likely declared/used only in the `.cxx`, **UNKNOWN exact field**) |
| `PctMutex`, `total_pct_done`, `download_pct_done` | `thrMutex`, `int` | Progress reporting state, separately mutex-guarded from `pimLoop`'s own `Mutex` |
| `execute_status_string` | `btkString` | Human-readable current step label (backs `GetInstallStep`) |
| `out_shipcode_mathcad` | `btkString` | Mathcad-specific shipcode output |
| `backupFile`, `OriginalFile` | `btkFSEntry` | Customize-dialog backup/restore paths |
| `UpdateOfProduct` | `pimEntitlement*` | Reference (not owned) to the previously-installed entitlement being updated |
| `update_from_version`, `update_from_shipcode`, `update_from_path` | `btkString` | Prior-version metadata |
| `current_download_url_references` | `pimXmlFile*` | Active download-URL XML during a network install |
| `currentUrlRoot` | `btkString` | Root URL prefix for the current download session |
| `DownloadXmlBackups` | `DownloadXmlMap` (`btkMap<char*, pimEntDlBackup*>`) | Per-media-ID backup of download URL state, restorable across timeout/retry |
| `PreRequisitesToHandle` | `PreRequisiteMap` (`btkMap<char*, pimEntitlement*>`) | Hard/soft prerequisite entitlement references |
| `SoftPreRequisites` | `StringXArray` | IDs of prerequisites treated as "soft" (advisory, not blocking) |

## Public APIs (selected — full list in header)

Grouped by concern (see `pim_core/includes/pimEntitlement.h` lines 200–322 for the
complete signatures):

- **Init/identity**: `Init(entitlementXmlFile, root_match, cd_creator)`,
  `Init(application_definition, download_url_references, MediaID)`,
  `UpdateMediaUrls()`, `CacheInstaller()`, `RemoveCache()`, `RefreshXmlNodes()`,
  `GetID()`, `GetTag()`, `GetPref()`, `GetARP()`, `GetPrefix()`, `GetFamily()`,
  `GetRequiredParentEntitlement()`, `OriginalXMLFilePath()`.
- **Install-me / reconfigure-me flags**: `SetInstallMe`/`GetInstallMe`,
  `SetReconfigureMe`/`GetReconfigureMe`, `AreWeAnUpdate(...)`.
- **MSI/SFX query**: `IsMSIInstall`, `IsMSIHybrid`, `HasMSIInstall`,
  `IsMSISameVersionInstalled`, `IsSFXSameVersionInstalled`, `IsMSIAnUpdate`,
  `IsMSINewInstall`, `pimIsSameMSIMajorVersionInstalled`, `HasEngNotebookInstall`,
  `IsSFXInstall`.
- **Classification**: `IsRequired`, `IsPrerequisite`, `IsParentOnly`,
  `IsWGMAdaptorInstall`.
- **Prerequisite graph**: `HasPrerequisite`, `IsPrerequisiteSatisfied`,
  `PrerequisiteNeeded`, `IsOnlySoftPrerequisiteNeeded`, `AddPrerequisite`,
  `DropPrerequisite`, `RefreshPrerequisites`, `GetNextPrerequisite`,
  `IsSoftPrerequisite`, `HasPrerequisitesInstallSucceeded`.
- **Info for UI**: `GetLabel`, `GetSize`, `GetLocalSizeEstimate`, `GetSizeString`,
  `GetDownloadSize`, `GetDownloadSizeString`, `GetVersion`, `GetVersionShipcode`,
  `GetShipcode`, `GetInstallStatus`, `GetDownloadStatus`.
- **Customize support**: `SupportCustomize`, `BackupFile`, `RestoreFromBackup`,
  `IsQualityAgentRequired`, `SetQualityAgent`, `SetQualityAgentRequired`,
  `IsQualityAgentEnabled`, `CanEnableQualityAgent`.
- **Loadpoint/eligibility**: `SetInstallPoint`, `GetInstallPoint`,
  `CheckOKToInstall`, `CheckOKToUninstall`, `IsWGMExesRunning`, `IsUpdateChanged`.
- **License/PSF data**: `GetLicensePrefixes`, `GetLicenseSources`,
  `AppendLicenseSource`, `ReplaceLicenseSource`, `ClearLicenseSource`,
  `ClearLicenseFeatures`, `AppendLicenseFeatures`, `ClearLicenseSourcesNFeatures`,
  `LicenseSourcesHasBeenInit`, `InitRequiredSet`.
- **Lifecycle control**: `StartOrRestartInstall`, `Reconfigure`, `IsInRollback`,
  `Uninstall`, `Cancel`, `IsCancelFlagSet`, `IsDone`, `HasErrors`, `HasWarnings`,
  `Kill`, `CanCancelMSI`.
- **Progress reporting**: `GetErrors`, `GetWarnings`, `GetInstallStep`,
  `GetProgress`.
- **XML access**: `GetXMLPtr()`, `GetUpdateXMLPtr()`.

`friend class upimDlg; friend class upimSilentProgress;` — the uninstall dialog and
its silent-mode progress counterpart get direct access to private members, bypassing
the public API (tight coupling, see Risk Analysis).

## Protected APIs

The actual install pipeline, invoked (in some order determined inside
`pimEntitlement.cxx`, not fully traced in this pass) from `OnExecute()`:

| Method | Role |
|---|---|
| `OnExecute()` | Thread entry point (invoked by `pimLoop::Execute()` via `thrThread`) |
| `OnInstall()` | Fresh install path |
| `OnUpdate(bool update_cdsections)` / `UpdateSizes(bool)` | Update-existing-install path |
| `OnReconfigure()` | Reconfigure path — fully traced, see Risk Analysis for a confirmed cache-file-deletion bug. Stage order (confirmed): optional `RollbackMe`-driven teardown of the previously-installed config (Services stop / Shortcuts remove / PSF rollback / Scripts rollback, each gated by a `No*` property read off the ORIGINAL as-shipped `.p.xml`) → `OnInstallCustomActions("PreReconfigure")` → PTC install-record update (`QualityAgentOptIn` registry value) → optional cache-file relocation (`NoLPXmlLog` gated) → Scripts recreate → PSF recreate → Services start → `OnInstallCustomActions("PostReconfigure")` → Shortcuts recreate → done. Cancellation is checked at the same points as `OnInstall()`, each followed by `xmlPtr->DoSave()` before returning — except 2 non-cancellation failure paths, see Risk Analysis. |
| `OnRollback(bool from_during_install)` | Rollback path (both mid-install-failure and explicit rollback) — fully traced, see Risk Analysis. Stage order (confirmed, resolving `docs/04_installation_flow.md` §6's speculation): `OnUninstallCustomActions("PreUninstall", false, false)` (failure only logged, does NOT abort — contrast with `OnReconfigure()`'s fail-fast custom-actions handling) → optional cache-location reset back to Application Data (`NoLPXmlLog` gated, no `Erase()` side effect — contrast with `OnReconfigure()`'s equivalent block) → `UninstallShortcuts()` → PSF rollback → Scripts rollback → Services uninstall → RegEdit uninstall → Copier rollback/uninstall → PTC install-record removal → uninstall-registry-key removal → (only when `from_during_install`) empty "Common Files" directory cleanup for `creobase.xml`. Confirmed exactly 2 `from_during_install`-specific branches exist in the whole function (Copier's `Rollback()` vs. `Uninstall()` method choice, and the final directory-cleanup gate) — resolving the doc's "not traced in full" note. Confirmed: this function has **zero early-return points** — a single, uninterrupted sequential run to completion, unlike `OnInstall()`/`OnReconfigure()`'s many early exits, consistent with a rollback path's need to attempt every teardown step regardless of an individual step's failure. |
| `OnTerminate()` | Thread teardown |
| `OnInstallCustomActions(when, return_on_errors, check_cancel)` / `OnUninstallCustomActions(...)` | Runs XML-declared custom actions at named lifecycle points (`when`) |
| `StartCopy()` / `WaitForCopyToComplete()` | Drives `IamInCopy` (`pimCopier`→`pimCopyLoop`) |
| `InstallMSI()` | Drives the MSI sub-step |
| `ApplyRegistryChanges()` | Drives the registry sub-step |
| `InstallScripts()` | Drives a transient `pimScriptLoop` |
| `PerformServiceAction(pimServices::ServiceAction sa = InstallAction)` / `UninstallService()` | Drives the `pimServices`→`pimServiceLoop` sub-step |
| `InstallPSF()` | Drives a transient `pimPsfLoop` |
| `InstallShortcuts()` / `UninstallShortcuts()` | Drives the `pimShortcuts`→`pimShortcutLoop` sub-step |
| `AreWeAnUpdate(out_name, out_ver, out_shipcode, out_xmlfile)` (protected overload) | Internal update-detection helper backing the public `AreWeAnUpdate(bool)` |
| `GenerateUninstallName(btkString&)` | Builds the uninstall registry display name |
| `UpdateUrls(pimXmlFile*, btkString&)` | Rewrites download URLs against a new root (implementation seen in `docs/03_application_startup.md`-adjacent research: matches trailing filename to `<CDSECTION>` entries) |
| `IsExeService(btkFSEntry&, btkString&)` | Detects whether an EXE payload is actually a Windows service |
| `pimCleanupUninstall(btkWString curDir, bool group_flag)` | Post-uninstall directory cleanup |

## Private Utilities

No separately-named "utility" section beyond the protected pipeline methods above —
`pimEntitlement` has no private methods, only private data members (all listed under
Members).

## Called By

- `pimSessionInfo::AddEntitlement(...)` constructs and registers instances.
- `pim/pim_src/pimTop.cxx` reads entitlement metadata (`GetFamily`, `GetTag`,
  `SetQualityAgent`, `SetInstallMe`, `GetXMLPtr`) while building the session, and calls
  `Wait()` on every entitlement at shutdown.
- `pim_ui` dialogs (`pimInstallMgrDlg` and friends) drive `StartOrRestartInstall`,
  `Cancel`, `GetProgress`, `GetInstallStep`, etc. to run and display installs
  (**not individually traced this pass** — see `docs/modules/pim_ui.md`).
- `InstallPreReqSilent` (`pim/pim_src/pimTop.cxx`) walks `GetNextPrerequisite`/
  `IsPrerequisiteSatisfied`/`StartOrRestartInstall` for silent-mode prerequisite
  installation.

## Calls Into

`pimCopier`, `pimServices`, and (by the established pattern) `pimMSICopier`,
`pimSFXCopier`, `pimShortcuts`, `pimRegEdit`(wrapper), `pimDownloader`; transient
`pimScriptLoop`/`pimPsfLoop`; `pimXmlFile` (heavily, for reading/writing every
property on its product XML); `pimConvert`/`SetupConverter`; `pimSessionInfo` (back-
references, e.g. for license source lookups); Xerces DOM APIs directly in several
places (per the earlier `UpdateUrls` research in this session).

## Lifetime

Created by `pimSessionInfo::AddEntitlement` and held in `pimSessionInfo`'s
`EntitlementArray`/`InstalledArray` (`dsXArray<pimEntitlement*>`) for the life of the
session. As a `pimLoop`, its *thread* lifetime is separate and shorter: spawned by
`StartOrRestartInstall()`/`Uninstall()`/`Reconfigure()` (which presumably call the
inherited `Execute()`) and joined via the inherited `Wait()`.

## Ownership Model

Owned by `pimSessionInfo` (array of pointers — deletion responsibility not verified in
this pass, likely in `pimSessionInfo`'s destructor or `DropEntitlement`).
`pimEntitlement` in turn owns `IamInCopy` and (by pattern) the other 6 wrapper
objects, but does **not** own the `pimEntitlement*` pointers it stores as
prerequisites (`PreRequisitesToHandle`) — those point to sibling entries in the same
`pimSessionInfo` arrays.

## Thread Safety

Inherits `pimLoop`'s mutex-guarded lifecycle flags. Adds its own separate
`PctMutex` specifically for `total_pct_done`/`download_pct_done`, meaning progress
reporting and cancel/done-state are independently synchronized — a caller reading
`GetProgress()` and `IsDone()` in sequence is not guaranteed a consistent snapshot
across both (two separate lock acquisitions, no combined transaction).

## Extension Points

- New product-family detection: extend the family-name checks currently duplicated
  in `pim/pim_src/pimTop.cxx` (see `docs/modules/pim.md`) — this class exposes
  `GetFamily()` as the data source.
- New install-pipeline step: would require adding a new protected `On*`/`InstallX()`
  method and wiring it into wherever `OnInstall()`/`OnExecute()` sequences the existing
  steps (not line-traced in this pass — the exact call order inside the 7,649-line
  `.cxx` is a prerequisite for safely adding a step, and belongs in a deeper follow-up
  read of `pimEntitlement.cxx`).
- New CustomizeAttribs enum value: extend the nested `enum CustomizeAttribs` and
  `SupportCustomize`'s handling.

## Risk Analysis

- **7,649-line single file** implementing this class — the single highest-risk file
  in the codebase for any change; no sub-object decomposition of the install pipeline
  beyond the loop-wrapper delegation already described.
- **`friend class upimDlg; friend class upimSilentProgress;`** breaks encapsulation
  deliberately — those two UI classes can read/write private state directly, so a
  change to `pimEntitlement`'s private layout can silently break UI code that the
  public API contract would not have exposed as a dependency.
- Two independent mutexes (`pimLoop::Mutex` and `PctMutex`) guarding related state
  (done/cancel vs. progress) create a documented cross-mutex-consistency risk (see
  Thread Safety) — any refactor should consider whether these need to be combined or
  ordered consistently to avoid deadlock if ever locked together.
- `IamInMSI` is a `bool`, not an owning pointer to an MSI wrapper — suggests the MSI
  sub-step ownership pattern may differ from the CAB/service pattern in ways not
  visible from the header alone; worth verifying directly in `pimEntitlement.cxx`
  before assuming full symmetry with `IamInCopy`.
- Raw `char*` keys in `PreRequisiteMap`/`DownloadXmlMap` (`btkMap<char*, ...>`) —
  lifetime of the key strings relative to the map is not verifiable from the header;
  a dangling-key bug is structurally possible if a caller frees a string still
  referenced as a map key (**not confirmed as an actual bug** — flagged as a shape-of-
  the-code risk only).
- **CONFIRMED BUG, HIGH SEVERITY (`OnReconfigure()`, `pim_core_src/pimEntitlement.cxx:6880-6896`)**:
  the function's very first block computes the path to `RollbackMe` (the
  ORIGINAL, as-shipped `.p.xml` under `[LP]/bin/pim/xml/`) by taking a LOCAL
  copy of `xmlPtr`'s own CURRENT cache-file path (`pimGetAppData/pim/<name>.xml`,
  per the code's own comment `// this is the cache`) and, purely as a side
  effect of extracting its bare filename (`.GetTail()`), unconditionally
  **deletes that cache file from disk first**:
  ```cpp
  src = xmlPtr->getFilePath();// this is the cache
  if (src.IsFile())
      src.Erase();
  path /= src.GetTail();
  ```
  `Erase()` is confirmed, by its consistent usage everywhere else in this
  codebase (`pimFrictionlessTrialLicenseGet.cxx`, `pimPsfLoop.cxx`,
  `pimShortcutLoop.cxx`, `pimCustomActions.cxx`, etc. — always paired with
  a real on-disk temp/backup file being removed), to delete the file at the
  given path, not merely clear an in-memory string. There is no functional
  need to delete the file just to read its own filename — `GetTail()`
  works identically whether or not the file exists on disk. **CONFIRMED
  DOWNSTREAM CONSEQUENCE**: `xmlPtr`'s own internal file path (its
  `xmlfile` member, set via `SetXml()`/read via `getFilePath()`) is
  UNCHANGED by this — `src` is a separate local copy — so the deleted
  cache file is only restored once `xmlPtr->DoSave()` is called (traced
  into `pimXmlFile::DoSave()` → `DoWrite(xmlfile)`, confirming it writes to
  exactly this same path). Two confirmed early-return paths exist between
  this `Erase()` and the function's first subsequent `DoSave()` call, both
  leaving the cache file **permanently deleted with nothing to replace
  it**:
  1. `:6921-6925` — `PerformServiceAction(pimServices::StopAction)`
     returning `false` (service-stop failure) — no `DoSave()` anywhere
     before this `return;`.
  2. `:7023-7024` — `OnInstallCustomActions("PreReconfigure")` returning
     `false` — likewise no `DoSave()` before this `return;`, and this path
     requires no cancellation at all, only an ordinary custom-action
     failure, making it the more easily reachable of the 2.
  **Reachability**: `OnReconfigure()` is only invoked (per
  `docs/04_installation_flow.md` §4) for an entitlement already installed
  and cached, so `src.IsFile()` is confirmed true in the realistic common
  case — this `Erase()` almost always actually executes. **Contrast,
  confirmed by direct comparison**: `OnRollback()`'s own mirror-image
  "relocate the cache file" block (`:7317-7325`, moving the reference back
  from `.p.xml` to Application Data) computes an equivalent new path via
  `SetXml()` with **no preceding `Erase()`** — the 2 sibling relocation
  blocks are asymmetric, reinforcing that `OnReconfigure()`'s `Erase()` call
  looks like an erroneous side effect rather than an intentional design
  element. A fix would drop the `Erase()` call entirely — `GetTail()`'s
  correctness does not depend on it — or, if deleting the old cache
  location really is intended as part of the relocation, defer it until
  AFTER the new location has been confirmed written.
- **Confirmed, related design note**: this same `RollbackMe`-path
  computation is duplicated near-verbatim a 2nd time later in the function
  (`:7067-7082`, gated by `NoLPXmlLog`), where it is followed by
  `xmlPtr->SetXml(src)` — this 2nd occurrence's own `Erase()` is a no-op in
  practice by the time it runs (the file was already removed by the 1st
  occurrence, so `src.IsFile()` is false here), but the duplication itself
  (2 near-identical 6-line path-computation blocks in the same function) is
  a maintainability smell independent of the bug above.

## Usage Example (as evidenced by call sites)

```cxx
// pattern seen in pim/pim_src/pimTop.cxx (InstallPreReqSilent)
pimEntitlement *ptr_pr_E = NULL;
int pr_idx = 0;
while (pimGetSessionInfo()->GetEntitlement(i)->GetNextPrerequisite(&ptr_pr_E, pr_idx, true))
{
    if (!ptr_pr_E->IsPrerequisiteSatisfied())
    {
        ptr_pr_E->SetInstallPoint(Installwhere);
        ptr_pr_E->StartOrRestartInstall();   // spawns the entitlement's own thread
        while (!ptr_pr_E->IsDone())
            Sleep(100);
        if (!ptr_pr_E->IsPrerequisiteSatisfied(true))
            LG_ERROR(LOG_SERVICE, "Prerequisite installation failed.");
    }
    pr_idx++;
}
```

---
*Phase 5 of the requested 20-phase documentation set.*
