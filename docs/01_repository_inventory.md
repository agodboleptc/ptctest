# PIM Repository Inventory

> Source analyzed: archive `installmgr.zip` (uploaded 2026-09-21), extracted to a working
> tree containing four top-level module folders. All findings below are traced to specific
> files. Anything not observable from the supplied archive is marked **UNKNOWN**.

## 0. Corpus Scope (read this first)

The archive contains **187 files / ~82,000 lines of C++** across 4 modules. It contains:

- Header (`includes/*.h`) and implementation (`*_src/*.cxx`) files for `pim`, `pim_core`,
  `pim_ui`, `pim_util`.
- Localized message tables (`pim_core/messages/<lang>/pim.msg`).

It does **not** contain:

- Any build system files (no `Makefile`, `.vcxproj`, `.sln`, `SConstruct`, CMake, etc.) —
  **UNKNOWN** how these 4 modules are actually compiled/linked (unity-build `.cxx`
  aggregator files strongly suggest an internal PTC build tool, not a standard one — see
  §3).
- Any `.rc` resource script, dialog template, icon, or bitmap resource.
- The `btk` toolkit source (base class library used pervasively — `btkString`,
  `btkFSEntry`, `btkXmlFile`-level DOM helpers, `thrThread`, `btkUnzip`, `uiDialog`, etc.).
  Two `btk` files (`btkunzip.h`, `btkunzip.cxx`) were supplied in an earlier, separate
  upload in this session and are documented here only where they explain call sites in
  `pim_core`.
- Any other PTC shared library referenced by `pim_core/includes/pim_msg.h` / DLL import
  headers (see §5) — e.g. whatever exports `ginst_install_service`, `authorize_main`,
  `pro_search_component_set`.
- The small launcher `.exe` projects that actually own `main`/`WinMain` (see §4).

Every phase of this documentation set inherits this limitation. Where a conclusion
depends on one of these missing pieces, it is marked **UNKNOWN / EXTERNAL**.

## 1. Folder Structure

```
pim/            Top-level orchestration module; builds the exported "pim" component.
├── includes/       Public headers for this module (4 files)
└── pim_src/        Implementation (7 .cxx files + 1 unity-build aggregator)

pim_core/       Core install-engine module: XML/entitlement model, install "Loop" step
                classes, session state, logging, prerequisite/package/platform/language
                managers.
├── includes/       47 headers
├── pim_core_src/   41 .cxx files + 1 unity-build aggregator
└── messages/       Localized message catalogs, one directory per language (11 languages)

pim_ui/         Dialog/UI layer (product selection, license entry, progress, EULA, etc.)
├── includes/       21 headers
└── pim_ui_src/     26 .cxx files + 1 unity-build aggregator

pim_util/       Low-level OS/platform utilities: CAB extraction, MSI wrapper, FLEXnet
                license query, Windows helpers, string scrambling, product registration.
├── includes/       11 headers
└── pim_util_src/   9 .cxx files + 1 unity-build aggregator
```

Each module's `<module>_src/<module>_src.cxx` is a **unity-build aggregator**: it
`#include`s every sibling `.cxx` file in the directory and contains no other code, e.g.
`pim_core/pim_core_src/pim_core_src.cxx` includes all 40 other `.cxx` files in that
folder. This is the only concrete evidence of how compilation units are organized: each
module compiles as a single translation unit.

## 2. File Categories

