# `pimCustomDlg`

**Header**: `pim_ui/includes/pimCustomDlg.h` (218 lines)
**Implementation**: `pim_ui/pim_ui_src/pimCustomDlg.cxx` (2,276 lines) + SAB (Smart Address Bar) glue shared with `pimInstallMgrDlg` in `pim_ui/pim_ui_src/pimSAB.cxx` (589 lines)
**Module**: `pim_ui`

> **Correction (this pass)**: an earlier pass (documenting `pimInstallMgrDlg`) flagged `pimCustomDlg` as a "likely-superseded parallel class, not confirmed dead," based on its method-name overlap with `pimInstallMgrDlg`'s embedded Customize tab and its revision history appearing to stop in 2018. A full read of both `.cxx` files for this pass **confirms `pimCustomDlg` is NOT dead code** — it is a live, still-invoked, standalone "Customize" popup dialog with 2 confirmed construction/`Display()` call sites (see Called By), it is compiled into the real build (`pim_ui_src.cxx` directly `#include`s `pimCustomDlg.cxx`, and `pimInstallMgrDlg.h` itself `#include`s `pimCustomDlg.h`), and its shared `pimSAB.cxx` file has revision entries through 2026. The method-name overlap with `pimInstallMgrDlg`'s embedded Customize tab is real and substantial (see Dependencies/Risk Analysis) — the two classes implement the same feature (per-product shortcuts/features/commands/licenses/misc-input customization) via two independent, structurally near-identical code paths — but "duplicate implementation of the same feature" is a different (and, for change-impact purposes, more dangerous) finding than "dead code," and this correction is now propagated to `docs/classes/pimInstallMgrDlg.md`, `ai-context/*.yaml`, and both `README.md` files.

## Purpose

`pimCustomDlg` is a **standalone modal dialog** ("Customize Application Settings")
that lets a user configure per-product install options — feature/language/platform
selection, shortcuts, command-line configuration, simulated license types, misc
user inputs, and (historically) a Help/Advanced web-server-install path — for
**one entitlement at a time**, picked from a list of all customizable products in
the session. It is a second, independent UI surface for the same category of
settings that `pimInstallMgrDlg`'s own embedded "Customize" tab-set already
exposes inline in the main wizard — this class is popped up on demand from 2
separate buttons rather than being part of the wizard's own step flow (see Called
By). Both code paths write to the same `pimEntitlement`/XML-backed manager classes
(`pimShortcutMgr`, `pimHelpMgr`, `pimCommandMgr`, `pimPackageMgr`/`pimLanguageMgr`/
`pimPlatformMgr`), so they are two front-ends over the same underlying state, not
two independent features.

**Scope note**: every method in the header (`pimCustomDlg` itself and its 5 nested
helper classes — `uiCustomTree`, `uiCustomAppList`, `pimSimulateLicenseTable`,
`pimMiscTable`, `pimCommandTable`) was read in full across both `.cxx` files
(`pimCustomDlg.cxx` for the class bodies, `pimSAB.cxx` for the `*SAB*` method
family). This is a complete pass, unlike the necessarily partial pass on
`pimInstallMgrDlg` (which has a much larger, only-partially-traced `.cxx` pair).

## Responsibilities

- Present a picker (`mCustomizableProducts`, a `uiCustomAppList`) of every
  entitlement in the session that both wants to be installed (or, in reconfigure
  mode, is marked for reconfigure) and reports `SupportCustomize(pimEntitlement::Any)`.
- For the currently-selected product (`currentProduct`), drive up to 6 tabs, each
  backed by its own `RefreshXTab()` method and its own nested widget class:
  Application Features (`uiCustomTree`/`mAppFeaturesTree`), Shortcuts, Command
  Configuration (`pimCommandTable`), Simulated Licenses (`pimSimulateLicenseTable`),
  Misc User Inputs (`pimMiscTable`), and a disabled/removed Help/Advanced tab
  (see Risk Analysis).
