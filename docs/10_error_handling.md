# PIM Error Handling Framework

> Traced from `pim/includes/pimExit.h` (full, 49 lines), `pim/pim_src/pimExit.cxx`
> (full, 35 lines), plus the error/exception patterns already documented in
> `docs/classes/pimLoop.md` (Phase 5), `docs/04_installation_flow.md` (Phase 7),
> `docs/05_prerequisite_framework.md` (Phase 8), and `docs/06_entitlement_framework.md`
> (Phase 9). No source was modified.

## 1. Four Independent Error/Status Channels

PIM does not have one unified error-handling mechanism — it has (at least) **four
channels**, each with a different scope, lifetime, and consumer, that a full
diagnosis of any failure must cross-reference:

| # | Channel | Scope | Lifetime | Consumer |
|---|---|---|---|---|
| 1 | Process-wide error-code stack (`pimHitError`/`pimGetLastError`) | Whole process | Until `pimClearErrors()` | The `pim*Run` functions' return value (`pim/pim_src/pimTop.cxx`) |
| 2 | Per-Loop `Errors`/`Warnings` text buffers (`pimLoop`) | One install step / one entitlement | Reset on each `Execute()` | UI progress display (`GetErrors`/`GetWarnings`), silent-mode logging |
| 3 | Persistent XML status property (`ENTITLEMENT_STATUS_STRING`) | One entitlement, saved to disk | Survives process restart (until overwritten) | `GetInstallStep()`, any tool re-reading the saved product XML |
| 4 | `CA_WARNINGS`/`CA_WARNINGS_WITH_ERROR` XML properties | One entitlement's custom-action run | Set during `OnInstallCustomActions`, read once at pipeline end | Final status message selection (`docs/06_entitlement_framework.md` §4b) |

## 2. Channel 1: The Process-Wide Error-Code Stack

`pim/includes/pimExit.h` / `pim/pim_src/pimExit.cxx`:

```cxx
static intXArray Errors;   // dsXArray<int>, module-file-local

void pimHitError(int e)      { Errors += e; }
void pimClearErrors()        { Errors.Clear(); }
int  pimGetLastError()       { return Errors.IsEmpty() ? PIM_SUCCESS : Errors[Errors.GetSize()-1]; }
```

This is a **simple append-only stack of integer codes**, not tied to any particular
entitlement or thread — any code anywhere in the process can call `pimHitError()`, and
`pimGetLastError()` always returns the most recently pushed code (or `PIM_SUCCESS =
0` if none). It is explicitly reset once per run
(`pimClearErrors()` is called near the top of `pimInstallerRun`/
`pimFrictionlessTrialRun`, `docs/03_application_startup.md`). Because it's a single
global stack shared across every entitlement's own thread, **a code pushed by one
entitlement's background thread is indistinguishable, at read time, from one pushed by
another** — `pimGetLastError()` only tells you *that* something failed most recently,
not *which* entitlement.

### 2.1 The Complete Error-Code Taxonomy (`pim/includes/pimExit.h`)

Codes are grouped into hundred-ranges by subsystem, with a "general" code at each
range's start and specific codes below it — an explicit, source-documented
convention:

| Range | General code | Specific codes | Meaning |
|---|---|---|---|
| 0 | `PIM_SUCCESS = 0` | — | No error |
| -1xx | *(none defined here)* | -100, -101, -102, -103 (documented only in comments, **not** `#define`d — these belong to the external, not-included launcher `pim_setup.cxx`) | -100: "in use pim_setup.cxx"; -101: user canceled (debug item); -102: `pim_rm.exe` failed to copy itself to temp space; -103: `installmgr.dll` (the `pim` module) could not be loaded/found |
| -3xx | `PIM_GENERAL_LIBRARY_ERROR = -300` | `PIM_INITIALIZE_LIBRARY_ERROR -301`, `PIM_BAD_INPUTS_ERROR -310`, `PIM_INSTALLER_DOES_NOT_SUPPORT -312`, `PIM_INPUT_OUT_OF_DATE -313`, `PIM_INSTALLER_OUT_OF_DATE -314` | Library/general-init and input-compatibility errors |
| -4xx | `PIM_GENERAL_COMMS_ERROR = -400` | `PIM_INITIALIZE_COMMS_ERROR -401`, `PIM_PTC_IT_COMMS_ERROR -411` | Browser/credentials init and PTC.com network reachability |
| -5xx | `PIM_GENERAL_XML_ERROR = -500` | `PIM_INITIALIZE_XML_ERROR -501` | Xerces init and general XML errors (also reused for the 32/64-bit platform-mismatch abort in `pimInstallerRun`, `docs/03_application_startup.md` §3) |
| -6xx | `PIM_GENERAL_DOWNLOAD_ERROR = -600` | `PIM_SOFTWARE_NOT_FOUND -603`, `PIM_PREQUISITE_NOT_SATISFIED -609`, `PIM_INSTALL_ERROR_SEE_LOG -610`, `PIM_UNINSTALL_ERROR_SEE_LOG -620` | Download/install/uninstall/prerequisite failures — note `-609` is the exact code pushed by `HasPrerequisitesInstallSucceeded()`'s hard-failure path (`docs/05_prerequisite_framework.md` §5) |
| -7xx | `PIM_GENERAL_SOFTWARE_ERROR = -700` | `PIM_INITIALIZE_UI_ERROR -701` | UI/general software errors |
| -8xx | `PIM_GENERAL_PERMISSIONS_ERROR = -800` | *(no specific subcodes defined)* | Reserved for permissions errors; no code in this archive was observed pushing this range specifically |

