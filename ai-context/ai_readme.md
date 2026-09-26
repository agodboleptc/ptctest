# AI Development Context — PIM

> Read this before making any code change to the PIM codebase. It is the entry
> point into the full documentation set (`docs/`, `generated/doxygen/`,
> `ai-context/*.yaml`) produced across Phases 1–18 of this analysis, built entirely
> from source evidence in the `installmgr.zip` archive (see
> `docs/01_repository_inventory.md` §0 for exactly what that archive does and does
> not contain). Nothing here is invented; where evidence was incomplete, it says so.

## System Purpose

PIM ("Parametric Installation Management") is PTC's native Windows C++ installer
application for Creo, Mathcad, Windchill Workgroup Manager, and related product
lines. It is a full in-house install engine, not a thin MSI wrapper: it parses
product-definition XML, resolves cross-product prerequisites, and executes a
pipeline of typed install steps (file/archive copy, MSI, self-extracting EXE,
registry edits, shortcuts, scripts, Windows services, and a "PSF" step) per product,
under a custom non-MFC UI.

**Start here for architecture**: `docs/02_architecture_overview.md`.
**Start here for the object model**: `docs/classes/pimEntitlement.md` +
`docs/classes/pimLoop.md`.
**Start here for machine-readable structure**: `ai-context/architecture.yaml`,
`ai-context/knowledge_graph.yaml`.

## Critical Modules (in order of how much a change there can break)

1. **`pim_core`'s `pimMSILoop`** (`pim_core/pim_core_src/pimMSILoop.cxx`) —
   **CONFIRMED LIVE, COMPOUNDING REINSTALL BUG, the single highest-severity
   finding in this entire documentation effort**: in the MSI-API execution
   strategy (`OnInstall()`, active when `use_msiexec` is `false`), a
   `fallback_to_msiexec` flag is checked at the END OF EVERY remaining loop
   iteration (not once after the loop) and is never reset once set. A single
   `<MSI>` node whose install command can't be pattern-matched causes **every
   remaining node's iteration** to re-invoke `pimMSIExec()`, which re-scans and
   can silently **re-install already-installed packages** (its eligibility
   check does not look at the "already installed" attribute) — an O(N²)
   blowup in redundant `msiexec.exe` launches in the worst case. Reachable on
   real legacy product XML, not merely hypothetical. See
   `ai-context/business_rules.yaml`'s `pimmsiloop_fallback_causes_repeated_reinstall`
   and `docs/classes/pimMSILoop.md`. Fix candidate: reset the flag after
   handling it, or restructure the fallback to avoid re-scanning the whole list.
2. **`pim_core`'s `pimRegEditLoop`** (`pim_core/pim_core_src/pimRegEditLoop.cxx`)
   — **CONFIRMED LIVE DATA-COMPLETENESS BUG**: `OnRollback()`'s node-list loop
   double-increments its index (the enclosing `for` statement's own `index++`
   AND an explicit `item(index++)` call both fire per iteration), so it silently
   skips every other `<REGISTRY>` entry on **every** pass, forever — confirmed
   by direct contrast with the correctly single-incrementing equivalent loop in
   `OnInstall()` in the same file. Uninstalling or rolling back a product can
   leave roughly half its registry entries behind, with no error surfaced. See
   `ai-context/business_rules.yaml`'s
   `pimregeditloop_rollback_skips_half_of_entries` and
   `docs/classes/pimRegEditLoop.md`. Fix candidate: remove the redundant
   increment and re-verify against `Remove()`'s bottom-up subkey-deletion
   ordering constraint.
3. **`pim_core`'s `pimServices`** (`pim_core/pim_core_src/pimServices.cxx`) —
   **CONFIRMED LIVE CONCURRENCY BUG**: its `Create_low()` releases its own
   serialization lock immediately after starting the operation's thread, instead
   of holding it for the full operation like its 6 sibling singleton wrapper
   classes. A second concurrent `Create()`/`Uninstall()` call would delete a
   still-running `pimServiceLoop` out from under its own thread — a use-after-free
   / destroy-while-running race. Currently latent (no traced caller invokes it
   concurrently) but reachable, unguarded code. See
   `ai-context/business_rules.yaml`'s `pimservices_lock_does_not_serialize` and
   `docs/classes/pimServices.md`. Fix before adding ANY concurrent caller of this
   class.
4. **`pim_ui`'s `pimInstallMgrDlg`** (`pim_ui/pim_ui_src/pimInstallMgrDlg.cxx` +
   `pimInstallMgrActions.cxx` + header, 5,575+ lines) — the single main-window
   wizard dialog driving the entire interactive install experience, and the
   **largest set of confirmed findings in a single class in this entire
   documentation effort**: a `StepForward()`/`StepBack()` wizard-step
   guard-condition asymmetry that can land the wizard on a tab it never
   actually visited going forward; `UpdateParentEntitlements()`/
   `UpdateChildEntitlements()` duplicated verbatim (modulo one stray comment)
   between this class and its own nested `uiCustomAppTree` class, so a fix
   applied to only one copy silently desyncs the two; a permanently no-op
   `RefreshFeatureTab()`; an unreachable `OnTabSelect()` Help/Advanced branch;
   a message-ID bug in `SetPortError()`; and a 3-way duplicated exit/cleanup
   path across `OnClose()`/`OnFinishFromEula()`/`OnFinishFromBetaEula()`. Also
   flagged `pimCustomDlg` as a parallel class with a near-identical
   method-name set — **corrected in a later pass (see Critical Modules #11)
   from an initial "likely superseded" guess to confirmed live code**. A
   LATER PASS read this class's previously-uncatalogued 3rd `.cxx` file,
   `pim_ui_src/pimEntitlementRefresh.cxx` (1,514 lines), in full and found 4
   more confirmed issues, the most severe in this class: the School- and
   Beta-mode trial-license wait loops in `EntitlementRefresh()` use
   `while (test == NULL || ++max >= 40)` where the correct sibling
   Trial-mode loop uses `&&`/`<=` — because `||` short-circuits, the retry
   counter never advances while the license fails to parse, so **the loop
   never terminates**, hanging the wizard indefinitely with no timeout and
   no error dialog. Also confirmed: a missing `else` in `IsSufficientSpace()`
   silently discards 2 special-case size calculations; the nested
   `uiCheckButtonCell` class's entire implementation is dead code (its only
   would-be caller lives inside a class-wide `#if 0`-disabled
   `uiApplicationsList` class); and the `PIM_HIDE_CUSTOMIZE_SCREEN` env var
   is checked with directly contradictory effect across 4 sites in the same
   file. See `ai-context/business_rules.yaml`'s
   `piminstallmgrdlg_stepback_stepforward_asymmetry`,
   `piminstallmgrdlg_updateparent_child_duplication`,
   `piminstallmgrdlg_refreshfeaturetab_and_helptab_dead`,
   `piminstallmgrdlg_setporterror_message_id_bug`,
   `pimentitlementrefresh_school_beta_wait_loop_never_terminates`,
   `pimentitlementrefresh_issufficientspace_missing_else`,
   `pimentitlementrefresh_uicheckbuttoncell_dead_code`,
   `pimentitlementrefresh_hide_customize_screen_contradictory`,
   `piminstallmgrdlg_eula_decline_order_bug` (a further, dedicated pass fully
   traced the ~550-line `OnPushButtonActivate()`, previously flagged as not
   traced to the same depth — see below), and `docs/classes/pimInstallMgrDlg.md`.
5. **`pim_core`'s `pimEntitlement`** (`pim_core/pim_core_src/pimEntitlement.cxx`,
   7,649 lines) — the entire install/update/reconfigure/uninstall/rollback
   orchestrator for every product. Highest blast-radius file in the codebase. See
   `ai-context/change_impact.yaml`'s `pimEntitlement` entry before touching it.
