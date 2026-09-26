# PIM Developer Onboarding Guide

> Synthesizes Phases 1–14. Every claim here is cross-referenced to the doc that
> established it — follow the links for the underlying source evidence. No source
> was modified in producing this guide.

## What Is PIM

PIM ("Parametric Installation Management," per the original task brief — not
spelled out as an acronym anywhere in the source itself) is PTC's native Windows C++
installer application for its Creo/Mathcad/Windchill Workgroup Manager and related
product lines. It is not a thin wrapper around Windows Installer — it's a full
in-house install engine: it parses product-definition XML, resolves prerequisites
between products, and executes a pipeline of typed install steps (file/archive copy,
MSI, self-extracting EXE, registry edits, shortcuts, scripts, Windows services, and a
"PSF" step) per product, all under a custom (non-MFC) dialog UI.

**Read first**: `docs/01_repository_inventory.md` §0 for exactly what is and isn't in
this codebase snapshot — critically, no build files, no resource files, the `btk`
base toolkit, and the external launcher EXEs are all **not included**. Every
onboarding step below inherits that limitation.

## Architecture Overview (5-minute version)

Four modules, one dependency direction:

```
pim (DLL, exported entry points: pimInstallerRun / pimFrictionlessTrialRun / pimRenewLicenseRun)
 -> pim_ui   (dialogs, on a custom non-MFC UI toolkit)
 -> pim_core (the install engine: pimSessionInfo, pimEntitlement, the Loop family, pimXmlFile, logging)
 -> pim_util (OS/MSI/CAB/FlexNet utilities, bottom of the stack)
```

External launcher EXEs (`pim.exe`, `pim_rm.exe`, and by inference `pim_re.exe`/
`pim_rl.exe` — see `docs/03_application_startup.md` §1) load the `pim` DLL and call
one exported function. **Full diagram and detail**: `docs/02_architecture_overview.md`.

The central object model, in one sentence: `pimSessionInfo` owns a collection of
`pimEntitlement` (one per installable product); each `pimEntitlement` is itself a
threaded `pimLoop` that drives 7 owned sub-step wrapper objects (`pimCopier`,
`pimMSICopier`, `pimSFXCopier`, `pimShortcuts`, `pimRegEdit`*, `pimServices`,
`pimDownloader`) plus two transient step types (`pimScriptLoop`, `pimPsfLoop`).
**\*Correction**: `pimRegEdit` is actually a process-wide singleton, not a
per-entitlement owned object like the other 6 — see
`docs/04_installation_flow.md` §7.

## How to Build

**Not possible from this archive alone.** No `Makefile`/`.vcxproj`/`.sln`/CMake
files are present (`docs/01_repository_inventory.md` §3). What *is* known:

- Each of the 4 modules compiles as a single translation unit via a unity-build
  aggregator (`<module>_src.cxx` `#include`s every sibling `.cxx`,
  `docs/01_repository_inventory.md` §1).
- `pim` builds to a DLL exporting `DLLEXPORTAPI`-marked functions
  (`pim/includes/exp_pim_dll.h`); `pim_core`/`pim_ui`/`pim_util` are statically
  linked into it.
- Real build tooling, compiler flags, and target platform lists must come from
  whatever internal PTC build system produces this (not this archive). If you're
  onboarding onto the *real* PTC repository (of which this archive is a partial
  export), find that build system's entry point first — this documentation set
  cannot substitute for it.
- External dependencies you'll need available to build: the `btk` toolkit,
  Xerces-C++, libzip, the Windows SDK (Cabinet/FDI, MSI), FlexNet Licensing client
  libraries, and whatever PTC-internal DLL(s) back `pim/includes/imp_pim_dll.h`'s
  imports.

## How to Debug

1. **Enable tracing**: set `PIM_DEBUG` in the environment before launching — switches
   the log level to `LG_T_TRACE` and dumps the process environment into the log
   (careful: this can leak secrets into the log file — `docs/09_logging_framework.md`
   §7).
2. **Find the log**: `pim_installmgr.log[.N]` under `%PIM_LOG_ENV%` if that env var
   is set, else the current user's My Documents folder
   (`docs/09_logging_framework.md` §4). Remember rotation renumbers `.1`/`.2`/`.3`
   every run based on modification time, not a stable run ID.
3. **Read the saved product XML**, not just the log — the `status_message`
   (`ENTITLEMENT_STATUS_STRING`) property is the one channel that survives a crash
   (`docs/10_error_handling.md` §4).
4. **Check `msiexec_return`** on any entitlement with an MSI component and compare
   against the exit-code table in `docs/11_troubleshooting.md` §1 before assuming a
   "failure" — `3010`/`1641` are actually success-with-reboot.
