# Class: `pimSessionInfo`

**File:** `pim_core/includes/pimSessionInfo.h` (281 lines) / `pim_core/pim_core_src/pimSessionInfo.cxx` (not read line-by-line in this pass, except `TryAuthorize()` and its immediate dependencies, `:1911-1984`, fully traced in a further, dedicated pass — see Risk Analysis)
**Module:** `pim_core`

## Purpose

The top-level session state object for one run of PIM. Holds the collection of
products being considered (`pimEntitlement` instances), what's already installed,
license source/security state, and a generic string-keyed property bag used as the
de facto configuration/context blackboard across the entire codebase.

## Responsibilities

- Own the entitlement collections: `EntitlementArray` (candidates for this session)
  and `InstalledArray` (already-installed products).
- Own `pimInterrogator* xmlWhatsInstalled` — machine-state discovery.
- Own `pimLicenseSource* ValidateLicenseSources` and the license-source workflow:
  add/modify/drop license sources, validate them (`GetLicenseSourceByIdx` returns a
  `LicenseStatus` enum), detect triads, track firewall exceptions
  (`ValidateFirewallStatus`, `GetFirewallExceptionWarning`).
- Own credential state for browser/network auth challenges (`SecurityArray` of
  `pimSecurity*`, keyed conceptually by site URL).
- Own the generic property bag (`SetProperty`/`GetProperty`, backed by `xmlPtr` — a
  `pimXmlFile*` session XML document, not a simple in-memory map).
- Persist/restore session state across process runs (`Save()`,
  `FoundPreviousSessionInfoFile()`, `LoadPreviousSessionInfoFile()`,
  `ClearTempSaveFiles()`).
- Coordinate trial/school/beta license acquisition (`GetTrialLicense`/
  `GetTrialLicenseLoop`, `GetSchoolLicense`/`GetSchoolLicenseLoop`,
  `GetBetaLicense`/`GetBetaLicenseLoop` — each pairs a boolean action with an
  accessor for the underlying `pimPTCLicenseGet*` loop object).
- Own default command/PSF initialization (`InitDefaultCommands`) — writes the
  entered license sources + discovered LTR features into an entitlement's PSF-related
  XML entries.
- Test connectivity to PTC's servers (`TestCommunicationToPTC`).
- Coordinate graceful shutdown (`PrepareForShutdown`).

## Dependencies

- `pimXmlFile` (session XML document — `xmlPtr`).
- `pimInterrogator` (installed-product/license-source discovery).
- `pimLicenseSource` (license validation logic).
- `pimConvert` (included; used for property resolution).
- `pimEntitlement` (forward-declared; owns arrays of these).
- `pimPTCLicenseGet`, `pimPTCRenewLicenseGet`, `pimFrictionlessTrialLicenseGet`
  (forward-declared collaborator classes for the trial/renewal license-fetch flows —
  **not documented in this pass**).
- `thrRWLock` (external `btk` — `Mutex` member, a read/write lock rather than a plain
  mutex, implying concurrent readers are expected).
- **`pimNonBrowserAuthenticationCB()`** (`pim_ui/pim_ui_src/pimAuthDlg.cxx:87-112`,
  a `pim_ui` free function — new citation, traced in a dedicated pass on
  `TryAuthorize()`): this `pim_core` file includes `pim_ui/includes/pimAuthDlg.h`
  directly (`pimSessionInfo.cxx:102`) to call it — an unusual, confirmed
  `pim_core`-depends-on-`pim_ui` direction (`pimGetAvailable.cxx`'s own
  analogous caller instead uses a local forward declaration, avoiding the
  direct header dependency). See `docs/classes/pimAuthDlg.md`'s Called By
  section for this function's own body.
- **`pimAuthorizeToPTC()`/`pimAuthorizeFailedDirectCall()`/`pimAuthorizeFailedMsgDirectCall()`**
  (`pim_core/pim_core_src/pimPTCDotCom.cxx:540-679,681-702,704-724`, new
  citations): the actual PTC.com server round-trip and its 2 failure-message
  helpers. See Risk Analysis for confirmed bugs in how `TryAuthorize()`
  uses their return values (or fails to).

## Members

