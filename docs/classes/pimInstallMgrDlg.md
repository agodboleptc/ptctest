# `pimInstallMgrDlg`

`pim_ui/includes/pimInstallMgrDlg.h` (847 lines, full read) / `pim_ui/pim_ui_src/pimInstallMgrDlg.cxx` (3556 lines) / `pim_ui/pim_ui_src/pimInstallMgrActions.cxx` (2019 lines) / `pim_ui/pim_ui_src/pimEntitlementRefresh.cxx` (1514 lines, full read)

> **Correction, resolved (found while documenting `pimEntitlementTree`,
> resolved in a later pass)**: this doc originally listed only 2 `.cxx`
> files for `pimInstallMgrDlg`'s method bodies. A 3rd exists —
> `pim_ui_src/pimEntitlementRefresh.cxx` (1,514 lines) — discovered via
> `grep` while tracing `pimEntitlementTree`'s callers, and at that time
> explicitly flagged as **not traced in depth**. **That file has since been
> read in full** (a complete pass, all 1,514 lines) and is now folded into
> this doc below, at the same evidentiary standard as the rest of this file
> — see the `EntitlementRefresh()` family entries under Public/Protected
> APIs, the `uiCheckButtonCell` entry under Dependencies, and 4 new Risk
> Analysis findings (a confirmed hang-on-failure bug in the School/Beta
> trial-license wait loops, a confirmed missing-`else` bug in
> `IsSufficientSpace()`, confirmed fully-dead `uiCheckButtonCell` code, and a
> confirmed self-contradictory use of `PIM_HIDE_CUSTOMIZE_SCREEN`). See
> `docs/classes/pimEntitlementTree.md` for the original discovery note.

## Purpose

`pimInstallMgrDlg` is the single main-window wizard dialog that drives the
entire interactive install experience: Welcome → EULA (or Beta EULA) →
License Source → Applications → Application Update/Selection → Customize
Apps → Installation (or Install Update) → Summary. It is by far the largest
class documented in this effort (its `.cxx` files alone total 7,089 lines
across 3 files, exceeding even `pimEntitlement.cxx`'s 7,649 lines when
combined with its header) and is the class this documentation set's own
`README.md` previously flagged as "the largest unread UI logic."

**Scope note**: given the scale, this doc documents `pimInstallMgrDlg`
itself — its architecture, state machine, and the areas read in full or
close depth in this pass — at the same evidentiary standard as every other
class doc in this set (every claim traced to a cited file/line), but does
**not** claim exhaustive line-by-line coverage of every event-handler branch
the way smaller classes received. Specific areas read in full: the
constructor's component-tree assembly, `Initialize()`/`Display()`/`OnClose()`,
the license-server port-validation cluster (all 13 methods), the customize-tab
refresh cluster (`RefreshShortcutsTab`/`RefreshFeatureTab`/
`RefreshCommandCfgTab`/`RefreshLicensesTab`/`RefreshMiscTab`/`RefreshDlg`/
`OnTabSelect`), `UpdateParentEntitlements`/`UpdateChildEntitlements`, the
full wizard step state machine (`StepBack`/`StepForward`/
`pimTransitionToInstallStep`/`pimTransitionToFinishStep`), and — as of this
pass — the complete `pim_ui_src/pimEntitlementRefresh.cxx` (all 1,514 lines):
the `EntitlementRefresh()`/`EntitlementRefresh_low()` pair, the
`EntitlementRefresh_update{Customize,Space,Nextbtn,Qagent}()` cluster,
`IsSufficientSpace()`, `EntitlementDownloadPreAction()`,
`ApplicationsNextPreAction()`, `CustomizeNextPreAction()`, the 2 free-function
tree callbacks (`QualityAgentChangedInTree()`/`SelectionChangedInTree()`), and
the nested `uiCheckButtonCell` class's full implementation, and — as of a
further, dedicated pass specifically on `OnPushButtonActivate()`
(`pim_ui_src/pimInstallMgrActions.cxx:643-1193`, 551 lines) — the **complete**
button-dispatch body, branch by branch, including its 2 dispatch chains
(an `if`/`else if` sequence for the wizard/license buttons, `:647-1062`,
followed by a 2nd, independently-headed `if` chain for the Command Config
table and CPU-ID/port buttons, `:1063-1179`) and its common tail
(`:1181-1192`). See Responsibilities and Risk Analysis for the confirmed
findings this surfaced. **Still not** individually traced to the same depth:
the EULA/license-source/application-selection per-screen business logic
bodies this dispatch delegates to (`EulaNextPreAction()`,
`LicenseIDNextBackPreAction()`, etc., in `pimInstallMgrActions.cxx`) beyond
what `OnPushButtonActivate()`'s own call sites and return-value handling
already reveal about them.

## Responsibilities

- Own and lay out **every** UI component in the wizard (the constructor
  alone is ~350 lines of `AddComponent()` calls) across the Welcome, EULA,
  Beta EULA, License Source (with a nested license-server-port
  configuration sub-UI), Applications, Application Update, Customize Apps
  (itself a multi-tab sub-dialog), Installation, Install Update, and Summary
  screens.
- Drive the wizard's step navigation as a **hardcoded, hand-written linear
  state machine** over a `uiLayout *CurrentLayout` pointer
  (`StepBack()`/`StepForward()`, `pim_ui_src/pimInstallMgrActions.cxx:106-389`)
  — not a table-driven or data-structure-based state machine; each step's
  forward and backward transitions are independently written `if`/`else if`
  chains (see Risk Analysis for a confirmed asymmetry between them).
- Validate and manage the FlexNet license server's 3 configurable ports
  (Primary/vendor-daemon port, Secure/"Eport", and the `lmgrd` port) — range/
  format/collision/busy-process checks, auto-expanding a "Vendor Daemon
  Ports" section on any port problem, and surfacing warnings vs. errors
  distinctly (the port-validation cluster, `pimInstallMgrDlg.cxx:983-1253`).
  This is the most recently and heavily developed area of the file, per its
  own revision history (12+ entries in 2026 alone, up through `$$93`).
- Drive the embedded "Customize Apps" multi-tab sub-dialog (Shortcuts,
  Application Features, Command Configuration, Simulate Licenses, Misc user
  inputs, Help/Advanced), refreshing each tab's contents on selection
  (`OnTabSelect()`, `pimInstallMgrDlg.cxx:2211-2246`).
- Propagate a user's language/platform selections from a "parent" entitlement
  down to (or up from) dependent "child" entitlements sharing the same
  `RequiredParentEntitlement` tag, and specifically WGM base-platform
  selections to WGM adapter entitlements
  (`UpdateParentEntitlements()`/`UpdateChildEntitlements()`,
  `pimInstallMgrDlg.cxx:1374-1809` — see Risk Analysis for a confirmed
  large-scale duplication of this exact logic elsewhere in the same file).
- Act as the de facto process-wide UI singleton: a single stack-local
  instance is constructed once by `pim/pim_src/pimTop.cxx:724`
  (`pimInstallMgrDlg Dlg;`), and a raw file-static pointer
  (`static pimInstallMgrDlg *MainUIDialog`, set to `this` in the constructor,
  `pimInstallMgrDlg.cxx:114, 276`) is exposed via the free function
  `GetMainUIDialog()` and used by at least 6 other `pim_ui` files
  (`pimTextDlg.cxx`, `pimPTCLicenseGet.cxx`, `pimLicenseIDRefresh.cxx`,
  `pimEntitlementRefresh.cxx`, `pimEntitlementTree.cxx`,
  `pimProgressRefresh.cxx`) to reach back into the main dialog from
  elsewhere in `pim_ui` — a global-pointer-based singleton, structurally
  unrelated to the `GetInstance()` singleton family documented for the 7
  `pim_core` owner-wrapper classes.
