# `rpimDlg`

**Header**: `pim_ui/includes/rpimDlg.h` (63 lines)
**Implementation**: `pim_ui/pim_ui_src/rpimDlg.cxx` (288 lines)
**Module**: `pim_ui`

## Purpose

`rpimDlg` is the UI for `pim_re.exe`'s "Renew License" flow — a small,
dedicated dialog (not the main install wizard) that prompts for a PTC.com
account username/password, shows the resolved host ID, and drives a background
FLEXnet license-renewal operation (via the companion `pimPTCRenewLicenseGet`
worker class) to fetch and install a renewed node-locked or license-server
license file.

**Scope note**: both files (header + `.cxx`, 351 combined lines) were read in
full — a complete pass, this class being small enough for exhaustive coverage.
The companion worker class `pimPTCRenewLicenseGet`
(`pim_ui/includes/pimPTCRenewLicenseGet.h`, 95 lines) was located but **not**
read in depth this pass, unlike the analogous
`pimFrictionlessTrialLicenseGet` worker traced in full while documenting
`pimFrictionlessTrialDlg` — that deep trace was necessary there specifically
because `pimFrictionlessTrialDlg::Display()` has a confirmed bug that skips
its dialog's modal event loop. `rpimDlg::Display()` has no such bug (see
Risk Analysis #1), so `pimPTCRenewLicenseGet`'s own internals were not
independently verified here.

This class is the closest sibling in this codebase to `pimFrictionlessTrialDlg`
(documented previously in this effort) — structurally near-identical
(`LicenseGenerateTimer`, `RenewLicenseButton`/`ContinueButton`, `SetURL()`/
`GetAuth()`, a private `Refresh()`, an `OnTimerExpired()` with the same dead
reentrancy-guard shape) and predates it by 3 years (`rpimDlg`: created
2015-09-29 per `$$1`; `pimFrictionlessTrialDlg`: created 2018-11-27 per its
own `$$1`) — strongly suggesting `pimFrictionlessTrialDlg` was cloned from
`rpimDlg` (itself likely cloned from the older `pimAuthDlg`, see Dependencies).
This lineage is directly useful evidence, not just trivia: comparing the two
siblings' `Display()` implementations side-by-side is what confirms
`pimFrictionlessTrialDlg`'s `Sleep(60000)` bug is a genuine, avoidable defect
introduced when adapting this class's correct pattern, not an unavoidable
characteristic of the shared design — see Risk Analysis #1.

## Responsibilities

- Prompt for a username/password (2 `uiInputPanel`s) and enable
  `RenewLicenseButton` only once both are non-empty (via `Refresh()`, called
  from `OnInputPanelInput()`).
- Resolve and display the host ID for whichever license (node-locked file or
  license-server `license.dat`) is being renewed, determined in `Initialize()`
  by pattern-matching the `NODE_LOCKED_LICENSE_PATH_PROPERRTY` session
  property: a `"*:*"` value (a drive-letter path) is treated as a node-locked
  file via `pimNeedsLocalNodeLicenseRenew()`; a `"*@*"` value (a
  server-style address) is treated as a license-server renewal via
  `pimNeedsLocalLicenseServerRenew()` against a hardcoded
  `C://Program Files//PTC//FLEXnet Admin License Server//licensing//license.dat`
  path.
- On "Renew License" button press, disable all input, start a background
  `pimPTCRenewLicenseGet` worker's FLEXnet renewal operation, and arm a 1-tick
  polling `uiTimer`.
- Provide a "Reset Password" button that simply opens `RESET_PWD_LINK` in the
  system browser (`pimSystemURL()`) — no in-dialog logic.

## Dependencies

- `pimSessionInfo` — `NODE_LOCKED_LICENSE_PATH_PROPERRTY`, `HOSTID1_PROPERTY`,
  `EXISTING_FLEX_LOCATION_PROPERTY`, credentials via `SetSecurity()`.
