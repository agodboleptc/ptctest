# Class: `pimScriptLoop`

**File:** `pim_core/includes/pimScriptLoop.h` (41 lines) / `pim_core/pim_core_src/pimScriptLoop.cxx` (455 lines)
**Module:** `pim_core`
**Inherits:** `pimLoop`

## Purpose

Install-step class that generates (and, on rollback, removes) small script files
on disk from a template-and-include mechanism, substituting property placeholders
(license source, temp directories, etc.) into the output.

## Responsibilities

- Iterate `<SCRIPT>` elements in the entitlement's XML and, for each, locate a
  named script definition in a **separate template XML document** (`CreateScript`).
- Compose a script's final content from that template's `<INCLUDE name="...">`
  references (resolved against sibling `<INCLUDE>` definitions elsewhere in the
  same template document) plus a literal `<BODY>` block, concatenated in document
  order.
- Resolve property placeholders in both the script content and its destination
  path via `pimConvert` before writing.
- Inject licensing-related properties used by scripts: `[LM_LICENSE_FILE]` (from
  either the "Simulate" license-source list or the command manager's license IDs),
  `[DCAD_TEMP]`, `[PARAMETRIC]`, `[SIMULATE]` (the latter two resolved from
  `<PSF id="parametric">`/`<PSF id="simulate">` nodes' `name` attribute).
- Filter which `<SCRIPT>` nodes apply based on: the parent `<PACKAGE>`'s
  `install` attribute (skip if `"N"`), and a `platform` attribute matched against
  the entitlement's actual install-platform list (`pimPlatformMgr`).
- On rollback, remove only scripts marked `installed="Y"`, with one explicit
  exception: a script named `"silentGroupUninstall"` is never removed by this
  class (skipped unconditionally in `OnRollback`).

## Dependencies

- `pimLoop` (base class).
- `pimConvert` / `SetupConverter` (property resolution).
- `pimPlatformMgr` (platform-name filtering).
- `pimCommandMgr` (license-ID lookup for `[LM_LICENSE_FILE]`, and simulate-license
  initialization).
- `pimSessionInfo` (`pimGetSessionInfo()->GetLicenseSourceByIdx` for the "Simulate"
  license path).
- `LocateXMLFile` (`pim_core`'s `pimLocate.cxx`, not independently re-read this
  pass) — resolves the template XML file by media ID, falling back to
  `[LP]/bin/pim/xml` if not found via the primary lookup.

## Members

| Member | Type | Access | Purpose |
|---|---|---|---|
| `do_rollback` | `int` | private | Selects install vs. rollback behavior in `OnExecute` |

## Public APIs

| Method | Purpose |
|---|---|
| `pimScriptLoop(pimXmlFile*)` | Ctor; `do_rollback = false` |
| `SetRollback(bool)` | Marks this Loop to run `OnRollback()` instead of `OnInstall()` when executed |

## Protected APIs

| Method | Purpose |
|---|---|
| `OnExecute()` | Dispatches to `OnInstall()` or `OnRollback()` based on `do_rollback` |
| `OnInstall()` | For each eligible `<SCRIPT>` node: resolves/loads the referenced template document (cached across iterations while the `source` attribute is unchanged — a new template is only loaded when `source` changes), calls `CreateScript`, and on success sets `installed="Y"` |
| `OnRollback()` | For each `<SCRIPT installed="Y">` node (excluding `"silentGroupUninstall"` by name): calls `RemoveScript` and clears the `installed` attribute on success |
| `OnTerminate()` | Standard thread teardown |
| `CreateScript(name, dest, templateXML, Converter)` | Finds the `<SCRIPT name="...">` node matching `name` inside `templateXML`, concatenates its `<INCLUDE>`/`<BODY>` content, resolves placeholders in both content and `dest`, creates the destination directory if needed, and writes the file |
| `RemoveScript(dest, Converter)` | Resolves `dest`'s placeholders and erases the file if it exists |

## Private Utilities

None beyond the protected methods above.

## Called By

`pimEntitlement::InstallScripts()` — created and driven **transiently** (no
persistent owner-wrapper class), per `docs/classes/pimEntitlement.md` and
`docs/02_architecture_overview.md` §5's original finding (confirmed correct for
this class — `pimScriptLoop` is one of the two genuine exceptions to the
owner-wrapper pattern, the other being `pimPsfLoop`).

## Calls Into

`pimXmlFile` (both the entitlement's own XML and the loaded template XML),
`pimConvert`, `pimCommandMgr`, `pimPlatformMgr`, `pimSessionInfo`, `LocateXMLFile`,
`btkOFileStream`/`btkFileStream` (direct file I/O for script output).

## Lifetime

Created and destroyed per call inside `pimEntitlement::InstallScripts()` — no
persistent instance across multiple install operations.

## Ownership Model

Not owned by a wrapper singleton (unlike `pimCopyLoop`/`pimSFXLoop`/etc.) —
instantiated directly by `pimEntitlement` as a local, per-call object. This means
script generation is **not** subject to the process-wide serialization that
governs copy/MSI/SFX/shortcut/service/download operations; multiple entitlements'
threads can run their own `pimScriptLoop` instances concurrently without
contention on a shared singleton.

## Thread Safety

Inherits `pimLoop`'s mutex-guarded lifecycle flags. No additional synchronization
members of its own.

## Extension Points

- New injected script property: extend the property-injection block in
  `OnInstall()` (currently `[LM_LICENSE_FILE]`, `[DCAD_TEMP]`, `[PARAMETRIC]`,
  `[SIMULATE]`).
- New always-preserved script name (like `"silentGroupUninstall"`): extend the
  name check at the top of `OnRollback()`'s loop body.

## Risk Analysis

- **Special-cased script name (`"silentGroupUninstall"`) is a hardcoded string
  literal** inside `OnRollback()` rather than an XML-declared attribute — renaming
  that script in any product XML without also updating this source would silently
  remove the "never delete" protection.
- The template-caching optimization (only reloading `script_xml_templates` when
  the `source` attribute changes) assumes `<SCRIPT>` nodes referencing the same
  template are contiguous or at least don't interleave with nodes referencing a
  different template in a way that causes thrashing — not a correctness risk, but
  worth knowing if template-loading performance is ever investigated.
- `CreateScript`/`RemoveScript` are structurally near-identical to
  `pimPsfLoop::CreatePsf`/`RemovePsf` (same template/include/body composition
  pattern) — the two classes appear to have evolved in parallel rather than
  sharing a common helper; a bug fix in one's composition logic should be checked
  against the other.

## Usage Example (as evidenced by call sites)

```cxx
// pattern inside pimEntitlement::InstallScripts()
pimScriptLoop *Scripts = XNew pimScriptLoop(xmlPtr);
Scripts->Execute();
Scripts->Wait();
bool ok = !Scripts->HasErrors();
delete Scripts;
```

---
*Extends Phase 5 of the requested 20-phase documentation set (remaining Loop subclasses, done at the same depth as `pimCopyLoop`).*
