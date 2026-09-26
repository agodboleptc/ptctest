# Module: `pim_util`

> Evidence-based; see `docs/01_repository_inventory.md` §0 for corpus scope.

## Purpose

Lowest-level module: thin wrappers around OS/platform APIs and third-party install
technologies (Windows Cabinet SDK, Windows Installer, FlexNet Licensing), plus
generic utilities (string obfuscation, path conversion, OS-version detection) used by
`pim_core`'s Loop classes and by `pim`'s top-level logic. No file in this module was
observed including a `pim_core` or `pim_ui` header — it sits at the bottom of the
dependency stack (confirmed in `docs/02_architecture_overview.md` §4).

## Responsibilities

- **CAB archive handling**: `pimCab.h/.cxx` + `pimFdi32.h` — wraps the Windows
  Cabinet SDK (FDI). Confirmed exact signatures (previously unknown before this
  archive upload):
  ```cxx
  bool pimCopyCabinetArch(cStringT src, cStringT dest, btkUnzipFilelist &extracted_files, StringXArray &errors);
  bool pimListCabinetContent(cStringT src, cStringT dest, btkUnzipFilelist &extracted_files, StringXArray &errors);
  void pimCancelCopy();
  int  pimMinimumDiskUsage(int filesize);
  int  pimSetClusterSize(const char *path);
  ```
  These are exactly the two functions `pim_core/pim_core_src/pimCopyLoop.cxx`
  originally called from `pimInstallCab`/`pimRemoveCab` before this session's edit
  switched those call sites to `btkUnzip` for `.zip` archives (see the "Zip extraction"
  work earlier in this session) — `pimCab.cxx` itself was **not modified** and still
  provides the legacy `.cab` path if re-enabled.
- **MSI wrapper**: `pimMSI.h/.cxx` — hashing/unhashing MSI product codes
  (`pimHashMSICode`/`pimUnhashMSICode`), MSI DLL lifecycle (`pimInitMSIDLL`,
  `pimOKToRunMsiexec`, `pimFreeMsiexec`), size/version/location queries
  (`pimGetDiskSizeFromMSI`, `pimGetVersionInstalled`, `pimGetInstallLocation`,
  `pimGetUpgradeCodeFromRegistryByProductCode`), raw MSI database access
  (`pimMSIOpenDatabase` returning `MSIHANDLE`).
- **FlexNet Licensing queries**: `pimFLEXnet.h/.cxx` — server/license-file state
  (`pimIsFLEXServerRunning`, `pimIsFLEXServerActivelyBorrowingLicenses`,
  `pimNeedsLocalLicenseServerRenew`/`pimNeedsLocalNodeLicenseRenew`,
  `pimIsLicenseFileExpired`, `pimIsValidTriadLicenseFile`, `pimGetAllCPUid`).
- **Product registration**: `pimRegisterProduct.h/.cxx`.
- **String obfuscation**: `pimScramble.h/.cxx` — `pimRequestorId`, `pimAppId`, and a
  family of URL getters (`pimProductionUrl`, `pimTestUrl`, `pimDevUrl`,
  `pimRenewLicensePRODUrl[SecondHalf]`, `pimRenewLicenseTestUrl[SecondHalf]`,
  `pimSimLivePRODUrl`) whose values are presumably obfuscated in the binary (the
  "scramble" name plus `BTK_UNSCRAMBLE_31_S` seen elsewhere in `pim_core` strongly
  implies a shared string-obfuscation scheme against casual binary inspection).
- **OS/platform detection**: `pimWindows.h/.cxx` — `pimIsServer2008`,
  `pimIsServer2008R2`, `pimIsServer2012`, `pimIsWindows7`, `pimWindowsPathTest`, and
  related helpers used by `pim/pim_src/pimTop.cxx` for compatibility warnings (e.g. the
  WGM-on-incompatible-OS check).
- **Path/encoding conversion**: `pimConvert.h/.cxx` — used throughout `pim_core` to
  resolve XML-declared paths/properties against session state (`SetupConverter`/
  `Converter.Convert(...)` pattern seen in `pimCopyLoop.cxx`, `pimEntitlement.cxx`).
- **Installer copy staging**: `pimCopyInstaller.h/.cxx`.

