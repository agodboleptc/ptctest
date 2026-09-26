# Module: `pim`

> Evidence-based; see `docs/01_repository_inventory.md` §0 for corpus scope.

## Purpose

Top-level orchestration module. Builds into a DLL (evidenced by
`pim/includes/exp_pim_dll.h` using `DLLEXPORTAPI`) that exports the handful of
"Run" entry points an external launcher EXE calls to drive install, uninstall,
trial-registration, and license-renewal flows. This is the module a `.exe` process
actually loads.

## Responsibilities

- Own the three top-level run functions (`pimInstallerRun`, `pimFrictionlessTrialRun`,
  `pimRenewLicenseRun`) in `pim/pim_src/pimTop.cxx` — see `docs/03_application_startup.md`
  for the full trace.
- Own process-wide initialization/teardown of Xerces-C (`pimXmlInit.cxx`), the browser
  credential subsystem (`pimBrowserInit.cxx`), UI toolkit (`pimUIInit.cxx`), and
  general/library init (`pimGeneralInit.cxx`).
- Hold CLI-argument parsing and resulting global run-mode state
  (`pimCommandLineArgs`, `pimGetProductMode`/`pimSetProductMode`, trial/school/beta/
  silent/allpacks/basepack flags — all in `pimGeneralInit.cxx`).
- Own a simple process-lifetime error stack (`pimHitError`/`pimClearErrors`/
  `pimGetLastError`, `pim/pim_src/pimExit.cxx`).
