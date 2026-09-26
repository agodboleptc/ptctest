# PIM Documentation & Knowledge System — Deliverable Index

This is the complete output of the 20-phase documentation request against the
`installmgr.zip` source archive (PTC's PIM — Parametric Installation Management).
**Start with `ai-context/ai_readme.md`** if you are an AI agent about to modify this
codebase; start with `docs/01_repository_inventory.md` if you are a human onboarding
onto it for the first time.

## Ground Rules Followed Throughout

- No source code was modified. Doxygen comments (Phase 6) are unapplied `.patch`
  files only.
- Every claim is traceable to a specific file/line/call site cited in the relevant
  doc. Anything not directly confirmed is marked `UNKNOWN`, `inferred`, or
  "not individually re-traced this pass" — never presented as fact.
- Where evidence was incomplete (missing build files, missing `btk` toolkit source,
  missing launcher EXEs, missing sample XML — see `docs/01_repository_inventory.md`
  §0), that gap is stated explicitly rather than filled with a plausible guess.
- Corrections discovered mid-analysis (e.g. `pimRegEdit` turning out to be a
  singleton, not a per-entitlement object as originally assumed) are documented as
  corrections in place, not silently retconned into earlier docs.

## Structure

```
docs/
├── 01_repository_inventory.md       Phase 1 — corpus scope, folder structure, tech stack, what's missing
├── 02_architecture_overview.md      Phase 2 — modules, dependencies, ownership, Mermaid diagrams
├── 03_application_startup.md        Phase 3 — full pimInstallerRun trace, init sequence, shutdown
├── 04_installation_flow.md          Phase 7 — OnInstall pipeline, skip-flags, rollback, error paths
├── 05_prerequisite_framework.md     Phase 8 — the IS_INSTALLED_FUNC dispatcher and its 2 wired rules
├── 06_entitlement_framework.md      Phase 9 — lifecycle/state model synthesis
├── 07_registry_usage.md             Phase 10 — 2 registry mechanisms, full key catalog
├── 08_xml_configuration.md          Phase 11 — 6 document types, ~230-constant vocabulary
├── 09_logging_framework.md          Phase 12 — logger + message-catalog systems
├── 10_error_handling.md             Phase 13 — 4 error/status channels, full error-code taxonomy
├── 11_troubleshooting.md            Phase 14 — 10 playbooks (MSI, EXE, registry, rollback, etc.)
├── 12_developer_onboarding.md       Phase 15 — build/debug/trace/extend guide
├── 13_closing_summary.md            Closing summary of the entire effort — final tally, ranked
│                                    high-severity findings, architectural corrections, honest limits
├── modules/                         Phase 4 — pim.md, pim_core.md, pim_ui.md, pim_util.md
├── classes/                         Phase 5 (+ 52 extension passes) — 35 full docs: pimLoop, pimEntitlement,
│                                    pimSessionInfo, pimXmlFile, pimCopyLoop, pimSFXLoop, pimScriptLoop,
│                                    pimShortcutLoop, pimServiceLoop, pimPsfLoop, pimDownloadLoop, pimRegEditLoop,
│                                    pimMSILoop, pimGetAvailable, pimGetMediaDetails, pimAvailableProduct, pimGetApplicationsList,
│                                    Cmp_cStrings, pimSilent, pimTranslateMgr, pimCopier, pimMSICopier, pimSFXCopier, pimShortcuts, pimRegEdit,
│                                    pimServices, pimDownloader, pimShortcutMgr, pimInstallMgrDlg, pimCustomDlg,
│                                    pimEntitlementTree, pimFrictionlessTrialDlg, rpimDlg, pimAuthDlg — ALL 10 Loop
│                                    subclasses (pimGetAvailable a 10th, previously uncatalogued, discovered late)
│                                    + all 7 owner-wrapper singletons + pimGetMediaDetails (a related but distinct
│                                    pim_core singleton) + pimAvailableProduct (pimGetAvailable's own value-object/
│                                    collection pair, promoted from a companion mention) complete + pimGetApplicationsList
│                                    + pimSilentTestXmlIsUseable (both not classes -- free-function groups, one in
│                                    `pim`, one in `pim_core`) + Cmp_cStrings (not documentable
│                                    at full depth -- no implementation anywhere in this archive) + the first 7
│                                    pim_ui-module classes
└── diagrams/                        Standalone .mmd extractions of every Mermaid diagram (see diagrams/README.md)

generated/
└── doxygen/                         Phase 6 (+ 51 extension passes) — 33 unapplied .patch files (verified to apply cleanly, no new patch for Cmp_cStrings -- it has no header to annotate) + README.md

ai-context/                          Phases 16-19 — machine-readable YAML + ai_readme.md
├── architecture.yaml
├── modules.yaml
├── classes.yaml
├── flows.yaml
├── business_rules.yaml
├── dependencies.yaml
├── ownership.yaml
├── registry.yaml
├── xml_schema.yaml
├── runtime_states.yaml
├── knowledge_graph.yaml
├── change_impact.yaml
└── ai_readme.md
```

This matches the requested Phase 20 layout exactly, with one addition: `docs/diagrams/`
holds standalone extractions of diagrams that also appear inline in their source
`.md` files (inline is primary/authoritative; the standalone files exist for tooling
that wants diagrams separately).

## Coverage Honesty Statement

Not everything in this ~82,000-line, 4-module codebase received equal depth:

| Depth | What |
|---|---|
| **Full** (module doc + narrative flow docs + cross-referenced everywhere) | `pim`, `pim_core`, `pim_util` |
| **Lighter** (module doc only, dialogs not individually read — `pimShortcutMgr`, `pimInstallMgrDlg`, `pimCustomDlg`, `pimEntitlementTree`, `pimFrictionlessTrialDlg`, `rpimDlg`, and `pimAuthDlg` excepted, now full class-level detail) | `pim_ui` |
| **Full class-level detail** (class doc + Doxygen patch + change-impact entry, all backed by a full header+`.cxx` source read) | `pimLoop`, `pimEntitlement`, `pimSessionInfo`, `pimXmlFile`, `pimCopyLoop`, `pimSFXLoop`, `pimScriptLoop`, `pimShortcutLoop`, `pimServiceLoop`, `pimPsfLoop`, `pimDownloadLoop`, `pimRegEditLoop`, `pimMSILoop`, `pimGetAvailable`, `pimGetMediaDetails`, `pimAvailableProduct`, `pimGetApplicationsList`, `pimSilentTestXmlIsUseable`, `pimTranslateMgr`, `pimCopier`, `pimMSICopier`, `pimSFXCopier`, `pimShortcuts`, `pimRegEdit`, `pimServices`, `pimDownloader`, `pimShortcutMgr`, `pimInstallMgrDlg`, `pimCustomDlg`, `pimEntitlementTree`, `pimFrictionlessTrialDlg`, `rpimDlg`, `pimAuthDlg` — **all 10 Loop subclasses (the original 9 plus `pimGetAvailable`, a 10th confirmed subclass discovered late), `pimGetMediaDetails` (a related but functionally distinct `pim_core` singleton), `pimAvailableProduct` (`pimGetAvailable`'s own value-object/collection pair, promoted from a companion mention), `pimGetApplicationsList` and `pimSilentTestXmlIsUseable` (both not classes — free-function groups in `pim` and `pim_core` respectively, covering the silent-install application-list and XML-match/merge pipelines), and all 7 owner-wrapper singletons, complete, plus the first 7 `pim_ui` classes** (`pimInstallMgrDlg`'s own doc carries an explicit Scope note disclosing which large method bodies, notably the ~550-line `OnPushButtonActivate()`, were not traced to the same depth) |
| **Full doc, but no implementation exists to trace** (class doc + change-impact entry; **no** Doxygen patch, since there is no header declaring it anywhere in this archive) | `Cmp_cStrings` — a `btkMap` comparator confirmed to have zero implementation in `installmgr.zip`; this row documents the boundary of what's confirmable (every declaration/usage site) rather than a traced behavior |
| **Partial evidence only** (cited where relevant, no dedicated class doc) | `pimInterrogator`, `pimPackageMgr`/`pimPlatformMgr`/`pimLanguageMgr`, `pimFrictionlessTrialLicenseGet` (read in full to confirm `pimFrictionlessTrialDlg`'s headline finding), `pimCustomActionsLoop` (an 11th, newly-discovered `pimLoop` subclass, found while documenting `pimSilentCreateEntitlement()`'s `<PROPERTY>` skip list — see item 34 in "What Would Extend This Set Further" below; the "all 10 Loop subclasses" phrasing in the row above predates this discovery) |
| **Not read this pass** | `pimCDMaker.cxx`, `pimPTCRenewLicenseGet` (located while documenting `rpimDlg`, not read in depth) |

This was a deliberate scoping decision (agreed with the requester at the outset:
"foundation first, then continue phase-by-phase") to produce a small number of
deeply-verified, richly cross-referenced documents rather than a large number of
shallow, unverified ones. `ai-context/change_impact.yaml`'s `full_detail` field and
every doc's own "not traced this pass" notes make the boundary explicit everywhere
it matters, so nothing here should be mistaken for more thoroughly verified than it
is.

## What Would Extend This Set Further

> **All 55 items below are now complete** (the last, item 55, closes in the only sense
> an "absence" item can — independent re-verification rather than new tracing; see its
> own entry for what that means and doesn't mean). This section is kept as a record of
> what was found and how, not as a live task list.

If continued, the highest-value next targets (in priority order, matching what
`docs/*.md` files already flag as open questions) would be:
1. ~~The remaining 6 Loop subclasses at the same depth as `pimCopyLoop`~~ — **done**:
   `pimSFXLoop`, `pimScriptLoop`, `pimShortcutLoop`, `pimServiceLoop`, `pimPsfLoop`,
   `pimDownloadLoop` now all have full class docs, Doxygen patches, and change-impact
   entries. This pass also surfaced and propagated a correction across the whole
   documentation set: all 7 Loop-owner wrapper classes (not just `pimRegEdit`) are
   process-wide singletons, not per-entitlement objects — see `correction_history` in
   `ai-context/ownership.yaml` and `ai-context/knowledge_graph.yaml`.
2. ~~The 7 owner-wrapper singleton classes themselves at full class-doc depth~~ —
   **done**: `pimCopier`, `pimMSICopier`, `pimSFXCopier`, `pimShortcuts`, `pimRegEdit`,
   `pimServices`, `pimDownloader` now all have full class docs (each backed by a full
   header+`.cxx` read), Doxygen patches, and change-impact entries. This pass found the
   7 singletons are NOT uniform in how they enforce serialization: **`pimServices`'
   own lock does not actually serialize whole operations — a confirmed, live
   use-after-free-class race condition** (see `ai-context/business_rules.yaml`'s
   `pimservices_lock_does_not_serialize` and `docs/classes/pimServices.md`, now the
   top item in `ai-context/ai_readme.md`'s High-Risk Areas); `pimMSICopier`/
   `pimSFXCopier` share one external gate rather than independent locks; 4 of the 7
   have a flag-only `Cancel()` compensated for at the call site; and `pimRegEdit.h`'s
   own comment about blocking behavior is confirmed stale/wrong.
3. ~~`pimRegEditLoop` at full class-doc depth~~ — **done**: this Loop subclass now
   has a full class doc, Doxygen patch, and change-impact entry. This pass
   surfaced a confirmed, high-confidence finding: `OnRollback()`'s node-list loop
   double-increments its index (the `for` loop's own increment plus an explicit
   `item(index++)` call), silently skipping every other `<REGISTRY>` entry on
   **every** pass, forever — confirmed by direct contrast with the correctly
   single-incrementing equivalent loop in `OnInstall()` in the same file.
   Uninstall/rollback can leave roughly half a product's registry entries
   behind. See `ai-context/business_rules.yaml`'s
   `pimregeditloop_rollback_skips_half_of_entries` and `docs/classes/pimRegEditLoop.md`.
4. ~~`pimMSILoop` at full class-doc depth~~ — **done**: the 9th and final Loop
   subclass (previously the last remaining `light_detail` entry) now has a full
   class doc, Doxygen patch, and change-impact entry. This pass surfaced **the
   single highest-severity finding of the entire documentation effort**:
   `OnInstall()`'s MSI-API install path has a `fallback_to_msiexec` flag that is
   checked at the end of **every** remaining loop iteration (not once after the
   loop) and is never reset once set — so a single legacy `<MSI>` node with an
   unparseable install command causes every remaining node's iteration to
   re-invoke `pimMSIExec()`, which re-scans and can silently **re-install
   already-installed packages** (its eligibility check ignores the
   "already installed" attribute), an O(N²) blowup in the worst case. Also
   confirmed the `same_msihybrid` skip-redundant-install property has no actual
   effect anywhere in this file. See `ai-context/business_rules.yaml`'s
   `pimmsiloop_fallback_causes_repeated_reinstall`, `docs/classes/pimMSILoop.md`,
   and the now-updated #1 slot in `ai-context/ai_readme.md`'s High-Risk Areas.
   **All 9 Loop subclasses and all 7 owner-wrapper singleton classes now have
   full-depth documentation — the core `pim_core` install-step machinery is
   completely covered, with zero remaining `light_detail_classes` entries.**
   (**Corrected in a much later pass, item 13 below**: a 10th `pimLoop`
   subclass, `pimGetAvailable`, was found still uncatalogued — "all 9" was
   never actually all of them.)
5. ~~`pimShortcutMgr` at full class-doc depth~~ — **done**: the first
   `pim_ui`-module class in this documentation effort (also called directly
   from `pim_core`) now has a full class doc, Doxygen patch, and
   change-impact entry. This pass surfaced a **confirmed coverage gap** in
   `AreChangesPending()` — it can only detect a shortcut that *changed*
   while present in both compared XML snapshots, never one *added or
   removed* entirely, with a real consequence in `pimEntitlement.cxx`'s
   update/reconfigure decision logic — plus a **confirmed caller-side
   array-indexing bug** in `pimSilent.cxx`'s `pimSilentFixupShortcuts()`
   (found while tracing this class's usage: 4 of 8 shortcut-state-copy calls
   use the wrong array's loop index). See `ai-context/business_rules.yaml`'s
   `pimshortcutmgr_areschangespending_misses_add_remove` and
   `pimsilentfixupshortcuts_array_index_bug`, and
   `docs/classes/pimShortcutMgr.md`.
6. ~~`pim_ui`'s `pimInstallMgrDlg` at full class-doc depth~~ — **done**: the
   main wizard dialog class (header + `pimInstallMgrDlg.cxx` +
   `pimInstallMgrActions.cxx`, 5,575+ lines, the largest class in this
   entire documentation effort) now has a full class doc — with an explicit
   Scope note disclosing which large method bodies, notably the ~550-line
   `OnPushButtonActivate()`, were not traced to the same depth — a Doxygen
   patch, and a change-impact entry. This pass surfaced the **largest set of
   confirmed findings in a single class in this documentation effort**: a
   `StepForward()`/`StepBack()` wizard-step guard-condition asymmetry that
   can land the wizard on a tab it never actually visited going forward; a
   duplicated `UpdateParentEntitlements()`/`UpdateChildEntitlements()`
   implementation shared verbatim (modulo one stray comment) with the
   nested `uiCustomAppTree` class; a permanently no-op `RefreshFeatureTab()`;
   an unreachable `OnTabSelect()` Help/Advanced branch; a message-ID bug in
   `SetPortError()`; a 3-way duplicated exit/cleanup path across
   `OnClose()`/`OnFinishFromEula()`/`OnFinishFromBetaEula()`; and
   `pimCustomDlg` flagged as a parallel class with a near-identical
   method-name set — this "likely-superseded" speculation was **corrected**
   one item below, when `pimCustomDlg` itself was documented and confirmed
   live. See `ai-context/business_rules.yaml`'s
   `piminstallmgrdlg_stepback_stepforward_asymmetry`,
   `piminstallmgrdlg_updateparent_child_duplication`,
   `piminstallmgrdlg_refreshfeaturetab_and_helptab_dead`,
   `piminstallmgrdlg_setporterror_message_id_bug`, and
   `docs/classes/pimInstallMgrDlg.md`.
7. ~~`pim_ui`'s `pimCustomDlg` at full class-doc depth~~ — **done**: a
   standalone "Customize Application Settings" popup dialog (header + `.cxx`,
   2,276 lines, plus the shared `pimSAB.cxx`, 589 lines) now has a full class
   doc, Doxygen patch, and change-impact entry. **This pass corrected the
   prior item's "likely-superseded, not confirmed dead" speculation**: a
   full source read confirms `pimCustomDlg` is live, invoked from 2 confirmed
   call sites (`pimInstallMgrDlg::OnPushButtonActivate()`'s `CustomizeBtn`
   branch, and a newly-discovered second caller,
   `pimEntitlementTree::OnPushButtonActivate()`'s per-row customize-icon
   handler), and directly `#include`d into the unity build via
   `pim_ui_src.cxx`. This pass's own findings: a **confirmed wrong-static-
   handle bug** — `SetSABPath()`/`DisableSAB()`/`EnableSAB()` all guard on
   `main_dlg_sab_handle` (a `static` owned by `pimInstallMgrDlg`) instead of
   this class's own `help_dlg_sab_handle`, currently masked only because
   `pimInstallMgrDlg::InitSAB()` has always already run by the time either
   confirmed caller reaches `Display()`; a **confirmed UX gap** — the
   entitlement-tree customize icon resolves but never forwards the clicked
   row's specific product, so the dialog always opens on the first eligible
   one; and extensive **structural duplication** with `pimInstallMgrDlg`'s
   embedded Customize tab-set, including 3 renamed-but-identical nested
   table classes. See `ai-context/business_rules.yaml`'s
   `pimcustomdlg_is_not_dead_code`,
   `pimcustomdlg_sab_methods_guard_wrong_static_handle`,
   `pimentitlementtree_customize_icon_ignores_clicked_row`, and
   `docs/classes/pimCustomDlg.md`.
8. ~~`pim_ui`'s `pimEntitlementTree` at full class-doc depth~~ — **done**:
   the checkbox tree driving the wizard's Applications selection/reconfigure
   step (header + `.cxx`, 2,323 lines total) now has a full class doc,
   Doxygen patch, and change-impact entry — and is confirmed to be the 2nd
   `pimCustomDlg` call site (via its per-row "customize" icon). This pass
   surfaced 2 confirmed bugs: `OnOptionMenuSelect()` calls
   `eptr_new_xml->SetXml()` unconditionally, immediately after an
   `if (eptr_new_xml)` guard that covers only the 2 preceding lines — a
   null-pointer-dereference crash risk when
   `pimGetMediaDetails::GetProductXml()` returns `NULL` for the selected
   media; and `ReconfigureDisplay_low()`'s `only_one` branch strips one
   character too many from a computed family-group node id (`"_Creo_app"` →
   `"_Cre"` instead of `"_Creo"`), confirmed by contrast with the
   correctly-computed sibling extraction in `UpdateGroupStatus()` in the
   same file. Tracing this class's callers also surfaced a
   **previously-uncatalogued 3rd `.cxx` file** for `pimInstallMgrDlg`'s own
   method bodies, `pim_ui_src/pimEntitlementRefresh.cxx` (1,514 lines, at the
   time not itself traced — since read in full and folded into
   `docs/classes/pimInstallMgrDlg.md`, see item 12 below). See
   `ai-context/business_rules.yaml`'s
   `pimentitlementtree_optionmenuselect_null_deref`,
   `pimentitlementtree_reconfiguredisplay_low_only_one_off_by_one`, and
   `docs/classes/pimEntitlementTree.md`.
9. ~~`pim_ui`'s `pimFrictionlessTrialDlg` at full class-doc depth~~ — **done**:
   the UI for `pim_rl.exe`'s Frictionless/Commercial Trial license-retrieval
   flow (header + `.cxx`, 410 lines, plus the companion worker
   `pimFrictionlessTrialLicenseGet`, 843 lines, read in full for context) now
   has a full class doc, Doxygen patch, and change-impact entry. This pass
   surfaced **the single highest-severity confirmed finding among the
   `pim_ui` classes documented so far**: for this class's only confirmed
   real usage (`pimFrictionlessTrialRun()`, which always sets
   `pimGetInHouseMode()` to 1 or 2 before calling `Display()`), `Display()`
   calls `FrictionlessLicenseGenerate()` — which starts a background
   FLEXnet license-retrieval worker and arms a polling timer — and then
   blocks on a raw `Sleep(60000)` instead of calling `uiDialog::Activate()`.
   Because the modal event loop never runs, the timer never fires, so
   `OnTimerExpired()` — confirmed by an archive-wide grep to be the ONLY
   call site for `pimFrictionlessTrialLicenseGet::HeartBeat()` in this
   codebase — is never invoked. `HeartBeat()` is where the retrieved license
   is actually saved to disk and the local PSF updated: **none of that
   completion logic ever executes**, and the worker object itself leaks.
   Also confirmed dead: a reentrancy guard checked but never set, plus
   `SetURL()`/`GetAuth()`/`Refresh()`, all with zero confirmed callers. See
   `ai-context/business_rules.yaml`'s
   `pimfrictionlesstrialdlg_display_never_processes_completion` and
   `docs/classes/pimFrictionlessTrialDlg.md`.
10. ~~`pim_ui`'s `rpimDlg` at full class-doc depth~~ — **done**: the UI for
   `pim_re.exe`'s "Renew License" flow (header + `.cxx`, 351 lines total) —
   the closest sibling class to `pimFrictionlessTrialDlg` and 3 years
   older — now has a full class doc, Doxygen patch, and change-impact
   entry. This pass's value is primarily **comparative**: `rpimDlg::Display()`
   correctly calls `uiDialog::Activate()` unconditionally, with no
   mode-dependent branch and no `Sleep()` call — confirming by direct
   contrast that `pimFrictionlessTrialDlg::Display()`'s `Sleep(60000)` bug
   (item 9 above) is a genuine, avoidable defect, not an inherent limitation
   of their shared dialog design. `rpimDlg` shares 2 of
   `pimFrictionlessTrialDlg`'s smaller confirmed-dead-code findings
   (`SetURL()`/`GetAuth()` with zero callers; the identical dead `static
   in_timer` reentrancy guard in `OnTimerExpired()`) but **not** its
   `Refresh()` finding — `rpimDlg::Refresh()` is confirmed actively used.
   Also identified `pimAuthDlg` as the likely original template both
   `rpimDlg` and `pimFrictionlessTrialDlg` were partially copied from — it
   alone fully wires the shared `RetryMsg` member (declared but never used
   in either sibling) through to a visible label. See
   `ai-context/business_rules.yaml`'s
   `rpimdlg_display_confirms_pimfrictionlesstrialdlg_bug_is_avoidable`,
   `rpimdlg_vestigial_shared_interface_methods`, and
   `docs/classes/rpimDlg.md`.
11. ~~`pim_ui`'s `pimAuthDlg` at full class-doc depth~~ — **done**: the
   generic PTC.com credential prompt (header + `.cxx`, 308 lines total) —
   the oldest of the 3-class dialog family (2011) and its confirmed original
   template, called directly from `pim_core` (`pimSessionInfo.cxx`,
   `pimGetAvailable.cxx`) — now has a full class doc, Doxygen patch, and
   change-impact entry, completing this family's investigation. This pass
   surfaced a **confirmed structural gap**: `Initialize()` has no
   idempotency guard, yet this class's `AuthDialog` singleton is confirmed
   genuinely REUSED — `Initialize()`/`Display()` invoked multiple times on
   the same instance within one process, via
   `pimSessionInfo::TryAuthorize()`'s confirmed `do`/`while` retry loop and
   2 further confirmed call paths — exactly the one class in this family
   that needed the guard its own 2 derivative classes both have. Also
   refined (not contradicted) the earlier `rpimDlg`/`pimFrictionlessTrialDlg`
   findings: `SetURL()` genuinely IS called on `pimAuthDlg` (twice, unlike
   its 2 dead copies), but the `URL` member it sets is STILL never read,
   even in this original template — so the "dead `URL` member" pattern
   traces back to 2011, not something introduced by copying. Also confirmed
   `GetAuth()`'s `bool` return value is always `false` and ignored by both
   confirmed callers, and a commented-out `//AuthDialog->SetBadInputs();`
   reveals a confirmed never-implemented retry-feedback feature. See
   `ai-context/business_rules.yaml`'s
   `pimauthdlg_initialize_missing_idempotency_guard_on_reused_singleton`,
   `pimauthdlg_url_member_dead_even_in_original`, and
   `docs/classes/pimAuthDlg.md`.
12. ~~`pim_ui_src/pimEntitlementRefresh.cxx` at the same depth~~ — **done**:
   the previously-uncatalogued 3rd `.cxx` file for `pimInstallMgrDlg`'s own
   method bodies (1,514 lines) has been read in full and folded into
   `docs/classes/pimInstallMgrDlg.md`. This pass's headline finding, the
   single most severe confirmed bug in this class: `EntitlementRefresh()`'s
   School- and Beta-mode trial-license wait loops use
   `while (test == NULL || ++max >= 40)` where the correct sibling
   Trial-mode loop uses `&&`/`<=` — because `||` short-circuits, the retry
   counter never advances while the license fails to parse, so **the loop
   never terminates**, hanging the whole wizard with no timeout and no error
   dialog. Also confirmed: a missing `else` in `IsSufficientSpace()` before
   an unconditional `GetSize(dbl)` call silently discards 2 special-case
   size calculations (found by direct contrast with the correctly-guarded
   sibling logic in `EntitlementRefresh_updateSpace()` in the same file);
   the nested `uiCheckButtonCell` class's entire implementation is confirmed
   dead code (its only would-be caller lives inside a class-wide `#if 0`-
   disabled `uiApplicationsList` class, and both its `SetAlignedLabel()`
   overloads are separately hard-disabled via `#if 0 // Eyal does not want
   labels`); and the `PIM_HIDE_CUSTOMIZE_SCREEN` env var is checked with
   directly contradictory effect (hide vs. show/enable) across 4 distinct
   sites in this one file. See `ai-context/business_rules.yaml`'s
   `pimentitlementrefresh_school_beta_wait_loop_never_terminates`,
   `pimentitlementrefresh_issufficientspace_missing_else`,
   `pimentitlementrefresh_uicheckbuttoncell_dead_code`,
   `pimentitlementrefresh_hide_customize_screen_contradictory`, and
   `docs/classes/pimInstallMgrDlg.md`'s Risk Analysis.
13. ~~`pimGetAvailable` at the same depth~~ — **done**: the web-media
   Applications-screen download-availability search (header + `.cxx`, 715
   lines, plus its companion `pimGetAvailableProduct.h`/`.cxx`, 237 lines,
   also read and annotated in full), discovered while tracing
   `pim_ui_src/pimEntitlementRefresh.cxx`'s `EntitlementDownloadPreAction()`,
   now has a full class doc, Doxygen patch, and change-impact entry. **This
   pass corrects every prior "all 9 Loop subclasses" claim in this
   documentation set**: `pimGetAvailable` (`class pimGetAvailable : public
   pimLoop`) is a 10th confirmed `pimLoop` subclass, previously only
   mentioned in passing in `docs/modules/pim_core.md` and never counted in
   any Loop-subclass tally. Confirmed findings: 2 memory leaks in
   `OnExecute()` (`ImageXml` never freed on the success path;
   `ProductDefinitionXml` never freed on 2 filtered-out paths); the
   declaring comment on `auth_status` (`// -3 abort, 0 success`) is wrong —
   `0` is set on authentication *failure*, not success; `IsAuthorized()` has
   zero callers anywhere in this archive; and `pim_core/pimLocate.cxx` holds
   its own cross-module raw-pointer alias to the same instance (the same
   architectural pattern already flagged as a live bug for `pimSAB.cxx`'s
   statics — see item 7 above — but here confirmed currently safe given the
   exact call sites that exist today). See `ai-context/business_rules.yaml`'s
   `pimgetavailable_onexecute_memory_leaks`,
   `pimgetavailable_auth_status_comment_wrong`,
   `pimgetavailable_isauthorized_dead_and_locate_alias_fragile`, and
   `docs/classes/pimGetAvailable.md`.
14. ~~`pimGetMediaDetails` at the same depth~~ — **done**: the shared
   media-details cache/fetcher `pimGetAvailable` itself depends on (header +
   `.cxx`, 276 lines), and a singleton called from at least 7 confirmed
   files across `pim_core` and `pim_ui`, now has a full class doc, Doxygen
   patch, and change-impact entry. **This pass's headline finding**:
   `GetDetails()`'s cache-hit fast path reads its shared cache arrays with
   **no lock at all**, while every mutating path holds `Mutex` — a genuine
   cross-thread race, since this singleton is confirmed called from both the
   UI thread and a background `pimLoop` thread
   (`pimGetAvailable::OnExecute()`). Also confirmed: `GetProductXml()`'s
   single-retry-on-timeout path refreshes the details cache but retries with
   the stale, pre-refresh URL — confirmed by direct contrast with the
   correct sibling pattern in `pimEntitlement::UpdateMediaUrls()`; a 2nd,
   likely-redundant cache-replace call on that same retry path whose safety
   depends on an external, unconfirmed `dsXArray::Replace()` contract; and —
   most directly useful — confirmation that `GetProductXml()`'s `NULL`
   returns are the exact, concrete root cause of the already-documented
   null-pointer-dereference bug in `pimEntitlementTree::OnOptionMenuSelect()`
   (item 8 above), plus discovery of a confirmed-dead near-duplicate of that
   same bug inside the `#if 0`-disabled `uiApplicationsList::OnOptionMenuSelect()`.
   See `ai-context/business_rules.yaml`'s
   `pimgetmediadetails_cache_hit_path_unlocked`,
   `pimgetmediadetails_getproductxml_stale_url_retry`,
   `pimgetmediadetails_getproductxml_null_is_root_cause_of_entitlementtree_bug`,
   and `docs/classes/pimGetMediaDetails.md`.
15. ~~`pimGetApplicationsList` at the same depth~~ — **done**: **not a
   class** — a free-function group (`pimGetApplicationsList`/
   `pimAddApplicationsToList`/`IsApplicationInInstallList`) in the `pim`
   module sharing a file-scope `static StringXArray`, discovered while
   tracing `pimGetAvailable::OnExecute()`'s use of it, now has a full doc,
   Doxygen patch (only the 4 traced declarations in `pimGeneralInit.h`
   annotated), and change-impact entry. **This pass's headline finding**:
   `InstallPreReqSilent()` (`pim/pim_src/pimTop.cxx:904-966`, on the
   silent-install path) uses this list's `.GetSize()` alone — never its
   content — to bound a loop indexing a completely different array (session
   entitlements), so its entire prerequisite-installation loop **silently
   never runs** whenever no application-limiting command-line flag was
   passed, the common case for a silent install. Also confirmed: the shared
   list is populated in 2 incompatible formats depending on which of 3 flags
   is used (`-APPLICATIONS`: bare tags; `-XML`/`-XMLALL`: full filesystem
   paths, also injected unconditionally on every silent install by
   `AddMandatoryAppsForSilentInstall()`) — 2 of 3 confirmed consumers filter
   via exact match and can never recognize a path-format entry, while only
   `IsApplicationInInstallList()`'s substring search is format-agnostic. See
   `ai-context/business_rules.yaml`'s
   `pimgetapplicationslist_installprereqsilent_wrong_loop_bound`,
   `pimgetapplicationslist_heterogeneous_format_breaks_exact_match_consumers`,
   and `docs/classes/pimGetApplicationsList.md`.
16. ~~`pimAvailableProduct` at the same depth~~ — **done**: the small value
   object (+ its keyed collection `pimGetAvailableProducts`)
   `pimGetAvailable` accumulates web-media search results into, **promoted
   from a brief "companion" mention** (item 13 above originally folded it in
   without full individual scrutiny, even though both files were already
   read in full at that time) **to its own full-depth doc**, Doxygen-patch
   correction, and change-impact entry. **This pass's headline finding, missed
   by the earlier lighter treatment**: `AddInstance(const char *V, const char
   *shipcode, const char *media_id)` never references its own `shipcode`
   parameter — it builds and dedups its `VerShipcodes` entry from `V` (the
   bare version) alone, despite the member name and `Sort()`'s
   `pimCompareVerShipcode()` comparator both implying a combined
   version+shipcode identifier is intended. A 2nd `AddInstance()` call for
   the same version under a different shipcode is silently treated as a
   duplicate and its media ID is dropped — a shipcode option can silently go
   missing from `pimEntitlementTree`'s version/shipcode dropdown as a
   result. See `ai-context/business_rules.yaml`'s
   `pimavailableproduct_addinstance_ignores_shipcode` and
   `docs/classes/pimAvailableProduct.md`.
17. ~~`pimGetAvailableProducts` at the same depth~~ — **already covered**:
   this request was answered without new work, since `pimGetAvailableProducts`
   is the inseparable collection class documented together with
   `pimAvailableProduct` in item 16 above (both live in the same 2 files and
   were analyzed together) — see `docs/classes/pimAvailableProduct.md` for
   its Members/APIs/Called By/Risk Analysis.
18. ~~`Cmp_cStrings` at the same depth~~ — **done**: **not documentable at
   full depth in the usual sense** — this `btkMap` comparator, flagged as
   "declared but not defined" while documenting `pimAvailableProduct`, has
   **no implementation anywhere in this archive**, confirmed by an
   exhaustive case-insensitive grep across `pim/`, `pim_core/`, `pim_ui/`,
   and `pim_util/` (exactly 8 total matching lines). **CONFIRMED**:
   independently forward-declared via a local `extern` statement in 4
   separate `.cxx` files (`upimDlg.cxx`, `pimsilentDlg.cxx`,
   `pimEntitlement.cxx`, `pimGetAvailableProduct.cxx`) with no shared header
   — a maintenance hazard with no compiler-enforced single source of truth
   if its signature ever changes. **CONFIRMED**: `pimsilentDlg.cxx`'s
   declaration is dead/unused. Its likely purpose (a `strcmp()`-style
   `char*` comparator) is explicitly labeled **inferred, not confirmed** —
   there is no body to verify it against. No Doxygen patch was created (no
   header exists to annotate). See `ai-context/business_rules.yaml`'s
   `cmp_cstrings_no_implementation_and_redeclared_4_times` and
   `docs/classes/Cmp_cStrings.md`.
19. ~~`pimAvailableProduct`'s `Print()` at the same depth~~ — **done**: a
   dedicated pass on `Print()` specifically (both
   `pimAvailableProduct::Print()` and `pimGetAvailableProducts::Print()`,
   which fans out to it) confirmed **both are dead code**: zero live
   callers anywhere in this archive. An exhaustive grep for `.Print(` across
   `pim`, `pim_core`, and `pim_ui` finds exactly 3 matches total —
   `pimGetAvailable.cxx:474` and `:478`
   (`//AvailableProductsArray.Print(pimDbgLog);`), **both commented out**,
   and one unrelated `Test.Print(output_file)` call on a completely
   different object in `pim/pim_src/pimTop.cxx:3042`. `pimDbgLog` itself —
   the argument these dead calls would pass — is referenced nowhere else in
   this codebase outside those same 2 disabled lines and a 3rd commented
   `//pimDbgLog << endl;` at `pimGetAvailable.cxx:564`, so it isn't
   confirmed to be a real, live logger object either. Both overloads remain
   fully implemented and callable — this is confirmed-unreachable code, not
   a runtime bug. Updated the annotated `pimGetAvailableProduct.h` and
   regenerated its patch; `pimAvailableProduct`'s own `risk_level` is
   unchanged (the `AddInstance()` bug in item 16 above remains the more
   significant finding). See `ai-context/business_rules.yaml`'s
   `pimavailableproduct_print_dead_code` and
   `docs/classes/pimAvailableProduct.md`.
20. ~~`IsApplicationInInstallList` at the same depth~~ — **done**: a
   dedicated pass on this one free function specifically (already fully
   read while documenting item 15 above) re-confirmed its call-site list is
   unchanged — still exactly the 2 calls, both inside
   `AddMandatoryAppsForSilentInstall()`, its only caller — and added a
   **confirmed structural risk in the opposite direction** from item 15's
   findings: its substring search (`.Pos()`, not exact match), while
   confirmed the one consumer robust to the bare-tag-vs-full-path format
   split, can itself false-positive whenever one product's filename is a
   literal substring of another entry already in `applications_list` (e.g.
   a hypothetical `"newcreobase.xml"` entry would make a check for
   `"creobase.xml"` return `true` even though `"creobase.xml"` itself was
   never requested). Checked pairwise against the 3 real confirmed
   arguments used in this codebase today (`"creobase.xml"`,
   `"creobase.p.xml"`, `"qualityagent.xml"`) — none collides, so this is
   recorded as a confirmed structural risk, not a demonstrated live bug.
   Updated the annotated `pimGeneralInit.h`'s `IsApplicationInInstallList`
   comment and regenerated its patch; `pimGetApplicationsList`'s own
   `risk_level` is unchanged (the `InstallPreReqSilent()` bug in item 15
   above remains the more significant finding). See
   `ai-context/business_rules.yaml`'s
   `isapplicationininstalllist_substring_false_positive_risk` and
   `docs/classes/pimGetApplicationsList.md`.
21. ~~`AddMandatoryAppsForSilentInstall` at the same depth~~ — **done**: a
   dedicated pass on this function (`pim/pim_src/pimTop.cxx:1282-1316`,
   already fully read while documenting item 15 above) **corrected** item
   15's "commonly EMPTY" framing of `InstallPreReqSilent()`'s input list —
   since this function always runs first in the one confirmed call path and
   unconditionally *attempts* to inject `creobase.xml` (and, if present,
   `qualityagent.xml`), the list is typically non-empty; the real, more
   precisely confirmed consequence is a **size mismatch**, not usually a
   fully empty list. Surfaced 2 new confirmed bugs: (1) the
   `qualityagent.xml` injection block, including its own independent
   `qualityagent.xml` existence check, is nested entirely inside
   `creobase.xml`'s existence check, so quality agent is silently **not**
   auto-installed on any media shipping `qualityagent.xml` without
   `creobase.xml` — contradicting this function's own name and the
   "Mandatory Installation of Quality Agent" intent independently recorded
   in `pimGeneralInit.h`'s and `pimTop.cxx`'s own `13-Aug-26` changelog
   entries (`$$34`/`$$148`); and (2) `-allpacks` mode's utility-XML entries
   are appended only to `pimSilentInstallFromXML()`'s own local list, never
   via `pimAddApplicationsToList()`, so they never reach the shared
   `applications_list` `InstallPreReqSilent()` later independently re-reads
   — every `-allpacks` utility entitlement's prerequisites are silently
   never checked. This function has **no header declaration anywhere in
   this archive** (there is no `pimTop.h`) — no Doxygen patch was created
   for it, the same situation as `Cmp_cStrings` (item 18 above). See
   `ai-context/business_rules.yaml`'s
   `addmandatoryappsforsilentinstall_qualityagent_gated_on_creobase`,
   `addmandatoryappsforsilentinstall_allpacks_invisible_to_installprereqsilent`,
   and `docs/classes/pimGetApplicationsList.md`.
22. ~~`pimSilentTestXmlIsUseable` at the same depth~~ — **done**: **not a
   class** — a 7-function free-function group
   (`pimSilentTestXmlIsUseable`/`pimIsProductXmlMatch`/`pimIsVersionMatch`/
   `pimSilentCanMatchNeeds`/`pimSilentFixupPSF`/`pimSilentFixupShortcuts`/
   `pimSilentCreateEntitlement`) in `pim_core/includes/pimSilent.h` +
   `pim_core/pim_core_src/pimSilent.cxx` (both read in full), implementing
   the silent-install XML-match-and-merge pipeline
   `pimSilentInstallFromXML()`'s per-entry loop drives immediately after
   item 15's population logic. **Confirmed findings**:
   `pimSilentTestXmlIsUseable()` leaks a `pimXmlFile` object on its
   XML-syntax-error path — the `delete` that runs on the success path is
   placed after the error-check block, so the early `return false;` inside
   it skips the cleanup. More severely, `pimSilentCanMatchNeeds()` leaks
   **both** of its 2 internal `pimXmlFile` objects whenever either input
   file fails to parse — every internal early-return and the final success
   return correctly delete both first, only the outer failure fallthrough
   does not, a clean, well-evidenced contrast. `pimSilentFixupShortcuts()`
   in this same file feeds the already-documented caller-side indexing bug
   in `pimShortcutMgr` (see that class's own entry above). This header was
   small enough for complete, all-7-function annotation — no coverage gaps
   to disclose. See `ai-context/business_rules.yaml`'s
   `pimsilenttestxmlisuseable_leaks_pimxmlfile_on_syntax_error`,
   `pimsilentcanmatchneeds_leaks_2_pimxmlfiles_on_parse_error`, and
   `docs/classes/pimSilent.md`.
23. ~~`pimSilentFixupPSF` at the same depth~~ — **done**: a dedicated pass on
   this one free function (already fully read while documenting item 22
   above) re-confirmed its call-site list is unchanged — still exactly 1
   call, inside `pimSilentCreateEntitlement()`, its only caller — and traced
   its dependency on `pimCommandMgr::DeleteCommand()`/`CanDeleteCommand()`'s
   explicitly documented "cannot delete the last command of a type"
   constraint (`pim_core/pim_core_src/pimCommandMgr.cxx:405-451,454-515`) to
   surface 2 new confirmed findings. **Confirmed bug**: the `pDiUmMMY<N>`
   dummy placeholder this function creates to work around that constraint —
   so a name colliding with a different license type in `Eb` can be
   removed — can itself become permanently stuck/undeletable in `Eb`'s
   final PSF/command set, if no other surviving command shares its license
   type by the time the final cleanup loop runs; the same "last of a type"
   rule then silently blocks deleting the dummy too, since neither of this
   function's relevant `DeleteCommand()` calls checks the return value.
   **Confirmed discrepancy**: the collision-handling code's own inline
   comment says it removes "the foobar command" (singular), but it actually
   deletes **every** command of the colliding license type via
   `GetAllCommandNamesByType()`. See `ai-context/business_rules.yaml`'s
   `pimsilentfixuppsf_dummy_placeholder_can_get_stuck`,
   `pimsilentfixuppsf_collision_deletes_entire_type_not_just_colliding_name`,
   and `docs/classes/pimSilent.md`.
24. ~~`pimSilentCanMatchNeeds` at the same depth~~ — **done**: a dedicated
   pass on this one free function (already fully read while documenting
   item 22 above) re-confirmed its call-site list is unchanged — still
   exactly 1 call, inside `pimSilentTestXmlIsUseable()`, its only caller —
   and traced its dependency on `pimPackageMgr::GetInstallPackageNames()`
   (`pim_core/pim_core_src/pimPackageMgr.cxx:611-666`) to surface a new
   confirmed finding. **Confirmed bug**: this function's package-
   availability check silently changes meaning depending on 3 global,
   process-wide command-line mode flags
   (`pimGetBasePackMode()`/`pimGetCreoNGCRIMode()`/`pimGetAllPacksMode()`)
   unrelated to the specific `A`/`B` XML pair being compared — this
   function's own header comment ("returns false if any packages marked for
   install in A are not known in B") only describes the default,
   no-special-mode case, confirmed by direct contrast with its
   platform/language sibling checks in the same function
   (`pimPlatformMgr::GetInstallPlatformNames()`/
   `pimLanguageMgr::GetInstallLanguageNames()`), which have no such
   dependency. Under `-allpacks` the check becomes **stricter** (every
   package `A`'s XML declares must exist in `B`, regardless of `A`'s own
   `install="Y"` markings); under `-basepack` it becomes **looser** (only
   `required="Y"` packages are checked, ignoring `A`'s actual `install="Y"`
   selections). **Corrected by item 26 below**: this is confirmed to be by
   design, not a validation gap. See `ai-context/business_rules.yaml`'s
   `pimsilentcanmatchneeds_package_check_mode_dependent` and
   `docs/classes/pimSilent.md`.
25. ~~`pimSilentFixupShortcuts` at the same depth~~ — **done**: a dedicated
   pass on this one free function (already fully read while documenting
   item 22 above, and already partially cited from the caller side in
   `docs/classes/pimShortcutMgr.md`'s Risk Analysis for a confirmed
   `B_avail[i]`-vs-`A_wants[i]` indexing bug on its 4 `SetShortcut*State()`
   calls) re-confirmed its call-site list is unchanged — still exactly 1
   call, inside `pimSilentCreateEntitlement()`, its only caller — and found
   a **2nd, distinct confirmed bug** in this same function. Its
   `GetShortcutProgramMenu()`/`SetShortcutProgramMenu()` pair ignores the
   `Get`'s return value and calls the `Set` unconditionally;
   `pimShortcutMgr::GetShortcutProgramMenu()` leaves its output untouched
   when a shortcut has no `<PROGRAMSMENU>` child, and since the same
   `btkString` variable is declared once outside the loop and reused every
   iteration, a stale Program Menu group name from an earlier, unrelated
   shortcut can be copied onto the current shortcut (or blanked to empty,
   on the first iteration) — confirmed by direct contrast with the
   correctly return-value-guarded
   `GetShortcutStartDir()`/`SetShortcutStartDir()` call 3 lines later in the
   same function. See `ai-context/business_rules.yaml`'s
   `pimsilentfixupshortcuts_stale_programmenu_copy` and
   `docs/classes/pimSilent.md`.
26. ~~`pimSilentCreateEntitlement` at the same depth~~ — **done**: a
   dedicated pass on this one free function (already fully read while
   documenting item 22 above) re-confirmed its call-site list is unchanged
   — still exactly 1 call, immediately after a successful
   `pimSilentTestXmlIsUseable()` check — and found a finding that
   **corrects item 24 above**. Its own package-selection loop (`:576-586`)
   calls the **identical** `pimPackageMgr::GetInstallPackageNames()` on the
   same `A`, under the same 3 global mode flags, that
   `pimSilentCanMatchNeeds()` uses for its compatibility check — and since
   this function only ever runs immediately after a successful
   `pimSilentCanMatchNeeds()` call on the same `A`/`B` pair within the same
   process, the 2 calls are guaranteed to see identical mode flags and
   therefore an identical "wanted" package set. This means item 24's
   "`-basepack` validation gap"/"`-allpacks` overly strict" characterization
   was too strong: both are confirmed to be **by design**, since
   `pimSilentCanMatchNeeds()` validates exactly the package set
   `pimSilentCreateEntitlement()` will subsequently attempt to install — the
   2 functions are consistent with each other, not in conflict. The one
   part of the original finding that still stands: both functions' own
   header comments describe a plain `install="Y"` check, an incomplete
   description of the actual, mode-dependent behavior — a documentation
   accuracy issue, not a behavioral one. No new headline bug was found
   specific to this function's own control flow beyond the already-noted
   lower-confidence `<MSI>`-node last-value-wins caveat (see Extension
   Points in `docs/classes/pimSilent.md`). See
   `ai-context/business_rules.yaml`'s
   `pimsilentcreateentitlement_package_selection_matches_canmatchneeds` and
   `docs/classes/pimSilent.md`.
27. ~~`pimIsProductXmlMatch` at the same depth~~ — **done**: a dedicated pass
   on this one free function (already fully read while documenting item 22
   above) re-confirmed its call-site list is unchanged — still exactly 2
   calls, both inside `pimSilentTestXmlIsUseable()`/`pimIsVersionMatch()`,
   no external caller — and found a **confirmed structural limitation
   shared identically by `pimIsVersionMatch()`** (which duplicates the exact
   same control-flow shape for its own `Ea`/`Eb` initialization): neither
   function has a cross-schema fallback for `B`. `pimEntitlement::Init(path,
   root_match)` requires the XML's root element to be exactly `<PRODUCT>`
   when `root_match` is `NULL`, or exactly `<EXTERNAL_INSTALLER>` when
   `root_match` is that string — a document can only satisfy one. Whichever
   schema succeeds for `A` is the **only** one ever tried for `B` within
   that same call; if `B`'s `Init()` fails under that schema, the function
   returns `false` without ever attempting the other schema for `B`. A
   mixed-schema `A`/`B` pair (one `<PRODUCT>`-rooted, one
   `<EXTERNAL_INSTALLER>`-rooted) is confirmed to always report "no match"
   even with identical `<TAG>`/version/shipcode values, surfaced as
   `PIM_SOFTWARE_NOT_FOUND`. Not confirmed reachable with any real
   product/media XML pair in this archive (no sample
   `<EXTERNAL_INSTALLER>`-rooted XML exists to test), but the mechanism
   itself is fully confirmed from both functions' own control flow and
   `pimEntitlement::Init()`'s documented root-tag matching contract. See
   `ai-context/business_rules.yaml`'s
   `pimisproductxmlmatch_no_cross_schema_fallback` and
   `docs/classes/pimSilent.md`.
28. ~~`pimSilentFixupPSF`'s dummy-placeholder bug at the same depth~~ —
   **done**: a further, dedicated re-scrutiny of one already-documented
   finding (item 23's stuck-`pDiUmMMY<N>`-dummy bug), not a new function.
   **Reachability, precisely characterized**: the dummy survives exactly
   when no entry in `A`'s final wanted command set independently declares
   its own license type as the colliding command's *old* type in `B` — a
   plausible, non-degenerate scenario (e.g. a single command's license type
   changing between product versions), not merely a contrived edge case.
   **Downstream consequence, newly traced**: `pimEntitlement::InstallScripts()`
   (`pim_core/pim_core_src/pimEntitlement.cxx:5627-5631`) constructs a
   `pimScriptLoop` (one of the already-documented `pimLoop` subclasses)
   directly on the **same** `pimXmlFile*` document `pimSilentFixupPSF()`
   mutates — confirmed object identity, at real install-execution time.
   `pimScriptLoop::OnInstall()` (`pim_core/pim_core_src/pimScriptLoop.cxx:71-127`)
   reads the *first* name `GetAllCommandNames()` returns — unconditionally,
   with no type-based selection — to populate `[LM_LICENSE_FILE]` for that
   entitlement's generated install scripts, so a stuck dummy landing at that
   position would corrupt `[LM_LICENSE_FILE]` for the entire entitlement
   using its own internally-mismatched license-identifier data, not just
   leave an inert PSF entry behind. Whether the dummy reliably reaches that
   position in any real product XML is not confirmed either way in this
   archive; the mechanism connecting the 2 functions via the shared XML
   document is fully confirmed. See `ai-context/business_rules.yaml`'s
   `pimsilentfixuppsf_dummy_placeholder_can_get_stuck` and
   `docs/classes/pimSilent.md`.
29. ~~`pimSilentFixupShortcuts`'s stale-Program-Menu-copy bug at the same
   depth~~ — **done**: a further, dedicated re-scrutiny of one
   already-documented finding (item 25's `GetShortcutProgramMenu()`/
   `SetShortcutProgramMenu()` stale-value bug), not a new function.
   **Reachability, confirmed**: `<PROGRAMSMENU>` is confirmed to be an
   **independently optional** per-shortcut child element —
   `pim_core/pim_core_src/pimShortcutLoop.cxx:88-341` parses it alongside
   `<STARTMENU>`/`<DESKTOP>`/`<QUICKLAUNCH>` as sibling children of
   `<SHORTCUT>` in the same `else if` chain, each independently
   present-or-absent (a shortcut offered only via Desktop/Quicklaunch
   legitimately has no `<PROGRAMSMENU>` node at all) — so this bug's trigger
   condition is a normal configuration, not a contrived edge case.
   **Downstream consequence, newly traced, concrete and deterministic**
   (unlike item 28's document-order-dependent caveat):
   `pimEntitlement::InstallShortcuts()`
   (`pim_core/pim_core_src/pimEntitlement.cxx:5778-5787`) calls
   `pimShortcuts::GetInstance().Create(xmlPtr)` on the **same** `pimXmlFile*`
   document `pimSilentFixupShortcuts()` mutates — confirmed object identity,
   at real install-execution time — constructing a `pimShortcutLoop`
   (another already-documented `pimLoop` subclass) on it.
   `pimShortcutLoop::OnInstall()`
   (`pim_core/pim_core_src/pimShortcutLoop.cxx:324-326,654-671`) reads that
   shortcut's `<PROGRAMSMENU>` text and uses it **directly as a filesystem
   subfolder path** (`location /= (cStringT)programsmenu;`) to place the
   installed `.lnk` file. Confirmed consequences: a stale non-empty value
   misplaces the current shortcut's icon into a **different,
   earlier-processed shortcut's** Start Menu folder; a stale empty value
   (first loop iteration) causes the Program-Menu placement to be **silently
   skipped entirely**, even if this shortcut's own "create in Programs Menu"
   flag was set `true` — a visible, user-facing install defect, not merely a
   data-integrity concern. See `ai-context/business_rules.yaml`'s
   `pimsilentfixupshortcuts_stale_programmenu_copy` and
   `docs/classes/pimSilent.md`.
30. ~~`pimIsProductXmlMatch`'s structural limitation at the same depth~~ —
   **done**: a further, dedicated re-scrutiny of one already-documented
   finding (item 27's no-cross-schema-fallback structural limitation, shared
   identically by `pimIsVersionMatch()`), not a new function.
   **Reachability, more precisely characterized**: this codebase's **own**
   other 2 product-XML resolution call sites do **not** assume a fixed
   schema for a single file — `pim_core/pim_core_src/pimEntitlement.cxx:634-635`
   (resolving a product's own installed `.p.xml`) tries `Init(file, NULL) ||
   Init(file, "EXTERNAL_INSTALLER") || Init(file, "HIDDEN_PRODUCT")` in
   sequence, and `pimEntitlement.cxx:967-968` (resolving a `<PREREQUISITE>`
   reference) tries `Init(x) || Init(x, "EXTERNAL_INSTALLER")`. Both confirm,
   elsewhere in this exact codebase, that a product's root schema is treated
   as unpredictable and deliberately checked for by trying multiple schemas
   — `pimIsProductXmlMatch()`/`pimIsVersionMatch()` are the only 2 functions
   in this file that instead silently assume `A` and `B` share whichever
   single schema matched `A` first. This is direct, confirmed evidence — not
   mere speculation — that this codebase's own design does not guarantee a
   product's on-media XML (`B`) uses the same root schema as the user's
   silent-install request XML for that same product (`A`).
   **Downstream consequence, newly traced**: `pimIsProductXmlMatch()`'s only
   caller is `pimSilentTestXmlIsUseable()` (`pimSilent.cxx:54`), whose only
   caller in turn is `pimSilentInstallFromXML()`'s per-file loop
   (`pim/pim_src/pimTop.cxx:1532`). When a mixed-schema false-negative makes
   `pimSilentTestXmlIsUseable()` return `false`, the `pimTop.cxx` caller's
   `if` block at `:1532` is simply skipped for that entry — no `else`
   branch, `abort` is **not** set `true`, and the outer loop continues to
   the next entry: that product silently receives **no entitlement at
   all**. Since `pimSilentInstallFromXML()`'s own final return value is
   `pimGetLastError()` (`pimTop.cxx:2073`, `pim/pim_src/pimExit.cxx:16-33`)
   — a process-wide static array's **last** appended error, not a per-file
   record — a mixed-schema false-negative on one product in a multi-XML
   batch silent install can be silently **overwritten** in the reported
   exit code by any later, unrelated error, losing which specific product
   failed the match and why. See `ai-context/business_rules.yaml`'s
   `pimisproductxmlmatch_no_cross_schema_fallback` and
   `docs/classes/pimSilent.md`.
31. ~~`pimSilentCanMatchNeeds`'s package check mode-dependent finding at the
   same depth~~ — **done**: a further, dedicated re-scrutiny of one
   already-documented finding (item 24's package-check mode-dependent
   finding, later corrected — not overturned — by item 26's
   `pimSilentCreateEntitlement()` consistency finding), not a new function.
   **Reachability, more precisely characterized**: the 3 mode flags are
   **not** a clean, mutually exclusive 3-way choice. `-allpacks`/`-basepack`
   **are** enforced mutually exclusive by the CLI parser itself
   (`pim/pim_src/pimGeneralInit.cxx:309-320`, each `else if` branch clears
   the other), but `-releaselink` (`pimGetCreoNGCRIMode()`) has **no such
   interaction with either** — a real command line can freely combine
   `-releaselink -basepack`, silently making BasePack's definition win in
   `GetInstallPackageNames()`'s `if`/`else if` chain (BasePack checked
   first, CreoNGCRI second) while `pimGetCreoNGCRIMode()` remains fully
   active for every *other* purpose elsewhere in the codebase (10+
   independent call sites in `pim_ui`) — a confirmed, reachable interaction
   via ordinary command-line flags, not a hypothetical one.
   **Downstream consequence, newly traced**:
   `pimPackageMgr::SetPackageInstallState()`
   (`pim_core/pim_core_src/pimPackageMgr.cxx:250-336`) writes the
   mode-dependent package selection directly onto the `<PACKAGE>` node's own
   `install="Y"`/`"N"` attribute on `Eb`'s shared `xmlPtr`.
   `pimMSILoop::IsEligibleForInstall()`
   (`pim_core/pim_core_src/pimMSILoop.cxx:937-969`) reads that **same**
   attribute at real install-execution time — via
   `pimEntitlement::InstallMSI()` constructing a `pimMSILoop` (already a
   fully documented `pimLoop` subclass) through
   `pimMSICopier::MSIInstall()`/`MSICopy_low()` on this same `xmlPtr` — to
   decide whether a package's MSI feature/CDSECTION is **actually,
   physically installed**. This confirms the mode-dependent logic is the
   literal, unbroken, real-world determinant of installed product features,
   not merely a validation/documentation-accuracy concern. See
   `ai-context/business_rules.yaml`'s
   `pimsilentcanmatchneeds_package_check_mode_dependent` and
   `docs/classes/pimSilent.md`.
32. ~~`pimSilentCreateEntitlement`'s MSI-node last-value-wins caveat at the
   same depth~~ — **done**: a further, dedicated re-scrutiny of one
   already-documented finding (item 26's `<MSI>`-node property-copy loop
   caveat, previously flagged only as a lower-confidence structural note,
   "not a demonstrated bug"), not a new function. **Upgraded to a CONFIRMED
   BUG.** **Reachability, confirmed** (not merely hypothetical, upgrading
   the prior "no product XML available to test" caveat): `pimMSILoop::pimMSIExec()`
   (`pim_core/pim_core_src/pimMSILoop.cxx:154-161`, the actual MSI
   install-execution driver, already a fully documented `pimLoop` subclass)
   independently iterates **every** `<MSI>` node in the document via the
   identical `getElementsByTagName(pimMSI)` call, skipping only
   `IsEligibleForInstall()` rejects, and executes each eligible node as its
   own separate MSI install action — confirming multi-`<MSI>`-node product
   XML is a real, designed-for, normally-executed configuration, not a
   hypothetical edge case. `pimMSIExec()`'s own `<MSIARGUMENT>` reading even
   correctly concatenates **all** `<MSIARGUMENT>` children of a single node,
   by contrast with this function's cruder single-value
   `GetChildNodeByNodeName()` read. **Downstream consequence, newly
   traced**: the single `format`/`Cmd` value pair this function's loop
   stamps onto **every** `<MSI>` node in `B` (only the **last** `<MSI>`
   node's values survive being read from `A`) is exactly what
   `pimMSILoop::pimMSIExec()` reads **per node** at real install-execution
   time — `attribFormat` (`pim_core/pim_core_src/pimMSILoop.cxx:596-614`)
   directly selects that node's install **UI mode** (`"full"` interactive
   wizard, `"basic"`, or silent), and the `<MSIARGUMENT>` text feeds that
   node's `msiexec.exe` command line. If `B` legitimately has 2+ MSI
   packages meant to install under different UI modes or arguments, this
   bug forces all of them to adopt whichever single mode/argument pair
   belonged to the last `<MSI>` node read from `A` — a confirmed,
   install-time consequential data-corruption bug that can visibly change
   which installer UI a user sees during a multi-package MSI install, not
   merely a latent XML-consistency concern. See
   `ai-context/business_rules.yaml`'s
   `pimsilentcreateentitlement_msi_node_last_value_wins` and
   `docs/classes/pimSilent.md`.
33. ~~`pimSilentFixupPSF`'s collision-deletes-entire-type discrepancy at the
   same depth~~ — **done**: a further, dedicated re-scrutiny of one
   already-documented finding (item 23's collision-deletes-entire-type
   discrepancy), not a new function. **Reachability, precisely
   characterized**: this function's own opening "Drop" step
   (`pim_core/pim_core_src/pimSilent.cxx:361-365`) already deletes every
   original command in `B` before the collision-handling block ever runs —
   but `pimCommandMgr::CanDeleteCommand()`'s live "last of a type" count
   (`pim_core/pim_core_src/pimCommandMgr.cxx:454-515`, counting `<PSF>`
   nodes by `<LICTYPE>` at call time) makes that Drop loop deterministically
   leave **exactly 1 surviving command per originally distinct license
   type**. A given `lictype2`'s **first** collision can therefore only ever
   delete that one straggler — matching the comment's singular framing; the
   discrepancy only actually manifests when 2+ `A_wants` entries in the
   same call independently collide against different `B` commands that
   originally shared the same `lictype2`, a real but narrower scenario than
   "any non-colliding command of that type" suggests. **Downstream
   consequence, newly traced** — an internal interaction with item 28's
   stuck-dummy bug, not an external consumer: because each later same-type
   collision's delete sweep runs before its own new dummy exists, it
   deletes the *previous* collision's straggler/dummy while its own dummy
   survives — an unintentional cleanup mechanism for every dummy but the
   **last** one created per shared `lictype2`. This **narrows, without
   retracting**, item 28's stuck-dummy finding: the `pimScriptLoop`
   `[LM_LICENSE_FILE]` corruption risk is confirmed to apply to at most 1
   dummy per originally-distinct license type per call, not every dummy
   this function creates. See `ai-context/business_rules.yaml`'s
   `pimsilentfixuppsf_collision_deletes_entire_type_not_just_colliding_name`
   and `docs/classes/pimSilent.md`.
34. ~~`pimSilentCreateEntitlement`'s `<PROPERTY>` skip list at the same
   depth~~ — **done**: a further, dedicated re-scrutiny of one
   already-documented mechanism (item 26's `<PROPERTY>` skip list,
   `[SHIPCODE]`/`[VERSION]`/`[SOURCE]`/`CustomActions`), not a new function.
   Traced a distinct, confirmed downstream reason for each of the 4 skipped
   names. **`[SHIPCODE]`**: read immediately after `Eb`'s creation by
   `pimSilentInstallFromXML()` (`pim/pim_src/pimTop.cxx:1556-1559`) to
   populate the session's own `SHIPCODE_PROPERTY` from `Eb`'s own value —
   must reflect which physical media/build `B` actually is, not whatever
   `A`'s request XML declares. **`[VERSION]`**: originally set by
   `pimEntitlement::Init()` itself (`pim_core/pim_core_src/pimEntitlement.cxx:944,1195`)
   from `Eb`'s own `<PRODUCT version>` attribute, later read by
   `pimCustomActionsLoop` (`pim_core/pim_core_src/pimCustomActions.cxx:542`)
   for version-gated custom-action behavior and by `pimSessionInfo`
   (`pim_core/pim_core_src/pimSessionInfo.cxx:1801,2316`) for
   cross-entitlement version matching. **`[SOURCE]`**: unconditionally
   overwritten anyway moments after this function returns
   (`pim/pim_src/pimTop.cxx:1598`). **`CustomActions`**: gates **many**
   real install/uninstall lifecycle hook points via
   `pimEntitlement::OnInstallCustomActions()`/`OnUninstallCustomActions()`'s
   `xmlPtr->GetProperty("CustomActions", str)` truthy-check
   (`pim_core/pim_core_src/pimEntitlement.cxx:5151,5882,6024,6669,6753,6837,6842,7023,7159,7228,7309`),
   each constructing a `pimCustomActionsLoop` — an **11th, newly-discovered
   `pimLoop` subclass** (`pim_core/includes/pimCustomActions.h` +
   `pim_core_src/pimCustomActions.cxx`) not previously mentioned anywhere in
   this documentation set — on the same `xmlPtr`; the property's mere
   presence, not even its value, gates whether `B`'s own custom-action
   fixups run at all. Also **verified a non-issue, not a bug**: the copy
   loop's `name` variable is declared once outside the loop, so a
   `<PROPERTY>` node lacking a `name` attribute would leave `name` stale —
   but the copy loop's own guard (`attribName != NULL`) means 0 copies
   happen either way, so the stale value has no effect, unlike the
   superficially similar but genuinely live bug already documented in
   `pimSilentFixupShortcuts()`'s Program Menu handling. See
   `ai-context/business_rules.yaml`'s
   `pimsilentcreateentitlement_property_skip_list_confirmed_correct` and
   `docs/classes/pimSilent.md`.
35. ~~`pimIsVersionMatch`'s shipcode comparison at the same depth~~ —
   **done**: a further, dedicated re-scrutiny of one already-documented
   mechanism (item 27's shared no-cross-schema-fallback limitation touched
   on `pimIsVersionMatch()`'s own version/shipcode logic without examining
   the shipcode comparison itself), not a new function. **Confirmed, not
   enforced**: the shipcode check only runs if **both** `A` and `B` expose
   a shipcode (`pimEntitlement::GetShipcode()`,
   `pim_core/pim_core_src/pimEntitlement.cxx:2066-2090`, returns `false` if
   the root `<PRODUCT>` node has neither `appshipcode` nor `shipcode`).
   `pimEntitlement::Init()` (`pim_core/pim_core_src/pimEntitlement.cxx:846-870`)
   only requires `tag`/`version` — not `shipcode`/`appshipcode` — so a
   valid, `Init()`-succeeding product XML can legitimately omit both,
   silently bypassing the gate; not confirmed against any specific real
   product/media XML in this archive (none exists), but the mechanism is
   fully confirmed from `Init()`'s own attribute requirements.
   **Downstream consequence, newly traced**: when skipped this way, the
   function reaches `return true;` via the **exact same path** as an
   explicit shipcode match — nothing distinguishes "ran and passed" from
   "never ran" (no log, no warning, no property set), and
   `pimSilentTestXmlIsUseable()`'s differentiated-error logic
   (`Major`/`Minor`) is only ever inspected on the *rejected* path, so it's
   never consulted when the check was silently skipped. **2 verified
   non-issues**: `Minor` is only set `true` when shipcodes are exactly
   equal, not when `B`'s is legitimately newer (the function's own
   documented common case) or when the check is skipped — but since
   `Major`/`Minor` are only read on the rejected path, this has no effect;
   and `pimCompareShipcode()`'s parameter names (`new_ship`/`old_ship`)
   don't reflect an enforced argument-order contract — confirmed via its
   only other call site (`pimEntitlement::GetSize()`) to be a generic,
   symmetric comparator. See `ai-context/business_rules.yaml`'s
   `pimisversionmatch_shipcode_check_optional_not_enforced` and
   `docs/classes/pimSilent.md`.
36. ~~`pimSilentCanMatchNeeds`'s platform and language checks at the same
   depth~~ — **done**: a further, dedicated re-scrutiny of one
   already-documented mechanism (item 24's platform/language checks,
   previously only characterized as "straightforward `install=\"Y\"`
   lookups" in contrast to the package check's mode-dependence), not a new
   function. **Confirmed genuinely mode-free**:
   `GetInstallPlatformNames()`/`GetInstallLanguageNames()` really are
   plain, unconditional `install="Y"` checks, no global mode flags for
   either. **But confirmed to lack the package check's later-confirmed
   consistency guarantee** (item 26): unlike the package check
   (`pimSilentCreateEntitlement()`'s package-selection loop calls the
   identical `GetInstallPackageNames()` this check uses, guaranteeing
   consistency), platform and language selection has **no such
   guarantee**. **CORRECTED** (found in a further, dedicated pass on item
   37's `Ea`/`Eb` reference-vs-copy semantics, which required re-reading
   `pimSilentCreateEntitlement()`'s full body): this claim was incomplete.
   `pimSilentCreateEntitlement()` (`pim_core/pim_core_src/pimSilent.cxx:549-569`)
   **does** mirror `A`'s platform/language onto `B` first, via the
   **identical** `GetInstallPlatformNames()`/`GetInstallLanguageNames()`
   calls this check uses — the same consistency pattern already confirmed
   for package selection, not an absent one. The real, narrower gap is
   that this internally-consistent mirroring is **subsequently undone** by
   a *second*, independent write — everything below remains accurate and
   describes that 2nd write, not a gap in `pimSilentCreateEntitlement()`
   itself. `pimSilentInstallFromXML()`'s post-creation code
   (`pim/pim_src/pimTop.cxx:1602-1635`, run **after** this validation and
   after `pimSilentCreateEntitlement()`'s own consistent mirroring)
   re-derives what actually gets selected on `Eb` from entirely different
   inputs: **platform** via
   `pimPlatformMgr::InitPlatformState(NULL)`
   (`pim_core/pim_core_src/pimPlatformMgr.cxx:104-125`), which derives the
   platform from `btkGetPlatform()` — the current machine's own runtime
   OS — with a 2-level fallback chain (`i486_nt`/`x86e_win64`); **language**
   via 9 hardcoded `SetLanguageInstallState()` calls
   (`pim/pim_src/pimTop.cxx:1608-1627`) gated by
   `pimShouldWeInitLanguageID()` (`pim/pim_src/pimGeneralInit.cxx:531-539`),
   driven by the `-LANG <XX>` command-line flag list
   (`pimGeneralInit.cxx:269-273`) or `-allpacks` (which force-selects all
   9) — independent of `A`'s own `install="Y"` markings. This validation
   never receives the CLI args or the running machine's platform, so it
   structurally cannot validate what `-lang`/`-allpacks`/auto-detection
   will actually select against `B`'s real coverage. **Downstream
   consequence, newly traced**: both `pimTop.cxx` call sites discard the
   return value of `SetLanguageInstallState()`/`InitPlatformState()`, both
   of which fail **silently** (no log, no error) whenever the target
   doesn't exist in `B` — a `-lang XX` (or `-allpacks`) request naming a
   language `B` genuinely lacks is silently dropped with zero diagnostic
   anywhere in the pipeline, and if the auto-detected platform and both
   fallbacks all fail to match anything in `B`, the entitlement silently
   ends up with no platform marked for install at all. See
   `ai-context/business_rules.yaml`'s
   `pimsilentcanmatchneeds_platform_language_selection_diverges_from_validation`
   and `docs/classes/pimSilent.md`.
37. ~~`pimSilentFixupPSF`'s `Ea`/`Eb` reference-vs-copy semantics at the same
   depth~~ — **done**: a further, dedicated re-scrutiny of one
   already-documented function's own parameters (item 22's
   `pimSilentFixupPSF(pimEntitlement& Ea, pimEntitlement& Eb)`), not a new
   function. **Confirmed asymmetric roles**: both parameters are declared
   as symmetric, non-`const` references, but a full call trace confirms
   every `Ea`/`A_cmds` call is a read-only accessor
   (`GetCommandInfoByName()`/`GetAllCommandNames()`), never a mutator,
   while `Eb`/`B_cmds` is the **sole** read-write target
   (`DeleteCommand()`/`AppendAddCommand()`) — `Ea` could safely be
   `const pimEntitlement&`. **Confirmed ephemeral-vs-persistent
   lifetimes**, traced from the sole call site
   (`pimSilentCreateEntitlement()`, `pim_core/pim_core_src/pimSilent.cxx:507-521,736`):
   `Ea` is a genuinely **ephemeral**, stack-allocated `pimEntitlement`,
   `Init()`'d fresh from `A`'s file path and destroyed (its `xmlPtr`
   explicitly deleted, `pimEntitlement::~pimEntitlement()`,
   `pim_core/pim_core_src/pimEntitlement.cxx:226-234`) the moment
   `pimSilentCreateEntitlement()` returns; `Eb` is **not a copy at all** —
   `pimEntitlement* Eb = pimGetSessionInfo()->GetEntitlement(entitlement_index)`
   points directly at the real, session-owned, **persistent** object
   `AddEntitlement(B)` just created, and `pimSilentFixupPSF(Ea, *Eb)`
   mutates that same object by reference — confirming why every
   already-documented downstream consumer (`pimScriptLoop`,
   `pimShortcutLoop`, `pimMSILoop`) genuinely reads the identical object
   this function wrote to, not a snapshot. **Reachability, precisely
   characterized (a real but confirmed unreachable structural hazard)**:
   `pimEntitlement` owns a raw `xmlPtr` pointer, explicitly `delete`d in
   its destructor, but defines **no custom copy constructor or assignment
   operator** anywhere — relying on compiler-generated (shallow-copy)
   defaults, which would double-free `xmlPtr` if 2 `pimEntitlement`s ever
   shared it via a by-value copy or assignment. An exhaustive search
   (every `pimEntitlement` local/parameter not declared as a pointer or
   reference, across the entire archive) confirms the **only 3**
   stack-allocated, value-type `pimEntitlement` instances anywhere in this
   codebase are all in this same file — `pimIsProductXmlMatch()`'s and
   `pimIsVersionMatch()`'s own `Ea`/`Eb` (item 24, `:110,150`), and
   `pimSilentCreateEntitlement()`'s own `Ea` (item 27, `:507`) — and every
   one confirmed used safely: default-constructed, `Init()`'d as a member
   call, never copy-constructed or assigned from another `pimEntitlement`.
   Every other entitlement anywhere in the codebase is held via
   `dsXArray<pimEntitlement*>` (`pim_core/includes/pimSessionInfo.h:96-97`)
   or a raw pointer, never by value — making the reference-parameter
   choice here the correct, load-bearing design decision that avoids ever
   triggering this latent hazard. **Verified non-issue**: although `Ea` is
   destroyed immediately after this call, every value
   `pimSilentFixupPSF()` copies from it into `Eb` (desc/licIdentifiers/
   features, via `GetCommandInfoByName()`'s `btkString`/`StringXArray`
   out-parameters) is a genuine value copy, not a reference back into
   `Ea`'s own DOM tree — `Eb`'s resulting document holds no dangling
   reference to `Ea` after `Ea` disappears. **Also required a correction
   to item 36 above** (discovered incidentally while re-reading
   `pimSilentCreateEntitlement()`'s full body for this pass — see item 36's
   own "CORRECTED" text). See `ai-context/business_rules.yaml`'s
   `pimsilentfixuppsf_ea_eb_reference_semantics` and
   `docs/classes/pimSilent.md`.
38. ~~`pimSilentCreateEntitlement`'s session-index lookup at the same
   depth~~ — **done**: a further, dedicated re-scrutiny of one
   already-documented function's own out-parameter mechanism (item 27's
   `entitlement_index`), not a new function. **CONFIRMED BUG**: the
   `entitlement_index` out-parameter (`pim_core/pim_core_src/pimSilent.cxx:520`)
   is set to a valid, non-negative index immediately after
   `AddEntitlement(B)` succeeds, but `A`'s own parseability isn't checked
   until `:522-524` — if `Ea.Init(A)` fails under both schemas (normal and
   `EXTERNAL_INSTALLER`), the function returns `false` at `:530` without
   resetting `entitlement_index`. Traced the only caller
   (`pim/pim_src/pimTop.cxx:1534-1540`):
   ```cpp
   k = -1;
   pimSilentCreateEntitlement(AskedToInstallThisList[i], str, k);   // return value never checked
   if (k < 0) { abort = true; return pimGetLastError(); }
   ```
   this function's own `bool` return value is **never inspected** by its
   only caller — `k`'s sign is the **sole** failure-detection channel.
   Since `k` is already overwritten to a valid, non-negative index at
   `:520` — before `A`'s parse is verified — a failure on the `:522-530`
   path leaves `k >= 0` even though the function is reporting failure, so
   `if (k < 0)` **cannot** catch this specific failure mode. `B` remains,
   permanently, in the session's live `EntitlementArray` at that index
   (`AddEntitlement(B)` already succeeded and is never rolled back), but
   was never fixed up with `A`'s requested PSF/shortcut/package data — the
   function returns before ever reaching
   `pimSilentFixupPSF()`/`pimSilentFixupShortcuts()` (`:736,739`) — so the
   caller proceeds to treat a half-created, unrequested-content
   entitlement as if creation fully succeeded. **Reachability, precisely
   characterized**: not confirmed reachable via this codebase's own single
   call site — `pimSilentTestXmlIsUseable()` (`pimTop.cxx:1532`, called
   immediately before, gating entry into this branch, on the identical `A`
   input) already requires `pimIsProductXmlMatch()`'s internal
   `Ea.Init(A, NULL) || Ea.Init(A, "EXTERNAL_INSTALLER")` to succeed for
   the same file before this call is ever reached — so by the time this
   function runs, `A`'s parseability under one of the same 2 schemas is
   already established, synchronously, in the same process. A confirmed
   gap in this function's own error-handling design, latent given the
   current call graph, not a live crash/corruption risk today. **Verified
   non-issue, a related but distinct mechanism**:
   `pimSessionInfo::AddEntitlement(cStringT)`'s own pre-existing-entry
   guard compares a filesystem path against `GetID()` (a product tag), so
   it never actually fires in practice — confirming `AddEntitlement()`
   returning `true` always means a brand-new array entry was truly just
   appended, never a reused one — the session-index lookup's own core
   arithmetic is otherwise sound; only its reporting back to the caller on
   a later failure is broken. See `ai-context/business_rules.yaml`'s
   `pimsilentcreateentitlement_session_index_leaks_on_later_failure` and
   `docs/classes/pimSilent.md`.
39. ~~`pimSilentCanMatchNeeds`'s package check reachability at the same
   depth~~ — **done**: a further, dedicated re-scrutiny of one
   already-documented finding's underlying dependency (item 26's package
   check mode-dependence), not a new function. **CONFIRMED BUG**: traced
   `A_pkg.GetInstallPackageNames()`/`B_pkg.GetAllPackageNames()`
   (`pim_core/pim_core_src/pimSilent.cxx:307-308`) into `pimPackageMgr`
   (`pim_core/pim_core_src/pimPackageMgr.cxx:611-666,550-575`) — neither
   function null-checks a `DOMNamedNodeMap::getNamedItem()` result before
   dereferencing it via `->getNodeValue()`: `GetAllPackageNames()` does
   this for `name` only; `GetInstallPackageNames()` does it for `name`,
   and — depending on which of the 3 global mode flags is active —
   `install`, `required`, or `parent` as well. **CONFIRMED DISCREPANCY, by
   direct contrast within the same class**:
   `pimPackageMgr::GetPackageInfoByName()` (`:113-170`), a sibling method
   in this exact file reading the exact same 4 attributes off the exact
   same `<PACKAGE>` node type, guards every one of them
   (`if (attribRequired) {...}`, `if (attribInstall) {...}`,
   `if (attribParent) {...}`, `if (attribLabel) {...}`) before
   dereferencing — direct, confirmed evidence that this class's own author
   anticipated these attributes can legitimately be absent on a real
   `<PACKAGE>` node, making `GetInstallPackageNames()`/`GetAllPackageNames()`'s
   unguarded access an inconsistency, not a deliberate "always present"
   assumption. **Reachability, precisely characterized (which attribute
   crashes depends on the active CLI mode)**: `install` is dereferenced
   unconditionally in **both default mode and `-allpacks` mode** — it is
   the *left* operand of `||`, so C++'s left-to-right short-circuit
   evaluation means it is always evaluated *before* `pimGetAllPacksMode()`
   is ever consulted, defeating the natural assumption that "install
   everything" mode would tolerate a missing `install` attribute.
   `required` is dereferenced unconditionally whenever `-basepack` or
   `-releaselink` is active, for every `<PACKAGE>` node, not just ones
   ultimately selected. `parent` is dereferenced additionally under
   `-releaselink` specifically, whenever that same node's `required` isn't
   `"Y"`. `name` is dereferenced only for a node the active mode's
   condition has already decided to select. Since this check runs on
   **both** `A` (a user-supplied silent-install request XML) and `B` (the
   matched product definition on the install media) — and since no
   `<PACKAGE>`-node authoring/schema-validation code exists anywhere in
   this archive to confirm these 4 attributes are always populated by
   whatever external tooling builds real product XML — this is **not
   confirmed reachable** against a real product/media XML pair (none
   exists in this archive), but the crash mechanism itself, and the
   internal inconsistency against this same class's own defensively-coded
   sibling, are fully confirmed from source. **Confirmed narrower than the
   platform/language checks' analogous risk**: `pimPlatformMgr`/
   `pimLanguageMgr`'s equivalent functions share the identical unguarded
   `name`/`install` pattern (2 vulnerable attributes each), but lack this
   function's extra `-basepack`/`-releaselink` branching — so the package
   check specifically has the **widest** exposure of the 3 sibling checks
   (4 distinct vulnerable attributes vs. 2). See
   `ai-context/business_rules.yaml`'s
   `pimsilentcanmatchneeds_package_check_crashes_on_null_attribute` and
   `docs/classes/pimSilent.md`.
40. ~~`pimSilentFixupShortcuts`'s stale-Program-Menu-copy fix at the same
   depth~~ — **done**: a further, dedicated pass verifying one
   already-proposed remediation (item 22's stale-Program-Menu-copy bug),
   not a new function. **FIX VERIFIED**: gating `SetShortcutProgramMenu()`
   on `GetShortcutProgramMenu()`'s return value, mirroring the adjacent
   `GetShortcutStartDir()`/`SetShortcutStartDir()` pattern, is **confirmed
   sufficient**. Traced `SetShortcutProgramMenu()`'s own implementation
   (`pim_ui/pim_ui_src/pimShortcutMgr.cxx:169-189`) to confirm precisely
   what *not* calling it leaves behind: it only ever mutates an
   **existing** `<PROGRAMSMENU>` child's text content, found via a
   `while(child)` search loop; when skipped, `Eb`'s shortcut node is
   **not touched at all**, so its own original, media-template-authored
   value (if any) survives completely intact — the semantically correct
   outcome, matching exactly what the already-correct `StartDir` pattern
   already does. No partial or residual corruption case remains. **NEW
   LIMITATION, found in the same pass, shared by the fix and the pattern
   it mirrors alike — not introduced or left unaddressed by the fix
   itself**: `SetShortcutProgramMenu()` — and
   `pimShortcutMgr::SetShortcutState()` (`:426-454`, the shared primitive
   behind `SetShortcutProgramsMenuState()`'s create/remove toggle) — can
   **only** mutate a child node that already exists on `Eb`'s shortcut;
   neither ever calls `createElement()`/`appendChild()` to add a missing
   `<PROGRAMSMENU>` node. Confirmed identical in `SetShortcutStartDir()`
   (`:213-233`) itself — the "already correct" reference pattern shares
   this exact same structural ceiling, previously unexamined because it
   was only ever cited as the good contrasting example, never scrutinized
   on its own. Consequence: even with the fix applied, if `A`'s request
   XML wants to newly grant Program-Menu placement (or a custom group
   name) to a shortcut whose `B` media template never defined a
   `<PROGRAMSMENU>` child at all, that customization is **silently,
   completely unrealizable** — both `SetShortcutProgramMenu()` (the name
   text) and `SetShortcutProgramsMenuState()` (the enable/disable toggle)
   return `false` and change nothing. This is not a corruption risk and
   not something the proposed fix was ever meant to solve —
   `<PROGRAMSMENU>` being independently optional per shortcut means some
   shortcuts structurally cannot be granted Program-Menu placement via
   this function, by design, regardless of the fix.
   `pimShortcutLoop::OnInstall()`'s own parsing (`pimShortcutLoop.cxx:159-189`)
   confirms this is a silent, non-crashing outcome at install time too: a
   `<PROGRAMSMENU>` child that was never created simply never matches that
   `else if` branch, leaving its local `programsmenu` at its initialized
   default — the shortcut installs with no Program-Menu entry, with no
   error anywhere in the pipeline. A more complete remediation, if this
   narrower customization gap is ever judged worth closing, would have
   `SetShortcutProgramMenu()`/`SetShortcutState()` create the missing
   child node when absent, rather than only the minimal `if`-guard
   verified here. See `ai-context/business_rules.yaml`'s
   `pimsilentfixupshortcuts_stale_programmenu_copy` and
   `docs/classes/pimSilent.md`.
41. ~~`pimSilentCreateEntitlement`'s quality-agent flag copy at the same
   depth~~ — **done**: a further, dedicated pass on one previously
   untouched area of an already-documented function (item 27's
   `pimSilentCreateEntitlement()`), not a new function. **CONFIRMED BUG,
   asymmetric silent override**: traced
   `Ea.IsQualityAgentEnabled(tf)`/`Eb->SetQualityAgent(tf)`
   (`pim_core/pim_core_src/pimSilent.cxx:593-599`) into
   `pimEntitlement::SetQualityAgent()`
   (`pim_core/pim_core_src/pimEntitlement.cxx:3225-3248`) — **enabling**
   (`in==true`) always succeeds whenever `Eb`'s `<QUALITYAGENT>` node
   exists at all, but **disabling** (`in==false`) is silently **refused** —
   the function's own `bool` return stays `false`, `enable` is left
   unchanged — whenever `Eb`'s own node is marked `required="Y"`, a
   business rule about `B`'s own media definition, entirely unrelated to
   what `A` requested. `pimSilentCreateEntitlement()` discards this return
   value at both call sites (`:596,598`), so a user's explicit "disable
   quality agent" request in `A` can be silently overridden by `B`'s own
   required flag, with **zero diagnostic anywhere**. **CONFIRMED, by direct
   contrast with this codebase's own GUI path**:
   `pim_ui/pim_ui_src/pimCustomDlg.cxx:1630-1655` (building the
   "Customize" dialog's QualityAgent checkbox) calls
   `IsQualityAgentRequired()` *first* and, when required, disables ("greys
   out") the "QualityAgentOptIn" checkbox control entirely —
   structurally preventing a live user from ever attempting the disable
   this function blindly attempts; `uiCustomTree::OnUpdate()`
   (`:1727-1730`) only ever calls `SetQualityAgent()` when the checkbox
   was actually interactive. No equivalent precondition check exists in
   the silent-install path. **Reachability, precisely characterized**:
   unlike the package check (protected by `pimSilentCanMatchNeeds()`'s own
   earlier validation), nothing in this pipeline pre-validates
   QualityAgent state at all — `pimSilentCanMatchNeeds()` (`:227-346`)
   never inspects `<QUALITYAGENT>` in either `A` or `B`. A plausible,
   real-world trigger: `A` is an older saved/exported install-request XML
   with quality agent explicitly disabled, applied against `B`, a newer
   product release whose media definition has since made quality agent
   `required="Y"` (a policy change) — the user's prior, explicit opt-out
   is silently discarded on reapplication, with no upstream gate to catch
   it. **CONFIRMED DOWNSTREAM CONSEQUENCE, traced for the first time**:
   `Eb`'s final `enable` state is read at real install-execution time by
   `pimEntitlement::OnInstall()` (`:6499-6509`) and `OnReconfigure()`
   (`:7051-7060`) to write (`pimAppendValue(PtcKey, "QualityAgentOptIn", "1")`)
   or remove (`pimRemoveName(PtcKey, "QualityAgentOptIn")`) a real Windows
   registry value. The GUI's own "PHM" (Product Health Monitoring) message
   key and "legal text" control naming
   (`pimUICustomDlgEnablePHM`/`CustomizeScreenQualityAgentLegalText`)
   confirm this is a genuine, user-facing telemetry/usage-reporting opt-in
   setting with consent implications — not merely a latent
   XML-consistency concern. **Verified non-issue**: the enable direction
   carries no equivalent restriction — a request to *turn on* quality
   agent, or a `B` with no `<QUALITYAGENT>` node at all, is never a live
   corruption risk; only a *disable* request against a *required* `B`
   silently fails. See `ai-context/business_rules.yaml`'s
   `pimsilentcreateentitlement_quality_agent_disable_silently_overridden`
   and `docs/classes/pimSilent.md`.
42. ~~`pimSilentFixupPSF`'s `DeleteCommand` return-value check at the same
   depth~~ — **done**: a further, dedicated pass on one already-documented
   function's own error-handling mechanism (item 22's `pimSilentFixupPSF()`),
   not a new function. Traced all 3 `DeleteCommand()` call sites
   (`pim_core/pim_core_src/pimSilent.cxx:364,391,413`) against
   `pimCommandMgr::DeleteCommand()`/`CanDeleteCommand()`/`AppendAddCommand()`
   (`pim_core/pim_core_src/pimCommandMgr.cxx:270-451,454-515`). **Site 1,
   the Drop step (`:364`) — verified non-issue**: a `false` return here is
   the loop's own expected, by-design terminal state, already documented
   by its own inline comment. **Site 2, collision-handling (`:391`) —
   confirmed, by an exhaustive counting proof, to always succeed,
   conditionally**: since `b_cmd_names` is captured *before* the new dummy
   exists, the type's member count remaining just before deleting the
   *k*-th of *n* names is always `(n-k)+1` — always `>1` — so every delete
   in this loop is guaranteed to pass `CanDeleteCommand()`, *provided*
   `AppendAddCommand()` (`:389`) itself succeeded in creating the dummy;
   that precondition is also never checked, and traced to depend on
   `FindPsfTempateNode(lictype2)` finding a matching `<PSF_TEMPLATE>` — not
   confirmed either way in this archive (no product/media XML sample
   exists). If that precondition failed, the subsequent re-add loop
   (`:397-404`) would find the original, wrong-type node still present
   under the same name (`FindPsfNode()` matches by name, not `<LICTYPE>`)
   and merely update its license identifiers/description/features in
   place via `AppendEditCommand()` — which never touches `<LICTYPE>` —
   leaving the **wrong** license type permanently attached, silently
   defeating the entire collision-fixup mechanism. **Site 3, final cleanup
   (`:413`) — confirmed, generalizing the already-documented stuck-dummy
   finding beyond dummies specifically**: this is where checking the
   return value would matter most. **Any** command — dummy or genuinely
   original — that is the sole remaining representative of its license
   type by this point gets silently, permanently stuck whenever `A_wants`
   contains no command of that type at all, confirming
   `pimSilentFixupPSF()`'s own stated goal (its file header comment, "we
   only need to eliminate the names from `B` that are not part of
   `A_wants`") is structurally **unachievable** for an entire license type
   in that case — not a corner case. A stuck non-dummy leftover carries
   real, valid (if unrequested) license data, distinct in character from
   the dummy's internally-mismatched data, but can equally reach
   `pimScriptLoop::OnInstall()`'s unconditional `arr[0]` read for
   `[LM_LICENSE_FILE]` — a real, valid, but unrequested license silently
   determining the entitlement's generated install scripts. See
   `ai-context/business_rules.yaml`'s
   `pimsilentfixuppsf_dummy_placeholder_can_get_stuck` and
   `docs/classes/pimSilent.md`.
43. ~~`pimSilentFixupPSF`'s `AppendAddCommand` return-value check at the
   same depth~~ — **done**: a further, dedicated pass on this same
   function's own error-handling mechanism (item 22's `pimSilentFixupPSF()`),
   not a new function, mirroring the `DeleteCommand()` return-value pass
   (item 42) but finding a more severe, more clearly reachable failure
   mode. Traced both `AppendAddCommand()` call sites
   (`pim_core/pim_core_src/pimSilent.cxx:389,403`) against
   `pimCommandMgr::AppendAddCommand()`/`AppendEditCommand()`/`FindPsfTempateNode()`
   (`pim_core/pim_core_src/pimCommandMgr.cxx:310-403,923-929`). It is
   **add-or-update**: an existing `name` is simply updated (always
   succeeds), but creating a genuinely **new** node depends on
   `FindPsfTempateNode()` finding a matching `<PSF_TEMPLATE>` — if none
   exists, no node is ever created and the function returns `false` with
   **zero trace anywhere in `Eb`'s document**. **Site 1 (`:389`), the
   dummy — confirmed lower reachability**: its license type (`lictype2`)
   is read from an **existing, live** `B` command, plausibly (though not
   certainly) already templated. **Site 2 (`:403`), the main re-add loop —
   confirmed, more severe and more clearly reachable**: when `A_wants[i]`
   is a name genuinely new to `B`, its license type comes from **`A`'s own
   document**, entirely independent of `B`'s template set — if `B`'s
   media lacks a matching template (a real, plausible
   cross-version/cross-product mismatch, e.g. `A` is an older or
   customized request XML naming an add-on license feature the
   currently-matched `B` media doesn't include), `A`'s entire request for
   that named command is **dropped without a trace** — not stuck like a
   dummy, not present with wrong data, simply **absent** from `Eb`, as if
   the user never asked for it. **Confirmed by this function's own
   reasoning**: its inline comment introducing this loop ("now we know
   that all the names we want to use are either not already used or match
   the same type...") shows the author reasoned through the
   name/type-collision problem the earlier block handles, but never
   considered whether a `<PSF_TEMPLATE>` exists at all for a genuinely new
   name's license type — a confirmed blind spot in the function's own
   design, not a deliberately accepted risk. **Downstream consequence,
   traced for the first time**: since the failed `AppendAddCommand()`
   never adds anything to `Eb`, the name in question never even appears
   in the final cleanup loop's `B_avail` re-fetch — there is nothing to
   strand, unlike the `DeleteCommand()` findings. The user's requested
   licensed feature/capability is simply never provisioned in the
   generated entitlement at all, with no dummy, no log, no property, and
   no error anywhere in the entire pipeline to indicate the omission — a
   strictly silent drop, arguably harder to diagnose than either of the
   `DeleteCommand()` findings, since there is no leftover artifact for a
   later investigation to even notice. See
   `ai-context/business_rules.yaml`'s
   `pimsilentfixuppsf_dummy_placeholder_can_get_stuck` and
   `docs/classes/pimSilent.md`.
44. ~~`pimSilentCreateEntitlement`'s MSI-node fix at the same depth~~ —
   **done**: a further, dedicated pass verifying one already-proposed
   remediation (item 24's `<MSI>`-node last-value-wins bug), not a new
   function. **FIX VERIFIED (PARTIALLY)**: traced `pimMSIExec()`'s own
   attribute reads (`pim_core/pim_core_src/pimMSILoop.cxx:202-207`) off
   the **identical** `<MSI>` nodelist this function iterates — **confirms
   both `name` and `PRODUCTCODE` genuinely exist on real `<MSI>` nodes**
   in this codebase, grounding the proposed per-node-identity fix in
   fact. **But the fix's own "match by either" phrasing is
   under-specified — the 2 keys have opposite tradeoffs** for the
   cross-version matching this function exists to perform:
   `PRODUCTCODE` is a Windows Installer GUID conventionally expected to
   change on most new MSI builds (a well-established external
   convention, not itself confirmed from this archive, since no
   MSI-authoring documentation exists in it) — a `PRODUCTCODE`-only
   match would likely find **no** correspondence at all between `A`'s
   (older/customized) request XML and `B`'s (newly matched) media in the
   realistic case this function handles, silently degrading the "fix" to
   "no customization ever survives" — trading the confirmed corruption
   bug for a confirmed loss of the feature's entire purpose. `name`, by
   contrast, is a stable, human-authored label far more likely to
   persist across a product's version history, making it the practically
   workable primary key — but is **not** enforced unique by any schema
   in this archive, so 2 `<MSI>` nodes sharing the same name (not
   confirmed to occur, but not ruled out) could still collide under
   name-only matching. A robust implementation would need to try
   `PRODUCTCODE` first (exact package identity, when it happens to
   match) and fall back to `name` (broader, version-tolerant reach) —
   neither the original bug report nor the proposed fix's own phrasing
   specifies this fallback design. **New asymmetry, found while
   verifying the fix, orthogonal to the name-collapsing bug**: even
   before per-node matching, the existing write loop's own
   `format`-attribute handling (`:654-661`) only overwrites `B`'s
   `format` attribute **if `B`'s own node already has one** (no `else`
   branch to create it), while the `<MSIARGUMENT>` child-element handling
   immediately below (`:663-673`) **does** create a missing child. Any
   correctly-scoped per-node fix must consciously preserve or correct
   this existing create-vs-update-only asymmetry between the 2 fields it
   copies, not just the node-identity matching — a design detail the
   original bug report and its proposed fix did not address. See
   `docs/classes/pimSilent.md`'s Risk Analysis.
45. ~~`pimSilentCreateEntitlement`'s PROPERTY skip list fix at the same
   depth~~ — **done**: a further, dedicated pass verifying one
   already-proposed, low-priority hardening fix (item 25's `<PROPERTY>`
   copy loop), not a new function. **FIX VERIFIED (COSMETIC)**: the
   proposed fix ("initialize the loop's `name` variable fresh each
   iteration") is confirmed to change nothing observable today — traced
   precisely which guard does the real work: the inner for-loop's own
   `attribName != NULL` condition (`:712`) is the **sole**, always-present
   safety net for a nameless `<PROPERTY>` node, regardless of what the
   outer skip-list `continue` (`:710-711`) does with a stale `name`.
   Applying the fix (`name.Clear()`, or re-declaring `name` inside the
   loop — confirmed equivalent) changes nothing observable:
   `PropertyNamesToKeepUnchanged.Find("")` finds no match so the
   `continue` no longer fires, but the inner loop's own guard still
   independently produces 0 copies — the same net result. The fix's real
   value is closing a **latent maintenance trap**, not fixing a live bug:
   a plausible future refactor "simplifying" the inner loop's condition to
   just `i < map->getLength()` (reasoning, incorrectly, that the outer
   `continue` already handles the no-name case) would silently
   reintroduce exactly the kind of stale-value corruption already
   confirmed live in `pimSilentFixupShortcuts()`'s Program Menu bug, since
   the outer `continue`'s protection was never real to begin with.
   **CONFIRMED BUG, found in the same pass, by tracing this loop's full
   return-value chain — a new finding, distinct from the `name`-staleness
   issue**: `pimXmlFile::GetProperty()`'s own return value is *also*
   discarded at both value-copy sites (`:719`, `:725`); if `A` ever has 2+
   `<PROPERTY>` nodes sharing the same `name` (not schema-prevented in
   this archive), `GetPropertyNode()`'s document-order-first-match search
   could return a *different* node than the one currently being iterated,
   and a missing attribute there would leave `value` stale — silently
   written to `Eb` regardless. Not confirmed reachable against a real
   product XML (none exists in this archive), but the mechanism is fully
   confirmed from source, and mirrors — in a previously unexamined
   location — the exact "discarded `Get()` return + reused stale
   variable" pattern already confirmed live in
   `pimSilentFixupShortcuts()`'s Program Menu bug. **A more robust
   alternative, already available in the code's own hands**: the
   general-attribute branch (`:722-727`) already has the exact DOM
   attribute node in hand (`attrib`, from `map->item(i)`, `:714`) —
   `attrib->getNodeValue()` would read it directly, with no document-wide
   re-search, no dependency on `name` uniqueness, and no staleness risk
   at all, making the current indirect `GetProperty()`/`SetProperty()`
   roundtrip unnecessary for this branch specifically. See
   `docs/classes/pimSilent.md`'s Risk Analysis.
46. ~~`pimSilentFixupShortcuts`'s array index fix at the same
   depth~~ — **done**: a further, dedicated pass verifying one
   already-documented, caller-side-cited fix (item 25's wrong-array-index
   bug, originally documented from `pimShortcutMgr`'s side). **FIX
   VERIFIED (SUFFICIENT)**: replacing `B_avail[i]` with `A_wants[i]` in the
   4 `SetShortcut*State()` calls (`:457-460`) eliminates the wrong-shortcut
   risk with no residual gap — the enclosing
   `if (B_avail.Find(A_wants[i]) != -1)` guard already proves `A_wants[i]`
   exists in `Eb`'s document, so `FindShortcutNode(A_wants[i])` (called
   inside `SetShortcutState()`) is guaranteed to locate the same shortcut
   the 2 immediately-following field copies already correctly target, and
   `B_avail`'s snapshot stays valid since nothing in this function mutates
   `Eb`'s document structure. **NEW LIMITATION, generalized from item 40's
   stale-Program-Menu-copy fix finding**, which traced `SetShortcutState()`
   only as the primitive behind `SetShortcutProgramsMenuState()`: all 4 of
   `SetShortcutStartMenuState()`/`SetShortcutProgramsMenuState()`/
   `SetShortcutDesktopState()`/`SetShortcutQuicklaunchState()` delegate
   identically to this one shared primitive, and since all 4 corresponding
   child elements are independently optional per shortcut, it can
   legitimately return `false` for a **correctly identified** shortcut
   whenever that one location option was never offered by `Eb`'s
   template — none of the 4 array-index call sites check this return
   value, fix or no fix. **Verified non-issue, found while tracing this
   same call chain**: `GetShortcutInfoByID()`'s own discarded return value
   and its once-outside-the-loop output booleans (`sm`/`pm`/`dt`/`ql`) are
   not reachable as a staleness bug — the id is sourced from the same
   document's own `GetAllShortcutIDs()` scan, guaranteeing the node is
   always found, and the function pre-initializes all 4 output booleans to
   `false` before scanning children, unlike its `GetShortcutProgramMenu()`
   sibling. **Adjacent, pre-existing risk, found in the same trace**:
   `GetAllShortcutIDs()` (`pim_ui/pim_ui_src/pimShortcutMgr.cxx:312-313`)
   dereferences its `id`-attribute node with no null check — a latent
   crash if any `<SHORTCUT>` node ever lacks an `id` attribute, underlying
   both `A_wants` and `B_avail` alike. See `docs/classes/pimSilent.md`'s
   Risk Analysis.
47. ~~`pimSilentCreateEntitlement`'s session-index lookup fix at the same
   depth~~ — **done**: a further, dedicated pass verifying item 38's 2
   proposed fixes. **FIX VERIFIED (SUFFICIENT, either one alone)**: (a) the
   caller also checking this function's own `bool` return value, or (b)
   the function resetting `entitlement_index` to `-1` on its own failure
   path — confirmed there is exactly 1 such path (`:524-531`), not
   several, since no other early return exists between `:520` (where the
   index is set) and the final `return true;` at `:743`. **New downstream
   consequence, traced one step further than "the caller proceeds as if
   creation fully succeeded"**: followed the caller's loop forward from the
   undetected-failure point — execution falls through the full per-entry
   post-creation block (`pimTop.cxx:1542-1751`) and, once the whole
   `AskedToInstallThisList` loop completes without any other entry
   aborting, reaches `pimTop.cxx:1777`'s `SessionInfo.Save()`, which
   **persists** the corrupted session document — including the orphaned
   `<ENTITLEMENT>` node `AddEntitlement()` appended for the half-created
   `B` — to `sessioninfo.xml` on disk. Either fix prevents this, since both
   make the caller's existing early-abort fire before that line is ever
   reached, discarding the corrupted in-memory state when the caller's
   stack-local `SessionInfo` object is destroyed on return. **Confirmed bug
   in the 2nd fix's own literal wording**: "call `DropEntitlement(B)`" is
   confirmed non-functional as written — `DropEntitlement(cStringT)`
   matches by product tag (`GetID()`), and `B` is a filesystem path, the
   identical mismatch already confirmed dead code in `AddEntitlement()`'s
   own guard; a working rollback needs `DropEntitlement(entitlement_index)`
   (the sibling `int` overload) instead. **Verified non-issue**: given the
   traced consequence, a full rollback isn't actually necessary for
   correctness today, since either detection fix alone already prevents
   ever reaching `Save()`. See `docs/classes/pimSilent.md`'s Risk Analysis.
48. ~~`pimSilentFixupPSF`'s `Ea` const-reference hardening at the same
   depth~~ — **done**: a further, dedicated pass verifying item 37's minor
   suggestion (`Ea` could safely be declared `const pimEntitlement&`).
   **CONFIRMED NOT PURELY MECHANICAL**: this function's only direct call
   on `Ea`, `Ea.GetXMLPtr()` (`:357`), is itself not declared `const`
   (`pimEntitlement.h:320`), so `const pimEntitlement& Ea` as literally
   proposed **fails to compile**. **Confirmed safe prerequisite**:
   `const`-qualifying `GetXMLPtr()` itself (same non-`const`
   `pimXmlFile*` return type — ordinary "shallow const," since the method's
   body is the single trivial `return xmlPtr;`) resolves this with zero
   behavior change anywhere, since `pimCommandMgr`'s constructor already
   accepts a plain non-`const` `pimXmlFile*` regardless; confirmed zero
   existing `const pimEntitlement` usage anywhere in this codebase, so this
   would be a pure widening, never a narrowing of any existing call site.
   **CONFIRMED SCOPE CEILING**: even with both changes applied, the
   `const` only blocks a non-`const` `pimEntitlement`-level call directly
   on `Ea` — reading `pimCommandMgr.h` in full confirms **none** of its
   public methods, including the exact 3 read-only accessors this function
   calls on `A_cmds`, are declared `const` (and confirmed zero `const
   pimXmlFile` usage in any header either), so `A_cmds` itself could still
   mutate `Ea`'s document with no compiler objection, before or after the
   fix. The hardening is confirmed to be a **documentation-only signal** at
   this function's own top-level signature — accurately reflecting today's
   already-verified read-only usage, but structurally incapable of
   catching the realistic mistake (`A_cmds` accidentally mutating `Ea`'s
   document), since that mistake lives one layer below what a `const Ea`
   parameter can ever reach, and this codebase has no const-correct API
   anywhere in that layer to build on. See `docs/classes/pimSilent.md`'s
   Risk Analysis.
49. ~~`pimSilentCanMatchNeeds`'s platform and language checks fix at the
   same depth~~ — **done**: a further, dedicated pass verifying item 36's
   2 proposed fixes. **FIX (a) VERIFIED SUFFICIENT ONLY FOR
   DIAGNOSABILITY**: checking `SetLanguageInstallState()`/
   `InitPlatformState()`'s return values and logging the failure makes
   the outcome visible but changes nothing about it — traced the
   zero-platform case one level further into
   `pimPackageMgr::RefreshAFeatureNode()`, confirming every
   `<CDSECTION>`/`<MSI>` node carrying a `platform` attribute gets set
   `install="N"` whenever no platform is ever selected — a near-total,
   silent install failure logging alone can only report after the fact,
   never prevent. **CORRECTION, found in the same pass**:
   `InitPlatformState()`'s "2-level fallback" is more precisely a
   **mutually exclusive, conditionally-chosen single fallback** —
   `x86e_win64`/`arm64_win64` detected → try `i486_nt` only; anything
   else → try `x86e_win64` only — not both names tried in sequence.
   **FIX (b) CONFIRMED ARCHITECTURALLY FEASIBLE for both language and
   platform, resolving the original "if feasible" hedge**:
   `pimShouldWeInitLanguageID()`/`pimGetAllPacksMode()` and
   `btkGetPlatform()` are all global, parameterless accessors already
   read this same way elsewhere in this exact file — no CLI-context
   plumbing needed. **New nuance**: a validating copy must exactly
   replicate the corrected single-fallback branch, or it produces
   false-negative rejections for A/B pairs that succeed today via the
   fallback. **New maintenance-hazard**: implementing it duplicates the
   9-hardcoded-language rule and the platform fallback into a 2nd,
   independent copy of logic `pimTop.cxx` already owns, with nothing
   keeping the 2 in sync. **New, unresolved policy question**: extending
   validation to auto-detected platform would let a machine merely
   lacking that platform have its otherwise-valid single-product install
   rejected outright, rather than proceeding with that piece silently
   missing, as today — this codebase never states which behavior is
   intended. See `docs/classes/pimSilent.md`'s Risk Analysis.
50. ~~`pimSilentFixupPSF`'s dummy-placeholder fix at the same depth~~ —
   **done**: a further, dedicated pass verifying item 28's 2-option
   proposed fix. **Option 1 ("guarantee no dummy survives") confirmed
   already true in half the cases, structurally unachievable in the
   other half**: when another `A`-wanted command independently shares
   the dummy's license type, its own `AppendAddCommand()` call already
   restores the type's live count to ≥2 before the final cleanup loop
   runs — the dummy is **already** correctly deleted today, no fix
   needed. When no such entry exists, `CanDeleteCommand()`'s floor has
   **no override anywhere in `pimCommandMgr`** — confirmed structurally
   impossible to reduce any license type to 0 live commands via any
   operation ordering. Closing this remaining gap needs a genuinely
   **new** `pimCommandMgr` capability: an in-place "retype" operation
   reusing `AppendAddCommand()`'s own template-cloning machinery
   (`FindPsfTempateNode()` + deep-cloning) against the existing node,
   never passing through a 0-count instant. **Option 2 ("select by
   type/role, not array position 0") confirmed architecturally
   precedented, but concretely under-specified**: `pimScriptLoop::OnInstall()`
   itself, a few lines below the `arr[0]` lookup, already performs
   exactly this kind of role-based selection for a *different* command —
   `FindNodelistAttribMatch(pimPSF, pimid, "parametric")`
   (`pimScriptLoop.cxx:135`) — but no canonical selector value for "the
   license-bearing command" is confirmed to exist anywhere in this
   archive. **Further confirmed**: `AppendEditCommand()` never creates a
   missing `<FEATURE_NAME>` child, only updates an existing one,
   deepening the already-flagged uncertainty over whether the dummy can
   even reach `pimScriptLoop`'s `arr` at all. **New, dummy-independent
   design smell**: `arr[0]`'s "just take the first command" assumption
   is fragile on its own terms, since this same function already relies
   on multiple, role-distinct PSF commands coexisting. See
   `docs/classes/pimSilent.md`'s Risk Analysis.
51. ~~`pimSilentCreateEntitlement`'s package selection at the same depth~~ —
   **done**: a fresh, dedicated pass on a previously-uncovered part of an
   already-documented function (item 26), directly analogous to item 41's
   quality-agent bug. **CONFIRMED BUG**: `SetPackageInstallState(name, false)`
   (`pim_core/pim_core_src/pimPackageMgr.cxx:304-332`) silently **refuses**
   — leaving the package's `install` attribute unchanged — whenever that
   package is `required="Y"` **or** already `installed="Y"` (except
   `"prime_converter"`), and this function's loop discards the return
   value at both call sites (`:582,584`). **Confirmed by direct contrast
   with the GUI path, mirroring the quality-agent precedent exactly**:
   `uiCustomTree::RefreshPkg()` (`pim_ui/pim_ui_src/pimCustomDlg.cxx:1819-1904`)
   checks the identical `req || installed` condition and proactively
   disables the checkbox — no equivalent precondition exists in the
   silent-install path. **New nuance**: even on a refused disable, 2
   hardcoded package names (`"prime_converter"`/`"creo_simulate"`) still
   have their global mode flags toggled to the attempted value regardless,
   diverging from the unchanged XML attribute. **Verified non-issue,
   confirmed wasteful but not incorrect**: this function's own
   `B_pkg.Refresh()` call (`:586`) computes `<CDSECTION>`/`<MSI>`/`<SFX>`
   install eligibility using platform/language values `pimTop.cxx`'s
   post-creation code immediately overwrites and recomputes via its own,
   separate `Refresh()` call afterward — entirely superseded before
   anything reads it, harmless. **Cross-referenced finding**:
   `GetAllPackageNames()` (`:577`) shares `GetInstallPackageNames()`'s
   already-confirmed unguarded null-attribute crash risk. See
   `ai-context/business_rules.yaml`'s
   `pimsilentcreateentitlement_package_disable_silently_overridden` and
   `docs/classes/pimSilent.md`.
52. ~~`pim_ui`'s remaining gaps~~ — **done**: a dedicated pass on 2
   previously-flagged not-traced-to-depth items, `pimSessionInfo::TryAuthorize()`
   (`pim_core/pim_core_src/pimSessionInfo.cxx:1911-1984`, cited only via its
   call-site signature; its sibling caller, `pimGetAvailable::GetSecurity()`,
   was already fully traced, see item 13 above) and `pimInstallMgrDlg`'s own
   ~550-line `OnPushButtonActivate()` (`pim_ui_src/pimInstallMgrActions.cxx:643-1193`,
   previously flagged as not traced to the same depth as the rest of the
   class). **CONFIRMED BUG, HIGH SEVERITY (`TryAuthorize()`)**: the
   `do`/`while` credential-retry loop has no handling at all for
   `pimAuthorizeToPTC()` returning `false` (a transport/network failure) —
   the loop spins forever re-prompting for credentials it never
   re-validates against the server, confirmed by direct contrast with
   `pimGetAvailable::GetSecurity()`'s correct `-3` abort on the identical
   failure. Also confirmed: a matching `pimXmlFile` leak on the server-rejects
   path; the same discarded-return-value/stale-reused-`str` pattern found
   repeatedly elsewhere in this codebase
   (`pimAuthorizeFailedMsgDirectCall(out, str)`); and credential-store
   aliasing between the media URL and the PTC.com production URL, via
   `SetSecurity()`/`GetSecurity()`/`DelSecurity()`'s own `// HACK` comment
   mapping both to the same `SecurityArray` slot. **CONFIRMED BUG, HIGH
   SEVERITY (`OnPushButtonActivate()`)**: the EULA screen's arm calls
   `EulaNextPreAction()` (kicks off license acquisition) **before**
   checking whether the user declined the license — the reverse of the
   Beta EULA arm's correct decline-checked-first order in the same
   function. Also confirmed, in the same pass: a control-flow-traced
   `Refresh()`-after-`Destroy()` gap (no `return` after
   `OnFinishFromEula()`/`OnFinishFromBetaEula()`, severity hedged on
   `uiDialog`'s own post-`Destroy()` semantics, outside this archive); a
   disabled-Finish-button bug on exit-confirmation "No"; and a confirmed
   re-entrancy bug via `pimTextDlg`'s "proceed anyway" flow re-entering this
   same method with `skip_ports_modification` never reset. Plus a handful
   of lower-severity findings (chain 2's missing final `else`, a
   `last_license_btn` set-order asymmetry, asymmetric undo of `mCmdEdt`, a
   `currentProduct` null-deref risk, `MultipleHostIDButton`'s array-size
   mismatch, the busy cursor never cleared on some paths, dual
   reconfigure-mode-read sources, and `ResetPort`/`EportReset` covering
   only 2 of the 3 ports). See `ai-context/business_rules.yaml`'s new
   `pimsessioninfo_tryauthorize_infinite_loop_on_transport_failure` and
   `piminstallmgrdlg_eula_decline_order_bug` rules, and
   `docs/classes/pimSessionInfo.md`'s and `docs/classes/pimInstallMgrDlg.md`'s
   Risk Analysis sections.
53. ~~`pimTranslateMgr.cxx`~~ — **done**: first full-depth pass on this
   class (`pim_core/includes/pimTranslateMgr.h`, 32 lines +
   `pim_core/pim_core_src/pimTranslateMgr.cxx`, 129 lines, both read in
   full — previously cited only via call-site signatures). Confirmed the
   translation-file matching algorithm: each `<TRANSLATION file? element
   attribute attributeMatch modify?>` entry resolves its target node in the
   product document via `pimXmlFile::FindNodelistAttribMatch()` (first
   document-order match) and overwrites either that node's text content or
   a named attribute with the entry's `<TEXT>` value. **CONFIRMED BUG**:
   `TranslateFrom(pimXmlFile*)` guards computing its `filename` member on
   `ptr->getFilePath()` (the TRANSLATION document) but assigns the value
   from `xmlProduct->getFilePath()` (the PRODUCT document — a **different**
   object) — checking one object's path presence, then unconditionally
   dereferencing the other, unguarded. **CONFIRMED REACHABLE** via 2 of
   `pimEntitlementTree::OnOptionMenuSelect()`'s 3 near-identical call
   sequences (the "sibling"/"parent" media-ID-bump arms): their
   `eptr_new_xml` comes straight from `pimGetMediaDetails::GetProductXml()`
   (confirmed to never call `SetXml()` internally) and, on a
   `DownloadXmlBackups` cache hit inside `pimEntitlement::Init()`, never
   gets a file path set at all before reaching this method — every
   `file`-scoped `<TRANSLATION>` entry is then silently skipped for those 2
   arms, while the function's 3rd, correctly-wired arm is unaffected — a
   within-function asymmetry. **CONFIRMED, dead code containing a
   compile-breaking typo**: a separate constructor call site,
   `pim_core/pim_core_src/pimEntitlement.cxx:995`, reads
   `Eptr->GetXMLPtr()` — `Eptr` (lowercase `p`) is declared nowhere in that
   function, file, or the entire `pim_core` module (only `EPtr`, capital
   `P`, exists) — confirmed permanently dead since its guarding
   `#ifdef PIM_TRANSLATE_XML` can never be true in this translation unit
   (the macro is `#define`d in exactly one place in this whole archive,
   `pim/pim_src/pimTop.cxx:185`, a different unity-build target). Also
   corrected: `OnOptionMenuSelect()`'s already-documented `eptr_new_xml`
   unconditional-dereference null-crash bug actually occurs 3 times in this
   function, not once as previously documented. See
   `docs/classes/pimTranslateMgr.md` (new), `ai-context/business_rules.yaml`'s
   new `pimtranslatemgr_translatefrom_mismatched_guard_wrong_object_path`
   rule, and the updated `docs/classes/pimEntitlementTree.md`.
54. ~~Direct confirmation of `pimEntitlement::OnReconfigure()`'s body and the untraced
   middle section of `OnRollback()`~~ — **done**: both fully traced
   (`OnReconfigure()`: `pim_core/pim_core_src/pimEntitlement.cxx:6866-7222`,
   357 lines; `OnRollback(bool from_during_install)`: `:7287-7565`, 279
   lines). **CONFIRMED BUG, HIGH SEVERITY**: `OnReconfigure()`'s very first
   block computes `RollbackMe`'s path (the original as-shipped `.p.xml`
   under `[LP]/bin/pim/xml/`) by taking a local copy of `xmlPtr`'s own
   CURRENT cache-file path (`pimGetAppData/pim/<name>.xml`, per the code's
   own comment) and, purely as a side effect of extracting its bare
   filename via `GetTail()`, unconditionally deletes that cache file from
   disk first (`if (src.IsFile()) src.Erase();`) — there is no functional
   need to delete the file just to read its own filename. `xmlPtr`'s own
   file-path member is unaffected (a separate local copy is erased), so the
   deleted file is only restored once `xmlPtr->DoSave()` next runs. 2
   confirmed early-return paths leave it permanently deleted with nothing
   to replace it: `PerformServiceAction(pimServices::StopAction)` failing
   (no `DoSave()` before the return), and
   `OnInstallCustomActions("PreReconfigure")` failing (likewise no
   `DoSave()` before the return — reachable via an ordinary custom-action
   failure, no cancellation needed). **Confirmed by direct contrast**:
   `OnRollback()`'s own mirror-image "relocate the cache file" block has no
   such `Erase()` side effect — the 2 sibling relocation blocks are
   asymmetric. Also confirmed, resolving `docs/04_installation_flow.md`'s
   own open questions: `OnRollback()`'s untraced middle section mirrors
   `OnInstall()`'s stage list in reverse exactly as speculated
   (UninstallShortcuts → PSF rollback → Scripts rollback → Services
   uninstall → RegEdit uninstall → Copier rollback/uninstall → PTC-record/
   uninstall-key removal), exactly 2 `from_during_install`-specific branches
   exist (Copier's `Rollback()` vs. `Uninstall()` choice, and a final
   `creobase.xml`-only empty-directory cleanup), and the function has ZERO
   early-return points — a single, uninterrupted sequential run to
   completion, consistent with its own `OnUninstallCustomActions("PreUninstall")`
   failure being logged but not fatal (the opposite of `OnReconfigure()`'s
   fail-fast custom-actions handling). See `docs/classes/pimEntitlement.md`
   (new blockquote + Risk Analysis), `ai-context/business_rules.yaml`'s new
   `pimentitlement_onreconfigure_erases_cache_file_as_path_side_effect`
   rule, and the updated `docs/04_installation_flow.md`.
55. ~~Whatever real build system, sample product XML, and launcher EXE source exist in
   the actual PTC repository this archive was exported from~~ — **done, in the only
   sense this item admits**: independently RE-VERIFIED, via exhaustive search rather
   than repeating the Phase-1 assertion, that none of these are present anywhere in
   `installmgr.zip`. `find . -iname "Makefile*" -o -iname "*.mk" -o -iname
   "CMakeLists*" -o -iname "*.vcxproj*" -o -iname "*.sln" -o -iname "*.cmake" -o -iname
   "SConstruct" -o -iname "*.gyp" -o -iname "build.xml" -o -iname "*.bat" -o -iname
   "*.sh"` across the entire extracted tree: **zero matches**. `find . -iname "*.xml"`:
   **zero matches**. The archive's top level is confirmed to be exactly the 4 module
   folders (`pim`, `pim_core`, `pim_ui`, `pim_util`) described since Phase 1 — no other
   files or directories exist at all. No phase of this analysis could substitute for
   having these, and none does. 2 cross-references from later phases DO strengthen
   (without closing) this gap: the `PIM_TRANSLATE_XML` macro-scoping proof found while
   documenting `pimTranslateMgr` (item 53) is the first mechanism-level confirmation
   that `pim` and `pim_core` truly compile as separate translation units, not merely a
   naming-convention inference; and `pim_core/messages/usascii/pim.msg.LOCAL` (already
   noted in `docs/01_repository_inventory.md` §2) was confirmed, via direct `diff`
   against the main `pim.msg`, to be a stale, unreferenced older snapshot — missing 2
   message IDs the main file has — not a build artifact. See the updated
   `docs/01_repository_inventory.md` §3/§8 and `ai-context/architecture.yaml`. Also
   fixed in this pass: 7 stale "not yet generated" cross-reference placeholders in
   `docs/01_repository_inventory.md`, `docs/02_architecture_overview.md`, and
   `docs/03_application_startup.md`, all pointing at docs (`docs/classes/*.md`,
   `docs/03_application_startup.md`, `docs/04_installation_flow.md`,
   `docs/08_xml_configuration.md`, `docs/09_logging_framework.md`) that have existed
   since their own respective phases completed.