| Member | Type | Purpose |
|---|---|---|
| `xmlPtr` | `pimXmlFile*` | Session-state XML document backing the property bag |
| `xmlWhatsInstalled` | `pimInterrogator*` | Installed-software discovery |
| `ValidateLicenseSources` | `pimLicenseSource*` | License source validation |
| `install_mode` | `btkString` | Current install mode string (`SetInstallMode`/`GetInstallMode`) |
| `Mutex` | `thrRWLock*` | Read/write lock over session state |
| `SecurityArray` | `dsXArray<pimSecurity*>` | Per-site remembered credentials |
| `EntitlementArray`, `InstalledArray` | `dsXArray<pimEntitlement*>` | Candidate vs. already-installed products |
| `EULA` | `bool` | EULA acceptance state |
| `CurrentSessionInfo`, `PreviousSessionInfo` | `btkFSEntry` | Session save-file paths |
| `isUpdate`, `isGroupUninstall` | `bool` | Mode flags |
| `LicenseGetThread` | `pimPTCLicenseGet*` | Active license-fetch thread, if any |
| `LicenseServerStatus`, `LicenseServerName` | `btkString` | Cached license-server state |
| `FirewallExceptionWarning` | `btkString` | Cached firewall-check result text |

## Public APIs (selected — full list in header)

- **Lifecycle**: ctor/dtor, `Save()`, `PrepareForShutdown()`,
  `FoundPreviousSessionInfoFile()`, `LoadPreviousSessionInfoFile()`,
  `ClearTempSaveFiles()`.
- **Property bag**: `SetProperty(name, value, attrib=NULL)`,
  `GetProperty(name, value&, attrib=NULL)` — the mechanism behind essentially every
  `*_PROPERTY` constant defined at the bottom of the header (`PLATFORM_PROPERTY`,
  `SHIPCODE_PROPERTY`, `MEDIA_PROPERTY`, `EXERUN_PROPERTY`, etc. — 25+ keys).
- **Entitlement management**: `AddEntitlement(cStringT)`,
  `AddEntitlement(pimXmlFile*, pimXmlFile*, const btkString&)`,
  `GetEntitlementSize`, `GetEntitlement(cStringT|int)`, `DropEntitlement(cStringT|int)`,
  and the parallel `*InstalledEntitlement*` set for the installed-products list.
- **License source management**: `AddLicenseSource`, `AddLicenseSourceNew`,
  `IsLicenseSourceAlreadyAdded`, `ModifyLicenseSource`, `HasLicenseIndex`,
  `HasLicense`, `GetSizeLicenseSources`, `GetNewLicenseSourceIndex`,
  `GetLicenseSourceByIdx`, `DelLicenseSourceByIdx`, `ResetLicenseSource`,
  `CanLicensePrefixes`, `GetAllLicenseFeatures`, `IsLicenseServerEntitlement`,
  `GetLicenseServerEntitlement`, `GetInstalledLicenseServerXml`.
- **Security/credentials**: `SetSecurity`, `GetSecurity`, `DelSecurity`.
- **Communication**: `TestCommunicationToPTC`.
- **Trial/school/beta licensing**: `TryAuthorize`
  (`:1911-1984`, fully traced in a further, dedicated pass — see Risk
  Analysis for confirmed bugs), `GetTrialLicense`/
  `GetTrialLicenseLoop`, `GetSchoolLicense`/`GetSchoolLicenseLoop`,
  `GetBetaLicense`/`GetBetaLicenseLoop`. `TryAuthorize()` is a parameterless
  `do`/`while` retry loop: while `GetSecurity(url)` (the production URL)
  finds no cached credentials, or the previous `pimAuthorizeToPTC()` attempt
  was rejected by the server, it calls the `pim_ui` free function
  `pimNonBrowserAuthenticationCB()` (`pim_ui/pim_ui_src/pimAuthDlg.cxx:87-112`)
  to show the PTC.com login dialog, then calls
  `pimAuthorizeToPTC(locale, User, Passwd, ret_http, &out)`
  (`pim_core/pim_core_src/pimPTCDotCom.cxx:540-679`). Returns `true` on a
  successful authorization; `false` **only** when the user cancels the
  credential dialog — see Risk Analysis for why every other outcome keeps
  looping instead of returning failure.
- **Misc**: `GetInterrogator()`, `GetDefaultLoadpoint(btkFSEntry&)`,
  `GetNextProcessingLabel(bool reset)`, `GetXMLPtr()`, `StartValidateThread()`,
  `GetOldShipCode()`, `pimIsPTCDProcessAllowedInFirewall()`,
  `pimIsLmgrdProcessAllowedInFirewall()`, `GetDefaultPortValues(port&, eport&)`.

