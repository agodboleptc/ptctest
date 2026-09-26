# `pimEntitlementTree`

**Header**: `pim_ui/includes/pimEntitlementTree.h` (127 lines)
**Implementation**: `pim_ui/pim_ui_src/pimEntitlementTree.cxx` (2,196 lines)
**Module**: `pim_ui`

## Purpose

`pimEntitlementTree` is the checkbox-tree widget that drives the wizard's
"Applications" step — the screen where a user selects which products/features to
install (new/download mode) or reconfigure (reconfigure mode), grouped into
per-family nodes (Creo, Mathcad, Utilities, Help Centers, WGM, etc., discovered at
runtime from the product/image XML rather than hardcoded family names — a
`$$26`/2015 revision entry explicitly documents this as a refactor away from
hardcoded family nodes). It owns the on-screen row-level UI components it creates
per entitlement (quality-agent checkboxes, per-row "customize" icon buttons,
per-row version/shipcode option menus) and is the confirmed source of the 2nd
`pimCustomDlg` call site found while documenting that class.

**Scope note**: every method in the 127-line header was read in full against the
2,196-line `.cxx` file (all 39 methods, contiguous line-by-line coverage
confirmed by cross-checking every `pimEntitlementTree::` signature against the
read ranges). This is a complete pass. One adjacent file was NOT traced in depth:
`pim_ui/pim_ui_src/pimEntitlementRefresh.cxx` (1,514 lines) — a **previously
uncatalogued 3rd `.cxx` file implementing `pimInstallMgrDlg::` methods**
(`EntitlementRefresh()`, `EntitlementRefresh_low()`,
`EntitlementRefresh_updateCustomize/Space/Nextbtn/Qagent()`,
`IsSufficientSpace()`, `ApplicationsNextPreAction()`, `CustomizeNextPreAction()`,
`EntitlementDownloadPreAction()`) that is the single largest driver of this
class's public API (`AddEntitlement()`/`RemoveAllEntitlementNodes()`/
`SetReconfigure()`/`SetColumns()`/`SetCallback()`/`InitDisplay()`/
`UpdateInitDisplay()`/`UpdateDisplay()`/`GetPrerequisiteSizeRequired()` are all
called from this one file, confirmed by grep). **This is a correction to
`docs/classes/pimInstallMgrDlg.md`**, whose own Scope note only listed
`pimInstallMgrDlg.cxx` and `pimInstallMgrActions.cxx` as the class's method-body
files — `pimEntitlementRefresh.cxx` is a 3rd, and its own internals remain
untraced (only its call-site signatures into `pimEntitlementTree` were read,
via targeted grep and 2 short excerpts, not its full body).

## Responsibilities

- Build and maintain a `uiTree` of per-family group nodes, each containing
  entitlement rows and (in non-reconfigure mode) their unsatisfied prerequisite
  rows nested under a synthetic "prerequisite"/"Utilities" node.
- Track selection/reconfigure checkbox state per row and propagate it: a child's
  checkbox change can force its `GetRequiredParentEntitlement()` parent's
  checkbox on too; a family group node's checkbox reflects the aggregate
  selected/unselected/sensitive state of its children (tri-state:
  none/some/all — see `UpdateGroupStatus()`).
- Own and lazily create/destroy 3 kinds of per-row UI components as entitlements
  are added/removed: quality-agent checkboxes (`mAgentboxes`), "customize" icon
  push-buttons (`mIcons`), and version/shipcode option menus (`mVersionMenus`) —
  each tracked in a parallel `btkXArray` searched linearly by a `"tag%suffix"`
  component-ID convention (e.g. `"creo.xml%qagent"`).
- Handle the version/shipcode dropdown's cascading re-fetch: changing a row's
  selected media ID re-downloads that product's XML via `pimGetMediaDetails`,
  re-translates it if the UI language differs from the media's own language via
  `pimTranslateMgr`, and propagates the same shipcode to every sibling
  entitlement sharing the same `GetRequiredParentEntitlement()` (or to the
  parent itself), so a family of related products (e.g. all WGM adapters under
  one WGM base) stays on a consistent version.
