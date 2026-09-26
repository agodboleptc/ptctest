# Class: `pimCopyLoop`

**File:** `pim_core/includes/pimCopyLoop.h` (67 lines) / `pim_core/pim_core_src/pimCopyLoop.cxx` (663 lines)
**Module:** `pim_core`
**Inherits:** `pimLoop`

## Purpose

The install-step class responsible for copying files and extracting archives
(historically `.cab`, now `.zip` — see the "Risk Analysis"/history note below) from a
product's source media/cache into its install destination, and for the corresponding
removal/rollback of those files on uninstall or failed install.

## Responsibilities

- Copy/extract a product's file payload from `Source` to `Dest` (`OnInstall`).
- Extract archive contents specifically via `pimInstallCab` (despite the name, as of
  this session's edit this method now extracts `.zip` archives via `btkUnzip`, not
  `.cab` via the Windows Cabinet SDK — see `docs/classes/pimCopyLoop.md`'s "History"
  section below and `pim_util/includes/pimCab.h` for the legacy CAB-specific API this
  replaced at this call site).
- Remove previously-extracted files during uninstall/rollback (`pimRemoveCab`,
  `OnRollback`, `OnRollbackCabs`).
- Remove an entire loadpoint directory tree (`OnRemoveLoadpoint`) — used for the
  "Common Files" removal hack noted in the file's revision history
  ("$$15 Hack to remove Common Files in Creo Hybrid").
- Track installed-size accounting (`installed_size`, `expected_installed_size`,
  exposed via `pimGetInstalledSize`/`pimGetExpectedInstalledSize`) using
  `pimMinimumDiskUsage` (from `pim_util`) to convert raw archive-entry sizes into
  on-disk cluster-rounded size estimates.
- Support cooperative cancellation mid-copy via an override of `Cancel()` that also
  calls the global `pimCancelCopy()` (from `pim_util/includes/pimCab.h` — a
  process-wide "please stop the current copy operation" signal, presumably checked by
  whatever low-level copy loop `OnInstall` runs).

## Dependencies

- `pimLoop` (base class).
- `pimXmlFile` (constructor argument, inherited `xmlPtr`).
- `pim_util`'s `pimCab.h` API: `pimCancelCopy()`, `pimMinimumDiskUsage(int)` (both
  still used), and (pre-this-session) `pimCopyCabinetArch`/`pimListCabinetContent`
  (no longer called from this file, but still declared in `pimCab.h` and still
  included via `#include <pimCab.h>`).
- `btkUnzip`/`btkUnzipFilelist` (`<btkunzip.h>`, external `btk`/`libzip`-backed) — the
  actual archive-reading implementation used by `pimInstallCab`/`pimRemoveCab` as of
  this session's edit.
- `pimConvert` (`SetupConverter`/`Converter.Convert(str)` pattern for resolving
  XML-declared paths).
- `pimEntitlement` header's `pimurl`/`pimCDSECTION`/etc. tag constants (used
  elsewhere in the file for XML traversal — not fully re-verified this pass).

## Members

| Member | Type | Access | Purpose |
|---|---|---|---|
| `do_rollback`, `do_uninstall` | `int` | private | Mode flags selecting which branch `OnExecute` takes |
| `Source` | `btkString` | protected | Source archive/file path |
| `Dest` | `btkFSEntry` | protected | Destination directory |
| `installed_size`, `expected_installed_size` | `double` | protected | Running size accounting |
| `ignore_already_installed` | `bool` | protected | Skip-if-present flag |

## Public APIs

| Method | Purpose |
|---|---|
| `pimCopyLoop(pimXmlFile*)` | Ctor; initializes `do_rollback`/`ignore_already_installed` to false |
| `Cancel()` (override) | Calls `pimLoop::Cancel()` then the global `pimCancelCopy()` |
| `SetUninstall()` | Sets `do_uninstall = true` — comment: "content that has attrib `installed=Y` will be removed" |
| `SetRollback()` | Sets `do_rollback = true` |
| `SetIgnoreAlreadyInstalled()` | Sets `ignore_already_installed = true` |
| `pimSetCopySource(btkString&)` / `pimSetDestPath(btkFSEntry&)` | Configure `Source`/`Dest` before `Execute()` |
| `pimGetInstalledSize()` / `pimGetExpectedInstalledSize()` | Read back size accounting after completion |