- Own `pimCDMaker` (`pim/pim_src/pimCDMaker.cxx`) — **not traced in this pass**; name
  suggests it supports building CD/media images rather than installing from them,
  invoked from the DLL-exported `pimInitXMLFromMsi`/`pimInitXMLFromSFX` functions per
  their doc comments ("cd_maker calls this ... Populate details from the MSI into the
  xmlfile"). Full behavior **UNKNOWN** — not read in this pass; flagged for a future
  module-detail pass if needed.
- Define the DLL's public export contract (`exp_pim_dll.h`) and its inbound import
  contract from external PTC platform DLL(s) (`imp_pim_dll.h`).

## Dependencies

- `pim_core` — `pimSessionInfo`, `pimXmlFile`, `pimEntitlement` (indirectly, through
  `pimSessionInfo`), `pimLog`, `pimTranslateMgr`.
- `pim_ui` — `pimInstallMgrDlg`, `pimFrictionlessTrialDlg`, `rpimDlg`,
  `pimsilentDlg`, `upimDlg`, `pimTextDlg`, `pimGetNewPimDlg`.
- `pim_util` — `pimMSI` (`btkDlmGetImageVersionW` for `msi.dll` version), `pimFLEXnet`
  (triad/license-source logic in `pimTop.cxx`), `pimRegisterProduct`, `pimScramble`
  (trial SCN/email/PSF obfuscation), `pimWindows` (`pimWindowsPathTest`,
  `pimIsServer2008R2`/etc. platform checks).
- External: Xerces-C++, the UI toolkit (`<uicxx.h>`, `<uit_l01_skin_chooser.h>`), the
  browser/credentials library (`<bs_pro_browser_security.h>`), `btk` toolkit broadly,
  and whatever binary supplies `imp_pim_dll.h`'s symbols.

## Consumers

The external launcher `.exe`s (`pim.exe`, `pim_rm.exe`, and by inference `pim_re.exe`/
`pim_rl.exe`) — **not included in this archive**. No other module in the archive calls
into `pim`; dependency flow is one-directional (`pim` depends on the other three, not
vice versa — confirmed in `docs/02_architecture_overview.md` §4).

## Owned Components

- Process-wide init state flags (`is_xml_init`, `is_init`, `is_browser_init`,
  `is_ui_init` — each a file-local `static bool` guarding one-time init).
- CLI-derived global state in `pimGeneralInit.cxx` (destination path, license source/
  preference, applications/language filter lists, trial/school/beta/silent/upgrade/
  backup/all-packs/base-pack flags, frictionless SCN/email/PSF values, product mode).
- The error stack in `pimExit.cxx`.

## Used Components (not owned)

`pimSessionInfo` and `pimEntitlement` instances are created as stack locals /
through `pimSessionInfo::AddEntitlement` inside the `Run` functions but are not
long-lived module state — they live and die within a single `pim*Run` call.

## Failure Modes

| Failure | Where | Result |
|---|---|---|
| Xerces init throws | `pimXmlInit()` | `pimInstallerRun` returns `PIM_INITIALIZE_XML_ERROR` immediately |
| `EnableThreading`/general init throws | `pimGeneralInit()` | Returns `PIM_INITIALIZE_LIBRARY_ERROR` |
| Browser/credentials init throws | `pimBrowserInit()` | Returns `PIM_INITIALIZE_COMMS_ERROR` |
| UI init throws or `uiInitialize` fails | `pimUIInit()` | Returns `PIM_INITIALIZE_UI_ERROR`; on this specific error, shutdown skips `pimUITerminate(true)` (guarded by `if (pimGetLastError() != PIM_INITIALIZE_UI_ERROR)`) |
| No PTC.com connectivity on a fresh network install | `pimInstallerRun`, network-install branch | Returns `PIM_PTC_IT_COMMS_ERROR` if `NO_PTCDOTCOM_PROPERTY` is set |
| Platform bitness mismatch (32-bit OS running a 64-bit-only or media-declared-incompatible build) | `pimInstallerRun`, both media and network branches | Blocking `uiMessage` error dialog, `pimHitError(PIM_GENERAL_XML_ERROR)`, `return -104` |
| `pimInstallMgrDlg::Initialize()` fails | `pimInstallerRun` | `pimHitError(PIM_INITIALIZE_UI_ERROR)`; flow still proceeds to the `Wait()`/shutdown tail (does not early-return) |

No retry/backoff logic is visible for any of these; all are one-shot try/catch guards
around external calls.

## Extension Points

- New CLI flags: extend the `if/else if` chain in `pimCommandLineArgs`
  (`pimGeneralInit.cxx`).
- New top-level run modes: add another `DLLEXPORTAPI` function alongside
  `pimInstallerRun`/`pimFrictionlessTrialRun`/`pimRenewLicenseRun` in `pimTop.cxx` +
  `exp_pim_dll.h`, following the same init-prefix/dialog/teardown skeleton.
- New "product family" detection: extend the family-name string comparisons in the
  physical-media branch of `pimInstallerRun` (currently matches `"Windchill Workgroup
  Manager"`, `"PTC Mathcad"`, `"Creo AR Plugins"`, `"Help Centers"`, `"Creo"`, plus a
  `mkscomponents.xml`/`schematics.xml` tag check) and the corresponding
  `PIM_*_MODE` enum (defined in `pim_core`, not this module — **UNKNOWN exact header**,
  likely `pimCore.h` or `pimGeneralInit.h`).

## Risks

- **`pimTop.cxx` is a single 3,284-line file** covering three entire application
  flows with 149 revision-history entries spanning 2011–2026 — high change-coupling
  risk; a change intended for one run mode (e.g. trial) can easily affect shared
  helper logic used by the install mode.
- The bitness-check pattern (`GetSystemWow64DirectoryA` + `GetLastError() ==
  ERROR_CALL_NOT_IMPLEMENTED`) is a documented-in-comment workaround
  ("this windows function always fails ... this will indicate we are running PIM on a
  32-bit machine") — fragile if Windows behavior around this API ever changes.
- Global mutable state via file-local `static` variables in `pimGeneralInit.cxx` means
  `pim*Run` functions are not safely re-entrant / cannot process two independent runs
  in the same process — consistent with each being launched as its own OS process
  via a dedicated EXE.
- No visible rollback of already-succeeded init steps (Xerces, browser, UI) if a later
  init step fails — relies on process exit to clean up.

## Key Source Files

- `pim/pim_src/pimTop.cxx` (3,284 lines) — the three Run entry points.
- `pim/pim_src/pimGeneralInit.cxx` (726 lines) — CLI parsing + global run-mode state.
- `pim/pim_src/pimUIInit.cxx`, `pimXmlInit.cxx`, `pimBrowserInit.cxx`, `pimExit.cxx` —
  narrow init/teardown modules.
- `pim/pim_src/pimCDMaker.cxx` (221 lines, not traced in depth this pass).
- `pim/includes/exp_pim_dll.h`, `imp_pim_dll.h` — the module's only formal interface
  contracts.

## Important Functions

| Function | File | Role |
|---|---|---|
| `pimInstallerRun` | `pimTop.cxx` | Main install/uninstall entry point |
| `pimFrictionlessTrialRun` | `pimTop.cxx` | Trial/frictionless licensing entry point |
| `pimRenewLicenseRun` | `pimTop.cxx` | License renewal entry point |
| `InstallPreReqSilent` | `pimTop.cxx` | Silent-mode prerequisite install loop (polls `IsDone()`/`IsPrerequisiteSatisfied()` on each entitlement's prerequisites) |
| `pimCommandLineArgs` | `pimGeneralInit.cxx` | CLI flag parser |
| `pimGeneralInit`, `pimXmlInit`, `pimBrowserInit`, `pimUIInit` | respective files | One-time subsystem init, each idempotent via a static guard |
| `pimHitError`/`pimGetLastError`/`pimClearErrors` | `pimExit.cxx` | Process error-code stack |

---
*Phase 4 of the requested 20-phase documentation set.*
