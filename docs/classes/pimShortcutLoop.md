# Class: `pimShortcutLoop`

**File:** `pim_core/includes/pimShortcutLoop.h` (82 lines) / `pim_core/pim_core_src/pimShortcutLoop.cxx` (1,057 lines — largest of the 9 Loop subclasses after `pimEntitlement` itself)
**Module:** `pim_core`
**Inherits:** `pimLoop`

## Purpose

Install-step class that creates and removes Windows shortcuts (`.lnk` files) at
up to 5 distinct locations — Desktop, Start Menu, Programs Menu, Quick Launch, and
the product's own install loadpoint — per entitlement.

## Responsibilities

- Iterate `<SHORTCUT>` elements in the entitlement's XML; for each, read child
  elements `<NAME>`, `<ICON>`, `<STARTINDIR>`, `<INDEX>`, `<PATH>`, `<ARGS>`,
  `<WIN7APPID>`, and up to 5 location elements (`<DESKTOP>`, `<STARTMENU>`,
  `<PROGRAMSMENU>`, `<QUICKLAUNCH>`, and a loadpoint variant), each independently
  toggleable via a `create` attribute.
- For each location element, support **both directions in the same `OnInstall()`
  pass**: if `create="Y"`, (re)create the shortcut at that location; if
  `create="N"` and the element was previously marked `installed="Y"`, remove it.
  This means `OnInstall()` is also the mechanism for reconfigure-time shortcut
  toggling, not just fresh creation.
- Skip a shortcut entirely if it's tied to a `<PACKAGE>` (via a `package`
  attribute) that is itself not selected for install (`install="N"`).
- Support per-shortcut "all users" placement (`allusers` attribute) vs.
  current-user placement.
- Preserve an existing shortcut's target path/working-directory/arguments if one
  already exists at the destination when recreating (icon/icon-index are *not*
  preserved — always taken from the XML).
- On rollback, remove shortcuts previously marked `installed="Y"`, honoring a
  `NoLPReconfigure` XML property that changes reconfigure-time loadpoint-shortcut
  handling.

## Dependencies