| Category | Location(s) | Notes |
|---|---|---|
| Public headers | `*/includes/*.h` | One header per class/subsystem, consistent naming `pim<Name>.h` |
| Implementation | `*/‌*_src/*.cxx` | One `.cxx` per header, plus the unity aggregator |
| DLL export/import contracts | `pim/includes/exp_pim_dll.h`, `pim/includes/imp_pim_dll.h` | See §5 |
| Localization data | `pim_core/messages/<lang>/pim.msg` (+ `usascii/pim.msg.LOCAL`) | Message-catalog format, keyed IDs consumed via `pim_core/includes/pim_msg.h` / `pimMessage` (see `docs/09_logging_framework.md`). **Re-verified in a later pass**: `pim.msg.LOCAL` is confirmed to be an OLDER snapshot of the `usascii` catalog, not a build artifact or a locale variant — a direct `diff` against `pim.msg` shows it is missing at least 2 message IDs present in the main file (`pimUISpaceNeeded`, `pimUIInstallStatusDownloadError`), and its opening line lacks the main file's UTF-8 BOM. No code in this archive references the `.LOCAL` file by name — it is confirmed unused, a leftover from an editing/merge step rather than a load-bearing artifact. |
| Registry logic | `pim_core/pim_core_src/pimRegEdit.cxx`, `pimRegEditLoop.cxx`, `pim_util/pim_util_src/pimRegisterProduct.cxx`, parts of `pimInterrogator.cxx`, `pimLocate.cxx`, `pimPrerequisite.cxx`, `pim_util/pim_util_src/pimMSI.cxx`, `pimUtil.cxx`, `pimWindows.cxx` | `pimRegEditLoop.cxx` contains the actual Win32 registry calls (`win32RegCreateKeyExA`, `win32RegSetValueExA`, `win32RegOpenKeyExA`, `HKEY_LOCAL_MACHINE`/`HKEY_CURRENT_USER`/`HKEY_CLASSES_ROOT`/`HKEY_USERS`) |
| XML processing | `pim_core/pim_core_src/pimXmlFile.cxx` (+ `.h`), consumers throughout `pim_core` and `pim/pim_src/pimTop.cxx` | Built directly on Xerces-C DOM types (`DOMNode`, `DOMNodeList`, `DOMElement`, `DOMNamedNodeMap`, `XMLString`) — see §6 |
| Configuration/product data | No literal `.xml` files shipped in the archive — **re-verified**: `find . -iname "*.xml"` across the entire extracted tree returns zero matches | Product/entitlement XML files (`creobase.xml`, `image.xml`, `mkscomponents.xml`, `schematics.xml`, `*trial.xml`, FlexNet admin/lmgrd XML, and the `<TRANSLATION>`-bearing translation files consumed by `pimTranslateMgr`, see `docs/classes/pimTranslateMgr.md`) are referenced **by name in code** (e.g. `pim/pim_src/pimTop.cxx`) but are runtime-supplied data, not part of this source tree. Every XML schema documented in this set (`docs/08_xml_configuration.md`, `ai-context/xml_schema.yaml`) is therefore inferred entirely from parsing code (`getElementsByTagName`/`getNamedItem` calls and the string constants in `pim_core/includes/pimDefs.h`), never confirmed against a real sample document — no schema-validation contradiction has been found, but a genuine sample could still reveal fields or structures the parsing code never exercises. |
| Resource files | None present | UI is built on a custom dialog toolkit (`uiDialog`, `uiTable`, `uiList`, `uiTree`, `uiInputPanel` — see `pim_ui/includes/pimInstallMgrDlg.h`), not Win32 `.rc`/MFC resources, so the absence is expected rather than an omission |
| Installer components | `pim_core/pim_core_src/pimCopyLoop.cxx`, `pimMSILoop.cxx`, `pimSFXLoop.cxx`, `pimScriptLoop.cxx`, `pimShortcutLoop.cxx`, `pimRegEditLoop.cxx`, `pimServiceLoop.cxx`, `pimPsfLoop.cxx`, `pimDownloadLoop.cxx`, and `pim_util/pim_util_src/pimCab.cxx`, `pimMSI.cxx`, `pimCopyInstaller.cxx` | See §7 |

## 3. Build System

**UNKNOWN, RE-VERIFIED IN A LATER PASS.** No build definition files of any kind are
present in the archive — re-confirmed via an exhaustive filename search across every
file in the extracted tree for any `Makefile*`, `*.mk`, `CMakeLists*`, `*.vcxproj*`,
`*.sln`, `*.cmake`, `SConstruct`, `*.gyp`, `build.xml`, `*.bat`, or `*.sh` pattern:
**zero matches**. The only structural evidence remains the unity-build aggregator
`.cxx` files described in §1, which implies:

- Each of the 4 modules is compiled as one translation unit.
- Build tooling is almost certainly an internal PTC build system (naming conventions
  like `exp_pim_dll.h`/`imp_pim_dll.h`, `DLLEXPORTAPI`/`DLLIMPORTAPI` macros from
  `<btkdeclspecs.h>`, and the `btk`-prefixed toolkit strongly resemble a large internal
  C++ platform, consistent with this being PTC's Creo/Mathcad/Windchill installer,
  "PIM" = Parametric Installation Management per the task brief).
- No compiler flags, target platform lists, or link dependency manifests are derivable
  from source alone beyond what's inferable from `#include`s (§6).

