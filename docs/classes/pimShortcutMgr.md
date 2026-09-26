# `pimShortcutMgr`

`pim_ui/includes/pimShortcutMgr.h` (64 lines) / `pim_ui/pim_ui_src/pimShortcutMgr.cxx` (486 lines, full read)

## Purpose

`pimShortcutMgr` is a lightweight, stateless-except-for-a-pointer wrapper over
a product's `<SHORTCUT>` XML elements, providing a UI/business-logic-facing
query and mutation API that is independent of `pimShortcutLoop`/`pimShortcuts`
(the install-time class family documented in `docs/classes/pimShortcutLoop.md`
and `docs/classes/pimShortcuts.md`). It is **not** part of the `pim_core`
Loop/owner-wrapper family: it lives in `pim_ui`, has no singleton, no thread,
and no `Execute()` — it is a plain synchronous helper constructed fresh,
stack-local, wherever a caller needs to read or edit shortcut settings before
an actual install/reconfigure runs. Despite living in `pim_ui`, it is also
used directly from two `pim_core` files (`pimEntitlement.cxx`,
`pimSilent.cxx`) — it is not UI-only in practice.

This is the first `pim_ui`-module class to receive full class-doc depth in
this documentation effort.

## Responsibilities

- Look up a `<SHORTCUT id="...">` node by ID (`FindShortcutNode()`) and
  report its label, associated package, and which of **4** location types
  (Start Menu, Programs Menu, Desktop, Quick Launch) are currently marked
  `create="Y"` (`GetShortcutInfoByID()`).
- Toggle each of those same 4 location types' `create` attribute
  independently, by ID (`SetShortcutStartMenuState()`/
  `SetShortcutProgramsMenuState()`/`SetShortcutDesktopState()`/
  `SetShortcutQuicklaunchState()`, all backed by the shared private
  `SetShortcutState()`).
- Get/set the Programs Menu subfolder name and start-in directory for a
  specific shortcut, by ID (`Get`/`SetShortcutProgramMenu()`,
  `Get`/`SetShortcutStartDir()`).
- Enumerate all shortcut IDs in the document (`GetAllShortcutIDs()`), or all
  IDs that declare a specific location child element regardless of its
  `create` state (`GetMatchingShortcutIDs()`, private).
- Determine whether an "environment choice" prompt (all-users vs.
  current-user, per the `Set`/`GetEnvironmentChoice()` pair below) should be
  shown at all (`ShowEnvironmentChoice()`), and read/write the actual
  all-users/current-user choice as a document-wide setting mirrored across
  every `<SHORTCUT>` node's `allusers` attribute
  (`Set`/`GetEnvironmentChoice()`).
- Compare two `pimShortcutMgr` instances — typically one wrapping a new/
  incoming product XML and one wrapping an already-installed or
  previously-loaded XML — to decide whether shortcut-related settings
  changed between them (`AreChangesPending()`), confirmed used to help decide
  whether an update/reconfigure needs to touch shortcuts at all (see Called
  By).

## Dependencies

- `pimXmlFile` (the wrapped document; `FindNodelistAttribMatch()`,
  `GetNextNodelistItem()`, `PreRead`/`PostRead`/`PreWrite`/`PostWrite`).
- No dependency on `pimShortcutLoop`/`pimShortcuts`/`pimLoop` at all — this
  class reads/writes the same `<SHORTCUT>` XML vocabulary independently, not
  through any shared helper with the install-time Loop class.

## Members

| Member | Type | Purpose |
|---|---|---|
| `xml` | `pimXmlFile*` | The wrapped document; not owned (the destructor is empty — no `delete`). |

This class has no other state — every method operates directly on `xml`'s
DOM tree on each call; there is no cached/derived state to keep in sync.

## Public/Protected APIs

- `pimShortcutMgr(pimXmlFile *ptr)` / `~pimShortcutMgr()` — trivial
  constructor/destructor; does not take ownership of `ptr`.