5. Use `docs/11_troubleshooting.md`'s ten playbooks as a starting checklist before
   diving into source — they're built directly from the mechanisms documented in
   Phases 5–13, not generic advice.

## How to Trace an Installation End-to-End

Read, in order:
1. `docs/03_application_startup.md` — from process launch through
   `pimInstallMgrDlg.Display()`.
2. `docs/04_installation_flow.md` — one entitlement's `OnExecute()` →
   `OnInstall()`/`OnUpdate()`/`OnReconfigure()`/`OnRollback()`, with the exact
   XML-declared skip-flags (`NoCopyStep`, `NoRegistryActions`, etc.) and
   custom-action hook points (`PreInstall`, `PostCopy`, etc.) that govern each run.
3. `docs/05_prerequisite_framework.md` and `docs/06_entitlement_framework.md` for
   the gating/state layer wrapped around that pipeline.

## How to Add a New Feature

There is no plugin system anywhere in this codebase — every extension point found is
a **source change plus a matching XML convention**, not a registration API:

- New CLI flag → extend the `if/else if` chain in `pimCommandLineArgs`
  (`pim/pim_src/pimGeneralInit.cxx`, `docs/modules/pim.md`).
- New session-wide property → just call `pimSessionInfo::SetProperty`/`GetProperty`
  with a new string key; no schema to update (`docs/classes/pimSessionInfo.md`) —
  but see that doc's Risk Analysis for the discoverability cost of this.
- New top-level run mode (alongside install/trial/renew) → add another
  `DLLEXPORTAPI` function in `pimTop.cxx` + `exp_pim_dll.h`, following the existing
  init-prefix → dialog → teardown skeleton (`docs/03_application_startup.md` §2, §7).

## How to Add a New Prerequisite Rule

Fully specified in `docs/05_prerequisite_framework.md`:

1. Write a new check function in `pim_core/pim_core_src/pimPrerequisite.cxx`
   returning `bool`.
2. Add an `else if (function_name == "yourFunctionName")` branch to
   `pimIsPrerequisiteSatisfied` (the dispatcher, line 304 of that file) calling it.
3. Reference the **exact same string** from a product XML's
   `<IS_INSTALLED_FUNC>yourFunctionName</IS_INSTALLED_FUNC>` element, with whatever
   attributes your function needs (see the `pimCreoTestPlatformAgent` example's
   `creoagent_version`/`creosvcs_version` attributes for the pattern).

**Pitfall**: there is no compile-time link between steps 2 and 3 — a typo in either
place silently degrades to "prerequisite never satisfied," with zero diagnostic
output pointing at the mismatch. Always grep both the dispatcher and the target
product XML together when adding or renaming a rule.

## How to Add a New Product

A "product" in PIM is a `<PRODUCT>` XML document plus whatever archive/MSI/SFX
payloads it references. See `docs/08_xml_configuration.md` §2 for the full
reconstructed schema (root attributes, `<CDSECTION>`, `<DISTRIBUTION>`/`<MSI>`,
`<REGISTRY>`, `<SHORTCUT>`, `<SCRIPT>`, `<SERVICE>`, `<CUSTOMACTION>`,
`<PREREQUISITE>`, `<PSF>`). Key things to get right, each backed by a specific
mechanism documented earlier:

- Root `name`/`version`/`shipcode`/`arp` attributes drive the Add/Remove Programs
  entry (`docs/07_registry_usage.md` §3) and uninstall-name generation
  (`docs/classes/pimEntitlement.md`).
- `<CDSECTION>` entries' `orig_value` must match the trailing filename of the
  corresponding download-URL entry exactly, or URL rewriting during a network
  install will fail to associate the two (`docs/08_xml_configuration.md` §5).
- Any stage you want to skip needs the matching `NoXxx` property set explicitly
  (`docs/04_installation_flow.md` §2's table) — there's no "auto-skip if this
  section is absent" behavior confirmed for every stage; check the specific stage's
  gating condition.

**No sample product XML exists in this archive** to copy from
(`docs/08_xml_configuration.md` §11) — you'll need a real example from the actual
PTC repository/build outputs to bootstrap a new one reliably.

## How to Extend the Install-Step ("Loop") Family

Every install step is a `pimLoop` subclass (`docs/classes/pimLoop.md`). To add one:

1. Subclass `pimLoop`, taking a `pimXmlFile*` in the constructor.
2. Implement your own `OnExecute()`/`OnInstall()`/`OnRollback()`/`OnTerminate()` —
   these are **not virtual on `pimLoop` itself**; each existing subclass defines its
   own set independently and `thrThread`'s dispatch (external) is what actually
   invokes them.
3. Follow the "one owner wraps one Loop" pattern used by 6 of the 7 existing step
   types (`pimCopier`→`pimCopyLoop`, etc., `docs/02_architecture_overview.md` §5) —
   or the transient-instantiation pattern used by `pimScriptLoop`/`pimPsfLoop` if a
   persistent per-entitlement wrapper isn't warranted.
4. Wire it into `pimEntitlement`'s pipeline (the sequence documented in
   `docs/04_installation_flow.md` §2) — this requires editing the 7,649-line
   `pimEntitlement.cxx` directly; there's no step-registration mechanism.