**CONFIRMED, concrete evidence for "each module is a separate translation unit"**
(found while giving `pimTranslateMgr` its own dedicated pass, see
`docs/classes/pimTranslateMgr.md`): `PIM_TRANSLATE_XML` is `#define`d in exactly one
place in the entire archive, `pim/pim_src/pimTop.cxx:185` — a plain `.cxx` file, not a
shared header — and its own `#ifdef PIM_TRANSLATE_XML` guard at `:474` (same file) is
therefore live, while an identically-guarded block in
`pim_core/pim_core_src/pimEntitlement.cxx:992` (a **different** module's unity build,
`pim_core_src.cxx`, which never `#include`s `pimTop.cxx` or anything that does) can
never see this macro and is permanently dead code — concealing a compile-breaking
undeclared-identifier typo (`Eptr` for `EPtr`) that would only surface if `pim_core`
and `pim` were ever merged into one compilation unit. This is the first
directly-observed, mechanism-level confirmation (not just naming-convention inference)
that `pim` and `pim_core` genuinely compile as separate translation units in whatever
the real build system is.

## 4. Entry Points

No `main()`, `WinMain()`, `DllMain()`, or `ServiceMain()` exists anywhere in the archive
(confirmed by exhaustive grep across all 187 files). Instead, `pim/pim_src/pimTop.cxx`
defines three top-level "Run" functions that are the *de facto* application entry points,
each fully self-contained (init → build session → show dialog → run → teardown):

| Function | File | Purpose (from code) |
|---|---|---|
| `pimInstallerRun(int argc, const char *argv[])` | `pim/pim_src/pimTop.cxx` | Main install/uninstall flow: locates a physical media image or falls back to network/PTC.com entitlement lookup, builds a `pimSessionInfo`, loads applicable product entitlement XML files, determines "product mode" (Creo/Mathcad/WGM/Schematics/MKS/FlexOnly/ARPlugin/CreoHelp), shows `pimInstallMgrDlg`, then finalizes/shuts down. |
| `pimFrictionlessTrialRun(int argc, const char *argv[])` | same file | Trial/frictionless-licensing flow driven by positional CLI args (`-optnum <opt_num> <scn> <email> <psf> [-cleanup|-trial_reg|-frictionless_reg]`), decodes obfuscated SCN/email/PSF values, shows `pimFrictionlessTrialDlg`. |
| `pimRenewLicenseRun(int argc, const char *argv[])` | same file | License-renewal flow, shows `rpimDlg`. |

These three functions — plus `pimSilentInstallFromXML`, `pimUninstallFromXML`,
`pimInitXMLFromMsi`, `pimInitXMLFromSFX`, `pimRegistryKnownInstalls`, `pimCleanup`,
`pimGetVersion`, `pimShowHelp[WGM]`, `pimGetMessage`, `pimTestForNewerVersion` — are
declared `DLLEXPORTAPI` in `pim/includes/exp_pim_dll.h`, confirming the `pim` module
builds into a **DLL** (name UNKNOWN — never spelled out as a literal filename in the
supplied source; comment history in `pimTop.cxx` calls the products `setup.exe`,
`pim_rm.exe` (uninstall, renamed from `Uninstall.exe`), `pim_re.exe` (reconfigure,
renamed from `reconfigure.exe`), and `pim_rl.exe` (renew-license, matching the
`argv` layout parsed in `pimFrictionlessTrialRun`)).

**Conclusion:** the real `WinMain`/CLI-argument-parsing entry points live in small
external `.exe` launcher projects (`setup.exe`, `pim_rm.exe`, `pim_re.exe`, `pim_rl.exe`)
that load this DLL and call one of the exported `pim*Run` functions. **None of those
launcher projects are included in this archive — UNKNOWN.**

## 5. Executables, Libraries, DLLs

| Component | Type | Evidence |
|---|---|---|
| `pim` module | **DLL** (exported component) | `pim/includes/exp_pim_dll.h` declares `DLLEXPORTAPI` functions; `DLLEXPORTAPI` macro comes from external `<btkdeclspecs.h>` |
| `pim_core`, `pim_ui`, `pim_util` | Almost certainly statically linked into the `pim` DLL | No `exp_*_dll.h`/`imp_*_dll.h` pair exists for these 3 modules (only `pim` has one), and their headers are consumed directly (not through an import-declaration header) by `pim/pim_src/*.cxx` |
| External launcher EXEs (`setup.exe`, `pim_rm.exe`, `pim_re.exe`, `pim_rl.exe`) | Executables | Named only in comments (see §4); source **not present — UNKNOWN** |
| `pim/includes/imp_pim_dll.h` | Import contract | Declares `extern "C" DLLIMPORTAPI` functions the `pim` DLL consumes from *other* PTC-internal DLLs: service control (`ginst_install_service`, `ginst_start_service`, `ginst_stop_service`, `ginst_remove_service`, `ginst_is_service_installed`, `ginst_is_service_running`, `ginst_service_current_state`), string/codepage helpers (`make_strtows`, `make_wscopy`, `make_scopy`), locale (`get_language`, `langLoadExtTbl`, `set_local`, `set_run_mode`), `pro_search_component_set`, `msg_init`, `ptc_exit_run`, `pro_get_displaydatecode`, `msgID_sput_buffer`, and `authorize_main` (from a comment: "proe/uitools/httpdlg"). **The DLL(s) providing these symbols are not in this archive — UNKNOWN which binaries.** |