## Protected / Private APIs

None declared `protected`; all data members are `private`, all methods `public`.
No private utility methods are declared in the header (implementation-only helpers,
if any, live in `pimSessionInfo.cxx` and were not enumerated in this pass).

## Called By

- `pim/pim_src/pimTop.cxx` — constructs one `pimSessionInfo` per run, drives nearly
  every public method listed above (property setting, entitlement loading, license
  source discovery/consolidation, `StartValidateThread`, `Save`,
  `PrepareForShutdown`).
- `pim_ui` dialogs — `SetSessionInfoFile`-style wiring (seen as
  `Dlg.SetSessionInfoFile(&SessionInfo)` in `pimTop.cxx`; the method itself lives on
  the dialog classes, not on `pimSessionInfo`) and subsequent reads/writes of
  properties and entitlements to drive the UI.
- `pimEntitlement` — reads back session-level context via `pimGetSessionInfo()` (a
  free function, declared alongside the class, presumably returning the singleton/
  current session).
- **`TryAuthorize()`'s own 3 call sites, new citations traced in a
  dedicated pass**: `GetTrialLicense()` (`:2003`), `GetSchoolLicense()`
  (`:2129`), `GetBetaLicense()` (`:2184`) — all 3 in this same file. Those
  in turn are called from `pimInstallMgrDlg::EulaNextPreAction()`/
  `BetaEulaNextPreAction()` (`pim_ui/pim_ui_src/pimEulaRefresh.cxx:132,139,233`),
  themselves reached from `pimInstallMgrDlg::OnPushButtonActivate(NextStepBtn)`
  (`pim_ui_src/pimInstallMgrActions.cxx:902,912`) — see
  `docs/classes/pimInstallMgrDlg.md`'s Risk Analysis for a confirmed bug in
  that caller's own ordering that compounds with `TryAuthorize()`'s
  findings below. A 4th, `#if 0`-disabled copy exists inside
  `GetTrialLicense()` itself (`:2005-2077`) — confirmed dead, and would not
  compile if re-enabled (see Risk Analysis).

## Calls Into

`pimXmlFile` (property storage), `pimInterrogator`, `pimLicenseSource`,
`pimEntitlement` (constructs/holds), FlexNet-related utility functions (indirectly,
via `pimInterrogator`/`pimLicenseSource` — not traced in this pass).

## Lifetime

One instance per process run, stack-allocated in each `pim*Run` function in
`pim/pim_src/pimTop.cxx` (`pimSessionInfo SessionInfo;`) — lives for the duration of
that function call and is destroyed on return.

## Ownership Model

Owns its `EntitlementArray`/`InstalledArray` (arrays of `pimEntitlement*`) and
`SecurityArray` (`pimSecurity*`) — deletion responsibility presumed to be in the
destructor (not verified line-by-line in this pass). Owns `xmlWhatsInstalled`
(`pimInterrogator*`) and `ValidateLicenseSources` (`pimLicenseSource*`) similarly.

## Thread Safety

Guarded by a `thrRWLock* Mutex` (read/write lock, external `btk`) rather than a plain
mutex — implies the design intent is many concurrent readers (e.g. UI polling
properties/entitlements while a background validate thread runs) with exclusive
writers. `StartValidateThread()` explicitly starts a background thread (presumably
validating license sources concurrently with UI interaction — consistent with
`pimTop.cxx` calling it right before showing the dialog). Exact locking granularity
(whether `Mutex` wraps every public method or is used more narrowly) was **not
verified against the `.cxx`** in this pass.

## Extension Points

- New session-wide property: no schema changes needed — call `SetProperty`/
  `GetProperty` with a new key. (See `docs/modules/pim_core.md` Risks for the
  discoverability downside of this flexibility.)
- New license-source status: extend the `LicenseStatus` enum (13 values currently,
  from `UNKNOWN` through `TRIAD_TO_SINGLE_CONFLICT`) and `GetStatusTranslateMsg`'s
  handling of it.

## Risk Analysis

- The property bag is the *de facto* configuration mechanism for the whole
  application, backed by an XML document rather than a typed structure — every
  property name is a string literal matched by equality; there is no
  compile-time check that a property being read was ever actually set anywhere, or
  that its name is spelled consistently across the ~15 files that reference these
  `#define`d constants.