- `bool GetShortcutInfoByID(const char *id, btkString &Label, btkString &assoc_package, bool &start_menu, bool &programsmenu, bool &desktop, bool &quicklaunch)`
  — resolves a shortcut's label (`<NAME>` child text), its linked package
  (`package` attribute on the `<SHORTCUT>` node itself), and the 4
  location-type booleans. **Does not report a 5th, "Loadpoint" state at
  all** — see Risk Analysis for why this matters relative to
  `pimShortcutLoop`.
- `bool ShowEnvironmentChoice()` — scans every `<SHORTCUT>` node; returns
  `false` (suppress the prompt) the instant it finds **any** node with
  `showenvironment="N"`; returns `true` only if it scans the entire document
  without finding one. A single shortcut's opt-out suppresses the prompt
  for the whole document.
- `bool SetShortcutStartMenuState/SetShortcutProgramsMenuState/SetShortcutDesktopState/SetShortcutQuicklaunchState(const char *id, bool install)`
  — four public setters, all thin wrappers over the private
  `SetShortcutState(id, <location tag>, install)`.
- `bool GetShortcutDesktopState(btkString &value)` — **not** the `Get`
  counterpart its name and the 4 setters above would suggest. It takes no
  `id` at all: it finds every shortcut ID that has **any** `<DESKTOP>` child
  (via the private `GetMatchingShortcutIDs()`, which does not check
  `create` state) and returns the `create` value of the **first one found in
  document order**. There is no way to ask "what is shortcut X's desktop
  state?" through this method — only "what is *a* (arbitrary, first-found)
  shortcut's desktop state?". See Risk Analysis.
- `bool SetShortcutProgramMenu(const char *id, const char *value)` /
  `bool GetShortcutProgramMenu(const char *id, btkString &value)` — get/set
  the `<PROGRAMSMENU>` child's text content (the subfolder name) for a
  specific shortcut ID.
- `bool SetShortcutStartDir(const char *id, const char *value)` /
  `bool GetShortcutStartDir(const char *id, btkString &value)` — get/set the
  `<STARTINDIR>` child's text content for a specific shortcut ID.
- `bool GetAllShortcutIDs(StringXArray &out)` — every `<SHORTCUT>` node's
  `id` attribute, in document order, via the `GetNextNodelistItem()`
  stateful-cursor iteration pattern documented in `docs/classes/pimXmlFile.md`.
- `bool SetEnvironmentChoice(bool all_users)` — sets `allusers="Y"`/`"N"` on
  **every** `<SHORTCUT>` node that already has an `allusers` attribute
  (nodes lacking one are left untouched); returns `true` if at least one was
  set. This is a document-wide setting mirrored redundantly across every
  node, not a genuinely per-shortcut one.
- `bool GetEnvironmentChoice(bool &all_users)` — reads the `allusers` value
  from the **first** `<SHORTCUT>` node in document order that has one, then
  stops (consistent with the setting being document-wide, not per-shortcut).
- `bool AreChangesPending(pimShortcutMgr &cmp_to)` — see Responsibilities
  and the **confirmed coverage gap** in Risk Analysis.

## Private Utilities

- `DOMNode *FindShortcutNode(const char *id)` — `xml->FindNodelistAttribMatch(pimSHORTCUT, pimid, id)`, the single lookup primitive nearly every other method builds on.
- `bool SetShortcutState(const char *id, const XMLCh Match[], bool install)`
  — shared implementation behind the 4 public location-toggle setters; sets
  `create="Y"`/`"N"` on the matching location child and calls
  `xml->SetContentChangedFlag(true)` (see Risk Analysis for the 3 sibling
  write methods that do **not** call this).
- `bool GetMatchingShortcutIDs(StringXArray &out, const XMLCh Match[])` —
  every shortcut ID that has a child element named `Match`, **regardless of
  that child's own `create` state** — used only by `GetShortcutDesktopState()`.
- `bool GetShortcutState(const char *id, const XMLCh Match[], btkString &value)`
  — reads the `create` attribute of a specific shortcut's location child;
  used by `GetShortcutDesktopState()`'s per-candidate-ID loop.

## Called By

