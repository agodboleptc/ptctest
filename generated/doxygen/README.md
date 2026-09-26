# Doxygen Comment Patches

Generated patches (not applied — source was not modified) adding Doxygen-style
comments to the 32 header files documented in depth (`docs/classes/*.md`), all in
`pim_core/includes/` except `pimShortcutMgr.h`/`pimInstallMgrDlg.h`/`pimCustomDlg.h`/
`pimEntitlementTree.h`/`pimFrictionlessTrialDlg.h`/`rpimDlg.h`/`pimAuthDlg.h`
(`pim_ui/includes/`) and `pimGeneralInit.h` (`pim/includes/`):

- Phase 5/6 original 5: `pimLoop.h`, `pimCopyLoop.h`, `pimXmlFile.h`,
  `pimSessionInfo.h`, `pimEntitlement.h`.
- Loop-subclass extension pass (parity with `pimCopyLoop.h`'s treatment, added when
  documenting "the remaining Loop subclasses at the same depth"): `pimSFXLoop.h`,
  `pimScriptLoop.h`, `pimShortcutLoop.h`, `pimServiceLoop.h`, `pimPsfLoop.h`,
  `pimDownloadLoop.h`.
- Owner-wrapper singleton extension pass (parity with the above, added when
  documenting "the 7 owner-wrapper singleton classes at the same depth"):
  `pimCopier.h`, `pimMSICopier.h`, `pimSFXCopier.h`, `pimShortcuts.h`,
  `pimRegEdit.h`, `pimServices.h`, `pimDownloader.h`.
- Final Loop-subclass extension (parity with the above, added when documenting
  "pimRegEditLoop at the same depth" — the 9th and last Loop subclass):
  `pimRegEditLoop.h`.
- Final light-detail-class upgrade (parity with the above, added when documenting
  "pimMSILoop at the same depth" — the last remaining `full_detail: false` Loop
  subclass, completing full-depth coverage of the entire Loop-subclass family):
  `pimMSILoop.h`.
- First `pim_ui` extension (parity with the above, added when documenting
  "pimShortcutMgr at the same depth" — the first `pim_ui`-module class to
  receive full-depth documentation in this effort): `pimShortcutMgr.h`
  (`pim_ui/includes/`, not `pim_core/includes/`).
- Second `pim_ui` extension (parity with the above, added when documenting
  "pimInstallMgrDlg at the same depth" — the largest class documented in
  this entire effort, the main wizard dialog driving the whole interactive
  install experience): `pimInstallMgrDlg.h` (`pim_ui/includes/`, 847 lines).
  Given the header's scale (~150 members, ~90 methods, 10 nested classes),
  annotation focused on the class-level doc, brief one-line class-level
  comments on each nested class, and full Doxygen on the specific methods
  this pass traced with concrete evidence (construction/init/display/close,
  the license-server port-validation cluster, the customize-tab refresh
  cluster, `UpdateParentEntitlements`/`UpdateChildEntitlements`, and the
  wizard step state machine) — methods whose bodies were not traced in this
  pass (e.g. the ~550-line `OnPushButtonActivate()`) were left without
  invented behavioral claims, per the "no invented behavior" rule.
- Third `pim_ui` extension (parity with the above, added when documenting
  "pimCustomDlg at the same depth"): `pimCustomDlg.h` (`pim_ui/includes/`, 218
  lines) plus its 5 nested helper classes, all fully annotated (unlike
  `pimInstallMgrDlg.h`, this header was small enough for complete, not
  partial, per-method annotation). **This pass corrected an earlier
  speculative finding**: `pimInstallMgrDlg.h`'s own class-level comment
  (and this README) previously described `pimCustomDlg` as "likely
  superseded" — a full read of both `.cxx` files confirmed it is in fact
  live, still-invoked code with 2 confirmed call sites, not dead or
  superseded. See below.
