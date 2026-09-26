# `pimAuthDlg`

**Header**: `pim_ui/includes/pimAuthDlg.h` (60 lines)
**Implementation**: `pim_ui/pim_ui_src/pimAuthDlg.cxx` (248 lines)
**Module**: `pim_ui`

## Purpose

`pimAuthDlg` is a generic PTC.com account-credential prompt — a small dialog
(username/password + "Reset Password"/"Create Account" links) that pops up
whenever a network operation against a PTC.com resource returns an
authentication challenge. It is exposed to the rest of the codebase through 2
free functions, `pimInitAuthenticationCB()` (registers a Creo browser-security
authentication pre-action callback) and `pimNonBrowserAuthenticationCB()` (a
direct, synchronous "prompt and store credentials" call), rather than being
constructed directly by callers.

**This is the oldest of the 3 related dialogs documented in this effort**
(created 2011-10-17 per its own `$$1` revision entry, vs. `rpimDlg`'s 2015 and
`pimFrictionlessTrialDlg`'s 2018) and is now confirmed to be **the original
template both of those classes were partially copied from** — see
`docs/classes/rpimDlg.md`'s Dependencies section, written before this class
itself was documented at this depth. Unlike its 2 derivative classes,
`pimAuthDlg` is **called directly from `pim_core`** (`pimSessionInfo.cxx`,
`pimGetAvailable.cxx`), not just from `pim_ui`-internal code — matching the
pattern already documented for `pimShortcutMgr`.

**Scope note**: both files (header + `.cxx`, 308 combined lines) were read in
full — a complete pass, this class being small enough for exhaustive coverage.

## Responsibilities

- Prompt for a username/password (2 `uiInputPanel`s) and enable `LoginButton`
  only once both are non-empty (via `Refresh()`, called from
  `OnInputPanelInput()`).
- Provide "Reset Password" and "Create Account" buttons that simply open
  `RESET_PWD_LINK`/`CREATE_CUST_ACCT` in the system browser — no in-dialog
  logic.
- Display an optional retry message (`AuthRetryLabel`) set via
  `SetRetryMsg()`, shown once per `Display()` call.
- Act as a **process-wide, lazily-created, reused singleton** (`static
  pimAuthDlg *AuthDialog`, `XNew`'d on first use by either free function and
  never destroyed or reset to `NULL`) — the same "global pointer set once,
  never cleared" idiom already documented for `pimInstallMgrDlg`'s
  `MainUIDialog`/`GetMainUIDialog()`, except here the accessor functions
  (`pimInitAuthenticationCB()`/`pimNonBrowserAuthenticationCB()`) both
  correctly lazily-create it (`if (! AuthDialog) AuthDialog = XNew
  pimAuthDlg();`) rather than assuming a prior construction. **Critically,
  and unlike `rpimDlg`/`pimFrictionlessTrialDlg` (each constructed exactly
  once per process as a stack-local object), this singleton is confirmed to
  be reused — `Initialize()`/`Display()` called multiple times on the same
  instance within one process** — see Risk Analysis #1.

## Dependencies

- `pimSessionInfo` — `GetSecurity()`/`SetSecurity()` (credential cache keyed
  by URL), passed as the `auth_data`/`SessionInfo` parameter to both free
  functions.
- `bs_pro_browser_security`/`pimPTCDotCom` — the Creo browser-security
  authentication-callback API (`ProBrowserAuthSetAuthenticationCB`,
  `ProSiteURLGet`, `ProSiteCredentialsSet`) used by `pimRequestForUserPwd()`;
  not traced beyond this file's call-site usage.
- `pimWindows` — `pimSystemURL()` for the 2 external-link buttons.

## Members

| Member | Type | Purpose |
|---|---|---|
| `UserNameLabel`/`UserNameInput`, `PwLabel`/`PwInput`, `EmailLabel` | `uiLabel`/`uiInputPanel` | credential entry |
| `ResetPWButton`, `LoginButton`, `AccountButton` | `uiPushButton` | the 3 actions |
| `HelpLabel` | `uiLabel` | never populated via code in this file — presumably resource/layout-bound (not confirmed; the underlying `.res`/layout files are not in this archive) |
| `AuthRetryLabel` | `uiLabel` | shows `RetryMsg`, set in `Display()` |
| `initialized` | `bool` | **guards nothing — see Risk Analysis #1** |
| `URL` | `btkWString` | **confirmed write-only even in this, the original class — see Risk Analysis #2** |
| `User`, `Password` | `btkWString` | populated live from `OnInputPanelInput()`, read by `GetAuth()`/`Refresh()` |
| `RetryMsg` | `btkWString` | **confirmed genuinely live** — set by `SetRetryMsg()`, read by `Display()` via `AuthRetryLabel.SetWLabel(RetryMsg)` |

A file-scope `static pimAuthDlg *AuthDialog` is set to `this` in the
constructor with the same "create function to return this to outside use"
comment found on `rpimDlg`'s `rDlg` and `pimFrictionlessTrialDlg`'s
`frictionlessDlg` statics — but here the "function" genuinely exists and is
used: `pimInitAuthenticationCB()`/`pimNonBrowserAuthenticationCB()` are
exactly that accessor, the one case of this repeated idiom in the codebase
that was actually completed.

## Public/Protected APIs

- **`Initialize()`** — **has NO idempotency guard** (contrast with
  `rpimDlg::Initialize()`/`pimFrictionlessTrialDlg::Initialize()`, which both
  `if (initialized) return true;` before doing anything else): every call
  re-invokes `uiDialog::Initialize()` and unconditionally re-runs
  `LoginButton.SetSensitive(false)`. See Risk Analysis #1.
- **`Display()`** — `uiDialog::Display()` → set `AuthRetryLabel` from the
  current `RetryMsg` → `uiDialog::Activate()` → return `true`; resets
  `initialized = false` on failure. This part IS the correct pattern (same
  shape as `rpimDlg::Display()`).
- **`SetURL(const wchar_t *)`** — **DOES have confirmed callers** (2, both in
  this file's own free functions) — contrast with `rpimDlg`/
  `pimFrictionlessTrialDlg`'s copies, which have zero callers anywhere. But
  see Risk Analysis #2: even here, the `URL` member it sets is never read by
  anything.
- **`SetRetryMsg(const wchar_t *)`** — confirmed genuinely live (see Members);
  the one member/method pair in this whole 3-class family that is fully
  wired end-to-end.
- **`GetAuth(btkWString &, btkWString &)`** — copies `User`/`Password` out,
  **always `return false`** regardless of whether real credentials were
  entered. Confirmed called from both `pimRequestForUserPwd()` and
  `pimNonBrowserAuthenticationCB()`, and in **both cases the caller ignores
  the return value entirely** — so this is the "live but the return value is
  dead" counterpart to `rpimDlg`/`pimFrictionlessTrialDlg`'s "not called at
  all" copies of the same signature. See Risk Analysis #3.
- **`OnPushButtonActivate(uiPushButton &)`** — `LoginButton` → `Exit(1)` +
  `Destroy()` immediately, with an explicit comment "we can't verify here..
  just exit with true" (credential verification happens in the caller, via
  `ProSiteCredentialsSet()`/an HTTP round-trip, not in this dialog);
  `ResetPWButton`/`AccountButton` → open their respective links.
- **`OnClose()`** — `Exit(0)` + `Destroy()`.

## Private/Protected Utilities

- **`Refresh()`** — enables `LoginButton` only when both `User` and
  `Password` are non-empty. Confirmed actively used (called from
  `OnInputPanelInput()`), same as `rpimDlg::Refresh()` and unlike the fully
  dead `pimFrictionlessTrialDlg::Refresh()`.
- **`OnInputPanelInput(uiInputPanel &)`** — copies whichever input panel
  changed into `User`/`Password`, then calls `Refresh()`.

## Free Functions (declared in this class's own header)

- **`pimInitAuthenticationCB(pimSessionInfo *S)`** — lazily creates
  `AuthDialog` if needed, then registers `pimRequestForUserPwd` as the Creo
  browser-security layer's authentication pre-action callback
  (`ProBrowserAuthSetAuthenticationCB`). This is the **asynchronous/
  event-driven** integration path: whenever the browser-security layer
  itself detects a site requiring authentication (during some other network
  operation already in flight), it invokes `pimRequestForUserPwd()`.
- **`pimRequestForUserPwd(void *auth_handle, void *auth_data)`** *(file-scope,
  not a class member, but the actual callback body)* — resolves the
  challenged site's URL via `ProSiteURLGet()`; if `pimSessionInfo` already
  has cached credentials for that URL, sets them via
  `ProSiteCredentialsSet()` and returns immediately (no dialog shown). Only
  if credentials are NOT already known does it call `AuthDialog->SetURL()`/
  `Initialize()`/`Display()`, then loop retrying `Activate()` while
  `ProSiteCredentialsSet()` reports `BAD_INPUTS` (`status == -2`). **Contains
  a confirmed never-implemented feature**: a commented-out
  `//AuthDialog->SetBadInputs();` call immediately before the retry
  `Activate()` — `SetBadInputs()` does not exist anywhere in this codebase
  (confirmed by an archive-wide grep) — see Risk Analysis #4.
- **`pimNonBrowserAuthenticationCB(pimSessionInfo *SessionInfo, const wchar_t
  *URL, const char *retry_msg = NULL)`** — the **synchronous, direct-call**
  integration path used by `pim_core` code that needs credentials for a
  specific URL right now (not via the browser-security callback
  mechanism): lazily creates `AuthDialog`, sets the URL and retry message,
  `Initialize()`s and `Display()`s it, and on success stores the entered
  credentials via `SessionInfo->SetSecurity()`. Returns `true`/`false` for
  success/cancel — **this return value IS checked by every confirmed
  caller**, unlike `GetAuth()`'s.

## Called By

Both free functions are called directly from `pim_core`, not just `pim_ui`:

- `pim_core/pim_core_src/pimSessionInfo.cxx:1927,1937` —
  `pimSessionInfo::TryAuthorize()`'s `do`/`while (do_continue)` retry loop:
  calls `pimNonBrowserAuthenticationCB()` again on every iteration where
  cached credentials are missing/empty — **confirmed real, reachable
  multi-invocation of the `AuthDialog` singleton within a single process**,
  the evidence base for Risk Analysis #1. A structurally identical, older
  copy of this same loop exists at `pimSessionInfo.cxx:2005-2044+` inside an
  `#if 0`-disabled block, superseded by the extracted `TryAuthorize()`
  method call at `pimSessionInfo.cxx:2003` — a confirmed instance of this
  project's now-familiar `#if 0` dead-code pattern (see
  `ai-context/ai_readme.md`'s High-Risk Areas item on `#if`/`#ifdef`-guarded
  code). **`TryAuthorize()` itself fully traced in a further, dedicated
  pass**: see `docs/classes/pimSessionInfo.md`'s Risk Analysis for a
  confirmed infinite-loop bug (the retry loop above never terminates when
  `pimAuthorizeToPTC()` fails on a transport/parse error, as opposed to an
  explicit server rejection), a matching `pimXmlFile` leak on every such
  iteration, and a confirmed credential-key aliasing with
  `pimGetAvailable::GetSecurity()` below.