- Apply a large cluster of hardcoded, product-tag-keyed special-case rules
  (`creoviewexpress.xml`, `mathcadpdsi.xml`, `qualityagent.xml`,
  `ar_plugin_solid/nx/in.xml`, `creobase.xml`, `mkscomponents.xml`, WGM
  adaptor/base tags, etc.) governing default install state and checkbox
  sensitivity in `UpdateDisplay_low()` — this codebase's now-familiar pattern
  (seen previously in `pimEntitlement.cxx`, `pimMSILoop.cxx`) of business rules
  expressed as inline product-tag string comparisons rather than data-driven
  XML attributes.
- Provide the customize-icon click path into `pimCustomDlg` (see Risk
  Analysis and `docs/classes/pimCustomDlg.md`).

## Dependencies

- `pimSessionInfo` — the source of truth for `GetEntitlement()`/
  `GetInstalledEntitlement()`; `mItems` holds borrowed (not owned) pointers into
  its arrays, matching the ownership pattern already documented for other
  `pim_ui`/`pim_core` classes that reference `pimSessionInfo`-owned entitlements.
- `pimEntitlement` — nearly every method reads or writes entitlement state
  (`GetInstallMe()`/`SetInstallMe()`, `GetReconfigureMe()`/`SetReconfigureMe()`,
  `GetRequiredParentEntitlement()`, `HasPrerequisite()`/`GetNextPrerequisite()`,
  `IsPrerequisiteSatisfied()`, `SupportCustomize()`, `GetInstallStatus()`, and
  many more).
- `pimInstallMgrDlg` — **bidirectional coupling, not a clean one-way
  parent-owns-child relationship**: `pimInstallMgrDlg.h` declares
  `pimEntitlementTree ApplicationsTree;` as a direct member AND
  `friend class pimEntitlementTree;` (`pimInstallMgrDlg.h:842`), so this class
  reaches back up to its owner's private state via the global
  `GetMainUIDialog()` pointer (`GetMainUIDialog()->AvailableDownloads->...`,
  `GetMainUIDialog()->EntitlementRefresh_updateNextbtn()`,
  `GetMainUIDialog()->EntitlementRefresh_updateSpace()`) rather than through a
  narrow, one-directional interface.
- `pimGetMediaDetails` — a singleton (`GetInstance()`) queried in
  `OnOptionMenuSelect()` for `GetDetails()`/`GetProductXml()` when a row's
  version dropdown selection changes.
- `pimTranslateMgr` — invoked to re-translate a freshly-fetched product XML
  when the UI language differs from the media's own language, fully traced in
  a further, dedicated pass — see `docs/classes/pimTranslateMgr.md`.
  **CONFIRMED BUG, found via this exact call site**: `OnOptionMenuSelect()`
  constructs 3 near-identical `pimTranslateMgr`/`LocateTranslationFile()`
  sequences (`:1885`, `:1956`, `:2006` — for "this application changed",
  "sibling shares this parent", and "this is the parent" respectively); only
  the first explicitly calls `eptr_new_xml->SetXml(E->GetTag())` before
  constructing `pimTranslateMgr`. The other 2 rely on `aPtr->Init(eptr_new_xml, ...)`
  aliasing `aPtr`'s own `xmlPtr` to `eptr_new_xml` and a later
  `aPtr->GetXMLPtr()->SetXml(cachefile)` call to set `eptr_new_xml`'s file
  path indirectly — which is confirmed to NOT happen when
  `pimEntitlement::Init(pimXmlFile*, ...)` takes its `DownloadXmlBackups`
  early-return branch (an already-seen MediaID), since that branch aliases
  `xmlPtr` to the CACHED backup object instead, leaving `eptr_new_xml` itself
  with no file path ever set. See `docs/classes/pimTranslateMgr.md`'s Risk
  Analysis for the full trace and consequence (silently skipped `file`-scoped
  translation entries on these 2 arms).
