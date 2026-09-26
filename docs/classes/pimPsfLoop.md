# Class: `pimPsfLoop`

**File:** `pim_core/includes/pimPsfLoop.h` (44 lines) / `pim_core/pim_core_src/pimPsfLoop.cxx` (538 lines)
**Module:** `pim_core`
**Inherits:** `pimLoop`

## Purpose

Install-step class that generates a product's **PSF file** — a per-product
configuration/launch file that captures license source and feature selections —
plus a `.bat` wrapper that invokes the product executable with that file as an
argument, so the running application can read its licensing configuration at
startup.

**What "PSF" stands for is never spelled out in any comment in this archive**
(flagged as UNKNOWN since Phase 1). This pass confirms its *behavior* in detail
even though the literal acronym expansion remains unconfirmed — see
`docs/06_entitlement_framework.md` and `pim_core/includes/pimEntitlement.h`'s
comment ("call to setup the PSF entries in the entitlement xml with the entered
LicenseSources + found LTR features... the default license setup") for the
license-configuration framing this class implements mechanically.

## Responsibilities

- Generate the PSF's content from a template document using the **identical
  `<INCLUDE>`/`<BODY>` composition mechanism as `pimScriptLoop::CreateScript`**
  (near-line-for-line structural duplication — see Risk Analysis).
- **Preserve user-added customizations across regeneration**: before overwriting
  an existing PSF file, `ReadPsfUserData` scans it for a `"USER - PSF"` marker
  comment and captures every non-empty line after it; that captured text is
  appended back onto the newly-generated file. This directly implements the
  intent behind the revision-history comment "don't delete the old psf."
- Suppress a specific line pattern (`*PTC_SUPPRESS_LMLIC_ENV*`) from the
  newly-generated content when the *previously preserved* user data itself
  contains that marker — i.e., a user opting out of the license-environment-variable
  behavior once continues to suppress it on every subsequent regeneration.
- Write a companion `.bat` file (same base name, `.bat` extension) that invokes
  `[LP]/bin/[EXE]` with the PSF file's own path as its sole argument — this is the
  actual launch mechanism for whatever executable name `[EXE]` resolves to.
- Support two distinct removal modes: `RemovePsf` (deletes both the PSF file and
  its `.bat` wrapper) and `RemoveLogicalPsf` (deletes **only** the `.bat` wrapper,
  deliberately leaving the PSF file itself in place — the code's own comment: "don't
  delete file physically").

## Dependencies

- `pimLoop` (base class).
- `pimConvert` (property resolution, including a `[THISFILE]` placeholder set to
  the destination file's own tail name before content conversion — allowing a PSF
  template to reference its own eventual filename).
- `pimXmlFile` (both the entitlement's XML and the loaded template document).

## Members

| Member | Type | Access | Purpose |
|---|---|---|---|
| `do_rollback` | `int` | private | Selects install vs. rollback in `OnExecute` |

## Public APIs

| Method | Purpose |
|---|---|
| `pimPsfLoop(pimXmlFile*)` | Ctor; `do_rollback = false` |
| `SetRollback(bool)` | Marks this Loop for rollback behavior |

## Protected APIs

| Method | Purpose |
|---|---|
| `OnExecute()` | Dispatches to `OnInstall()`/`OnRollback()` |
| `OnInstall()` | Iterates `<PSF>` elements, resolves/loads the referenced template (same caching-by-`source`-attribute pattern as `pimScriptLoop`), calls `CreatePsf` |
| `OnRollback()` | Removes previously-created PSF entries |
| `OnTerminate()` | Standard thread teardown |
| `CreatePsf(name, dest, templateXML, Converter)` | Composes content from `<INCLUDE>`/`<BODY>` (identical algorithm to `pimScriptLoop::CreateScript`), reads and re-appends preserved user data via `ReadPsfUserData`, applies the `PTC_SUPPRESS_LMLIC_ENV` line-suppression rule, writes the PSF file, then writes the `.bat` wrapper invoking `[LP]/bin/[EXE] "<psf-file>" %*` |
| `RemovePsf(dest, Converter)` | Deletes both the PSF file and its `.bat` wrapper |
| `RemoveLogicalPsf(dest, Converter)` | Deletes only the `.bat` wrapper; the PSF file itself is intentionally left on disk |
| `ReadPsfUserData(file, out)` | Scans an existing PSF file for the `"USER - PSF"` marker line and returns every non-empty line found after it (skipping a fixed instructional comment line: `"// Add User specific environment or run applications below here*"`) |

## Private Utilities

None beyond the protected helpers above.

## Called By

`pimEntitlement::InstallPSF()` — created and driven **transiently**, exactly like
`pimScriptLoop` (no persistent owner-wrapper class — confirmed as one of the two
genuine exceptions to the singleton-owner pattern found across the other 7 Loop
types).

## Calls Into

`pimXmlFile`, `pimConvert`, `btkFileStream`/`btkOFileStream`/`btkIFileStream`
(direct file I/O for both reading prior user data and writing the new PSF/`.bat`
pair).

## Lifetime

Created and destroyed per call inside `pimEntitlement::InstallPSF()`.

## Ownership Model

Not owned by a singleton wrapper — instantiated directly by `pimEntitlement` per
call, the same pattern as `pimScriptLoop`. No process-wide serialization applies
to PSF generation.

## Thread Safety

Inherits `pimLoop`'s mutex-guarded lifecycle flags. No additional synchronization
of its own.

## Extension Points

- New user-data preservation marker or suppression pattern: extend
  `ReadPsfUserData`'s matching logic and/or `CreatePsf`'s suppression check.
- New removal semantics beyond "physical" vs. "logical": add a third `RemoveXxxPsf`
  variant following the existing two-method pattern.

## Risk Analysis

- **Near-duplicate logic with `pimScriptLoop::CreateScript`** — the `<INCLUDE>`/
  `<BODY>` composition algorithm is essentially copy-pasted between the two
  classes. A correctness fix or behavior change made in one (e.g., how include
  resolution handles a missing `<INCLUDE>` target) should be checked against the
  other, since there is no shared helper enforcing consistency.
- **User-data preservation depends on an exact string marker** (`"USER - PSF"`)
  in the existing file — if a user's manual edit accidentally alters or removes
  that marker line, their custom additions will silently fail to be preserved on
  the next regeneration, with no error surfaced (the function just returns
  `false`/empty).
- **`RemoveLogicalPsf` intentionally leaves a file on disk** — a general-purpose
  "clean up everything this class created" assumption (e.g. in an uninstaller
  audit) would incorrectly conclude the PSF file was removed; anyone reasoning
  about complete uninstall cleanliness needs to know this distinction exists.
- **"PSF" acronym remains unresolved** — while this pass strongly characterizes
  its *function* (license-configuration launch file + batch wrapper), no source
  comment in this archive spells out the literal expansion. Do not assert a
  specific expansion (e.g. "Parametric Setup File") as fact without independent
  confirmation from outside this archive.

## Usage Example (as evidenced by call sites)

```cxx
// pattern inside pimEntitlement::InstallPSF()
pimPsfLoop *PsfFiles = XNew pimPsfLoop(xmlPtr);
PsfFiles->Execute();
PsfFiles->Wait();
bool ok = !PsfFiles->HasErrors();
delete PsfFiles;
```

---
*Extends Phase 5 of the requested 20-phase documentation set (remaining Loop subclasses, done at the same depth as `pimCopyLoop`).*