5. Give it a `NoYourStep`-style XML skip flag if it should be optional, following the
   existing convention.

## Key Classes to Learn (in priority order)

1. **`pimLoop`** (`docs/classes/pimLoop.md`) — the threading/cancel/error contract
   every install step shares. Understand this before touching any step type.
2. **`pimEntitlement`** (`docs/classes/pimEntitlement.md`,
   `docs/06_entitlement_framework.md`) — the 7,649-line central orchestrator. Highest
   change-risk file in the codebase; read `docs/04_installation_flow.md` before
   editing it.
3. **`pimXmlFile`** (`docs/classes/pimXmlFile.md`) — the DOM wrapper with a
   **confirmed real locking-bug history** ("Fixed xml mutex issues," June 2026). Any
   change touching `PreRead`/`PreWrite`/`PushReadToWrite`/`PopWriteToRead` should be
   treated as high-risk.
4. **`pimSessionInfo`** (`docs/classes/pimSessionInfo.md`) — the top-level session
   state and property bag.
5. **`pimCopyLoop`** (`docs/classes/pimCopyLoop.md`) — a good "smallest complete
   example" of the Loop pattern, and the one class this session's earlier work
   actually modified (CAB→ZIP extraction) — worth reading as a template for what a
   focused, well-scoped change to one Loop subclass looks like.

## Common Debugging Techniques (grounded in this codebase specifically)

- **Grep the message catalog, not just the code.** Every `pimLogXxx`/`pimUIXxx`
  message ID has a human-readable definition in
  `pim_core/messages/usascii/pim.msg` (`docs/09_logging_framework.md` §5.2) — faster
  than tracing `pimMessage(pimSomeId, ...)` call sites to guess what a log line means.
- **Distinguish the message's `class` (Error/Warning/Info/Prompt) from the log
  macro used to emit it** — they're independently maintained
  (`docs/09_logging_framework.md` §5.2) and can disagree.
- **Check both the function return value and `pimGetLastError()`** — they are not
  guaranteed to agree for the same failure (`docs/10_error_handling.md` §2.2's
  documented `-104`-vs-`PIM_GENERAL_XML_ERROR` example).
- **For anything registry-related, remember the singleton.** A slow/stuck registry
  operation in one entitlement can make `pimRegEdit::GetInstance()` appear to hang
  for every other concurrently-installing entitlement
  (`docs/04_installation_flow.md` §7).
- **Don't assume rollback ran.** `ROLLBACK_CANCELLED_INSTALL` is not defined anywhere
  in this archive (`docs/04_installation_flow.md` §5) — verify against the actual
  build's compiler flags before assuming a canceled/interrupted install was rolled
  back automatically.

## Testing Requirements

**Not established in this pass.** No test files, test framework references, or CI
configuration were found anywhere in the archive during Phase 1's inventory. Whatever
testing discipline exists for PIM (manual QA, an internal test harness, etc.) is
external to this codebase snapshot — this is a gap future onboarding material should
fill in from the real engineering org, not from source alone.

## Safe Modification Practices (summary — full detail in `ai-context/ai_readme.md` once generated)

- Treat `pimEntitlement.cxx`, `pimXmlFile.cxx`/`.h`, and anything touching the
  `pimRegEdit` singleton as high-risk; changes there warrant extra review and, ideally,
  a fresh full read of the affected method before editing (this documentation set's
  per-phase "not individually re-traced this pass" notes mark exactly where that's
  still needed).
- Prefer the existing "owner wraps one Loop" pattern for new install steps rather than
  inventing a new lifecycle shape.
- Keep the dispatcher-based extension points (prerequisites, custom-action `when`
  points) documented in the same place their matching XML string lives — this
  codebase has no compiler safety net for that link, so documentation/comments are
  the only guardrail.

---
*Phase 15 of the requested 20-phase documentation set. No source was modified.*