Confirmed via a full-archive grep of `pimShortcutMgr` — 6 call sites, split
between `pim_ui` and `pim_core`:

- `pim_ui/pim_ui_src/pimCustomDlg.cxx` (3 sites: lines 184, 733, 1113) — each
  constructs `pimShortcutMgr shortcutmgr(currentProduct->GetXMLPtr())` as a
  stack-local instance in response to a UI checkbox toggle
  (Desktop/StartMenu/ProgramsMenu/Taskbar), then calls `GetAllShortcutIDs()`
  followed by the corresponding `Set*State()` in a loop over **every**
  shortcut ID — confirming the "checkbox toggles the state for every
  shortcut in the product uniformly" UI behavior.
- `pim_ui/pim_ui_src/pimInstallMgrActions.cxx` (2 sites: lines 1252, 1452) —
  not individually re-traced beyond confirming their existence in this pass;
  same construction pattern (`currentProduct->GetXMLPtr()`).
- `pim_ui/pim_ui_src/pimInstallMgrDlg.cxx` (1 site: line 1839) — same pattern.
- `pim_core/pim_core_src/pimEntitlement.cxx` (lines 4109-4115, 4152-4158) —
  **confirmed real business use of `AreChangesPending()`**: inside the
  update/reconfigure decision logic, two `pimShortcutMgr` instances are
  constructed over two *different* `pimXmlFile` objects
  (`this->GetXMLPtr()` — the new/current entitlement — vs.
  `UpdateOfProduct->GetXMLPtr()` or `product_xml->GetXMLPtr()` — the
  previously-installed one), and `MyShortcutMgr.AreChangesPending(InstalledShortcutMgr)`
  contributes to the boolean decision of whether an update/reconfigure needs
  to do any work at all. See Risk Analysis for the confirmed coverage gap
  this exposes.
- `pim_core/pim_core_src/pimSilent.cxx` (`pimSilentFixupShortcuts()`,
  lines 423-475+) — a silent/scripted-install helper that copies shortcut
  settings from a user-supplied XML (`Ea`) onto a media's default XML
  (`Eb`), using `GetAllShortcutIDs()` on both and iterating by ID. **A
  confirmed indexing bug was found in this caller** while researching this
  class's usage (see Risk Analysis) — flagged for completeness since it
  stems directly from this class's ID-array-based API shape, even though
  the bug itself lives in `pimSilent.cxx`, not in `pimShortcutMgr` itself.

## Calls Into

- `pimXmlFile` only (`FindNodelistAttribMatch`, `GetNextNodelistItem`,
  `PreRead`/`PostRead`/`PreWrite`/`PostWrite`, `SetContentChangedFlag`).

## Lifetime