## Protected APIs

| Method | Purpose |
|---|---|
| `OnExecute()` | Thread entry point; dispatches to `OnInstall`/`OnRollback`/`OnRemoveLoadpoint` based on flags (exact dispatch logic not re-verified line-by-line this pass, inferred from the flag/method set) |
| `OnInstall()` | Main install path: iterates the XML's `<CDSECTION>` content, resolves each entry's source/destination path via `pimConvert`, and for archive entries calls `pimInstallCab` |
| `OnRollback()` | Uninstall/rollback path, calls `pimRemoveCab` per previously-installed entry |
| `OnRollbackCabs()` | Comment: "this can't work unless the cabs are present. future use for rollback if cancel mid install; cannot use for uninstall process" — a narrower, currently-limited-applicability rollback helper |
| `OnRemoveLoadpoint()` | Deletes an entire directory tree (the "Common Files" removal hack) |
| `OnTerminate()` | Thread teardown; releases the logger via `btkLogManager::GetInstance()->GetLogger("LogService")->Release()`, sets `mDone = true` under `Mutex`, calls `ClearInProgress()` |
| `pimInstallCab(src, dest, cab_installed_size&, errors&)` | **Edited this session.** Checks `source.GetExt().ToLower() == "zip"`; on match, uses `btkUnzip::SetArchiveName` + `GetFileList` (for per-entry size accounting via `pimMinimumDiskUsage`) + `UnzipAll` to extract; appends `"Zip extraction failed."`/`"Unexpected archive format."` to `errors` on failure |
| `pimRemoveCab(src, dest, errors&)` | **Edited this session.** Same `.zip` check; uses `btkUnzip::GetFileList` (paths relative to the archive) joined with `dest` (`destination / extracted_files[i].Name`) to locate and `Erase()` each previously-extracted file |

## Private Utilities

None beyond the private data members — no private methods declared.

## Called By

- `pimEntitlement::StartCopy()`/`WaitForCopyToComplete()` — the wrapper class
  `pimCopier` (see `pim_core/includes/pimCopier.h`: `pimCopyLoop *CopyLoop;`) owns and
  drives an instance of this class as part of `pimEntitlement`'s install pipeline (see
  `docs/classes/pimEntitlement.md`).

## Calls Into

`btkUnzip` (archive reading, post-edit), `pimConvert` (path resolution), `pimMessage`/
`LG_INFO`/`LG_ERROR`/`LG_DEBUG` (logging, e.g. `pimLogRemoveDirectory`,
`pimLogRemoveFile`, `pimLogRemoveDirectoryError`, `pimLogRemoveFileError`,
`pimLogExtractCab`), `pimMinimumDiskUsage`/`pimCancelCopy` (`pim_util`), `btkFSEntry`/
`btkFSList` (filesystem enumeration/deletion).

## Lifetime

Created on demand by `pimCopier` (`CopyLoop = XNew pimCopyLoop(xmlPtr);`, by the
established owner-wrapper pattern — see `docs/02_architecture_overview.md` §5), lives
for the duration of one copy/extract/rollback operation, then presumably persists on
the `pimCopier` instance for the entitlement's lifetime (not independently verified
this pass).

## Ownership Model

Owned exclusively by its `pimCopier` wrapper; does not own `xmlPtr` (borrowed from
the owning `pimEntitlement`).

**Correction (added after a deeper trace of `pimCopier.cxx` while documenting the
remaining Loop subclasses):** `pimCopier` (`pim_core/includes/pimCopier.h`) is
itself a **process-wide singleton**, not a per-entitlement object as this
document originally implied and as `docs/02_architecture_overview.md` §5
originally stated for all 6 owner-wrapper classes. Confirmed directly from source:

```cxx
// pim_core/pim_core_src/pimCopier.cxx
pimCopier *pimCopier::OnlyCopier = NULL;
pimCopier *pimCopier::OnlyUninstallCopier = NULL;

pimCopier& pimCopier::GetInstance()
{
    if (OnlyCopier == NULL)
        OnlyCopier = XNew pimCopier();
    return (*OnlyCopier);
}
```