- `pimPrerequisite`/`pimGetMediaDetails.h` — prerequisite XML lookup helpers
  (`#include`d; not traced beyond their call-site signatures here).
- `pimCustomDlg` — constructed and `Display()`ed directly from
  `OnPushButtonActivate()`'s per-row customize-icon handler; see
  `docs/classes/pimCustomDlg.md` for that class's own full writeup (this doc's
  Risk Analysis only covers the caller-side half of that relationship).
- `btkscale31` — an unscrambling helper (`BTK_UNSCRAMBLE_31_S`) used once, to
  read an `always_install_ar_plugin` environment-variable override name; not
  traced further (a code-obfuscation/anti-tamper utility outside this
  documentation effort's scope).

## Members

| Member | Type | Purpose |
|---|---|---|
| `mItems` | `btkXArray<pimEntitlement *>` | borrowed entitlement pointers currently shown in the tree |
| `mAgentboxes` | `btkXArray<uiCheckButton *>` | owned; one per row with a quality-agent column cell |
| `AgentHandler` | `uitTreeChkBoxCol` | opaque handle from `uit_tree_checkbox_col_register()`, lazily created, never explicitly unregistered (see Risk Analysis) |
| `mIcons` | `btkXArray<uiPushButton *>` | owned; one per row supporting `SupportCustomize()` |
| `mVersionMenus` | `btkXArray<uiOptionMenu *>` | owned; one per row with >1 shipcode option, outside trial/school/beta mode |
| `family_grp` | `StringXArray` | family names discovered at runtime (see `GetFamilyListForTree()`) |
| `family_count` | `int` | `family_grp`'s size, cached separately |
| `SelectionChangedCB`/`QualityAgentChangedCB`/`VersionChangedCB` | `void (*)(void)` | raw C function-pointer callbacks (no user-data closure) invoked into the owning dialog |
| `is_init` | `bool` | `InitDisplay()` guard |
| `reconfigure_mode` | `bool` | set via `SetReconfigure()`; selects `UpdateDisplay_low()`/`ReconfigureDisplay_low()` code paths throughout |
| `columns_flag` | `int` | bitmask of `PIM_SHOW_*` column-visibility flags |

Note: 3 of the 7 `PIM_SHOW_*` flag `#define`s in the header
(`PIM_SHOW_DOWNLOAD_SIZE`, `PIM_SHOW_INSTALL_SIZE`, `PIM_SHOW_QAGENT_STATUS`)
are hardcoded to `0` with an inline comment ("disable ... column for now"),
permanently disabling those columns regardless of what a caller ORs into
`SetColumns()` — confirmed dead-flag pattern, see Risk Analysis.

## Public/Protected APIs

- **`AddEntitlement(pimEntitlement &)`** — appends to `mItems` if not already
  present by tag; does NOT create the tree node itself (see `AddEntitlementNode()`
  below, invoked lazily from `UpdateDisplay_low()`/`ReconfigureDisplay_low()`).
- **`InitDisplay()`/`UpdateInitDisplay()`** — idempotent first-time setup
  (`InitDisplay()`) vs. a re-run for family-group changes after the tree is
  already initialized (`UpdateInitDisplay()`) — both call
  `GetFamilyListForTree()` and build the per-family group nodes; near-duplicate
  bodies (see Risk Analysis).
- **`SetColumns(int flags)`** — inserts/deletes tree columns to match the
  bitmask; idempotent per column via `GetColumnLabel()` presence checks.
- **`SetReconfigure(bool)`/`GetReconfigure()`** — the mode switch read
  throughout the rest of the class.
- **`GetPrerequisiteSizeRequired(double &size_required)`** — **accumulates
  into the caller's reference without zeroing it first** — both confirmed
  call sites (`pim_ui_src/pimEntitlementRefresh.cxx:1094` and `:1460`) correctly
  `dbl = 0.0;` immediately before calling, so this is not a live bug, but the
  function's own contract (caller must pre-zero) is undocumented and
  unenforced — a future caller that reuses an already-accumulated value would
  silently double-count.
- **`SetCallback(CallbackType, void (*)(void))`** — registers one of the 3 raw
  function-pointer callbacks (`SelectionChanged`/`QualityAgentChanged`/
  `VersionChanged`).
- **`UpdateDisplay(pimEntitlement *ptr = NULL)`** — the main refresh entry
  point: with a specific entitlement, updates just that row
  (`UpdateDisplay_low()`/`ReconfigureDisplay_low()`); with `NULL`, iterates all
  of `mItems` and additionally calls the `_finish()` variant. **The
  `ReconfigureDisplay_low(mItems[i], max == 1)` call for the all-items case
  passes `only_one=true` exactly when there is exactly 1 entitlement in the
  tree — the trigger condition for the confirmed off-by-one bug documented in
  Risk Analysis.**
- **`OnPushButtonActivate(uiPushButton &)`** — matches component IDs ending in
  `"icon"`, resolves the row's `pimEntitlement*` from `GetUserData()`, and
  constructs/`Initialize()`s/`Display()`s a `pimCustomDlg` — this is the
  confirmed 2nd `pimCustomDlg` call site (see `docs/classes/pimCustomDlg.md`
  Risk Analysis #2 for the confirmed UX gap this creates).
- **`OnCheckButtonActivate(uiCheckButton &)`** — the quality-agent column
  checkbox handler; sets `pimEntitlement::SetQualityAgent()`, notifies the
  checkbox-column helper API, fires `QualityAgentChangedCB`, and calls
  `GetMainUIDialog()->EntitlementRefresh_updateNextbtn()` directly (see
  Dependencies re: the friend-based back-reference).
- **`OnOptionMenuSelect(uiOptionMenu &, cStringT mediaid)`** — the version/
  shipcode dropdown handler; see Responsibilities and **Risk Analysis #1 (the
  confirmed unconditional null-pointer-dereference bug)**, the highest-severity
  finding in this class.
- **`OnUpdate(char *Node, int State)`** — the generic tree-checkbox-toggled
  callback; dispatches by whether the toggled node id starts with `"_"`
  (a family group node) or not (an individual entitlement row), with separate
  reconfigure-mode and normal-mode branches; delegates most of the actual state
  propagation to `SetSelectionStatus()`/`FindInstallAppItemListByTag()`.
- **`OnOptionMenuActivate`/`OnActivate`/`OnSelect`/`OnCellSelect`** — all 4
  are confirmed pure `return UI_SUCCESS;` no-ops. Plausibly intentional (this
  tree reacts to checkbox/option-menu *changes*, handled by `OnUpdate()`/
  `OnCheckButtonActivate()`/`OnOptionMenuSelect()`, not to plain
  activation/selection events) rather than unfinished — not confirmed either
  way, flagged as an open question consistent with this project's "no invented
  behavior" rule.
- **`OnExpand`/`OnCollapse`** — trivial delegates to `ExpandNode()`/
  `CollapseNode()`.
- **`Hide()`/`Show()`** — toggle visibility of every entitlement row and every
  family group node; used by `pimEntitlementRefresh.cxx` around mode-transition
  points (not traced in depth this pass — see Scope note).

## Private Utilities

- **`GetFamilyListForTree()`** — 3-way source of family names: a physical
  media image's `<FAMILY>` XML nodes (`LocatePhysicalImage()`), a hardcoded
  single `"Creo"` family in reconfigure mode with no physical image, or
  `GetMainUIDialog()->AvailableDownloads->GetFamilyList()` for the web-install
  case. The reconfigure-mode hardcoded `"Creo"` fallback is a confirmed
  simplification (not exercised for other product families in reconfigure
  mode without a physical image) rather than a data-driven lookup — not proven
  to be wrong (no counter-example found), but notably the one hardcoded family
  name left after the `$$26` refactor's stated goal of removing hardcoded
  family nodes.
- **`AddEntitlementNode(pimEntitlement *E, bool prerequisite_flag)`** — the
  per-row node-insertion logic: resolves the correct family parent/type,
  finds an insertion point via `FindAfterNodeByType()`, creates the row plus
  its icon/version-menu/quality-agent cell components as applicable, and
  recursively adds any unsatisfied prerequisites as child "prerequisite" rows
  (skipped entirely in reconfigure mode). Its initial
  `btkString use_type = "OtherApp_app";` local is unconditionally overwritten
  2 lines later by `use_type = family + "_app";` — a confirmed dead
  initialization, harmless.
- **`FindAfterNodeByType()`** — linear scan over same-type nodes comparing
  `pimEntitlement::GetPref()` values to find correct sort-order insertion
  point.
- **`RemoveEntitlementNode()`/`RemoveItemByTag()`/`RemoveAgentBox()`/
  `RemoveIcon()`/`RemoveVersionMenu()`** — `RemoveItemByTag()`'s body has an
  explicit `//delete tmp;` comment left in place, correctly NOT deleting the
  `pimEntitlement*` being removed from `mItems` — consistent with this class
  not owning those pointers (see Dependencies). `RemoveAgentBox()`/
  `RemoveIcon()`/`RemoveVersionMenu()` DO `delete` their respective owned UI
  component objects.
- **`UpdateDisplay_low()`/`ReconfigureDisplay_low()`** — the per-row state
  computation for normal vs. reconfigure mode respectively; see Responsibilities
  for the hardcoded product-tag rule cluster inside `UpdateDisplay_low()`, and
  Risk Analysis for the confirmed bug inside `ReconfigureDisplay_low()`.
- **`UpdateDisplay_finish()`/`ReconfigureDisplay_finish()`** — post-pass work
  after all rows are updated: propagating child selections up to parents,
  showing/hiding prerequisite rows, mode-specific lockouts (trial/school mode
  disables `"_Creo"`/`"pma.xml"`; WGM mode disables the WGM family node plus
  `creoagent.xml`/`vfs.xml`), and a final `UpdateGroupStatus()` pass per family.
- **`UpdateAgentStatus()`** — pushes every agent-checkbox's UI-checked state
  back into its entitlement's `SetQualityAgent()`; declared but its only
  caller in this file is inside an `#if 0`-disabled block in
  `UpdateDisplay_finish()` (see Risk Analysis) — otherwise unreferenced within
  this file (not confirmed whether an external caller exists; not searched
  outside this file this pass).
- **`UpdateGroupStatus(btkString &grp_node)`** — computes and applies the
  tri-state (all/some/none selected) family-group checkbox state and
  sensitivity by scanning the family's child node types.
- **`SetSelectionStatus(btkString &use_type, bool state)`** — bulk
  select/deselect for every node of a given type (used for family-group
  checkbox toggles), with parent-entitlement propagation and a
  `pimGetCreoNGCRIMode()`/required/update-in-progress skip guard.

## Called By

Exclusively from `pimInstallMgrDlg`'s own method-body files (this class has no
callers outside `pim_ui`, and no callers in `pim_core`):