- Dispatch **every** wizard/toolbar button press through one 551-line
  method, `OnPushButtonActivate(uiPushButton&)`
  (`pimInstallMgrActions.cxx:643-1193`, fully traced in a further, dedicated
  pass) — 2 sequential dispatch chains (an `if`/`else if` sequence for the
  wizard-navigation and license buttons, `:647-1062`, then a 2nd,
  independently-headed `if` chain for the Command Config table, CPU-ID
  list, and port buttons, `:1063-1179`) followed by a common tail
  (`:1181-1192`) that conditionally calls `Refresh()`. The `NextStepBtn`
  branch alone (`:782-1041`) is the wizard's actual forward-navigation
  logic: license-server port validation, Beta EULA / EULA decline
  handling and trial/school/beta license acquisition (reaching
  `pimSessionInfo::TryAuthorize()`, see Risk Analysis and
  `docs/classes/pimSessionInfo.md`), the Mathcad MSI-hybrid update
  confirmation, and the various `*NextPreAction()` guards, before finally
  calling `StepForward()`. See Risk Analysis for multiple confirmed bugs
  found while tracing this dispatch in full.

## Dependencies

- `uiDialog` (base class; the underlying UI toolkit's window/dialog
  primitive — external, not part of this codebase).
- `pimSessionInfo` (the whole-session entitlement/property state this dialog
  reads and mutates throughout).
- `pimShortcutMgr` (`docs/classes/pimShortcutMgr.md`) — confirmed used by
  `RefreshShortcutsTab()` (`pimInstallMgrDlg.cxx:1839`).
- `pimPackageMgr`/`pimLanguageMgr`/`pimPlatformMgr` — used throughout the
  Customize/Applications logic (e.g. `UpdateParentEntitlements()`).
- `pimCommandMgr` (`RefreshCommandCfgTab()`, `RefreshLicensesTab()`'s
  `InitSimulateLicenses()`).
- `pimHelpMgr` (`RefreshHelpTab()`, `RefreshDlg()`'s HelpAdvanced branch —
  see Risk Analysis for this tab's confirmed disabled status).
- `pimGetAvailable` (`AvailableDownloads` member; `WaitForAvailableDownloadData()`).
- `pimWaitDlg` (modal wait dialog shown while `AvailableDownloads` searches).
- `pimEntitlementTree` (the `ApplicationsTree` member — the main
  application-selection tree control; a separate, not-traced-in-this-pass
  class).
- `pimCleanCache` (`OnClose()`/`OnFinishFromEula()`/`OnFinishFromBetaEula()`
  — synchronously runs and blocks on a cache-cleanup Loop before exiting,
  via a `Sleep(1000)`-polling `do`/`while (!Cleaner.IsDone())` loop, unless
  `PIM_KEEP_CACHE` is set).
- `pimisPortAvailableForFlexNet` (external port-availability/process-name
  check used by the port-validation cluster).
- **`pimSessionInfo::TryAuthorize()`/`GetTrialLicense()`/`GetSchoolLicense()`/`GetBetaLicense()`**
  (`pim_core/pim_core_src/pimSessionInfo.cxx:1911-1984,1986-2214`, new
  citations, traced in a dedicated pass on `OnPushButtonActivate()`'s
  `NextStepBtn` branch): reached via `EulaNextPreAction()`/
  `BetaEulaNextPreAction()` (`pimInstallMgrActions.cxx:902,912`). See
  `docs/classes/pimSessionInfo.md`'s Risk Analysis for a confirmed
  infinite-loop bug in `TryAuthorize()` that, combined with a confirmed
  ordering bug in this class's own EULA-decline handling (see Risk
  Analysis below), can freeze the wizard or trap a user trying to decline.
- **`pimTextDlg`** (`pim_ui/includes/pimTextDlg.h` + `pim_ui_src/pimTextDlg.cxx`,
  cited only via specific traced methods, per the top-level README's
  Coverage Honesty Statement — no dedicated class doc exists): the
  license-server port-validation warning dialog (`:849`) is a `pimTextDlg`
  instance whose own `OnPushButtonActivate()` (`pimTextDlg.cxx:82-98`), on
  a "proceed anyway" confirmation, sets `skip_ports_modification="Y"` and
  then **re-enters `GetMainUIDialog()->OnPushButtonActivate(NextStepBtn)`**
  — a confirmed re-entrant call back into this same method while the
  outer call is still on the stack inside `Display()`. See Risk Analysis.
- **`pimFlexDlg`/`pimCmdCfgDlg`** (`pim_ui/includes/pimFlexDlg.h`/
  `pimCmdCfgDlg.h`, cited only via specific traced call sites, per the
  Coverage Honesty Statement): `AdvancedHostidSetupBtn`
  (`pimInstallMgrActions.cxx:743-759`) and `mCmdAdd`/`mCmdEdt`/`mCmdDel`
  (`:1063-1123`) respectively — see Risk Analysis for a confirmed
  set-order asymmetry and an asymmetric-undo finding in these 2 branches.
- Nested classes declared in the **same header** and largely implemented in
  the **same** `.cxx` files (treated as tightly-coupled dependencies, not
  individually documented to full class-doc depth in this pass):
  `uiLSTInputPanel`, `uiLicenseSummaryTable`, `EntitlementItem`,
  `uiInstallationTable`, `uiAppStatus`,
  `uiCheckButtonCell` (fully implemented in `pimEntitlementRefresh.cxx:129-273`
  — **confirmed entirely dead code**, see Risk Analysis), `uiCustomAppTree`
  (which independently re-implements `UpdateParentEntitlements()`/
  `UpdateChildEntitlements()` — see Risk Analysis), `uiCustomApplicationList`,
  `pimCustomSimulateLicenseTable`, `pimCustomMiscTable`,
  `pimCustomCommandTable`.

## Members

Given ~150 declared members (nearly every line of the class body is a UI
component), only the structurally significant ones are listed; see the
header (`pimInstallMgrDlg.h:439-676`) for the full component inventory.

| Member | Type | Purpose |
|---|---|---|
| `CurrentLayout` | `uiLayout*` | The wizard's current-step pointer; the entire state machine is implemented as comparisons against this pointer's identity. |
| `SessionInfo` | `pimSessionInfo*` | The session state this dialog reads/writes throughout. |
| `currentProduct` | `pimEntitlement*` | The entitlement currently selected in the Customize/Applications screens. |
| `reconfigure_mode` | `bool` | Selects the reconfigure-specific branch in most of the flows described above. |
| `complete_quiet_exit` | `bool` | Gates whether `OnClose()`/`OnFinish*()` skip the "are you sure you want to exit" confirmation. |
| `Port`, `Eport`, `LmgrdPort` | `PortInfo` (nested struct: `portStr`, `isAvailable`) | The 3 license-server port values and their last-validated availability. |
| `portValidationResult` | `ValidationResult` (nested struct: status enum + message) | The single current port-validation outcome, overwritten by whichever port-check ran most recently (see Risk Analysis). |
| `AvailableDownloads` | `pimGetAvailable*` | Background download-availability search object; waited on by `WaitForAvailableDownloadData()`. |
| `waitDlgPtr` | `pimWaitDlg*` | Modal wait dialog shown during that wait. |
| `LicenseSummaryTable`, `InstallationTable`, `mCommandTable`, `mSimulateLicenseTable`, `mMiscTable`, `mCustomizableProducts`, `mAppFeaturesTree` | (nested class instances) | The concrete tab/table controls for License Summary, Installation progress, Command Config, Simulate Licenses, Misc inputs, the customizable-product list, and the App Features tree, respectively. |