- `pimPTCRenewLicenseGet` — the license-renewal worker (analogous to
  `pimFrictionlessTrialLicenseGet`/`pimPTCLicenseGet`, documented while
  covering `pimFrictionlessTrialDlg`); **not traced in depth this pass** (see
  Scope note) — its `HeartBeat()`/`Errored()`/`Execute()`/
  `InstallServerIfNecessary()`/`SetProcessingLabel()`/`InitProcessingLabel()`
  are used with the exact same call shape as the analogous
  `pimFrictionlessTrialLicenseGet` methods.
- `pimAuthDlg` — **the likely original template both this class and
  `pimFrictionlessTrialDlg` were partially copied from.** `pimAuthDlg`
  declares the identical `btkWString URL, User, Password, RetryMsg;` member
  set and a `GetAuth()`/`SetURL()` pair, but — unlike `rpimDlg` and
  `pimFrictionlessTrialDlg` — actually wires up `RetryMsg` end-to-end
  (`SetRetryMsg()` → `AuthRetryLabel.SetWLabel(RetryMsg)`, confirmed in
  `pim_ui_src/pimAuthDlg.cxx:98-173`). See Risk Analysis #3.
- `pimFLEXnet`, `pimRegisterProduct`, `pimPrerequisite`, `pimPTCDotCom`,
  `bs_pro_browser_security`, `pimWindows`, `pimScramble` — `#include`d; not
  traced beyond their call-site usage in this file.

## Members