- `pim_ui/pim_ui_src/pimEntitlementRefresh.cxx` — by far the primary caller
  (confirmed via grep: ~35 call sites across `EntitlementRefresh()`,
  `EntitlementRefresh_low()`, and the `EntitlementRefresh_update*()` family),
  driving the full `AddEntitlement()`/`RemoveAllEntitlementNodes()`/
  `SetReconfigure()`/`SetColumns()`/`SetCallback()`/`InitDisplay()`/
  `UpdateInitDisplay()`/`UpdateDisplay()`/`GetPrerequisiteSizeRequired()`/
  `Hide()`/`GetReconfigure()` surface. **This file's own internals are not
  traced in this pass** — see this doc's Scope note.
- `pim_ui/pim_ui_src/pimInstallMgrActions.cxx:1436` —
  `ApplicationsTree.UpdateDisplay(NULL)`, a single call site (this file's
  broader `OnPushButtonActivate()` was the source of the confirmed
  `pimCustomDlg` `CustomizeBtn` call site documented separately).

## Calls Into

`pimSessionInfo`, `pimEntitlement` (extensively), `pimCustomDlg` (constructs
directly), `pimGetMediaDetails` (singleton), `pimTranslateMgr`, `pimInstallMgrDlg`
(via the `GetMainUIDialog()` global + friend access), and the `uit_tree_checkbox_col_*`
free-function API for the quality-agent checkbox column.

