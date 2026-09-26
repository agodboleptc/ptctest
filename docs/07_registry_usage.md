# PIM Registry Usage

> Traced from `pim_core/pim_core_src/pimRegEditLoop.cxx` (742 lines),
> `pim_core/includes/pimRegEdit.h` (singleton wrapper), `pim_util/pim_util_src/`
> `pimRegisterProduct.cxx` (678 lines), and the registry key literals found in
> `pim_core/pim_core_src/pimPrerequisite.cxx` (Phase 8). No source was modified.
>
> **`pimRegEditLoop` and `pimRegEdit` now each have a dedicated full-depth class
> doc** (`docs/classes/pimRegEditLoop.md`, `docs/classes/pimRegEdit.md`), added in
> a later extension pass with a full member/API/risk breakdown matching the other
> 8 Loop subclasses and 6 sibling wrapper singletons. Most importantly, that pass
> found a **confirmed bug in `OnRollback()`**: a double-incremented loop index
> silently skips every other `<REGISTRY>` entry on every pass, so uninstall/
> rollback can leave roughly half a product's registry entries behind. This is
> now the single highest-confidence, highest-impact finding across the entire
> documentation effort — see `docs/classes/pimRegEditLoop.md`'s Risk Analysis and
> `ai-context/business_rules.yaml`'s `pimregeditloop_rollback_skips_half_of_entries`
> before relying on this document's description of `OnRollback()` as fully
> correct.

## 1. Two Distinct Registry Mechanisms

PIM writes to the registry through **two structurally different paths** that should
not be conflated:

1. **Generic, XML-declared key/value creation** (`pimRegEditLoop`, driven by
   `<REGISTRY>` elements in a product's own definition XML) — the mechanism a product
   author uses to request arbitrary registry entries as part of its install.
2. **Hardcoded, PIM-internal bookkeeping** (`pim_util`'s `pimRegisterProduct.cxx`,
   plus a few scattered hardcoded paths elsewhere) — Add/Remove Programs entries, PTC's
   own install-tracking records, shared-utility reference counting, and Creo file-type
   registration. These paths are compiled into PIM itself, not driven by product XML.

Both ultimately go through the same low-level `win32Reg*` wrapper calls (external,
presumably from `<ptc_win32.h>`, declared reachable via `pim/includes/imp_pim_dll.h`).

## 2. Mechanism 1: XML-Declared Registry Entries (`pimRegEditLoop`)

### 2.1 XML Schema (element: `<REGISTRY>`, from `pimDefs.h`'s `pimREGISTRY` constant)

`pimRegEditLoop::OnInstall()` (and its rollback counterpart) enumerates
`getElementsByTagName(pimREGISTRY)` on the entitlement's XML. For each `<REGISTRY>`
node, the following are read (attribute constant names from `pimDefs.h`):

| Attribute/content | Constant | Purpose |
|---|---|---|
| Key root | `Key` parameter to `Create()`, one of the literal tokens `[HKCR]`, `[HKLM]`, `[HKCU]`, `[HKU]` | Selects `HKEY_CLASSES_ROOT`/`HKEY_LOCAL_MACHINE`/`HKEY_CURRENT_USER`/`HKEY_USERS` |
| `id` | `pimid` | The subkey path under the chosen root (property-placeholder-expanded via `pimConvert`, e.g. `[LP]` substitution) |
| `valuename` | `pimvaluename` | Name of the value to set; empty means "the key's default value" |
| default text content | — | The value to write |
| `valuetype` | `pimvaluetype` | Registry value type — added in revision `$$5` (Q-12-02); when `REG_DWORD`, the text content is parsed as an integer first |
| platform | `plat` parameter | If non-empty, registers a `[THIS_ARCH]` substitution before conversion, letting a value differ per install platform |

### 2.2 Validation Logic

`pimRegEditLoop::Create(...)` (`pim_core/pim_core_src/pimRegEditLoop.cxx:351`):

1. Maps the `[HKxx]` token to a real `HKEY` root; **returns `false` immediately for
   any unrecognized token** — there is no default root and no error message beyond the
   boolean return.
2. Resolves property placeholders in the subkey path via the supplied `pimConvert`.
3. `win32RegCreateKeyExA(root, id, ..., KEY_SET_VALUE, ...)` — creates or opens the
   key; distinguishes `REG_CREATED_NEW_KEY` vs `REG_OPENED_EXISTING_KEY` via the
   `created`/`existed` out-parameters (used for logging/rollback bookkeeping, not for
   altering behavior).
4. If a value is supplied, `win32RegSetValueExA(...)` writes it, with `REG_DWORD`
   values converted from the text representation via `Input.ToInt(...)`.
5. **Every successful key/value write is also appended to a `pimRegFileCreate`
   buffer** (`RegFileCreate.AppendKey(...)` / `Append_REG_SZ(...)`) — see §4.

### 2.3 Failure Logic

`Create()` returns `false` if the root token is unrecognized or if
`win32RegCreateKeyExA` fails (permissions, invalid path, etc.) — the caller
(`pimEntitlement::ApplyRegistryChanges()`, `docs/04_installation_flow.md` §5) treats
any such failure as fatal to the current install step, aborting the pipeline.

### 2.4 Recovery Logic (Rollback)

Rollback of XML-declared registry entries goes through the **`pimRegEdit` singleton**
(`pimRegEdit::GetInstance().Uninstall(xmlPtr)`), not a symmetric per-entry `Remove()`
call site directly comparable to `Create()` — `pimRegEditLoop::Remove(Key, Id,
ValueName)` exists as a protected method but its call pattern during rollback was not
individually re-traced in this pass (see `docs/04_installation_flow.md` §6 for the
overall `OnRollback()` sequence, and §7 there for the singleton correction).

### 2.5 File-Extension / ProgID Registration (a specialized use of the same class)

`pimRegEditLoop::CreateNumericExtensions`/`RemoveNumericExtensions` and
`GetHighestCreoFileVer`/`SetHighestCreoFileVer` implement Creo's classic
versioned-file-extension registration pattern (e.g. registering `.prt`, `.prt.1`,
`.prt.2`, … against a `creoFile`-style ProgID under `HKEY_CLASSES_ROOT`, evidenced by
the literal call `Create(DotRegFile, "[HKLM]", str, "", "creoFile", NULL, ...)` at
line 564 and the "highest version" bookkeeping below).

| Registry Path | Value | Purpose | R/W | Consumers |
|---|---|---|---|---|
| `HKEY_CURRENT_USER\Software\PTC` | `creoFileVers` (`REG_DWORD`) | Tracks the highest numeric file-extension index PIM has registered for this user, so subsequent installs/uninstalls know how many numbered extensions to manage | Read: `GetHighestCreoFileVer`; Write: `SetHighestCreoFileVer` | `pimRegEditLoop`'s numeric-extension registration logic |

## 3. Mechanism 2: PIM-Internal Bookkeeping (`pim_util/pimRegisterProduct.cxx`)

All paths below are **hardcoded string literals** in `pimRegisterProduct.cxx`, not
driven by product XML. Called directly from `pimEntitlement.cxx`'s install/uninstall
pipeline (`docs/04_installation_flow.md`).

| Registry Path | Value(s) | Purpose | R/W | Validation Logic | Consumers |
|---|---|---|---|---|---|
| `HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\<UninstallName>` | `DisplayName`, `UninstallString`, `DisplayIcon`, `URLUpdateInfo`, `URLInfoAbout`, `DisplayVersion`/`VersionMajor`/`VersionMinor`, `InstallLocation`, PTC contact-info fields (`pimAddStandardPTCContactInfo`), no-modify/no-repair flags | The standard Windows Add/Remove Programs entry for an installed product | Write: `pimInitializeUninstall`+`pimWriteLocation`/`pimWriteVersion`/`pimAppendValue`/`pimWriteNoModifyNoRepair`; Read: `pimOpenUninstallKey`/`pimReadRegValue`; Delete: `pimRemoveUninstallRegKey` | None beyond standard key creation success/failure | `pimEntitlement::OnInstall()` (creation, `docs/04_installation_flow.md` §2), `pimEntitlement::OnInstall()`'s upgrade-cleanup block (removal of the *old* version's entry, §3 there) |
| `HKLM\SOFTWARE\PTC\<Name>[\<Release>[\<Datecode>]]` | Install-time record fields (exact value set not enumerated in this pass) | An internal "PTC install record" distinct from the Windows uninstall entry — used for PIM's own bookkeeping of what's installed, walked recursively (`Name` → `Release` → base `SOFTWARE\PTC`) on removal, deleting each level if it becomes empty | Write: `pimInitializePTCRecord`; Read: `pimOpenPTCRecord`; Delete: `pimRemovePTCRecord` (recursively prunes empty parent keys up to and including `SOFTWARE\PTC` itself if nothing else remains) | None beyond key-existence checks during the recursive delete | `pimEntitlement.cxx`'s upgrade pre-cleanup (`pimRemovePTCRecord`, `docs/04_installation_flow.md` §3) |
| `HKLM\SOFTWARE\PTC\PlatformUseLoadpoints\` | One value per loadpoint referencing a shared "Platform" utility install | Reference-counts which install loadpoints depend on a shared platform-services utility, so it isn't removed while still in use by another product | Read/Write/Enumerate via `win32RegCreateKeyExA`/`win32RegOpenKeyExA`/`win32RegEnumValueA` | Presence/absence of a loadpoint's own value entry | `pimUpdateUtilityAppList`/`pimWriteUUIDForUtility`/`pimClearUtilityLoadPoint` family (referenced from `pim/pim_src/pimTop.cxx`'s shutdown sequence, `docs/03_application_startup.md` §3) |
| `HKLM\SOFTWARE\PTC\QAgentUseLoadpoints\` | Same shape as above, for Quality Agent | Same reference-counting pattern, specifically for the shared Quality Agent utility | Same as above | Same utility-list functions, `PIM_ADD_FOR_QAGENT` mode |
| `HKLM\SOFTWARE\PTC\PTC Windchill Workgroup Manager\` | Per-subkey `InstallLocation` | Read-only detection of existing WGM client installs (enumerates subkeys, reads `InstallLocation` via `RegGetValueA`) | Read-only | None (pure discovery) | `pimCheckForWGMInstalls` |
| `HKLM\SOFTWARE\PTC` (base, enumerated) | Subkey enumeration across all PTC products | General "what PTC software is on this machine" utility scan | Read-only (enumeration) | None | `pimGetUtilityList`, `pimGetProductsInstalled` |

## 4. Cross-Cutting: The `.reg` File Mirror (`pimRegFileCreate`)

`pim_core/includes/pimRegFileCreate.h` documents its own purpose directly in a header
comment: *"This utility class is used to generate a Windows .reg file that will do the
same registry changes that PIM does via Windows API functions... This allows admins to
use this file when copying an existing install to another host."* Every key/value
`pimRegEditLoop::Create()` successfully writes is mirrored into this buffer
(`AppendKey`/`Append_REG_SZ`), which is later written out via `WriteFile()`/
`WriteCombinedRegFile()`. This means **every XML-declared registry change (Mechanism
1) has a human-auditable, admin-reusable textual record** — the PIM-internal
bookkeeping paths (Mechanism 2) do **not** appear to go through this mirror (no
`pimRegFileCreate` usage was found in `pimRegisterProduct.cxx` in this pass).

## 5. Prerequisite-Check Registry Reads (cross-reference, Phase 8)

Two registry locations are read (never written) purely to answer "is this prerequisite
satisfied," documented fully in `docs/05_prerequisite_framework.md` §4:

| Path | Values read | Purpose |
|---|---|---|
| `HKLM\SOFTWARE\PTC\CreoAgent` | `AgentPackageVersion`, `PLATFORM_PACKAGE_VERSION_REG`-named value | Version-gate for the `pimCreoTestPlatformAgent` prerequisite rule |
| `HKLM\SOFTWARE\PTC\Quality Agent\Agent` | Executable path | `pimGetQAgentPathFromReg`, used for Quality Agent cleanup (PHM), not prerequisite gating itself |

## 6. Related Features / Consumers Summary

| Feature | Registry mechanism used |
|---|---|
| Add/Remove Programs listing | Mechanism 2 (Uninstall key) |
| Product-declared custom registry settings | Mechanism 1 (`<REGISTRY>` in product XML) |
| Creo file-type (ProgID) associations | Mechanism 1, specialized (`CreateNumericExtensions`) |
| Shared Platform/Quality-Agent utility lifecycle | Mechanism 2 (LoadPoints keys) |
| WGM install detection | Mechanism 2 (read-only) |
| Prerequisite satisfaction (Creo Agent, WGM VFS) | Read-only checks, `pimPrerequisite.cxx` (Phase 8) + `pimMSI`'s upgrade-code lookup (also registry-backed, under the Windows Installer's own key space — not independently traced in this pass) |
| Admin redeployment via `.reg` file | Mechanism 1's `pimRegFileCreate` mirror only |

## 7. Risk Analysis

- **Two independent registry-writing code paths** (Mechanism 1 vs 2) means a reviewer
  auditing "what does PIM write to the registry" must check both
  `pimRegEditLoop.cxx` (generic, XML-driven — the actual paths are not visible in
  source, only in whatever product XML instantiates them at runtime) and
  `pimRegisterProduct.cxx` (fixed, source-visible paths). The generic mechanism's
  real-world footprint is **entirely data-dependent** and cannot be fully enumerated
  from source alone.
- The recursive-delete-if-empty pattern in `pimRemovePTCRecord` (walking `Name` →
  `Release` → base `SOFTWARE\PTC`) is a classic pattern for accidentally deleting a
  shared parent key if the emptiness check has an edge-case bug — not confirmed as an
  actual bug in this pass, but worth extra scrutiny before modifying.
- `Create()`'s silent `return false` for an unrecognized `[HKxx]` token means a typo
  in a product XML's registry root produces no specific diagnostic beyond a generic
  step failure — consistent with the broader "unstructured error strings" pattern
  noted in `docs/classes/pimLoop.md`.

---
*Phase 10 of the requested 20-phase documentation set. No source was modified.*
