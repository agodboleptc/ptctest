# `pimFrictionlessTrialDlg`

**Header**: `pim_ui/includes/pimFrictionlessTrialDlg.h` (76 lines)
**Implementation**: `pim_ui/pim_ui_src/pimFrictionlessTrialDlg.cxx` (334 lines)
**Module**: `pim_ui`

## Purpose

`pimFrictionlessTrialDlg` is the UI for **`pim_rl.exe`'s "Frictionless Trial" /
"Commercial Trial" license-retrieval flow** — a small, focused dialog (not the
main install wizard) that shows the user's host ID, SCN, and application name,
lets them confirm/enter an account email, and drives a background FLEXnet
license-retrieval operation (via the companion `pimFrictionlessTrialLicenseGet`
worker class) to fetch and install a trial license file automatically ("frictionless"
— no manual license-portal steps). It also supports a "cleanup" mode that removes
a previously-configured frictionless trial license instead of installing one.

**Scope note**: both files (header + `.cxx`, 410 combined lines) were read in
full — a complete pass, this class being small enough for exhaustive coverage.
The companion worker class `pimFrictionlessTrialLicenseGet`
(`pim_ui/includes/pimFrictionlessTrialLicenseGet.h`, 95 lines +
`pim_ui/pim_ui_src/pimFrictionlessTrialLicenseGet.cxx`, 748 lines) was also read
in full, since understanding this class's single most important finding (see
Risk Analysis #1) requires tracing exactly what `HeartBeat()` does and confirming
it is never called — but `pimFrictionlessTrialLicenseGet` is documented here only
as far as that finding requires, not as its own independent full class doc (its
`pimUpdatePSF()`/`pimRemovePSF()` PSF-file-rewriting logic, for example, is
described only at a summary level).

## Responsibilities

- Display read-only account/host info (Host ID, SCN, application name — resolved
  from a small hardcoded `TRIAL_OPTNUM` → display-name lookup table) and an
  editable email field.
- On "Renew License"/"Cleanup" button press, kick off
  `pimFrictionlessTrialLicenseGet::Execute()` (which starts a background
  `pimFrictionlessTrialGenerateLoop` — a `pimLoop` subclass, matching the
  well-established `pim_core` background-thread-with-polling pattern documented
  throughout this project) and arm a 1-tick repeating `uiTimer`
  (`LicenseGenerateTimer`) intended to poll it via `OnTimerExpired()`.
- Switch between 2 visually distinct modes in `Initialize()`: normal
  (renew/generate a license) vs. `CLEANUP_ACTION == "Y"` (hide most fields, show
  only a cleanup confirmation + Continue button).