## Lifetime

A single `pimEntitlementTree` instance, `ApplicationsTree`, is a direct
(non-pointer) member of `pimInstallMgrDlg` (`pimInstallMgrDlg.h:568`) — its
lifetime is exactly its owner's lifetime. Since `pimInstallMgrDlg` itself is
constructed as a plain stack-local object at each of `pimTop.cxx`'s 3 Run
entry points (per `docs/classes/pimInstallMgrDlg.md`), `ApplicationsTree`'s
construction/destruction is likewise automatic, with no separate heap
allocation for the tree object itself. Individual rows and their per-row UI
components (`mAgentboxes`/`mIcons`/`mVersionMenus` entries) are dynamically
`XNew`'d/`delete`d over the object's lifetime as entitlements are added/removed
via `AddEntitlementNode()`/`RemoveEntitlementNode()`.

## Ownership Model

- **Does not own**: `mItems`' `pimEntitlement*` pointers (borrowed from
  `pimSessionInfo`'s arrays — confirmed by `RemoveItemByTag()`'s explicit
  non-deleting behavior).
- **Owns**: every `uiCheckButton*`/`uiPushButton*`/`uiOptionMenu*` in
  `mAgentboxes`/`mIcons`/`mVersionMenus` — each `XNew`'d in
  `AddEntitlementNode()` and `delete`d in the matching `RemoveXxx()` method.