**Only two of these codes were confirmed, in this pass, as actually pushed via
`pimHitError()` from traced call sites**: `PIM_INITIALIZE_UI_ERROR` (multiple call
sites in `pim/pim_src/pimTop.cxx`) and `PIM_PREQUISITE_NOT_SATISFIED` (in
`pimEntitlement::HasPrerequisitesInstallSucceeded()`). `PIM_GENERAL_XML_ERROR` is
also confirmed used, but via direct `return -104` control flow in some
`pimInstallerRun` branches rather than `pimHitError` (see §2.2 note below) — the two
mechanisms (function return value vs. the global stack) are not always used together
even for what looks like the same conceptual error. The remaining codes are declared
but their push sites were not individually located in this pass.

### 2.2 Inconsistency Note: Return Value vs. Global Stack

`pimInstallerRun`'s 32/64-bit platform-mismatch checks (`docs/03_application_startup.md`
§3) do **both** `pimHitError(PIM_GENERAL_XML_ERROR)` **and** `return -104` — a literal
`-104`, which is **not** one of the `#define`d codes in `pimExit.h` at all (the
comment block only documents `-100` through `-103` as launcher-reserved, and `-104` is
unlisted anywhere). This means the function's actual return value in that specific
failure path does not match any named constant, while the global error stack
separately records a different, named code (`PIM_GENERAL_XML_ERROR`, -500) for the
same event. A caller inspecting only the return value and a caller inspecting only
`pimGetLastError()` would see two different, uncorrelated numbers for the identical
failure.

## 3. Channel 2: Per-Loop `Errors`/`Warnings` Buffers

Fully documented in `docs/classes/pimLoop.md`. Key error-handling-specific points:

- **Unstructured text, not codes** — `AppendError`/`AppendWarning` accept either a
  literal string or a message-catalog ID (`docs/09_logging_framework.md` §5), always
  producing plain newline-joined text with no severity/category metadata attached at
  the buffer level (severity is implied only by which buffer — `Errors` vs.
  `Warnings` — text was appended to, and optionally by an ad hoc `prefix_label`).
- **Mutex-guarded but not thread-attributed** — multiple sub-steps could in principle
  append to the same entitlement's buffers from different call sites within the same
  thread (sub-steps run sequentially, not concurrently, within one entitlement's
  thread, per `docs/04_installation_flow.md`), so this is safe in practice, but the
  buffer itself carries no record of *which* stage produced which line beyond
  whatever the caller wrote into the text.
- **Reset on `Execute()`, not on read** — `GetErrors`/`GetWarnings` are non-destructive
  reads; a caller must track "have I already shown this" itself if polling
  repeatedly during a long-running install.

## 4. Channel 3: Persistent XML Status (`ENTITLEMENT_STATUS_STRING`)

Documented fully in `docs/06_entitlement_framework.md` §4b. The key error-handling
property: because this is written to the entitlement's own XML file
(`xmlPtr->DoSave()`) at nearly every pipeline stage, **it survives a process crash or
restart** in a way the in-memory `pimLoop` buffers and the process-wide error stack do
not — if PIM is killed mid-install, the last-saved status string is the only
after-the-fact record of how far it got, from among these three channels. It is,
however, a **single string**, overwritten on each stage transition — no history of
prior states is retained in the entitlement's XML itself.

## 5. Channel 4: `CA_WARNINGS` / `CA_WARNINGS_WITH_ERROR`