## Public/Protected APIs

Selected structurally significant methods (see the header for the complete
list of ~90 declared methods):

- `pimInstallMgrDlg()` / `~pimInstallMgrDlg()` — constructor performs the
  entire UI component-tree assembly (~350 lines) and sets the global
  `MainUIDialog` pointer; destructor calls `LocateAllowPTCDotCom(false, NULL)`
  and deletes `AvailableDownloads` if set.
- `bool Initialize()` — one-time setup (guarded by the `initialized` flag):
  sets every wizard step's initial nav-image/style, product-mode-specific
  label text (WGM/Mathcad/reconfigure/web-media variants), dark-mode style
  registration (`checkDarkModeCB`), and app-features search-widget init.
- `bool Display()` — calls the base `uiDialog::Activate()` (presumed
  blocking/modal, consistent with `pimTop.cxx`'s single stack-local
  construct-Initialize-then-presumably-Display call pattern).
- `void Refresh()` — the idle-callback target (`DoMainDialogRefresh()`,
  `pimInstallMgrDlg.cxx:118-125`) registered so background state changes
  (e.g. a download search completing) can trigger a UI refresh from outside
  the dialog's own event handlers.
- `bool OnClose()` / `bool OnFinishFromEula()` / `bool OnFinishFromBetaEula()`
  — **3 independently-written, near-identical exit paths**
  (`pimInstallMgrDlg.cxx:735-911`), each: confirms exit intent (unless
  `complete_quiet_exit`), calls `OKToExitProgress()` if an install is
  in-progress, then runs `pimCleanCache` synchronously before
  `uiDialog::Exit(0)`/`Destroy()`. See Risk Analysis for the confirmed
  triplication.
- `void WaitForAvailableDownloadData(const char *alternate_message = NULL)`
  — if `AvailableDownloads` hasn't finished its search, shows a modal
  `pimWaitDlg` wrapping it as a `pimLoop*` until it completes.
- Applications-screen refresh cluster (`pim_ui_src/pimEntitlementRefresh.cxx`,
  full file, 1,514 lines — the class's 3rd `.cxx` file, see correction note
  above):
  - `void EntitlementRefresh()` (`:304-750`) — the Applications screen's
    top-level per-mode entry point (`new`/`reconfigure`, dispatched off the
    `INSTALLMODE_PROPERTY` session property). In `new` mode, first resolves
    any pending Trial/School/Beta license generation (each of the 3 modes
    polls its own `SessionInfo->Get{Trial,School,Beta}LicenseLoop()`; see
    Risk Analysis for a confirmed hang bug in 2 of these 3 near-identical
    polling blocks), then, for web-media (`MEDIA_PROPERTY == "N"`) installs,
    lazily constructs/drives `AvailableDownloads` via
    `EntitlementDownloadPreAction()`; in `reconfigure` mode, re-runs every
    installed entitlement's PSF command definitions
    (`pimCommandMgr::UpdateCommand()`) and calls `EntitlementRefresh_low()`.
  - `void EntitlementRefresh_low()` (`:754-1012`) — rebuilds `ApplicationsTree`
    for whichever of 3 sub-modes (`new`/`reconfigure`/`new_download`) is
    active: clears and repopulates the tree's entitlement nodes, sets its
    column-visibility flags (version/size/status/quality-agent), registers
    its 3 tree-callback function pointers
    (`QualityAgentChangedInTree`/`SelectionChangedInTree`, the latter for
    both `SelectionChanged` and `VersionChanged`), then calls all 4
    `EntitlementRefresh_update*()` methods below.
  - `void EntitlementRefresh_updateCustomize()` (`:1014-1054`) — enables the
    `CustomizeBtn` only if some install/reconfigure-selected entitlement
    supports customization **and** `getenv("PIM_HIDE_CUSTOMIZE_SCREEN")` is
    set — see Risk Analysis for why this condition is confirmed
    self-contradictory against the env var's own name and its use elsewhere
    in this same file.
  - `void EntitlementRefresh_updateSpace()` (`:1056-1201`) — recomputes and
    displays the "space needed / space available" HTML label; also contains
    WGM-specific pre-flight checks (blocks Next with an
    `pimLaunchIfNecessaryInsufficientSpaceBalloon()` balloon for a pending
    downgrade, a lower WGM version already present, or a running UWGM
    client). The correctly-guarded `if (...same-version...) dbl = 0.0; else
    GetSize(dbl);` pattern here is the confirmed-correct sibling used to spot
    the `IsSufficientSpace()` bug below.
  - `void EntitlementRefresh_updateNextbtn()` (`:1203-1322`) — enables
    `NextStepBtn` if any update-mode entitlement needs installing, has an
    unsatisfied prerequisite, or (Mathcad-specific) needs a platform
    migration; unconditionally enabled in `reconfigure` mode, per an explicit
    `/* allow next button always in reconfigure */` comment covering a large
    commented-out per-entitlement reconfigure loop (`:1301-1321`, confirmed
    dead, superseded by the unconditional `SetSensitive(true)` at `:1213`).
  - `void EntitlementRefresh_updateQagent()` (`:1324-1422`) — computes and
    displays the tri-state "enable Quality Agent for all" checkbox
    (checked/unchecked/mixed) across all install-selected or
    reconfigure-selected entitlements; forced checked-and-insensitive in
    Trial/School/Beta/Cloud modes.
  - `bool IsSufficientSpace()` (`:1424-1479`) — the `ApplicationsNextPreAction`
    path's disk-space gate; **confirmed to contain a missing-`else` bug**,
    see Risk Analysis.
  - `bool EntitlementDownloadPreAction()` (`:279-302`) — lazily constructs
    `AvailableDownloads` and loops `SearchForAvailableDownloads()` until it
    returns non-zero (aborting on `-3`), then calls
    `LocateAllowPTCDotCom(true, AvailableDownloads)`; this is the confirmed
    caller-side connection to `pimGetAvailable::GetSecurity(bool retry)`
    (already documented in `docs/classes/pimAuthDlg.md` as a `pimAuthDlg`
    construction site).
  - `bool ApplicationsNextPreAction()` (`:1481-1505`) — WGM-mode-only guard:
    blocks Next with a "customize_pkg" balloon if any non-update entitlement
    has a 0-byte computed size.
  - `bool CustomizeNextPreAction()` (`:1508-1514`) — resets `currentProduct`,
    re-points `mAppFeaturesTree`'s session, and calls
    `CustomEntitlementRefresh()` (defined in `pimInstallMgrDlg.cxx:2287`, not
    in this file); always returns `false` (never blocks the Next transition).
  - 2 free-function tree callbacks, registered via `ApplicationsTree.SetCallback()`
    (`:824-826, 890-892, etc.`) rather than being class members:
    `QualityAgentChangedInTree()` (`:114-117`, forwards to
    `EntitlementRefresh_updateQagent()`) and `SelectionChangedInTree()`
    (`:119-124`, forwards to `EntitlementRefresh_updateCustomize()` and
    `EntitlementRefresh_updateNextbtn()` — a commented-out
    `EntitlementRefresh_updateSpace()` call here has a `//called it at the
    end` note; consistent with `EntitlementRefresh_low()` and
    `EntitlementRefresh()` both calling `EntitlementRefresh_updateSpace()`
    explicitly after tree rebuilds, so this is a confirmed intentional
    relocation, not an accidental omission).
