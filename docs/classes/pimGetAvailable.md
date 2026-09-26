# Class: `pimGetAvailable`

**File:** `pim_core/includes/pimGetAvailable.h` (66 lines, full read) / `pim_core/pim_core_src/pimGetAvailable.cxx` (649 lines, full read)
**Companion:** `pim_core/includes/pimGetAvailableProduct.h` (74 lines, full read) / `pim_core/pim_core_src/pimGetAvailableProduct.cxx` (163 lines, full read) — `pimAvailableProduct`/`pimGetAvailableProducts`, the small value-object/collection pair `pimGetAvailable` uses internally to accumulate results. **Now has its own full-depth doc, `docs/classes/pimAvailableProduct.md`** (promoted from a brief companion mention in a later pass), which surfaced a confirmed bug in `AddInstance()` that this doc's original companion treatment missed — see that doc's Risk Analysis.
**Module:** `pim_core`
**Inherits:** `pimLoop`

> **Correction (found while documenting this class)**: every prior deliverable in
> this documentation set that claims "all 9 Loop subclasses, complete" (the top-level
> `README.md`, `ai-context/ai_readme.md`, `ai-context/change_impact.yaml`,
> `ai-context/knowledge_graph.yaml`, `generated/doxygen/README.md`, and the
> `pimMSILoop.md`/`pimRegEditLoop.md`/`pimShortcutLoop.md` docs themselves) undercounts
> the family by one. `pimGetAvailable` (`class pimGetAvailable : public pimLoop`,
> `pimGetAvailable.h:21`) is a 10th confirmed `pimLoop` subclass — a background-thread
> web-download-availability search, previously only mentioned in passing in
> `docs/modules/pim_core.md`'s dependency list and never given a full class doc, a
> Doxygen patch, or counted in any "N Loop subclasses" tally anywhere in this set.

## Purpose

Runs, on a background thread (via inherited `pimLoop`/`thrThread` machinery), the
web-based "what products/versions are available to download" search used by
web-media (`MEDIA_PROPERTY == "N"`) installs: it authenticates to PTC's servers,
walks a media-availability XML feed, retrieves each candidate product's own
`image.xml`/product-definition XML over HTTP, and adds every valid, in-scope
product as an entitlement on the current `pimSessionInfo`.

## Responsibilities

- Resolve PTC.com credentials for both the general Production URL and the Media
  Production URL (`GetSecurity(bool retry)`), prompting the user via
  `pimNonBrowserAuthenticationCB()` (the same `pimAuthDlg`-backed callback
  documented in `docs/classes/pimAuthDlg.md`) only if no cached security is found.
- Perform a cheap, synchronous PTC authorization pre-check
  (`SearchForAvailableDownloads()`, via `pimAuthorizeToPTC()`) before committing to
  the expensive background search — distinguishing "bad account/invalid auth ID"
  (retryable) from "abort" (unrecoverable/cancelled) from "authorized, proceed."
- On the background thread (`OnExecute()`): fetch the top-level media-availability
  feed, parse each candidate's `image.xml`, filter by product mode
  (Mathcad-only vs. Creo), major-version compatibility
  (`pimCmpDottedVersions(..., CURR_MAJOR_VERSION)`), and an app allow-list
  (`pimGetApplicationsList()`); for each surviving entitlement, retrieve its full
  product-definition XML, filter out `<HIDDEN_PRODUCT>` entries, and call
  `SessionInfo->AddEntitlement()` to register it — including a translation pass
  (`pimTranslateMgr`) when the UI language differs from the OS language, and
  Trial/School/Mathcad-mode-specific `SetInstallMe()`/`SetQualityAgent()` defaults.
- Accumulate discovered products into `AvailableProductsArray`
  (`pimGetAvailableProducts`, a `btkMap`-backed collection of `pimAvailableProduct`,
  each holding a product's known version/shipcode/media-ID triples, keyed and
  sortable) and a separate flat family-group name list (`family_grp`/`family_count`)
  parsed from the same `image.xml`'s `<FAMILY>` elements.