There are in fact **two** singleton instances — `OnlyCopier` (install/forward
copies, via `GetInstance()`) and `OnlyUninstallCopier` (uninstall/rollback copies,
via `GetUninstallInstance()`) — so install-copy and uninstall-copy operations each
get their own independent serialization domain. Within either domain,
`Copy_low()` gates entry with `xmlMutex.TryLock()`: if another copy using that
same singleton is already in progress, the call returns `false` immediately (the
header's own comment: "return 0 if you should try again after a sleep"). **At
most one install-copy and one uninstall-copy can be in flight across the entire
process at any moment, regardless of how many entitlements are installing
concurrently.** This same pattern (singleton + `TryLock` + `1`/`0`/negative
return contract) was subsequently confirmed to hold for all 7 owner-wrapper
classes: `pimCopier`, `pimMSICopier`, `pimSFXCopier`, `pimShortcuts`,
`pimServices`, `pimDownloader`, and `pimRegEdit` (the last already corrected in
`docs/04_installation_flow.md` §7). See `docs/classes/pimSFXLoop.md`,
`pimScriptLoop.md`, `pimShortcutLoop.md`, `pimServiceLoop.md`, `pimPsfLoop.md`,
and `pimDownloadLoop.md` for the sibling Loop types' Ownership Model sections,
and `ai-context/ownership.yaml` for the corrected machine-readable graph.

## Thread Safety

Inherits `pimLoop`'s mutex-guarded lifecycle flags. `OnTerminate()` explicitly manages
a per-thread logger handle (`GetLoggerManager()`/`Release()`), consistent with each
Loop instance running on its own OS thread and needing its own logger-area
registration (see `docs/03_application_startup.md` §5 for the generic pattern).

## Extension Points

- Additional archive formats: extend the `if (source.GetExt().ToLower() == "zip")`
  check in `pimInstallCab`/`pimRemoveCab` with further `else if` branches (this is
  exactly the change point used to swap CAB→ZIP this session; the same seam would be
  used to add e.g. `.7z` support).

## Risk Analysis

- **Naming now mismatches behavior**: `pimInstallCab`/`pimRemoveCab`, `#include
  <pimCab.h>`, and the class's own historical purpose all reference "Cab", but the
  actual archive format handled at these two call sites is now `.zip`. This is a
  legitimate maintainability risk introduced by this session's own change — a future
  reader will reasonably expect CAB behavior from the name and find ZIP behavior in
  the body. Consider a follow-up rename pass (not done here, as the task this session
  was scoped narrowly to swapping the extraction mechanism, not renaming the public
  surface).
- `#include <pimCab.h>` is now used only for `pimCancelCopy`/`pimMinimumDiskUsage` in
  this file — the two archive-specific functions it declares
  (`pimCopyCabinetArch`/`pimListCabinetContent`) are no longer called here, but the
  header (and by extension the legacy CAB implementation in `pim_util`) remains
  otherwise live and unreferenced from this file. See `docs/modules/pim_util.md`
  Risks for the broader CAB-vs-ZIP reconciliation question.
- `OnRollbackCabs`'s own comment admits limited applicability ("can't work unless the
  cabs are present... cannot use for uninstall process") — a pre-existing, self-
  documented limitation, not introduced by this session's change, but relevant to
  anyone extending rollback behavior.

## Usage Example (as evidenced by call sites)

```cxx
// pattern inside pimEntitlement's install pipeline (pimCopier owns CopyLoop)
CopyLoop->pimSetCopySource(sourceArchivePath);
CopyLoop->pimSetDestPath(destDir);
CopyLoop->Execute();          // spawns thread -> OnExecute -> OnInstall -> pimInstallCab
CopyLoop->Wait();
if (CopyLoop->HasErrors()) { btkString errs; CopyLoop->GetErrors(errs); }
double installedKB = CopyLoop->pimGetInstalledSize();
```

---
*Phase 5 of the requested 20-phase documentation set.*