- `pim_core/pim_core_src/pimGetAvailable.cxx:85,88` —
  `pimGetAvailable::GetSecurity(bool retry)`: calls
  `pimNonBrowserAuthenticationCB()` once without a retry message, and again
  (from a different call site, gated on its own `retry` parameter) with an
  explicit `pimUIAuthFailed` message — a 2nd confirmed real call path that
  can re-invoke the same singleton.
- `pim_ui/pim_ui_src/pimPTCLicenseGet.cxx:208,210` — `pimNonBrowserAuthenticationCB()`
  called from the commercial-trial license-retrieval worker (the class
  `pimFrictionlessTrialDlg::FrictionlessLicenseGenerate()` constructs when
  `pimGetInHouseMode() == 2`, per `docs/classes/pimFrictionlessTrialDlg.md`)
  — not traced in depth this pass, cited as a 3rd confirmed call path.
- `pimInitAuthenticationCB()` itself has no confirmed caller found in this
  archive via grep (the browser-security-callback registration path) — its
  own caller was not located this pass; flagged as an open item rather than
  asserted dead, since a registration call could plausibly live in a file or
  code path not grepped for this specific function name pattern.

## Calls Into

`pimSessionInfo` (`GetSecurity()`/`SetSecurity()`), the Creo browser-security
API (`ProSiteURLGet`/`ProSiteCredentialsSet`/`ProBrowserAuthSetAuthenticationCB`),
`pimSystemURL()`.