- Track authorization outcome (`auth_status`) and overall search completion
  (`IsSearchDone()`, wrapping the inherited `IsDone()` with a `SessionInfo->Save()`
  side effect) for polling consumers.

## Dependencies

- `pimLoop` (base class — threaded `Execute`/`Cancel`/`IsDone` contract).
- `pimSessionInfo` (`SessionInfo` member — both the source of cached security and
  the destination for `AddEntitlement()`).
- `pimGetAvailableProducts`/`pimAvailableProduct` (`pim_core/includes/pimGetAvailableProduct.h`)
  — the accumulator this class populates and exposes read-only slices of
  (`GetShipcodeOptions`/`GetAssociatedMediaIDs`).
- `pimGetMediaDetails` (singleton, `GetInstance()`) — supplies per-media-ID
  `<image.xml>`-adjacent "media details" XML used to resolve the actual
  product-definition URL to fetch.
- `pimTranslateMgr` — applies a UI-language translation pass to a freshly
  retrieved product-definition XML when the OS language differs from the UI
  language, fully traced in a further, dedicated pass — see
  `docs/classes/pimTranslateMgr.md`. This call site (`:582`) is confirmed
  UNAFFECTED by that pass's headline mismatched-guard bug, since
  `ProductDefinitionXml->SetXml(Prod.GetXmlName())` is called moments earlier
  (`:559`), unlike 2 of the 3 analogous call sites in
  `pimEntitlementTree::OnOptionMenuSelect()`.
- `pimAuthorizeToPTC`/`pimEntitlementRequestToPTC`/`pimEntitlementAuthFailed`/
  `pimSendNRecvFromPTC` (external PTC.com HTTP/auth transport functions, declared
  elsewhere in `pim_core`, not traced to their own implementations in this pass).