- Propagate per-child language/platform selections up to a parent entitlement
  (`UpdateParentEntitlements()`) or, in WGM mode, down from a `uwgm.xml` base to
  its adapter children (`UpdateChildEntitlements()`) when the user clicks OK.
- Back up every candidate entitlement's XML file before showing the dialog, and
  restore from backup if the user clicks Cancel — the entire dialog session is
  transactional per-entitlement.
- Own its own Smart-Address-Bar (SAB) instance (`help_dlg_sab_handle`, a
  process-wide `static` in the shared `pimSAB.cxx`) for the Help/Advanced tab's
  web-server-install path picker, distinct from `pimInstallMgrDlg`'s own
  `main_dlg_sab_handle`.

## Dependencies

- `pimSessionInfo` — enumerates `GetEntitlement(i)`/`GetInstalledEntitlement(i)`
  to build the product picker and to back up/restore on Cancel.
- `pimEntitlement` — `currentProduct`; `SupportCustomize()`, `GetRequiredParentEntitlement()`,
  `GetInstallMe()`/`GetReconfigureMe()`, `BackupFile()`/`RestoreFromBackup()`,
  `GetXMLPtr()`, `AreWeAnUpdate()`, `SetQualityAgent()`, `IsQualityAgentRequired()`/
  `IsQualityAgentEnabled()`.
- `pimShortcutMgr` — constructed on the fly in `OnCheckButtonActivate()`,
  `RefreshShortcutsTab()`, and `OnRadioGroupSelect()` from `currentProduct->GetXMLPtr()`;
  not held as a member (matches the pattern already documented in `docs/classes/pimShortcutMgr.md`).
- `pimHelpMgr` — the Help/Advanced tab's web-server-install path/checkbox state.
- `pimCommandMgr` — the Command Configuration tab; delegates the actual add/edit
  UI to `pimCmdCfgDlg` (not traced this pass).
- `pimPackageMgr`/`pimLanguageMgr`/`pimPlatformMgr` — feature/language/platform
  selection state, both for the current product's own tree (`uiCustomTree`) and
  for `UpdateParentEntitlements()`/`UpdateChildEntitlements()`'s cross-entitlement
  propagation.
- `pimInstallMgrDlg` — **not a real dependency of this class's own logic**, but the
  two classes are coupled through 2 shared process-wide `static` globals in
  `pimSAB.cxx` (`main_dlg_sab_handle`, `help_dlg_mode`) and through casts in that
  file's callback functions that assume a `void *user_data2` is one type or the
  other depending on `help_dlg_mode` — see Risk Analysis.
- `pimLocate`/`pimGeneralinit`/`pimLog`/`pimWindows` — help-guide PDF lookup,
  general init helpers, logging, Windows-specific calls (not traced in detail).

## Members

### `pimCustomDlg` (the dialog itself)

| Member | Type | Purpose |
|---|---|---|
| `mCustomizableProducts` | `uiCustomAppList` | product picker list |
| `mCustomTabs` | `uiTab` | the tab container switching between the 6 sub-layouts |
| `mAppFeaturesLayout`/`mAppFeaturesTree` | `uiLayout`/`uiCustomTree` | Application Features tab |
| `mCommandCfgLayout`/`mCommandTable`/`mCmdAdd`/`mCmdEdt`/`mCmdDel` | — | Command Configuration tab |
| `mShortcutsLayout`/`mDesktopCkbn`/`mStartCkbn`/`mProgramsCkbn`/`mTaskbarCkbn`/`mEnvironLabel`/`mAllUsersRG` | — | Shortcuts tab |
| `mSimulateLicensesLayout`/`mSimulateLicenseTable` | — | Simulated Licenses tab |
| `mHelpAdvanced`/`mHelpServerInstall`/`mHelpTextArea` | — | Help/Advanced tab — **`mHelpAdvanced` is never `AddComponent()`'d** (commented out in the constructor); see Risk Analysis |
| `mMiscLay`/`mMiscTable` | — | Misc User Inputs tab |
| `mStdBtnOK`/`mStdBtnCancel` | `uiPushButton` | OK/Cancel |
| `currentProduct` | `pimEntitlement*` | the product currently shown; not owned |
| `initialized` | `bool` | `Initialize()` guard |
| `reconfigure_mode` | `bool` | set once in `Initialize()` from `INSTALLMODE_PROPERTY == "reconfigure"` |
| `SessionInfo` | `pimSessionInfo*` | not owned |

