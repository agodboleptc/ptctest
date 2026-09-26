# Module: `pim_core`

> Evidence-based; see `docs/01_repository_inventory.md` §0 for corpus scope.

## Purpose

The install engine and shared runtime state of PIM. Everything that models "what is
installable, what is installed, and how to install/uninstall/rollback it" lives here:
the XML DOM wrapper, the session/entitlement object model, the family of threaded
"Loop" install-step classes, prerequisite handling, package/platform/language
selection managers, logging, and localization support.

By file/line count this is the largest module: 47 headers, 41 `.cxx` files (+ unity
aggregator), ~41,000+ of the archive's ~82,000 total lines are concentrated here
(`pimEntitlement.cxx` alone is 7,649 lines).

## Responsibilities

- **XML processing**: `pimXmlFile` (`pimXmlFile.h/.cxx`) wraps Xerces-C DOM
  parse/read/write for every XML document in the system.
- **Session/product model**: `pimSessionInfo` (owns the entitlement collection,
  license/security context, a session-wide string-keyed property bag) and
  `pimEntitlement` (one per installable product; itself a `pimLoop`, i.e. a threaded,
  cancelable unit of work).
- **Install-step execution**: the `pimLoop` base class plus 9 concrete step types —
  `pimCopyLoop`, `pimMSILoop`, `pimSFXLoop`, `pimScriptLoop`, `pimShortcutLoop`,
  `pimRegEditLoop`, `pimServiceLoop`, `pimPsfLoop`, `pimDownloadLoop` — each paired
  with an owner class (`pimCopier`, `pimMSICopier`, `pimSFXCopier`,
  `pimShortcuts`, `pimRegEdit`, `pimServices`, `pimDownloader`) except
  `pimScriptLoop`/`pimPsfLoop`, which `pimEntitlement` creates directly. **All 7
  owner classes are process-wide singletons** (`GetInstance()`), each serializing
  its operation type across every concurrently-installing entitlement — not
  per-entitlement objects (see `docs/02_architecture_overview.md` §5, corrected).
- **Prerequisite graph**: hard/soft prerequisite tracking between `pimEntitlement`
  instances (`AddPrerequisite`, `IsPrerequisiteSatisfied`, `GetNextPrerequisite`,
  `IsOnlySoftPrerequisiteNeeded`).
