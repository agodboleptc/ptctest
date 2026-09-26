# PIM Troubleshooting Guide

> Every playbook below is grounded in mechanisms already traced and cited in Phases
> 1–13 (`docs/01_repository_inventory.md` through `docs/10_error_handling.md`), plus
> evidence from `pim_core/pim_core_src/pimMSILoop.cxx` (MSI exit-code handling,
> originally read at this level in this pass). Where a step relies on something not
> fully confirmed in source, it is marked accordingly rather than presented as
> certain. No source was modified.
>
> **`pimMSILoop` now has its own dedicated full-depth class doc**
> (`docs/classes/pimMSILoop.md`), added in a later extension pass covering both of
> its execution strategies (the `msiexec.exe` child process and the in-process MSI
> API) in full, with a complete member/API/risk breakdown. That pass found a
> **confirmed, high-severity bug**: the MSI-API install path's `fallback_to_msiexec`
> flag is checked at the end of every remaining loop iteration and never reset, so a
> single unparseable `<MSI>` node can cause already-installed packages to be
> silently reinstalled repeatedly. This is now the single highest-severity finding
> across the entire documentation effort — see `docs/classes/pimMSILoop.md`'s Risk
> Analysis and `ai-context/business_rules.yaml`'s
> `pimmsiloop_fallback_causes_repeated_reinstall` before relying on this guide's
> description of MSI install behavior as complete.

## How to Read These Playbooks

For any install problem, first collect these three things — they are the
independent channels documented in `docs/10_error_handling.md` §1, and a real
diagnosis usually needs more than one of them:

1. **The log file** — `pim_installmgr.log[.N]` under `%PIM_LOG_ENV%` if set, else the
   user's My Documents folder (`docs/09_logging_framework.md` §4). Re-run with
   `PIM_DEBUG=1` set in the environment for `LG_T_TRACE`-level detail (caution: this
   also dumps the full process environment into the log — see that doc's §7).
2. **The entitlement's saved product XML** (`ENTITLEMENT_STATUS_STRING` /
   `"status_message"` property, plus `msiexec_return`, `CA_WARNINGS[_WITH_ERROR]`) —
   this is the one channel that survives a crash (`docs/10_error_handling.md` §4).
3. **`pimGetLastError()`'s value**, if the failure happened before/during
   `pimInstallerRun`/`pimFrictionlessTrialRun`/`pimRenewLicenseRun` itself returned —
   compare against the taxonomy in `docs/10_error_handling.md` §2.1 (remembering it's
   process-wide, not entitlement-specific).

---

## 1. MSI Failure

**Symptom**: An entitlement with an MSI component stalls, errors, or the product
appears not installed after "completing."

**Mechanism** (`pim_core/pim_core_src/pimMSILoop.cxx`, confirmed in this pass):
`msiexec`'s process exit code is captured, written to the entitlement's XML as the
`msiexec_return` property, and dispatched through `ProcessErrorCode(int, ...)`:

| Exit code | Meaning (Windows Installer standard codes) | PIM's treatment |
|---|---|---|
| `0` | Success | Treated as success; `<CDSECTION>`/package marked installed |
| `3010` | `ERROR_SUCCESS_REBOOT_REQUIRED` | **Treated as success** — logs a warning (`pimUIInstallStatusErrorMsi_3010`), still records the uninstall command for later use |
| `1641` | `ERROR_SUCCESS_REBOOT_INITIATED` | **Treated as success** — logs `"MSI success; but REBOOT initiated!!!"` |
| `1602`, `1259` | User canceled / AppHelp block | Sets PIM's own cancel flag (`pimLoop::Cancel()`) so the UI reflects a cancellation, not a hard error |
| `1618` | Another install already in progress | Sets cancel flag + specific message (`pimUIInstallStatusErrorMsi_1618`) |
| `1637`, `1633`, `1632`, `1625`, `1619`, `1613`, `1620`, `1603` | Various MSI-specific failures (unsupported platform, unwritable temp, rejected transform, package open failure, unsupported package version, invalid package, general install failure) | Each mapped to its own specific message ID |
| Anything else | Unrecognized | Falls back to a generic `pimUIInstallStatusError_User_msg` |

On **any** non-success code (including 3010/1641's own logging path, though those are
still ultimately treated as success for pipeline continuation), `OnFailure(map)` runs:
it finds the corresponding `<PACKAGE>` XML node and sets its `install` attribute to
`"N"` — **the failed package is deselected going forward**, meaning a naive re-run
without addressing the underlying issue will simply skip it rather than retry it.

**Diagnostic steps**:
1. Check the entitlement's `msiexec_return` XML property or the log for the numeric
   exit code — match it against the table above.