## 6. Third-Party / External Dependencies

Identified purely from `#include` directives and API usage (none of these are bundled):

| Dependency | Evidence | Role |
|---|---|---|
| **`btk` toolkit** (PTC-internal base class library) | Pervasive: `btkString`, `btkFSEntry`, `btkFSList`, `btkWString`, `thrThread`, `btkFileStream`, `btkObject`, `btkXArray`/`btkClassXArray`, `btkdeclspecs.h`, etc., in nearly every file | Foundational string/filesystem/threading/collections layer for the whole codebase. Source not included except `btkunzip.h`/`.cxx` (supplied separately this session) |
| **libzip** (via `btk`) | `pim_core`/uploaded `btkunzip.cxx`: `#include <zip.h>`, `zip_open`, `zip_stat_index`, `zip_fopen_index`, comment `"$$16 Migration from InfoZip to libZip"` | Real ZIP archive reading, wrapped by `btkUnzip` |
| **Xerces-C++** | `pim_core/pim_core_src/pimXmlFile.cxx`, `pimEntitlement.cxx`, `pim/pim_src/pimTop.cxx`: `DOMNode`, `DOMNodeList`, `DOMElement`, `DOMNamedNodeMap`, `XMLString::compareString`; commit comment in `pimCopyLoop.cxx` "$$19 Xerces crash and lint warnings fix" | DOM-based parsing of all product/entitlement XML |
| **Windows Installer (MSI)** | `pim_util/includes/pimMSI.h`, `pim_util/pim_util_src/pimMSI.cxx`, `pim_core/includes/pimMSILoop.h`, `pimMSICopier.h` | Installs/queries `.msi` packages |
| **Windows Cabinet SDK (FDI)** | `pim_util/includes/pimFdi32.h`, `pim_util/includes/pimCab.h`, `pim_util/pim_util_src/pimCab.cxx` | Legacy `.cab` extraction (superseded per this session's separate change to `pimCopyLoop.cxx`'s `pimInstallCab`/`pimRemoveCab` to use `btkUnzip`/libzip for `.zip` instead) |
| **FlexNet Licensing (Flexera)** | `pim_util/includes/pimFLEXnet.h`, `pim_util/pim_util_src/pimFLEXnet.cxx` | License source discovery/validation (`GetLicenseSourceByIdx`, triad partner logic in `pimTop.cxx`) |
| **Win32 API** | Throughout, notably `pim_core/pim_core_src/pimRegEditLoop.cxx` (`HKEY_*`, `win32RegCreateKeyExA`, etc.), `GetSystemWow64DirectoryA`, `MessageBoxW` | Registry, OS/platform checks |
| Unnamed PTC platform DLL(s) | `pim/includes/imp_pim_dll.h` (§5) | Service control, i18n, exit/shutdown hooks — **UNKNOWN which binary(ies)** |

## 7. Installer Components (Overview — full detail in `docs/04_installation_flow.md`)

The install engine is built around a common base class, `pimLoop` (`pim_core/includes/pimLoop.h`,
`class pimLoop : public thrThread`), giving every install "step" a uniform threaded
lifecycle: `Execute()`, `Cancel()`, `Pause()`/`Resume()`, `Wait()`, `IsDone()`,
`HasErrors()`/`GetErrors()`, `HasWarnings()`/`GetWarnings()`. Concrete step types found in
`pim_core`:

| Loop class | File | Apparent responsibility |
|---|---|---|
| `pimCopyLoop` | `pimCopyLoop.h/.cxx` | File copy + CAB/ZIP archive extraction (`pimInstallCab`/`pimRemoveCab`) |
| `pimMSILoop` | `pimMSILoop.h/.cxx` | MSI package install/uninstall |
| `pimSFXLoop` | `pimSFXLoop.h/.cxx` | Self-extracting `.exe` package install |
| `pimScriptLoop` | `pimScriptLoop.h/.cxx` | Runs install-time scripts |
| `pimShortcutLoop` | `pimShortcutLoop.h/.cxx` | Creates/removes shortcuts (paired with `pimShortcuts.h/.cxx`, `pim_ui`'s `pimShortcutMgr`) |
| `pimRegEditLoop` | `pimRegEditLoop.h/.cxx` | Creates registry keys/values per XML-declared rules |
| `pimServiceLoop` | `pimServiceLoop.h/.cxx` | Installs/starts/stops Windows services (via `ginst_*` imports) |
| `pimPsfLoop` | `pimPsfLoop.h/.cxx` | Creates/removes a "PSF" file (`CreatePsf`/`RemovePsf`/`RemoveLogicalPsf`/`ReadPsfUserData`); exact meaning of "PSF" is **UNKNOWN** — not spelled out in any comment, used alongside `scn`/`email` license-decode fields in `pimFrictionlessTrialRun` |
| `pimDownloadLoop` (+ `pimDownloader`) | `pimDownloadLoop.h/.cxx`, `pimDownloader.h/.cxx` | Web download of installer payloads |
| `pimEntitlement` | `pimEntitlement.h/.cxx` | **Also derives from `pimLoop`** (`class pimEntitlement : public pimLoop`) — represents one installable product and orchestrates its sub-Loops |

`pim_util` supplies lower-level, non-threaded helpers consumed by these loops:
`pimCab`/`pimFdi32` (CAB), `pimMSI` (MSI COM/API wrapper), `pimCopyInstaller`,
`pimRegisterProduct`, `pimFLEXnet`, `pimScramble` (string obfuscation, used for the
trial SCN/email/PSF decoding seen in `pimTop.cxx`), `pimWindows` (OS/platform detection:
`pimIsServer2008`, `pimIsWindows7`, etc.), `pimConvert`.

## 8. What's Missing / Explicitly UNKNOWN

- Build/project files, compiler settings, target list.
- Resource files, icons, version-info resources.
- Any literal `.xml` product/entitlement definition files (schema is inferable from
  parsing code only — see `docs/08_xml_configuration.md`).
- The `btk` toolkit source (beyond `btkunzip.*`).
- The external PTC platform DLL(s) backing `imp_pim_dll.h`.
- The launcher `.exe` projects (`setup.exe`, `pim_rm.exe`, `pim_re.exe`, `pim_rl.exe`).
- Exact DLL/EXE output filenames (only inferred/comment-derived names given above).

> **Re-verification pass (a dedicated pass specifically confirming this section's own
> claims, not a new discovery)**: every bullet above was independently re-checked via
> exhaustive filename search across the entire extracted tree — zero build-system
> files of any kind (`Makefile*`/`*.mk`/`CMakeLists*`/`*.vcxproj*`/`*.sln`/`*.cmake`/
> `SConstruct`/`*.gyp`/`build.xml`/`*.bat`/`*.sh`), zero `.xml` files, and the archive's
> top-level structure is confirmed to be exactly the 4 module folders described in §1
> — no other top-level files or directories exist at all (no `README`, no `.gitignore`,
> no launcher-project folder). **No phase of this documentation set could substitute
> for having these** — every conclusion in this set that touches build/link
> configuration, a real product/translation XML's actual shape, or the launcher EXEs'
> own argument-parsing/entry-point code remains inference from call-site evidence only,
> never confirmed against the real artifact. Two concrete, evidence-backed strengthenings
> did emerge from later phases despite this gap, both cross-referenced in §3 and §2
> above: (1) the `PIM_TRANSLATE_XML` macro-scoping proof (a dead, compile-breaking typo
> in `pim_core` gated by a macro that only exists in `pim`'s own translation unit) is
> the first mechanism-level confirmation that the 4 modules truly compile separately,
> not just a naming-convention inference; (2) `pim.msg.LOCAL` was confirmed (via direct
> `diff`) to be a stale, unreferenced older snapshot of the `usascii` message catalog,
> not a build artifact. Neither finding closes the underlying UNKNOWN — the real build
> system, a real product/translation XML sample, and the launcher `.exe` source remain
> entirely outside this archive.

---
*Generated from `installmgr.zip` (2026-09-21 upload). No source was modified. Phase 1 of
the requested 20-phase documentation set — see chat for sequencing. Re-verified, with 2
cross-references to later-phase findings added, in a dedicated later pass ("build
system/sample XML at the same depth").*