- **No explicit destructor** is declared on `pimEntitlementTree` itself (the
  header lists only a constructor). Cleanup of the 3 owned-pointer arrays
  relies entirely on `RemoveAllEntitlementNodes()`/`RemoveEntitlementNode()`
  having been called for every row before the object is destroyed; if the
  owning `pimInstallMgrDlg` were ever torn down with rows still present
  (not confirmed to happen on any traced exit path — `pimEntitlementRefresh.cxx`
  is not traced in depth), the individually-`XNew`'d component objects would
  leak. Low practical severity given this is an installer process that exits
  shortly after teardown, but a confirmed structural gap.
- **`AgentHandler`** (`uitTreeChkBoxCol`) is likewise never explicitly
  unregistered anywhere in this file once `uit_tree_checkbox_col_register()`
  creates it.

## Thread Safety

No threading primitives anywhere in this class — UI-thread-only code, consistent
with every other `pim_ui` dialog/widget class documented in this effort.

## Extension Points

- A new family/product-tag-specific special case (matching the existing
  `creoviewexpress.xml`/`mathcadpdsi.xml`/`ar_plugin_*`/WGM-tag pattern) must be
  added directly inside `UpdateDisplay_low()`'s (or, for reconfigure mode,
  `ReconfigureDisplay_low()`'s, which currently has none of these special
  cases — see Risk Analysis for the asymmetry this implies) hardcoded
  `if`/`else if` chain — there is no data-driven mechanism for these rules.
- A new column (beyond the 7 `PIM_SHOW_*` flags, 3 of which are currently
  hardcoded off) needs a `SetColumns()` insert/delete branch AND a
  corresponding cell-population branch in `UpdateDisplay_low()`/
  `ReconfigureDisplay_low()` — omitting the second produces an always-empty
  column.