Always stack-local and short-lived at every confirmed call site — constructed
immediately before use and allowed to go out of scope at the end of the
enclosing block/function. Never stored as a member, never heap-allocated,
never a singleton. Multiple instances routinely coexist (e.g.
`AreChangesPending()`'s two-instance comparison pattern), each wrapping its
own `pimXmlFile*`.

## Ownership Model

Owns nothing — `xml` is a borrowed, non-owned pointer (confirmed by the
empty destructor). Callers are responsible for the `pimXmlFile`'s own
lifetime, which in every confirmed call site is itself owned elsewhere (a
`pimEntitlement`'s XML, accessed via `GetXMLPtr()`).

## Thread Safety

- Every DOM access is bracketed by the standard `pimXmlFile`
  `PreRead()`/`PostRead()` or `PreWrite()`/`PostWrite()` protocol, consistent
  with every other class in this codebase that touches `pimXmlFile` directly.
- This class holds no lock of its own and has no shared/static state, so its
  own thread-safety is entirely inherited from whatever guarantees
  `pimXmlFile` provides for concurrent access to the *same* underlying
  document from two different `pimShortcutMgr` instances (or from a
  `pimShortcutMgr` and, say, a concurrently-running `pimShortcutLoop`
  install thread on the same XML) — not independently re-verified in this
  pass beyond what `docs/classes/pimXmlFile.md` already documents about that
  class's own locking protocol and its confirmed prior lock-bug history.
- `GetAllShortcutIDs()`/`GetMatchingShortcutIDs()`/`ShowEnvironmentChoice()`/
  `Set`/`GetEnvironmentChoice()` all use `pimXmlFile::GetNextNodelistItem()`,
  the same **stateful, per-`pimXmlFile`-instance cursor** flagged as a
  highest-risk lock-transition area in `docs/classes/pimXmlFile.md`. Two
  `pimShortcutMgr` instances wrapping the **same** underlying `pimXmlFile`
  and calling any of these methods concurrently would share that cursor —
  not confirmed to be reachable from any traced call site in this pass (every
  confirmed site either uses a single instance sequentially or pairs two
  instances over two *different* `pimXmlFile` objects), but worth flagging
  for any future caller that might construct two instances over the same
  document.

## Extension Points

- Any new location type added to `pimShortcutLoop`'s install-time vocabulary
  (currently 5: Desktop/StartMenu/ProgramsMenu/QuickLaunch/Loadpoint, per
  `docs/classes/pimShortcutLoop.md`) needs a matching addition here if the UI
  is meant to let users toggle it — this class currently only covers 4 of
  those 5 (see Risk Analysis).
- A caller needing a genuine per-ID desktop-state query (rather than
  `GetShortcutDesktopState()`'s "first match in the document" behavior)
  would need a new method — `GetShortcutState()` (private) already does the
  right per-ID lookup and could be exposed directly with an `id` parameter.
- `AreChangesPending()`'s add/remove-detection gap (see Risk Analysis) should
  be fixed by also checking for IDs present in `cmp_to` but absent from
  `this` (removal) and IDs present in `this` but absent from `cmp_to`
  (addition) — both currently fall through unnoticed.

## Risk Analysis

- **CONFIRMED: `AreChangesPending()` cannot detect an added or removed
  shortcut, only a changed one.** Its loop iterates only over
  `this->GetAllShortcutIDs()`; for each ID, it calls
  `cmp_to.GetShortcutInfoByID(id, ...)` and only compares fields **if that
  call returns `true`** (i.e., the ID exists in `cmp_to` too). An ID present
  in `this` but not in `cmp_to` (a shortcut newly added to the product XML)
  is silently skipped — the `if` block is simply never entered for it. An ID
  present in `cmp_to` but not in `this` (a shortcut removed entirely) is
  never even considered, since the outer loop only iterates `this`'s ID
  list. Given the confirmed real use of this method in
  `pimEntitlement.cxx`'s update/reconfigure decision logic (see Called By),
  this means: **adding or removing a `<SHORTCUT>` definition entirely,
  between an installed product version and its update, is not by itself
  sufficient to make `AreChangesPending()` report `true`** — only a change
  to a shortcut ID that exists in *both* versions is detected. If nothing
  else in the surrounding decision logic separately catches a pure add/
  remove, an update could silently skip shortcut-related reinstall work it
  actually needs.
- **CONFIRMED, though currently inert: `SetShortcutProgramMenu()`,
  `SetShortcutStartDir()`, and `SetEnvironmentChoice()` never call
  `xml->SetContentChangedFlag(true)`**, unlike `SetShortcutState()` (the
  4 location-toggle setters' shared implementation), which does. Verified
  by reading all four methods side by side
  (`pimShortcutMgr.cxx:169-189, 213-233, 322-354` vs. `:426-454`). **This is
  confirmed to have no observable effect in the current codebase**: a
  full-archive grep found zero call sites anywhere that read
  `pimXmlFile::ContentChanged()` (the flag's only accessor) to gate any
  decision — `pimXmlFile::DoSave()` itself unconditionally calls
  `DoWrite()` regardless of the flag's value, merely resetting it as a side
  effect. So today, omitting the flag-set call does **not** cause any
  content to be silently dropped. It is nonetheless a genuine internal
  inconsistency in this class's own convention (4 of 7 mutating methods set
  the flag, 3 do not, with no principled reason found for the split), and it
  is a latent landmine: if a future change ever makes persistence
  conditional on `ContentChanged()` (a very plausible optimization for a
  codebase that otherwise treats this flag as meaningful — every other class
  in this codebase that mutates `pimXmlFile` content, e.g.
  `pimRegEditLoop`/`pimShortcutLoop`/`pimMSILoop`/`pimServiceLoop`, is
  scrupulously consistent about setting it), edits made via these 3 methods
  would start silently failing to persist with no code change to this file
  itself required to trigger it.
- **CONFIRMED caller-side indexing bug found while tracing usage**
  (`pim_core/pim_core_src/pimSilent.cxx`'s `pimSilentFixupShortcuts()`,
  lines 447-460): the function iterates `for (i = 0; i < A_wants.GetSize(); i++)`
  and, having confirmed `A_wants[i]` also exists as an ID in `B_avail`
  (`B_avail.Find(A_wants[i]) != -1`), correctly uses `A_wants[i]` as the ID
  argument for `GetShortcutProgramMenu`/`SetShortcutProgramMenu`/
  `GetShortcutStartDir`/`SetShortcutStartDir` (lines 462-466) — but for the
  4 location-state calls immediately above (lines 457-460:
  `B_shtcuts.SetShortcutStartMenuState(B_avail[i], sm)` etc.), it uses
  **`B_avail[i]`** — the element at loop index `i` in a *different* array
  that is not guaranteed to be in the same order as `A_wants`. Since `i`
  indexes `A_wants`, `B_avail[i]` can name a completely different shortcut
  ID than the one just matched, whenever the two arrays' document orders
  diverge. This would apply the wrong location-toggle state to the wrong
  shortcut during a silent-install shortcut fixup. This is a defect in the
  caller, not in `pimShortcutMgr` itself, but it is a direct, confirmed
  consequence of this class's ID-array-based API being easy to misuse when
  two independently-populated ID arrays are combined — worth flagging to
  whoever owns `pimSilent.cxx` as a priority fix candidate (the fix is
  replacing `B_avail[i]` with `A_wants[i]` in those 4 calls, matching the
  correct pattern already used two calls later in the same block).
- **`GetShortcutDesktopState()` is not a per-ID query despite its name and
  its sibling setter's signature.** It silently returns "the desktop
  `create` state of whichever shortcut happens to declare a `<DESKTOP>`
  child first in document order," not a caller-specified shortcut's state.
  Its one confirmed caller (`pimMSILoop`'s `prime_msi` special case,
  `docs/classes/pimMSILoop.md`) only works correctly because that scenario
  is known to involve a single relevant shortcut; a future caller assuming
  ID-specific behavior from the name alone would get a wrong answer on any
  document with more than one Desktop-capable shortcut in a mixed state.
- **Missing Loadpoint coverage**: `pimShortcutLoop` supports 5 location
  types; this class's entire public surface (get, set, and the
  `GetShortcutInfoByID()` bulk query) covers only 4. A caller cannot audit
  or toggle Loadpoint-shortcut state through this class at all.

## Usage Example

```cpp
// Traced from pimEntitlement.cxx:4109-4115 (update/reconfigure decision logic)
pimShortcutMgr MyShortcutMgr(this->GetXMLPtr());              // new/current XML
pimShortcutMgr InstalledShortcutMgr(UpdateOfProduct->GetXMLPtr()); // previously-installed XML

if (MyShortcutMgr.AreChangesPending(InstalledShortcutMgr))
{
    return true; // an update/reconfigure needs to touch shortcuts
    // NOTE: a shortcut ADDED or REMOVED entirely between the two versions
    // will NOT be caught here -- only a changed shortcut that exists in both.
}
```

```cpp
// Traced from pimCustomDlg.cxx:182-189 (a "toggle all Desktop shortcuts" checkbox handler)
pimShortcutMgr shortcutmgr(currentProduct->GetXMLPtr());
StringXArray arr;
shortcutmgr.GetAllShortcutIDs(arr);
for (int i = 0; i < (int)arr.GetSize(); i++)
    shortcutmgr.SetShortcutDesktopState(arr[i], Btn.IsChecked());
```
