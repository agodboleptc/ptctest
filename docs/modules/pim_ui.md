# Module: `pim_ui`

> Evidence-based; see `docs/01_repository_inventory.md` §0 for corpus scope.

## Purpose

The dialog/presentation layer. Every screen the user sees — product/language/platform
selection, license entry, EULA, progress, error/warning dialogs, browser-based
authentication, trial/renewal flows — is implemented here, on top of an external
custom UI toolkit (`uiDialog`, `uiTable`, `uiList`, `uiTree`, `uiInputPanel`,
`uiCheckButton`, `uiComponent`, `uiLayout`, `uiPushButton`, `uiLabel`,
`uiProgressBar`, `uiTimer` — all external, not MFC, not included in this archive).

## Responsibilities

- **Main install dialog**: `pimInstallMgrDlg` (`pim_ui/includes/pimInstallMgrDlg.h`) —
  by far the largest UI class, defining not just the dialog itself but roughly a dozen
  nested custom widget classes (`uiLSTInputPanel`, `uiLicenseSummaryTable`,
  `uiInstallationTable`, `uiCheckButtonCell`, `uiApplicationsList`, `uiCustomAppTree`,
  `uiCustomApplicationList`, `pimCustomSimulateLicenseTable`, `pimCustomMiscTable`,
  `pimCustomCommandTable`) used to render the license-source table, the
  application/product tree, and the per-product progress rows.
- **Alternate top-level dialogs**: `pimFrictionlessTrialDlg` (trial/frictionless
  registration flow, shown by `pimFrictionlessTrialRun`), `rpimDlg` (license renewal,
  shown by `pimRenewLicenseRun`), `upimDlg` (uninstall — inferred from name/pairing
  with `PIM_UNINSTALL_DLG` in `pim/pim_src/pimUIInit.cxx`), `pimsilentDlg` (silent-mode
  progress UI), `pimGetNewPimDlg`.
- **Supporting dialogs**: `pimAcrobatDlg`, `pimAuthDlg` (credential prompts, backs
  `pimGetSecurityUIPrompt` called from `pim/pim_src/pimUIInit.cxx`), `pimBrowserDlg`,
  `pimCmdCfgDlg`, `pimCustomDlg` (custom install package/language/platform picker,
  shares the `CurrentPkgMgr`/`CurrentLangMgr`/`CurrentPlatMgr` pattern with
  `pimInstallMgrDlg`), `pimFlexDlg`, `pimInsufficientSpace`, `pimPTCLicenseGet`,
  `pimPTCRenewLicenseGet`, `pimFrictionlessTrialLicenseGet`, `pimTextDlg`,
  `pimWaitDlg`.
- **UI-driving "refresh"/"action" logic split out from the dialog headers**:
  `pimEntitlementRefresh.cxx`, `pimEulaRefresh.cxx`, `pimLicenseIDRefresh.cxx`,
  `pimProgressRefresh.cxx`, `pimWelcomeRefresh.cxx`, `pimInstallMgrActions.cxx` — these
  `.cxx`-only files (no matching header — confirmed by their absence from
  `pim_ui/includes/`) implement page-specific UI update/action logic presumably as
  member functions of `pimInstallMgrDlg` defined out-of-line.
- **Shortcut management UI**: `pimShortcutMgr`.
- **File picking**: `pimFileOpen`.
- **Entitlement tree widget**: `pimEntitlementTree`.
- **`pimSAB`**: purpose **UNKNOWN** — not read this pass; name suggests "Search And
  Browse" or a status/area-bar widget, unconfirmed.

## Dependencies

- `pim_core` — `pimSessionInfo`, `pimEntitlement`, `pimPackageMgr`, `pimPlatformMgr`,
  `pimLanguageMgr`, `pimInterrogator`.
- `pim_util` — likely `pimFLEXnet`/`pimWindows` for license/platform display logic
  (**not individually verified per dialog file this pass**).
- External: the `ui*`-prefixed custom dialog toolkit (external, not included),
  `btk` toolkit.

## Consumers

`pim` module (`pimTop.cxx` constructs and drives `pimInstallMgrDlg`,
`pimFrictionlessTrialDlg`, `rpimDlg` directly). No other module in the archive
consumes `pim_ui`.