2. If `3010`/`1641`: this is not actually a failure — a reboot is required (or was
   initiated). Reboot and re-run/reconfigure.
3. If `1618`: another MSI install (possibly another PIM entitlement, or an unrelated
   installer) is running concurrently — wait for it to finish and retry.
4. If `1603` (general failure): consult the *Windows Installer's own* verbose log
   (not PIM's log — PIM does not appear to capture `msiexec /l*v` output itself in
   the traced code) for the underlying MSI-level cause.
5. Check whether the corresponding `<PACKAGE install="...">` was set to `"N"` by a
   prior failed attempt (§8, "Incomplete Installation") — this explains "it just
   skips this component now" symptoms.

**Recovery**: No automatic retry was found in the traced code for a genuinely-failed
(non-3010/1641) MSI step — the pipeline stage returns `false` and
`pimEntitlement::OnInstall()` aborts (`docs/04_installation_flow.md` §5). Recovery is
manual: fix the underlying MSI issue, ensure the `<PACKAGE>` node's `install`
attribute is reset (if it was auto-deselected), and re-run.

---

## 2. EXE (SFX) Failure

**Symptom**: A self-extracting-executable-based component fails to install.

**Status**: `pimSFXLoop.cxx` was **not read in this pass** (documented as a gap in
`docs/modules/pim_core.md`). By structural analogy with `pimCopyLoop`/`pimMSILoop`
(same `pimLoop`-subclass pattern, same `Errors`/`Warnings` reporting), a failure would
surface the same way: `GetErrors()`/log lines, and the entitlement's pipeline
aborting at `InstallMSI()`'s sibling step. **Do not assume SFX-specific exit-code
handling mirrors MSI's table above** — this has not been confirmed; treat any
SFX-specific troubleshooting as requiring a fresh read of `pimSFXLoop.cxx` before
relying on specifics beyond "check `Errors`/log for the stage that failed."

---

## 3. Registry Validation Failure

**Symptom**: A product-declared registry key/value isn't created, or the install
aborts during the registry stage.

**Mechanism** (`docs/07_registry_usage.md` §2): `pimRegEditLoop::Create()` returns
`false` in exactly two situations: (a) the `<REGISTRY>` element's key-root token isn't
one of the 4 recognized literals `[HKCR]`/`[HKLM]`/`[HKCU]`/`[HKU]` (silent failure,
no specific diagnostic), or (b) the underlying `win32RegCreateKeyExA` call itself
fails (permissions, invalid characters in the path, etc.).

**Diagnostic steps**:
1. Confirm the entitlement's product XML `<REGISTRY>` elements use one of the 4 valid
   root tokens exactly (case-sensitive literal match, no typo tolerance).
2. Check whether PIM is running with sufficient privileges to write to
   `HKEY_LOCAL_MACHINE` (most product registry entries target this hive per the
   examples traced) — a non-elevated run would produce a Win32 access-denied failure
   here with only a generic step-failure surfaced to PIM's own error channel.
3. Check the `<REGISTRY>`'s `id` (subkey path) for unresolved property placeholders
   (e.g. a literal `[LP]` string instead of an expanded path) — `pimConvert` failures
   to resolve a placeholder would produce an invalid registry path silently passed to
   the Win32 API.
4. Cross-check `ApplyRegistryChanges()`'s cancel-handling: if the install appears
   "stuck" rather than failed outright, remember `pimRegEdit` is a **process-wide
   singleton** (`docs/04_installation_flow.md` §7) that serializes registry
   operations across every concurrently-installing entitlement — a stuck/slow
   registry operation in *one* entitlement can make `Create()` appear to hang for
   *all* others waiting on the singleton (`Create()`'s busy-retry loop,
   `docs/04_installation_flow.md` §5).

**Recovery**: Fix the XML (root token/path) or the privilege context, then re-run.
Rollback of partial registry writes goes through the same singleton's `Uninstall()`
path (`docs/04_installation_flow.md` §6).

---

## 4. Version Mismatch

**Symptom**: PIM reports a prerequisite or a component as needing a newer version
that appears to already be installed, or refuses to proceed for version reasons.

**Mechanism**: Version comparisons are done via `pimCmpDottedVersions(installed,
required) != -1` throughout (`docs/05_prerequisite_framework.md` §4) — a dotted
version-string comparator, not a semantic-version library. Two confirmed rules
depend on it: `pimCreoTestPlatformAgent` (registry-read Creo Agent/Platform Services
versions) and `pimWGMTestVFS` (MSI upgrade-code-based installed-version lookup via
`pimGetVersionInstalled`).