- `pimNonBrowserAuthenticationCB()` (declared via a local `extern` forward
  declaration at the top of `pimGetAvailable.cxx:48`, defined in
  `pim_ui/pim_ui_src/pimAuthDlg.cxx` — see `docs/classes/pimAuthDlg.md`, which
  already documents this as one of `pimAuthDlg`'s 2 further confirmed call paths
  beyond `pimSessionInfo::TryAuthorize()`'s retry loop).
- `pim_core/pim_core_src/pimLocate.cxx`'s free function `LocateAllowPTCDotCom()`
  — see Ownership Model for the cross-module raw-pointer alias this creates.

## Members

| Member | Type | Purpose |
|---|---|---|
| `SessionInfo` | `pimSessionInfo*` | Not owned; the session this search reads credentials from and writes discovered entitlements into. |
| `AvailableProductsArray` | `pimGetAvailableProducts` | Owned by value; the accumulated, sorted product/version/media-ID results. |
| `auth_status` | `int` | `-1` initial/unknown, `0` = authentication failed (bad account / invalid auth ID), `1` = authorized and search completed successfully, `-3` = an internal "should never happen" security-lookup failure inside `OnExecute()`. **The header's own inline comment, `// -3 abort, 0 success`, is confirmed wrong** — see Risk Analysis. |
| `family_grp` | `StringXArray` | Flat list of `<FAMILY>` group names parsed from the last-processed `image.xml`. |
| `family_count` | `int` | `family_grp`'s size, cached redundantly (see Risk Analysis). |

## Public APIs

| Method | Purpose |
|---|---|
| `pimGetAvailable(pimSessionInfo &S)` | Ctor; binds `SessionInfo`, sets `auth_status = -1`; passes `S.GetXMLPtr()` up to `pimLoop`'s constructor. |
| `~pimGetAvailable()` | Empty — no explicit cleanup (see Risk Analysis for what this means for `AvailableProductsArray`'s own contents). |
| `int SearchForAvailableDownloads(bool retry)` | The main entry point, called from the UI thread. Calls `GetSecurity(retry)`, then a synchronous `pimAuthorizeToPTC()` pre-check; returns `1` (true) and starts the background thread (`Execute()`) on success, `0` (false) if the account/auth-ID check fails (retryable — see Called By for the caller's retry loop), or `-3` on any other failure/abort path (including `GetSecurity()` itself failing). |
| `int IsAuthorized()` | Mutex-protected read of `auth_status`. **Confirmed zero callers anywhere in this archive** — see Risk Analysis. |
| `bool IsSearchDone()` | Wraps `IsDone()`; additionally calls `SessionInfo->Save()` the first time it observes completion. |
| `int GetSize()` | `AvailableProductsArray.GetSize()` — number of unique discovered product names. |
| `const StringXArray &GetShipcodeOptions(const char *XmlName)` | Forwards to `AvailableProductsArray`; the version/shipcode options discovered for one product XML. |
| `const StringXArray &GetAssociatedMediaIDs(const char *XmlName)` | Forwards to `AvailableProductsArray`; the media IDs discovered for one product XML. |
| `StringXArray &GetFamilyList()` | Inline accessor, returns `family_grp` by non-const reference. |
| `int GetFamilySize()` | Inline accessor, returns `family_count`. |

## Protected APIs

| Method | Purpose |
|---|---|
| `void OnExecute()` | Thread entry point — the full web search/parse/filter/register pipeline described in Responsibilities. |
| `void OnTerminate()` | Standard thread teardown: releases the per-thread logger, sets `mDone = true` under `Mutex`, calls the inherited `ClearInProgress()`. |
| `bool GetSecurity(bool retry)` | Resolves or prompts for PTC.com credentials — see Responsibilities and the `GetSecurity()` design note in Risk Analysis. |

## Private Utilities

None — the class has no `private` methods; `SearchForAvailableDownloads`,
`IsAuthorized`, `IsSearchDone`, `GetSize`, and the `GetShipcodeOptions`/
`GetAssociatedMediaIDs`/`GetFamilyList`/`GetFamilySize` accessors are all
`public`, and `OnExecute`/`OnTerminate`/`GetSecurity` are `protected` (per the
header, `pimGetAvailable.h:30-34`).

## Called By

- `pimInstallMgrDlg::EntitlementDownloadPreAction()`
  (`pim_ui/pim_ui_src/pimEntitlementRefresh.cxx:279-302`) — the **sole confirmed
  construction site** (`AvailableDownloads = XNew pimGetAvailable(*SessionInfo);`,
  guarded by `if (!AvailableDownloads)`), and the driver of the
  `SearchForAvailableDownloads()` retry loop: `do { ret =
  AvailableDownloads->SearchForAvailableDownloads(ret == 0); if (ret == -3) {
  delete AvailableDownloads; AvailableDownloads = NULL; return false; } } while
  (ret == 0);` — confirming the `0`/`1`/`-3` return-value contract documented
  above (a `0` retries with `retry = true` on the next iteration, showing the
  user an "auth failed" message; a `-3` deletes the instance and aborts
  immediately). On success, calls `LocateAllowPTCDotCom(true, AvailableDownloads)`
  — see Ownership Model.
- `pimPTCLicenseGet::HeartBeat()` (`pim_ui/pim_ui_src/pimPTCLicenseGet.cxx:249-262`)
  — a **2nd, previously-uncatalogued driver** of the same
  `EntitlementDownloadPreAction()` entry point, reached from a license-retrieval
  heartbeat's `GetNeedsEntitlements()` branch: if `AvailableDownloads` is `NULL`,
  calls `EntitlementDownloadPreAction()` to create and kick off the search; if
  non-`NULL` but not yet `IsDone()`, defers (`in_hb = false; return true;`) to
  poll again on the next heartbeat tick.
- `pimEntitlementTree.cxx` (`GetFamilyListForTree()`,
  `pim_ui_src/pimEntitlementTree.cxx:178-179`, and the version/shipcode dropdown
  population at `:682-683`) — read-only consumer of the completed results via
  `GetMainUIDialog()->AvailableDownloads->...`.
- `pim_core/pim_core_src/pimLocate.cxx`'s `LocateXMLFile()`/`LocateTranslationFile()`
  (`:88-103`, `:264-276`) — read-only consumers of `GetAssociatedMediaIDs()`
  through the cross-module alias described in Ownership Model.
- `~pimInstallMgrDlg()` (`pim_ui_src/pimInstallMgrDlg.cxx:459-464`) — the sole
  destruction site: un-registers the `pimLocate.cxx` alias
  (`LocateAllowPTCDotCom(false, NULL)`) **before** `delete AvailableDownloads`.

## Calls Into

- `pimSessionInfo` (`GetSecurity`/`SetSecurity`/`DelSecurity`/`AddEntitlement`/
  `GetProperty`/`Save`/`GetXMLPtr`, extensively).
- `pimGetMediaDetails::GetInstance().GetDetails(...)` (per-media-ID details XML).
- `pimTranslateMgr::TranslateFrom()` / `LocateTranslationFile()`.
- `pimAuthorizeToPTC`, `pimEntitlementRequestToPTC`, `pimEntitlementAuthFailed`,
  `pimSendNRecvFromPTC` (PTC.com HTTP/auth transport, external to this pass).
- `pimNonBrowserAuthenticationCB()` → `pimAuthDlg` (see `docs/classes/pimAuthDlg.md`).
- `pimGetApplicationsList`, `pimCmpDottedVersions`, `pimGetProductMode`,
  `pimRunInTrialMode`/`pimRunInSchoolMode` (product-mode/version filtering).

## Lifetime

Heap-allocated (`XNew`), owned by `pimInstallMgrDlg` as its `AvailableDownloads`
member, constructed at most once per process (guarded by a `NULL` check at both
of its 2 confirmed call paths) and destroyed exactly once, either by
`~pimInstallMgrDlg()` at process/wizard teardown, or earlier by
`EntitlementDownloadPreAction()`'s own retry loop if the initial authorization
sequence aborts (`ret == -3`) before ever succeeding. Once a search has
succeeded and registered itself with `pimLocate.cxx` (see Ownership Model),
neither confirmed caller ever re-enters `EntitlementDownloadPreAction()` while
the member is still set — so, in the traced call graph, an instance is never
both registered with `pimLocate.cxx` and later deleted by the retry-loop abort
path in the same process (see Ownership Model for why this matters).

## Ownership Model

`pimInstallMgrDlg` owns the single instance outright (constructs via `XNew`,
deletes in its own destructor). Two further wrinkles, both confirmed by direct
reading:

1. **A background thread mutates session/entitlement state owned by the UI
   thread's `pimSessionInfo`.** `OnExecute()` calls `SessionInfo->AddEntitlement()`,
   `SetProperty()`, and multiple `pimEntitlement` mutators, all from the
   `pimLoop`-spawned worker thread, while the UI thread may concurrently be
   reading the same `pimSessionInfo` (e.g. via `pimEntitlementTree`'s refresh
   logic). No mutex around these specific calls was found in this class; any
   synchronization would have to come from `pimSessionInfo`/`pimXmlFile`'s own
   locking (already documented, with a confirmed real bug history, in
   `docs/classes/pimXmlFile.md`) rather than from `pimGetAvailable` itself.
2. **A raw, non-owning pointer alias crosses the `pim_ui`/`pim_core` module
   boundary.** `pim_core/pim_core_src/pimLocate.cxx` declares its own file-static
   `static pimGetAvailable *AvailableDownloads = NULL;` (`pimLocate.cxx:40`,
   distinct from and not to be confused with `pimInstallMgrDlg`'s member of the
   same name), set only via the free function `LocateAllowPTCDotCom(bool tf,
   pimGetAvailable *in)` (`:46-52`), which also flips a second file-static,
   `LocateSearchPTCDotCom`. This is the **same architectural pattern** already
   flagged as a confirmed live bug for `pimSAB.cxx`'s `main_dlg_sab_handle`/
   `help_dlg_sab_handle` statics (see `docs/classes/pimCustomDlg.md`'s Risk
   Analysis) — a shared mutable pointer with no encapsulated lifetime guarantee,
   coordinated only by call-site discipline. Here, tracing every confirmed call
   site (see Called By) shows the discipline currently holds: `LocateAllowPTCDotCom`
   is called exactly twice (register-on-success in `EntitlementDownloadPreAction()`,
   clear-in-destructor in `~pimInstallMgrDlg()`), and the retry loop's own
   `delete AvailableDownloads` (on a `-3` abort) always happens *before* any
   successful registration could occur within that same call, so the two never
   race against each other in the current code. **This is confirmed safe today,
   but fragile**: it depends entirely on both of `EntitlementDownloadPreAction()`'s
   2 confirmed callers continuing to guard their calls with `if (AvailableDownloads
   == NULL)` (which they currently both do). A future 3rd call site that omits
   that guard, or a future refactor of the retry loop, could re-delete an
   already-registered instance without clearing `pimLocate.cxx`'s alias, leaving
   it dangling.

## Thread Safety

Inherits `pimLoop`'s `Mutex`-guarded lifecycle flags (`mDone`, etc.). `auth_status`
is explicitly read/written under `Mutex` in `IsAuthorized()`/`OnExecute()`'s
error path, and again under `Mutex` in `OnTerminate()`'s `mDone = true`. No
other member (`AvailableProductsArray`, `family_grp`, `family_count`) is
guarded by any lock, despite all three being written on the background thread
in `OnExecute()` and read from the UI thread once `IsDone()` returns true
elsewhere (`pimEntitlementTree.cxx`, `pimLocate.cxx`) — consistent with (and
relying on) the same "background thread writes, UI thread only reads after
polling `IsDone()`" convention already documented for other Loop subclasses in
this set (e.g. `pimDownloadLoop`'s `GetStatus()`), rather than any explicit
synchronization primitive of its own.

## Extension Points

- A new product-filtering rule (beyond product-mode, major-version, and the
  app allow-list) would extend the `if`/`continue` chain inside `OnExecute()`'s
  main `for (i loop` — there is no data-driven filter registry, matching the
  hardcoded-`if`/`else if` pattern already flagged elsewhere in this
  documentation set (e.g. `pimEntitlementTree::UpdateDisplay_low()`'s hardcoded
  product-tag special cases).
- Any future caller of `EntitlementDownloadPreAction()` (a 3rd driver beyond the
  2 confirmed today) **must** preserve the existing `if (AvailableDownloads ==
  NULL)` guard pattern — see Ownership Model for why relaxing it would
  reintroduce a dangling-pointer risk in `pim_core/pimLocate.cxx`.

## Risk Analysis

- **CONFIRMED: two memory leaks in `OnExecute()`.** (1) `ImageXml` (a
  heap-allocated `pimXmlFile*`, `OnExecute():286`) is explicitly `delete`d on
  every early-exit/`continue` branch (parse error, version-too-old, missing
  shipcode, etc. — 5 separate `delete ImageXml;` sites confirmed between
  `:291` and `:362`) but is **never deleted on the successful path** — after
  the entitlement loop (`:376-439`) and family-group loop (`:442-469`) both
  complete normally, control falls out of the enclosing `if (node)` block
  (`:471`) and `ImageXml` goes out of scope with no matching `delete`. This
  leaks one full parsed DOM tree per successfully-processed available image,
  every time a web-media search runs. (2) `ProductDefinitionXml`
  (`OnExecute():547`) is `delete`d only when `DoRead()` fails
  (`ErrorOccured()`, `:552-557`); if the retrieved XML parses successfully but
  is then filtered out as a `<HIDDEN_PRODUCT>` (`:574`, `hidden_product_nl->
  getLength() != 0`) or `SessionInfo->AddEntitlement()` determines it "is
  determined not to be a product definition file" (`:622-625`), the pointer is
  silently dropped with no `delete` in either branch. Both leaks are bounded by
  the lifetime of one `OnExecute()` run in a short-lived installer process, but
  are confirmed, real, and would compound with the number of available
  products/media entries a given search encounters.
- **CONFIRMED: `auth_status`'s declaring comment is wrong.** `pimGetAvailable.h:26`
  reads `int auth_status;  // -3 abort, 0 success`. Tracing every assignment
  shows `0` is set exactly when authentication has **failed** (`MSG_A_NOACCOUNT`/
  `MSG_INVALID_AUTHID`, `pimGetAvailable.cxx:124` and `:211`) — the opposite of
  "success" — while the actual success value, `1`, is set only in `OnExecute()`
  (`:217`) after a fully successful `pimEntitlementRequestToPTC()` call, and is
  not mentioned in the comment at all. `-3` is set only for a "should never
  happen" internal security-lookup failure inside `OnExecute()` (`:194`), not
  for every abort path (`SearchForAvailableDownloads()`'s own `-3` return value,
  used far more often by its caller, is a *function return value*, not this
  member). Anyone relying on the header comment rather than reading the
  assignments would misinterpret `IsAuthorized()`'s result.
- **CONFIRMED: `IsAuthorized()` has zero callers anywhere in this archive** —
  a full-archive grep finds only its own declaration and definition. The
  `Mutex`-protected read it implements is never actually consulted by any
  confirmed caller; both `SearchForAvailableDownloads()`'s own return value and
  `IsSearchDone()`/`IsDone()` are what callers actually poll instead. Confirmed
  dead public API, in the same category as other "written carefully, read by
  nobody" findings catalogued elsewhere in this set (e.g. `pimAuthDlg`'s `URL`
  member).
- **CONFIRMED, but not a bug: a fragile cross-module raw-pointer alias.** See
  Ownership Model — currently safe given the exact 2 call sites that exist
  today, but with no structural safeguard against a future change breaking that
  invariant, unlike (for contrast) a reference-counted or `GetInstance()`-style
  singleton.
- **Design note, not a bug: `GetSecurity()`'s cross-URL credential reuse.** If
  the user has already authenticated against the general PTC.com Production
  URL, `GetSecurity()` (`:64-77`) opportunistically copies those same
  credentials into the Media Production URL's security store via `SetSecurity()`
  rather than prompting again — an assumption that both PTC.com service
  endpoints accept the same credentials. Not confirmed as incorrect (no
  evidence either way in this archive), but worth flagging as an assumption
  baked into the auth flow rather than something independently verified per
  URL. **Confirmed, found in a further, dedicated pass on
  `pimSessionInfo::TryAuthorize()`**: this assumption is stronger than "a
  copy" in practice — `SetSecurity()`/`GetSecurity()`/`DelSecurity()`'s own
  `// HACK` comment (`pim_core/pim_core_src/pimSessionInfo.cxx:544-584,586-615,617-`)
  maps the media URL and the production URL to the **same**
  `SecurityArray` slot, so `TryAuthorize()`'s own credential
  lookups/deletes and this method's are confirmed to alias, not merely
  mirror one another — see `docs/classes/pimSessionInfo.md`'s Risk
  Analysis.
- **Minor: `family_count` is redundant with `family_grp.GetSize()`.** The two
  are always updated together (`family_grp += group; family_count++;`,
  `OnExecute():467-468`) and reset together
  (`family_grp.Clear(); family_count=0;`, `:443-444`) — `GetFamilySize()` could
  be `return family_grp.GetSize();` with no `family_count` member at all. Not a
  bug (the two are never observed out of sync in this pass), just an
  unnecessary duplicated invariant a future edit to one call site could break.

## Usage Example (as evidenced by call sites)

```cxx
// pim_ui_src/pimEntitlementRefresh.cxx:279-302 (pimInstallMgrDlg::EntitlementDownloadPreAction)
if (!AvailableDownloads)
    AvailableDownloads = XNew pimGetAvailable(*SessionInfo);
if (AvailableDownloads)
{
    int ret = -99;
    do
    {
        ret = AvailableDownloads->SearchForAvailableDownloads(ret == 0); // retry with a visible error message on the 2nd+ attempt
        if (ret == -3) // abort -- e.g. user cancelled the credential prompt
        {
            delete AvailableDownloads;
            AvailableDownloads = NULL;
            return false;
        }
    } while (ret == 0); // 0 == bad account / invalid auth ID -- retry
    LocateAllowPTCDotCom(true, AvailableDownloads); // register with pim_core/pimLocate.cxx
    return true;
}
```

---
*Extends this documentation set's `pim_ui`-focused extension series into
`pim_core`: discovered while documenting `pim_ui_src/pimEntitlementRefresh.cxx`'s
`EntitlementDownloadPreAction()`, and now given full-depth Loop-subclass
treatment — the 10th confirmed `pimLoop` subclass in this codebase.*