## Owned Components

Each dialog owns its own widget tree (rows/cells/tables built from the nested classes
in `pimInstallMgrDlg.h`) and, where applicable, transient `pimPackageMgr`/
`pimPlatformMgr`/`pimLanguageMgr` pointers (`CurrentPkgMgr`/`CurrentLangMgr`/
`CurrentPlatMgr`, seen in both `pimInstallMgrDlg.h` and `pimCustomDlg.h`) scoped to
whatever product is currently selected in the UI.

## Used Components (not owned)

`pimSessionInfo` and its `pimEntitlement` array are passed in (`SetSessionInfoFile`)
rather than owned — the dialogs are views over state that `pim`/`pim_core` own.

## Failure Modes

| Failure | Where | Result |
|---|---|---|
| `pimInstallMgrDlg::Initialize()` fails | Called from `pim/pim_src/pimTop.cxx` | Caller records `PIM_INITIALIZE_UI_ERROR` and skips `Refresh()`/`Display()` |
| Insufficient disk space | `pimInsufficientSpace` dialog exists specifically for this | Presumably blocks/warns before install proceeds — **not traced in this pass** |
| Credential/auth failure during browser challenge | `pimAuthDlg` + `pimSecurityCallback` (`pim/pim_src/pimBrowserInit.cxx`) | Returns `-3` (ABORT) up through `pimGetSecurityUIPrompt` if the user cancels the prompt loop |

## Extension Points

- New wizard page/dialog: follow the existing per-dialog header+.cxx pattern; page
  logic that needs to be split from declaration (as `*Refresh.cxx`/`*Actions.cxx` files
  are) can be added as additional out-of-line member-function files without touching
  the dialog's header.
- New custom table/tree widget for `pimInstallMgrDlg`: follow the nested-class pattern
  already used for `uiApplicationsList`/`uiCustomAppTree`/etc.

## Risks

- `pimInstallMgrDlg.h` bundles ~10+ distinct widget classes and the main dialog class
  in one header — high coupling, hard to reason about or test any one widget in
  isolation.
- UI logic is split across a header-declared class and several headerless `.cxx`
  files (`*Refresh.cxx`, `pimInstallMgrActions.cxx`) whose member-function
  declarations must therefore live back in `pimInstallMgrDlg.h` — easy for a future
  edit to declare a method in the wrong place or lose track of which `.cxx` implements
  which declared method, given there's no 1:1 file-to-header mapping here (unlike the
  rest of the codebase).
- No resource files (`.rc`) exist, so all layout is presumably done in code via the
  `ui*` toolkit's layout API (`uiLayout`, `AddLayout`, `AddLabelToLayout`, etc.) —
  visual changes require rebuilding, not resource editing.

## Key Source Files

- `pim_ui/includes/pimInstallMgrDlg.h` — largest header in the module, main dialog +
  nested widgets.
- `pim_ui/pim_ui_src/pimInstallMgrDlg.cxx`, `pimInstallMgrActions.cxx`,
  `pimEntitlementRefresh.cxx`, `pimProgressRefresh.cxx`, `pimWelcomeRefresh.cxx`,
  `pimEulaRefresh.cxx`, `pimLicenseIDRefresh.cxx` — the main dialog's behavior, split
  across files by concern.
- `pim_ui/pim_ui_src/pimFrictionlessTrialDlg.cxx`, `rpimDlg.cxx`, `upimDlg.cxx`,
  `pimsilentDlg.cxx` — alternate top-level dialogs.

## Important Functions

Not individually enumerated in this pass (would require reading every `.cxx` in the
module — reserved for a deeper follow-up if needed). Known entry points from other
modules: `pimInstallMgrDlg::Initialize()`, `::Refresh()`, `::Display()`,
`::SetSessionInfoFile()` (all called from `pim/pim_src/pimTop.cxx`, confirming their
existence and call order — see `docs/03_application_startup.md` §3).

---
*Phase 4 of the requested 20-phase documentation set. This module received a lighter
pass than `pim`/`pim_core`/`pim_util` — flagged for deeper analysis in a future
iteration (e.g. during Phase 5 class docs) if UI behavior detail becomes important.*