- `pimLoop` (base class).
- `pimConvert` (property resolution for all path/name/argument fields).
- External shell-link primitives: `pimCreateShellLink`/`pimGetShellLink`
  (presumably `btk`-toolkit wrappers around COM `IShellLink`/`IPersistFile` —
  not independently confirmed against `btk` source, which isn't in this archive).
- `pimGetStartMenuPrograms`/`pimGetDocuments` (`pim_util`, path resolution for
  per-user vs. all-users shell folders).

## Members

| Member | Type | Access | Purpose |
|---|---|---|---|
| `do_rollback` | `int` | private | Selects install vs. rollback in `OnExecute` |
| `is_reconfigure` | `int` | private | Alters loadpoint-shortcut handling during reconfigure |
| `StartInDir` | `btkString` | private | Working directory for the shortcut currently being processed; also used as an output/preservation value in `CreateShellLink` |

## Public APIs

| Method | Purpose |
|---|---|
| `pimShortcutLoop(pimXmlFile*)` | Ctor; `do_rollback = false`, `is_reconfigure = false` |
| `SetRollback(bool)` | Marks this Loop for rollback behavior |
| `SetReconfigure(bool)` | Marks this Loop as running in a reconfigure context |

## Protected APIs

| Method | Purpose |
|---|---|
| `OnExecute()` | Dispatches to `OnInstall()` or `OnRollback()` |
| `OnInstall()` | Per `<SHORTCUT>` node: package-eligibility check, read all child fields, then for each of the 5 location types either create (if `create="Y"`) or remove-if-previously-installed (if `create="N"` and `installed="Y"`) |
| `OnRollback()` | Removes shortcuts previously marked `installed="Y"` across all location types |
| `OnTerminate()` | Standard thread teardown |
| `CreateShellLink(LinkPath, CmdPath, WorkDir, IconPath, CmdArgs, IconIdx, Win7AppId)` | Thin wrapper resolving a default working directory (`pimGetDocuments`) when none is supplied, then delegates to the external `pimCreateShellLink` |
| `IsEmptyDirectory(btkFSEntry&)` | Used when deciding whether to remove a now-empty Programs-Menu subfolder after removing its last shortcut |
| `CreateShortcutProgramsmenu` / `RemoveShortcutProgramsmenu` | Builds `<ProgramsMenuFolder>/<Name>.lnk`, sanitizing illegal filename characters (`/ * \| \`) from the name first; preserves target/workdir from an existing link via `pimGetShellLink` before overwriting |
| `CreateShortcutStartmenu` / `RemoveShortcutStartmenu` | Same pattern, Start Menu root (no subfolder) |
| `CreateShortcutDesktop` / `RemoveShortcutDesktop` | Same pattern, Desktop |
| `CreateShortcutQuickLaunch` / `RemoveShortcutQuickLaunch` | Same pattern, Quick Launch (current-user only — no `allusers` parameter on these two, unlike the others) |
| `CreateShortcutLoadPoint` / `RemoveShortcutLoadPoint` | Same pattern, but at an explicit `Loadpoint` path parameter rather than a shell-folder lookup |

## Private Utilities

None beyond the protected helpers above.

## Called By

`pimShortcuts::Create_low()` (see Ownership Model below).

## Calls Into

`pimConvert`, `pimCreateShellLink`/`pimGetShellLink` (external), `pimGetStartMenuPrograms`/`pimGetDocuments` (`pim_util`), `pimXmlFile` (DOM read/write, including `PushReadToWrite`/`PopWriteToRead` transitions inside the install loop since it both reads shortcut definitions and writes back `installed`/`create` attribute changes in the same pass).

## Lifetime

Created on demand by `pimShortcuts` (see Ownership Model), lives for one
create/rollback operation.

## Ownership Model

**Correction relative to this documentation set's earlier assumption**: `pimShortcuts`
(`pim_core/includes/pimShortcuts.h`) is a **process-wide singleton**
(`static pimShortcuts *OnlyShortcuts`, `GetInstance()`), following the identical
structural pattern confirmed directly against `pimCopier.cxx`
(`docs/classes/pimCopyLoop.md`) — not a per-entitlement owned object as originally
documented in `docs/02_architecture_overview.md` §5. Only one shortcut
create/remove/reconfigure-cleanup operation can be in flight process-wide at a
time; the singleton's `Create`/`Uninstall`/`ReconfigureCleanup` methods use the
same `TryLock`-based `1`/`0`/negative return contract as `pimCopier::Copy`.

## Thread Safety

Inherits `pimLoop`'s mutex-guarded lifecycle flags. No additional synchronization
members of its own — relies on the XML document's own `PreRead`/`PreWrite`
locking (`pimXmlFile`) for the DOM mutations interleaved with reads in
`OnInstall()`.

## Extension Points

- New shortcut location type: add a new child-element case in `OnInstall()`'s
  parsing loop and a matching `CreateShortcutXxx`/`RemoveShortcutXxx` pair,
  following the existing 5-location pattern.
- New per-shortcut XML attribute: extend the attribute-reading block at the top
  of the per-node loop in `OnInstall()`.

## Risk Analysis

- **Largest Loop subclass by a wide margin** (1,057 lines) — the single
  `OnInstall()` method interleaves parsing, eligibility filtering, and 5 separate
  create-or-remove branches in one pass; a change to shared parsing logic near the
  top of the loop affects all 5 location types simultaneously.
- **Icon/icon-index are never preserved** across a shortcut recreation, unlike
  target path/working-directory/arguments — an icon change made manually by a user
  (outside PIM) will be silently overwritten on the next reconfigure, while a
  manually-changed target path would be preserved. This asymmetry is easy to miss
  when reasoning about "does PIM respect user customization of shortcuts."
- **Filename sanitization only happens for the Programs-Menu path**
  (`CreateShortcutProgramsmenu`'s illegal-character substitution) — not confirmed
  present in the other 4 location-specific creators in this pass; if a product
  name containing `/`, `*`, `|`, or `\` is ever used for a Desktop/Start-Menu/
  Quick-Launch/loadpoint shortcut, verify the same sanitization applies before
  assuming parity.

## Usage Example (as evidenced by call sites)

```cxx
// pattern inside pimEntitlement::InstallShortcuts()
pimShortcuts &Mgr = pimShortcuts::GetInstance();
int ret = Mgr.Create(xmlPtr);   // 0 if another shortcut op is in progress process-wide
```

---
*Extends Phase 5 of the requested 20-phase documentation set (remaining Loop subclasses, done at the same depth as `pimCopyLoop`).*