Custom-action-originated warnings/errors are recorded as XML properties (setter not
traced to a specific line in this pass — presumably set from within
`OnInstallCustomActions`/`OnUninstallCustomActions` when a custom action itself
reports a problem) and consulted **only once**, at the very end of `OnInstall()`, to
pick between `pimUIInstallStatusComplete`/`CompleteWithWarnings`/`CompleteWithErrors`
(`docs/04_installation_flow.md` §2, `docs/06_entitlement_framework.md` §4b). This is
the one place where a lower-severity signal (a custom action's own warning) can alter
the final reported outcome of an otherwise-successful pipeline run without any
individual pipeline stage having itself returned `false`.

## 6. Exception Handling Pattern

The dominant idiom, seen consistently across `pim/pim_src/pimXmlInit.cxx`,
`pimGeneralInit.cxx`, `pimBrowserInit.cxx`, `pimUIInit.cxx` (all documented in
`docs/03_application_startup.md` §2), is:

```cxx
try
{
    SomeExternalInitCall();
}
catch (...)
{
    return -99;
}
```

A **bare `catch (...)`** swallowing all exception types and translating them to a
single sentinel value (`-99`, not one of the named `PIM_*_ERROR` constants either —
consistent with the "unlisted magic number" pattern in §2.2), with no logging of what
was actually caught. This means the specific exception type/message thrown by, e.g.,
a Xerces initialization failure is **discarded** at this boundary — only the fact that
*something* threw is preserved, as a single, un-typed, un-messaged `-99`.

The one significant exception to "swallow and continue" is `pimLoop::OnUnhandled`
(`docs/classes/pimLoop.md`) — an exception escaping a **background worker thread**
(as opposed to these synchronous init calls on the main thread) is treated as fatal:
logged, then `btkCrash(...)` is called, terminating the process. There is no
try/catch boundary around the body of any `OnExecute()` implementation itself that
would let a Loop subclass recover from its own internal exception — only the
`thrThread` framework's top-level unhandled-exception hook catches it, after the fact,
by crashing.

## 7. Return-Value Conventions (Summary)

| Pattern | Where used | Meaning of `false`/negative |
|---|---|---|
| `bool` return, `errors&` out-param | Most `pim_core`/`pim_util` step functions (`pimInstallCab`, `ApplyRegistryChanges`, etc.) | Failure; caller consults the `errors` array or the owning `pimLoop`'s `Errors` buffer for detail |
| `int` return, small negative sentinel | Init functions (`pimXmlInit`, `pimGeneralInit`, etc.) | `-99` (generic, uncorrelated with any named code) or occasionally a real `PIM_*_ERROR` constant, inconsistently (§2.2) |
| `int` return, named `PIM_*_ERROR` constant | Top-level `pim*Run` functions | The DLL's own exported contract (`exp_pim_dll.h`) — this is the *only* channel with any documented, named taxonomy (§2.1), and even it isn't used with full consistency (§2.2) |
| `void`, state read back separately | `pimHitError`, `AppendError`/`AppendWarning` | Fire-and-forget; caller must separately poll `pimGetLastError()`/`HasErrors()`/`GetErrors()` |

## 8. Risk Analysis

- **No single source of truth for "what went wrong."** Fully diagnosing a failure
  can require checking all four channels (§1) plus the raw log file
  (`docs/09_logging_framework.md`) — there is no unified error-reporting object or
  correlation ID tying a specific failure event to a specific entitlement, stage, and
  human-readable message all in one place.
- **Un-named magic sentinels** (`-99`, `-104`) coexist with a named, documented
  taxonomy (`pimExit.h`) without consistent use (§2.2, §6) — a maintainer extending
  error handling should prefer the named constants and audit existing call sites
  before assuming `pimGetLastError()`'s value always matches a function's own return
  value for the same event.
- **Exceptions are opportunistically discarded** (`catch (...)` with no logging) at
  every init boundary — if a *specific* external failure needs distinguishing (e.g.,
  "which DLL failed to load" during Xerces/UI/browser init), the current code
  provides no way to recover that information after the fact; a future change wanting
  better diagnostics here would need to add logging *inside* each `catch` block, not
  just rely on the sentinel return value.
- **Global error stack has no entitlement/thread attribution** (§2) — in a
  multi-entitlement concurrent install (multiple `pimEntitlement` threads running
  simultaneously, `docs/06_entitlement_framework.md`), two failures on different
  threads racing to push onto the same global `Errors` array could result in
  `pimGetLastError()` reporting whichever pushed last, not necessarily the one most
  relevant to whatever the caller was actually asking about.

---
*Phase 13 of the requested 20-phase documentation set. No source was modified.*