- Port-validation cluster (`pimInstallMgrDlg.cxx:983-1253`): `SetPortValue()`,
  `ResetPort()` (restores session defaults via
  `pimGetSessionInfo()->GetDefaultPortValues()`), `GetPortStatus()`,
  `SetPortStatusLow()` (the shared implementation: empty-or-"0" sentinel
  handling, format/range/collision-with-lmgrd/busy-process checks, in that
  order, short-circuiting on the first failure), `SetPortStatus()` (dispatches
  Primary/Secure to `SetPortStatusLow()`), `GetPortValue()`,
  `SetAndValidateLicenseServerPorts()` (the top-level entry point — see Risk
  Analysis for its confirmed disabled Secure-port check), `SetPortWarning()`/
  `SetPortError()` (templated message-formatting setters — see Risk Analysis
  for a confirmed message-ID copy-paste bug), `SetPortsValidationSuccess()`,
  `GetPortsValidationResult()`.
- `bool UpdateParentEntitlements()` / `bool UpdateChildEntitlements()`
  (`pimInstallMgrDlg.cxx:1374-1809`) — see Responsibilities and Risk Analysis.
- Customize-tab refresh cluster: `RefreshShortcutsTab()`, `RefreshHelpTab()`,
  `RefreshFeatureTab()` (confirmed permanent no-op — see Risk Analysis),
  `RefreshCommandCfgTab()`, `RefreshLicensesTab()`, `RefreshMiscTab()`,
  `RefreshDlg()` (builds the dynamic tab list from
  `currentProduct->SupportCustomize(...)` checks), `OnTabSelect()` (the
  dispatcher).
- `void StepBack()` / `void StepForward()`
  (`pim_ui_src/pimInstallMgrActions.cxx:106-389`) — the wizard's linear step
  state machine; see Responsibilities and Risk Analysis.
- `void pimTransitionToInstallStep()` / `void pimTransitionToFinishStep(bool some_failed = false)`
  (`pimInstallMgrActions.cxx:391-439`) — set the Installation/InstallUpdate
  step's nav image to current/success and toggle the
  `InstallDoneLabel`/`InstallDonePartialLabel` pair based on `some_failed`.
- `bool OnPushButtonActivate(uiPushButton&)` / `bool OnCheckButtonActivate(uiCheckButton&)` /
  `bool OnRadioGroupSelect(uiRadioGroup&, cStringT)` / `uiTimerOp OnTimerExpired(uiTimer&, void*)`
  (`pimInstallMgrActions.cxx:643-1610`) — the central event-dispatch methods;
  `OnPushButtonActivate()` alone spans 551 lines
  (`pimInstallMgrActions.cxx:643-1193`), **now fully traced branch-by-branch
  in a further, dedicated pass** (see Scope note above and Risk Analysis).
  2 dispatch chains: an `if`/`else if` sequence (`:647-1062`) covering
  `CustomizeBtn`, `SimpleLicenseGenerate`/`AdvancedHostidSetupBtn`,
  `AddLicSourceBtn`, `Backup_Info`, `DoNotHaveLic`, `EulaPrint`/`EulaEnglish`,
  `NextStepBtn` (the main wizard-forward logic — license-server port
  validation, Beta EULA / EULA screens, the Mathcad update-confirmation
  loop, and the `*NextPreAction()` guards), and `BackStepBtn`; then a 2nd,
  independently-headed `if` chain (`:1063-1179`, not `else if` — see Risk
  Analysis) covering the Command Config table buttons (`mCmdAdd`/`mCmdEdt`/
  `mCmdDel`), `MultipleHostIDButton`, the port-reset/info buttons, and a
  final `else` that opens each `ExplorAppButtons` entry's help text. A
  common tail (`:1181-1192`) conditionally calls `Refresh()`.

## Called By

- `pim/pim_src/pimTop.cxx` — confirmed via full-archive grep to be
  constructed as a stack-local `pimInstallMgrDlg Dlg;` at **3 separate
  points** in this file (lines 724, ~1090s, ~1190s), each followed by
  `Dlg.SetSessionInfoFile(&SessionInfo)` → `Dlg.Initialize()` →
  (session/license-source setup) → `Dlg.Display()` (confirmed at line 864
  for the first site; the other two call `Display()` immediately after
  `Initialize()` with no intervening setup, per lines ~1099-1105 and
  ~1195-1197) — i.e. 3 independent top-level entry points in `pimTop.cxx`
  each run their own full, separate instance of the wizard, not one shared
  instance reused across call paths. `Display()`'s `uiDialog::Activate()`
  is presumed blocking/modal, consistent with this run-to-completion
  pattern; not independently re-verified against the external UI toolkit's
  own source (not present in this archive).
- `DoMainDialogRefresh()` (`pimInstallMgrDlg.cxx:118-125`) — a free function
  registered as a UI idle callback; calls `MainUIDialog->Refresh()` if the
  global pointer is set, allowing background code to request a UI refresh
  without holding a reference to the dialog instance directly.
- `GetMainUIDialog()` — called from at least 6 other `pim_ui` source files
  (`pimTextDlg.cxx`, `pimPTCLicenseGet.cxx`, `pimLicenseIDRefresh.cxx`,
  `pimEntitlementRefresh.cxx`, `pimEntitlementTree.cxx`,
  `pimProgressRefresh.cxx`), confirmed via full-archive grep; individual call
  sites were not traced in this pass.

## Calls Into

- `pimSessionInfo`, `pimEntitlement` (extensively, throughout every screen's
  refresh/pre-action logic).
- `pimShortcutMgr`, `pimPackageMgr`, `pimLanguageMgr`, `pimPlatformMgr`,
  `pimCommandMgr`, `pimHelpMgr` (see Dependencies).
- `pimCleanCache`, `pimGetAvailable`, `pimWaitDlg`.
- `pimisPortAvailableForFlexNet` (external port/process check).
- The underlying UI toolkit (`uiDialog`, `uiLayout`, `uiTab`, `uiLabel`,
  `uiPushButton`, `uiCheckButton`, `uiRadioGroup`, `uiHTMLWindow`, etc. — all
  external to this codebase).

## Lifetime