- `LicenseStatus` has 13 enumerators covering a wide space of license-file/server
  validity states — the branching logic that assigns/interprets these (largely in
  `pimSessionInfo.cxx` and `pimTop.cxx`, e.g. the triad-consolidation loop) is
  intricate and was only partially traced (`pimTop.cxx`'s consumption of it) in this
  pass; a deeper read of `pimSessionInfo.cxx` and `pimLicenseSource.cxx` would be
  needed before safely modifying license-status semantics.
- Read/write lock (`thrRWLock*`) usage pattern not fully verified — if any code path
  takes a write lock while already holding a read lock (or vice versa) without going
  through a documented upgrade path, that's a classic deadlock/starvation risk common
  to RW-lock designs; flagged for a deeper follow-up rather than confirmed as an
  actual bug.
- **CONFIRMED BUG, HIGH SEVERITY, found in a further dedicated pass fully
  tracing `TryAuthorize()`: the function loops forever, with no user
  prompt, whenever the PTC.com round-trip itself fails (network
  unreachable, or an unparseable response) — as opposed to the server
  actively rejecting the credentials.** (`pim_core/pim_core_src/pimSessionInfo.cxx:1911-1984`,
  depends on `pim_core/pim_core_src/pimPTCDotCom.cxx:540-679`.)
  ```cpp
  do
  {
      if (!GetSecurity(url, User, Passwd) || User.IsEmpty() || Passwd.IsEmpty())
      {
          // ... show login dialog via pimNonBrowserAuthenticationCB(), re-read GetSecurity() ...
      }
      if (do_continue)
      {
          if (pimAuthorizeToPTC(pimGetLocaleShort(false), User, Passwd, ret_http, &out))
          {
              if (!pimAuthorizeFailedDirectCall(out))
                  { do_continue = false; delete out; out = NULL; }   // success
              else
                  DelSecurity(url);                                  // server REJECTED -- retry with new creds
              // else: pimAuthorizeToPTC() returned FALSE -- NO branch handles this at all
          }
      }
  } while (do_continue);
  ```
  `pimAuthorizeToPTC()` (`pimPTCDotCom.cxx:540-679`) returns `false`
  whenever the HTTP round-trip itself fails or the server's response
  doesn't parse (`ret_http != 1`) — as distinct from a successful
  round-trip that the server then rejects (handled by the
  `pimAuthorizeFailedDirectCall(out)` branch above). There is **no `else`**
  for `pimAuthorizeToPTC()` returning `false`: `do_continue` stays `true`,
  the cached credentials are left untouched (`DelSecurity()` is only
  called on the *rejection* branch), so the next iteration's
  `GetSecurity(url)` call succeeds again, skips the login dialog entirely,
  and immediately retries the identical, still-failing PTC.com call — an
  **unbounded retry loop with no user prompt, no retry cap, and no
  back-off**, running synchronously on the UI thread while
  `OnPushButtonActivate()` (see `docs/classes/pimInstallMgrDlg.md`) has
  already disabled both wizard navigation buttons. **Confirmed by direct
  contrast** with the sibling caller `pimGetAvailable::SearchForAvailableDownloads()`
  (`pim_core/pim_core_src/pimGetAvailable.cxx:114-136`, see
  `docs/classes/pimGetAvailable.md`), which calls the identical
  `pimAuthorizeToPTC()` and, on the same failure, returns `-3` to a caller
  that then aborts — the correct, already-precedented handling for this
  exact failure mode elsewhere in this same codebase.
- **CONFIRMED BUG, found in the same pass: a `pimXmlFile` is leaked on
  every iteration where the server rejects the credentials (or the
  round-trip fails to parse), including every pass of the infinite loop
  above.** `pimAuthorizeToPTC(..., &out)` (`pimPTCDotCom.cxx:651`) does
  `(*out) = XNew pimXmlFile();` **unconditionally, overwriting whatever
  `*out` already pointed to without deleting it first**. `TryAuthorize()`'s
  own `static pimXmlFile* out` (`:1919`) is deliberately *kept* (not
  deleted) after a confirmed server rejection, specifically so the next
  iteration can read the server's message out of it — but that next
  iteration's `pimAuthorizeToPTC()` call immediately overwrites the
  pointer without freeing the previous allocation. Every rejected retry,
  and every iteration of the infinite loop above, leaks one more
  `pimXmlFile`.
- **Verified non-issue, a related design observation**: the `static
  pimXmlFile* out` at `:1919` provides no actual benefit — every path that
  exits the `do`/`while` loop explicitly nulls it first (`:1955` on
  success, `:1969` when the user cancels, and the redundant `delete` at
  `:1976-1980` runs against an already-NULL pointer, since `:1966-1970`
  already ran). Being `static` only makes the function non-reentrant for
  no compensating benefit, since it is always `NULL` on entry regardless.
- **CONFIRMED, by direct contrast with this codebase's own GUI-adjacent
  helper**: `pimAuthorizeFailedMsgDirectCall()`'s return value is
  discarded at the one call site that uses it (`:1926`), and its output
  parameter (`str`, the server's retry message) is declared once, outside
  the `do`/`while` loop (`:1913`) — the same "discarded `Get()` + reused
  stale variable" shape already confirmed live elsewhere in this
  documentation set (see `docs/classes/pimSilent.md`'s Program-Menu and
  `<PROPERTY>`-copy findings). If the server's rejection response ever
  lacks a message, the login dialog would show a stale retry message
  carried over from an earlier iteration, or an empty one on the first —
  not confirmed against a real PTC.com response in this archive, since no
  network access exists to test it.
- **CONFIRMED, a related structural finding**: `TryAuthorize()`'s
  credential lookups alias with `pimGetAvailable::GetSecurity()`'s
  (`docs/classes/pimGetAvailable.md`) through `SetSecurity()`/
  `GetSecurity()`/`DelSecurity()`'s own `// HACK` comment
  (`pimSessionInfo.cxx:544-584,586-615,617-`), which maps the media URL and
  the production URL to the **same** `SecurityArray` slot (the production
  one). Confirmed consequences: credentials entered through
  `TryAuthorize()`'s login dialog are the same ones
  `pimGetAvailable::GetSecurity()` finds; and `DelSecurity()`, called from
  either path (`TryAuthorize()`'s rejection branch above, or
  `SearchForAvailableDownloads()`'s own equivalent), wipes the *other*
  path's cached credentials too. This is a real, if narrow, cross-feature
  coupling between 2 otherwise-independent authentication flows sharing
  one process.
- **Verified non-issue, checked while tracing the same lock usage as the
  RW-lock caution above**: `SetSecurity()` mutates an *existing* entry
  (`ptr->setSecurity(...)`, `:567`) while holding only a **read** lock
  (`Mutex->SetReadLock()`, `:560`) — a real inconsistency with the RW-lock
  design's own intent (write access should require a write lock), but
  confirmed **not** independently reachable as a live bug in this pass
  without a concurrent second thread also touching `SecurityArray`
  through a write-locked path at the same moment — flagged as a
  contributing data point for the RW-lock caution above, not a new,
  independently-confirmed bug.
- **CONFIRMED, minor**: `annotated/pim_core/includes/pimSessionInfo.h`'s
  existing Doxygen comment for `TryAuthorize()` (`@return true if
  authorization against PTC succeeded.`) is confirmed **misleading**: it
  doesn't state that `false` only ever means "the user cancelled the
  credential dialog," nor that a genuine authorization failure (network
  down, bad response) is not a possible return value at all — see the
  infinite-loop finding above.
- **CONFIRMED dead code, found while tracing the loop's own call sites**:
  the `#if 0`-disabled duplicate of this call inside `GetTrialLicense()`
  (`:2005-2077`) would not compile if re-enabled as written — it declares
  `btkString str, str2;` and then, a few lines later in the same scope,
  `btkString url, str, str2;`, redeclaring `str`/`str2` — confirmed
  evidence this dead block has rotted since being superseded, not merely
  disabled.

## Usage Example (as evidenced by call sites)

```cxx
// pim/pim_src/pimTop.cxx pattern
pimSessionInfo SessionInfo;
SessionInfo.SetProperty(EXERUN_PROPERTY, argv[0]);
SessionInfo.SetProperty(LICENSED_PROPERTY, "N");
if (SessionInfo.AddEntitlement(files[i]))
{
    pimEntitlement *ent = SessionInfo.GetEntitlement(j);
    ent->SetQualityAgent(true);
}
SessionInfo.StartValidateThread();
SessionInfo.Save();
```

---
*Phase 5 of the requested 20-phase documentation set.*