### Nested classes (all declared in `pimCustomDlg.h`, defined in `pimCustomDlg.cxx`)

- **`uiCustomTree : public uiTree`** — the Application Features/Languages/Platforms
  checkbox tree. Owns `CurrentPkgMgr`/`CurrentLangMgr`/`CurrentPlatMgr` (`XNew`'d
  in `SetEntitlement()`, `delete`d in the destructor and re-`delete`d on every
  subsequent `SetEntitlement()` call), plus a `needs_refresh` flag and a
  `TreeCkBtns` array (declared, never populated or read in the traced code —
  confirmed dead member, see Risk Analysis).
- **`uiCustomAppList : public uiList`** — the product picker. `RefreshList()` is a
  confirmed no-op (empty body); the actual list population happens directly in
  `pimCustomDlg::EntitlementRefresh()` via `mCustomizableProducts.Clear()`/`Append()`/
  the base `uiList::Refresh()` (not this class's own `RefreshList()`).
- **`pimSimulateLicenseTable : public uiTable`** — dynamically builds one row +
  one `uiInputPanel` per simulated license type read from the `SimulateLicenseTypes`
  XML property.
- **`pimMiscTable : public uiTable`** — dynamically builds one row + one
  `uiInputPanel` or `uiOptionMenu` per `<USER_INPUT>` XML node, keyed by a `type`
  attribute of `"IP"` (free text) or `"List"` (option menu).
- **`pimCommandTable : public uiTable`** — the Command Configuration tab's row
  list; `Init()` stashes borrowed pointers to the Add/Edit/Delete buttons so
  `OnSelect()` can toggle their sensitivity based on selection count and
  `pimCommandMgr::CanDeleteCommand()`. Declares `friend class pimCustomDlg;`.

## Public/Protected APIs

- **`Initialize(pimSessionInfo *)`** — idempotent (`if (initialized) return true;`);
  reads `INSTALLMODE_PROPERTY` to set `reconfigure_mode`, then calls `InitSAB()`.
  Contains a `#if 0`-disabled Windows-7-specific taskbar-pin-checkbox-hiding block
  (dead, matching the project's other confirmed `#if 0`/`#ifdef` dead-code
  instances catalogued in `ai-context/ai_readme.md`'s High-Risk Areas).
- **`Display()`** — calls `EntitlementRefresh()` (populates the picker and
  auto-selects the first eligible product), then `uiDialog::Display()`, toggles
  `SetHelpModeSAB(true)` for the duration of `uiDialog::Activate()` (the modal
  event loop), then `SetHelpModeSAB(false)`, and returns `Activate()`'s result
  (1 = OK, 0 = Cancel, matching `OnPushButtonActivate()`'s `Exit(1)`/`Exit(0)`).
- **`OnPushButtonActivate(uiPushButton &)`** — `mStdBtnOK`: fixes up parent/child
  entitlement propagation (`UpdateParentEntitlements()` normally,
  `UpdateChildEntitlements()` in WGM mode — mutually exclusive, gated on
  `pimGetProductMode() == PIM_WGM_MODE`) then `Exit(1)`+`Destroy()`.
  `mStdBtnCancel`: restores every candidate entitlement from its backup, then
  `Exit(0)`+`Destroy()`. `mCmdAdd`/`mCmdEdt`/`mCmdDel`: delegate to `pimCmdCfgDlg`
  for add/edit, or call `pimCommandMgr::DeleteCommand()` directly for delete.
- **`OnClose()`** — literally `return OnPushButtonActivate(mStdBtnCancel);` — the
  window-close (X button) path is a single-line delegate to the Cancel path, so
  there is no duplicated cleanup logic here (contrast with `pimInstallMgrDlg`'s
  own confirmed 3-way `OnClose()`/`OnFinishFromEula()`/`OnFinishFromBetaEula()`
  duplication).
- **`OnCheckButtonActivate(uiCheckButton &)`** — the 4 shortcut-location checkboxes
  each loop `for (i = 0; i < arr.GetSize(); i++)` over `GetAllShortcutIDs()` and
  call the matching `SetShortcut*State(arr[i], ...)` — confirmed to use the loop's
  own array/index consistently (this is the exact call site referenced as a
  correctly-written comparison point in `docs/classes/pimShortcutMgr.md`'s writeup
  of the `pimSilent.cxx` `pimSilentFixupShortcuts()` array-index bug). The
  `mHelpServerInstall` branch toggles `pimHelpMgr` web/local-install state and
  calls `currentProduct->GetXMLPtr()->DoSave()` directly (an explicit, immediate
  save — not gated by `SetContentChangedFlag()`, consistent with that flag being
  a no-op everywhere per the project's earlier `pimXmlFile` findings).
- **`OnInputPanelInput`/`OnOptionMenuSelect`** — both the free function on
  `pimCustomDlg` and the near-identical member on `pimMiscTable` walk the same
  `<USER_INPUT>` DOM node list looking for a matching `name` attribute and set
  its text content — 2 independent, near-duplicate DOM-walking implementations
  in the same file (see Risk Analysis).
- **`OnRadioGroupSelect`** — the all-users/current-user shortcut environment
  choice; delegates straight to `pimShortcutMgr::SetEnvironmentChoice()`.
- **`OnListSelect`** — product-picker selection; looks up the matching
  `pimEntitlement` by ID (reconfigure vs. normal list) and calls `RefreshDlg()`.
- **`OnOptionMenuSelect`** — delegates to `mMiscTable.OnOptionMenuSelect()`.
- **`OnHelp()`** — locates and opens `pim_install_guide.pdf` via `pimSystemShowFile()`.
- **`EntitlementRefresh()`** *(void, not `bool`)* — populates `mCustomizableProducts`
  from either `GetInstalledEntitlement()` (reconfigure) or `GetEntitlement()`
  (normal), backing up each candidate's XML file as it goes, and auto-selects +
  `OnListSelect()`s the first eligible one.
- **`RefreshDlg()`** — rebuilds `mCustomTabs`' layout set from `currentProduct->SupportCustomize(...)`
  flags (mirrors `pimInstallMgrDlg::RefreshDlg()`'s own conditional `AddLayout()`
  pattern almost line-for-line), then dispatches to `OnTabSelect()` for the
  now-current tab.
- **`UpdateParentEntitlements()`/`UpdateChildEntitlements()`** — see Purpose and
  Risk Analysis; structurally very close to, but not identical to, the pair of
  same-named methods inside `pimInstallMgrDlg`/its nested `uiCustomAppTree` (see
  Risk Analysis for the specific differences found).
- **`RefreshShortcutsTab()`/`RefreshHelpTab()`/`RefreshFeatureTab()`/`RefreshCommandCfgTab()`/`RefreshLicensesTab()`/`RefreshMiscTab()`**
  — one per tab; **`RefreshFeatureTab()` is a confirmed unconditional `return false;`**
  no-op, exactly like `pimInstallMgrDlg::RefreshFeatureTab()`. Here, though, this
  appears to be **intentional, not a bug**: the actual feature-tree refresh work
  happens inside `uiCustomTree::SetEntitlement()`/`Refresh()`, called directly from
  `RefreshDlg()` and `uiCustomTree::OnUpdate()` — `OnTabSelect()`'s call to
  `RefreshFeatureTab()` for the Application Features tab has nothing left to do.
  This makes it plausible (not proven — `pimInstallMgrDlg`'s own tree/tab wiring
  was not re-examined against this hypothesis this pass) that `pimInstallMgrDlg`'s
  identical-looking `RefreshFeatureTab()` no-op finding has the same benign
  explanation; flagged here as an open question for whoever revisits that class.
- **`ShowSAB()`/`HideSAB()`/`DisableSAB()`/`EnableSAB()`/`SetSABPath()`/`IsSABTextMode()`/`InitSAB()`/`SetHelpModeSAB()`**
  — defined in the shared `pim_ui_src/pimSAB.cxx`, not in `pimCustomDlg.cxx`
  itself. **`SetSABPath()`, `DisableSAB()`, and `EnableSAB()` all guard on the
  wrong static handle — see Risk Analysis, the highest-severity confirmed finding
  for this class.**

## Private Utilities

- **`InitSAB(void *)`** — creates `help_dlg_sab_handle` via `uit_sab_create()` and
  wires up its refresh/reload/popup/list2text/text2list/free callbacks, all
  shared free functions in `pimSAB.cxx` that switch behavior on the `help_dlg_mode`
  static bool rather than on `this`.
- **`uiCustomTree::RefreshPkg()`/`Refresh()`/`Init()`** — the recursive
  package-tree builder (features + languages + platforms, root-then-children),
  including the `SupportOnlyOne()` single-platform-selection enforcement logic
  (radio-like behavior implemented over checkbox nodes) and the reconfigure-mode
  variant that only shows already-`install`ed nodes. **`Refresh()` (the 4-argument
  overload) always `return`s `false`** regardless of whether it actually
  refreshed anything (line ~2162) — every traced caller discards this return
  value, so it is confirmed inert, not a live bug, but a landmine for any future
  caller that starts checking it.
- **`pimSimulateLicenseTable`/`pimMiscTable`'s `AddInputPanel()`/`AddOptionListMenu()`/`DeleteRows()`/`CreateNewRow()`**
  — near-identical low-level `ui_do_operation()`-based dynamic cell-component
  management, duplicated between the two table classes (see Risk Analysis).

## Called By

- `pim_ui/pim_ui_src/pimInstallMgrActions.cxx:649` —
  `pimInstallMgrDlg::OnPushButtonActivate()`'s `CustomizeBtn` branch: constructs a
  stack-local `pimCustomDlg`, `Initialize(SessionInfo)`s it, `Display()`s it, then
  calls `EntitlementRefresh_low()` on the main wizard dialog to reflect any
  changes made in the popup.
- `pim_ui/pim_ui_src/pimEntitlementTree.cxx:1328-1348` —
  `pimEntitlementTree::OnPushButtonActivate()`'s per-row "customize" icon-column
  button handler (`pushed.GetId()` matching `"*icon"`): constructs a stack-local
  `pimCustomDlg`, `Initialize(pimGetSessionInfo())`s it, `Display()`s it. **The
  specific `pimEntitlement *E` resolved from the clicked row's user data is
  fetched (line 1335) but never passed to `pimCustomDlg` — `Initialize()`'s
  signature has no parameter for it.** See Risk Analysis.

No other call sites exist anywhere in the archive (confirmed by a full-archive
grep for `pimCustomDlg` outside its own header/`.cxx`/the shared `pimSAB.cxx`).
Both confirmed callers are themselves inside `pim_ui` — `pimCustomDlg` is not
called from `pim_core` or `pim` directly.

## Calls Into

`pimShortcutMgr`, `pimHelpMgr`, `pimCommandMgr`, `pimPackageMgr`, `pimLanguageMgr`,
`pimPlatformMgr`, `pimCmdCfgDlg` (not traced), `pimEntitlement` (via
`currentProduct`), `pimSessionInfo`, and the shared SAB free functions in
`pimSAB.cxx`.

## Lifetime

Both confirmed callers construct `pimCustomDlg` as a **plain C++ stack-local
object**, `Initialize()` it, `Display()` it (which blocks on the modal
`uiDialog::Activate()` event loop until OK/Cancel/Close), and let it fall out of
scope on function return — no dynamic allocation, no `GetInstance()` singleton
pattern (unlike the 7 `pim_core` owner-wrapper classes documented earlier in this
effort). A fresh `pimCustomDlg` instance is created per button-click, not reused
across calls.

## Ownership Model

`pimCustomDlg` does not own `SessionInfo` or `currentProduct` (both borrowed
pointers, set from the caller-supplied `pimSessionInfo` and resolved internally
from it). It DOES own its nested widget-class instances as direct (non-pointer)
members, and `uiCustomTree` additionally heap-owns its 3 `pimPackageMgr`/
`pimLanguageMgr`/`pimPlatformMgr` instances (`XNew`'d, `delete`d on every
`SetEntitlement()` call and in the destructor). The 2 shared `pimSAB.cxx` statics
(`main_dlg_sab_handle`, `help_dlg_sab_handle`, `help_dlg_mode`) are **process-wide,
owned by neither class** — see Risk Analysis for the correctness implications.

## Thread Safety

No threading primitives anywhere in this class or its nested classes — like every
`pim_ui` dialog class documented so far, this is UI-thread-only code, consistent
with the underlying `uicxx` toolkit's single-threaded event-loop model. Not
re-entrant: `Display()`'s modal `Activate()` call is expected to be the only
active instance of this dialog at a time, though nothing in the class itself
enforces that (no singleton guard, no reentrancy check) — if a future caller
somehow triggered a nested `Display()` call while one was already active, the
`static` SAB handles would be silently shared/clobbered between the two.

## Extension Points

- A new customizable setting needs the same 3-part pattern already documented for
  `pimInstallMgrDlg`: a `RefreshXTab()` method, an `OnTabSelect()` dispatch
  branch, and a `RefreshDlg()` conditional `AddLayout()` call — miss one and the
  new tab is invisible or non-functional, exactly as `RefreshFeatureTab()`'s
  no-op (probably benign) and the disabled `mHelpAdvanced` tab (confirmed
  non-benign, see Risk Analysis) both demonstrate.
- Any change to per-product customize behavior (shortcuts, features, commands,
  licenses, misc inputs) must be evaluated against **both** this class and
  `pimInstallMgrDlg`'s embedded Customize tab-set, since they are independent
  implementations of overlapping functionality — see Risk Analysis.

## Risk Analysis

1. **CONFIRMED BUG: `pimCustomDlg::SetSABPath()`, `DisableSAB()`, and `EnableSAB()`
   all guard on the wrong static SAB handle** (`pim_ui/pim_ui_src/pimSAB.cxx:509-556`).
   `pimSAB.cxx` declares 2 separate process-wide `static uit_smart_ab_t` handles:
   `main_dlg_sab_handle` (set only by `pimInstallMgrDlg::InitSAB()`) and
   `help_dlg_sab_handle` (set only by `pimCustomDlg::InitSAB()`). All 3 of
   `pimCustomDlg`'s own `SetSABPath()`/`DisableSAB()`/`EnableSAB()` check
   `if (main_dlg_sab_handle)` — the OTHER class's handle — before acting on
   `help_dlg_sab_handle` (or, in `SetSABPath()`'s case, `main_dlg_sab_handle` is
   checked but `help_dlg_sab_handle` is what's actually passed to
   `uit_sab_set_path()`/`uit_sab_set_sensitive()`). Contrast with
   `pimCustomDlg::ShowSAB()`/`HideSAB()`/`IsSABTextMode()`, which correctly use no
   guard or the correct `help_dlg_sab_handle`, and with `pimInstallMgrDlg`'s own
   5 SAB methods, which all correctly guard on `main_dlg_sab_handle`. This is a
   confirmed copy-paste error isolated to exactly these 3 methods.
   **Consequence**: if `pimCustomDlg::Display()` is ever reached in a process
   where `pimInstallMgrDlg::InitSAB()` has not already run and set
   `main_dlg_sab_handle` to non-`NULL`, `pimCustomDlg`'s own Help/Advanced SAB
   path-setting and enable/disable calls silently no-op (return `false`,
   nothing logged) even though `help_dlg_sab_handle` itself is valid. In the
   traced call sites, `pimInstallMgrDlg` is always the main wizard dialog and
   very likely has already run `InitSAB()` earlier in the same process by the
   time either `CustomizeBtn` or the entitlement-tree customize icon can be
   clicked — which is almost certainly why this has never been observed as a
   visible failure. This masking is coincidental (a non-`NULL`-check on the
   wrong variable, not a correct cross-class synchronization), so it should not
   be relied on; any future code path that can reach `pimCustomDlg::Display()`
   before `pimInstallMgrDlg::InitSAB()` has run (or in a process that never
   constructs `pimInstallMgrDlg` at all) would hit a silently broken
   Help/Advanced address bar.
2. **CONFIRMED UX GAP: the per-row "customize" icon in `pimEntitlementTree` does
   not pre-select that row's product.** `pimEntitlementTree::OnPushButtonActivate()`
   (`pim_ui/pim_ui_src/pimEntitlementTree.cxx:1328-1348`) resolves the specific
   `pimEntitlement *E` for the clicked row's icon (line 1335,
   `pushed.GetUserData((void **)&E)`) but never passes it to
   `pimCustomDlg` — `Initialize(pimSessionInfo *)`'s signature has no parameter
   for a target entitlement. `pimCustomDlg::EntitlementRefresh()` instead always
   auto-selects the *first* enumeration-order entitlement that is
   installable/reconfigurable and `SupportCustomize()`-capable, regardless of
   which row's icon was clicked. The user can still reach the intended product
   manually via the dialog's own `mCustomizableProducts` picker, so this is a
   UX inconvenience, not a data-correctness bug — but it means the per-row
   button's apparent promise ("customize *this* product") is not honored by the
   implementation.
3. **Two independent, near-duplicate implementations of the same customize
   feature set.** `pimCustomDlg`'s `RefreshShortcutsTab()`/`RefreshCommandCfgTab()`/
   `RefreshLicensesTab()`/`RefreshMiscTab()`/`RefreshDlg()`/
   `UpdateParentEntitlements()`/`UpdateChildEntitlements()`/`OnCheckButtonActivate()`
   have direct, near-identical counterparts inside `pimInstallMgrDlg` (confirmed
   by name and, for `UpdateParentEntitlements()`/`RefreshDlg()`, by close reading
   of both bodies) — and `pimInstallMgrDlg.h`'s own nested classes
   `pimCustomSimulateLicenseTable`, `pimCustomMiscTable`, and
   `pimCustomCommandTable` are renamed structural duplicates of this class's
   `pimSimulateLicenseTable`, `pimMiscTable`, and `pimCommandTable`. A fix,
   security patch, or new validation rule applied to the customize feature set
   in only one of the two classes leaves the other silently unpatched. This is
   the same category of risk as the already-documented
   `UpdateParentEntitlements()`/`uiCustomAppTree` duplication *inside*
   `pimInstallMgrDlg` alone, now shown to extend across 2 entire classes, not
   just one class and its nested helper.
   **Difference found between the two `UpdateParentEntitlements()` copies**:
   `pimCustomDlg`'s version is gated by `if (!reconfigure_mode)` for its "normal"
   branch and unconditionally runs its own separate `else` branch keyed off
   `SessionInfo->GetInstalledEntitlementSize()` for reconfigure mode — a
   reconfigure-mode code path that was NOT confirmed present in
   `pimInstallMgrDlg`'s copy during that class's own pass (not re-verified here;
   flagged as an open comparison for a future pass that wants to fully reconcile
   the two copies line-by-line).
4. **`mHelpAdvanced` (the Help/Advanced tab layout) is a partially-removed
   feature, not a clean removal.** The `$$21` revision entry
   ("Remove mHelpAdvanced tab and it's references; not required now") comments
   out `AddComponent(mHelpAdvanced)` in the constructor and the
   `mCustomTabs.AddLayout(mHelpAdvanced.GetId(), NULL)` call and the
   `OnTabSelect()` dispatch branch for it in `RefreshDlg()`/`OnTabSelect()` — but
   the `SupportCustomize(pimEntitlement::HelpAdvanced)` block in `RefreshDlg()`
   still unconditionally executes `EnableSAB()`/`DisableSAB()`/`SetSABPath()`
   calls for a tab layout that is never added to `mCustomTabs` and can therefore
   never be shown. Combined with finding #1 above, this means: for any product
   that reports `SupportCustomize(HelpAdvanced)` true, `RefreshDlg()` performs
   SAB state changes for a tab the user can never see, using method calls that
   are themselves confirmed broken via the wrong-handle guard. Net effect is
   currently harmless (no visible UI, calls are no-ops per finding #1) but is 2
   layers of dead/broken code stacked on each other, not 1.
5. **2 independent DOM-walking implementations for `<USER_INPUT>` matching.**
   `pimCustomDlg::OnInputPanelInput()` and `pimMiscTable::OnOptionMenuSelect()`
   both walk `getElementsByTagName("USER_INPUT")` looking for a node whose
   `name` attribute matches a `type_str` derived from the UI cell, then call
   `setTextContent()` on the first match — nearly identical logic duplicated
   for the input-panel (free text) vs. option-menu (list) cases, in the same
   file, rather than sharing one helper. A fix to the matching logic (e.g. to
   handle a `name` collision, or to add a new attribute check) applied to only
   one of the two would desynchronize free-text vs. list-type misc inputs.
6. **`uiCustomTree`'s `TreeCkBtns` member is declared but never populated or
   read** in any traced method — confirmed dead data member, low severity.
7. **`uiCustomTree::Refresh(Pkg, Lang, Plat, no_expand)` (the 4-argument
   overload) always returns `false`**, even on a fully successful refresh —
   every traced caller discards the return value, so this is currently inert,
   but see Private Utilities.
8. **The `#if 0`-disabled Windows-7 taskbar-pin block in `Initialize()`** is a
   third confirmed instance (alongside `ROLLBACK_CANCELLED_INSTALL` and
   `ERROR_ON_UNSIGNED`, both catalogued in `ai-context/ai_readme.md`'s High-Risk
   Areas) of `#if`/`#ifdef`-guarded dead code in this codebase — verify the
   actual build's preprocessor state before assuming Windows-7-specific
   taskbar-pin suppression is or isn't active.

## Usage Example

Both confirmed real usages follow the same pattern (from
`pim_ui/pim_ui_src/pimInstallMgrActions.cxx:647-656`):

```cpp
if (pushed == CustomizeBtn)
{
    pimCustomDlg customConfigApplicationsDlg;
    if (customConfigApplicationsDlg.Initialize(SessionInfo))
    {
        customConfigApplicationsDlg.Display();     // blocks (modal) until OK/Cancel/Close
        EntitlementRefresh_low();                   // reflect any changes back in the main wizard
        return true;
    }
}
```

Note `Initialize()`'s only parameter is the shared `pimSessionInfo*` — there is no
way to pre-select a specific entitlement (see Risk Analysis #2); the dialog
always starts on whichever eligible product enumerates first.