Stack-local instance, constructed at one of 3 independent top-level entry
points in `pimTop.cxx` (see Called By), living for the duration of that
function's scope (i.e., for the whole interactive wizard session at whichever
of the 3 entry points is actually reached for a given run — these are
mutually exclusive alternative run modes, not confirmed to ever coexist in
one process in this pass). Not heap-allocated, not reference-counted, not a
`GetInstance()`-style singleton — the "singleton" behavior (global
reachability via `GetMainUIDialog()`) is achieved purely through a raw
file-static pointer set once in the constructor and never cleared, meaning
`GetMainUIDialog()` would return a dangling pointer if called after the
owning stack frame returns — not confirmed to actually happen in any traced
call path, but structurally possible given no destructor-side reset of
`MainUIDialog` to `NULL` was observed (`~pimInstallMgrDlg()`,
`pimInstallMgrDlg.cxx:459-464`, does not clear it). This also means that if
any 2 of the 3 `pimTop.cxx` entry points were ever reached sequentially in
the same process, the second construction would silently repoint
`MainUIDialog` at the newer instance — not confirmed to happen, but worth
noting as a structural possibility given 3 independent construction sites
exist in one file.

## Ownership Model

Owned by the stack frame of whichever `pimTop.cxx` function constructs it —
not owned by `pimSessionInfo` or any other class. It in turn holds
non-owning pointers into `pimSessionInfo`'s entitlement arrays
(`currentProduct`, and indirectly through `SessionInfo`), and owns
`AvailableDownloads`/`waitDlgPtr` outright (both explicitly `delete`d: the
former in the destructor, the latter immediately after each
`WaitForAvailableDownloadData()` call completes).

## Thread Safety

- This is a UI-thread class; no member mutex or lock was observed, and none
  would be expected for a dialog whose methods are all UI-event-handler
  callbacks presumed to run on a single UI/main thread.
- `DoMainDialogRefresh()`'s idle-callback pattern is the one confirmed
  mechanism by which background/worker-thread state (e.g. `AvailableDownloads`
  completing its search on its own thread, per `pimGetAvailable`'s `pimLoop`
  ancestry) can prompt a UI-thread refresh — consistent with the general
  "background Loop threads signal, UI thread polls/refreshes" pattern used
  elsewhere in this codebase (e.g. `pimEntitlement`'s own progress-polling
  loops documented in `docs/classes/pimEntitlement.md`).

## Extension Points

- Any new wizard step must be added to **both** `StepForward()` and
  `StepBack()` as new, manually-mirrored `if`/`else if` branches — there is
  no shared step-order data structure to update once. Given the confirmed
  asymmetry already present between the two (see Risk Analysis), extra care
  is warranted: write the new branch pair, then diff it against a
  semantically-equivalent existing pair to check the guard conditions match.
- A new customize-tab should follow the established
  `RefreshXTab()`-plus-`OnTabSelect()`-dispatch-branch-plus-
  `RefreshDlg()`-conditional-`AddLayout()` pattern — omitting any one of the
  three (as `RefreshFeatureTab()`/`mHelpAdvanced` appear to have, see Risk
  Analysis) leaves a tab either invisible or refreshing as a no-op.
- A new license-server port type would extend the `LICENSE_SERVER_PORT` enum
  and the `PortInfo`/`SetPortValue`/`GetPortStatus`/`SetPortStatusLow`-family
  dispatch chains — note `SetAndValidateLicenseServerPorts()` currently only
  actively validates the Primary port (see Risk Analysis); a genuinely new
  port would need to decide whether to follow that precedent or restore full
  multi-port validation.

## Risk Analysis

- **CONFIRMED: `StepForward()`/`StepBack()` have a real guard-condition
  asymmetry around the Customize Apps step.** `StepForward()`'s
  `ApplicationUpdateLayout → CustomizeAppsLayout` transition
  (`pimInstallMgrActions.cxx:324`) requires
  `! getenv("PIM_HIDE_CUSTOMIZE_SCREEN") && currentProduct` — but
  `StepBack()`'s corresponding `InstallationLayout → CustomizeAppsLayout`
  transition (`pimInstallMgrActions.cxx:153`) checks only
  `! getenv("PIM_HIDE_CUSTOMIZE_SCREEN")`, with **no `currentProduct`
  check**. If `currentProduct` were `NULL` when stepping forward from
  Application Update (causing `StepForward()` to skip Customize Apps and go
  straight to Installation), a user who then clicks Back from Installation
  would be taken to Customize Apps anyway — a step they never actually
  visited — because `StepBack()` cannot see the `currentProduct` state that
  `StepForward()` used to decide to skip it. This was found by directly
  comparing the two functions' corresponding branches side by side, the
  same technique that surfaced the `pimRegEditLoop`/`pimMSILoop` bugs
  earlier in this documentation effort.
- **CONFIRMED: `UpdateParentEntitlements()`/`UpdateChildEntitlements()` are
  duplicated near-verbatim** between `pimInstallMgrDlg`
  (`pimInstallMgrDlg.cxx:1374-1529, 1693-1750`) and the nested
  `uiCustomAppTree` class in the **same file**
  (`pimInstallMgrDlg.cxx:1532-1687, 1752-1809`) — line-by-line comparison
  shows the two `UpdateParentEntitlements()` bodies are identical apart from
  a single stray `//pradumn` comment in `uiCustomAppTree`'s copy. A future
  fix to the parent/child language-or-platform-propagation logic (or to the
  WGM `uwgm.xml` adapter-propagation special case) applied to one copy and
  not the other would silently diverge the two classes' behavior. This is
  the same class of finding as the exit-code-table duplication documented
  for `pimMSILoop` and the template-composition duplication between
  `pimScriptLoop`/`pimPsfLoop`.
- **CONFIRMED: `RefreshFeatureTab()` is a permanent no-op**
  (`pimInstallMgrDlg.cxx:1939-1942`: its entire body is `return false;`),
  yet `OnTabSelect()` calls it unconditionally every time the "Application
  Features" tab is selected (`pimInstallMgrDlg.cxx:2218-2221`). The tab's
  actual content refresh happens through a separate path
  (`mAppFeaturesTree.SetEntitlement(currentProduct, reconfigure_mode)`,
  called once from `RefreshDlg()` before the tab is even shown,
  `pimInstallMgrDlg.cxx:2201`) — so this is not necessarily a functional
  gap (the real refresh already happened), but `RefreshFeatureTab()` itself
  is confirmed dead code that should either be removed or documented as
  intentionally empty.
- **CONFIRMED: the "Help/Advanced" customize tab is disabled via two
  independent commented-out call sites**: its registration in `RefreshDlg()`
  (`//mCustomTabs.AddLayout(mHelpAdvanced.GetId(), NULL);`,
  `pimInstallMgrDlg.cxx:2171`) and its refresh dispatch in `OnTabSelect()`
  (the entire `else if (Selected == mHelpAdvanced.GetId())` branch is
  wrapped in a `/* ... */` block, `pimInstallMgrDlg.cxx:2238-2242`) are both
  commented out. `RefreshHelpTab()` itself remains fully implemented and
  callable (`pimInstallMgrDlg.cxx:1911-1937`) but, per this pass's tracing,
  is unreachable from the tab-selection path — consistent with a deliberate
  feature disablement rather than an oversight, though the "why" is not
  stated in any comment found in this pass.
- **CONFIRMED: `SetPortError()`'s `PORT_OUT_OF_RANGE_ERROR` and
  `PORT_INVALID_FORMAT_ERROR` cases both resolve to the identical message ID**,
  `pimUIPortsInvalidFormat` (`pimInstallMgrDlg.cxx:1228-1236`) — a port value
  that is syntactically valid but out of the legal 1024-65535 range (e.g.
  `70000`) would show the user an "invalid format" message rather than an
  "out of range" one. A small, confirmed copy-paste bug in otherwise
  carefully-templated error-message code.