- Fourth `pim_ui` extension (parity with the above, added when documenting
  "pimEntitlementTree at the same depth" — the 2nd confirmed `pimCustomDlg`
  caller, and the checkbox tree driving the wizard's Applications step):
  `pimEntitlementTree.h` (`pim_ui/includes/`, 127 lines), fully annotated
  (small enough for complete coverage). This pass also surfaced a
  **previously uncatalogued 3rd `.cxx` file** for `pimInstallMgrDlg`'s own
  method bodies, `pim_ui_src/pimEntitlementRefresh.cxx` (1,514 lines, at the
  time flagged not-traced — since read in full and folded into the
  `pimInstallMgrDlg.h` patch, see the eighth `pim_ui` extension entry below)
  — see `docs/classes/pimEntitlementTree.md`'s Scope note and the correction
  recorded in `docs/classes/pimInstallMgrDlg.md`.
- Fifth `pim_ui` extension (parity with the above, added when documenting
  "pimFrictionlessTrialDlg at the same depth" — the UI for `pim_rl.exe`'s
  Frictionless/Commercial Trial license-retrieval flow, not the main install
  wizard): `pimFrictionlessTrialDlg.h` (`pim_ui/includes/`, 76 lines), fully
  annotated (small enough for complete coverage). This pass also read the
  companion worker class `pimFrictionlessTrialLicenseGet` in full (not
  annotated as its own header — no `docs/classes/pimFrictionlessTrialLicenseGet.md`
  exists — but necessary to confirm this class's headline finding, below).
- Sixth `pim_ui` extension (parity with the above, added when documenting
  "rpimDlg at the same depth" — the UI for `pim_re.exe`'s "Renew License"
  flow, and the closest sibling class to `pimFrictionlessTrialDlg`):
  `rpimDlg.h` (`pim_ui/includes/`, 63 lines), fully annotated (small enough
  for complete coverage). This pass's value is largely comparative: unlike
  `pimFrictionlessTrialDlg::Display()`, `rpimDlg::Display()` correctly calls
  `uiDialog::Activate()`, confirming by direct contrast that the sibling
  class's `Sleep(60000)` bug is a genuine defect, not an inherent limitation
  of the shared dialog design.
- Seventh `pim_ui` extension (parity with the above, added when documenting
  "pimAuthDlg at the same depth" — the oldest of the 3-class dialog family
  and its confirmed original template, called directly from `pim_core`):
  `pimAuthDlg.h` (`pim_ui/includes/`, 60 lines), fully annotated (small
  enough for complete coverage). This pass confirmed `pimAuthDlg`'s
  `AuthDialog` singleton is genuinely reused (re-`Initialize()`d) across
  multiple authentication challenges in one process — via a confirmed
  `pim_core` retry loop, `pimSessionInfo::TryAuthorize()` — and surfaced
  that `Initialize()` has no idempotency guard, exactly the one class in
  this family that needed one.
- Ninth extension, back into `pim_core` (parity with the above, added when
  documenting "pimGetAvailable at the same depth" — discovered while tracing
  `pim_ui_src/pimEntitlementRefresh.cxx`'s `EntitlementDownloadPreAction()`):
  `pimGetAvailable.h` (`pim_core/includes/`, 66 lines) plus its companion
  `pimGetAvailableProduct.h` (`pim_core/includes/`, 74 lines), both fully
  annotated (small enough for complete coverage). **This pass corrects every
  prior "all 9 Loop subclasses" claim in this documentation set**:
  `pimGetAvailable` (`class pimGetAvailable : public pimLoop`) is a 10th
  confirmed `pimLoop` subclass, previously only mentioned in passing in
  `docs/modules/pim_core.md` and never counted in any Loop-subclass tally.
  Surfaced 2 confirmed memory leaks in `OnExecute()` (`ImageXml` never freed
  on the success path; `ProductDefinitionXml` never freed on 2 filtered-out
  paths), a confirmed wrong declaring comment on `auth_status`
  (`// -3 abort, 0 success` — `0` is actually failure, not success), a
  confirmed zero-caller dead public method (`IsAuthorized()`), and a
  cross-module raw-pointer alias in `pim_core/pimLocate.cxx` (the same
  architectural pattern already flagged as a live bug in `pimSAB.cxx`, but
  here confirmed currently safe given the exact call sites that exist today).
  See docs/classes/pimGetAvailable.md's Risk Analysis.
- Tenth extension, staying in `pim_core` (parity with the above, added when
  documenting "pimGetMediaDetails at the same depth" — the shared media-details
  cache/fetcher `pimGetAvailable` itself depends on, and a widely-used
  singleton called from at least 7 confirmed files across `pim_core` and
  `pim_ui`): `pimGetMediaDetails.h` (`pim_core/includes/`, 36 lines), fully
  annotated (small enough for complete coverage). Confirmed findings: the
  cache-hit fast path in `GetDetails()` reads its shared array with **no
  lock at all**, while every mutating path holds `Mutex` — a confirmed
  incomplete-locking bug; `GetProductXml()`'s timeout-retry path refreshes
  the details cache but then retries with the stale, pre-refresh URL string,
  confirmed by contrast with the correct sibling pattern in
  `pimEntitlement::UpdateMediaUrls()`; a 2nd, likely-redundant (severity
  unconfirmed — depends on an external `dsXArray::Replace()` contract not in
  this archive) `Replace()` call on that same retry path; and confirmation
  that `GetProductXml()`'s `NULL` returns are the exact, concrete source of
  the already-documented null-pointer-dereference bug in
  `pimEntitlementTree::OnOptionMenuSelect()` — plus discovery of a
  confirmed-dead near-duplicate of that same bug pattern inside
  `uiApplicationsList::OnOptionMenuSelect()` (`pimInstallMgrActions.cxx:1806-1858`,
  wrapped in `#if 0` along with the rest of that class's method bodies). See
  docs/classes/pimGetMediaDetails.md's Risk Analysis.
- Eleventh extension, back into the `pim` module (parity with the above,
  added when documenting "pimGetApplicationsList at the same depth" —
  discovered while tracing `pimGetAvailable::OnExecute()`'s use of it):
  `pimGeneralInit.h` (`pim/includes/`, 130+ lines total, but **only the 4
  declarations directly traced were annotated** — `pimCommandLineArgs`,
  `pimAddApplicationsToList`, `pimGetApplicationsList`,
  `IsApplicationInInstallList` — leaving the file's dozens of other unrelated
  get/set functions uncommented, per the "no invented behavior" rule).
  **Not a class** — `pimGetApplicationsList` is a free function operating on
  a file-scope `static StringXArray`; see
  `docs/classes/pimGetApplicationsList.md` for how this doc's usual template
  was adapted. Confirmed findings: `applications_list` is populated in 2
  **incompatible formats** depending on which of 3 command-line flags is
  used (`-APPLICATIONS`: bare tags; `-XML`/`-XMLALL`: full filesystem paths),
  with no tagging of which — 2 of 3 confirmed consumers filter via exact
  match and can never recognize a path-format entry, while the 3rd
  (`IsApplicationInInstallList()`) uses a format-agnostic substring search.
  Separately, and more severely: `InstallPreReqSilent()`
  (`pim_src/pimTop.cxx:904-966`, on the silent-install path) uses this list's
  **size alone** — never its content — to bound a loop indexing a completely
  different array (`pimGetSessionInfo()->GetEntitlement(i)`), so its entire
  prerequisite-installation loop silently never runs whenever the
  command-line application list is empty (the common case). See
  `docs/classes/pimGetApplicationsList.md`'s Risk Analysis.
- Twelfth extension, an update to the existing `pimGetAvailableProduct.h`
  patch (parity with the above, added when documenting
  "pimAvailableProduct at the same depth" — promoting it from a brief
  "companion" mention inside `pimGetAvailable.h`'s own patch/doc to its own
  full-depth pass): no new header, but a corrected/expanded class-level
  comment on `pimAvailableProduct` and a corrected per-method comment on
  `AddInstance()`. **Confirmed bug found in this pass that the original
  companion-level treatment missed**: `AddInstance(const char *V, const char
  *shipcode, const char *media_id)`'s `shipcode` parameter is never
  referenced in its body — it builds and dedups its `VerShipcodes` entry
  from `V` (the bare version) alone, despite the member name and the
  `pimCompareVerShipcode()` comparator used in `Sort()` both implying a
  combined version+shipcode identifier is intended. A 2nd `AddInstance()`
  call for the same version under a different shipcode is silently treated
  as a duplicate and dropped, along with its media ID. See
  `docs/classes/pimAvailableProduct.md`'s Risk Analysis.
- Thirteenth extension, a 2nd update to the existing
  `pimGetAvailableProduct.h` patch (parity with the above, added when
  documenting "pimAvailableProduct's `Print()` at the same depth"): added
  Doxygen comments to both `Print()` overloads (previously uncommented on
  both classes). **Confirmed dead code found in this pass**:
  `pimAvailableProduct::Print()` and `pimGetAvailableProducts::Print()`
  (which simply fans out to the former) have zero live callers anywhere in
  this archive — an exhaustive grep for `.Print(` across `pim`, `pim_core`,
  and `pim_ui` finds only 2 matches, both commented out
  (`pimGetAvailable.cxx:474,478`, `//AvailableProductsArray.Print(pimDbgLog);`),
  plus one unrelated `Test.Print(...)` call on a different object in
  `pimTop.cxx:3042`. `pimDbgLog` itself is likewise referenced nowhere else
  in this codebase outside those same disabled debug-print lines. See
  `docs/classes/pimAvailableProduct.md`'s Risk Analysis.
- Fourteenth extension, a 2nd update to the existing `pimGeneralInit.h`
  patch (parity with the above, added when documenting
  "`IsApplicationInInstallList` at the same depth" — a dedicated pass on the
  one free function in this group not yet given its own focused
  re-scrutiny): re-confirmed its call-site list is unchanged (still exactly
  the 2 calls, both inside `AddMandatoryAppsForSilentInstall()`, its only
  caller) and expanded its Doxygen comment with a **confirmed structural
  risk in the opposite direction** from the file's other findings: its
  substring search (`Pos()`, not exact match), while confirmed the one
  format-agnostic consumer robust to the bare-tag-vs-full-path split, can
  itself false-positive whenever one product's filename is a literal
  substring of another entry already in `applications_list` (e.g. a
  hypothetical `"newcreobase.xml"` entry would make a check for
  `"creobase.xml"` return `true` even though `"creobase.xml"` itself was
  never requested). Checked pairwise against the 3 real confirmed arguments
  used in this codebase today (`"creobase.xml"`, `"creobase.p.xml"`,
  `"qualityagent.xml"`) — none collides, so this is recorded as a confirmed
  structural risk, not a demonstrated live bug. See
  `docs/classes/pimGetApplicationsList.md`'s Risk Analysis.
- Fifteenth extension, a new header (parity with the above, added when
  documenting "`pimSilentTestXmlIsUseable` at the same depth"):
  `pimSilent.h` (`pim_core/includes/`, 24 lines), fully annotated — **not a
  class**, a 7-function free-function group (like `pimGeneralInit.h`'s)
  implementing the silent-install "does the user's requested product XML
  match, and can it be satisfied by, this media's product XML" validation
  and merge pipeline, discovered while tracing
  `pimSilentInstallFromXML()`'s per-entry validation loop (see the eleventh
  extension entry above). **Confirmed findings**: `pimSilentTestXmlIsUseable()`
  leaks a `pimXmlFile` on its XML-syntax-error path (the `delete` that runs
  on the success path is skipped by the early `return false;`); and
  `pimSilentCanMatchNeeds()` leaks **2** `pimXmlFile` objects whenever
  either of its 2 input files fails to parse — every internal early-return
  inside its success branch correctly deletes both first, making the one
  outer failure-fallthrough path that doesn't stand out clearly by
  contrast. See `docs/classes/pimSilent.md`'s Risk Analysis.
- Sixteenth extension, an update to the existing `pimSilent.h` patch
  (parity with the above, added when documenting "`pimSilentFixupPSF` at the
  same depth"): expanded its Doxygen comment after tracing its dependency on
  `pimCommandMgr::DeleteCommand()`/`CanDeleteCommand()`'s explicitly
  documented "cannot delete the last command of a type" constraint
  (`pim_core/pim_core_src/pimCommandMgr.cxx:405-451,454-515`). **Confirmed
  findings**: the `pDiUmMMY<N>` dummy placeholder this function creates to
  work around that constraint (so a name colliding with a different license
  type in `Eb` can be removed) can itself become permanently stuck/
  undeletable in `Eb`'s final PSF/command set, if no other surviving command
  ends up sharing its license type by the time the final cleanup loop runs
  — the same "last of a type" rule then blocks deleting the dummy too,
  silently, since neither of this function's relevant `DeleteCommand()`
  calls checks the return value; and the collision-handling code deletes
  **every** command of the colliding license type (via
  `GetAllCommandNamesByType()`), not just the single named command its own
  inline comment says it removes. See `docs/classes/pimSilent.md`'s Risk
  Analysis.
- Seventeenth extension, a 2nd update to the existing `pimSilent.h` patch
  (parity with the above, added when documenting "`pimSilentCanMatchNeeds`
  at the same depth"): expanded its Doxygen comment after tracing its
  dependency on `pimPackageMgr::GetInstallPackageNames()`
  (`pim_core/pim_core_src/pimPackageMgr.cxx:611-666`). **Confirmed
  finding**: this function's package-availability check silently changes
  meaning depending on 3 global, process-wide command-line mode flags
  (`pimGetBasePackMode()`/`pimGetCreoNGCRIMode()`/`pimGetAllPacksMode()`)
  that have nothing to do with the specific `A`/`B` XML pair being
  compared — directly contradicting this function's own header comment
  ("returns false if any packages marked for install in A are not known in
  B"), which only holds in the default, no-special-mode case. Confirmed by
  direct contrast with its 2 sibling checks in the same function
  (`pimPlatformMgr::GetInstallPlatformNames()`/
  `pimLanguageMgr::GetInstallLanguageNames()`), both a simple unconditional
  `install="Y"` lookup with no such dependency. Under `-allpacks`, the check
  becomes stricter (every declared package must exist in `B`, not just
  requested ones); under `-basepack`, it becomes looser (only
  `required="Y"` packages are checked, ignoring the user's actual
  selections). **Corrected by the nineteenth extension below**: this is
  confirmed to be by design, not a validation gap. See `docs/classes/pimSilent.md`'s
  Risk Analysis.
- Eighteenth extension, a 3rd update to the existing `pimSilent.h` patch
  (parity with the above, added when documenting "`pimSilentFixupShortcuts`
  at the same depth"): expanded its Doxygen comment with a 2nd confirmed bug
  in this function, distinct from the caller-side indexing bug already
  documented in `docs/classes/pimShortcutMgr.md`'s Risk Analysis (the 4
  `SetShortcut*State()` calls using `B_avail[i]` instead of `A_wants[i]`).
  **Confirmed finding**: its `GetShortcutProgramMenu()`/
  `SetShortcutProgramMenu()` pair ignores the `Get`'s return value and calls
  the `Set` unconditionally, so a stale Program Menu group name from an
  earlier, unrelated shortcut in the same loop can be copied onto the
  current shortcut (or blanked to empty, on the first iteration) — confirmed
  by direct contrast with the correctly return-value-guarded
  `GetShortcutStartDir()`/`SetShortcutStartDir()` call 3 lines later in the
  same function. See `docs/classes/pimSilent.md`'s Risk Analysis.
- Nineteenth extension, a 4th update to the existing `pimSilent.h` patch
  (parity with the above, added when documenting
  "`pimSilentCreateEntitlement` at the same depth"): expanded its Doxygen
  comment with a finding that **corrects** the seventeenth extension entry
  above. `pimSilentCreateEntitlement()`'s own package-selection loop calls
  the identical `pimPackageMgr::GetInstallPackageNames()` on the same `A`,
  under the same 3 global mode flags, that `pimSilentCanMatchNeeds()` uses
  for its compatibility check — and since `pimSilentCreateEntitlement()`
  only ever runs immediately after a successful `pimSilentCanMatchNeeds()`
  call on the same `A`/`B` pair within the same process, the 2 calls are
  guaranteed to see identical mode flags and therefore an identical set of
  "wanted" packages. This means the earlier characterization of `-basepack`
  as causing a "validation gap" and `-allpacks` as "overly strict" was too
  strong: both are confirmed to be **by design**, since
  `pimSilentCanMatchNeeds()` validates exactly the package set
  `pimSilentCreateEntitlement()` will subsequently attempt to install — the
  2 functions are consistent with each other, not in conflict. The one part
  of the original finding that still stands: both functions' own header
  comments describe a plain `install="Y"` check, an incomplete description
  of the actual, mode-dependent behavior — a documentation accuracy issue,
  not a behavioral one. See `docs/classes/pimSilent.md`'s Risk Analysis.
- Twentieth extension, a 5th update to the existing `pimSilent.h` patch
  (parity with the above, added when documenting "`pimIsProductXmlMatch` at
  the same depth"): expanded its Doxygen comment (and `pimIsVersionMatch()`'s,
  which shares the exact same limitation). **Confirmed structural
  limitation**: neither function has a cross-schema fallback for `B` —
  whichever root-XML schema (`NULL`, i.e. a normal `<PRODUCT>` root, or
  `"EXTERNAL_INSTALLER"`) succeeds for `A` is the only one ever tried for
  `B` in that call; if `B`'s `Init()` fails under that schema, the function
  returns `false` without ever retrying `B` under the other schema. A
  mixed-schema `A`/`B` pair — one `<PRODUCT>`-rooted, one
  `<EXTERNAL_INSTALLER>`-rooted — is confirmed to always report "no match"
  even with identical `<TAG>`/version/shipcode values. Not confirmed
  reachable with any real product/media XML pair in this archive (no sample
  `<EXTERNAL_INSTALLER>`-rooted XML exists to test), but the mechanism
  itself is fully confirmed from both functions' own control flow and
  `pimEntitlement::Init()`'s documented root-tag matching contract. See
  `docs/classes/pimSilent.md`'s Risk Analysis.
- Twenty-first extension, a 6th update to the existing `pimSilent.h` patch
  (parity with the above, added when documenting "`pimSilentFixupPSF`'s
  dummy-placeholder bug at the same depth" — a further, dedicated
  re-scrutiny of one already-documented finding, not a new function):
  expanded its Doxygen comment with 2 new confirmed details. **Reachability,
  precisely characterized**: the stuck-dummy bug fires exactly when no entry
  in `A`'s final wanted command set independently shares the colliding
  command's *old* license type — a plausible, non-degenerate scenario (e.g.
  a single command's license type changing between product versions), not
  merely a contrived edge case. **Downstream consequence, newly traced**:
  `pimEntitlement::InstallScripts()` (`pim_core/pim_core_src/pimEntitlement.cxx:5627-5631`)
  constructs a `pimScriptLoop` (already a fully documented `pimLoop`
  subclass) directly on the **same** `pimXmlFile*` document
  `pimSilentFixupPSF()` mutates, at real install-execution time.
  `pimScriptLoop::OnInstall()` (`pim_core/pim_core_src/pimScriptLoop.cxx:71-127`)
  reads the *first* command name `GetAllCommandNames()` returns —
  unconditionally, with no type-based selection — to populate
  `[LM_LICENSE_FILE]` for that entitlement's generated install scripts. If
  the stuck dummy lands at that position, its own internally-mismatched
  license-identifier data would drive `[LM_LICENSE_FILE]` for the entire
  entitlement, not just its own inert PSF entry. Whether the dummy reliably
  reaches that position in any real product XML is not confirmed either way
  in this archive; the mechanism connecting the 2 functions via the shared
  XML document is fully confirmed. See `docs/classes/pimSilent.md`'s Risk
  Analysis.
- Twenty-second extension, a 7th update to the existing `pimSilent.h` patch
  (parity with the above, added when documenting
  "`pimSilentFixupShortcuts`'s stale-Program-Menu-copy bug at the same
  depth" — a further, dedicated re-scrutiny of one already-documented
  finding, not a new function): expanded its Doxygen comment with 2 new
  confirmed details. **Reachability, confirmed** (not merely hypothetical):
  `<PROGRAMSMENU>` is confirmed to be an independently optional per-shortcut
  child element — evidenced by `pim_core/pim_core_src/pimShortcutLoop.cxx:88-341`,
  which parses it as one of several independently present-or-absent sibling
  children of `<SHORTCUT>` (a shortcut offered only via Desktop/Quicklaunch
  legitimately has no `<PROGRAMSMENU>` node at all) — so this bug's trigger
  condition is a normal configuration, not a contrived edge case.
  **Downstream consequence, newly traced, concrete and deterministic**
  (unlike the prior extension's document-order-dependent caveat):
  `pimEntitlement::InstallShortcuts()` (`pim_core/pim_core_src/pimEntitlement.cxx:5778-5787`)
  calls `pimShortcuts::GetInstance().Create(xmlPtr)` on the **same**
  `pimXmlFile*` document `pimSilentFixupShortcuts()` mutates, at real
  install-execution time. `pimShortcutLoop::OnInstall()`
  (`pim_core/pim_core_src/pimShortcutLoop.cxx:324-326,654-671`) reads that
  shortcut's `<PROGRAMSMENU>` text and uses it **directly as a filesystem
  subfolder path** to place the installed `.lnk` file. If the stale value is
  non-empty (copied from a different shortcut), the icon is installed into
  *that other shortcut's* Start Menu folder — a visible misplacement; if the
  stale value is empty (the first loop iteration), the Program-Menu
  placement is silently skipped entirely, even if this shortcut's own
  "create in Programs Menu" boolean flag was set `true`. See
  `docs/classes/pimSilent.md`'s Risk Analysis.
- Twenty-third extension, an 8th update to the existing `pimSilent.h` patch
  (parity with the above, added when documenting "`pimIsProductXmlMatch`'s
  structural limitation at the same depth" — a further, dedicated
  re-scrutiny of one already-documented finding, not a new function):
  expanded its Doxygen comment with 2 new confirmed details. **Reachability,
  more precisely characterized**: this codebase's own other 2 product-XML
  schema-resolution call sites (`pim_core/pim_core_src/pimEntitlement.cxx:634-635`,
  resolving a product's own `.p.xml`, tries `NULL`/`EXTERNAL_INSTALLER`/
  `HIDDEN_PRODUCT` in sequence; `:967-968`, resolving a `<PREREQUISITE>`
  reference, tries `NULL`/`EXTERNAL_INSTALLER`) confirm that a product's
  root schema is treated elsewhere in this exact codebase as unpredictable
  and deliberately checked for by trying multiple schemas — direct,
  confirmed evidence that `pimIsProductXmlMatch()`/`pimIsVersionMatch()`'s
  single-schema-for-both-`A`-and-`B` assumption is unsafe by this
  codebase's own design, not merely a hypothetical concern. **Downstream
  consequence, newly traced**: this function's only caller,
  `pimSilentTestXmlIsUseable()`, is itself only called from
  `pimSilentInstallFromXML()`'s per-file loop (`pim/pim_src/pimTop.cxx:1532`),
  which simply skips (no abort, no distinct error) any product whose match
  fails this way. Since `pimSilentInstallFromXML()`'s own final return value
  is `pimGetLastError()` (`pim/pim_src/pimTop.cxx:2073`, `pim/pim_src/pimExit.cxx:16-33`)
  — a process-wide static array's **last** appended error, not a per-file
  record — a mixed-schema false-negative on one product in a multi-XML batch
  silent install can be silently overwritten in the reported exit code by
  any later, unrelated error, losing which specific product failed the
  match and why. See `docs/classes/pimSilent.md`'s Risk Analysis.
- Twenty-fourth extension, a 9th update to the existing `pimSilent.h` patch
  (parity with the above, added when documenting "`pimSilentCanMatchNeeds`'s
  package check mode-dependent finding at the same depth" — a further,
  dedicated re-scrutiny of one already-documented finding, not a new
  function): expanded its Doxygen comment with 2 new confirmed details.
  **Reachability, more precisely characterized**: the 3 mode flags are not
  cleanly mutually exclusive — `-allpacks`/`-basepack` **are** enforced
  mutually exclusive by the CLI parser itself
  (`pim/pim_src/pimGeneralInit.cxx:309-320`, each `else if` branch clears
  the other), but `-releaselink` (`pimGetCreoNGCRIMode()`) has **no such
  interaction with either** — a real command line can freely combine
  `-releaselink -basepack`. Since `GetInstallPackageNames()`'s `if`/`else
  if` chain checks `pimGetBasePackMode()` first, `pimGetCreoNGCRIMode()`
  second, combining them makes BasePack silently win for package selection
  while `pimGetCreoNGCRIMode()` remains fully active for every other
  purpose elsewhere in the codebase (10+ independent call sites in
  `pim_ui`) — a confirmed, reachable interaction via ordinary command-line
  flags, not a hypothetical one. **Downstream consequence, newly traced**:
  whatever this mode-dependent selection resolves to is not merely a
  validation-time concern — `pimPackageMgr::SetPackageInstallState()`
  (`pim_core/pim_core_src/pimPackageMgr.cxx:250-336`) writes it directly
  onto the `<PACKAGE>` node's own `install="Y"`/`"N"` attribute on `Eb`'s
  shared `xmlPtr`. `pimMSILoop::IsEligibleForInstall()`
  (`pim_core/pim_core_src/pimMSILoop.cxx:937-969`) reads that **same**
  attribute at real install-execution time — via
  `pimEntitlement::InstallMSI()` constructing a `pimMSILoop` (already a
  fully documented `pimLoop` subclass) through
  `pimMSICopier::MSIInstall()`/`MSICopy_low()` on this same `xmlPtr` — to
  decide whether a package's MSI feature/CDSECTION is actually, physically
  installed. This confirms the mode-dependent logic is the literal,
  unbroken, real-world determinant of installed product features, not
  merely a validation/documentation-accuracy concern. See
  `docs/classes/pimSilent.md`'s Risk Analysis.
- Twenty-fifth extension, a 10th update to the existing `pimSilent.h` patch
  (parity with the above, added when documenting
  "`pimSilentCreateEntitlement`'s MSI-node last-value-wins caveat at the
  same depth" — a further, dedicated re-scrutiny of one already-documented
  finding, previously flagged only as a lower-confidence structural note,
  not a new function): expanded its Doxygen comment with a confirmed
  upgrade to a full bug plus 2 new confirmed details.
  **Upgraded to CONFIRMED BUG**: the `<MSI>`-node copy loop's read side
  (from `A`) declares `format`/`Cmd` once outside the loop, so every
  iteration silently overwrites them — only the **last** `<MSI>` node in
  `A`'s document order survives — and the write loop then stamps that one
  surviving value pair onto **every** `<MSI>` node in `B`, erasing
  distinctions between `B`'s own, independently-configured MSI packages.
  **Reachability, confirmed** (not merely hypothetical, upgrading the prior
  "no product XML available to test" caveat): `pimMSILoop::pimMSIExec()`
  (`pim_core/pim_core_src/pimMSILoop.cxx:154-161`, the actual MSI
  install-execution driver, already a fully documented `pimLoop` subclass)
  independently iterates **every** `<MSI>` node via the identical
  `getElementsByTagName(pimMSI)` call, skipping only
  `IsEligibleForInstall()` rejects, and executes each eligible node as its
  own separate MSI install action — confirming multi-`<MSI>`-node product
  XML is a real, designed-for, normally-executed configuration, not a
  hypothetical edge case. **Downstream consequence, newly traced**: the
  single `format`/`Cmd` pair stamped onto every `<MSI>` node in `B` is
  exactly what `pimMSILoop::pimMSIExec()` reads **per node** at real
  install-execution time — `attribFormat` (`pim_core/pim_core_src/pimMSILoop.cxx:596-614`)
  directly selects that node's install **UI mode** (`"full"` interactive
  wizard, `"basic"`, or silent), and the `<MSIARGUMENT>` text feeds that
  node's `msiexec.exe` command line. If `B` legitimately has 2+ MSI
  packages meant to install under different UI modes or arguments, this
  bug forces all of them to adopt whichever single mode/argument pair
  belonged to the last `<MSI>` node read from `A` — a confirmed,
  install-time consequential data-corruption bug that can visibly change
  which installer UI a user sees during a multi-package MSI install. See
  `docs/classes/pimSilent.md`'s Risk Analysis.
- Twenty-sixth extension, an 11th update to the existing `pimSilent.h` patch
  (parity with the above, added when documenting "`pimSilentFixupPSF`'s
  collision-deletes-entire-type discrepancy at the same depth" — a further,
  dedicated re-scrutiny of one already-documented finding, not a new
  function): expanded its Doxygen comment with 2 new confirmed details.
  **Reachability, precisely characterized**: this function's own opening
  "Drop" step (`pim_core/pim_core_src/pimSilent.cxx:361-365`) already
  deletes every original command in `B` before the collision-handling block
  runs — but `pimCommandMgr::CanDeleteCommand()`'s live "last of a type"
  count (`pim_core/pim_core_src/pimCommandMgr.cxx:454-515`, counting
  `<PSF>` nodes by `<LICTYPE>` at call time) makes that Drop loop
  deterministically leave **exactly 1 surviving command per originally
  distinct license type**. A given `lictype2`'s **first** collision can
  therefore only ever delete that one straggler — matching the comment's
  singular framing; the discrepancy only actually manifests when 2+
  `A_wants` entries in the same call independently collide against
  different `B` commands that originally shared the same `lictype2`, a
  real but narrower scenario than "any non-colliding command of that type"
  suggests. **Downstream consequence, newly traced** — an internal
  interaction with the already-documented stuck-dummy bug, not an external
  consumer: because each later same-type collision's delete sweep runs
  before its own new dummy exists, it deletes the *previous* collision's
  straggler/dummy while its own dummy survives — an unintentional cleanup
  mechanism for every dummy but the **last** one created per shared
  `lictype2`. This narrows, without retracting, the twenty-third
  extension's stuck-dummy finding: the `pimScriptLoop` `[LM_LICENSE_FILE]`
  corruption risk is confirmed to apply to at most 1 dummy per
  originally-distinct license type per call. See
  `docs/classes/pimSilent.md`'s Risk Analysis.
- Twenty-seventh extension, a 12th update to the existing `pimSilent.h`
  patch (parity with the above, added when documenting
  "`pimSilentCreateEntitlement`'s `<PROPERTY>` skip list at the same
  depth" — a further, dedicated re-scrutiny of one already-documented
  finding, not a new function): expanded its Doxygen comment with a
  confirmed downstream reason for each of the 4 skipped names, plus a
  verified non-issue. **`[SHIPCODE]`**: read immediately after `Eb`'s
  creation by `pimSilentInstallFromXML()` (`pim/pim_src/pimTop.cxx:1556-1559`)
  to populate the session's own `SHIPCODE_PROPERTY` from `Eb`'s own value.
  **`[VERSION]`**: originally set by `pimEntitlement::Init()` itself
  (`pim_core/pim_core_src/pimEntitlement.cxx:944,1195`) from `Eb`'s own
  `<PRODUCT version>` attribute, later read by `pimCustomActionsLoop`
  (`pim_core/pim_core_src/pimCustomActions.cxx:542`) for version-gated
  custom-action behavior and by `pimSessionInfo`
  (`pim_core/pim_core_src/pimSessionInfo.cxx:1801,2316`) for
  cross-entitlement matching. **`[SOURCE]`**: unconditionally overwritten
  anyway moments after this function returns
  (`pim/pim_src/pimTop.cxx:1598`). **`CustomActions`**: gates many real
  install/uninstall lifecycle hook points via
  `pimEntitlement::OnInstallCustomActions()`/`OnUninstallCustomActions()`'s
  `xmlPtr->GetProperty("CustomActions", str)` truthy-check
  (`pim_core/pim_core_src/pimEntitlement.cxx:5151,5882,6024,6669,6753,6837,6842,7023,7159,7228,7309`),
  each constructing a `pimCustomActionsLoop` — a **newly-discovered
  `pimLoop` subclass** (`pim_core/includes/pimCustomActions.h` +
  `pim_core_src/pimCustomActions.cxx`) not previously mentioned anywhere in
  this doc set — on the same `xmlPtr`; the property's mere presence, not
  even its value, gates whether `B`'s own custom-action fixups run at all.
  **Verified non-issue**: the copy loop's `name` variable is declared once
  outside the loop, so a `<PROPERTY>` node lacking a `name` attribute would
  leave `name` stale — but the copy loop's own guard
  (`attribName != NULL`) means 0 copies happen either way, so the stale
  value has no effect, unlike the superficially similar but genuinely live
  bug already documented in `pimSilentFixupShortcuts()`. See
  `docs/classes/pimSilent.md`'s Risk Analysis.
- Twenty-eighth extension, a 13th update to the existing `pimSilent.h`
  patch (parity with the above, added when documenting "`pimIsVersionMatch`'s
  shipcode comparison at the same depth" — a further, dedicated
  re-scrutiny of one already-documented finding, not a new function):
  expanded its Doxygen comment with a confirmed structural gap plus 2
  verified non-issues. **Confirmed, not enforced**: the shipcode check only
  runs if **both** `A` and `B` expose a shipcode
  (`pimEntitlement::GetShipcode()`, `pim_core/pim_core_src/pimEntitlement.cxx:2066-2090`,
  returns `false` if the root `<PRODUCT>` node has neither `appshipcode`
  nor `shipcode`). `pimEntitlement::Init()`
  (`pim_core/pim_core_src/pimEntitlement.cxx:846-870`) only requires
  `tag`/`version` — not `shipcode`/`appshipcode` — so a valid,
  `Init()`-succeeding product XML can legitimately omit both, silently
  bypassing the gate; not confirmed against any specific real
  product/media XML in this archive (none exists), but the mechanism is
  fully confirmed from `Init()`'s own attribute requirements. **Downstream
  consequence, newly traced**: when skipped this way, the function reaches
  `return true;` via the **exact same path** as an explicit shipcode
  match — nothing distinguishes "ran and passed" from "never ran" (no log,
  no warning, no property set), and `pimSilentTestXmlIsUseable()`'s
  differentiated-error logic (`Major`/`Minor`) is only ever inspected on
  the *rejected* path, so it's never consulted when the check was silently
  skipped. **2 verified non-issues**: `Minor` is only set `true` when
  shipcodes are exactly equal, not when `B`'s is legitimately newer (the
  function's own documented common case) or when the check is skipped —
  but since `Major`/`Minor` are only read on the rejected path, this has no
  effect; and `pimCompareShipcode()`'s parameter names
  (`new_ship`/`old_ship`) don't reflect an enforced argument-order
  contract — confirmed via its only other call site
  (`pimEntitlement::GetSize()`) to be a generic, symmetric comparator. See
  `docs/classes/pimSilent.md`'s Risk Analysis.
- Twenty-ninth extension, a 14th update to the existing `pimSilent.h`
  patch (parity with the above, added when documenting
  "`pimSilentCanMatchNeeds`'s platform and language checks at the same
  depth" — a further, dedicated re-scrutiny of one already-documented
  finding, not a new function): expanded its Doxygen comment confirming
  the checks are mode-free but adding a confirmed consistency gap.
  **Confirmed mode-free**: `GetInstallPlatformNames()`/`GetInstallLanguageNames()`
  (`pim_core/pim_core_src/pimPlatformMgr.cxx:253-281`/`pim_core/pim_core_src/pimLanguageMgr.cxx:147-175`)
  are plain, unconditional `install="Y"` checks, no global mode flags for
  either. **Confirmed consistency gap, newly traced**: unlike the package
  check (confirmed by an earlier pass to validate exactly what
  `pimSilentCreateEntitlement()` will select, since both call the
  identical `GetInstallPackageNames()`), platform and language have no
  such guarantee — `pimSilentInstallFromXML()`'s post-creation code
  (`pim/pim_src/pimTop.cxx:1602-1635`, run *after* this validation)
  re-derives what actually gets selected on `Eb` from entirely different
  inputs: platform via `pimPlatformMgr::InitPlatformState(NULL)`
  (`pim_core/pim_core_src/pimPlatformMgr.cxx:104-125`), which uses
  `btkGetPlatform()` — the current machine's own runtime OS — with a
  2-level fallback chain; language via 9 hardcoded
  `SetLanguageInstallState()` calls gated by `pimShouldWeInitLanguageID()`
  (`pim/pim_src/pimGeneralInit.cxx:531-539`), driven by the `-LANG`
  command-line flag list or `-allpacks` (which force-selects all 9) — none
  of which this validation ever sees. **Downstream consequence, newly
  traced**: both `pimTop.cxx` call sites discard their
  `Set*InstallState()` calls' return values, which fail **silently** (no
  log, no error) whenever the target doesn't exist in `B` — a `-lang XX`
  (or `-allpacks`) request for a language `B` genuinely lacks is silently
  dropped with zero diagnostic anywhere in the pipeline, and if the
  auto-detected platform and both fallbacks all fail to match anything in
  `B`, the entitlement silently ends up with no platform marked for
  install at all.
  **CORRECTED (found while documenting the thirtieth extension below, which
  required re-reading `pimSilentCreateEntitlement()`'s full body)**: the
  "lacks the package check's consistency guarantee" framing above was
  **incomplete**. `pimSilentCreateEntitlement()` (`pim_core/pim_core_src/pimSilent.cxx:549-569`)
  **does** mirror `A`'s declared platform/language onto `B` first, via the
  identical `GetInstallPlatformNames()`/`GetInstallLanguageNames()` calls
  this check uses — the same consistency pattern already confirmed for
  package selection. The corrected gap: this internally-consistent
  mirroring is **subsequently undone** by a 2nd, independent write —
  `pimTop.cxx`'s post-creation code overwrites/expands the same 2
  properties afterward using entirely different, never-validated inputs.
  See `docs/classes/pimSilent.md`'s Risk Analysis.
- Thirtieth extension, a 15th update to the existing `pimSilent.h` patch
  (parity with the above, added when documenting
  "`pimSilentFixupPSF`'s `Ea`/`Eb` reference-vs-copy semantics at the same
  depth" — a further, dedicated re-scrutiny of one already-documented
  finding, not a new function): expanded its Doxygen comment with a
  confirmed asymmetric-roles finding, a real-but-unreachable structural
  hazard, and a verified non-issue. **Confirmed asymmetric roles**: both
  parameters are declared as symmetric, non-`const` references, but every
  `Ea`/`A_cmds` call in this function is a read-only accessor
  (`GetCommandInfoByName()`/`GetAllCommandNames()`), never a mutator,
  while `Eb`/`B_cmds` is the sole read-write target
  (`DeleteCommand()`/`AppendAddCommand()`) — `Ea` could safely be declared
  `const pimEntitlement&`. Traced `pimSilentCreateEntitlement()`'s own
  construction of `Ea`/`Eb` (`pim_core/pim_core_src/pimSilent.cxx:507-521,736`):
  `Ea` is a genuinely ephemeral, stack-allocated object destroyed (its
  `xmlPtr` explicitly deleted) the moment `pimSilentCreateEntitlement()`
  returns, while `Eb` is **not a copy at all** — a pointer directly to the
  real, session-owned, persistent object `AddEntitlement(B)` just created,
  mutated by reference — confirming why every already-documented
  downstream consumer (`pimScriptLoop`, `pimShortcutLoop`, `pimMSILoop`,
  etc.) genuinely reads the identical object this function wrote to.
  **Reachability, precisely characterized (a real but confirmed
  unreachable structural hazard)**: `pimEntitlement` owns a raw `xmlPtr`
  pointer, explicitly deleted in its destructor, but defines no custom
  copy constructor or assignment operator — relying on compiler-generated
  (shallow-copy) defaults that would double-free `xmlPtr` if 2
  `pimEntitlement`s ever shared it via a by-value copy. An exhaustive
  search confirms the only 3 stack-allocated, value-type `pimEntitlement`
  instances anywhere in this codebase are all in this same file, and every
  one is confirmed used safely; every other entitlement is held via
  `dsXArray<pimEntitlement*>` or a raw pointer, never by value. **Verified
  non-issue**: although `Ea` is destroyed immediately after this call,
  every value this function copies from it into `Eb` is a genuine value
  copy, not a reference back into `Ea`'s own DOM tree. See
  `docs/classes/pimSilent.md`'s Risk Analysis.
- Thirty-first extension, a 16th update to the existing `pimSilent.h` patch
  (parity with the above, added when documenting
  "`pimSilentCreateEntitlement`'s session-index lookup at the same depth" —
  a further, dedicated re-scrutiny of one already-documented function's own
  mechanism, not a new function): expanded its Doxygen comment with a
  confirmed bug, a reachability characterization, and a related verified
  non-issue. **CONFIRMED BUG**: the `entitlement_index` out-parameter
  (`pim_core/pim_core_src/pimSilent.cxx:520`) is set to a valid,
  non-negative index immediately after `AddEntitlement(B)` succeeds, but
  `A`'s own parseability isn't checked until `:522-524` — if `Ea.Init(A)`
  fails under both schemas, the function returns `false` at `:530` without
  resetting `entitlement_index`. Traced the only caller
  (`pim/pim_src/pimTop.cxx:1534-1540`): this function's `bool` return value
  is never inspected; the caller relies solely on `k`'s sign
  (pre-initialized `-1`, checked via `if (k < 0)`), which cannot catch this
  specific failure mode since `k` is already overwritten to a valid index —
  execution proceeds treating a half-created `B` (added to the session but
  never reaching `pimSilentFixupPSF()`/`pimSilentFixupShortcuts()`) as if
  creation fully succeeded. **Reachability, precisely characterized**: not
  confirmed reachable via this codebase's own single call site —
  `pimSilentTestXmlIsUseable()` (called immediately before, on the
  identical `A` input) already requires `A`'s parse to succeed under one of
  the same 2 schemas before this call is ever reached, making the bug a
  confirmed but currently latent gap in this function's own
  error-handling design. **Verified non-issue, a related but distinct
  mechanism**: `pimSessionInfo::AddEntitlement(cStringT)`'s own
  pre-existing-entry guard compares a filesystem path against `GetID()` (a
  product tag), so it never actually fires — confirming `AddEntitlement()`
  succeeding always means a brand-new array entry was just appended, so the
  session-index lookup's own core arithmetic is otherwise sound. See
  `docs/classes/pimSilent.md`'s Risk Analysis.
- Thirty-second extension, a 17th update to the existing `pimSilent.h`
  patch (parity with the above, added when documenting
  "`pimSilentCanMatchNeeds`'s package check reachability at the same
  depth" — a further, dedicated re-scrutiny of one already-documented
  finding, not a new function): expanded its Doxygen comment with a
  confirmed crash bug and a precisely characterized, mode-dependent
  reachability trace. **CONFIRMED BUG**: traced
  `A_pkg.GetInstallPackageNames()`/`B_pkg.GetAllPackageNames()`
  (`pim_core/pim_core_src/pimSilent.cxx:307-308`) into `pimPackageMgr`
  (`pim_core/pim_core_src/pimPackageMgr.cxx:611-666,550-575`) — neither
  function null-checks `DOMNamedNodeMap::getNamedItem()`'s result before
  calling `->getNodeValue()` on it, for `name` (both functions) and,
  depending on the active mode flag, `install`/`required`/`parent` as well
  (`GetInstallPackageNames()` only). **CONFIRMED DISCREPANCY, by direct
  contrast within the same class**: `pimPackageMgr::GetPackageInfoByName()`
  (`:113-170`), a sibling method in this same file reading the exact same
  4 attributes off the exact same `<PACKAGE>` node type, guards every one
  of them before dereferencing — confirmed evidence this class's own
  author anticipated these attributes can legitimately be absent, making
  the unguarded access an inconsistency, not a deliberate assumption.
  **Reachability, precisely characterized**: `install` is dereferenced
  unconditionally in both default mode and `-allpacks` mode (it's the left
  operand of `||`, evaluated before `pimGetAllPacksMode()` is ever
  checked, defeating the assumption that "install everything" mode
  tolerates a missing `install` attribute); `required` is dereferenced
  unconditionally under `-basepack`/`-releaselink`; `parent` additionally
  under `-releaselink` when `required` isn't `"Y"`; `name` only for a node
  the active mode has already decided to select. Not confirmed reachable
  against a real product/media XML pair (none exists in this archive, and
  no `<PACKAGE>`-node-authoring code exists in this archive either), but
  the crash mechanism and the internal inconsistency are fully confirmed
  from source. Also confirmed this exposure is the **widest** of the 3
  sibling checks: `pimPlatformMgr`/`pimLanguageMgr`'s equivalent functions
  share the same unguarded `name`/`install` pattern (2 vulnerable
  attributes each), but lack package's extra mode-branching (4 vulnerable
  attributes). See `docs/classes/pimSilent.md`'s Risk Analysis.
- Thirty-third extension, an 18th update to the existing `pimSilent.h`
  patch (parity with the above, added when documenting
  "`pimSilentFixupShortcuts`'s stale-Program-Menu-copy fix at the same
  depth" — a further, dedicated pass verifying one already-proposed fix,
  not a new function): expanded its Doxygen comment with a fix
  verification and a newly found, related limitation. **FIX VERIFIED**:
  gating `SetShortcutProgramMenu()` on `GetShortcutProgramMenu()`'s return
  value (mirroring the adjacent `GetShortcutStartDir()`/`SetShortcutStartDir()`
  pattern) is confirmed **sufficient** — traced `SetShortcutProgramMenu()`'s
  own implementation (`pim_ui/pim_ui_src/pimShortcutMgr.cxx:169-189`) to
  confirm skipping the call when `Get` fails leaves `Eb`'s shortcut
  untouched, so its own original template value survives intact,
  eliminating every corruption path in the original finding. **NEW
  LIMITATION, shared by the fix and the pattern it mirrors alike**: neither
  `SetShortcutProgramMenu()` nor `SetShortcutState()`
  (`pimShortcutMgr.cxx:426-454`, the shared primitive behind
  `SetShortcutProgramsMenuState()`'s toggle) nor `SetShortcutStartDir()`
  itself (`:213-233`) can ever create a missing child node — all 3 only
  mutate an existing one. Even with the fix applied, `A` can never grant a
  shortcut a Program-Menu (or start-directory) customization that `B`'s own
  media template never defined in the first place — a silent, non-crashing,
  by-design ceiling, not a defect the fix introduces or leaves unaddressed,
  confirmed consistent with `pimShortcutLoop::OnInstall()`'s own parsing
  simply never matching an absent node. See `docs/classes/pimSilent.md`'s
  Risk Analysis.
- Thirty-fourth extension, a 19th update to the existing `pimSilent.h`
  patch (parity with the above, added when documenting
  "`pimSilentCreateEntitlement`'s quality-agent flag copy at the same
  depth" — a further, dedicated pass on one previously untouched area of
  an already-documented function, not a new function): expanded its
  Doxygen comment with a confirmed silent-override bug, a GUI-side
  contrast, and a traced real-world consequence. **CONFIRMED BUG**: traced
  `Ea.IsQualityAgentEnabled(tf)`/`Eb->SetQualityAgent(tf)`
  (`pim_core/pim_core_src/pimSilent.cxx:593-599`) into
  `pimEntitlement::SetQualityAgent()`
  (`pim_core/pim_core_src/pimEntitlement.cxx:3225-3248`) — enabling always
  succeeds whenever `Eb`'s `<QUALITYAGENT>` node exists, but disabling is
  silently **refused** whenever `Eb`'s own node is marked `required="Y"`,
  a business rule unrelated to `A`'s request; the caller discards this
  return value entirely, so a user's explicit disable request can be
  silently overridden with zero diagnostic. **CONFIRMED, by direct
  contrast with this codebase's own GUI path**:
  `pim_ui/pim_ui_src/pimCustomDlg.cxx:1630-1655` checks
  `IsQualityAgentRequired()` first and disables ("greys out") the
  "QualityAgentOptIn" checkbox control entirely when required —
  structurally preventing a live user from ever attempting the disable
  this function blindly attempts, with no equivalent precondition check in
  the silent-install path. **Reachability**: unlike the package check,
  nothing in this pipeline pre-validates QualityAgent state —
  `pimSilentCanMatchNeeds()` never inspects it — so an older saved request
  XML with quality agent explicitly disabled, applied against a newer
  media definition that has since made it required, silently loses the
  opt-out on reapplication. **CONFIRMED DOWNSTREAM CONSEQUENCE**: `Eb`'s
  final `enable` state is read at real install-execution time by
  `pimEntitlement::OnInstall()`/`OnReconfigure()` to write or remove a real
  `"QualityAgentOptIn"` Windows registry value — the GUI's own "PHM"/legal-text
  framing confirms this is a genuine, user-facing telemetry opt-in setting
  with consent implications, not a latent XML-consistency concern.
  **Verified non-issue**: the enable direction carries no equivalent
  restriction. See `docs/classes/pimSilent.md`'s Risk Analysis.
- Thirty-fifth extension, a 20th update to the existing `pimSilent.h`
  patch (parity with the above, added when documenting
  "`pimSilentFixupPSF`'s `DeleteCommand` return-value check at the same
  depth" — a further, dedicated pass on one already-documented function's
  own error-handling mechanism, not a new function): expanded its Doxygen
  comment with a precise, per-call-site trace of all 3 `DeleteCommand()`
  call sites and a genuine generalization of the already-documented
  stuck-dummy finding. **Site 1, the Drop step (`:364`) — verified
  non-issue**: a `false` return here is the loop's own expected, by-design
  terminal state, already documented by its own inline comment. **Site 2,
  collision-handling (`:391`) — confirmed, by an exhaustive counting
  proof, to always succeed, conditionally**: the freshly `AppendAddCommand()`'d
  dummy (`:389`) guarantees the type's member count stays `>1` through
  every deletion in the loop — provided that `AppendAddCommand()` call
  itself succeeds, which is also never checked, and traced to depend on
  `FindPsfTempateNode()` finding a matching `<PSF_TEMPLATE>` (not confirmed
  either way in this archive); if that precondition failed, the subsequent
  re-add loop would silently leave the wrong license type permanently
  attached to the colliding command, since neither `AppendAddCommand()`
  nor `AppendEditCommand()` ever updates an existing node's `<LICTYPE>`
  child. **Site 3, final cleanup (`:413`) — confirmed, generalizing the
  stuck-dummy finding beyond dummies specifically**: any command — dummy or
  genuinely original — that is the sole surviving representative of its
  license type gets permanently, silently stuck whenever `A_wants`
  contains no command of that type at all, confirming
  `pimSilentFixupPSF()`'s own stated goal of removing every name `A`
  doesn't want is structurally unachievable for such a type, not a corner
  case. A stuck non-dummy leftover carries real, valid (if unrequested)
  license data and can equally reach `pimScriptLoop::OnInstall()`'s
  unconditional `arr[0]` read for `[LM_LICENSE_FILE]`. See
  `docs/classes/pimSilent.md`'s Risk Analysis.
- Thirty-sixth extension, a 21st update to the existing `pimSilent.h`
  patch (parity with the above, added when documenting
  "`pimSilentFixupPSF`'s `AppendAddCommand` return-value check at the same
  depth" — a further, dedicated pass on this function's own error-handling
  mechanism, not a new function): expanded its Doxygen comment with a
  precise, per-call-site trace of both `AppendAddCommand()` call sites,
  mirroring the `DeleteCommand()` pass but finding a more severe, more
  clearly reachable failure mode. **Traced `AppendAddCommand()`'s own
  implementation**: it is add-or-update — an existing `name` is simply
  updated and always succeeds — but creating a genuinely new node depends
  on `FindPsfTempateNode()` finding a matching `<PSF_TEMPLATE>`; if none
  exists, nothing is created and the return is `false` with zero trace
  anywhere. **Site 1 (`:389`, the dummy) — confirmed lower reachability**:
  uses a license type read from an existing live `B` command, plausibly
  (though not certainly) already templated. **Site 2 (`:403`, the main
  re-add loop) — CONFIRMED BUG, more severe and more clearly reachable**:
  when `A_wants[i]` is a name genuinely new to `B`, its license type comes
  from `A`'s own document, entirely independent of `B`'s template set — if
  `B`'s media lacks a template for that type (a plausible
  cross-version/cross-product mismatch), `A`'s entire request for that
  named command is silently dropped, not stuck like a dummy, simply
  absent, with no diagnostic anywhere. **Confirmed by this function's own
  reasoning**: its inline comment introducing this loop shows the author
  reasoned through name/type collisions but never considered template
  availability — a genuine blind spot, not an accepted risk.
  **Downstream consequence**: the failed command never even appears in the
  final cleanup loop's re-fetch, so there is nothing to strand — the
  user's requested licensed feature is simply never provisioned, with no
  dummy, no log, and no error anywhere in the pipeline to indicate the
  omission. See `docs/classes/pimSilent.md`'s Risk Analysis.
- Thirty-seventh extension, a 22nd update to the existing `pimSilent.h`
  patch (parity with the above, added when documenting
  "`pimSilentCreateEntitlement`'s MSI-node fix at the same depth" — a
  further, dedicated pass verifying one already-proposed fix, not a new
  function): expanded its Doxygen comment with a fix verification, a
  confirmed under-specification in the fix's own phrasing, and a newly
  found, related asymmetry. **FIX PARTIALLY VERIFIED**: traced
  `pimMSIExec()`'s own attribute reads
  (`pim_core/pim_core_src/pimMSILoop.cxx:202-207`) off the identical
  `<MSI>` nodelist this function iterates — confirms both `name` and
  `PRODUCTCODE` genuinely exist on real `<MSI>` nodes in this codebase,
  grounding the proposed per-node-identity fix in fact. **But the fix's
  own "match by either" phrasing is under-specified**: `PRODUCTCODE` is a
  Windows Installer GUID conventionally expected to change on most new
  MSI builds — a `PRODUCTCODE`-only match would likely find no
  correspondence at all between `A`'s (older/customized) request and `B`'s
  (newly matched) media in this function's realistic use case, silently
  degrading the fix to "no customization ever survives" — trading the
  confirmed corruption bug for a confirmed loss of the feature's purpose.
  `name` is the practically workable key but isn't schema-enforced unique.
  A robust fix needs both — `PRODUCTCODE` tried first, `name` as
  fallback — a design detail neither the original finding nor its
  proposed fix specified. **New asymmetry found while verifying, orthogonal
  to the name-collapsing bug**: the existing write loop's `format`
  handling (`pim_core/pim_core_src/pimSilent.cxx:654-661`) only overwrites
  `B`'s attribute if it already exists (no creation fallback), while the
  adjacent `<MSIARGUMENT>` handling (`:663-673`) does create a missing
  child — any correctly-scoped per-node fix must consciously preserve or
  correct this too. See `docs/classes/pimSilent.md`'s Risk Analysis.
- Thirty-eighth extension, a 23rd update to the existing `pimSilent.h`
  patch (parity with the above, added when documenting
  "`pimSilentCreateEntitlement`'s PROPERTY skip list fix at the same
  depth" — a further, dedicated pass verifying one already-proposed,
  low-priority hardening fix, not a new function): expanded its Doxygen
  comment with a fix verification and a genuinely new, distinct bug found
  while tracing the loop's full return-value chain. **FIX VERIFIED
  (COSMETIC)**: the proposed fix ("initialize the loop's `name` variable
  fresh each iteration") is confirmed to change nothing observable
  today — traced precisely which guard does the real work: the inner
  for-loop's own `attribName != NULL` condition is the sole,
  always-present safety net for a nameless `<PROPERTY>` node, regardless
  of what the outer skip-list `continue` does with a stale `name`. The
  fix's real value is closing a latent maintenance trap: a plausible
  future refactor weakening the inner guard (mistakenly assuming the outer
  `continue` already handles nameless nodes) would silently reintroduce
  exactly the kind of stale-value corruption already confirmed live in
  `pimSilentFixupShortcuts()`'s Program Menu bug. **CONFIRMED BUG, found in
  the same pass — a new, distinct finding**: `pimXmlFile::GetProperty()`'s
  own return value is *also* discarded at both value-copy sites in this
  loop; if `A`'s document ever has 2+ `<PROPERTY>` nodes sharing the same
  `name` (not schema-prevented in this archive), `GetPropertyNode()`'s
  document-order-first-match search could return a *different* node than
  the one currently being iterated, and a missing attribute there would
  leave `value` stale — silently written to `Eb` regardless. The
  general-attribute branch could avoid this entirely by reading its
  already-in-hand attribute node directly instead of re-searching `Ea`'s
  whole document by name. See `docs/classes/pimSilent.md`'s Risk Analysis.
- Thirty-ninth extension, a 24th update to the existing `pimSilent.h`
  patch (parity with the above, added when documenting
  "`pimSilentFixupShortcuts`'s array index fix at the same depth" — a
  further, dedicated pass verifying one already-proposed fix, not a new
  function): expanded its Doxygen comment with a fix verification, a
  generalized limitation, a verified non-issue, and one adjacent
  pre-existing risk found while tracing the same call chain. **FIX
  VERIFIED (SUFFICIENT)**: replacing `B_avail[i]` with `A_wants[i]` in the
  4 `SetShortcut*State()` calls is confirmed to eliminate the
  wrong-shortcut risk entirely — the enclosing
  `if (B_avail.Find(A_wants[i]) != -1)` guard already proves `A_wants[i]`
  exists in `Eb`'s document, and nothing in this function mutates `Eb`'s
  document structure, so the `B_avail` snapshot stays valid for the
  loop's full duration. **NEW LIMITATION, generalized from an earlier
  pass**: all 4 `SetShortcut*State()` functions, not just
  `SetShortcutProgramsMenuState()` (the only one traced previously),
  delegate to the same shared `SetShortcutState()` primitive, which can
  legitimately return `false` for a correctly identified shortcut whenever
  that shortcut's specific location child element is absent — none of the
  4 call sites check this return value, fix or no fix. **VERIFIED
  NON-ISSUE**: `GetShortcutInfoByID()`'s own discarded return value and its
  once-outside-the-loop output booleans are confirmed unreachable as a
  staleness bug — `A_wants[i]` is sourced from the same document's own
  `GetAllShortcutIDs()` scan, guaranteeing the node is always found, and the
  function pre-initializes all 4 output booleans to `false` before
  scanning children, unlike its `GetShortcutProgramMenu()` sibling.
  **ADJACENT RISK, found in the same trace**: `GetAllShortcutIDs()`
  dereferences its `id`-attribute node with no null check, a latent crash
  if any `<SHORTCUT>` node ever lacks an `id` attribute — unrelated to and
  not introduced by the array-index fix. See `docs/classes/pimSilent.md`'s
  Risk Analysis.
- Fortieth extension, a 25th update to the existing `pimSilent.h` patch
  (parity with the above, added when documenting
  "`pimSilentCreateEntitlement`'s session-index lookup fix at the same
  depth" — a further, dedicated pass verifying 2 already-proposed fixes,
  not a new function): expanded its Doxygen comment with a fix
  verification, a deeper downstream consequence, and a confirmed defect in
  one fix's own literal wording. **FIX VERIFIED (SUFFICIENT, either one
  alone)**: (a) the caller checking this function's own `bool` return
  value in addition to `entitlement_index`'s sign, or (b) this function
  resetting `entitlement_index` to `-1` on its own failure path — confirmed
  there is exactly 1 such path, not several. **NEW DOWNSTREAM CONSEQUENCE,
  traced one step further**: an undetected failure doesn't just make the
  caller "proceed as if creation succeeded" in the abstract — it
  concretely reaches `pimTop.cxx:1777`'s `SessionInfo.Save()`, which
  **persists** the corrupted session document (including the orphaned
  `<ENTITLEMENT>` node) to `sessioninfo.xml` on disk; either fix prevents
  this by triggering the caller's existing early-abort before that line is
  ever reached. **CONFIRMED BUG in fix (b)'s own alternative wording**:
  "call `DropEntitlement(B)` to roll back" is confirmed **non-functional**
  as written — `DropEntitlement(cStringT)` matches by product tag
  (`GetID()`), and `B` is a filesystem path, the identical mismatch already
  confirmed dead code in `AddEntitlement()`'s own guard; a working rollback
  needs `DropEntitlement(entitlement_index)` (the `int` overload) instead.
  **VERIFIED NON-ISSUE**: given the traced consequence, a full rollback
  isn't actually necessary for correctness today — the simpler fix alone
  is fully sufficient, since either one makes the function return before
  `Save()` is ever reached. See `docs/classes/pimSilent.md`'s Risk
  Analysis.
- Forty-first extension, a 26th update to the existing `pimSilent.h`
  patch (parity with the above, added when documenting
  "`pimSilentFixupPSF`'s `Ea` const-reference hardening at the same
  depth" — a further, dedicated pass verifying one already-suggested,
  minor hardening, not a new function): expanded its Doxygen comment with
  a fix verification that found the proposal is not purely mechanical, and
  a confirmed ceiling on its real protection. **FIX VERIFIED (NOT PURELY
  MECHANICAL)**: this function's only direct call on `Ea`, `GetXMLPtr()`,
  is not itself declared `const`, so `const pimEntitlement& Ea` as
  literally proposed fails to compile; `const`-qualifying `GetXMLPtr()`
  itself is confirmed a safe prerequisite (trivial body, zero existing
  `const pimEntitlement` usage anywhere in this codebase to regress).
  **CONFIRMED SCOPE CEILING**: even fully applied, the hardening only
  blocks a non-`const` `pimEntitlement`-level call directly on `Ea` — read
  `pimCommandMgr.h` in full and confirmed **none** of its public methods,
  including the 3 read-only accessors this function actually calls, are
  declared `const`, so `A_cmds` itself could still mutate `Ea`'s document
  with no compiler objection, before or after the fix; confirmed **zero**
  `const pimXmlFile` usage anywhere in this codebase either. The hardening
  is confirmed to be a documentation-only signal at this function's own
  signature, not a real guard against the realistic mistake one layer
  below. See `docs/classes/pimSilent.md`'s Risk Analysis.
- Forty-second extension, a 27th update to the existing `pimSilent.h`
  patch (parity with the above, added when documenting
  "`pimSilentCanMatchNeeds`'s platform and language checks fix at the same
  depth" — a further, dedicated pass verifying 2 already-proposed fixes,
  not a new function): expanded its Doxygen comment with a diagnostic-only
  fix verification, a corrected fallback-branch characterization, and a
  confirmed feasibility with 2 new nuances for the 2nd fix. **FIX (a)
  VERIFIED SUFFICIENT ONLY FOR DIAGNOSABILITY**: logging the silent
  language/platform selection failures makes the outcome visible but
  changes nothing about it — traced the zero-platform case one level
  further into `pimPackageMgr::RefreshAFeatureNode()`, confirming it marks
  **every** platform-scoped `<CDSECTION>`/`<MSI>` feature `install="N"`
  when no platform is ever selected, a near-total failure logging alone
  cannot prevent. **CORRECTION**: `InitPlatformState()`'s fallback is a
  mutually exclusive, conditionally-chosen **single** fallback, not both
  names tried in sequence. **FIX (b) CONFIRMED ARCHITECTURALLY FEASIBLE**
  for both language and platform (resolving the original "if feasible"
  hedge) — but confirmed a validating copy must exactly replicate the
  corrected fallback branch to avoid false-negative rejections, and would
  create a 2nd, driftable copy of the same selection rule already live in
  `pimTop.cxx`; also confirmed a genuine, unaddressed policy question —
  whether a machine merely lacking the auto-detected platform should have
  its install rejected outright or proceed with that piece silently
  missing, as today. See `docs/classes/pimSilent.md`'s Risk Analysis.
- Forty-third extension, a 28th update to the existing `pimSilent.h`
  patch (parity with the above, added when documenting
  "`pimSilentFixupPSF`'s dummy-placeholder fix at the same depth" — a
  further, dedicated pass verifying one already-proposed, 2-option fix,
  not a new function): expanded its Doxygen comment with a split
  sufficiency verdict for option 1, a precedent-but-under-specified
  verdict for option 2, and a further confirmation deepening an
  already-flagged reachability uncertainty. **OPTION 1 ("guarantee no
  dummy survives") CONFIRMED ALREADY TRUE IN HALF THE CASES, STRUCTURALLY
  UNACHIEVABLE IN THE OTHER HALF**: when another `A`-wanted command
  independently shares the dummy's license type, today's code already
  cleans it up correctly; when none does, `CanDeleteCommand()`'s absolute
  floor makes it structurally impossible to remove via any operation
  ordering, requiring a genuinely new in-place "retype" `pimCommandMgr`
  capability (reusing `AppendAddCommand()`'s own template-cloning
  machinery) rather than a change to `pimSilentFixupPSF()` itself.
  **OPTION 2 ("select by type/role, not array position 0") CONFIRMED
  ARCHITECTURALLY PRECEDENTED, BUT CONCRETELY UNDER-SPECIFIED**: found
  `pimScriptLoop::OnInstall()` already performs exactly this kind of
  role-based lookup a few lines later, for a different command
  (`id="parametric"`) — but no canonical selector value for "the
  license-bearing command" is confirmed to exist anywhere in this
  archive. **Further confirmed**: `AppendEditCommand()` never creates a
  missing `<FEATURE_NAME>` child, only updates an existing one, deepening
  the doc's already-flagged uncertainty over whether the dummy can even
  reach the array `pimScriptLoop` reads from. **New, dummy-independent
  design smell**: `arr[0]`'s "just take the first command" assumption is
  fragile on its own terms, since this same function already relies on
  multiple, role-distinct PSF commands coexisting. See
  `docs/classes/pimSilent.md`'s Risk Analysis.
- Forty-fourth extension, a 29th update to the existing `pimSilent.h`
  patch (parity with the above, added when documenting
  "`pimSilentCreateEntitlement`'s package selection at the same depth" —
  a fresh, dedicated pass on a previously-uncovered part of an
  already-documented function): expanded its Doxygen comment with a new
  confirmed bug directly analogous to the already-documented quality-agent
  finding, plus a wasted-computation observation and a cross-referenced
  crash risk. **CONFIRMED BUG**: `SetPackageInstallState(name, false)`
  silently refuses — leaving the package's `install` attribute
  unchanged — whenever that package is `required="Y"` or already
  `installed="Y"` (except `"prime_converter"`), and this function
  discards the return value at both call sites — the identical shape as
  the quality-agent bug, confirmed by direct contrast with
  `uiCustomTree::RefreshPkg()`'s own proactive checkbox-disabling
  precondition check for the identical `req || installed` condition.
  **New nuance**: 2 hardcoded package names still have their global mode
  flags toggled to the refused value regardless, diverging from the
  unchanged XML attribute. **Verified non-issue, confirmed wasteful**:
  this function's own `Refresh()` call computes feature-install flags
  using platform/language values `pimTop.cxx` immediately overwrites and
  recomputes afterward — 100% superseded, harmless. **Cross-referenced**:
  `GetAllPackageNames()` shares `GetInstallPackageNames()`'s already-
  confirmed unguarded null-attribute crash risk. See
  `docs/classes/pimSilent.md`'s Risk Analysis.
- Forty-fifth extension, a first update to the existing `pimSessionInfo.h`
  patch (added when documenting "pim_ui's remaining gaps" — a dedicated
  pass on `pimSessionInfo::TryAuthorize()`, `pim_core_src/pimSessionInfo.cxx:1911-1984`):
  replaced its minimal 2-line Doxygen comment with a full description of
  the retry loop, its call chain (`pimNonBrowserAuthenticationCB()`,
  `pimAuthorizeToPTC()`), and its 3 real callers (`GetTrialLicense()`,
  `GetSchoolLicense()`, `GetBetaLicense()`). **CONFIRMED BUG, HIGH
  SEVERITY**: the loop has no handling at all for `pimAuthorizeToPTC()`
  returning `false` (a transport/network failure) — `do_continue` stays
  `true` and the loop spins forever re-prompting for credentials it never
  re-validates against the server, confirmed by direct contrast with
  `pimGetAvailable::GetSecurity()`'s handling of the identical failure,
  which aborts with `-3`. **CONFIRMED BUG**: a matching `pimXmlFile` leak
  — `pimAuthorizeToPTC()` unconditionally overwrites `*out` with a freshly
  allocated `pimXmlFile` without freeing the previous one, on every loop
  iteration where the server rejects the credentials. **Confirmed
  discarded-return-value / stale-reused-variable pattern** (the same
  template already found repeatedly elsewhere this session):
  `pimAuthorizeFailedMsgDirectCall(out, str)`'s return value is discarded
  and `str` is read afterward regardless. **Confirmed credential
  aliasing**: `SetSecurity()`/`GetSecurity()`/`DelSecurity()`'s own
  `// HACK` comment maps the media URL and the PTC.com production URL to
  the same `SecurityArray` slot, so this function's credential lookups
  alias (not merely mirror) `pimGetAvailable::GetSecurity()`'s. See
  `docs/classes/pimSessionInfo.md`'s Risk Analysis.
- Forty-sixth extension, a BRAND-NEW patch for a previously-uncovered class,
  `pimTranslateMgr` (`pim_core/includes/pimTranslateMgr.h`, 32 lines;
  `pim_core/pim_core_src/pimTranslateMgr.cxx`, 129 lines — both read in full
  for the first time, previously cited only via call-site signatures):
  added a full class-level comment plus per-method Doxygen on
  `TranslateFrom(pimXmlFile*)`/`TranslateFrom(btkFSEntry&)`/`FindNode()`.
  **CONFIRMED BUG**: `TranslateFrom(pimXmlFile*)` guards computing its
  `filename` member on `ptr->getFilePath()` (the TRANSLATION document) but
  assigns the value from `xmlProduct->getFilePath()` (the PRODUCT document
  — a different object) — checking one object's path presence, then
  unconditionally dereferencing the other, unguarded. **CONFIRMED
  REACHABLE** via 2 of `pimEntitlementTree::OnOptionMenuSelect()`'s 3
  near-identical call sequences: `eptr_new_xml` there comes straight from
  `pimGetMediaDetails::GetProductXml()` (confirmed to never call `SetXml()`
  internally) and, when `pimEntitlement::Init()` takes its
  `DownloadXmlBackups` early-return branch (an already-seen MediaID), never
  gets a file path set at all before reaching this method — every
  `file`-scoped `<TRANSLATION>` entry is then silently skipped for those 2
  arms, while the function's 3rd, correctly-wired arm is unaffected — a
  within-function asymmetry matching this session's repeatedly-confirmed
  "sibling code blocks, not all correctly wired" pattern. **CONFIRMED, dead
  code containing a compile-breaking typo**: one of this class's 8
  constructor call sites, `pim_core/pim_core_src/pimEntitlement.cxx:995`,
  reads `Eptr->GetXMLPtr()` — `Eptr` (lowercase `p`) is declared nowhere in
  that function, file, or the entire `pim_core` module (only `EPtr`,
  capital `P`, exists) — but is confirmed permanently dead: its guarding
  `#ifdef PIM_TRANSLATE_XML` can never be true in this translation unit,
  since the macro is `#define`d in exactly one place in this whole archive
  (`pim/pim_src/pimTop.cxx:185`, a different unity-build target that
  `pim_core`'s own unity build never `#include`s). A 3rd finding:
  3 further call sites inside `pim_ui_src/pimInstallMgrActions.cxx`'s
  already-documented `#if 0`-disabled `uiApplicationsList` class are dead
  for the reason already on record in `docs/classes/pimGetMediaDetails.md`.
  See `docs/classes/pimTranslateMgr.md`'s Risk Analysis.
- Forty-seventh extension, an update to the existing `pimEntitlement.h`
  patch (added when documenting "OnReconfigure/OnRollback at the same
  depth" — both previously flagged as not traced in
  `docs/04_installation_flow.md`): replaced `OnReconfigure()`'s and
  `OnRollback(bool)`'s minimal Doxygen comments with full descriptions of
  each function's confirmed stage order. **CONFIRMED BUG, HIGH SEVERITY**:
  `OnReconfigure()`'s very first block unconditionally `Erase()`s the
  entitlement's own cached XML file (`pimGetAppData/pim/<name>.xml`) purely
  as a side effect of extracting a filename for an unrelated path (the
  original as-shipped `.p.xml` used for `RollbackMe`) — there is no
  functional need to delete the file just to read its own filename. 2
  confirmed early-return paths (a service-stop failure, or a
  `PreReconfigure` custom-action failure) leave the deleted cache file
  permanently un-recreated, since neither calls `xmlPtr->DoSave()` first.
  Confirmed by direct contrast: `OnRollback()`'s own mirror-image
  cache-relocation block has no such `Erase()` side effect. Also confirmed,
  resolving the doc's own open questions: `OnRollback()`'s untraced middle
  section mirrors `OnInstall()`'s stage list in reverse exactly as
  speculated, has exactly 2 `from_during_install`-specific branches, and has
  zero early-return points of its own. See
  `docs/classes/pimEntitlement.md`'s Risk Analysis.

## Scope

Comments were added only where the corresponding class doc had verified,
source-grounded evidence for the behavior (per the "prefer implementation over naming
assumptions" / "no invented behavior" rules). Coverage per the Phase 6 spec, applied
identically to both extension passes:

- Classes/structs/enums: `pimLoop`, `pimCopyLoop`, `pimXmlFile` (+ its nested
  `DOMTreeErrorReporter`, `StrX`, `DOMPrintErrorHandler`, `DOMPrintFilter`),
  `pimSessionInfo` (+ nested `pimSecurity`, `LicenseStatus` enum), `pimEntitlement`
  (+ nested `pimEntDlBackup`, `CustomizeAttribs` enum), `pimSFXLoop`, `pimScriptLoop`,
  `pimShortcutLoop`, `pimServiceLoop`, `pimPsfLoop`, `pimDownloadLoop`, `pimCopier`,
  `pimMSICopier`, `pimSFXCopier`, `pimShortcuts`, `pimRegEdit`, `pimServices` (+
  nested `ServiceAction` enum), `pimDownloader` (+ its file-scope `pimDownloadData`
  helper class and free functions `pimSetTimeouts`/`pimGetTimeouts`/
  `pimSendNRecvInit`/`pimSendNRecvFromPTC`/`pimRecvFromPTC`), `pimRegEditLoop`,
  `pimMSILoop`, `pimShortcutMgr`, `pimInstallMgrDlg` (+ its 10 nested UI
  helper classes: `uiLSTInputPanel`, `uiLicenseSummaryTable`,
  `EntitlementItem`, `uiInstallationTable`, `uiAppStatus`,
  `uiCustomAppTree`, `uiCustomApplicationList`,
  `pimCustomSimulateLicenseTable`, `pimCustomMiscTable`,
  `pimCustomCommandTable` — each given a brief one-line class-level comment
  only, not full per-method annotation, per the Scope note in
  `docs/classes/pimInstallMgrDlg.md`; **`uiCheckButtonCell` is the
  exception** — its implementation lives entirely in
  `pim_ui_src/pimEntitlementRefresh.cxx`, read in full in a later pass, so it
  received full per-method annotation, including its confirmed-dead-code
  status, same as the primary classes below), `pimCustomDlg` (+ its 5 nested helper
  classes `uiCustomTree`, `uiCustomAppList`, `pimSimulateLicenseTable`,
  `pimMiscTable`, `pimCommandTable` — all given full per-method annotation,
  the header being small enough for complete coverage unlike
  `pimInstallMgrDlg.h`), `pimEntitlementTree` (all members and methods
  annotated, full coverage), `pimFrictionlessTrialDlg` (all members and
  methods annotated, full coverage), `rpimDlg` (all members and methods
  annotated, full coverage), `pimAuthDlg` (+ its 2 free functions
  `pimInitAuthenticationCB`/`pimNonBrowserAuthenticationCB`, all fully
  annotated), `pimGetAvailable` (a 10th, previously uncatalogued `pimLoop`
  subclass — see below) plus its companion header's `pimAvailableProduct`/
  `pimGetAvailableProducts` (all members and methods annotated, full
  coverage on both headers), `pimGetMediaDetails` (all members and methods
  annotated, full coverage), `pimGetApplicationsList` (**not a class** —
  a free-function subsystem in `pim/includes/pimGeneralInit.h`; only the 4
  declarations directly traced were annotated — `pimCommandLineArgs`,
  `pimAddApplicationsToList`, `pimGetApplicationsList`,
  `IsApplicationInInstallList` — the file's many other unrelated functions
  were left uncommented), and `pimSilentTestXmlIsUseable` (**not a class**
  — a 7-function free-function subsystem in `pim_core/includes/pimSilent.h`;
  all 7 declarations annotated, small enough for complete coverage).
- Public and protected methods on all 30 primary classes, plus the 4 directly
  traced free functions in `pimGeneralInit.h` and all 7 free functions in
  `pimSilent.h` (see above for
  `pimInstallMgrDlg`'s partial-coverage exception; `pimCustomDlg`,
  `pimEntitlementTree`, `pimFrictionlessTrialDlg`, `rpimDlg`, `pimAuthDlg`,
  `pimGetAvailable`, and `pimGetMediaDetails` received full coverage).
- Important macros: the `*_PROPERTY` session-property key `#define`s in
  `pimSessionInfo.h` (a representative subset was annotated where the key's purpose
  was independently confirmed via call sites in `pim/pim_src/pimTop.cxx`; the rest
  were left uncommented rather than guessed).
- Private members/methods were **not** commented, per the Phase 6 spec (only public
  members and important macros are in scope; private implementation detail is not) —
  applied consistently even where a private member is structurally significant (e.g.
  `pimServices::serviceAction`), in which case the significance is instead documented
  on the class-level comment and the accessor methods that touch it.
- Class-level doc comments on the 6 Loop-subclass extension headers record their
  singleton-vs-transient ownership model (`pimSFXLoop`/`pimShortcutLoop`/
  `pimServiceLoop` owned by a process-wide singleton wrapper; `pimScriptLoop`/
  `pimPsfLoop` created directly by `pimEntitlement`, per call, with no singleton
  wrapper) and any confirmed behavioral quirks (e.g. the SFX-vs-MSI exit-code 1618
  divergence on `pimSFXLoop`, the digital-signature-check no-op on
  `pimDownloadLoop`, the `pimServices::serviceAction` static-field coupling on
  `pimServiceLoop`).
- Class-level doc comments on the 7 owner-wrapper extension headers record each
  class's locking contract and any confirmed divergence from the group norm — most
  notably `pimServices`' confirmed early-`xmlMutex.Unlock()` race condition (the
  single highest-severity finding of this documentation effort until superseded —
  see below), the shared `pimOKToRunMsiexec()`/`pimFreeMsiexec()` gate coupling
  `pimMSICopier` and `pimSFXCopier` together, and `pimRegEdit.h`'s confirmed stale
  "these WAIT..." comment that contradicts its actual non-blocking `TryLock()`
  implementation — all traceable to the evidence in the matching `docs/classes/*.md`
  file.
- The `pimRegEditLoop.h` class-level doc comment records a **confirmed double-
  increment bug** in `OnRollback()`'s node-list loop (both the `for` statement's own
  `index++` and an explicit `item(index++)` call), which silently skips every other
  `<REGISTRY>` entry on every pass — contrast with the correctly single-incrementing
  equivalent loop in `OnInstall()` in the same file. This was the single
  highest-confidence, highest-impact finding across the documentation effort at the
  time it was found — see below for what superseded it. See
  `docs/classes/pimRegEditLoop.md`.
- The `pimMSILoop.h` class-level doc comment records **two further confirmed
  bugs**, the most significant found in the Loop-subclass family: (1)
  `OnInstall()`'s `fallback_to_msiexec` flag is checked at the end of **every**
  loop iteration (not once after the loop) and is never reset once set, so a
  single unparseable `<MSI>` node causes every remaining node's iteration to
  re-invoke `pimMSIExec()` — which re-scans and can silently **re-install
  already-installed packages**, an O(N²) redundant-reinstall risk on legacy
  product XML; and (2) the `same_msihybrid` skip logic only logs a debug
  message and has **no actual effect** on which MSIs get installed — the real
  install loop never checks it. See `docs/classes/pimMSILoop.md`'s Risk
  Analysis.
- The `pimShortcutMgr.h` class-level doc comment (`pim_ui/includes/`) records
  a **confirmed coverage gap** in `AreChangesPending()` — it can only detect a
  *changed* shortcut (one whose id exists in both compared instances), not one
  *added or removed* entirely between them, with a confirmed real consequence
  in `pimEntitlement.cxx`'s update/reconfigure decision logic — plus a
  confirmed caller-side indexing bug found in `pimSilent.cxx`'s
  `pimSilentFixupShortcuts()` while tracing this class's usage. See
  `docs/classes/pimShortcutMgr.md`'s Risk Analysis.

- The `pimInstallMgrDlg.h` class-level doc comment records **the largest set of
  confirmed findings in a single class in this documentation effort**: a
  guard-condition asymmetry between `StepForward()`/`StepBack()` around the
  Customize Apps step; `UpdateParentEntitlements()`/`UpdateChildEntitlements()`
  duplicated near-verbatim in the nested `uiCustomAppTree` class in the same
  file; `RefreshFeatureTab()` a permanent no-op despite being dispatched on
  every relevant tab selection; the Help/Advanced customize tab deliberately
  disabled via two independently commented-out call sites;
  `SetPortError()`'s `PORT_OUT_OF_RANGE_ERROR`/`PORT_INVALID_FORMAT_ERROR`
  sharing one message id; `OnClose()`/`OnFinishFromEula()`/
  `OnFinishFromBetaEula()` as three independently-written, near-identical
  exit paths; and a separate standalone class `pimCustomDlg` implementing a
  near-identical method-name set for the same customize feature set — this
  README previously described `pimCustomDlg` as "likely superseded"; that
  was **corrected** when `pimCustomDlg` itself was documented at full depth
  (see below) and confirmed to be live, not dead. See
  `docs/classes/pimInstallMgrDlg.md`'s Risk Analysis.

- The `pimCustomDlg.h` class-level doc comment records the **correction**
  of the "likely superseded" speculation above, plus its own confirmed
  findings: `SetSABPath()`/`DisableSAB()`/`EnableSAB()` (defined in the
  shared `pim_ui_src/pimSAB.cxx`) all guard on `main_dlg_sab_handle` — a
  `static` owned by `pimInstallMgrDlg`, not this class's own
  `help_dlg_sab_handle` — a confirmed copy-paste error currently masked
  only because `pimInstallMgrDlg::InitSAB()` has always already run by the
  time either of this class's 2 confirmed callers can reach `Display()`;
  `pimEntitlementTree`'s per-row "customize" icon resolves the specific
  `pimEntitlement` for that row but never passes it through (`Initialize()`
  has no parameter for a target entitlement), so the dialog always opens on
  the first eligible product rather than the one clicked; and extensive
  structural duplication with `pimInstallMgrDlg`'s own embedded Customize
  tab-set, down to 3 renamed-but-otherwise-identical nested table classes.
  See `docs/classes/pimCustomDlg.md`'s Risk Analysis.

- The `pimEntitlementTree.h` class-level doc comment records 2 confirmed
  bugs: `OnOptionMenuSelect()` dereferences `eptr_new_xml` via `->SetXml()`
  immediately after an `if (eptr_new_xml)` guard that covers only the 2
  preceding lines — a null-pointer-dereference crash risk when
  `pimGetMediaDetails::GetProductXml()` returns `NULL` for the selected
  media, the highest-severity finding in this class; and
  `ReconfigureDisplay_low()`'s `only_one` branch strips one character too
  many from the computed family-group node id (`"_Creo_app"` → `"_Cre"`
  instead of `"_Creo"`), confirmed by contrast with the correctly-computed
  sibling extraction in `UpdateGroupStatus()` in the same file — reachable
  whenever a reconfigure session has exactly 1 entitlement. This pass also
  confirmed `pimEntitlementTree` is the 2nd call site for `pimCustomDlg`
  (via its per-row "customize" icon), and surfaced a previously-uncatalogued
  3rd `.cxx` file for `pimInstallMgrDlg`'s own method bodies,
  `pim_ui_src/pimEntitlementRefresh.cxx` (not itself traced in depth). See
  `docs/classes/pimEntitlementTree.md`'s Risk Analysis.

- The `pimFrictionlessTrialDlg.h` class-level doc comment records the
  **single highest-severity confirmed finding among the `pim_ui` classes
  documented so far**: for the only confirmed real usage of this class
  (`pimTop.cxx`'s `pimFrictionlessTrialRun()`, which always sets
  `pimGetInHouseMode()` to 1 or 2 before calling `Display()`), `Display()`
  calls `FrictionlessLicenseGenerate()` (which starts a background
  license-retrieval worker and arms a polling timer) and then blocks on a
  raw `Sleep(60000)` instead of calling `uiDialog::Activate()` — so the
  timer never fires, so `OnTimerExpired()` (the ONLY call site anywhere in
  this codebase for `pimFrictionlessTrialLicenseGet::HeartBeat()`, confirmed
  by an archive-wide grep) never runs. `HeartBeat()` is where the retrieved
  license is actually saved to disk and the local PSF is updated — none of
  that completion logic ever executes in the confirmed real usage, and the
  worker object itself leaks (only `OnTimerExpired()` frees it). Also
  confirmed dead: a reentrancy guard in `OnTimerExpired()` that is checked
  but never set true, plus `SetURL()`/the `URL` member, `GetAuth()`, and the
  private `Refresh()` method, all with zero confirmed callers. See
  `docs/classes/pimFrictionlessTrialDlg.md`'s Risk Analysis.

- The `rpimDlg.h` class-level doc comment records this pass's main value:
  a **direct, confirmatory comparison** against `pimFrictionlessTrialDlg`,
  the closest sibling class in this codebase (structurally near-identical,
  3 years older). `rpimDlg::Display()` correctly calls
  `uiDialog::Activate()` unconditionally — proving by contrast that
  `pimFrictionlessTrialDlg::Display()`'s `Sleep(60000)` bug is a genuine,
  avoidable defect rather than a necessary consequence of the shared
  design. This pass also confirmed `rpimDlg` shares 2 of
  `pimFrictionlessTrialDlg`'s smaller confirmed-dead-code findings
  (`SetURL()`/the `URL` member/`GetAuth()` with zero callers; the same
  dead `static in_timer` reentrancy guard shape in `OnTimerExpired()`) but
  NOT all of them — `rpimDlg::Refresh()` is confirmed actively used (unlike
  `pimFrictionlessTrialDlg::Refresh()`), a useful negative-comparison
  reminder that structurally similar sibling classes are not uniformly
  vestigial in the same ways. Also identified `pimAuthDlg` as the likely
  original template both `rpimDlg` and `pimFrictionlessTrialDlg` were
  partially copied from (it alone fully wires up the shared `RetryMsg`
  member the other 2 declare but never use). See
  `docs/classes/rpimDlg.md`'s Risk Analysis.

- Eighth `pim_ui` extension, an update to the existing `pimInstallMgrDlg.h`
  patch (parity with the above, added when documenting "pimEntitlementRefresh.cxx
  at the same depth" — the previously-uncatalogued 3rd `.cxx` file for this
  class's method bodies, flagged not-traced when discovered and now read in
  full): added a fully-annotated class comment plus per-method Doxygen on
  `uiCheckButtonCell` (the one nested helper class this file implements) and
  on the `EntitlementRefresh()` family
  (`EntitlementRefresh`/`EntitlementRefresh_low`/`EntitlementRefresh_update{Customize,Space,Nextbtn,Qagent}`/
  `IsSufficientSpace`/`EntitlementDownloadPreAction`/`ApplicationsNextPreAction`/
  `CustomizeNextPreAction`). This pass's headline finding: 2 of the file's 3
  near-identical trial-license polling loops (School/Beta mode) use
  `while (test == NULL || ++max >= 40)` instead of the correct sibling's
  `&&`/`<=` — because `||` short-circuits, the retry counter never advances
  while the license fails to parse, so the loop never terminates. Also
  confirmed: `uiCheckButtonCell`'s entire implementation is dead code (its
  only would-be caller lives inside a class-wide `#if 0` block); a missing
  `else` in `IsSufficientSpace()` silently discards 2 special-case size
  calculations; and `PIM_HIDE_CUSTOMIZE_SCREEN` is checked with directly
  contradictory effect (hide vs. show/enable) across different branches of
  the same file. See `docs/classes/pimInstallMgrDlg.md`'s Risk Analysis.
- Ninth `pim_ui` extension, an update to the existing `pimInstallMgrDlg.h`
  patch (added when documenting "pim_ui's remaining gaps" — a dedicated
  pass on `OnPushButtonActivate(uiPushButton&)`,
  `pim_ui_src/pimInstallMgrActions.cxx:643-1193`, the file's ~550-line main
  wizard-button dispatch handler, previously flagged as not traced to the
  same depth as the rest of the class): added a full Doxygen comment
  describing its 2 dispatch chains and common tail. **CONFIRMED BUG, HIGH
  SEVERITY**: the EULA screen's arm calls `EulaNextPreAction()` (kicks off
  license acquisition) *before* checking whether the user declined the
  license — the reverse of the Beta EULA arm's correct decline-checked-
  first order. **Confirmed by control-flow trace**: `OnFinishFromEula()`/
  `OnFinishFromBetaEula()` both end with `uiDialog::Destroy()`, but their
  call sites here have no matching `return`, so execution falls through to
  the common tail's `Refresh()` call on an already-destroyed dialog;
  severity hedged since it depends on `uiDialog`'s own post-`Destroy()`
  semantics, outside this archive. **Confirmed**: answering "No" to the
  exit-confirmation prompt leaves `NextStepBtn` disabled although the
  comment beside that branch reads "we are exiting" — the label is on the
  non-exiting branch. **Confirmed re-entrancy**: `pimTextDlg`'s "proceed
  anyway" response re-enters this same method via
  `GetMainUIDialog()->OnPushButtonActivate(NextStepBtn)` after setting
  `skip_ports_modification="Y"`, which is never reset. Also confirmed a
  handful of lower-severity findings: chain 2's missing final `else`;
  `last_license_btn` set-order asymmetry; asymmetric undo of `mCmdEdt`; a
  `currentProduct` null-deref risk; `MultipleHostIDButton`'s array-size
  mismatch; the busy cursor never being cleared on some paths; dual
  reconfigure-mode-read sources; and `ResetPort`/`EportReset` covering only
  2 of the 3 ports. See `docs/classes/pimInstallMgrDlg.md`'s Risk Analysis.
- The `pimAuthDlg.h` class-level doc comment records this pass's completion
  of the 3-class dialog family investigation: `pimAuthDlg` (created 2011,
  the oldest of the 3, and called directly from `pim_core` —
  `pimSessionInfo.cxx`, `pimGetAvailable.cxx` — unlike its 2 derivative
  classes) is a heap-allocated, process-wide singleton (`AuthDialog`)
  confirmed to be genuinely REUSED — `Initialize()`/`Display()` invoked
  multiple times on the same instance within one process, via
  `pimSessionInfo::TryAuthorize()`'s confirmed `do`/`while` retry loop and 2
  further confirmed `pim_core`/`pim_ui` call paths. **CONFIRMED STRUCTURAL
  GAP**: `Initialize()` has no idempotency guard — exactly the one class in
  this family that needed one, since the other 2 are each constructed fresh
  exactly once per process. Also confirmed: the `URL` member is write-only
  even in this original template (refining, not contradicting, the earlier
  findings on its 2 copies — it is `SetURL()` the *method* that stopped
  being called in the later copies, not a previously-useful member becoming
  useless); `GetAuth()`'s `bool` return value is always `false` and ignored
  by every confirmed caller; and a commented-out `//AuthDialog->SetBadInputs();`
  reveals a confirmed never-implemented retry-feedback feature (no
  `SetBadInputs()` method exists anywhere in this codebase). See
  `docs/classes/pimAuthDlg.md`'s Risk Analysis.
- The `pimGetAvailable.h` class-level doc comment (`pim_core/includes/`)
  records a **correction to this entire documentation set**: `pimGetAvailable`
  (`class pimGetAvailable : public pimLoop`) is a 10th confirmed `pimLoop`
  subclass, previously mentioned only in passing in `docs/modules/pim_core.md`
  and never counted in any "N Loop subclasses" tally across this project's
  many deliverables. Discovered while tracing
  `pim_ui_src/pimEntitlementRefresh.cxx`'s `EntitlementDownloadPreAction()`
  (the web-media Applications-screen download-availability search). Confirmed
  findings: 2 memory leaks in `OnExecute()` (`ImageXml` never freed on the
  success path; `ProductDefinitionXml` never freed on 2 filtered-out paths);
  `auth_status`'s declaring comment, `// -3 abort, 0 success`, is wrong (`0`
  is set on authentication *failure*, not success); `IsAuthorized()` has zero
  callers anywhere in this archive; and a cross-module raw-pointer alias in
  `pim_core/pimLocate.cxx` (`static pimGetAvailable *AvailableDownloads`, set
  via `LocateAllowPTCDotCom()`) — the same architectural pattern already
  flagged as a live bug for `pimSAB.cxx`'s statics, here confirmed currently
  safe given the exact call sites that exist today, but with no structural
  safeguard against a future change breaking that invariant. See
  `docs/classes/pimGetAvailable.md`'s Risk Analysis.
- The `pimGetMediaDetails.h` class-level doc comment (`pim_core/includes/`)
  records findings on the shared media-details cache/fetcher singleton
  `pimGetAvailable` itself depends on, and which is called from at least 7
  confirmed files across `pim_core` and `pim_ui`. Confirmed: `GetDetails()`'s
  cache-hit fast path reads its shared cache array with **no lock at all**,
  while every mutating path holds `Mutex` — an incomplete-locking bug in a
  singleton called from both the UI thread and at least one background
  `pimLoop` thread; `GetProductXml()`'s timeout-retry path refreshes the
  cache but then retries with the stale, pre-refresh URL string, confirmed
  by direct contrast with the correct sibling pattern in
  `pimEntitlement::UpdateMediaUrls()`; a 2nd, likely-redundant `Replace()`
  call on that same retry path whose severity can't be fully resolved in
  this archive (depends on an external `dsXArray::Replace()` ownership
  contract); and confirmation that `GetProductXml()`'s `NULL` returns are
  the exact source of the already-documented null-pointer-dereference bug in
  `pimEntitlementTree::OnOptionMenuSelect()`, plus a confirmed-dead
  near-duplicate of that bug inside the `#if 0`-disabled
  `uiApplicationsList::OnOptionMenuSelect()`. See
  `docs/classes/pimGetMediaDetails.md`'s Risk Analysis, and the enrichment
  note now added to `docs/classes/pimEntitlementTree.md`'s Risk Analysis.
- The `pimGeneralInit.h` annotations (`pim/includes/`, only 4 of its many
  declarations) record `pimGetApplicationsList` and its 2 sibling free
  functions' findings — **not a class**, see
  `docs/classes/pimGetApplicationsList.md`. Confirmed: the shared
  `applications_list` static is populated in 2 incompatible formats (bare
  tags via `-APPLICATIONS`; full filesystem paths via `-XML`/`-XMLALL`, and
  unconditionally on every silent install via
  `AddMandatoryAppsForSilentInstall()`), with no record of which populated
  a given entry — 2 of its 3 confirmed consumers filter via exact match and
  can never recognize a path-format entry, while only
  `IsApplicationInInstallList()`'s substring search is format-agnostic. More
  severely: `InstallPreReqSilent()` (`pim/pim_src/pimTop.cxx:904-966`) uses
  this list's `.GetSize()` alone — never reading its content — to bound a
  loop indexing a completely unrelated array
  (`pimGetSessionInfo()->GetEntitlement(i)`). A later, dedicated pass on
  `AddMandatoryAppsForSilentInstall()` itself **corrected** this paragraph's
  original "the common case for a silent install" framing of when that list
  is empty — since `AddMandatoryAppsForSilentInstall()` always runs first
  in the one confirmed call path and unconditionally attempts to inject
  `creobase.xml`/`qualityagent.xml`, the list is typically non-empty; the
  real, more precisely confirmed consequence is a size **mismatch**, not
  usually a fully empty list — and surfaced 2 further confirmed bugs in that
  function itself (`qualityagent.xml`'s injection silently gated on
  `creobase.xml`'s existence; `-allpacks` utility entries invisible to
  `InstallPreReqSilent()`) — this function has no header anywhere in this
  archive, so no Doxygen patch exists for it. A later, dedicated pass on
  `IsApplicationInInstallList()` itself added a **confirmed structural risk
  in the opposite direction**: its substring search can false-positive
  whenever one entry's filename is a literal substring of another already
  in the list — checked pairwise against the 3 real confirmed arguments in
  this codebase (none collides), so recorded as a confirmed risk, not a
  demonstrated live bug. See `docs/classes/pimGetApplicationsList.md`'s Risk
  Analysis.
- The `pimSilent.h` annotations (`pim_core/includes/`, all 7 declarations)
  record `pimSilentTestXmlIsUseable` and its 5 sibling free functions'
  findings — **not a class**, see `docs/classes/pimSilent.md`. Discovered
  while tracing `pimSilentInstallFromXML()`'s per-entry validation loop (see
  the `pimGeneralInit.h` paragraph above). Confirmed: `pimSilentTestXmlIsUseable()`
  leaks a `pimXmlFile` on its XML-syntax-error path (the success path's
  `delete` is skipped by the early `return false;`); and
  `pimSilentCanMatchNeeds()` leaks **2** `pimXmlFile` objects whenever
  either of its 2 input files fails to parse, standing out clearly by
  contrast with every one of its internal early-returns correctly deleting
  both first. A later, dedicated pass on `pimSilentFixupPSF()` itself added
  2 more confirmed findings by tracing its dependency on
  `pimCommandMgr`'s explicitly documented "cannot delete the last command of
  a type" constraint: its own `pDiUmMMY<N>` collision-workaround placeholder
  can become permanently stuck/undeletable in `Eb`'s final command set under
  that same rule; and its collision handling deletes every command of the
  colliding license type, not just the single named command its own inline
  comment describes. A later, dedicated pass on `pimSilentCanMatchNeeds()`
  itself found its package-availability check silently changes meaning
  under 3 global command-line modes (`-basepack`/CreoNGCRI/`-allpacks`),
  contradicting its own header comment — confirmed by direct contrast with
  its platform/language sibling checks in the same function, which have no
  such dependency. A later, dedicated pass on `pimSilentFixupShortcuts()`
  itself found a 2nd confirmed bug in this function, distinct from the
  caller-side `B_avail[i]`-vs-`A_wants[i]` indexing bug already documented
  in `docs/classes/pimShortcutMgr.md`'s Risk Analysis: its
  `GetShortcutProgramMenu()`/`SetShortcutProgramMenu()` pair ignores the
  `Get`'s return value and calls the `Set` unconditionally, so a stale
  Program Menu group name from an earlier, unrelated shortcut can be copied
  onto the current one — confirmed by direct contrast with the correctly
  return-value-guarded `GetShortcutStartDir()`/`SetShortcutStartDir()` call
  3 lines later in the same function. A later, dedicated pass on
  `pimSilentCreateEntitlement()` itself **corrected** the
  `pimSilentCanMatchNeeds()` package-check finding above: since
  `pimSilentCreateEntitlement()`'s own package-selection loop calls the
  identical `GetInstallPackageNames()` on the same `A`, under the same mode
  flags, that `pimSilentCanMatchNeeds()` uses — and the 2 functions always
  run back-to-back on the same `A`/`B` pair in the same process — the
  earlier "`-basepack` validation gap"/"`-allpacks` overly strict"
  characterization was too strong; both are confirmed to be by design, with
  the 2 functions consistent with each other. Only the header-comment
  imprecision (both describe a plain `install="Y"` check) remains an
  accurate part of the original finding. A later, dedicated pass on
  `pimIsProductXmlMatch()` itself found a confirmed structural limitation
  shared identically by `pimIsVersionMatch()` (which duplicates the exact
  same control-flow shape): neither function has a cross-schema fallback
  for `B` — whichever root schema succeeds for `A` (a normal `<PRODUCT>`
  root, or `<EXTERNAL_INSTALLER>`) is the only one ever tried for `B`, so a
  mixed-schema `A`/`B` pair always reports "no match" even with identical
  `<TAG>`/version/shipcode values. A further, dedicated pass specifically on
  `pimSilentFixupPSF()`'s already-documented stuck-dummy bug precisely
  characterized its reachability (fires whenever no other `A`-wanted command
  independently shares the colliding command's old license type — a
  plausible, non-degenerate scenario) and traced its real downstream
  consumer for the first time: `pimEntitlement::InstallScripts()` runs
  `pimScriptLoop::OnInstall()` directly on the same XML document, which
  reads the first `GetAllCommandNames()` entry unconditionally to set
  `[LM_LICENSE_FILE]` for the entitlement's install scripts — so a stuck
  dummy landing at that position would corrupt the whole entitlement's
  license-file setting, not just leave an inert PSF entry behind. A further,
  dedicated pass specifically on `pimSilentFixupShortcuts()`'s
  already-documented stale-Program-Menu-copy bug confirmed it is reachable
  in normal use (`<PROGRAMSMENU>` is confirmed independently optional per
  shortcut) and traced a concrete, deterministic downstream consequence:
  `pimEntitlement::InstallShortcuts()` runs `pimShortcutLoop::OnInstall()`
  directly on the same XML document, which uses the shortcut's
  `<PROGRAMSMENU>` text as a filesystem subfolder path when placing the
  installed `.lnk` file — so a stale value misplaces the shortcut into a
  different, earlier-processed shortcut's Start Menu folder, or (if empty)
  silently drops its Program-Menu placement entirely. A further, dedicated
  pass specifically on `pimIsProductXmlMatch()`'s already-documented
  no-cross-schema-fallback structural limitation found this codebase's own
  other 2 product-XML schema-resolution call sites (`pimEntitlement.cxx:634-635,967-968`)
  try multiple schemas in sequence for a single file — direct, confirmed
  evidence, elsewhere in this exact codebase, that a product's root schema
  is not assumed fixed, undermining `pimIsProductXmlMatch()`/
  `pimIsVersionMatch()`'s single-schema-for-both-`A`-and-`B` assumption by
  this codebase's own design — and traced its real downstream consumer for
  the first time: `pimSilentInstallFromXML()`'s per-file loop simply skips
  any product whose match fails this way, and since its own final return
  value is `pimGetLastError()` (a process-wide static array's last appended
  error, not a per-file record), a mixed-schema false-negative on one
  product in a multi-XML batch can be silently overwritten in the reported
  exit code by any later, unrelated error. A further, dedicated pass
  specifically on `pimSilentCanMatchNeeds()`'s already-documented
  package-check mode-dependent finding found the 3 mode flags are not
  cleanly mutually exclusive — `-releaselink` can be freely combined with
  `-allpacks`/`-basepack` (unlike those 2, which the CLI parser itself
  enforces as mutually exclusive), silently subordinating `-releaselink`'s
  own package-selection effect to whichever of the other 2 is set — and
  traced its real downstream consumer for the first time:
  `pimPackageMgr::SetPackageInstallState()` writes the mode-dependent
  selection directly onto the `<PACKAGE>` node's `install` attribute on the
  shared XML document, which `pimMSILoop::IsEligibleForInstall()` reads at
  real MSI install-execution time to decide whether a package's feature is
  actually, physically installed — confirming this mode-dependent logic is
  the literal, real-world determinant of installed product features, not
  merely a validation/documentation-accuracy concern. A further, dedicated
  pass specifically on `pimSilentCreateEntitlement()`'s already-documented
  `<MSI>`-node last-value-wins caveat — previously flagged only as a
  lower-confidence structural note — upgraded it to a **confirmed bug**:
  `pimMSILoop::pimMSIExec()` (the real MSI install-execution driver)
  independently iterates and executes every eligible `<MSI>` node as its own
  install action, confirming multi-`<MSI>`-node product XML is a real,
  designed-for configuration, not hypothetical — and traced its real
  downstream consumer for the first time: the single `format`/`Cmd` pair
  this function's loop stamps onto every `<MSI>` node in `B` is exactly
  what `pimMSIExec()` reads per node at real install time, where `format`
  directly selects that node's installer UI mode (full wizard, basic, or
  silent) and the `<MSIARGUMENT>` text feeds its `msiexec.exe` command
  line — so a product with multiple, independently-configured MSI packages
  has this bug forcing all of them to adopt whichever single mode/argument
  pair belonged to the last `<MSI>` node in the user's request XML. A
  further, dedicated pass specifically on `pimSilentFixupPSF()`'s
  already-documented collision-deletes-entire-type discrepancy found this
  function's own opening "Drop" step already reduces `B` to exactly 1
  surviving command per originally distinct license type before the
  collision-handling block runs, so a given type's *first* collision can
  only ever delete that one straggler — matching the comment's singular
  framing; the discrepancy only manifests when 2+ collisions in the same
  call share an original license type — and traced an internal consequence
  with the already-documented stuck-dummy bug: each later same-type
  collision's delete sweep incidentally cleans up the *previous*
  collision's dummy, so only the *last* dummy created per shared type is
  actually exposed to the stuck-dummy/`[LM_LICENSE_FILE]` risk, narrowing
  rather than retracting that earlier finding. A further, dedicated pass
  specifically on `pimSilentCreateEntitlement()`'s already-documented
  `<PROPERTY>` skip list traced a distinct, confirmed downstream reason for
  each of the 4 skipped names — `[SHIPCODE]` feeds the session's own
  `SHIPCODE_PROPERTY` from `Eb`'s own value right after entitlement
  creation, `[VERSION]` is intrinsic to which XML file `Eb` was
  initialized from and is later read for version-gated custom-action
  behavior and cross-entitlement matching, `[SOURCE]` is overwritten
  anyway by the caller moments later, and `CustomActions` gates whether
  `B`'s own custom-action fixups run at all across the entire
  install/uninstall lifecycle via a newly-discovered `pimCustomActionsLoop`
  (`pimLoop` subclass) — and verified the copy loop's own stale-`name`
  pattern is a non-issue, since the following copy step's own guard
  prevents it from ever having an observable effect. A further, dedicated
  pass specifically on `pimIsVersionMatch()`'s shipcode comparison found the
  check is optional, not enforced — it only runs if both `A` and `B`
  expose a shipcode, which `pimEntitlement::Init()` doesn't require, so a
  valid product XML can legitimately lack one and silently bypass the
  gate — and traced that a skipped check reaches `return true;` via the
  exact same path as an explicit shipcode match, with no log, warning, or
  property recording which case occurred, so the caller's own
  differentiated-error logic never gets a chance to distinguish them. A
  further, dedicated pass specifically on `pimSilentCanMatchNeeds()`'s
  platform and language checks confirmed both are genuinely free of the
  package check's mode-dependence, but found they lack that check's
  later-confirmed consistency guarantee — `pimSilentInstallFromXML()`'s
  post-creation code re-derives `Eb`'s actual platform (from the current
  machine's own OS, via `btkGetPlatform()`) and language selections (from
  `-LANG`/`-allpacks` CLI flags) using entirely different inputs than what
  this validation checked against `A`'s XML, and both of those
  post-creation calls discard their own return values, which fail silently
  whenever the requested platform/language doesn't actually exist in `B` —
  so a mismatch here can go completely undiagnosed anywhere in the
  pipeline. **Corrected in a subsequent pass** (which required re-reading
  `pimSilentCreateEntitlement()`'s full body while investigating
  `pimSilentFixupPSF()`'s `Ea`/`Eb` semantics): `pimSilentCreateEntitlement()`
  actually does mirror `A`'s platform/language onto `B` first, consistent
  with validation, matching the package precedent — the corrected gap is
  that this mirroring is subsequently undone by `pimTop.cxx`'s separate,
  later, unvalidated overwrite. That same further, dedicated pass on
  `pimSilentFixupPSF()`'s `Ea`/`Eb` reference-vs-copy semantics found both
  parameters are symmetric references with confirmed asymmetric roles
  (`Ea` read-only, `Eb` the sole mutation target) and lifetimes (`Ea`
  ephemeral, destroyed when `pimSilentCreateEntitlement()` returns; `Eb` a
  pointer to the real, persistent, session-owned entitlement, not a copy)
  — and confirmed a real but currently unreachable double-free hazard in
  `pimEntitlement`'s own lack of custom copy semantics, safe today because
  every value-type `pimEntitlement` instance in the entire codebase (only
  3 exist, all in this file) is confirmed never copied or assigned. A
  further, dedicated pass specifically on `pimSilentCreateEntitlement()`'s
  session-index lookup found a confirmed bug: its `entitlement_index`
  out-parameter is set to a valid index immediately after `B` is added to
  the session, but before `A`'s own parseability is verified — if that
  later check fails, the function returns `false` without resetting
  `entitlement_index`, and since its only caller never checks this
  function's `bool` return value at all (relying solely on the
  out-parameter's sign), this specific failure path is silently swallowed
  and a half-created entitlement is treated as fully valid. Traced this to
  be currently unreachable via the codebase's own single call site, since
  the caller's own earlier validation step already guarantees `A` parses
  successfully by construction — a confirmed but latent gap in this
  function's own error-handling design. Also verified a related non-issue:
  the session's own duplicate-entry guard compares a filesystem path
  against a product tag and so never fires, confirming the index
  arithmetic itself is otherwise reliable. See
  `docs/classes/pimSilent.md`'s Risk Analysis. A further, dedicated pass
  specifically on `pimSilentCanMatchNeeds()`'s package check reachability
  traced its 2 `pimPackageMgr` accessor calls into their implementations
  and found a confirmed crash bug: neither function null-checks an XML
  attribute lookup before dereferencing it, and which specific attribute
  is at risk depends on which of the 3 global command-line mode flags is
  active — a `<PACKAGE>` node missing that attribute crashes the check
  outright rather than merely mis-selecting. Confirmed by direct contrast
  with a sibling method in the very same class that carefully guards the
  identical 4 attributes, and confirmed this exposure is the widest of the
  3 sibling checks (platform/language share a narrower, 2-attribute
  version of the same unguarded pattern). Not confirmed reachable against
  a real product/media XML pair, since none exists in this archive, but
  the mechanism and the internal inconsistency are fully confirmed from
  source. See `docs/classes/pimSilent.md`'s Risk Analysis. A further,
  dedicated pass specifically verifying `pimSilentFixupShortcuts()`'s
  already-proposed stale-Program-Menu-copy fix confirmed the fix (gating
  the `Set` call on the `Get` call's return value) fully eliminates the
  original corruption, but found the fix and the pattern it mirrors both
  share a separate, previously unexamined ceiling: none of the underlying
  `pimShortcutMgr` mutators can create a missing child node, only mutate an
  existing one, so `A` can never grant a shortcut a customization `B`'s own
  template never defined — a silent, by-design limit, not a defect the fix
  leaves behind. See `docs/classes/pimSilent.md`'s Risk Analysis. A
  further, dedicated pass on `pimSilentCreateEntitlement()`'s
  quality-agent flag copy — a previously untouched area of an
  already-documented function — found a confirmed silent-override bug:
  disabling quality agent silently fails whenever `B`'s own media
  definition marks it required, discarded by the caller with zero
  diagnostic, in direct contrast to this codebase's own GUI path, which
  checks the identical business rule first and disables the control
  entirely rather than attempting and silently failing. The overridden
  state controls a real Windows registry value read at real
  install/reconfigure time, confirmed by the GUI's own "PHM"/legal-text
  framing to be a genuine, consent-relevant telemetry opt-in setting, not
  a latent XML-consistency concern. See `docs/classes/pimSilent.md`'s Risk
  Analysis. A further, dedicated pass on `pimSilentFixupPSF()`'s
  `DeleteCommand()` return-value check traced all 3 call sites precisely:
  the Drop step's discarded return is a verified non-issue (its own
  by-design terminal state), the collision-handling delete is confirmed by
  an exhaustive counting proof to always succeed given its preceding
  `AppendAddCommand()` call itself succeeds (a precondition also never
  checked, depending on a `<PSF_TEMPLATE>` this archive can't confirm
  always exists), and the final cleanup loop's discarded return
  generalizes the already-documented stuck-dummy finding: any sole
  surviving command of a license type `A` doesn't want at all — not just
  dummies — gets permanently stuck, confirming the function's own stated
  goal is structurally unachievable for such a type. See
  `docs/classes/pimSilent.md`'s Risk Analysis. A further, dedicated pass on
  the same function's `AppendAddCommand()` return-value check found a more
  severe sibling: when the main re-add loop needs to create a genuinely
  new command for a name `A` wants that `B` never had, and `B`'s media has
  no `<PSF_TEMPLATE>` for the license type `A` specifies (sourced from
  `A`'s own document, unrelated to `B`'s template set), the command is
  silently dropped with zero trace anywhere — no dummy, no leftover, no
  diagnostic — confirmed by this function's own inline comment to be a
  genuine blind spot in its design. See `docs/classes/pimSilent.md`'s Risk
  Analysis. A further, dedicated pass verifying `pimSilentCreateEntitlement()`'s
  already-proposed `<MSI>`-node fix confirmed both candidate matching
  attributes (`name`/`PRODUCTCODE`) genuinely exist on real `<MSI>` nodes,
  but found the fix's own "match by either" phrasing under-specified: the
  2 keys have opposite tradeoffs for the cross-version matching this
  function performs, and a robust fix needs both, with `PRODUCTCODE`
  tried first and `name` as fallback. Also found a related, previously
  unexamined asymmetry in the existing write loop's handling of the 2
  fields it copies. See `docs/classes/pimSilent.md`'s Risk Analysis. A
  further, dedicated pass verifying the same function's `<PROPERTY>`
  copy loop's proposed hardening fix confirmed the fix is purely cosmetic
  today (a single guard already provides all real protection) but closes
  a genuine latent maintenance trap, and — by tracing the loop's full
  return-value chain — found a new, distinct bug: the same loop's
  `GetProperty()` calls also discard their return value, risking a
  stale-value write to `B` if `A` ever has duplicate-named `<PROPERTY>`
  nodes. See `docs/classes/pimSilent.md`'s Risk Analysis.

Not covered in this pass: `pimInterrogator`, the package/platform/language
managers, `pimFrictionlessTrialLicenseGet` (read in full to confirm
`pimFrictionlessTrialDlg`'s headline finding, but not annotated as its own
header), `pimPTCRenewLicenseGet` (`rpimDlg`'s analogous worker class,
located but not read in depth — `rpimDlg::Display()` has no bug requiring
that trace), `pimSessionInfo::TryAuthorize()` (a `pim_core` caller of
`pimAuthDlg`, cited only via its call-site signature — its sibling caller,
`pimGetAvailable::GetSecurity()`, is now itself fully traced, see above), and
the rest of `pim_ui` beyond
`pimShortcutMgr`/`pimInstallMgrDlg`/`pimCustomDlg`/`pimEntitlementTree`/
`pimFrictionlessTrialDlg`/`rpimDlg`/`pimAuthDlg` — none of these had a
full-depth class doc backing verified behavior, so annotating them here
would risk guessing. **All 10 Loop subclasses (the original 9, plus the
newly-discovered `pimGetAvailable`) and all 7 owner-wrapper singletons now
have full-depth documentation, completing that family — with one
correction: a further, dedicated pass on `pimSilentCreateEntitlement()`'s
`<PROPERTY>` skip list discovered an 11th `pimLoop` subclass,
`pimCustomActionsLoop` (`pim_core/includes/pimCustomActions.h` +
`pim_core_src/pimCustomActions.cxx`), consumed by
`pimEntitlement::OnInstallCustomActions()`/`OnUninstallCustomActions()`
across the install/uninstall lifecycle. It is cited in `docs/classes/pimSilent.md`
only via the specific traced methods relevant to that finding, per this
set's Coverage Honesty Statement — it does not yet have its own full-depth
class doc or header annotation, so the "completing that family" claim above
now has 1 known exception.**
`pimShortcutMgr`, `pimInstallMgrDlg`, `pimCustomDlg`, `pimEntitlementTree`,
`pimFrictionlessTrialDlg`, `rpimDlg`, and `pimAuthDlg` are the first 7
`pim_ui`-module classes added to this set. Extending this patch set to
more classes is a natural follow-up once those classes get their own
full-depth documentation.

## How to apply

Each `.patch` is a standard unified diff (`diff -u`, `a/`/`b/` labeled, `-p1`
compatible) against the corresponding file in the original `installmgr.zip` source
tree. From the repository root containing `pim_core/`:

```sh
patch -p1 < generated/doxygen/pimLoop.h.patch
patch -p1 < generated/doxygen/pimCopyLoop.h.patch
patch -p1 < generated/doxygen/pimXmlFile.h.patch
patch -p1 < generated/doxygen/pimSessionInfo.h.patch
patch -p1 < generated/doxygen/pimEntitlement.h.patch
patch -p1 < generated/doxygen/pimSFXLoop.h.patch
patch -p1 < generated/doxygen/pimScriptLoop.h.patch
patch -p1 < generated/doxygen/pimShortcutLoop.h.patch
patch -p1 < generated/doxygen/pimServiceLoop.h.patch
patch -p1 < generated/doxygen/pimPsfLoop.h.patch
patch -p1 < generated/doxygen/pimDownloadLoop.h.patch
patch -p1 < generated/doxygen/pimCopier.h.patch
patch -p1 < generated/doxygen/pimMSICopier.h.patch
patch -p1 < generated/doxygen/pimSFXCopier.h.patch
patch -p1 < generated/doxygen/pimShortcuts.h.patch
patch -p1 < generated/doxygen/pimRegEdit.h.patch
patch -p1 < generated/doxygen/pimServices.h.patch
patch -p1 < generated/doxygen/pimDownloader.h.patch
patch -p1 < generated/doxygen/pimRegEditLoop.h.patch
patch -p1 < generated/doxygen/pimMSILoop.h.patch
patch -p1 < generated/doxygen/pimShortcutMgr.h.patch
patch -p1 < generated/doxygen/pimInstallMgrDlg.h.patch
patch -p1 < generated/doxygen/pimCustomDlg.h.patch
patch -p1 < generated/doxygen/pimEntitlementTree.h.patch
patch -p1 < generated/doxygen/pimFrictionlessTrialDlg.h.patch
patch -p1 < generated/doxygen/rpimDlg.h.patch
patch -p1 < generated/doxygen/pimAuthDlg.h.patch
patch -p1 < generated/doxygen/pimGetAvailable.h.patch
patch -p1 < generated/doxygen/pimGetAvailableProduct.h.patch
patch -p1 < generated/doxygen/pimGetMediaDetails.h.patch
patch -p1 < generated/doxygen/pimGeneralInit.h.patch
patch -p1 < generated/doxygen/pimSilent.h.patch
patch -p1 < generated/doxygen/pimTranslateMgr.h.patch
```

or equivalently `git apply <file>.patch` if the tree is a git repository. All 33 were
verified to apply cleanly (`patch -p1 --dry-run`) against the original files in this
archive.

## Files

| Patch | Target | Lines changed (diff output) |
|---|---|---|
| `pimLoop.h.patch` | `pim_core/includes/pimLoop.h` | 249 |
| `pimCopyLoop.h.patch` | `pim_core/includes/pimCopyLoop.h` | 211 |
| `pimXmlFile.h.patch` | `pim_core/includes/pimXmlFile.h` | 478 |
| `pimSessionInfo.h.patch` | `pim_core/includes/pimSessionInfo.h` | 633 |
| `pimEntitlement.h.patch` | `pim_core/includes/pimEntitlement.h` | 841 |
| `pimSFXLoop.h.patch` | `pim_core/includes/pimSFXLoop.h` | 125 |
| `pimScriptLoop.h.patch` | `pim_core/includes/pimScriptLoop.h` | 120 |
| `pimShortcutLoop.h.patch` | `pim_core/includes/pimShortcutLoop.h` | 275 |
| `pimServiceLoop.h.patch` | `pim_core/includes/pimServiceLoop.h` | 191 |
| `pimPsfLoop.h.patch` | `pim_core/includes/pimPsfLoop.h` | 154 |
| `pimDownloadLoop.h.patch` | `pim_core/includes/pimDownloadLoop.h` | 218 |
| `pimCopier.h.patch` | `pim_core/includes/pimCopier.h` | 179 |
| `pimMSICopier.h.patch` | `pim_core/includes/pimMSICopier.h` | 172 |
| `pimSFXCopier.h.patch` | `pim_core/includes/pimSFXCopier.h` | 155 |
| `pimShortcuts.h.patch` | `pim_core/includes/pimShortcuts.h` | 140 |
| `pimRegEdit.h.patch` | `pim_core/includes/pimRegEdit.h` | 128 |
| `pimServices.h.patch` | `pim_core/includes/pimServices.h` | 163 |
| `pimDownloader.h.patch` | `pim_core/includes/pimDownloader.h` | 210 |
| `pimRegEditLoop.h.patch` | `pim_core/includes/pimRegEditLoop.h` | 184 |
| `pimMSILoop.h.patch` | `pim_core/includes/pimMSILoop.h` | 259 |
| `pimShortcutMgr.h.patch` | `pim_ui/includes/pimShortcutMgr.h` | 225 |
| `pimInstallMgrDlg.h.patch` | `pim_ui/includes/pimInstallMgrDlg.h` | 542 |
| `pimCustomDlg.h.patch` | `pim_ui/includes/pimCustomDlg.h` | 225 |
| `pimEntitlementTree.h.patch` | `pim_ui/includes/pimEntitlementTree.h` | 140 |
| `pimFrictionlessTrialDlg.h.patch` | `pim_ui/includes/pimFrictionlessTrialDlg.h` | 88 |
| `rpimDlg.h.patch` | `pim_ui/includes/rpimDlg.h` | 90 |
| `pimAuthDlg.h.patch` | `pim_ui/includes/pimAuthDlg.h` | 97 |
| `pimGetAvailable.h.patch` | `pim_core/includes/pimGetAvailable.h` | 67 |
| `pimGetAvailableProduct.h.patch` | `pim_core/includes/pimGetAvailableProduct.h` | 104 |
| `pimGetMediaDetails.h.patch` | `pim_core/includes/pimGetMediaDetails.h` | 52 |
| `pimGeneralInit.h.patch` | `pim/includes/pimGeneralInit.h` | 56 |
| `pimSilent.h.patch` | `pim_core/includes/pimSilent.h` | 1188 |
| `pimTranslateMgr.h.patch` | `pim_core/includes/pimTranslateMgr.h` | 102 |

No `.cxx` implementation files were annotated — Doxygen comments belong on the
declarations in the headers; implementation files were left untouched.