| Member | Type | Purpose |
|---|---|---|
| `UserNameLabel`/`UserNameInput`, `PwLabel`/`PwInput`, `EmailLabel` | `uiLabel`/`uiInputPanel` | credential entry |
| `ResetPWButton`, `RenewLicenseButton`, `ContinueButton` | `uiPushButton` | the 3 actions |
| `HostIdValueLabel`, `ProcessingLabel` | `uiLabel` | status display |
| `LicenseGenerateTimer` | `uiTimer` | polling timer — correctly driven here (contrast with `pimFrictionlessTrialDlg`, Risk Analysis #1) |
| `initialized` | `bool` | `Initialize()` guard |
| `SessionInfo` | `pimSessionInfo*` | not owned |
| `URL` | `btkWString` | **confirmed write-only — see Risk Analysis #2** |
| `User`, `Password` | `btkWString` | populated live from `OnInputPanelInput()`, read by `RenewLiceneGenerate()`/`GetAuth()`/`Refresh()` |
| `RetryMsg` | `btkWString` | **confirmed fully dead — declared, never read or written anywhere in this class** — see Risk Analysis #3 |

A file-scope `static rpimDlg *rDlg` is set to `this` in the constructor with
the comment "create function to return this to outside use" — never read
anywhere; see Risk Analysis #2.

## Public/Protected APIs

- **`Initialize()`** — idempotent; resolves node-locked-vs-server renewal mode
  from `NODE_LOCKED_LICENSE_PATH_PROPERRTY` (see Responsibilities), shows a
  `MessageBoxW` "Hostid mismatch." and returns `false` if the corresponding
  `pimNeedsLocal*Renew()` check fails. **If the property matches neither
  `"*:*"` nor `"*@*"` (e.g. empty, meaning no renewable license path was
  found), neither branch runs, `hostId` stays empty, and `Initialize()`
  proceeds anyway** with no error message — see Risk Analysis #4.
- **`Display()`** — `uiDialog::Display()` → `uiDialog::Activate()` → return
  `true`; on failure, resets `initialized = false`. **This is the CORRECT
  pattern — contrast directly with `pimFrictionlessTrialDlg::Display()`'s
  confirmed bug** (see Risk Analysis #1).
- **`RenewLiceneGenerate()`** *(sic — missing 's' in "License", a naming typo
  present in both the header and `.cxx`)* — disables all input, `XNew`'s a
  `pimPTCRenewLicenseGet`, calls `InstallServerIfNecessary()`/`Execute()`,
  arms `LicenseGenerateTimer`.
- **`OnTimerExpired(uiTimer &, void *user_data)`** — the poll callback:
  calls `ptr->HeartBeat()`; on `Errored()`, re-enables all input fields and
  clears the 2 text fields so the user can retry; on success, leaves fields
  disabled (no further action needed, `ContinueButton` is now the only
  active control). Both branches `delete ptr`. Contains the same
  **confirmed-dead** `static int in_timer` reentrancy guard found in
  `pimFrictionlessTrialDlg::OnTimerExpired()` — see Risk Analysis #5.
- **`OnPushButtonActivate(uiPushButton &)`** — `RenewLicenseButton` →
  generate; `ResetPWButton` → open `RESET_PWD_LINK` via `pimSystemURL()`;
  `ContinueButton` → close.
- **`OnInputPanelInput(uiInputPanel &)`** — copies whichever input panel
  changed into `User`/`Password`, then calls `Refresh()`.
- **`OnClose()`** — `Exit(0)` + `Destroy()`, no extra cleanup.
- **`SetURL(const wchar_t *)`/`GetAuth(btkWString &, btkWString &)`** —
  see Risk Analysis #2.

## Private Utilities

- **`Refresh()`** — enables `RenewLicenseButton` only when both `User` and
  `Password` are non-empty. **Confirmed actively used** (called from
  `OnInputPanelInput()`) — unlike `pimFrictionlessTrialDlg::Refresh()`, which
  was confirmed fully dead. This is a useful negative-comparison data point:
  the 2 sibling classes' otherwise-near-identical `Refresh()` methods are NOT
  uniformly vestigial — `rpimDlg`'s is load-bearing.

## Called By

- `pim/pim_src/pimTop.cxx:1190` — `pimRenewLicenseRun(int argc, const char
  *argv[])`, a dedicated top-level Run entry point (alongside
  `pimInstallerRun`/`pimFrictionlessTrialRun`/others), constructs a
  stack-local `rpimDlg Dlg;`, calls `SetSessionInfoFile()`, `Initialize()`,
  then unconditionally `Dlg.Display()` — no in-house-mode branch, no
  alternate code path. **This is the only confirmed construction site for
  this class anywhere in the archive** (confirmed via an archive-wide grep
  for `rpimDlg` outside its own header/`.cxx`).

## Calls Into

`pimPTCRenewLicenseGet`, `pimSessionInfo`, and (via `RenewLiceneGenerate()`'s
worker object) the `pim_core` FLEXnet Loop-based install machinery
(not traced in depth this pass, per Scope note).

## Lifetime

A single instance is constructed as a plain stack-local object inside
`pimRenewLicenseRun()`, exactly once per process run of that entry point — no
singleton pattern, no dynamic allocation for the dialog object itself.

## Ownership Model

Does not own `SessionInfo` (borrowed pointer). Each "Renew License" click
heap-allocates a fresh `pimPTCRenewLicenseGet` via `XNew` inside
`RenewLiceneGenerate()`; ownership is implicitly handed to
`LicenseGenerateTimer` as its `user_data`, and `OnTimerExpired()` — which,
unlike `pimFrictionlessTrialDlg`'s equivalent, IS confirmed to actually run
via `Display()`'s correct `Activate()` call — `delete`s it in both the
`Errored()` and success branches. This is a correct, complete ownership
handoff with no confirmed leak, in contrast to
`pimFrictionlessTrialDlg`'s confirmed leak of the analogous object.

## Thread Safety

No threading primitives in this class itself — UI-thread-only code, consistent
with every other `pim_ui` dialog class in this project. Background renewal
work happens inside the separately-scoped `pimPTCRenewLicenseGet`/`pimLoop`
worker classes.

## Extension Points

None specific to this class beyond the general `pim_ui` dialog pattern — it is
small and single-purpose.

## Risk Analysis

1. **`Display()` is CORRECT here — the direct comparison confirming
   `pimFrictionlessTrialDlg`'s bug is a real defect, not an inherent
   limitation.** `rpimDlg::Display()`
   (`pim_ui/pim_ui_src/rpimDlg.cxx:204-213`) is:
   ```cpp
   bool rpimDlg::Display()
   {
       if (uiDialog::Display())
       {
           if (uiDialog::Activate())
               return true;
           initialized = false;
       }
       return false;
   }
   ```
   — a plain, unconditional `Activate()` call, with no mode-dependent branch
   and no `Sleep()`. `LicenseGenerateTimer` therefore reliably fires and
   `OnTimerExpired()` reliably runs `HeartBeat()` to completion for this
   class. Since `rpimDlg` and `pimFrictionlessTrialDlg` share the same
   `LicenseGenerateTimer`/worker-polling design (and, per the class-lineage
   evidence above, `pimFrictionlessTrialDlg` was very likely cloned from this
   class or a close relative), this is strong corroborating evidence that
   `pimFrictionlessTrialDlg::Display()`'s `Sleep(60000)`-instead-of-`Activate()`
   branch (see `docs/classes/pimFrictionlessTrialDlg.md` Risk Analysis #1) was
   a mistake introduced specifically for the in-house-mode case, not a
   necessary consequence of this shared dialog family's design.
2. **Confirmed dead: `SetURL()`, the `URL` member, and `GetAuth()` have zero
   callers anywhere in this archive.** An archive-wide grep for
   `.SetURL(`/`->SetURL(` finds only `pimAuthDlg`'s own calls (on its own
   `AuthDialog` instance) and one unrelated call on a `pimGetNewPimDlg`
   instance in `pimTop.cxx` — none targets an `rpimDlg` instance. `GetAuth()`
   is similarly never called on an `rpimDlg` instance (contrast with
   `pimAuthDlg::GetAuth()`, which IS called, repeatedly, from
   `pimAuthDlg.cxx` on its own instance). The file-scope `rDlg` static
   self-pointer (constructor comment: "create function to return this to
   outside use") likewise has no accessor function anywhere and is never
   read — the same never-completed "static self-registering pointer" idiom
   confirmed dead in `pimFrictionlessTrialDlg`'s `frictionlessDlg` static.
3. **Confirmed dead: the `RetryMsg` member is declared but never read or
   written anywhere in this class** — not even a `SetRetryMsg()` method
   exists on `rpimDlg` (unlike `pimAuthDlg`, which fully wires `RetryMsg`
   through to a visible `AuthRetryLabel`). Combined with finding #2, this
   supports treating `rpimDlg` (and `pimFrictionlessTrialDlg`) as partial,
   never-fully-adapted copies of `pimAuthDlg`'s original dialog template —
   a documentation-methodology point worth remembering: shared member
   declarations across sibling classes are not evidence that all of them
   are equally wired up.
4. **`Initialize()` has no error path for the case where
   `NODE_LOCKED_LICENSE_PATH_PROPERRTY` matches neither `"*:*"` nor
   `"*@*"`.** Both recognized patterns show a `MessageBoxW` and return
   `false` on a host-ID mismatch, but if the property is simply absent or in
   an unrecognized format, `Initialize()` silently proceeds with an empty
   `hostId` and returns `true` (`initialized = true`). Not confirmed to be
   reachable via the only traced construction site without further tracing
   `pimIsNodeLicenseRenewMode()`'s own conditions (not read this pass) — an
   open question, not an asserted bug, per this project's "no invented
   behavior" rule.
5. **Confirmed dead reentrancy guard in `OnTimerExpired()`** — identical in
   shape to the confirmed-dead guard in `pimFrictionlessTrialDlg::OnTimerExpired()`:
   `static int in_timer = false;` is checked (`if (in_timer) return
   uiTimerRestart;`) but never set to `true` anywhere in the function. A 2nd
   confirmed instance of this exact defect pattern across the 2 sibling
   classes, strongly suggesting it was copy-pasted from one to the other (or
   from a shared, now-untraceable common ancestor) without ever being
   completed in either.
6. **Naming typo**: the method is `RenewLiceneGenerate()` (missing an "s" in
   "License") in both the header and `.cxx` — cosmetic only, noted for
   completeness/searchability, not a behavioral issue.

## Usage Example

The only confirmed real invocation (`pim/pim_src/pimTop.cxx:1190-1198`):

```cpp
rpimDlg Dlg;
Dlg.SetSessionInfoFile(&SessionInfo);
if (Dlg.Initialize())
{
    Dlg.Display();   // correctly runs uiDialog::Activate() -- see Risk Analysis #1
    SessionInfo.Save();
}
```