- **CONFIRMED, but intentional**: `SetAndValidateLicenseServerPorts()`'s
  Secure/"Eport" validation call is commented out
  (`/*else if (!SetPortStatus(SECURE_PORT)) { return false; }*/`,
  `pimInstallMgrDlg.cxx:1182-1184`), so only the Primary port is actively
  validated by the top-level entry point today. This matches the dated
  revision-history entry `"$$90 Temporary EPORT Disablement"` (14-Jul-26) —
  a confirmed deliberate, temporary product decision, not a bug. Anyone
  re-enabling full port validation should search for this exact comment
  before assuming the feature was simply never finished.
- **CONFIRMED: `OnClose()`, `OnFinishFromEula()`, and `OnFinishFromBetaEula()`
  are three independently-written, near-identical implementations**
  (`pimInstallMgrDlg.cxx:735-911`) of "confirm exit → check
  `OKToExitProgress()` → run `pimCleanCache` synchronously → `Exit`/`Destroy`"
  — a third confirmed instance of the duplication pattern seen elsewhere in
  this file. `OnClose()` additionally has a large `#if 0`-disabled block
  (`:766-784`) for an EULA-decline file-display feature, confirmed dead.
- **CORRECTION (later pass, documenting `pimCustomDlg` itself)**: this entry
  originally speculated, based on method-name overlap and revision-history
  dates, that a separate standalone class `pimCustomDlg`
  (`pim_ui/includes/pimCustomDlg.h`, `pim_ui/pim_ui_src/pimCustomDlg.cxx`,
  2276 lines) was "likely superseded" legacy code. **A full read of both
  `.cxx` files confirms this was wrong: `pimCustomDlg` is live, currently-used
  code**, constructed and `Display()`ed from 2 confirmed call sites —
  `pimInstallMgrDlg::OnPushButtonActivate()`'s own `CustomizeBtn` branch
  (`pim_ui_src/pimInstallMgrActions.cxx:649`, inside the very
  `OnPushButtonActivate()` this doc's Scope note flagged as not traced in
  depth) and `pimEntitlementTree::OnPushButtonActivate()`'s per-row
  "customize" icon handler (`pim_ui_src/pimEntitlementTree.cxx:1338`). The
  method-name overlap is real and is now documented as **duplicated
  implementation of the same live feature**, not dead code left over from a
  superseded design — `pimInstallMgrDlg`'s embedded Customize tab and the
  standalone `pimCustomDlg` popup are two independent, concurrently-live
  code paths over the same underlying per-product settings, invoked from
  different UI entry points. See `docs/classes/pimCustomDlg.md` for the full
  writeup, including a confirmed wrong-static-handle bug in 3 of
  `pimCustomDlg`'s SAB (Smart Address Bar) methods that is coupled to
  `pimInstallMgrDlg` through 2 shared `static` globals in `pimSAB.cxx`.
- **CONFIRMED, HIGH SEVERITY (found while fully tracing
  `pimEntitlementRefresh.cxx`): the School- and Beta-mode trial-license wait
  loops in `EntitlementRefresh()` can spin forever if the license file never
  parses, with no timeout and no error dialog.** `EntitlementRefresh()`
  contains 3 structurally identical polling blocks — one per Trial/School/Beta
  mode — each waiting for
  `SessionInfo->GetXMLPtr()->GetNextNodelistItem(pimLICENSEPREFIX, true)` to
  become non-`NULL` after a license-generation `HeartBeat()` call. The
  **Trial**-mode block (`:376-401`) uses the correct guard,
  `} while (test == NULL && ++max <= 40);`, and is followed by an
  `if (test == NULL) { <error dialog>; return; }` block if the 40 retries are
  exhausted. The **School**-mode block (`:466-471`) and **Beta**-mode block
  (`:535-539`) are near-verbatim copies of this same loop, but each was
  changed to `} while (test == NULL || ++max >= 40);` — with **no** follow-up
  null-check/error-dialog block at all. Because `||` short-circuits on its
  left operand, `++max` is **never evaluated while `test` stays `NULL`**
  (C++ only evaluates the right side of `||` when the left side is `false`) —
  so if the School or Beta trial license never parses, `max` never advances
  past 0 and the loop **never terminates**: `EntitlementRefresh()` (and the
  whole wizard) hangs in a tight `Sleep(0.1s)`-then-poll cycle indefinitely,
  with no way out short of killing the process. This is the same
  cross-comparison technique (3 near-identical blocks, one correct, two
  independently mutated the same broken way) that surfaced the
  `rpimDlg`/`pimFrictionlessTrialDlg` shared-template bugs earlier in this
  effort; here the "template" and its 2 miscopies are all in one function of
  one file.
- **CONFIRMED: `IsSufficientSpace()` has a missing `else` that silently
  discards its own two special-case space calculations**
  (`pimEntitlementRefresh.cxx:1447-1456`). The code reads:
  ```cpp
  dbl = 0.0;
  if ((...IsMSISameVersionInstalled()... || ...IsSFXSameVersionInstalled()...) && !...IsMSIHybrid())
      dbl = 0.0;
  else if (SessionInfo->GetEntitlement(i)->IsParentOnly())
      dbl = 300 * 1024 * 1024; //add pim package size
  SessionInfo->GetEntitlement(i)->GetSize(dbl);   // <-- unconditional, no "else"
  total_size_selected += dbl;
  ```
  `GetSize(dbl)` runs unconditionally immediately after the `if`/`else if`,
  overwriting whatever `dbl` was just set to in either branch — so the
  "already-installed same version costs 0 bytes" and "parent-only entitlement
  costs a flat 300MB" special cases have **no effect** on the space total
  this function computes. This is confirmed by direct comparison against the
  structurally identical, correctly-guarded logic 400 lines earlier in the
  same file, `EntitlementRefresh_updateSpace()` (`:1085-1089`): `if (...) dbl
  = 0.0; else SessionInfo->GetEntitlement(i)->GetSize(dbl);` — same
  condition, same assignment, but with the required `else` present. Practical
  effect: `IsSufficientSpace()`'s disk-space gate (called from
  `pimInstallMgrActions.cxx:1034`) can be stricter than intended for
  already-installed-same-version or parent-only entitlements, since their
  cost is always recomputed from `GetSize()` rather than zeroed/flattened as
  the (dead) special-case code intends.
- **CONFIRMED: `uiCheckButtonCell`'s entire implementation is dead code** —
  compiled in but unreachable. Its constructor/destructor, static factory
  `AddDynamicComponent()`, `DelDynamicComponent()`, and both
  `SetAlignedLabel()` overloads are fully implemented in
  `pimEntitlementRefresh.cxx:129-273`, but an archive-wide grep for
  `uiCheckButtonCell` shows its **only** would-be caller —
  `pimInstallMgrDlg::CreateQAgentButton()` / `CreatedQAgentBtns` — is
  declared exclusively inside the nested `uiApplicationsList` class, which is
  itself wrapped in a class-wide `#if 0` (`pimInstallMgrDlg.h:298-330`, added
  the same revision as `uiCheckButtonCell` itself per the header's `$$18`
  history entry). No other construction site (`XNew uiCheckButtonCell(...)`)
  exists anywhere in the archive outside `uiCheckButtonCell`'s own
  constructor definition. As a further, independent belt-and-suspenders
  layer, **both** `SetAlignedLabel()` overloads are *also* individually
  hard-disabled even if some future caller were reintroduced: each is
  wrapped in its own `#if 0 // Eyal does not want labels` block and
  unconditionally `return false;` (`:250-266`) — one of the few `#if 0`
  blocks in this whole codebase with a named, in-comment rationale, compared
  to the many unexplained `#if 0` blocks catalogued elsewhere in this
  documentation set.