- **Discovery**: `pimInterrogator` (what's already installed / known license sources),
  `pimLocate` (finding files: skin, translation, physical image, etc.), `pimGetAvailable`
  / `pimGetAvailableProduct` (available-product queries), `pimGetMediaDetails`.
- **Selection managers**: `pimPackageMgr`, `pimPlatformMgr`, `pimLanguageMgr` — compute
  which packages/platforms/languages apply to a product XML, consumed by `pim_ui`.
- **Logging & messages**: `pimLog` (`InitLog`, `LG_INFO`/`LG_ERROR`/`LG_DEBUG` macros,
  log rotation via `pimProcessOldLogs`), `pimMessage`/`pim_msg.h` (message-ID → localized
  string, backed by `messages/<lang>/pim.msg`).
- **Misc core services**: `pimCommandMgr`, `pimCleanCache`, `pimCustomActions`,
  `pimFlexAdminInstall`, `pimHelpMgr`, `pimMathcadPrime`, `pimPTCDotCom`,
  `pimTranslateMgr`, `pimUpdateMgr`, `pimSilent` (silent-install support), `pimSessionInfo`'s
  companion `pimLicenseSource`.

## Dependencies

- `pim_util` — CAB/MSI/FLEXnet/registry-registration/OS-utility functions consumed by
  the Loop classes and `pimEntitlement` (e.g. `pimCopyLoop` uses `pimCab`-family
  functions, `pimMSILoop` uses `pimMSI`).
- External: Xerces-C++ (DOM), `btk` toolkit (`btkString`, `btkFSEntry`, `thrThread`,
  `btkLogManager`, etc.), Win32 registry API (in `pimRegEditLoop.cxx`), PTC platform
  symbols from `imp_pim_dll.h` (`msgID_sput_buffer`, `ginst_*` service functions used
  by `pimServiceLoop`).

## Consumers

- `pim` — `pimTop.cxx` drives `pimSessionInfo`/`pimEntitlement`/`pimXmlFile`/
  `pimTranslateMgr`/`pimLog` directly.
- `pim_ui` — dialogs consume `pimSessionInfo`, `pimEntitlement`, `pimPackageMgr`,
  `pimPlatformMgr`, `pimLanguageMgr`, `pimInterrogator` to render and drive install UI.

## Owned Components

See `docs/02_architecture_overview.md` §5 (Ownership diagram, corrected) for the
full graph. In short: `pimSessionInfo` owns N `pimEntitlement`s + one
`pimInterrogator`. `pimEntitlement` does **not** own `pimCopier`/`pimMSICopier`/
`pimSFXCopier`/`pimShortcuts`/`pimRegEdit`/`pimServices`/`pimDownloader` — those
are process-wide singletons it merely *calls* (`GetInstance()`) — but it does
directly create and own `pimScriptLoop`/`pimPsfLoop` transiently, per call, with
no wrapper of any kind.

## Used Components (not owned)

`pimPackageMgr`/`pimPlatformMgr`/`pimLanguageMgr` are constructed ad hoc wherever
needed (e.g. inside `pimEntitlement.cxx`'s URL-update logic:
`pimPackageMgr nPkgs(application_definition, &nPlats, &nLangs);`) rather than held as
persistent members of any one class.

## Failure Modes

| Failure | Where | Result |
|---|---|---|
| XML parse error | `pimXmlFile::DoRead()` / callers check `ErrorOccured()` | Callers (e.g. `pimTop.cxx`) call `pimHitError(PIM_GENERAL_XML_ERROR)` and typically abort the current operation |
| Archive extraction failure (`.zip`, post this-session's change) | `pimCopyLoop::pimInstallCab`/`pimRemoveCab` | Appends `"Zip extraction failed."`/`"Unexpected archive format."` to the caller-supplied `errors` array; caller logs each via `pimLogExtractFileError` and surfaces `pimAddExtractFileError` |
| Registry write failure | `pimRegEditLoop.cxx` (`win32RegCreateKeyExA`/`win32RegSetValueExA` return codes) | **Not traced to exact error-propagation path in this pass** — flagged for Phase 10 (`docs/07_registry_usage.md`) |
| Prerequisite not satisfied | `pimEntitlement::IsPrerequisiteSatisfied()` | Caller (`InstallPreReqSilent` in `pim/pim_src/pimTop.cxx`) logs an error and sets session property `"preq_failed"="Y"`; UI path presumably surfaces this differently (**not traced this pass**) |
| Thread crash in a Loop's `OnExecute()` | `pimLoop::OnUnhandled` | Logs `"thread unhandled exception ... Crash in secondary thread"` and calls `btkCrash(...)` (external — behavior UNKNOWN, likely triggers a crash-report/terminate path) |

## Extension Points

- New install-step type: subclass `pimLoop`, implement `OnExecute`/`OnInstall`/
  `OnRollback`/`OnTerminate` (the pattern every existing `*Loop` follows), optionally
  add an owner wrapper class mirroring `pimCopier`/`pimMSICopier`/etc.
- New prerequisite rule: extend `pimEntitlement`'s prerequisite methods (implementation
  in `pimEntitlement.cxx`, not fully traced this pass — see Phase 8,
  `docs/05_prerequisite_framework.md`).
- New session-wide property: `pimSessionInfo::SetProperty`/`GetProperty` is a generic
  string-keyed bag already used for dozens of ad hoc values (`PLATFORM_PROPERTY`,
  `MEDIA_PROPERTY`, `SHIPCODE_PROPERTY`, etc.) — no schema/registration required to add
  more, which is convenient but also a discoverability risk (see Risks).

## Risks

- **`pimEntitlement.cxx` at 7,649 lines** is by far the largest, most central,
  highest-blast-radius file in the codebase — it is simultaneously the product model,
  the install/uninstall/rollback orchestrator, and the sole caller of every
  install-step singleton (plus the direct owner of its two transient
  `pimScriptLoop`/`pimPsfLoop` instances). Any change here has wide potential impact.
- The session property bag (`SetProperty`/`GetProperty` by string key) has no static
  schema — property names are defined as scattered `#define`s and matched only by
  string equality; a typo in a key name fails silently at the call site (returns
  `false`/empty rather than a compile error).
- `pimScriptLoop`/`pimPsfLoop` lack the owner-wrapper pattern used by the other 7 Loop
  types, meaning their lifecycle is managed ad hoc inside `pimEntitlement.cxx` at
  multiple call sites (~8 found) — inconsistent pattern increases the chance of a
  missed cleanup path being added for one but not the other in future changes.
- `pimRegEditLoop.cxx` performs live registry writes with root keys selected by a
  string switch (`"HKEY_LOCAL_MACHINE"`, etc.) driven by XML content — a malformed or
  malicious product XML could in principle target unintended registry roots; no
  allow-list narrowing was observed in this pass (full analysis belongs in Phase 10).
- **CONFIRMED**: `pimServices` (one of the 7 install-step singletons above) does not
  actually enforce "one operation at a time" the way its 6 siblings do —
  `Create_low()` releases its serialization lock immediately after starting the
  operation's thread rather than holding it for the operation's duration. A
  concurrent second call would delete a still-running `pimServiceLoop` out from
  under its own thread. See `docs/classes/pimServices.md` and
  `ai-context/business_rules.yaml`'s `pimservices_lock_does_not_serialize` — this
  is the highest-severity confirmed finding across this whole documentation set.
- `pimMSICopier` and `pimSFXCopier` (two of the 7 singletons) share a single
  external lock (`pimOKToRunMsiexec()`/`pimFreeMsiexec()` in `pim_util`) rather
  than each having an independent one — an MSI install and an SFX install cannot
  run concurrently process-wide, a tighter coupling than their separate class
  names suggest.

## Key Source Files

- `pim_core/pim_core_src/pimEntitlement.cxx` (7,649 lines)
- `pim_core/pim_core_src/pimRegEditLoop.cxx` (742 lines) — real Win32 registry calls
- `pim_core/pim_core_src/pimCopyLoop.cxx` — file copy / archive extraction (edited this
  session to use `btkUnzip` for `.zip`)
- `pim_core/pim_core_src/pimLoop.cxx` (248 lines) — shared threaded-step base behavior
- `pim_core/pim_core_src/pimXmlFile.cxx`, `pimSessionInfo.cxx` (not line-counted this
  pass), `pimLog.cxx` (173 lines)
- `pim_core/messages/<lang>/pim.msg` — localized message catalogs (11 languages)

## Important Functions

| Function/Method | File | Role |
|---|---|---|
| `pimLoop::Execute()` | `pimLoop.cxx` | Generic threaded-step launch (see `docs/03_application_startup.md` §5) |
| `pimEntitlement::IsPrerequisiteSatisfied`, `AddPrerequisite`, `GetNextPrerequisite` | `pimEntitlement.h/.cxx` | Prerequisite graph API |
| `pimCopyLoop::pimInstallCab` / `pimRemoveCab` | `pimCopyLoop.cxx` | Archive install/removal (ZIP, post this-session edit) |
| `InitLog` | `pimLog.cxx` | Log directory selection, rotation, logger/area creation |
| `GetLoggerManager` | `pimLog.cxx` | Per-thread logger accessor (called at the top of every `OnExecute()`, e.g. `pimCopyLoop::OnExecute`) |
| `pimSessionInfo::AddEntitlement` | `pimSessionInfo.cxx` (not read line-by-line this pass) | Parses a product XML file and registers a new `pimEntitlement` |

---
*Phase 4 of the requested 20-phase documentation set.*