## Lifetime

**The only one of the 3 sibling dialogs in this family that is a heap-allocated,
reused singleton rather than a single-use stack-local object.** `AuthDialog`
is `XNew`'d once (by whichever of the 2 free functions is called first) and
never `delete`d or reset — every subsequent authentication challenge in the
same process reuses the same instance, re-running `Initialize()`/`Display()`
on it.

## Ownership Model

Does not own `pimSessionInfo` (borrowed pointer, passed per-call as
`auth_data`/`SessionInfo`, not stored as a persistent member — a different,
more stateless pattern than `rpimDlg`/`pimFrictionlessTrialDlg`, which each
store a `pimSessionInfo *SessionInfo` member via `SetSessionInfoFile()`).
`AuthDialog` itself is a leaked-by-design process-lifetime singleton (never
freed, consistent with this being an installer process that exits shortly
after use).

## Thread Safety

No threading primitives — UI-thread-only code, consistent with every other
`pim_ui` dialog class in this project.

## Extension Points

None specific to this class — it is small, single-purpose, and its only
"extension" pattern (the retry-message mechanism) is already fully wired.

## Risk Analysis

1. **CONFIRMED STRUCTURAL GAP: `Initialize()` has no idempotency guard, and
   this class is the one member of its 3-class family confirmed to actually
   be reused (re-`Initialize()`d) within a single process.**
   `pim_ui/pim_ui_src/pimAuthDlg.cxx:146-154`:
   ```cpp
   bool pimAuthDlg::Initialize()
   {
       if (uiDialog::Initialize())
       {
           LoginButton.SetSensitive(false);
           initialized = true;
       }
       return (initialized);
   }
   ```
   has no `if (initialized) return true;` at the top — contrast directly
   with `rpimDlg::Initialize()`/`pimFrictionlessTrialDlg::Initialize()`,
   which both guard this way. Because `AuthDialog` is a heap singleton never
   reset between uses (unlike its 2 derivative classes, each constructed
   fresh exactly once per process), and because `pimNonBrowserAuthenticationCB()`
   is confirmed called from a real `do`/`while` retry loop
   (`pimSessionInfo::TryAuthorize()`, `pimSessionInfo.cxx:1911-1984`) as well
   as from 2 further confirmed call paths
   (`pimGetAvailable::GetSecurity()`, `pimPTCLicenseGet.cxx`), this class's
   `Initialize()` CAN and does get invoked more than once on the same
   instance within a single process. **Consequence not fully determined**:
   whether calling `uiDialog::Initialize()` a 2nd time on an already-
   initialized dialog is safe (idempotent no-op) or causes duplicate
   component registration/resource re-acquisition depends on `uiDialog`'s
   own implementation, which is not in this archive — flagged as a confirmed
   structural gap with an unconfirmed severity ceiling, not an asserted
   crash, per this project's "no invented behavior" rule. This is, however,
   a clear case where the ONE class in this family that actually needed the
   idempotency guard (being reused) is the ONE class missing it, while the
   2 classes that never needed it (single-use) both have it.