## Risk Analysis

1. **CONFIRMED BUG (highest severity): `OnOptionMenuSelect()` dereferences a
   pointer it just null-checked and then bypassed.**
   `pim_ui/pim_ui_src/pimEntitlementTree.cxx:1875-1881`:
   ```cpp
   eptr_new_xml = MD.GetProductXml(mediaid, E->GetTag());
   if (eptr_new_xml)
   {
       eptr_new_xml->DoWrite(NULL);
       LG_DEBUG(LOG_SERVICE, btkString(eptr_new_xml->GetXMLContents()) );
   }
   eptr_new_xml->SetXml(E->GetTag());   // <-- unconditional, outside the if() above
   ```
   The `if (eptr_new_xml)` guard covers only the `DoWrite()`/logging pair; the
   very next line calls `->SetXml()` on `eptr_new_xml` unconditionally. The
   function's own later code (`if (E && new_xml && eptr_new_xml)` at line 1904)
   proves the author knew `eptr_new_xml` could be `NULL` at this point in the
   flow — but that check comes AFTER the unconditional dereference already
   executed. Contrast with the 2 later, structurally similar blocks in the same
   function (the "shares same parent" and "is parent" branches, ~lines
   1940-1946 and 1982-1988), which pass their own `eptr_new_xml` result into
   `aPtr->Init(eptr_new_xml, ...)` as a parameter rather than dereferencing it
   directly — a safe pattern this first occurrence does not follow.
   **Consequence**: selecting a different shipcode/version from a row's
   version-menu dropdown, for a product not already locally known
   (`E->Init(NULL, new_xml, mediaid)` returns `false`), crashes the installer
   with a null-pointer dereference if `pimGetMediaDetails::GetProductXml()`
   returns `NULL` for that specific media ID + product tag combination — a
   real, reachable failure mode (network/media-server lookup failure for a
   specific SKU), not a purely theoretical one.
   **Enrichment (found while documenting `pimGetMediaDetails` itself)**: a
   full read of `GetProductXml()` (`pim_core/pim_core_src/pimGetMediaDetails.cxx:143-238`)
   confirms exactly which conditions make this reachable — it returns `NULL`
   whenever no `<url>` entry in the cached details document matches the
   target filename, or the download fails on 2 consecutive attempts after a
   `-418` timeout — both real outcomes of a network/media-feed lookup, not
   merely a hypothetical. Tracing this class's other callers also surfaced a
   near line-for-line duplicate of this exact function, with the identical
   unconditional-dereference bug, in `uiApplicationsList::OnOptionMenuSelect()`
   (`pim_ui_src/pimInstallMgrActions.cxx:1806-1858`) — but that entire
   function (and the rest of `uiApplicationsList`'s method bodies,
   `pimInstallMgrActions.cxx:1672-2019`) is wrapped in `#if 0`, confirmed not
   live, mirroring `uiApplicationsList`'s own `#if 0`-disabled class
   declaration in `pimInstallMgrDlg.h`. Strong evidence this bug pattern
   predates `pimEntitlementTree` rather than being unique to it. See
   `docs/classes/pimGetMediaDetails.md`.