**Diagnostic steps**:
1. Identify which rule is failing (`docs/05_prerequisite_framework.md` §4's two
   tables) from the log (`enable_logs=true` calls emit `pimUIPreReqVerNeeded`/
   `pimUIPreReqNeeded` messages naming the missing component and required version).
2. For `pimCreoTestPlatformAgent`: manually inspect
   `HKLM\SOFTWARE\PTC\CreoAgent\AgentPackageVersion` and the platform-services
   equivalent value — confirm the installed version string's dotted format actually
   compares as ≥ the XML-declared `creoagent_version`/`creosvcs_version` attributes.
   Remember this registry path/value set has been revised almost every Creo release
   (`docs/05_prerequisite_framework.md` §4's Risk note) — a version mismatch after a
   Creo upgrade may indicate the *rule itself* needs updating for a new key/value
   name, not that the installed software is genuinely outdated.
3. For `pimWGMTestVFS`: confirm the MSI upgrade code in the product XML's
   `<DISTRIBUTION>/<MSI upgradecode="...">` matches what's actually registered for
   the installed Windchill File System product — a mismatched upgrade code will
   report "not installed" even when a (differently-coded) version is present.

---

## 5. Prerequisite Failure

**Symptom**: An entitlement never starts installing, or fails with
`PIM_PREQUISITE_NOT_SATISFIED` (-609).

**Mechanism**: Fully documented in `docs/05_prerequisite_framework.md`. Two failure
shapes:
- **Gate failure**: `OnExecute()` returns immediately if `HasPrerequisite() &&
  !HasPrerequisitesInstallSucceeded()` — nothing runs, no error is raised at this
  point; it's a "not yet" state, not a failure (see §6, Pending Prerequisite State).
- **Hard failure after the fact**: inside `HasPrerequisitesInstallSucceeded()`'s wait
  loop, a prerequisite that reports done but whose `IsPrerequisiteSatisfied(true)`
  re-check still fails, **and is not a soft prerequisite**, pushes
  `PIM_PREQUISITE_NOT_SATISFIED` and aborts the dependent's install.

**Diagnostic steps**:
1. Check whether the prerequisite is registered soft or hard
   (`IsSoftPrerequisite`) — a soft prerequisite failing its check is **not** supposed
   to block the dependent; if it did, that would itself be worth escalating as a
   possible bug (the soft-downgrade logic is a direct string/tag match against
   `SoftPreRequisites`, so a tag mismatch could cause a soft prerequisite to be
   treated as hard).
2. Follow the "Version Mismatch" playbook (§4) if the specific failing rule is
   `pimCreoTestPlatformAgent`/`pimWGMTestVFS`.
3. Confirm the product XML's `<IS_INSTALLED_FUNC>` text content exactly matches one
   of the two dispatched function names (`docs/05_prerequisite_framework.md` §4) — an
   unrecognized name silently evaluates to "never satisfied" with **no diagnostic at
   all**, which looks identical in symptom to a genuinely failing check.

---

## 6. Pending Prerequisite State

**Symptom**: An entitlement's status is stuck at "pending prerequisite"
(`pimUIInstallStatusPendingPrerequisite`) for a long time.

**Mechanism**: `HasPrerequisitesInstallSucceeded()` busy-waits (`Sleep(1)` per pass,
`docs/05_prerequisite_framework.md` §5) until every prerequisite is done. This is
expected/normal while a prerequisite is genuinely still installing. It becomes a
problem if:
- The prerequisite entitlement's own thread is itself stuck (diagnose the
  *prerequisite's* status/log independently — the dependent's "pending" status gives
  no detail about what the prerequisite is doing).
- The prerequisite was never actually started (`StartOrRestartInstall()` never
  called on it) — in silent mode this is the caller's (`InstallPreReqSilent`)
  responsibility; confirm it was invoked for every unsatisfied prerequisite returned
  by `GetNextPrerequisite`.