- **CONFIRMED: `PIM_HIDE_CUSTOMIZE_SCREEN` is checked with directly
  contradictory effect across branches of the same file.** The env var's
  name implies "when set, hide/skip the Customize step," and 2 of its 4
  distinct check sites in `pimEntitlementRefresh.cxx` are consistent with
  that: `EntitlementRefresh()`'s `new`-mode initial check (`:608-609`,
  `if (getenv(...)) CustomizeBtn.SetVisible(false);`) and its
  per-entitlement loop (`:665-666`, same effect, `#ifndef HIDE_CUSTOMIZE`-
  gated — see below). But `EntitlementRefresh()`'s `reconfigure`-mode branch
  does the **opposite**: `if (getenv(...)) CustomizeBtn.SetVisible(true);`
  (`:731-732`) — setting the button **visible** when the same env var is set.
  And `EntitlementRefresh_updateCustomize()` (`:1035-1036, 1048-1049`) only
  ever grants `CustomizeBtn.SetSensitive(true)` **when the env var is set**,
  leaving it permanently insensitive (per its unconditional
  `SetSensitive(false)` at the top, `:1019`) when the env var is **unset** —
  the opposite of what "hide the customize screen when set" would suggest for
  a button's clickability. Separately, both the `:665-666` and `:731-732`
  sites are wrapped in `#ifndef HIDE_CUSTOMIZE` — a macro whose only
  `#define` in this file is commented out (`//#define HIDE_CUSTOMIZE`,
  `:112`), so both `#ifndef` blocks are unconditionally compiled in; the
  macro itself is confirmed permanently dead. Taken together, this env var's
  behavior across `pimInstallMgrDlg`'s Applications-screen refresh logic is
  confirmed inconsistent with its own name in at least 2 of 4 sites — no
  single authoritative definition of what "PIM_HIDE_CUSTOMIZE_SCREEN" is
  supposed to do exists anywhere else in this archive to resolve which side
  is the bug.