## Dependencies

- External only: Win32 API, Windows Installer API (`msi.h`/`Msi.dll`), Windows Cabinet
  SDK (`fdi.h`), FlexNet Licensing client library, `btk` toolkit. No dependency on
  `pim`, `pim_core`, or `pim_ui`.

## Consumers

- `pim_core` — `pimCopyLoop` (CAB, pre-this-session), `pimMSILoop`/`pimMSICopier`
  (MSI), various managers consuming `pimFLEXnet`/`pimConvert`.
- `pim` — `pimTop.cxx` calls `pimWindowsPathTest`, FlexNet triad functions, and
  `pimScramble`-based URL/obfuscation getters directly.

## Owned Components

None — this module is stateless/utility-style except for module-local statics inside
individual `.cxx` files (not enumerated in this pass).

## Used Components (not owned)

N/A — bottom of the dependency stack; it only calls out to external OS/vendor APIs.

## Failure Modes

| Failure | Where | Result |
|---|---|---|
| CAB extraction failure | `pimCopyCabinetArch`/`pimListCabinetContent` | Appends to caller-supplied `errors` array (signature confirms this contract); exact internal FDI error handling **not read this pass** |
| MSI database open failure | `pimMSIOpenDatabase` | Returns a null/invalid `MSIHANDLE` (Windows Installer API convention) — caller-side handling **not traced this pass** |
| FlexNet server unreachable | `pimIsFLEXServerRunning`/`pimIsFLEXServerActivelyBorrowingLicenses` | Returns `false`; calling code in `pim_core`/`pim` treats this as "no local server", driving the license-source UI flow seen in `pimTop.cxx` |

## Extension Points

- New archive format support: add a sibling to `pimCab.h`/`pimCab.cxx` (as this
  session's separate work effectively did inline inside `pim_core`'s `pimCopyLoop.cxx`
  via `btkUnzip`, rather than adding a new `pim_util` wrapper — worth reconciling
  which pattern should be canonical going forward).
- New OS-version check: extend `pimWindows.h/.cxx` alongside the existing
  `pimIsServer2008`/`pimIsWindows7`-style functions.

## Risks

- `pimScramble`'s obfuscation is almost certainly **not real security** — likely just
  binary/string-scan resistance (a common practice for hiding license/URL literals in
  installer code, not intended to resist a motivated reverse-engineer with source
  access, which is exactly the position this documentation set is written from).
  Treat any URLs/IDs surfaced through `pimScramble` as sensitive-by-intent even though
  they are not cryptographically protected.
- Because this module wraps FDI/MSI/FlexNet vendor APIs directly, it is the most
  likely place for platform-version-specific breakage (e.g. changes to Windows
  Installer or FlexNet client behavior across OS versions) — consistent with the CAB
  code being the one this session was asked to modernize toward ZIP.

## Key Source Files

- `pim_util/includes/pimCab.h`, `pim_util_src/pimCab.cxx`
- `pim_util/includes/pimMSI.h`, `pim_util_src/pimMSI.cxx`
- `pim_util/includes/pimFLEXnet.h`, `pim_util_src/pimFLEXnet.cxx`
- `pim_util/includes/pimScramble.h`, `pim_util_src/pimScramble.cxx`
- `pim_util/includes/pimWindows.h`, `pim_util_src/pimWindows.cxx`
- `pim_util/includes/pimConvert.h`, `pim_util_src/pimConvert.cxx`

## Important Functions

| Function | File | Role |
|---|---|---|
| `pimCopyCabinetArch` / `pimListCabinetContent` | `pimCab.h/.cxx` | Legacy CAB extract/list (see `pim_core.md` for the ZIP replacement at the call site) |
| `pimInitMSIDLL` / `pimMSIOpenDatabase` | `pimMSI.h/.cxx` | MSI subsystem bring-up and raw database access |
| `pimIsFLEXServerRunning` | `pimFLEXnet.h/.cxx` | Local/remote FlexNet server liveness check |
| `pimWindowsPathTest` | `pimWindows.h/.cxx` | Called unconditionally near the top of every `pim*Run` function in `pim/pim_src/pimTop.cxx` |

---
*Phase 4 of the requested 20-phase documentation set.*