6. **`pim_core`'s `pimXmlFile`** — the DOM wrapper every other class depends on for
   XML access, with a **confirmed real locking-bug history** ("Fixed xml mutex
   issues," 2026-06-26; "Added temporary logs to investigate locks issue,"
   2026-04-22, both from this file's own revision log). Treat any change to its
   `PreRead`/`PreWrite`/`PushReadToWrite`/`PopWriteToRead` protocol as high-risk.
7. **`pim_core`'s `pimLoop`** — the threading/cancel/error contract shared by all 10
   install-step types (the original 9, plus `pimGetAvailable`, a 10th confirmed
   subclass discovered late in this effort — see Critical Modules #16). A
   change here ripples through every step type at once.
8. **All 7 install-step "owner wrapper" classes** (`pimCopier`, `pimMSICopier`,
   `pimSFXCopier`, `pimShortcuts`, `pimRegEdit`, `pimServices`, `pimDownloader`) —
   discovered (Phase 7 for `pimRegEdit`; confirmed to generalize to all 7 in a
   later pass) to be **process-wide singletons**, not per-entitlement objects as
   this documentation set originally assumed. Each is *intended* to serialize its
   operation type (one copy, one MSI install, one SFX install, one shortcut op,
   one registry op, one service op, one download) across every
   concurrently-installing entitlement in the process — confirmed actually true
   for 6 of the 7; `pimServices` does not (see item 3 above). Additionally,
   `pimMSICopier`/`pimSFXCopier` share ONE gate, not two independent ones — an
   MSI install and an SFX install cannot run concurrently even though they are
   different classes. This was a multi-stage correction to earlier assumptions in
   this same documentation set — a reminder that structural assumptions from
   naming/pattern similarity should be verified per-class against actual source,
   not inferred from one example and assumed to generalize uniformly.
9. **`pim`'s `pimTop.cxx`** (3,284 lines) — the 3 top-level Run entry points, with
   149 revision-history entries spanning 2011–2026. High change-coupling risk
   between the install/trial/renew flows since they share this one file. Also
   the confirmed construction site for `pimInstallMgrDlg` (3 separate
   stack-local `pimInstallMgrDlg Dlg;` instances, one per Run entry point).
10. **`pim_ui`'s `pimShortcutMgr`** (`pim_ui/pim_ui_src/pimShortcutMgr.cxx`) — the
   first `pim_ui`-module class documented at full depth in this effort, and
   used directly from `pim_core` too (`pimEntitlement.cxx`, `pimSilent.cxx`),
   not just from UI dialogs. **CONFIRMED**: `AreChangesPending()` can only
   detect a shortcut that changed while present in both compared XML
   snapshots — never one added or removed entirely — with a real consequence
   in `pimEntitlement.cxx`'s update/reconfigure decision logic. Tracing this
   class's usage also surfaced a **confirmed caller-side array-indexing bug**
   in `pimSilent.cxx`'s `pimSilentFixupShortcuts()` (uses the wrong array's
   index for 4 of 8 shortcut-state-copy calls). See
   `ai-context/business_rules.yaml`'s
   `pimshortcutmgr_areschangespending_misses_add_remove` and
   `pimsilentfixupshortcuts_array_index_bug`, and `docs/classes/pimShortcutMgr.md`.
11. **`pim_ui`'s `pimCustomDlg`** (`pim_ui/pim_ui_src/pimCustomDlg.cxx`) — a
   standalone "Customize Application Settings" popup dialog, **CORRECTED in
   this pass from an earlier "likely superseded, not confirmed dead"
   speculation to CONFIRMED LIVE**: 2 confirmed call sites
   (`pimInstallMgrDlg::OnPushButtonActivate()`'s `CustomizeBtn` branch, and
   `pimEntitlementTree::OnPushButtonActivate()`'s per-row customize-icon
   handler), and `pim_ui_src.cxx` directly `#include`s `pimCustomDlg.cxx`
   into the build. **CONFIRMED BUG**: `SetSABPath()`/`DisableSAB()`/
   `EnableSAB()` (in the shared `pim_ui_src/pimSAB.cxx`) all guard on
   `main_dlg_sab_handle` — a `static` owned by `pimInstallMgrDlg` — instead
   of this class's own `help_dlg_sab_handle`, currently masked only because
   `pimInstallMgrDlg::InitSAB()` has always already run by the time either
   confirmed caller reaches `Display()`. Also confirmed: the
   `pimEntitlementTree` customize icon resolves but never forwards the
   clicked row's specific product (a UX gap, not a data bug), and this
   class's entire customize feature set is a near-complete structural
   duplicate of `pimInstallMgrDlg`'s own embedded Customize tab-set, down to
   3 renamed nested table classes. See `ai-context/business_rules.yaml`'s
   `pimcustomdlg_is_not_dead_code`, `pimcustomdlg_sab_methods_guard_wrong_static_handle`,
   `pimentitlementtree_customize_icon_ignores_clicked_row`, and
   `docs/classes/pimCustomDlg.md`.
12. **`pim_ui`'s `pimEntitlementTree`** (`pim_ui/pim_ui_src/pimEntitlementTree.cxx`)
   — the checkbox tree driving the wizard's Applications selection/reconfigure
   step, and the confirmed 2nd `pimCustomDlg` call site. **CONFIRMED BUG
   (crash risk)**: `OnOptionMenuSelect()` calls `eptr_new_xml->SetXml()`
   unconditionally, immediately after an `if (eptr_new_xml)` guard that
   covers only the 2 preceding lines — a null-pointer dereference if
   `pimGetMediaDetails::GetProductXml()` returns `NULL` for the selected
   media, reachable via a normal version/shipcode dropdown change.
   **CONFIRMED BUG**: `ReconfigureDisplay_low()`'s `only_one` branch strips
   one character too many from a computed family-group node id
   (`"_Creo_app"` → `"_Cre"` instead of `"_Creo"`), confirmed by contrast
   with the correctly-computed sibling extraction in `UpdateGroupStatus()`
   in the same file — reachable whenever a reconfigure session has exactly
   1 entitlement. Tracing this class's callers also surfaced a
   **previously-uncatalogued 3rd `.cxx` file** for `pimInstallMgrDlg`'s own
   method bodies, `pim_ui_src/pimEntitlementRefresh.cxx` (1,514 lines, at the
   time not itself traced — since read in full and folded into
   `docs/classes/pimInstallMgrDlg.md`, see Critical Module #4 above). See
   `ai-context/business_rules.yaml`'s
   `pimentitlementtree_optionmenuselect_null_deref`,
   `pimentitlementtree_reconfiguredisplay_low_only_one_off_by_one`, and
   `docs/classes/pimEntitlementTree.md`.
13. **`pim_ui`'s `pimFrictionlessTrialDlg`** (`pim_ui/pim_ui_src/pimFrictionlessTrialDlg.cxx`)
   — the UI for `pim_rl.exe`'s Frictionless/Commercial Trial license-retrieval
   flow (not the main install wizard). **CONFIRMED BUG, the single
   highest-severity finding among the `pim_ui` classes documented so far**:
   for the only confirmed real usage of this class
   (`pimFrictionlessTrialRun()`, which always sets `pimGetInHouseMode()` to 1
   or 2 before calling `Display()`), `Display()` calls
   `FrictionlessLicenseGenerate()` — which starts a background
   license-retrieval worker and arms a polling timer — and then blocks on a
   raw `Sleep(60000)` instead of calling `uiDialog::Activate()`. Because the
   modal event loop never runs, the timer never fires, so
   `OnTimerExpired()` — the ONLY call site anywhere in this codebase for
   `pimFrictionlessTrialLicenseGet::HeartBeat()`, confirmed by an
   archive-wide grep — is never invoked. `HeartBeat()` is where the
   retrieved license is actually saved to disk and the local PSF is
   updated: **none of that completion logic ever executes** in the confirmed
   real usage, and the freshly-allocated worker object leaks (only
   `OnTimerExpired()` frees it). `Display()` reports success regardless.
   See `ai-context/business_rules.yaml`'s
   `pimfrictionlesstrialdlg_display_never_processes_completion` and
   `docs/classes/pimFrictionlessTrialDlg.md`.
14. **`pim_ui`'s `rpimDlg`** (`pim_ui/pim_ui_src/rpimDlg.cxx`) — the UI for
   `pim_re.exe`'s "Renew License" flow, and the closest sibling class to
   `pimFrictionlessTrialDlg` (near-identical structure, 3 years older).
   No severe bug of its own — its value is **comparative**:
   `Display()` correctly calls `uiDialog::Activate()` unconditionally,
   confirming by direct contrast that `pimFrictionlessTrialDlg`'s
   `Sleep(60000)` bug (item 13 above) is a genuine, avoidable defect, not an
   inherent limitation of their shared dialog design. This class shares 2 of
   `pimFrictionlessTrialDlg`'s smaller confirmed-dead-code findings
   (`SetURL()`/`GetAuth()` with zero callers; the same dead reentrancy guard
   in `OnTimerExpired()`) but **not** its `Refresh()` finding —
   `rpimDlg::Refresh()` is confirmed actively used. Also identified
   `pimAuthDlg` as the likely shared template both `rpimDlg` and
   `pimFrictionlessTrialDlg` were partially copied from. See
   `ai-context/business_rules.yaml`'s
   `rpimdlg_display_confirms_pimfrictionlesstrialdlg_bug_is_avoidable`,
   `rpimdlg_vestigial_shared_interface_methods`, and
   `docs/classes/rpimDlg.md`.
15. **`pim_ui`'s `pimAuthDlg`** (`pim_ui/pim_ui_src/pimAuthDlg.cxx`) — the
   generic PTC.com credential prompt, the oldest of this 3-class dialog
   family (2011) and its confirmed original template, called directly from
   `pim_core` (`pimSessionInfo.cxx`, `pimGetAvailable.cxx`). **CONFIRMED
   STRUCTURAL GAP**: `Initialize()` has no idempotency guard — yet this
   class's `AuthDialog` singleton is confirmed genuinely REUSED
   (`Initialize()`/`Display()` invoked multiple times on the same instance
   within one process), via `pimSessionInfo::TryAuthorize()`'s confirmed
   `do`/`while` retry loop and 2 further confirmed call paths. This is
   exactly the one class in the family that needed the guard its own 2
   derivative classes both have. Also confirmed: the `URL` member is
   write-only even in this original template (`SetURL()` IS called here,
   unlike its 2 dead copies — but the value is never read, even here);
   `GetAuth()`'s `bool` return value is always `false`, ignored by both
   confirmed callers; and a commented-out `//AuthDialog->SetBadInputs();`
   reveals a confirmed never-implemented retry-feedback feature. See
   `ai-context/business_rules.yaml`'s
   `pimauthdlg_initialize_missing_idempotency_guard_on_reused_singleton`,
   `pimauthdlg_url_member_dead_even_in_original`, and
   `docs/classes/pimAuthDlg.md`.
16. **`pim_core`'s `pimGetAvailable`** (`pim_core/pim_core_src/pimGetAvailable.cxx`)
   — the web-media Applications-screen download-availability search,
   discovered while tracing `pim_ui_src/pimEntitlementRefresh.cxx`'s
   `EntitlementDownloadPreAction()`. **CORRECTS every prior "all 9 Loop
   subclasses" claim in this documentation set**: `pimGetAvailable`
   (`class pimGetAvailable : public pimLoop`) is a 10th confirmed `pimLoop`
   subclass, previously only mentioned in passing in `docs/modules/pim_core.md`
   and never counted in any Loop-subclass tally. **CONFIRMED**: 2 memory
   leaks in `OnExecute()` (`ImageXml` never freed on the success path;
   `ProductDefinitionXml` never freed when a product is filtered out); the
   declaring comment on `auth_status` (`// -3 abort, 0 success`) is wrong —
   `0` is set on authentication *failure*, not success; `IsAuthorized()` has
   zero callers anywhere in this archive; and `pim_core/pimLocate.cxx` holds
   its own cross-module raw-pointer alias to the same instance (the same
   architectural pattern already flagged as a live bug for `pimSAB.cxx`'s
   statics — see Critical Modules #11 — but here confirmed currently safe
   given the exact call sites that exist today). See
   `ai-context/business_rules.yaml`'s `pimgetavailable_onexecute_memory_leaks`,
   `pimgetavailable_auth_status_comment_wrong`,
   `pimgetavailable_isauthorized_dead_and_locate_alias_fragile`, and
   `docs/classes/pimGetAvailable.md`.
17. **`pim_core`'s `pimGetMediaDetails`** (`pim_core/pim_core_src/pimGetMediaDetails.cxx`)
   — the shared media-details cache/fetcher `pimGetAvailable` itself depends
   on, and a singleton called from at least 7 confirmed files across
   `pim_core` and `pim_ui`. **CONFIRMED, HIGH SEVERITY**: `GetDetails()`'s
   cache-hit fast path reads its shared cache arrays with **no lock at all**,
   while every mutating path holds `Mutex` — this singleton is confirmed
   called from both the UI thread and a background `pimLoop` thread
   (`pimGetAvailable::OnExecute()`), a genuine cross-thread race. **CONFIRMED**:
   `GetProductXml()`'s single-retry-on-timeout path refreshes the details
   cache (logging "Replacing download URLs") but then retries with the
   *same, stale, pre-refresh* URL string — confirmed by direct contrast with
   the correct sibling pattern in `pimEntitlement::UpdateMediaUrls()`, which
   actually applies the refreshed data before retrying. **CONFIRMED,
   severity unknown**: that same retry path also makes a 2nd, likely-redundant
   cache-replace call whose safety depends on an external, unconfirmed
   `dsXArray::Replace()` ownership contract not present in this archive.
   **CONFIRMED**: `GetProductXml()`'s `NULL` returns are the exact,
   concrete root cause of the already-documented null-pointer-dereference bug
   in `pimEntitlementTree::OnOptionMenuSelect()` (see Critical Modules #12) —
   and tracing this class's callers surfaced a confirmed-dead near-duplicate
   of that same bug pattern inside the `#if 0`-disabled
   `uiApplicationsList::OnOptionMenuSelect()`. See
   `ai-context/business_rules.yaml`'s `pimgetmediadetails_cache_hit_path_unlocked`,
   `pimgetmediadetails_getproductxml_stale_url_retry`,
   `pimgetmediadetails_getproductxml_null_is_root_cause_of_entitlementtree_bug`,
   and `docs/classes/pimGetMediaDetails.md`.
18. **`pim`'s `pimGetApplicationsList`** (`pim/pim_src/pimGeneralInit.cxx`) —
   **not a class**: a free-function group
   (`pimGetApplicationsList`/`pimAddApplicationsToList`/
   `IsApplicationInInstallList`) sharing a file-scope `static StringXArray`
   populated by `pimCommandLineArgs()`'s `-APPLICATIONS`/`-XML`/`-XMLALL`/
   `-FLEX` handling, discovered while tracing `pimGetAvailable::OnExecute()`'s
   use of it. **CONFIRMED BUG, HIGH SEVERITY**: `InstallPreReqSilent()`
   (`pim/pim_src/pimTop.cxx:904-966`, on the silent-install path) uses this
   list's `.GetSize()` alone — never its content — to bound a loop indexing
   a completely different array (`pimGetSessionInfo()->GetEntitlement(i)`).
   **CORRECTED** (found in a later, dedicated pass on
   `AddMandatoryAppsForSilentInstall()` itself, see below): the list is
   typically **not** empty in the one real call path, since
   `AddMandatoryAppsForSilentInstall()` always runs first and unconditionally
   attempts to inject `creobase.xml`/`qualityagent.xml` — the real,
   precisely-confirmed consequence is a **size mismatch**, not usually a
   fully empty list. **CONFIRMED**: the shared list is populated in 2
   incompatible formats depending on which of 3 flags is used
   (`-APPLICATIONS`: bare tags; `-XML`/`-XMLALL`: full filesystem paths) — 2
   of 3 confirmed consumers filter via exact match and can never recognize a
   path-format entry, while only `IsApplicationInInstallList()`'s substring
   search is format-agnostic. **CONFIRMED STRUCTURAL RISK** (found in a
   later, dedicated pass on `IsApplicationInInstallList()` itself), the
   opposite direction: its substring search (`.Pos()`) can itself
   false-positive whenever one product's filename is a literal substring of
   another entry already in the list (e.g. a hypothetical
   `"newcreobase.xml"` entry would make a check for `"creobase.xml"` return
   `true` even though `"creobase.xml"` itself was never requested) — checked
   pairwise against the 3 real confirmed arguments used in this codebase
   today, none collides, so this is a confirmed risk, not a demonstrated
   live bug. **CONFIRMED BUGS** (found in a later, dedicated pass on
   `AddMandatoryAppsForSilentInstall()` itself, `pim/pim_src/pimTop.cxx:1282-1316`
   — no header anywhere in this archive declares this function, so no
   Doxygen patch was created for it, the same situation as `Cmp_cStrings`):
   (1) `qualityagent.xml`'s "mandatory" injection is nested entirely inside
   `creobase.xml`'s existence check, so quality agent is silently **not**
   auto-installed on any media shipping `qualityagent.xml` without
   `creobase.xml` — contradicting the function's own name and the
   "Mandatory Installation of Quality Agent" intent independently recorded
   in `pimGeneralInit.h`'s and `pimTop.cxx`'s own `13-Aug-26` changelog
   entries (`$$34`/`$$148`); and (2) `-allpacks` mode's utility-XML entries
   are appended only to `pimSilentInstallFromXML()`'s own local list, never
   to the shared `applications_list`, so `InstallPreReqSilent()`'s later,
   independent `pimGetApplicationsList()` call never sees them — every
   `-allpacks` utility entitlement's prerequisites are silently never
   checked. See `ai-context/business_rules.yaml`'s
   `pimgetapplicationslist_installprereqsilent_wrong_loop_bound`,
   `pimgetapplicationslist_heterogeneous_format_breaks_exact_match_consumers`,
   `isapplicationininstalllist_substring_false_positive_risk`,
   `addmandatoryappsforsilentinstall_qualityagent_gated_on_creobase`,
   `addmandatoryappsforsilentinstall_allpacks_invisible_to_installprereqsilent`,
   and `docs/classes/pimGetApplicationsList.md`.
19. **`pim_core`'s `pimAvailableProduct`** (`pim_core/pim_core_src/pimGetAvailableProduct.cxx`)
   — the small value object (+ its keyed collection `pimGetAvailableProducts`)
   `pimGetAvailable` accumulates web-media search results into. **Promoted
   from a brief "companion" mention** (Critical Modules #16 originally
   folded it in without full scrutiny) **to its own full-depth pass**, which
   surfaced a confirmed bug the lighter treatment missed: **CONFIRMED**:
   `AddInstance(const char *V, const char *shipcode, const char *media_id)`
   never references its own `shipcode` parameter — it builds and dedups its
   `VerShipcodes` entry from `V` (the bare version) alone, despite the
   member name and `Sort()`'s `pimCompareVerShipcode()` comparator both
   implying a combined version+shipcode identifier is intended. A 2nd
   `AddInstance()` call for the same version under a different shipcode is
   silently treated as a duplicate and its media ID is dropped — a shipcode
   option can silently go missing from `pimEntitlementTree`'s
   version/shipcode dropdown as a result. See
   `ai-context/business_rules.yaml`'s
   `pimavailableproduct_addinstance_ignores_shipcode` and
   `docs/classes/pimAvailableProduct.md`. **A later, dedicated pass on
   `Print()` specifically** confirmed both `pimAvailableProduct::Print()`
   and `pimGetAvailableProducts::Print()` (which fans out to it) are dead
   code: zero live callers anywhere in this archive — their only 2
   references (`pimGetAvailable.cxx:474,478`) are both commented out, and
   the log target they'd pass, `pimDbgLog`, isn't confirmed to be a real,
   live logger object anywhere else in this codebase either. See
   `ai-context/business_rules.yaml`'s `pimavailableproduct_print_dead_code`.
20. **`Cmp_cStrings`** (referenced from `pim_ui`, `pim_core` — no owning
   module of its own) — **not documentable at full depth in the usual
   sense**: this `btkMap` comparator has **no implementation anywhere in
   this archive**, confirmed by an exhaustive grep across all 4 modules
   (exactly 8 total matching lines). **CONFIRMED**: independently
   forward-declared via a local `extern` statement in 4 separate `.cxx`
   files (`upimDlg.cxx`, `pimsilentDlg.cxx`, `pimEntitlement.cxx`,
   `pimGetAvailableProduct.cxx`) with no shared header — a maintenance
   hazard if its signature ever needs to change. **CONFIRMED**:
   `pimsilentDlg.cxx`'s declaration is dead/unused. Its likely purpose (a
   `strcmp()`-style `char*` comparator) is explicitly **inferred, not
   confirmed** — there is no body to verify it against. This entry exists to
   document the boundary of what this documentation set can and cannot see,
   not to claim a traced implementation. See
   `ai-context/business_rules.yaml`'s
   `cmp_cstrings_no_implementation_and_redeclared_4_times` and
   `docs/classes/Cmp_cStrings.md`.
21. **`pim_core`'s `pimSilentTestXmlIsUseable`** (`pim_core/pim_core_src/pimSilent.cxx`)
   — **not a class**: a 7-function free-function group
   (`pimSilentTestXmlIsUseable`/`pimIsProductXmlMatch`/`pimIsVersionMatch`/
   `pimSilentCanMatchNeeds`/`pimSilentFixupPSF`/`pimSilentFixupShortcuts`/
   `pimSilentCreateEntitlement`) implementing the silent-install "does the
   user's requested product XML match, and can it be satisfied by, this
   media's product XML" validation and merge pipeline, discovered while
   tracing `pimSilentInstallFromXML()`'s per-entry validation loop (see item
   18 above). **CONFIRMED BUG**: `pimSilentTestXmlIsUseable()` leaks a
   `pimXmlFile` object on its XML-syntax-error path — the `delete` that runs
   on the success path is placed after the error-check block, so the early
   `return false;` inside it skips the cleanup. **CONFIRMED BUG, more
   severe**: `pimSilentCanMatchNeeds()` leaks **both** of its 2 internal
   `pimXmlFile` objects whenever either input file fails to parse — every
   internal early-return and the final success return correctly delete both
   first, only the outer failure fallthrough does not, a clear contrast.
   `pimSilentFixupShortcuts()` in this same file feeds the already-documented
   caller-side indexing bug in item 10 above (`pimShortcutMgr`).
   **CONFIRMED BUG** (found in a later, dedicated pass on
   `pimSilentFixupPSF()` itself, tracing its dependency on
   `pimCommandMgr::DeleteCommand()`/`CanDeleteCommand()`'s explicitly
   documented "cannot delete the last command of a type" constraint):
   the `pDiUmMMY<N>` dummy placeholder this function creates to work around
   that constraint (so a name colliding with a different license type in
   `Eb` can be removed) can itself become permanently stuck/undeletable in
   `Eb`'s final PSF/command set, if no other surviving command ends up
   sharing its license type by the time the final cleanup loop runs — the
   same rule then silently blocks deleting the dummy too, since neither of
   this function's relevant `DeleteCommand()` calls checks the return
   value. **CONFIRMED DISCREPANCY**: the collision-handling code's own
   inline comment says it removes "the foobar command" (singular), but it
   actually deletes **every** command of the colliding license type via
   `GetAllCommandNamesByType()`. **CONFIRMED, more precisely characterized
   reachability + newly traced internal downstream consequence** (found in
   a further, dedicated pass specifically on this discrepancy — not a new
   function, a deeper re-scrutiny of one confirmed finding): this
   function's own opening "Drop" step already deletes every original
   command in `B` before the collision-handling block ever runs —
   `pimCommandMgr::CanDeleteCommand()`'s live "last of a type" count makes
   that Drop loop deterministically leave **exactly 1 surviving command
   per originally distinct license type**, so a given type's *first*
   collision can only ever delete that one straggler, matching the
   comment's singular framing. The discrepancy only actually manifests
   when **2 or more** `A_wants` entries in the same call independently
   collide against different `B` commands that originally shared the
   *same* license type — real but narrower than "any non-colliding
   command" suggests. Traced an internal consequence with the stuck-dummy
   bug above, not previously examined: because each later same-type
   collision's delete sweep runs before its own new dummy exists, it
   deletes the *previous* collision's straggler/dummy while its own dummy
   survives — an unintentional cleanup mechanism for every dummy but the
   **last** one created per shared license type. This **narrows, without
   retracting**, the stuck-dummy finding above: the `pimScriptLoop`
   `[LM_LICENSE_FILE]` corruption risk is confirmed to apply to at most 1
   dummy per originally-distinct license type per call. **CONFIRMED BUG**
   (found in a later, dedicated pass on `pimSilentCanMatchNeeds()` itself,
   tracing its
   dependency on `pimPackageMgr::GetInstallPackageNames()`): this function's
   package-availability check silently changes meaning depending on 3
   global, process-wide command-line mode flags
   (`pimGetBasePackMode()`/`pimGetCreoNGCRIMode()`/`pimGetAllPacksMode()`)
   unrelated to the specific `A`/`B` XML pair being compared — its own
   header comment ("returns false if any packages marked for install in A
   are not known in B") only describes the default, no-special-mode case,
   confirmed by direct contrast with its platform/language sibling checks in
   the same function, which have no such dependency. Under `-allpacks` the
   check becomes **stricter** (every package `A`'s XML declares must exist
   in `B`, regardless of `A`'s own `install="Y"` markings); under
   `-basepack` it becomes **looser** (only `required="Y"` packages are
   checked, ignoring the user's actual selections).
   **CONFIRMED BUG** (found in a later, dedicated pass on
   `pimSilentFixupShortcuts()` itself, a 2nd, distinct bug from its
   already-documented caller-side `B_avail[i]`-vs-`A_wants[i]` indexing bug
   in item 10 above): its `GetShortcutProgramMenu()`/
   `SetShortcutProgramMenu()` pair ignores the `Get`'s return value and
   calls the `Set` unconditionally; since `GetShortcutProgramMenu()` leaves
   its output untouched when a shortcut has no `<PROGRAMSMENU>` child, and
   the same variable is reused across every loop iteration, a stale Program
   Menu group name from an earlier, unrelated shortcut can be copied onto
   the current one (or blanked to empty, on the first iteration) — confirmed
   by direct contrast with the correctly return-value-guarded
   `GetShortcutStartDir()`/`SetShortcutStartDir()` call 3 lines later in the
   same function. **CORRECTED** (found in a later, dedicated pass on
   `pimSilentCreateEntitlement()` itself): the `-basepack`
   "validation gap"/`-allpacks` "overly strict" characterization above was
   too strong. `pimSilentCreateEntitlement()`'s own package-selection loop
   calls the **identical** `GetInstallPackageNames()` on the same `A`, under
   the same mode flags, to decide what to actually install on `B` — and
   since it only ever runs immediately after a successful
   `pimSilentCanMatchNeeds()` call on the same `A`/`B` pair in the same
   process, the 2 calls are guaranteed to see an identical "wanted" package
   set. `pimSilentCanMatchNeeds()` therefore validates *exactly* what
   `pimSilentCreateEntitlement()` will subsequently install — the 2
   functions are consistent with each other, confirmed by design. Only the
   header-comment imprecision remains an accurate part of the finding.
   **CONFIRMED, more precisely characterized reachability + newly traced
   downstream consequence** (found in a further, dedicated pass specifically
   on this package-check mode-dependent finding — not a new function, a
   deeper re-scrutiny of one confirmed finding): the 3 mode flags are
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
   `pimPackageMgr::SetPackageInstallState()`
   (`pim_core/pim_core_src/pimPackageMgr.cxx:250-336`) writes this
   mode-dependent selection directly onto the `<PACKAGE>` node's own
   `install="Y"`/`"N"` attribute on `Eb`'s shared `xmlPtr`.
   `pimMSILoop::IsEligibleForInstall()`
   (`pim_core/pim_core_src/pimMSILoop.cxx:937-969`) reads that **same**
   attribute at real install-execution time — via
   `pimEntitlement::InstallMSI()` constructing a `pimMSILoop` (already a
   fully documented `pimLoop` subclass) through
   `pimMSICopier::MSIInstall()`/`MSICopy_low()` on this same `xmlPtr` — to
   decide whether a package's MSI feature/CDSECTION is actually, physically
   installed. This confirms the mode-dependent logic is the literal,
   unbroken, real-world determinant of installed product features, not
   merely a validation/documentation-accuracy concern. See
   **CONFIRMED STRUCTURAL LIMITATION** (found in a later, dedicated pass on
   `pimIsProductXmlMatch()` itself, shared identically by
   `pimIsVersionMatch()`, which duplicates the exact same control-flow
   shape): neither function has a cross-schema fallback for `B` — whichever
   root-XML schema (`NULL`, i.e. a normal `<PRODUCT>` root, or
   `"EXTERNAL_INSTALLER"`) succeeds for `A` is the only one ever tried for
   `B` in that call; if `B`'s `Init()` fails under that schema, the function
   returns `false` without ever retrying `B` under the other schema. A
   mixed-schema `A`/`B` pair — one `<PRODUCT>`-rooted, one
   `<EXTERNAL_INSTALLER>`-rooted — is confirmed to always report "no match"
   even with identical `<TAG>`/version/shipcode values, surfaced as
   `PIM_SOFTWARE_NOT_FOUND`. Not confirmed reachable with any real
   product/media XML pair in this archive, but the mechanism itself is fully
   confirmed from both functions' own control flow and
   `pimEntitlement::Init()`'s documented root-tag matching contract.
   **CONFIRMED, precisely characterized reachability + newly traced
   downstream consequence** (found in a further, dedicated pass specifically
   on the already-documented `pimSilentFixupPSF()` dummy-placeholder bug —
   not a new function, a deeper re-scrutiny of one confirmed finding): the
   dummy survives exactly when no entry in `A`'s final wanted command set
   independently shares the colliding command's *old* license type — a
   plausible scenario (e.g. a license-type rename between product versions),
   not just a contrived edge case. `pimEntitlement::InstallScripts()`
   (`pim_core/pim_core_src/pimEntitlement.cxx:5627-5631`) constructs a
   `pimScriptLoop` (one of the already-documented `pimLoop` subclasses)
   directly on the **same** `pimXmlFile*` document `pimSilentFixupPSF()`
   mutates, at real install-execution time. `pimScriptLoop::OnInstall()`
   reads the *first* name `GetAllCommandNames()` returns — unconditionally,
   with no type-based selection — to populate `[LM_LICENSE_FILE]` for that
   entitlement's generated install scripts, so a stuck dummy landing at that
   position would corrupt `[LM_LICENSE_FILE]` for the entire entitlement
   using its own internally-mismatched data, not just leave an inert PSF
   entry behind. Whether the dummy reliably reaches that position in any
   real product XML is not confirmed either way in this archive; the
   mechanism connecting the 2 functions via the shared XML document is fully
   confirmed.
   **FIX VERIFIED (PARTIALLY), found in a further, dedicated pass verifying
   this bug's proposed fix — not a new function, a deeper scrutiny of one
   already-documented, 2-option remediation**: "either `pimSilentFixupPSF()`
   must guarantee no dummy survives, or `pimScriptLoop` must select the
   license command by type/role rather than array position 0." **Option 1
   confirmed already true in half the cases, structurally unachievable in
   the other half**: when another `A`-wanted command independently shares
   the dummy's license type, its own `AppendAddCommand()` call already
   restores the type's live count to ≥2 before the final cleanup loop
   runs — the dummy is **already** correctly deleted today, no fix needed.
   When no such entry exists, `CanDeleteCommand()`'s floor has **no
   override anywhere in `pimCommandMgr`** — confirmed structurally
   impossible to reduce any license type to 0 live commands via any
   operation ordering. Closing this remaining gap needs a genuinely
   **new** `pimCommandMgr` capability — an in-place "retype" operation
   reusing `AppendAddCommand()`'s own template-cloning machinery against
   the existing node, never passing through a 0-count instant. **Option 2
   confirmed architecturally precedented, but concretely under-specified**:
   `pimScriptLoop::OnInstall()` itself, a few lines below the `arr[0]`
   lookup, already performs exactly this kind of role-based selection for
   a *different* command — `FindNodelistAttribMatch(pimPSF, pimid,
   "parametric")` (`pimScriptLoop.cxx:135`) — but no canonical selector
   value for "the license-bearing command" is confirmed to exist anywhere
   in this archive. **Further confirmed**: `AppendEditCommand()` never
   creates a missing `<FEATURE_NAME>` child, only updates an existing
   one, deepening the already-flagged uncertainty over whether the dummy
   can even reach `pimScriptLoop`'s `arr` at all. **New, dummy-independent
   design smell**: `arr[0]`'s "just take the first command" assumption is
   fragile on its own terms, since this same function already relies on
   multiple, role-distinct PSF commands coexisting.
   **CONFIRMED, found in a further, dedicated pass specifically on this
   function's `DeleteCommand()` return-value check — not a new function, a
   precise per-call-site trace of what checking it would actually reveal,
   and a genuine generalization of the stuck-dummy finding above**: there
   are exactly 3 `DeleteCommand()` call sites. The **Drop step**'s
   discarded return is a **verified non-issue** — its own by-design
   terminal state, already documented by its own inline comment. The
   **collision-handling delete** is **confirmed, by an exhaustive counting
   proof, to always succeed** — the freshly `AppendAddCommand()`'d dummy
   guarantees the type's member count stays above 1 through every deletion
   in the loop — *conditional* on `AppendAddCommand()` itself succeeding,
   which is **also** never checked, and traced to depend on a matching
   `<PSF_TEMPLATE>` existing (not confirmed either way in this archive); if
   that precondition failed, the subsequent re-add loop would silently
   leave the wrong license type permanently attached to the colliding
   command, since neither `AppendAddCommand()` nor `AppendEditCommand()`
   ever updates an existing node's `<LICTYPE>`. The **final cleanup
   loop**'s discarded return is where checking matters most: **any**
   command — dummy or genuinely original — that is the sole surviving
   representative of its license type gets permanently, silently stuck
   whenever `A_wants` contains no command of that type at all, confirming
   `pimSilentFixupPSF()`'s own stated goal ("eliminate the names from `B`
   that are not part of `A_wants`") is structurally unachievable for an
   entire license type in that case — not a corner case. A stuck non-dummy
   leftover carries real, valid (if unrequested) license data, distinct
   from the dummy's mismatched data, but can equally reach
   `pimScriptLoop::OnInstall()`'s unconditional `arr[0]` read for
   `[LM_LICENSE_FILE]`.
   **CONFIRMED BUG, found in a further, dedicated pass specifically on this
   function's `AppendAddCommand()` return-value check — not a new
   function, mirroring the `DeleteCommand()` pass above but finding a more
   severe, more clearly reachable failure mode**: there are exactly 2
   `AppendAddCommand()` call sites, both discarding the return value.
   Traced its implementation: it is **add-or-update** — an existing `name`
   is simply updated and always succeeds — but creating a genuinely new
   node depends on finding a matching `<PSF_TEMPLATE>`; if none exists,
   nothing is created and the return is `false` with **zero trace
   anywhere**. The **dummy-creation site** is confirmed lower reachability
   — its license type is read from an existing live `B` command,
   plausibly (though not certainly) already templated. The **main re-add
   loop** is confirmed more severe and more clearly reachable: when `A`
   wants a command name genuinely new to `B`, its license type comes from
   `A`'s own document, entirely independent of `B`'s template set — if
   `B`'s media lacks a matching template (a plausible
   cross-version/cross-product mismatch), `A`'s entire request for that
   command is **silently dropped**, not stuck like a dummy, simply
   **absent**, with no diagnostic anywhere. **Confirmed by this function's
   own reasoning**: its inline comment introducing this loop shows the
   author reasoned through name/type collisions but never considered
   template availability — a genuine blind spot, not an accepted risk.
   **Downstream consequence**: the failed command never even appears in
   the final cleanup loop's re-fetch, so there is nothing to strand — the
   user's requested licensed feature/capability is simply never
   provisioned, with no dummy, no log, and no error anywhere in the
   pipeline — arguably harder to diagnose than either `DeleteCommand()`
   finding, since there is no leftover artifact to even notice.
   **CONFIRMED, precisely characterized reachability + newly
   traced downstream consequence** (found in a further, dedicated pass
   specifically on the already-documented `pimSilentFixupShortcuts()`
   stale-Program-Menu-copy bug — not a new function, a deeper re-scrutiny of
   one confirmed finding): `<PROGRAMSMENU>` is confirmed to be an
   **independently optional** per-shortcut child element —
   `pim_core/pim_core_src/pimShortcutLoop.cxx:88-341` parses it alongside
   `<STARTMENU>`/`<DESKTOP>`/`<QUICKLAUNCH>` as sibling children of
   `<SHORTCUT>` in the same `else if` chain, each independently
   present-or-absent (a shortcut offered only via Desktop/Quicklaunch
   legitimately has no `<PROGRAMSMENU>` node at all) — so this bug's trigger
   condition is a normal configuration, not a contrived edge case.
   `pimEntitlement::InstallShortcuts()`
   (`pim_core/pim_core_src/pimEntitlement.cxx:5778-5787`) calls
   `pimShortcuts::GetInstance().Create(xmlPtr)` on the **same** `pimXmlFile*`
   document `pimSilentFixupShortcuts()` mutates, constructing a
   `pimShortcutLoop` (another already-documented `pimLoop` subclass) on it at
   real install-execution time. `pimShortcutLoop::OnInstall()`
   (`pim_core/pim_core_src/pimShortcutLoop.cxx:324-326,654-671`) reads that
   shortcut's `<PROGRAMSMENU>` text and uses it **directly as a filesystem
   subfolder path** (`location /= (cStringT)programsmenu;`) to place the
   installed `.lnk` file — stronger and more deterministic than the
   `pimSilentFixupPSF()` finding above, since it does not depend on ambiguous
   document ordering. Confirmed consequences: a stale non-empty value
   misplaces the current shortcut's icon into a **different, earlier-processed
   shortcut's** Start Menu folder; a stale empty value (first loop iteration)
   causes the Program-Menu placement to be **silently skipped entirely**, even
   if this shortcut's own "create in Programs Menu" flag was set `true` — a
   visible, user-facing install defect, not merely a data-integrity concern.
   **CONFIRMED, more precisely characterized reachability + newly traced
   downstream consequence** (found in a further, dedicated pass specifically
   on the already-documented `pimIsProductXmlMatch()`/`pimIsVersionMatch()`
   no-cross-schema-fallback structural limitation — not a new function, a
   deeper re-scrutiny of one confirmed finding): this codebase's **own**
   other 2 product-XML resolution call sites do **not** assume a fixed
   schema for a single file — `pim_core/pim_core_src/pimEntitlement.cxx:634-635`
   (resolving a product's own installed `.p.xml`) tries `Init(file, NULL) ||
   Init(file, "EXTERNAL_INSTALLER") || Init(file, "HIDDEN_PRODUCT")` in
   sequence, and `pimEntitlement.cxx:967-968` (resolving a `<PREREQUISITE>`
   reference) tries `Init(x) || Init(x, "EXTERNAL_INSTALLER")`. Both
   confirm, elsewhere in this exact codebase, that a product's root schema
   is treated as unpredictable and deliberately checked for by trying
   multiple schemas — direct, confirmed evidence, not mere speculation,
   that `pimIsProductXmlMatch()`/`pimIsVersionMatch()`'s
   single-schema-for-both-`A`-and-`B` assumption is unsafe by this
   codebase's own design. `pimIsProductXmlMatch()`'s only caller,
   `pimSilentTestXmlIsUseable()`, is itself only called from
   `pimSilentInstallFromXML()`'s per-file loop
   (`pim/pim_src/pimTop.cxx:1532`), which simply skips (no `abort`, no
   distinct error) any product whose match fails this way. Since
   `pimSilentInstallFromXML()`'s own final return value is
   `pimGetLastError()` (`pimTop.cxx:2073`, `pim/pim_src/pimExit.cxx:16-33`)
   — a process-wide static array's **last** appended error, not a per-file
   record — a mixed-schema false-negative on one product in a multi-XML
   batch silent install can be silently overwritten in the reported exit
   code by any later, unrelated error, losing which specific product failed
   the match and why.
   **CONFIRMED BUG, upgraded from a lower-confidence structural note**
   (found in a further, dedicated pass specifically on
   `pimSilentCreateEntitlement()`'s already-documented `<MSI>`-node
   last-value-wins caveat — not a new function, a deeper re-scrutiny of one
   already-flagged caveat): `pimMSILoop::pimMSIExec()`
   (`pim_core/pim_core_src/pimMSILoop.cxx:154-161`, the actual MSI
   install-execution driver, already a fully documented `pimLoop` subclass)
   independently iterates and executes **every eligible** `<MSI>` node in
   the document as its own separate install action, confirming
   multi-`<MSI>`-node product XML is a real, designed-for configuration —
   not the hypothetical edge case it was previously flagged as (no
   multi-`<MSI>`-node XML was available in this archive to test against).
   `pimSilentCreateEntitlement()`'s `<MSI>`-node copy loop declares
   `format`/`Cmd` once outside its read loop from `A`, so only the **last**
   `<MSI>` node's values survive, and stamps that one pair onto **every**
   `<MSI>` node in `B`. `attribFormat`
   (`pim_core/pim_core_src/pimMSILoop.cxx:596-614`) directly selects each
   `<MSI>` node's install **UI mode** (`"full"` interactive wizard,
   `"basic"`, or silent) at real install time, and the `<MSIARGUMENT>` text
   feeds that node's `msiexec.exe` command line — so a product with 2+
   independently-configured MSI packages has this bug forcing all of them
   to adopt whichever single mode/argument pair belonged to the last
   `<MSI>` node in the user's request XML, a confirmed, install-time
   consequential data-corruption bug that can visibly change which
   installer UI a user sees.
   **FIX VERIFIED (PARTIALLY), found in a further, dedicated pass
   specifically verifying this bug's already-proposed fix — not a new
   function, a deeper scrutiny of one already-recommended remediation
   ("match by node identity, e.g. the `pimname`/`pimPRODUCTCODE`
   attribute")**: traced `pimMSIExec()`'s own attribute reads
   (`pim_core/pim_core_src/pimMSILoop.cxx:202-207`) off the identical
   `<MSI>` nodelist this function iterates — confirms both `name` and
   `PRODUCTCODE` genuinely exist on real `<MSI>` nodes, grounding the
   proposed fix in fact. **But the fix's own "either" phrasing is
   under-specified**: the 2 keys have opposite tradeoffs — `PRODUCTCODE`
   is a Windows Installer GUID conventionally expected to change on most
   new MSI builds, so a `PRODUCTCODE`-only match would likely find no
   correspondence at all between `A`'s (older/customized) request and
   `B`'s (newly matched) media in the realistic case this function
   handles, silently degrading the fix to "no customization ever
   survives." `name` is the practically workable key but isn't
   schema-enforced unique. A robust fix needs both — `PRODUCTCODE` tried
   first, `name` as fallback. **New asymmetry found while verifying,
   orthogonal to the name-collapsing bug**: the existing write loop's
   `format`-attribute handling only overwrites `B`'s attribute if it
   already exists (no creation fallback), while the `<MSIARGUMENT>`
   handling does create a missing child — any correctly-scoped per-node
   fix must consciously address this too.
   **CONFIRMED, found in a further, dedicated pass specifically on
   `pimSilentCreateEntitlement()`'s already-documented `<PROPERTY>` skip
   list — not a new function, a deeper re-scrutiny of one already-noted
   mechanism**: traced a distinct, confirmed downstream reason for each of
   the 4 skipped names (`[SHIPCODE]`/`[VERSION]`/`[SOURCE]`/`CustomActions`),
   not previously examined. **`[SHIPCODE]`**: read immediately after `Eb`'s
   creation by `pimSilentInstallFromXML()`
   (`pim/pim_src/pimTop.cxx:1556-1559`) to populate the session's own
   `SHIPCODE_PROPERTY` from `Eb`'s **own** value. **`[VERSION]`**:
   originally set by `pimEntitlement::Init()` itself
   (`pim_core/pim_core_src/pimEntitlement.cxx:944,1195`) from `Eb`'s own
   `<PRODUCT version>` attribute, later read by `pimCustomActionsLoop`
   (`pim_core/pim_core_src/pimCustomActions.cxx:542`) for version-gated
   custom-action behavior and by `pimSessionInfo`
   (`pim_core/pim_core_src/pimSessionInfo.cxx:1801,2316`) for
   cross-entitlement matching. **`[SOURCE]`**: unconditionally overwritten
   anyway moments after this function returns
   (`pim/pim_src/pimTop.cxx:1598`). **`CustomActions`**: gates **many**
   real install/uninstall lifecycle hook points via
   `pimEntitlement::OnInstallCustomActions()`/`OnUninstallCustomActions()`'s
   `xmlPtr->GetProperty("CustomActions", str)` truthy-check
   (`pim_core/pim_core_src/pimEntitlement.cxx:5151,5882,6024,6669,6753,6837,6842,7023,7159,7228,7309`),
   each constructing a `pimCustomActionsLoop` — a **newly-discovered
   `pimLoop` subclass** (`pim_core/includes/pimCustomActions.h` +
   `pim_core_src/pimCustomActions.cxx`) not previously mentioned anywhere
   in this doc set — on the same `xmlPtr`; the property's mere
   **presence**, not even its value, gates whether `B`'s own custom-action
   fixups run at all. Also **verified a non-issue, not a bug**: the copy
   loop's `name` variable is declared once outside the loop, so a
   `<PROPERTY>` node lacking a `name` attribute would leave `name` stale —
   but the copy loop's own guard (`attribName != NULL`) means 0 copies
   happen either way, so the stale value has no effect, unlike the
   superficially similar but genuinely live bug already documented above
   in `pimSilentFixupShortcuts()`'s Program Menu handling.
   **FIX VERIFIED (COSMETIC), found in a further, dedicated pass
   specifically verifying this loop's proposed hardening fix — not a new
   function, a deeper scrutiny of one already-recommended, low-priority
   remediation**: the fix ("initialize `name` fresh each iteration") is
   confirmed to change nothing observable today — the inner for-loop's own
   `attribName != NULL` guard is the sole, always-present protection,
   regardless of what the outer skip-list `continue` does with a stale
   `name`. The fix's real value is closing a latent maintenance trap: a
   plausible future refactor weakening the inner guard (mistakenly
   assuming the outer `continue` already handles nameless nodes) would
   silently reintroduce exactly the kind of stale-value corruption already
   confirmed live in `pimSilentFixupShortcuts()`'s Program Menu bug.
   **CONFIRMED BUG, found in the same pass — a new, distinct finding**:
   `pimXmlFile::GetProperty()`'s own return value is *also* discarded at
   both value-copy sites in this loop; if `A` ever has 2+ `<PROPERTY>`
   nodes sharing the same `name` (not schema-prevented in this archive),
   `GetPropertyNode()`'s document-order-first-match search could return a
   *different* node than the one currently being iterated, and a missing
   attribute there would leave `value` stale — silently written to `Eb`
   regardless. The general-attribute branch could avoid this entirely by
   reading its already-in-hand attribute node directly instead of
   re-searching `Ea`'s whole document by name.
   **CONFIRMED, found in a further, dedicated pass specifically on
   `pimIsVersionMatch()`'s shipcode comparison — not a new function, a
   deeper re-scrutiny of one already-noted mechanism**: the shipcode check
   is **optional, not enforced** — it only runs
   `if (Ea.GetShipcode(MOR_A) && Eb.GetShipcode(MOR_B))`, and
   `pimEntitlement::GetShipcode()` (`pim_core/pim_core_src/pimEntitlement.cxx:2066-2090`)
   returns `false` whenever the root `<PRODUCT>` node has neither
   `appshipcode` nor `shipcode`. `pimEntitlement::Init()`
   (`pim_core/pim_core_src/pimEntitlement.cxx:846-870`) only requires
   `tag`/`version` — not `shipcode`/`appshipcode` — so a valid,
   `Init()`-succeeding product XML can legitimately omit both, silently
   bypassing the gate (not confirmed against any specific real
   product/media XML in this archive, since none exists, but the mechanism
   is fully confirmed from `Init()`'s own attribute requirements). Traced
   the downstream effect for the first time: when skipped this way,
   `pimIsVersionMatch()` returns `true` via the **exact same path** as an
   explicit shipcode match — no log, warning, or property records that the
   comparison didn't actually run — and `pimSilentTestXmlIsUseable()`'s own
   differentiated-error logic (`Major`/`Minor`) is only ever inspected on
   the **rejected** path, so it's never consulted when the check was
   silently skipped. Also verified 2 non-issues: `Minor` is only set
   `true` when shipcodes are exactly equal, not when `B`'s is legitimately
   newer (the function's own documented common case) or when the check is
   skipped — but since `Major`/`Minor` are provably unread on every
   accepted path, it has no effect; and `pimCompareShipcode()`'s parameter
   names (`new_ship`/`old_ship`) don't reflect an enforced argument-order
   contract — confirmed via its only other call site
   (`pimEntitlement::GetSize()`) to be a generic, symmetric comparator, so
   this call's `A`-then-`B` order is valid, not a naming-driven bug.
   **CONFIRMED, found in a further, dedicated pass specifically on
   `pimSilentCanMatchNeeds()`'s platform and language checks — not a new
   function, a deeper re-scrutiny of one already-noted mechanism**: both
   are genuinely free of the package check's mode-dependence
   (`GetInstallPlatformNames()`/`GetInstallLanguageNames()`, plain,
   unconditional `install="Y"` checks, no global mode flags for either),
   but **lack the package check's later-confirmed consistency guarantee**.
   **CORRECTED** (found in a further, dedicated pass on
   `pimSilentFixupPSF()`'s `Ea`/`Eb` reference-vs-copy semantics, which
   required re-reading `pimSilentCreateEntitlement()`'s full body): this
   claim was incomplete. `pimSilentCreateEntitlement()`
   (`pim_core/pim_core_src/pimSilent.cxx:549-569`) **does** mirror `A`'s
   platform/language onto `B` first, via the **identical**
   `GetInstallPlatformNames()`/`GetInstallLanguageNames()` calls this
   check uses — the same consistency pattern already confirmed for
   package selection, not an absent one. The real, narrower gap is that
   this internally-consistent mirroring is **subsequently undone** by a
   *second*, independent write, described below — everything else in this
   paragraph remains accurate and describes that 2nd write, not a gap in
   `pimSilentCreateEntitlement()` itself.
   Unlike the package check (confirmed by an earlier pass to validate
   exactly what `pimSilentCreateEntitlement()` will select, since both
   call the identical `GetInstallPackageNames()`), `pimSilentInstallFromXML()`'s
   post-creation code (`pim/pim_src/pimTop.cxx:1602-1635`, run **after**
   this validation, and after `pimSilentCreateEntitlement()`'s own
   consistent mirroring) re-derives what actually gets selected on `Eb`
   from entirely different inputs: **platform** via
   `pimPlatformMgr::InitPlatformState(NULL)`
   (`pim_core/pim_core_src/pimPlatformMgr.cxx:104-125`), which uses
   `btkGetPlatform()` — the current machine's own runtime OS — with a
   2-level fallback chain; **language** via 9 hardcoded
   `SetLanguageInstallState()` calls gated by `pimShouldWeInitLanguageID()`
   (`pim/pim_src/pimGeneralInit.cxx:531-539`), driven by the `-LANG`
   command-line flag list or `-allpacks` (which force-selects all 9) —
   none of which this validation ever sees. Traced the downstream
   consequence for the first time: both `pimTop.cxx` call sites discard
   their `Set*InstallState()` calls' return values, which fail **silently**
   (no log, no error) whenever the target doesn't exist in `B` — a
   `-lang XX` (or `-allpacks`) request for a language `B` genuinely lacks
   is silently dropped with zero diagnostic anywhere in the pipeline, and
   if the auto-detected platform and both fallbacks all fail to match
   anything in `B`, the entitlement silently ends up with no platform
   marked for install at all.
   **FIX (a) VERIFIED SUFFICIENT ONLY FOR DIAGNOSABILITY, found in a
   further, dedicated pass verifying this bug's 2 proposed fixes — not a
   new function, a deeper scrutiny of 2 already-documented
   remediations**: checking the return values and logging the failure
   makes the outcome visible but changes nothing about it — traced the
   zero-platform case one level further into
   `pimPackageMgr::RefreshAFeatureNode()`, confirming every
   `<CDSECTION>`/`<MSI>` node carrying a `platform` attribute gets set
   `install="N"` whenever no platform is ever selected, a near-total
   silent install failure logging alone cannot prevent. **CORRECTION,
   found in the same pass**: `InitPlatformState()`'s "2-level fallback" is
   more precisely a **mutually exclusive, conditionally-chosen single
   fallback**, not both names tried in sequence. **FIX (b) CONFIRMED
   ARCHITECTURALLY FEASIBLE for both language and platform, resolving the
   original "if feasible" hedge**: `pimShouldWeInitLanguageID()`/
   `pimGetAllPacksMode()` and `btkGetPlatform()` are all global,
   parameterless accessors already read this same way elsewhere in this
   exact file — no CLI-context plumbing needed. **New nuance**: a
   validating copy must exactly replicate the corrected single-fallback
   branch, or it produces false-negative rejections. **New
   maintenance-hazard**: implementing it duplicates the 9-hardcoded-
   language rule and the platform fallback into a 2nd, independent copy,
   with nothing keeping it in sync with `pimTop.cxx`'s original. **New,
   unresolved policy question**: extending validation to auto-detected
   platform would let a machine merely lacking that platform have its
   otherwise-valid install rejected outright, rather than proceeding with
   that piece silently missing, as today — a real behavior decision this
   codebase never states an answer to.
   **CONFIRMED, found in a further, dedicated pass specifically on
   `pimSilentFixupPSF()`'s `Ea`/`Eb` reference-vs-copy semantics — not a
   new function, a deeper re-scrutiny of one already-documented
   function's own parameters**: both parameters are declared as
   symmetric, non-`const` references, but a full call trace confirms
   **asymmetric roles** — every `Ea`/`A_cmds` call is a read-only
   accessor (`GetCommandInfoByName()`/`GetAllCommandNames()`), never a
   mutator, while `Eb`/`B_cmds` is the **sole** read-write target
   (`DeleteCommand()`/`AppendAddCommand()`); `Ea` could safely be
   `const pimEntitlement&`. Traced the sole call site's own construction
   of both objects: `Ea` is a genuinely **ephemeral**, stack-allocated
   `pimEntitlement`, destroyed (its `xmlPtr` explicitly deleted) the
   moment `pimSilentCreateEntitlement()` returns, while `Eb` is **not a
   copy at all** — a pointer to the real, session-owned, **persistent**
   entitlement object, dereferenced and passed by reference — confirming
   why every already-documented downstream consumer (`pimScriptLoop`,
   `pimShortcutLoop`, `pimMSILoop`) genuinely reads the same object this
   function wrote to. **Reachability**: `pimEntitlement` owns a raw
   `xmlPtr` deleted in its destructor but defines no custom copy
   constructor or assignment operator, which would double-free `xmlPtr`
   if 2 instances ever shared it by value — a real but confirmed
   **currently unreachable** hazard: an exhaustive search confirms only
   3 stack-allocated, value-type `pimEntitlement` instances exist
   anywhere in this codebase (all in this same file, all used safely);
   every other entitlement is held via `dsXArray<pimEntitlement*>` or a
   raw pointer. **Verified non-issue**: values `pimSilentFixupPSF()`
   copies from `Ea` into `Eb` are genuine value copies, not references
   into `Ea`'s own DOM tree, so `Eb` holds no dangling reference after
   `Ea`'s destruction.
   **CONFIRMED NOT PURELY MECHANICAL, found in a further, dedicated pass
   verifying the `Ea` const-reference hardening suggested just above — not
   a new function, a deeper scrutiny of one already-documented, minor
   remediation**: this function's only direct call on `Ea`,
   `Ea.GetXMLPtr()`, is itself not declared `const`
   (`pimEntitlement.h:320`), so `const pimEntitlement& Ea` as literally
   proposed **fails to compile**. **Confirmed safe prerequisite**:
   `const`-qualifying `GetXMLPtr()` itself (same non-`const` return type, a
   trivial accessor that never mutates anything) resolves this with zero
   behavior change anywhere, and confirmed zero existing `const
   pimEntitlement` usage anywhere in this codebase to regress. **Confirmed
   scope ceiling**: even fully applied, the hardening only blocks a
   non-`const` `pimEntitlement`-level call directly on `Ea` — reading
   `pimCommandMgr.h` in full confirms **none** of its public methods,
   including the 3 read-only accessors this function calls, are declared
   `const`, so `A_cmds` itself could still mutate `Ea`'s document with no
   compiler objection, before or after the fix. The hardening is confirmed
   to be a documentation-only signal at this function's own signature, not
   a real guard against the realistic mistake one layer below.
   **CONFIRMED BUG, found in a further, dedicated pass specifically on
   `pimSilentCreateEntitlement()`'s session-index lookup — not a new
   function, a deeper re-scrutiny of one already-documented function's own
   out-parameter mechanism**: the `entitlement_index` out-parameter
   (`pim_core/pim_core_src/pimSilent.cxx:520`) is set to a valid,
   non-negative index immediately after `AddEntitlement(B)` succeeds, but
   `A`'s own parseability isn't checked until `:522-524` — if `Ea.Init(A)`
   fails under both schemas (normal and `EXTERNAL_INSTALLER`), the
   function returns `false` at `:530` without resetting
   `entitlement_index`. Traced the only caller
   (`pim/pim_src/pimTop.cxx:1534-1540`): this function's own `bool` return
   value is never inspected — the caller relies solely on `k`'s sign
   (pre-initialized `-1`, checked via `if (k < 0)`), which cannot catch
   this specific failure mode since `k` is already overwritten to a valid
   index — execution proceeds treating a half-created `B` (added to the
   session, but never reaching
   `pimSilentFixupPSF()`/`pimSilentFixupShortcuts()`) as if creation fully
   succeeded. **Reachability**: not confirmed reachable via the codebase's
   own single call site — `pimSilentTestXmlIsUseable()` (called
   immediately before, on the identical `A`) already requires `A`'s parse
   to succeed under one of the same 2 schemas before this call is ever
   reached, making the bug a confirmed but currently latent gap in this
   function's own error-handling design. **Verified non-issue, a related
   but distinct mechanism**: `pimSessionInfo::AddEntitlement(cStringT)`'s
   own pre-existing-entry guard compares a filesystem path against
   `GetID()` (a product tag), so it never actually fires — confirming
   `AddEntitlement()` succeeding always means a brand-new array entry was
   just appended, so the session-index lookup's own core arithmetic is
   otherwise sound.
   **FIX VERIFIED (SUFFICIENT, either one alone), found in a further,
   dedicated pass verifying this bug's 2 proposed fixes — not a new
   function, a deeper scrutiny of 2 already-documented remediations**:
   confirmed there is exactly 1 failure path to fix (`:524-531`), not
   several — no other early return exists between `:520` and the final
   `return true;` at `:743`. Fix (a), the caller also checking this
   function's `bool` return value, needs no change to `pimSilent.cxx`; fix
   (b), resetting `entitlement_index` internally, needs no change to
   `pimTop.cxx`. **New downstream consequence, traced one step further**:
   an undetected failure reaches `pimTop.cxx:1777`'s `SessionInfo.Save()`
   (once the whole `AskedToInstallThisList` loop completes without any
   other entry aborting), which **persists** the corrupted session
   document — including the orphaned `<ENTITLEMENT>` node — to
   `sessioninfo.xml` on disk; either fix prevents this by making the
   caller's existing early-abort fire before that line is ever reached.
   **Confirmed bug in the alternative fix's own literal wording**: "call
   `DropEntitlement(B)`" is confirmed non-functional, since `B` is a
   filesystem path and `DropEntitlement(cStringT)` matches by product tag —
   the identical mismatch already confirmed dead code in `AddEntitlement()`'s
   own guard above; a working rollback needs `DropEntitlement(entitlement_index)`
   instead. **Verified non-issue**: given the traced consequence, a full
   rollback turns out not to be necessary for correctness today, since
   either detection fix alone already prevents ever reaching `Save()`.
   **CONFIRMED BUG, found in a further, dedicated pass specifically on
   `pimSilentCanMatchNeeds()`'s package check reachability — not a new
   function, a deeper re-scrutiny of one already-documented finding's
   underlying dependency**: traced
   `A_pkg.GetInstallPackageNames()`/`B_pkg.GetAllPackageNames()`
   (`pim_core/pim_core_src/pimSilent.cxx:307-308`) into `pimPackageMgr`
   (`pim_core/pim_core_src/pimPackageMgr.cxx:611-666,550-575`) — neither
   function null-checks a `DOMNamedNodeMap::getNamedItem()` result before
   dereferencing it via `->getNodeValue()`, for `name` (both) and,
   depending on the active CLI mode, `install`/`required`/`parent`
   (`GetInstallPackageNames()` only). **CONFIRMED DISCREPANCY**:
   `pimPackageMgr::GetPackageInfoByName()`, a sibling method in this same
   class reading the exact same 4 attributes, guards every one of them
   before dereferencing — confirmed evidence this class's own author
   anticipated these attributes can legitimately be absent, making the
   unguarded access an inconsistency, not a deliberate assumption.
   **Reachability**: `install` is dereferenced unconditionally in both
   default mode and `-allpacks` mode (it's the left operand of `||`,
   evaluated before `pimGetAllPacksMode()` is ever checked, defeating the
   assumption that "install everything" mode tolerates a missing `install`
   attribute); `required` unconditionally under `-basepack`/`-releaselink`;
   `parent` additionally under `-releaselink` when `required` isn't `"Y"`;
   `name` only for a node the active mode has already decided to select.
   Not confirmed reachable against a real product/media XML pair (none
   exists in this archive, and no `<PACKAGE>`-node-authoring code exists
   in this archive either), but the crash mechanism and the internal
   inconsistency are fully confirmed from source. Also confirmed this
   exposure is the **widest** of the 3 sibling checks: `pimPlatformMgr`/
   `pimLanguageMgr`'s equivalents share the same unguarded `name`/`install`
   pattern (2 vulnerable attributes each), but lack package's extra
   mode-branching (4 vulnerable attributes).
   **FIX VERIFIED, found in a further, dedicated pass specifically
   verifying `pimSilentFixupShortcuts()`'s already-proposed
   stale-Program-Menu-copy fix — not a new function, a deeper scrutiny of
   one already-recommended remediation**: gating `SetShortcutProgramMenu()`
   on `GetShortcutProgramMenu()`'s return value, mirroring the adjacent
   `GetShortcutStartDir()`/`SetShortcutStartDir()` pattern, is **confirmed
   sufficient**. Traced `SetShortcutProgramMenu()`'s own implementation
   (`pim_ui/pim_ui_src/pimShortcutMgr.cxx:169-189`) to confirm skipping the
   call when `Get` fails leaves `Eb`'s shortcut untouched, so its own
   original template value survives intact, eliminating every corruption
   path in the original finding with no residual case. **NEW LIMITATION,
   shared by the fix and the pattern it mirrors alike**: neither
   `SetShortcutProgramMenu()` nor `SetShortcutState()` (`:426-454`, the
   shared primitive behind `SetShortcutProgramsMenuState()`'s toggle) nor
   `SetShortcutStartDir()` itself (`:213-233`) can ever create a missing
   child node — all 3 only mutate an existing one. Even with the fix
   applied, `A` can never grant a shortcut a Program-Menu (or
   start-directory) customization that `B`'s own media template never
   defined in the first place — a silent, non-crashing, by-design ceiling,
   confirmed consistent with `pimShortcutLoop::OnInstall()`'s own parsing
   simply never matching an absent node, not a defect the fix introduces
   or leaves unaddressed.
   **CONFIRMED BUG, found in a further, dedicated pass on
   `pimSilentCreateEntitlement()`'s quality-agent flag copy — a previously
   untouched area of an already-documented function, not a new
   function**: traced `Ea.IsQualityAgentEnabled(tf)`/`Eb->SetQualityAgent(tf)`
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
   this function blindly attempts, with no equivalent precondition check
   in the silent-install path. **Reachability**: unlike the package check,
   nothing in this pipeline pre-validates QualityAgent state —
   `pimSilentCanMatchNeeds()` never inspects it — so an older saved
   request XML with quality agent explicitly disabled, applied against a
   newer media definition that has since made it required, silently loses
   the opt-out on reapplication. **CONFIRMED DOWNSTREAM CONSEQUENCE**:
   `Eb`'s final `enable` state is read at real install-execution time by
   `pimEntitlement::OnInstall()`/`OnReconfigure()` to write or remove a
   real `"QualityAgentOptIn"` Windows registry value — the GUI's own
   "PHM"/legal-text framing confirms this is a genuine, user-facing
   telemetry opt-in setting with consent implications, not a latent
   XML-consistency concern. **Verified non-issue**: the enable direction
   carries no equivalent restriction.
   **CONFIRMED BUG, found in a fresh, dedicated pass on
   `pimSilentCreateEntitlement()`'s package selection — a previously
   untouched part of an already-documented function, directly analogous
   to the quality-agent bug just above**: traced
   `B_pkg.SetPackageInstallState(B_avail[i], false)`
   (`pim_core/pim_core_src/pimSilent.cxx:582,584`) into
   `pimPackageMgr::SetPackageInstallState()`
   (`pim_core/pim_core_src/pimPackageMgr.cxx:304-332`) — it silently
   **refuses** to disable a package, leaving its `install` attribute
   unchanged, whenever that package is `required="Y"` **or** already
   `installed="Y"` (except `"prime_converter"`); this function discards
   the return value at both call sites. **Confirmed by direct contrast
   with the GUI path, mirroring the quality-agent precedent exactly**:
   `uiCustomTree::RefreshPkg()` (`pim_ui/pim_ui_src/pimCustomDlg.cxx:1819-1904`)
   checks the identical `req || installed` condition and proactively
   disables the checkbox, with no equivalent precondition in the
   silent-install path. **New nuance**: even on a refused disable, 2
   hardcoded package names (`"prime_converter"`/`"creo_simulate"`) still
   have their global mode flags toggled to the attempted value regardless,
   diverging from the unchanged XML attribute. **Verified non-issue,
   confirmed wasteful**: this function's own `B_pkg.Refresh()` call
   (`:586`) computes `<CDSECTION>`/`<MSI>`/`<SFX>` install eligibility
   using platform/language values `pimTop.cxx`'s post-creation code
   immediately overwrites and recomputes via its own, separate `Refresh()`
   call afterward — entirely superseded, harmless. **Cross-referenced**:
   `GetAllPackageNames()` shares `GetInstallPackageNames()`'s already-
   confirmed unguarded null-attribute crash risk.
   **FIX VERIFIED (SUFFICIENT), found in a further, dedicated pass
   specifically verifying `pimSilentFixupShortcuts()`'s already-known
   array-index bug's proposed fix — not a new function, a deeper scrutiny
   of one already-documented, caller-side-cited remediation**: replacing
   `B_avail[i]` with `A_wants[i]` in the 4 `SetShortcut*State()` calls
   (`:457-460`) is confirmed sufficient to eliminate the wrong-shortcut
   risk with no residual gap — the enclosing
   `if (B_avail.Find(A_wants[i]) != -1)` guard already proves `A_wants[i]`
   exists in `Eb`'s document, so `FindShortcutNode(A_wants[i])` (called
   inside `SetShortcutState()`) is guaranteed to locate the same shortcut
   the 2 immediately-following field copies already correctly target, and
   `B_avail`'s snapshot (captured once, before either loop begins) stays
   valid since nothing in this function mutates `Eb`'s document structure.
   **NEW LIMITATION, generalized from the stale-Program-Menu-copy fix
   finding above**: that pass traced `SetShortcutState()` only as the
   primitive behind `SetShortcutProgramsMenuState()`. All 4 of
   `SetShortcutStartMenuState()`/`SetShortcutProgramsMenuState()`/
   `SetShortcutDesktopState()`/`SetShortcutQuicklaunchState()` delegate
   identically to this one shared primitive, and since all 4 corresponding
   child elements are independently optional per shortcut, it can
   legitimately return `false` for a **correctly identified** shortcut
   whenever that one location option was never offered by `Eb`'s
   template — none of the 4 array-index call sites check this return
   value, fix or no fix. **Verified non-issue, found while tracing this
   same call chain**: `GetShortcutInfoByID()`'s own discarded return value
   and its once-outside-the-loop output booleans are not reachable as a
   staleness bug — the id is sourced from the same document's own
   `GetAllShortcutIDs()` scan, guaranteeing the node is always found, and
   the function pre-initializes all 4 output booleans to `false` before
   scanning children, unlike its `GetShortcutProgramMenu()` sibling.
   **Adjacent, pre-existing risk, found in the same trace**:
   `GetAllShortcutIDs()` (`pim_ui/pim_ui_src/pimShortcutMgr.cxx:312-313`)
   dereferences its `id`-attribute node with no null check — a latent
   crash if any `<SHORTCUT>` node ever lacks an `id` attribute.
   See `ai-context/business_rules.yaml`'s
   `pimsilenttestxmlisuseable_leaks_pimxmlfile_on_syntax_error`,
   `pimsilentcanmatchneeds_leaks_2_pimxmlfiles_on_parse_error`,
   `pimsilentfixuppsf_dummy_placeholder_can_get_stuck`,
   `pimsilentfixuppsf_collision_deletes_entire_type_not_just_colliding_name`,
   `pimsilentcanmatchneeds_package_check_mode_dependent`,
   `pimsilentfixupshortcuts_stale_programmenu_copy`,
   `pimsilentcreateentitlement_package_selection_matches_canmatchneeds`,
   `pimsilentcreateentitlement_msi_node_last_value_wins`,
   `pimsilentcreateentitlement_property_skip_list_confirmed_correct`,
   `pimisversionmatch_shipcode_check_optional_not_enforced`,
   `pimsilentcanmatchneeds_platform_language_selection_diverges_from_validation`,
   `pimisproductxmlmatch_no_cross_schema_fallback`,
   `pimsilentfixuppsf_ea_eb_reference_semantics`,
   `pimsilentcreateentitlement_session_index_leaks_on_later_failure`,
   `pimsilentcanmatchneeds_package_check_crashes_on_null_attribute`,
   `pimsilentcreateentitlement_quality_agent_disable_silently_overridden`,
   `pimsilentcreateentitlement_package_disable_silently_overridden`, and
   `docs/classes/pimSilent.md`.

Full per-module detail: `docs/modules/*.md`, `ai-context/modules.yaml`.

## Business Rules an Agent Must Not Silently Reinterpret

These are documented in full in `ai-context/business_rules.yaml`; the ones most
likely to surprise an agent making a "reasonable-looking" change:

- **Prerequisite satisfaction is a bare string-matched dispatcher** with only 2
  rules actually wired up (`pimCreoTestPlatformAgent`, `pimWGMTestVFS`) despite
  several more check functions being declared. An unmatched function name in a
  product XML's `<IS_INSTALLED_FUNC>` silently means "never satisfied" — there is no
  error surfaced for a typo. Don't assume adding a new rule "just works" without also
  updating the exact XML string that must match it.
- **`IsOnlySoftPrerequisiteNeeded()`'s name is the opposite of a natural reading** —
  it reports whether there's an unsatisfied *hard* prerequisite, not a soft one.
  Read the implementation, not the name.
- **MSI exit codes 3010 and 1641 are SUCCESS**, not failure (reboot
  required/initiated). Codes 1602/1259/1618 are treated as user *cancellation*, not
  error. Don't "fix" these mappings without confirming against Windows Installer's
  own documented exit-code semantics first.
- **Any other non-zero MSI exit code permanently deselects the failed package**
  (`<PACKAGE install="N">`) — a naive retry after fixing the underlying cause will
  silently skip that package unless the flag is also reset.
- **`pimMSILoop` implements the exit-code table TWICE, independently** — once
  in `ProcessErrorCode(int, ...)` (used by the `msiexec.exe` execution
  strategy) and once in a separate, hand-duplicated `if`/`else if` chain
  inline inside the MSI-API execution strategy (`OnInstall()`). Changing one
  without the other desynchronizes how the two strategies report the same
  underlying failure.
- **`ROLLBACK_CANCELLED_INSTALL` is not `#define`d anywhere in this archive.**
  Multiple cancel-handling code paths in `pimEntitlement::OnInstall()` are guarded
  by this macro. Unless the real build defines it externally, canceling mid-install
  does **not** trigger automatic rollback in the current build — verify against the
  actual build configuration before assuming rollback-on-cancel behavior exists.
- **`ERROR_ON_UNSIGNED` is likewise never `#define`d anywhere in this archive.**
  `pimDownloadLoop::TestDigitialSignature()` checks a downloaded `.exe`/`.cab`
  against `pimIsDigitiallySignedByPTC()` but **unconditionally returns `true`**
  regardless of the outcome; the only failure-path logging is gated behind this
  undefined macro. Downloaded installer payloads are not actually blocked for
  failing signature verification in the traced build. Treat this as a real
  security-relevant finding, not a false alarm, before assuming signature
  enforcement is active anywhere in the download path.
- **SFX exit code 1618 is handled differently from the identical MSI exit code.**
  `pimMSILoop` treats 1618 as a cancellation; `pimSFXLoop` treats it as a hard
  failure, with the cancel call explicitly commented out and a code comment
  explaining why. This is confirmed intentional — do not "fix" it by unifying the
  two without product-level confirmation that they should match.
- **`pimMSICopier` and `pimSFXCopier` share one lock, not two.** Both call the
  same `pimOKToRunMsiexec()`/`pimFreeMsiexec()` functions, so an MSI install and
  an SFX install cannot run concurrently process-wide even though they are
  different singleton classes — don't assume "different class = independent
  concurrency domain" for this pair.
- **4 of the 7 singleton wrapper classes' `Cancel()` is flag-only.**
  `pimShortcuts`/`pimRegEdit`/`pimServices`/`pimDownloader`'s `Cancel()` does not
  stop the owned Loop — only `Kill()` does. Every confirmed caller compensates
  with a cancel-then-grace-period-then-`Kill()` idiom; a new caller that calls
  `Cancel()` alone and expects prompt termination would be wrong. `pimCopier`/
  `pimMSICopier`/`pimSFXCopier`'s `Cancel()` DOES propagate directly.
- **`pimServices`' own lock does not serialize whole operations** (see Critical
  Modules #3 above) — treat this as a live concurrency bug, not a documentation
  nuance.
- **`pimRegEdit.h`'s own comment about `Create()`/`Uninstall()` blocking is
  wrong.** It says they "WAIT until they are able to start... and then return
  immediately"; the implementation is a non-blocking `TryLock()`, and the
  confirmed caller busy-polls exactly like every other TryLock-gated wrapper.
  Trust the implementation, not this comment.
- **`pimRegEditLoop::OnRollback()` skips every other registry entry, forever.**
  A confirmed double-increment (the `for` loop's own `index++` plus an
  explicit `item(index++)` call) means odd-indexed `<REGISTRY>` nodes are
  never visited on any pass of the outer retry loop. Do not assume
  uninstall/rollback fully cleans up a product's registry footprint until
  this is fixed — see Critical Modules #2.
- **`pimMSILoop::OnInstall()`'s `fallback_to_msiexec` flag never resets,
  causing compounding re-installs.** A single `<MSI>` node with an
  unparseable install command triggers a full re-scan-and-install pass via
  `pimMSIExec()` after **every** remaining node in the same `OnInstall()`
  call — and because eligibility there doesn't check the "already
  installed" attribute, already-installed packages get silently
  reinstalled, repeatedly. Also, the `same_msihybrid` property's apparent
  skip-redundant-install intent is dead code — it has no actual effect. See
  Critical Modules #1.
- **`pimShortcutMgr::AreChangesPending()` cannot detect an added or removed
  shortcut** — only one that changed while present in both compared
  snapshots. Used in `pimEntitlement.cxx`'s update/reconfigure decision
  logic, so a pure shortcut addition/removal between product XML versions
  may not by itself trigger needed reinstall work. See Critical Modules #9.
- **`pimSilent.cxx`'s `pimSilentFixupShortcuts()` uses the wrong array's
  index** for 4 of its 8 shortcut-state-copy calls (`B_avail[i]` instead of
  `A_wants[i]`), applying a location-toggle state to the wrong shortcut ID
  whenever the two compared XML documents' `<SHORTCUT>` node orders differ.
  A confirmed bug in this caller, found while researching `pimShortcutMgr`'s
  usage — see `docs/classes/pimShortcutMgr.md`.
- **`pimInstallMgrDlg`'s `StepForward()`/`StepBack()` are two independently
  hand-written `if`/`else if` chains, not one shared step-order table** — a
  confirmed guard-condition asymmetry (a Customize-Apps skip guard in
  `StepForward()` with no mirrored guard in `StepBack()`) can land the wizard
  on a tab it never visited going forward. Any new wizard step must be added
  to BOTH functions by hand; there is no compiler- or data-checked link
  between them. See Critical Modules #4.
- **`pimInstallMgrDlg::UpdateParentEntitlements()`/`UpdateChildEntitlements()`
  are duplicated verbatim** (modulo one stray comment) in the nested
  `uiCustomAppTree` class in the same file. A fix to the parent/child
  language-platform propagation logic applied to only one copy silently
  desyncs the other. See Critical Modules #4.
- **`pimInstallMgrDlg::RefreshFeatureTab()` is a confirmed permanent no-op**,
  and `OnTabSelect()`'s Help/Advanced-tab branch is confirmed unreachable in
  the traced conditions. Don't "clean up" either as dead code without first
  confirming no external build flag or licensing state can still reach them.
- **`pimInstallMgrDlg::SetPortError()` uses the wrong message-catalog ID**
  for at least one license-server port-validation failure — an out-of-range
  port value produces a message that doesn't describe the actual failure.
  Note: the same code's disabling of EPORT-specific validation in
  `SetAndValidateLicenseServerPorts()` is confirmed INTENTIONAL, not a bug —
  don't conflate the two.
- **`pimCustomDlg` is CONFIRMED LIVE, not a dead/superseded parallel class —
  do not remove or deprioritize it.** It has a near-identical method-name
  set to `pimInstallMgrDlg`'s embedded Customize tab (`RefreshShortcutsTab`,
  `RefreshFeatureTab`, `UpdateParentEntitlements`, etc.), which an earlier
  pass wrongly took as evidence it was superseded legacy code; a full
  source read found 2 confirmed call sites instead. Treat the two classes
  as duplicate, independently-maintained implementations of the same
  feature, not as one live and one dead.
- **`pimCustomDlg::SetSABPath()`/`DisableSAB()`/`EnableSAB()` guard on the
  wrong process-wide static handle.** `pim_ui_src/pimSAB.cxx` has 2 separate
  static SAB handles, one per dialog class; these 3 `pimCustomDlg` methods
  check `pimInstallMgrDlg`'s handle instead of their own, currently masked
  only because `pimInstallMgrDlg::InitSAB()` always runs first in the
  traced call sites. Don't assume this is safe in a code path where that
  ordering isn't guaranteed.
- **`pimEntitlementTree`'s per-row "customize" icon doesn't pre-select that
  row's product** — it resolves the specific `pimEntitlement` for the
  clicked row but never passes it to `pimCustomDlg`, whose `Initialize()`
  has no parameter for a target entitlement. The dialog always opens on the
  first eligible product; the user must manually reselect the intended one
  from its own picker list. A UX gap, not a data-correctness bug.
- **`pimEntitlementTree::OnOptionMenuSelect()` can crash on a null pointer.**
  `eptr_new_xml->SetXml(E->GetTag())` runs unconditionally right after an
  `if (eptr_new_xml)` guard that covers only the 2 lines before it — if
  `pimGetMediaDetails::GetProductXml()` returns `NULL` for the selected
  media (a real, reachable media-lookup failure, not hypothetical), this
  dereferences a null pointer. Move the fix inside the existing guard rather
  than assuming the pointer is always valid by the time this line runs.
- **`pimEntitlementTree::ReconfigureDisplay_low()`'s single-entitlement path
  computes the wrong family-group node id.** It strips 5 characters from
  `"_" + use_type` to remove the 4-character `"_app"` suffix, producing
  `"_Cre"` instead of `"_Creo"` (or the equivalent for any family) — an
  off-by-one confirmed by contrast with the correct sibling computation in
  `UpdateGroupStatus()`. Triggers whenever a reconfigure session has exactly
  1 entitlement; the family-group checkbox silently fails to update.
- **`pimFrictionlessTrialDlg::Display()` never processes the license-retrieval
  worker's completion for its only confirmed real usage.** Its
  in-house-mode branch calls `Sleep(60000)` instead of `uiDialog::Activate()`,
  so the armed `LicenseGenerateTimer` never fires, so `OnTimerExpired()` — the
  ONLY call site anywhere in this codebase for
  `pimFrictionlessTrialLicenseGet::HeartBeat()` — is never invoked.
  `HeartBeat()` is where the license file is actually saved to disk and the
  PSF updated; none of that runs. Do not assume this class's confirmed real
  entry point (`pimFrictionlessTrialRun()`) actually persists a working trial
  license without independently verifying it.
- **`pimFrictionlessTrialDlg::SetURL()`/`GetAuth()`/`Refresh()` have zero
  confirmed callers**, despite matching the signatures of actively-used
  methods on sibling classes `pimAuthDlg`/`rpimDlg`. Don't assume a method
  is load-bearing just because a same-named/same-signature sibling method is.
- **`rpimDlg::Display()` is the CORRECT reference pattern for
  `pimFrictionlessTrialDlg`'s `Sleep(60000)` bug** — it calls
  `uiDialog::Activate()` unconditionally, with no mode-dependent branch. Use
  it as the fix template; do not assume the shared `LicenseGenerateTimer`/
  `OnTimerExpired()` polling design is inherently broken.
- **`rpimDlg::SetURL()`/`GetAuth()` have zero confirmed callers, but
  `rpimDlg::Refresh()` IS actively used** (unlike `pimFrictionlessTrialDlg`'s
  fully-dead `Refresh()`) — don't over-generalize "this class is a partial
  copy of `pimAuthDlg`" into "every method on it is vestigial." Verify each
  method independently.
- **`pimAuthDlg::Initialize()` has no idempotency guard, and this is the ONE
  class in its 3-class dialog family confirmed to actually be reused** — its
  `AuthDialog` singleton is re-`Initialize()`d across multiple authentication
  challenges within one process (confirmed via `pimSessionInfo::TryAuthorize()`'s
  retry loop). Don't assume this is safe without confirming
  `uiDialog::Initialize()`'s own re-entry behavior, or add the same
  `if (initialized) return true;` guard already present on `rpimDlg`/
  `pimFrictionlessTrialDlg`.
- **`pimAuthDlg::SetURL()` IS called (unlike its 2 dead copies), but the
  `URL` member it sets is STILL never read — even in this original 2011
  template.** This refines the earlier `rpimDlg`/`pimFrictionlessTrialDlg`
  findings: it's the `SetURL()` *call site* that disappeared in the later
  copies, not a previously-useful value that became useless through
  copying. `RetryMsg`, by contrast, is genuinely wired end-to-end only in
  `pimAuthDlg`.

## Safe Modification Practices

1. **Read the relevant `docs/classes/*.md` or `docs/modules/*.md` entry in full
   before editing** — each documents dependencies, thread-safety notes, and known
   risks specific to that file. `ai-context/change_impact.yaml` tells you which
   classes have that level of detail already (`full_detail: true`) and which don't.
2. **For anything not yet at `full_detail: true`** (the rest of `pim_ui`,
   `pimInterrogator`, the package/platform/language managers): do a fresh,
   targeted read of the actual `.cxx` before relying on this documentation
   set's characterization of it — those areas were characterized at module
   level (or lighter) evidence. **All 10 Loop subclasses (the original 9 —
   including `pimRegEditLoop` and `pimMSILoop` — plus `pimGetAvailable`, a
   10th confirmed `pimLoop` subclass discovered late in this documentation
   effort and previously uncounted in every "9 Loop subclasses" claim in this
   set) AND all 7 owner-wrapper singleton classes now have full-depth
   `docs/classes/*.md` entries**, each backed by a full header+`.cxx` source
   read — this is now the complete set of core `pim_core` install-step
   machinery, with zero remaining `light_detail_classes` entries in
   `ai-context/change_impact.yaml`. **1 known exception to "complete"**:
   a further, dedicated pass on `pimSilentCreateEntitlement()`'s
   `<PROPERTY>` skip list (item 21 above) discovered an **11th** `pimLoop`
   subclass, `pimCustomActionsLoop`
   (`pim_core/includes/pimCustomActions.h` +
   `pim_core_src/pimCustomActions.cxx`), consumed by
   `pimEntitlement::OnInstallCustomActions()`/`OnUninstallCustomActions()`
   across the install/uninstall lifecycle — cited in
   `docs/classes/pimSilent.md` only via the specific traced methods
   relevant to that finding, per this set's Coverage Honesty Statement; it
   does not yet have its own `full_detail: true` entry.
   **`pimShortcutMgr`, `pimInstallMgrDlg`, `pimCustomDlg`,
   `pimEntitlementTree`, `pimFrictionlessTrialDlg`, `rpimDlg`, and
   `pimAuthDlg` are the first 7 `pim_ui`-module classes added at the same
   depth** — the rest of `pim_ui` (including
   `pim_ui_src/pimEntitlementRefresh.cxx`, a `pimInstallMgrDlg` method-body
   file discovered while tracing `pimEntitlementTree`'s callers but not
   itself traced; `pimFrictionlessTrialLicenseGet`, read in full to confirm
   `pimFrictionlessTrialDlg`'s headline finding but not given its own class
   doc; and `pimPTCRenewLicenseGet`, located while documenting `rpimDlg` but
   not read in depth) remains at lighter or no evidence. The ~550-line
   `OnPushButtonActivate()` inside `pimInstallMgrDlg` itself, previously
   flagged here as not traced, was fully traced in a further, dedicated
   pass — see `docs/classes/pimInstallMgrDlg.md`'s Risk Analysis and
   `ai-context/business_rules.yaml`'s `piminstallmgrdlg_eula_decline_order_bug`.
3. **Follow existing patterns rather than inventing new ones**:
   - New install step → subclass `pimLoop`. If it needs process-wide serialization
     against concurrent operations of the same kind, follow the singleton-owner
     pattern used by 7 of the 9 existing Loop types (`pimCopier`, `pimMSICopier`,
     `pimSFXCopier`, `pimShortcuts`, `pimRegEdit`, `pimServices`, `pimDownloader` —
     all `GetInstance()` singletons, **not** per-entitlement objects). If it should
     run concurrently across entitlements with no shared-resource contention,
     follow the transient-instantiation pattern instead
     (`pimScriptLoop`/`pimPsfLoop` use this; document why if you add a third).
   - New prerequisite rule → add a check function *and* a dispatcher branch in
     `pimPrerequisite.cxx`, and grep for the exact string across any product XML
     that will reference it. There is no compiler-checked link between the two.
   - New session-wide property → `pimSessionInfo::SetProperty`/`GetProperty` needs
     no schema change, but that flexibility is also a discoverability liability —
     consider whether a new `*_PROPERTY` `#define` (documented, grep-able) is better
     than an ad hoc string literal.
4. **All 7 Loop-owner wrapper classes are singletons — none of them are
   per-entitlement — but they are NOT uniform in how they enforce that.** This
   session's own documentation originally got the singleton fact wrong for all 7,
   then partially corrected it for just `pimRegEdit` (Phase 7), then confirmed the
   same pattern holds for the other 6 (Phase 5 extension), then — on a full
   source read of all 7 — found the enforcement mechanism itself differs
   (`pimServices`' lock doesn't actually serialize; `pimMSICopier`/`pimSFXCopier`
   share one external gate instead of each having its own). Don't repeat any of
   these mistakes: don't assume per-entitlement ownership from a class merely
   being referenced inside `pimEntitlement`; don't assume a correction found for
   one class generalizes uniformly to its siblings without checking each one's
   own `.cxx` body; and don't assume "singleton" implies identical locking
   behavior across a family of superficially-similar classes.
5. **Check both the function return value and `pimGetLastError()`** when working
   with error paths — they are not guaranteed to agree for the same failure (a
   confirmed real inconsistency exists: `docs/10_error_handling.md` §2.2).
6. **Keep the four error/status channels in mind** when adding new failure handling
   (`docs/10_error_handling.md` §1): per-Loop `Errors`/`Warnings` text, the
   process-wide `pimHitError` code stack, the persistent XML status property, and
   the `CA_WARNINGS[_WITH_ERROR]` escalation flags. Adding error reporting in only
   one channel will look incomplete to whichever consumer reads a different one.

## High-Risk Areas (do not touch without extra review)

Ranked by confirmed evidence of risk, not just theoretical concern:

1. **`pimMSILoop::OnInstall()`'s `fallback_to_msiexec` never-reset flag** — a
   confirmed, compounding bug that can silently re-install already-installed
   MSI packages, up to O(N²) redundant `msiexec.exe` launches in the worst
   case. The single highest-severity finding in this entire documentation
   effort: it produces real, unwanted side effects (re-running installers)
   rather than merely omitting work, and — like the `pimRegEditLoop` bug
   below — is not latent behind an unexercised concurrency scenario; it fires
   whenever the MSI-API path is active and a single legacy `<MSI>` node lacks
   a recognizable command-line pattern.
2. **`pimRegEditLoop::OnRollback()`'s double-increment bug** — a confirmed,
   deterministic, silent skip of every other `<REGISTRY>` entry on
   uninstall/rollback. Fires on every normal rollback that has 2 or more
   registry entries.
3. **`pimServices`' `Create_low()` releases its lock before the operation
   finishes** — a confirmed, live use-after-free-class race condition, currently
   unexercised only because no caller invokes it concurrently.
4. `pimXmlFile`'s locking protocol — **confirmed real bug history**.
5. `pimEntitlement.cxx` as a whole — 7,649 lines, highest centrality, `friend`
   classes bypassing its own API.
6. `pimEntitlement::OnReconfigure()`'s cache-file-deletion bug — a confirmed
   HIGH-SEVERITY data-loss risk: the function's first block unconditionally
   `Erase()`s the entitlement's own cached XML file
   (`pimGetAppData/pim/<name>.xml`) purely as a side effect of computing an
   unrelated path (the original as-shipped `.p.xml`, used for
   `RollbackMe`) — there is no functional need to delete the file just to
   read its own filename. 2 confirmed early-return paths (a service-stop
   failure, or a `PreReconfigure` custom-action failure) leave the deleted
   cache file permanently un-recreated, since neither calls
   `xmlPtr->DoSave()` first. Reachable in the realistic common case, since
   `OnReconfigure()` only runs against an already-installed, already-cached
   entitlement. Confirmed by direct contrast with `OnRollback()`'s own
   mirror-image cache-relocation block, which has no such `Erase()` side
   effect. See `ai-context/business_rules.yaml`'s
   `pimentitlement_onreconfigure_erases_cache_file_as_path_side_effect` and
   `docs/classes/pimEntitlement.md`'s Risk Analysis.
7. `pimInstallMgrDlg`'s wizard step machine and customize-tab duplication —
   the `StepForward()`/`StepBack()` guard asymmetry and the
   `UpdateParentEntitlements()`/`UpdateChildEntitlements()`/`uiCustomAppTree`
   duplication (see Critical Modules #4) are confirmed, live, and affect the
   entire interactive install experience's navigation and entitlement
   propagation correctness — not merely one narrow feature.
8. `pimInstallMgrDlg::OnPushButtonActivate()`'s EULA-decline-order bug — a
   confirmed HIGH-SEVERITY, deterministic ordering defect on the wizard's
   own standard EULA screen: `EulaNextPreAction()` (kicks off license
   acquisition) is called BEFORE checking whether the user declined the
   license, the reverse of the Beta EULA screen's correct order in the
   SAME function. Reachable by any user who declines the standard license
   agreement — a common, ordinary wizard interaction, not an edge case.
   Also carries a confirmed control-flow-traced `Refresh()`-after-`Destroy()`
   gap, a disabled-Finish-button bug, and a confirmed re-entrancy bug via
   `pimTextDlg`'s "proceed anyway" flow. See
   `ai-context/business_rules.yaml`'s `piminstallmgrdlg_eula_decline_order_bug`.
9. Any of the 7 singleton owner-wrapper classes' busy-retry/status polling
   logic — session-wide concurrency implications for every concurrently-installing
   entitlement, not just the one making the change. Remember `pimMSICopier`/
   `pimSFXCopier` share one gate (not independent), and 4 of the 7 have a
   flag-only `Cancel()` compensated for at the call site — see Business Rules.
10. `pimDownloadLoop::TestDigitialSignature` — currently a confirmed no-op for
   enforcement purposes; changing it touches supply-chain-integrity behavior for
   every downloaded installer payload.
11. Any `#if`/`#ifdef`-guarded behavior (`ROLLBACK_CANCELLED_INSTALL` and
   `ERROR_ON_UNSIGNED` are the two confirmed examples, both undefined anywhere in
   this archive; `pimRegEditLoop`'s `IsOKToDelete()` allow-list is a third,
   `#if 0`-disabled example; `pimCustomDlg::Initialize()`'s Windows-7
   taskbar-pin-checkbox block is a fourth) — verify the actual build's
   compiler defines before assuming a code path is live or dead. Also note
   `pimMSILoop::Kill()`'s no-sleep busy-spin (bounded, but a confirmed
   inefficiency) and its total no-op behavior for the MSI-API execution path.
12. `pimShortcutMgr::AreChangesPending()`'s add/remove coverage gap and the
   confirmed `pimSilent.cxx` array-indexing bug it led to finding — both
   affect shortcut-related correctness during updates/reconfigures and
   silent installs, but are narrower in scope (one feature area) than the
   items above.
13. `pimCustomDlg`'s wrong-static-handle SAB bug and its duplication with
   `pimInstallMgrDlg`'s Customize tab-set (see Critical Modules #11) —
   currently masked by call-order luck, not truly safe; and remember this
   class is CONFIRMED LIVE, not dead code, despite an earlier pass's
   speculation to the contrary.
14. `pimEntitlementTree::OnOptionMenuSelect()`'s null-pointer-dereference
   crash risk (see Critical Modules #12) — a real, reachable failure mode
   (a media lookup returning `NULL` for a specific SKU) in the Applications
   step's version/shipcode dropdown, not a narrow edge case; confirmed root
   cause (see Critical Modules #17): `pimGetMediaDetails::GetProductXml()`
   returns `NULL` on no URL match or 2 consecutive timeouts; and its sibling
   `ReconfigureDisplay_low()` off-by-one, which silently breaks family-group
   checkbox state in any single-entitlement reconfigure session.
15. `pimFrictionlessTrialDlg::Display()`'s never-runs-`Activate()` bug (see
   Critical Modules #13) — the single highest-severity confirmed finding
   among the `pim_ui` classes documented so far: for this class's only
   confirmed real usage, the entire license-retrieval completion/save/PSF-
   update pipeline never executes, not merely a subset of it. Narrower in
   scope than items #1-#2 above (it affects only the Frictionless/Commercial
   Trial flow, not the main install wizard), but total within that scope.
16. `pimAuthDlg::Initialize()`'s missing idempotency guard on a confirmed-
   reused singleton (see Critical Modules #15) — reachable via a real
   `pim_core` retry loop (`pimSessionInfo::TryAuthorize()`), and affects a
   generic credential-prompt mechanism used across multiple PTC.com network
   operations, not one narrow feature. Severity ceiling not fully
   determined (depends on `uiDialog::Initialize()`'s own re-entry
   behavior, not in this archive).
17. `pimInstallMgrDlg::EntitlementRefresh()`'s School-/Beta-mode trial-license
   wait loops (see Critical Modules #4) — a confirmed infinite loop (not
   merely a bug, a genuine hang) reachable whenever a School- or Beta-mode
   trial license fails to parse: a `||`/`>=` guard where the correct sibling
   Trial-mode loop uses `&&`/`<=` means the retry counter never advances
   while the failure persists, so the wizard freezes with no timeout and no
   error path. Found by the same 3-way cross-comparison technique used
   throughout this effort (one correct block, two independently-mutated
   copies of it in the same function).
18. `pimSessionInfo::TryAuthorize()`'s infinite loop on transport failure —
   a confirmed HIGH-SEVERITY hang closely analogous to item #16 above: no
   branch at all handles `pimAuthorizeToPTC()` returning `false` (a
   network/transport failure, as opposed to a server credential rejection),
   so the `do`/`while` credential-retry loop simply re-prompts for
   credentials forever, confirmed by direct contrast with the sibling
   `pimGetAvailable::GetSecurity()`'s correct `-3` abort on the identical
   failure. Also carries a matching `pimXmlFile` leak on every iteration
   where the server rejects (not merely fails to reach) the credentials,
   and a confirmed credential-store aliasing between the media URL and the
   PTC.com production URL. See `ai-context/business_rules.yaml`'s
   `pimsessioninfo_tryauthorize_infinite_loop_on_transport_failure`.
19. `pimGetAvailable::OnExecute()`'s 2 confirmed memory leaks (see Critical
   Modules #16) — `ImageXml` never freed on the success path,
   `ProductDefinitionXml` never freed on 2 filtered-out paths. Lower severity
   than items above (bounded by one search's lifetime in a short-lived
   installer process, not a hang or data-corruption risk), but confirmed and
   compounding with the number of available products/media entries a given
   web-media search encounters.
20. `pimGetMediaDetails::GetDetails()`'s unlocked cache-hit read (see
   Critical Modules #17) — a genuine cross-thread race, not theoretical:
   this singleton is confirmed called from both the UI thread and a
   background `pimLoop` thread. Ranked above the `pimGetAvailable` leaks
   above because a data race in a shared cache has a less predictable
   failure mode (potential corruption/crash from any concurrent caller, not
   just the searching thread) than a bounded per-search memory leak.
21. `InstallPreReqSilent()`'s wrong-array-size loop bound (see Critical
   Modules #18) — ranked above the cache-race and memory-leak findings above
   because the failure mode is not merely a leak or a rare race but a
   **silently skipped feature**: whenever a silent install doesn't pass an
   application-limiting flag (the common case), this function's entire
   prerequisite-installation loop never executes at all, with no error, no
   log warning distinguishing this from "no prerequisites needed," and no
   confirmed caller checking for it. A scripted/enterprise silent-install
   pipeline could run for a long time without anyone noticing prerequisites
   were never actually being installed.
22. `pimAvailableProduct::AddInstance()`'s ignored `shipcode` parameter (see
   Critical Modules #19) — ranked below the items above since its effect is
   narrower (one specific shipcode/version collision silently dropping one
   media ID from a dropdown, not a whole feature or a data race) and lower
   probability (requires the same version to appear under 2 different
   shipcodes in one search), but still a confirmed, silent data-loss bug
   with no error path.
23. `pimTranslateMgr::TranslateFrom()`'s mismatched-guard bug — a confirmed,
   silently-skipped-UI-translation bug: the guard that decides whether to
   compute the current product's filename checks one object
   (`ptr->getFilePath()`, the translation document) but reads the value
   from a different one (`xmlProduct->getFilePath()`, the product
   document), reachable via 2 of `pimEntitlementTree::OnOptionMenuSelect()`'s
   3 near-identical call sites (a MediaID already cached earlier in the
   same session). Ranked below the items above since the consequence is
   purely cosmetic (a UI label stays in its original language instead of
   being translated, no crash, no data loss, no install-behavior change).
   See `ai-context/business_rules.yaml`'s
   `pimtranslatemgr_translatefrom_mismatched_guard_wrong_object_path` and
   `docs/classes/pimTranslateMgr.md`.
24. `pim_ui` broadly (`pimShortcutMgr`, `pimInstallMgrDlg`, `pimCustomDlg`,
   `pimEntitlementTree`, `pimFrictionlessTrialDlg`, `rpimDlg`, and
   `pimAuthDlg` excepted, now full class-level detail) — this documentation
   set gave the rest of it a lighter pass than the other 3 modules,
   including `pimFrictionlessTrialLicenseGet` (read in full but not given its own
   class doc) and `pimPTCRenewLicenseGet` (located while documenting `rpimDlg`
   but not read in depth); treat their actual behavior as less
   certain than what's written about `pim`/`pim_core`/`pim_util`.
   `pimInstallMgrDlg`'s own `OnPushButtonActivate()`, previously untraced, was
   fully traced in a further pass — see item #7 above.

## Testing Requirements

**Not established from source** — no test files, test framework references, or CI
configuration exist anywhere in the analyzed archive (`docs/01_repository_inventory.md`).
Any testing discipline for PIM is external to this codebase snapshot. Before merging
a change, at minimum exercise the specific playbook(s) in
`docs/11_troubleshooting.md` relevant to what you touched, plus the `test_areas`
listed for the affected class(es) in `ai-context/change_impact.yaml`.

## How a Future AI Agent Should Approach a Code Change Here

1. **Locate the affected class/module in `ai-context/change_impact.yaml` first.**
   Its `risk_level`, `depends_on`, `called_by`, and `test_areas` fields tell you the
   blast radius before you write any code.
2. **Read the cited `docs/*.md` file(s) in full**, not just the YAML summary — the
   YAML is a distillation; the narrative docs carry the reasoning and the specific
   line/file citations that justify each claim.
3. **If the target class/file is marked `full_detail: false` or isn't mentioned at
   all**, treat this documentation set as silent on it, not as having ruled anything
   out — do a fresh read before proceeding, the same way this analysis did for the
   classes it did cover.
4. **Cross-check any "obvious-looking" fix against the business rules section
   above** — several mechanisms in this codebase (MSI exit codes, the prerequisite
   dispatcher, the `ROLLBACK_CANCELLED_INSTALL` macro) look like bugs at first
   glance but are either intentional or of genuinely unconfirmed status; verify
   before "fixing."
5. **When you discover something this documentation set got wrong or missed**,
   correct it explicitly and note the correction (the way `docs/04_installation_flow.md`
   §7 and, later, `docs/02_architecture_overview.md` §5 correct the ownership-model
   assumption for `pimRegEdit`, then for all 7 singleton wrapper classes, then —
   on a full source read of all 7 — for the non-uniform locking/cancellation
   behavior within that same family) rather than silently overwriting — this
   keeps the documentation trustworthy for the next agent or human who reads it.
   Each of these corrections only happened because a later pass independently
   re-verified sibling classes or re-read a `.cxx` body in full instead of
   assuming the prior pass's characterization (header-only, or "this pattern
   generalizes") was the complete picture — a pattern worth repeating whenever
   you find one instance of something surprising in a family of similar-looking
   classes. The `pimRegEditLoop` double-increment bug and the `pimMSILoop`
   `fallback_to_msiexec` bug are both instances of this same discipline
   paying off: each was found only because this pass did a full, careful
   line-by-line read of the method in question — comparing
   `pimRegEditLoop::OnRollback()` directly against `OnInstall()`'s equivalent
   loop in the same file, and tracing exactly where and how often
   `pimMSILoop::OnInstall()`'s `fallback_to_msiexec` flag is read and set —
   rather than assuming a loop was correct because it looked structurally
   ordinary at a glance, or that a rarely-taken fallback branch was safe
   because it was clearly labeled as a fallback.
6. **Never present an inference as a fact.** This entire documentation set follows
   the discipline of marking anything not directly confirmed by a read/write call
   site as `UNKNOWN`, `inferred`, or `not individually re-traced this pass`. Preserve
   that discipline in anything you add.

## Document Index

| Concern | File |
|---|---|
| Repository inventory / corpus scope | `docs/01_repository_inventory.md` |
| Architecture | `docs/02_architecture_overview.md` |
| Startup trace | `docs/03_application_startup.md` |
| Install flow | `docs/04_installation_flow.md` |
| Prerequisites | `docs/05_prerequisite_framework.md` |
| Entitlement lifecycle/state | `docs/06_entitlement_framework.md` |
| Registry usage | `docs/07_registry_usage.md` |
| XML configuration | `docs/08_xml_configuration.md` |
| Logging | `docs/09_logging_framework.md` |
| Error handling | `docs/10_error_handling.md` |
| Troubleshooting playbooks | `docs/11_troubleshooting.md` |
| Developer onboarding | `docs/12_developer_onboarding.md` |
| Module docs | `docs/modules/{pim,pim_core,pim_ui,pim_util}.md` |
| Class docs (ALL 10 Loop subclasses + all 7 owner-wrapper singletons + `pimGetMediaDetails` + `pimAvailableProduct` + `pimGetApplicationsList` (not a class, a free-function group) + `Cmp_cStrings` (not documentable at full depth -- no implementation in this archive) + `pimSilentTestXmlIsUseable` (not a class, a 7-function free-function group) + `pimTranslateMgr` + 4 other core classes + 7 pim_ui classes — 35 full docs) | `docs/classes/{pimLoop,pimEntitlement,pimSessionInfo,pimXmlFile,pimCopyLoop,pimSFXLoop,pimScriptLoop,pimShortcutLoop,pimServiceLoop,pimPsfLoop,pimDownloadLoop,pimRegEditLoop,pimMSILoop,pimGetAvailable,pimGetMediaDetails,pimAvailableProduct,pimGetApplicationsList,Cmp_cStrings,pimSilent,pimTranslateMgr,pimCopier,pimMSICopier,pimSFXCopier,pimShortcuts,pimRegEdit,pimServices,pimDownloader,pimShortcutMgr,pimInstallMgrDlg,pimCustomDlg,pimEntitlementTree,pimFrictionlessTrialDlg,rpimDlg,pimAuthDlg}.md` |
| Doxygen patches (unapplied) | `generated/doxygen/*.patch` + `README.md` |
| Machine-readable context | `ai-context/*.yaml` (architecture, modules, classes, flows, business_rules, dependencies, ownership, registry, xml_schema, runtime_states, knowledge_graph, change_impact) |

---
*Phase 19 of the requested 20-phase documentation set. No source was modified.*