- **Cancellation was requested on the dependent but not the prerequisite**
  (`docs/05_prerequisite_framework.md` §5's `//ptr->Cancel();` dead-code note) — if a
  user cancels the *dependent* while waiting, the *prerequisite* keeps running
  untouched; the dependent's own wait loop exits, but the prerequisite entitlement
  may still be mid-install when the overall session appears to have stopped.

**Recovery**: Identify and directly address the prerequisite entitlement's own state;
there is no timeout in `HasPrerequisitesInstallSucceeded()` — it waits indefinitely
short of cancellation.

---

## 7. Verification Failure

**Status**: As established in `docs/04_installation_flow.md` §8 and
`docs/06_entitlement_framework.md` §5, **no distinct post-install verification stage
exists** in the traced code. What looks like "verification failing" is most likely
one of:
- A same-version check (`IsMSISameVersionInstalled`) incorrectly deciding a
  reinstall is/isn't needed — check the actual installed MSI product/version against
  what the check compares.
- A prerequisite re-check failing after its dependent claimed done (§5 above) — this
  *is* the closest thing to "verification" in this codebase.
- The `CA_WARNINGS_WITH_ERROR` property causing a "Complete with Errors" status
  despite the main pipeline stages all returning success
  (`docs/06_entitlement_framework.md` §4b) — check what custom action set this
  property and why.

---

## 8. Incomplete Installation

**Symptom**: Some but not all components of a product got installed.

**Diagnostic steps**:
1. Check each `<CDSECTION>`'s `installed` attribute and each `<PACKAGE>`'s `install`
   attribute in the saved product XML — a component whose MSI failed has its
   `<PACKAGE install="N">` set by `pimMSILoop::OnFailure` (§1 above), which is a
   **persistent** marker that will cause it to be skipped on subsequent runs unless
   explicitly reselected/reset.
2. Check `total_pct_done` / `execute_status_string` (or their last-saved
   `ENTITLEMENT_STATUS_STRING` value) to identify which pipeline stage the
   entitlement stopped at (`docs/06_entitlement_framework.md` §4).
3. Check for a stage-skip flag (`NoCopyStep`, `NoRegistryActions`, `NoScripts`,
   `NoServices`, `NoPSF`, `NoShortcuts`, `NoPreCopyMSI`, `NoPostCopyMSI`) accidentally
   set on the product XML — these are legitimate, XML-declared ways for a stage to be
   intentionally skipped, and can be mistaken for a failure if undocumented for that
   specific product (`docs/04_installation_flow.md` §2).

---

## 9. Interrupted Installation

**Symptom**: PIM was killed, crashed, or the machine lost power mid-install.

**Diagnostic steps**:
1. The **persistent XML status** (`docs/10_error_handling.md` §4) is the only
   channel that survives this — read the entitlement's saved product XML's
   `status_message` property directly to see the last known stage.
2. Check for a stuck **write lock** on the XML itself
   (`pimXmlFile::WriteLockEngaged()`) — `pimLoop::Kill()` is designed to release this
   if PIM's own `Kill()` path was used, but an external process kill (task manager,
   power loss) bypasses that cleanup entirely, so a subsequent run reading the same
   XML could behave unpredictably if the lock state is somehow persisted (not
   confirmed either way in this pass — `pimXmlFile`'s locks are almost certainly
   in-memory/per-process rather than file-based, but this should be verified before
   ruling it out as a cause of a "won't reopen" symptom).
3. Recall `docs/04_installation_flow.md` §5's finding: **`ROLLBACK_CANCELLED_INSTALL`
   is not defined anywhere in this archive.** An interrupted (not gracefully
   canceled) install very likely leaves the machine in a partial state with **no
   automatic rollback attempted at all** — treat this as the expected behavior, not a
   bug, unless the actual build defines that macro.

**Recovery**: Re-running the installer against the same XML should resume from
wherever the pipeline's `NoXxx`/`installed=`/`install=` state indicates it left off,
per the per-stage skip logic (§8) — but there is no dedicated "resume" mode
distinct from a normal re-run in the traced code.

---

## 10. Rollback Failure

**Symptom**: `OnRollback()` itself doesn't cleanly undo a failed/canceled install.

**Diagnostic steps**:
1. Confirm whether rollback was actually triggered at all — given the
   `ROLLBACK_CANCELLED_INSTALL` finding (§9), a canceled-mid-install scenario may
   never call `OnRollback()` in the first place; don't assume rollback ran just
   because the install failed.
2. For registry rollback specifically: `OnRollback()` uses the same **singleton**
   `pimRegEdit::GetInstance().Uninstall(xmlPtr)` path as the generic registry
   mechanism (`docs/04_installation_flow.md` §7) — if a *different* entitlement's
   registry operation is in progress on the singleton, this rollback step will
   itself block/retry, same as a forward install would (§3 above's serialization
   note).
3. The middle portion of `OnRollback()`'s sequence (between `UninstallShortcuts()`
   and the registry-singleton call) was **not individually traced line-by-line** in
   this pass (`docs/04_installation_flow.md` §6) — if a rollback failure is suspected
   in a stage other than shortcuts or registry, this is the point where a fresh,
   targeted read of `pimEntitlement.cxx`'s `OnRollback()` body (source lines
   ~7287–7567) would be needed before further diagnosis.

---
*Phase 14 of the requested 20-phase documentation set. No source was modified.*