2. **CONFIRMED BUG: `ReconfigureDisplay_low()`'s single-entitlement family-node
   id computation drops one character too many.**
   `pim_ui/pim_ui_src/pimEntitlementTree.cxx:1518-1520`:
   ```cpp
   str = "_" + use_type;                         // e.g. "_Creo_app" (9 chars)
   SetNodeState(str(0,str.GetLength() -5), true);          // -> "_Cre" (4 chars)
   SetNodeCheckboxSensitive(str(0,str.GetLength() -5), false);
   ```
   `use_type` is always `"<family>_app"` (an 8-character-minimum suffix of
   exactly `"_app"`, 4 characters); to strip that suffix from `"_" + use_type`
   the code needs `GetLength() - 4`, not `GetLength() - 5` — confirmed by the
   sibling, differently-computed but correctly-working family-id extraction in
   `UpdateGroupStatus()` (`grp_node(1, grp_node.GetSize()-1)`, which correctly
   strips only the leading underscore). The `-5` version drops one extra
   character (e.g. produces `"_Cre"` instead of `"_Creo"`), targeting a node id
   that does not exist in the tree.
   **Reachable when**: `UpdateDisplay(NULL)` is called in reconfigure mode with
   exactly 1 entitlement in `mItems` (`ReconfigureDisplay_low(mItems[i], max == 1)`
   passes `only_one=true` in exactly this case) — a single-product reconfigure
   session.
   **Consequence**: the family group node's checkbox is never actually forced
   to checked+insensitive as the `only_one` branch intends (the call silently
   targets a nonexistent node id); the family-level checkbox is left in
   whatever state it last had instead. Low-to-moderate user-visible impact (a
   checkbox cosmetic/consistency issue in a narrow single-product reconfigure
   scenario), but a clean, deterministic, always-reproducing bug for that
   scenario — not data-dependent on which family is involved.
3. **Asymmetric special-case coverage between normal and reconfigure mode.**
   `UpdateDisplay_low()`'s hardcoded product-tag rule cluster
   (`creoviewexpress.xml`/`mathcadpdsi.xml`/`qualityagent.xml`/
   `ar_plugin_*`/WGM-adaptor rules) has no counterpart inside
   `ReconfigureDisplay_low()`, which is a substantially shorter function with
   none of these special cases. Not confirmed to be a bug (reconfigure mode
   may legitimately not need the same forced-selection heuristics, since a
   reconfigure session starts from an already-installed, already-consistent
   state) — flagged as an open question for anyone extending either function,
   since the two are NOT structurally mirrored the way, e.g.,
   `pimInstallMgrDlg`'s `StepForward()`/`StepBack()` pair was found to almost-
   but-not-quite be.
4. **3 of 7 `PIM_SHOW_*` column flags are permanently hardcoded to `0`**
   (`PIM_SHOW_DOWNLOAD_SIZE`, `PIM_SHOW_INSTALL_SIZE`, `PIM_SHOW_QAGENT_STATUS`
   — `pimEntitlementTree.h:27-29`), with an inline comment stating this is
   temporary ("disable ... column for now"). Any caller `|`-ing these flags
   into `SetColumns()` gets no effect, silently — confirmed dead flags, not
   reachable via any code path in this archive (their bits are `#define`d to
   0 at the macro level, before any runtime logic runs).
5. **No explicit destructor / no explicit `AgentHandler` unregistration** —
   see Ownership Model. Low practical severity for a short-lived installer
   process, but a real structural gap if this class were ever reused in a
   longer-lived host.
6. **`GetPrerequisiteSizeRequired()`'s accumulate-only contract is
   undocumented and unenforced** — see Public/Protected APIs. Both traced
   callers get this right; a new caller would not be warned if it didn't.

## Usage Example

The confirmed `pimCustomDlg` call site (`OnPushButtonActivate()`,
`pim_ui/pim_ui_src/pimEntitlementTree.cxx:1328-1348`):

```cpp
bool pimEntitlementTree::OnPushButtonActivate(uiPushButton &pushed)
{
    if (btkString(pushed.GetId()).Match("*icon"))
    {
        pimEntitlement *E = NULL;
        pushed.GetUserData((void **)&E);
        if (E)
        {
            pimCustomDlg customConfigApplicationsDlg;
            if (customConfigApplicationsDlg.Initialize(pimGetSessionInfo()))
            {
                customConfigApplicationsDlg.Display();
                return UI_SUCCESS;
            }
        }
    }
    return UI_SUCCESS;
}
```

Note `E` is resolved but never forwarded into `pimCustomDlg` — see
`docs/classes/pimCustomDlg.md` Risk Analysis #2 for the confirmed consequence.