- **CONFIRMED BUG, HIGH SEVERITY, found in a further dedicated pass fully
  tracing `OnPushButtonActivate()`: the EULA screen's decline-to-exit flow
  runs license acquisition *before* checking whether the user actually
  declined, the reverse of the Beta EULA screen's own (correct) order.**
  (`pimInstallMgrActions.cxx:858-960`.)
  ```cpp
  // Beta EULA screen (:858-908) -- checks decline FIRST
  if (Beta_Decline.IsChecked())
  {
      <exit-confirmation question>
      if (<No>) return true;
      <show decline file>
      return OnFinishFromBetaEula();
  }
  else
      BetaEulaNextPreAction();   // -> GetBetaLicense() -> TryAuthorize()

  // EULA screen (:909-960) -- calls the pre-action FIRST
  if (!EulaNextPreAction())      // -> GetTrial/SchoolLicense() -> TryAuthorize()
      return false;
  if (!ExportCkbx.IsChecked())   // <-- decline state, checked SECOND
  {
      <exit-confirmation question>
      ...
  }
  ```
  `EulaNextPreAction()` (reached unconditionally, before the decline check)
  leads to `GetTrialLicense()`/`GetSchoolLicense()`
  (`pim_core/pim_core_src/pimSessionInfo.cxx:1986-2160`) and then
  `pimSessionInfo::TryAuthorize()` (`:1911-1984`) whenever the product is
  in Trial or School mode — popping up the PTC.com login dialog and
  starting a `pimPTCLicenseGet` thread. **Confirmed consequence**: a user
  in Trial/School mode who checks "decline" and presses the wizard's
  Finish/Next button is first forced through a login prompt they may not
  even want to complete, before ever being asked whether they meant to
  exit. If the user cancels that login dialog, `EulaNextPreAction()`
  returns `false` and the handler returns immediately at `:916` — **the
  decline-to-exit path is never reached at all**, trapping the user on
  the EULA screen. If PTC.com cannot be reached, see the next finding
  below (`TryAuthorize()`'s own infinite-loop bug) for a 2nd way this same
  ordering can hang the wizard. The Beta EULA screen's own logic
  (`:858-908`), checked first for comparison, gets this exactly right —
  confirming the EULA screen's ordering is a defect in *this* screen
  specifically, not a codebase-wide design choice.
- **CONFIRMED BUG, found in the same pass: after a user confirms exit from
  either EULA screen, `OnPushButtonActivate()` keeps running and reaches
  `Refresh()` on a dialog whose `Destroy()` has already been called.**
  (`pimInstallMgrActions.cxx:899,957,1184`, depends on
  `pimInstallMgrDlg.cxx:813-911`'s `OnFinishFromEula()`/
  `OnFinishFromBetaEula()`.)
  ```cpp
  bool ret = OnFinishFromEula();   // ends: uiDialog::Exit(0); uiDialog::Destroy(); return UI_SUCCESS;
  // :957 -- return value discarded, no `return` here
  ...                               // falls through the rest of NextStepBtn,
                                     // then chain 2's comparisons (all false)
  Refresh(...);                     // :1184 -- called on the now-destroyed dialog
  ```
  `OnFinishFromEula()`/`OnFinishFromBetaEula()` both end by calling
  `uiDialog::Exit(0)` then `uiDialog::Destroy()` before returning
  `UI_SUCCESS` — but `OnPushButtonActivate()` discards that return value at
  both call sites (`:899`, `:957`) and has no `return` statement following
  either call, unlike the port-validation failure path 3 branches earlier
  (`:801-802`, `return OnClose();`), which does return immediately. Control
  falls through the remainder of the `NextStepBtn` branch, past both
  dispatch chains' comparisons (all false for `NextStepBtn`), to the common
  tail's `Refresh()` call at `:1184` — a call against a dialog object this
  same function just told to destroy itself. Whether this is a live crash
  depends on `uiDialog::Destroy()`'s own semantics, which live in the
  external UI toolkit outside this archive — flagged with the same
  confidence hedge already used for `pimAuthDlg`'s Risk #1 finding, not
  claimed as a confirmed crash.
- **CONFIRMED BUG, found in the same pass: declining to exit (answering
  "No" to the exit-confirmation question) leaves the Finish/Next button
  permanently disabled, contradicting the comment on that exact branch.**
  (`pimInstallMgrActions.cxx:784-785,878-879,936-937`.)
  ```cpp
  NextStepBtn.SetSensitive(false);   // :784 -- disabled at the very top of NextStepBtn
  BackStepBtn.SetSensitive(false);   // :785
  ...
  else if (<UI_MESSAGE_NO>)
  {
      // we are exiting            <-- comment; this branch does NOT exit
      BackStepBtn.SetSensitive(true);   // :871/:929 -- only Back is re-enabled
      return true;                       // skips the tail Refresh() at :1184,
  }                                       // which is what would normally
                                          // re-enable Next via EulaRefresh
  ```
  `NextStepBtn` is unconditionally disabled at the top of every
  `NextStepBtn` press (`:784`). The "No, don't exit" branches on both the
  Beta EULA (`:878-879`) and EULA (`:936-937`) screens only re-enable
  `BackStepBtn`, and `return true` immediately — skipping the common tail's
  `Refresh()` call (`:1184`), which is where `EulaRefresh()`'s own logic
  would otherwise re-derive and re-enable the Finish button's sensitivity.
  **Confirmed consequence**: a user who answers "No" to "are you sure you
  want to exit" is left with a disabled Finish/Next button and must
  toggle the decline radio button/checkbox again (triggering
  `OnCheckButtonActivate()`/`OnRadioGroupSelect()`, which do call
  `EulaRefresh()`) before they can proceed — the inline comment ("we are
  exiting") describes the *other* branch's intent, not this one's actual
  effect.
- **CONFIRMED BUG, found in the same pass: the license-server port
  validation warning dialog re-enters `OnPushButtonActivate(NextStepBtn)`
  while the outer call is still executing, and sets a "skip validation"
  session property that nothing ever resets.**
  (`pimInstallMgrActions.cxx:803-857`, depends on
  `pim_ui_src/pimTextDlg.cxx:82-98`.)
  ```cpp
  // :849 -- shown modally from inside NextStepBtn's own License Source branch
  WarningDlg.Display();
  // pimTextDlg.cxx:88,94 -- its own OnPushButtonActivate(), on "proceed anyway":
  SessionInfo->SetProperty("skip_ports_modification", "Y");
  ...
  GetMainUIDialog()->OnPushButtonActivate(NextStepBtn);   // re-enters THIS function
  // back in the outer call, :851:
  return false;   // still returns false after the nested call already ran StepForward()
  ```
  If the user ticks the warning dialog's "proceed anyway" checkbox,
  `pimTextDlg::OnPushButtonActivate()` sets the session property
  `skip_ports_modification="Y"` and then directly calls
  `GetMainUIDialog()->OnPushButtonActivate(NextStepBtn)` — a confirmed
  re-entrant call into this exact method, still executing, from inside its
  own `Display()` call. The nested call runs a **full** `NextStepBtn`
  pass, including `StepForward()`; the outer call then resumes and
  unconditionally `return`s `false` at `:851`, discarding whatever the
  nested call already did. **Confirmed persistence bug**: `skip_ports_modification`
  is only ever set to `"Y"` at `pimTextDlg.cxx:88,94`, is persisted via
  `SessionInfo->Save()` (`:798`), and is never reset to `"N"` anywhere in
  this archive except by never ticking that box in the first place — so
  once a user "proceeds anyway" past a ports warning once, port validation
  is silently bypassed on every subsequent visit to the License Source
  screen for the rest of the session, even if the ports are changed to a
  now-genuinely-invalid configuration afterward.
- **Further findings, found in the same full trace of `OnPushButtonActivate()`,
  lower severity or narrower reach than the 4 above**:
  - **Structural**: the 2nd dispatch chain (`:1063-1179`, Command Config/
    CPU-ID/port buttons) begins with a bare `if`, not `else if` — any
    chain-1 branch that falls through without returning (e.g. `EulaPrint`/
    `EulaEnglish`, or `CustomizeBtn` if its dialog's `Initialize()` fails)
    also gets compared against every chain-2 button and, if none match,
    runs the terminal `else` at `:1167` (the `ExplorAppButtons` help-text
    loop) — harmless today since none of those buttons' IDs collide, but
    reads like a missing `else` on the chain boundary.
  - **`last_license_btn` is set in opposite order for the 2 license
    buttons**: `SimpleLicenseGenerate` (`:657-662`) sets it *after*
    generating; `AdvancedHostidSetupBtn` (`:743-759`) sets it *before* even
    showing the `pimFlexDlg`, so it's set even if the user cancels that
    dialog or `Initialize()` fails. `LicenseIDRefresh()`
    (`pim_ui_src/pimLicenseIDRefresh.cxx:163-174`) automatically re-runs
    `DoAdvancedLicenseGenerate()` whenever the session's auth status is
    `"E"` and `last_license_btn == &AdvancedHostidSetupBtn` — a stale
    `"E"` status combined with a cancelled Advanced dialog is a plausible
    (not traced end-to-end) path to an unrequested license-generation
    attempt.
  - **Asymmetric undo in `mCmdEdt` (`:1073-1111`)**: `UndoInit()` only
    reverses `InitCommand()` (the new-install path, on cancel);
    `UpdateCommand()` (the update/reconfigure path) is called before the
    dialog is shown and is never undone if the dialog is then cancelled or
    fails to initialize. `mCmdAdd` (`:1063-1072`) refreshes regardless of
    `Display()`'s return value, while `mCmdEdt` only refreshes when it
    returns exactly `1`.
  - **`currentProduct` is dereferenced with no NULL check** at `:1066`,
    `:1080`, `:1116` (the 3 Command Config table buttons) —
    `CustomizeNextPreAction()` (`pim_ui_src/pimEntitlementRefresh.cxx:1510`)
    explicitly sets `currentProduct = NULL`, and `StepForward()` itself
    checks for NULL before using it (per this doc's existing coverage) —
    so these 3 branches are safe only as long as the UI never lets the
    Command Config tab be reached while `currentProduct` is NULL, a
    guarantee enforced only by UI flow, not by these branches themselves.
  - **`MultipleHostIDButton`'s loop bound is sourced independently of its
    array** (`:1124-1151`): it indexes `cpuids[i]` for
    `i < pimGetCPUidsCount()`, a separately cached counter
    (`pim_util/pim_util_src/pimFLEXnet.cxx:253-276`), not `cpuids.GetSize()`.
    `pimGetAllCPUid()`'s own return value is discarded (`:1131`); if it
    fails, `cpuids` can be empty while the cached count is non-zero,
    making the index run out of bounds.
  - **The busy cursor is set but never cleared** (`:774,779`, both
    `uiSetCursorBusy(true)`) — every other file in this codebase that calls
    this pairs it with a matching `false` (e.g. `pimGetNewPimDlg.cxx:104,111`).
  - **Reconfigure mode is read from 2 different sources in the same
    branch** (`:1021-1039`): `pimRunInReconfigureMode(str)` at `:1028,1030`
    vs. the `reconfigure_mode` member at `:1034`; `IsSufficientSpace()` is
    evaluated before the `!reconfigure_mode` check that would otherwise
    skip it, so its own space-calculation work (and any UI it shows) runs
    even in reconfigure mode.
  - **`ResetPort`/`EportReset` cover only 2 of the 3 configurable ports**
    (`:1152-1161`) — no reset button exists for the `lmgrd` port
    documented above in Responsibilities, and neither branch re-validates
    ports after resetting one.

## Usage Example

```cpp
// Traced from pim/pim_src/pimTop.cxx:724-864 (first of 3 construction sites)
pimInstallMgrDlg Dlg;
Dlg.SetSessionInfoFile(&SessionInfo);
if (Dlg.Initialize())
{
    // ... session/license-source/host-id setup ...
    Dlg.Refresh();
    Dlg.Display(); // confirmed at pimTop.cxx:864 -- blocking/modal wizard run
}
```