- Provide a `GetAuth()`/`SetURL()` pair matching the same signature used by 2
  sibling classes, `pimAuthDlg` and `rpimDlg` (both documented — or listed for
  future documentation — elsewhere in this project) — but, unlike those 2
  classes, neither method has any confirmed caller for this class (see Risk
  Analysis #3).

## Dependencies

- `pimSessionInfo` — `HOSTID1_PROPERTY`, `TRIAL_OPTNUM`, `CLEANUP_ACTION`,
  `NODE_LOCKED_LICENSE_PATH_PROPERRTY`, `EXISTING_FLEX_LOCATION_PROPERTY`,
  `EXCHANGE_LICENSE_FILE`/security credentials via `SetSecurity()`.
- `pimFrictionlessTrialLicenseGet` — the actual license-retrieval worker; owns a
  `pimFrictionlessTrialGenerateLoop`/`pimFlexInstallLoop`/`pimFlexUpdateLoop`
  (all `pimLoop` subclasses declared in `pim_core/includes/pimFlexAdminInstall.h`,
  not traced in this pass beyond their role as this worker's background engine).
  **`HeartBeat()`'s body (`pim_ui_src/pimFrictionlessTrialLicenseGet.cxx:84-427`)
  is where the license file is actually parsed out of the server response,
  saved to disk, and the local PSF (product-selection-file) is updated via
  `pimUpdatePSF()`/`pimRemovePSF()`** — see Risk Analysis #1 for why this
  matters.
- `pimPTCLicenseGet` — the alternate worker used instead of
  `pimFrictionlessTrialLicenseGet` when `pimGetInHouseMode() == 2` (commercial
  trial) inside `FrictionlessLicenseGenerate()`; not traced in this pass.
- `pimGetInHouseMode()`/`pimSetInHouseMode()` (`pim/pim_src/pimGeneralInit.cxx`)
  — a process-wide `static int inhouse_mode` (default `0`), documented with an
  inline comment: `1` = frictionless mode (`-frictionless_reg`), `2` = trial
  mode (`-trial_reg`). Central to Risk Analysis #1.
- `pimFLEXnet`, `pimRegisterProduct`, `pimPrerequisite`, `pimPTCDotCom`,
  `bs_pro_browser_security`, `pimWindows` — `#include`d; not traced beyond their
  call-site usage in this file.

## Members

| Member | Type | Purpose |
|---|---|---|
| `InfoLay`, `DescLabel`, `CleanupLabel`, `UserNameLabel`, `UserNameInput`, `HostIdLabel`/`HostIdValueLabel`, `SCNLabel`/`SCNValueLabel`, `ApplicationNameLabel`/`ApplicationNameValueLabel`, `HelpLabel`, `ProcessingLabel`, `NoteInfoLabel` | various `uiLabel`/`uiLayout` | static/status display widgets |
| `RenewLicenseButton`, `ContinueButton`, `CleanupButton` | `uiPushButton` | the 3 possible actions |
| `LicenseGenerateTimer` | `uiTimer` | the polling timer at the center of Risk Analysis #1 |
| `initialized` | `bool` | `Initialize()` guard |
| `cleanup_flag` | `bool` | set by `CleanupLicenseFile()`, read once in `FrictionlessLicenseGenerate()` |
| `SessionInfo` | `pimSessionInfo*` | not owned |
| `URL`, `User`, `Password`, `RetryMsg` | `btkWString` | **`URL` is confirmed write-only — see Risk Analysis #3** |

A file-scope `static pimFrictionlessTrialDlg *frictionlessDlg` is set to `this`
in the constructor with the comment "create function to return this to outside
use" — see Risk Analysis #3 for why this never happened.

## Public/Protected APIs

- **`Initialize()`** — idempotent; populates the read-only fields from
  `SessionInfo`, resolves the display application name from a hardcoded
  `TRIAL_OPTNUM` → label chain (`PIM_ORDER_ANSYSLIVE` → "Ansys Live",
  `PIM_ORDER_DEX` → "Design Exploration", etc. — 6 hardcoded product codes,
  matching this codebase's now-familiar pattern of business logic expressed as
  inline string/constant comparisons), and switches the whole dialog into
  cleanup-mode widget visibility if `CLEANUP_ACTION == "Y"`.
- **`Display()`** — **the site of this class's highest-severity confirmed
  finding.** See Risk Analysis #1.
- **`FrictionlessLicenseGenerate()`** — disables both action buttons, then
  branches on `pimGetInHouseMode()`: `== 2` (commercial trial) constructs a
  `pimPTCLicenseGet`; anything else (including the default `0` and the
  frictionless value `1`) constructs a `pimFrictionlessTrialLicenseGet`. Either
  way, calls `->Execute()` then `LicenseGenerateTimer.Set(1, ptr)` to arm the
  polling timer — a timer that, per Risk Analysis #1, is confirmed to never
  actually fire for the only traced real usage.
- **`OnTimerExpired(uiTimer &, void *user_data)`** — the intended poll
  callback: calls `ptr->HeartBeat()` (the ONLY call site for
  `pimFrictionlessTrialLicenseGet::HeartBeat()` anywhere in this archive,
  confirmed by an archive-wide grep), and on completion (success or error)
  disables the action buttons, sets `ContinueButton`'s label to a close-button
  message, and hides `HelpLabel`. Contains a `static int in_timer` reentrancy
  guard that is **confirmed dead** — see Risk Analysis #2.
- **`OnPushButtonActivate(uiPushButton &)`** — `RenewLicenseButton` → generate;
  `CleanupButton` → set `cleanup_flag` then generate; `ContinueButton` → close.
- **`OnClose()`** — `Exit(0)` + `Destroy()`, no cleanup logic beyond that
  (unlike `pimInstallMgrDlg`'s much heavier `OnClose()`).
- **`SetURL(const wchar_t *)`/`GetAuth(btkWString &, btkWString &)`** — see Risk
  Analysis #3.
- **`CleanupLicenseFile()`** — trivial setter for `cleanup_flag`.
- **`SetSessionInfoFile(pimSessionInfo *)`** — inline in the header; trivial
  setter, no validation.

## Private Utilities

- **`Refresh()`** — enables/disables `RenewLicenseButton` based on whether
  `User` is empty. **Confirmed dead: declared, defined, but never called
  anywhere in this file** (grep-confirmed; it is `private`, so no external
  caller is possible either) — see Risk Analysis #3.

## Called By

- `pim/pim_src/pimTop.cxx:1094` — `pimFrictionlessTrialRun(int argc, const char
  *argv[])`, a dedicated top-level Run entry point (alongside
  `pimInstallerRun`/`pimRenewLicenseRun`/others) constructs a stack-local
  `pimFrictionlessTrialDlg Dlg;`, calls `SetSessionInfoFile()`, then
  `Initialize()`, then — based on `argv[6]` being exactly `"-frictionless_reg"`
  or `"-trial_reg"` — calls `pimSetInHouseMode(1)` or `pimSetInHouseMode(2)`
  respectively (`pimTop.cxx:985-986` declares these as
  `frictionless_trial_mode = 1`/`commecial_trial_mode = 2`) immediately before
  `Dlg.Display()`. **This is the only confirmed construction site for this
  class anywhere in the archive** (confirmed via an archive-wide grep for
  `pimFrictionlessTrialDlg` outside its own header/`.cxx`).

## Calls Into

`pimFrictionlessTrialLicenseGet`, `pimPTCLicenseGet`, `pimSessionInfo`, and (via
`FrictionlessLicenseGenerate()`'s worker objects) the `pim_core` FLEXnet
Loop-based install machinery.

## Lifetime

A single instance is constructed as a plain stack-local object inside
`pimFrictionlessTrialRun()`, exactly once per process run of that entry point —
no singleton pattern, no dynamic allocation for the dialog object itself.

## Ownership Model

Does not own `SessionInfo` (borrowed pointer, consistent with every other
`pim_ui` dialog class documented in this effort). Each button press that
triggers license generation heap-allocates a fresh
`pimFrictionlessTrialLicenseGet`/`pimPTCLicenseGet` via `XNew` inside
`FrictionlessLicenseGenerate()`; ownership of that pointer is implicitly handed
to `LicenseGenerateTimer` as its `user_data`, and `OnTimerExpired()` is the only
place that `delete`s it (in both the `Errored()` and success branches) — a
correct, if implicit, ownership handoff, PROVIDED `OnTimerExpired()` actually
runs (see Risk Analysis #1: for the only confirmed real usage, it never does,
so the freshly-`XNew`'d worker object is never `delete`d either — a confirmed
memory leak on top of the confirmed functional bug).

## Thread Safety

No threading primitives in this class itself — UI-thread-only code, consistent
with every other `pim_ui` dialog class in this project. The background license
retrieval work happens inside the separately-documented `pimLoop`-based worker
classes, which have their own (previously documented) threading contract.

## Extension Points

A new trial product code needs an entry added to `Initialize()`'s hardcoded
`TRIAL_OPTNUM` → display-name `if`/`else if` chain — there is no data-driven
lookup table.

## Risk Analysis

1. **CONFIRMED, HIGH-SEVERITY BUG: for the only confirmed real usage of this
   class, `Display()` never runs its event loop, so the license-retrieval
   worker's completion logic is never invoked.**
   `pim_ui/pim_ui_src/pimFrictionlessTrialDlg.cxx:265-270`:
   ```cpp
   if (pimGetInHouseMode()==1 || pimGetInHouseMode() == 2)
   {
       FrictionlessLicenseGenerate();  //OnPushButtonActivate(RenewLicenseButton);
       Sleep(60000);
       return true;
   }
   if (uiDialog::Activate())
       return true;
   ```
   `FrictionlessLicenseGenerate()` starts the background worker
   (`pimFrictionlessTrialLicenseGet::Execute()`, which spawns a
   `pimFrictionlessTrialGenerateLoop` — a `pimLoop` subclass following this
   project's well-documented background-thread contract) and arms
   `LicenseGenerateTimer`. But the very next line is a raw, blocking
   `Sleep(60000)` — **not** `uiDialog::Activate()`, the modal event loop that
   would actually let `LicenseGenerateTimer` fire and call
   `OnTimerExpired()`. `Display()` then unconditionally `return`s `true`.
   An archive-wide grep confirms `pimFrictionlessTrialDlg.cxx:220`
   (`OnTimerExpired()`'s `ptr->HeartBeat()` call) is the **only** call site for
   `pimFrictionlessTrialLicenseGet::HeartBeat()` anywhere in this codebase — and
   `HeartBeat()`'s own body
   (`pim_ui_src/pimFrictionlessTrialLicenseGet.cxx:84-427`) is where the
   retrieved license text is actually parsed, written to disk
   (`SaveToFile`/`OFS << licenseDotDat`), registered with
   `pimSessionInfo::AddLicenseSource()`, and used to update/remove the local
   PSF via `pimUpdatePSF()`/`pimRemovePSF()`. **None of that ever executes** in
   this code path, because `Activate()` — the only thing that could deliver
   timer ticks to `OnTimerExpired()` — never runs.
   **Confirmed reachable, not a rare edge case**: `pimFrictionlessTrialRun()`
   (this class's only confirmed caller) always calls `pimSetInHouseMode(1)` or
   `pimSetInHouseMode(2)` immediately before `Display()` whenever `argv[6]` is
   `"-frictionless_reg"` or `"-trial_reg"` — the 2 argument values this
   dedicated entry point's own name and purpose exist for. (If `argv[6]` were
   some other value, `inhouse_mode` would stay at its process-wide default of
   `0` and the interactive `Activate()` path would run instead — not confirmed
   whether real end-user invocations of this entry point ever pass a 3rd value
   for `argv[6]`, but nothing in the traced code suggests they do.)
   **Consequence**: the background FLEXnet license-retrieval network operation
   may well complete successfully within the 60-second sleep window, but its
   result is simply never read, saved to disk, or applied to the local PSF —
   and the freshly-`XNew`'d `pimFrictionlessTrialLicenseGet`/`pimPTCLicenseGet`
   worker object is never `delete`d either (a confirmed leak on top of the
   functional bug, since only `OnTimerExpired()` frees it). `Display()` reports
   success (`return true`) regardless of what actually happened. This is, by
   evidence and reachability, comparable in severity to the most serious
   confirmed findings elsewhere in this documentation effort (`pimMSILoop`'s
   `fallback_to_msiexec` bug, `pimRegEditLoop`'s double-increment) — the
   difference being that here the entire completion/persistence pipeline is
   skipped, not merely a subset of records.
2. **Confirmed dead reentrancy guard in `OnTimerExpired()`.**
   `static int in_timer = false;` is checked once (`if (in_timer) return
   uiTimerRestart;`) but is **never set to `true`** anywhere in the function —
   every branch either leaves it alone or explicitly resets it to `false`. The
   guard can therefore never actually trigger; it is inert. Low severity on its
   own (and moot in practice given finding #1 above means this function is
   never reached in the only confirmed real usage), but a second, independent
   confirmed defect in the same small function.
3. **3 further confirmed-dead code items, all consistent with this class
   having been adapted from sibling dialogs without being fully wired up**:
   `SetURL()` (and the `URL` member it sets) has zero callers anywhere in this
   archive (confirmed by grep — the only other `SetURL()` call sites in the
   codebase target `pimAuthDlg`/`pimGetNewPimDlg` instances, not this class);
   `GetAuth()` likewise has zero external callers (contrast with its 2 sibling
   implementations on `pimAuthDlg`/`rpimDlg`, which ARE called, from
   `pimAuthDlg.cxx`, on their own respective instances); and the private
   `Refresh()` method is declared and defined but never invoked. The
   `frictionlessDlg` static self-pointer (set in the constructor with an
   explicit "create function to return this to outside use" comment) is a
   3rd instance in this codebase of the same "static self-registering pointer"
   idiom used by `pimInstallMgrDlg`'s `MainUIDialog`/`GetMainUIDialog()` and
   `pimAuthDlg`'s own `AuthDialog` static — but unlike those 2, no accessor
   function was ever actually written for `frictionlessDlg`, so it is
   confirmed to be genuinely unused, not merely under-documented.

## Usage Example

The only confirmed real invocation (`pim/pim_src/pimTop.cxx:1094-1106`):

```cpp
pimFrictionlessTrialDlg Dlg;
Dlg.SetSessionInfoFile(&SessionInfo);

if (Dlg.Initialize())
{
    if ((btkString)argv[6] == "-frictionless_reg")
        pimSetInHouseMode(frictionless_trial_mode);   // 1
    else if ((btkString)argv[6] == "-trial_reg")
        pimSetInHouseMode(commecial_trial_mode);      // 2
    Dlg.Display();   // takes the Sleep(60000) branch -- see Risk Analysis #1
    SessionInfo.Save();
}
```