2. **Confirmed: the `URL` member is write-only even in this, the original
   template class** — refining, not contradicting, the earlier finding on
   `rpimDlg`/`pimFrictionlessTrialDlg`. `SetURL()` genuinely IS called here
   (twice, from this file's own 2 free functions) — unlike its 2 derivative
   copies, which have zero callers anywhere — but the `URL` member it
   populates is never read anywhere in this file either. So the "dead
   `URL` member" pattern traces all the way back to the original 2011 class,
   not something introduced only when the design was copied — it is the
   `SetURL()` *method* that stopped being called in the 2 later copies, not
   a case of a previously-useful member becoming useless through copying.
   Contrast with `RetryMsg`, which genuinely IS read (in `Display()`) — so
   not every shared member followed the same fate.
3. **`GetAuth()`'s `bool` return value is confirmed dead across all 3
   sibling classes' combined 5+ known call sites** — always `false`
   regardless of actual success, and every confirmed caller (both in this
   file) ignores the return value entirely, relying instead on the
   separately-checked `ProSiteCredentialsSet()`/`pimNonBrowserAuthenticationCB()`
   return values for actual success/failure signaling. Low severity (fully
   inert in every traced caller), but worth recording as a misleading API
   surface: a `bool`-returning "getter" that cannot report anything.
4. **Confirmed never-implemented feature**: `pimRequestForUserPwd()`'s
   commented-out `//AuthDialog->SetBadInputs();` (line 51) — no
   `SetBadInputs()` method exists anywhere in this codebase. On a
   `BAD_INPUTS` retry, the dialog re-`Activate()`s with no visual indication
   that the previous attempt failed (unlike `pimNonBrowserAuthenticationCB()`'s
   separate call path, which pre-sets a `retry_msg` via `SetRetryMsg()`
   before the first `Display()` when the caller has one available). Low
   severity (a UX polish gap, not a functional defect), noted for
   completeness per this project's practice of flagging commented-out
   call sites as evidence of confirmed-incomplete features.
5. **A structurally identical, disabled duplicate of `TryAuthorize()`'s retry
   loop exists behind `#if 0`** at `pimSessionInfo.cxx:2005` onward,
   superseded by the extracted `TryAuthorize()` method call — a further
   confirmed instance of this project's recurring `#if`/`#ifdef` dead-code
   pattern (see `ai-context/ai_readme.md`'s High-Risk Areas).

## Usage Example

The synchronous, direct-call integration path (the one with confirmed
`pim_core` callers), from `pim_core/pim_core_src/pimSessionInfo.cxx`'s
`TryAuthorize()`:

```cpp
do
{
    if (!(GetSecurity(btkWString(url), User, Passwd)) || User.IsEmpty() || Passwd.IsEmpty())
    {
        // ... resolve a retry message `str` if a previous attempt failed ...
        if (!pimNonBrowserAuthenticationCB(this, btkWString(url), str))
        {
            do_continue = false;   // user cancelled
            did_cancel = true;
        }
        else
            GetSecurity(btkWString(url), User, Passwd);   // re-fetch what was just entered
    }
    // ... attempt pimAuthorizeToPTC() with User/Passwd ...
} while (do_continue);
```

Each iteration through this loop that reaches the credential check can
re-invoke `AuthDialog->Initialize()` on the same, already-initialized
instance — the confirmed reachability path for Risk Analysis #1.
