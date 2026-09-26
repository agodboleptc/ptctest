# PIM Prerequisite Framework

> Traced from `pim_core/pim_core_src/pimEntitlement.cxx` (prerequisite graph methods,
> lines 1823–2003 and 4885–4980) and `pim_core/pim_core_src/pimPrerequisite.cxx`
> (400 lines, the satisfaction-check dispatcher and its concrete check functions).
> No source was modified.

## 1. Overview

Prerequisites in PIM are a **graph of `pimEntitlement` pointers**, not a separate data
type: any entitlement can register any other already-constructed entitlement as its
prerequisite (`AddPrerequisite`), optionally marked "soft" (advisory — installed if
possible, but does not block the dependent's own install if it fails). Whether a given
prerequisite is *satisfied* is answered by a **data-driven dispatcher**
(`pimIsPrerequisiteSatisfied`) that reads a function-name string out of the
prerequisite's own product XML and calls one of a small, fixed set of hardcoded C++
check functions matching that name — there is no plugin/registration mechanism; adding
a new rule means adding a new `else if` branch in `pimPrerequisite.cxx` and referencing
its exact string from a product XML's `<IS_INSTALLED_FUNC>` element.

## 2. Data Structures (on `pimEntitlement`)

| Member | Type | Purpose |
|---|---|---|
| `PreRequisitesToHandle` | `PreRequisiteMap` = `btkMap<char*, pimEntitlement*>` | This entitlement's prerequisites, keyed by the prerequisite's `GetID()` string |
| `SoftPreRequisites` | `StringXArray` | Tags (`GetTag()`) of prerequisites registered as "soft" |
| `prerequisite` (private bool) | — | Whether *this* entitlement is itself registered as someone else's prerequisite (`IsPrerequisite()`) |

## 3. Core API (from `pim_core/includes/pimEntitlement.h`, behavior confirmed against `.cxx`)

| Method | Behavior (confirmed from source) |
|---|---|
| `IsPrerequisite()` | Returns the `prerequisite` flag — is this entitlement *a* prerequisite of something else. |
| `HasPrerequisite()` | `PreRequisitesToHandle.GetSize() > 0` — does this entitlement *have* prerequisites. |
| `IsPrerequisiteSatisfied(bool enable_logs=false)` | If `IsPrerequisite()` and `xmlPtr` is set: reads the `<IS_INSTALLED_FUNC>` node from this entitlement's own XML and calls `pimIsPrerequisiteSatisfied(func_string, enable_logs)` (§4). If this entitlement is not itself a prerequisite, or has no such node, returns `false`. |
| `PrerequisiteNeeded()` | Iterates `PreRequisitesToHandle`; returns `true` if *any* registered prerequisite's `IsPrerequisiteSatisfied()` is `false`. |
| `IsOnlySoftPrerequisiteNeeded()` | Same iteration, but returns `true` only if every unsatisfied prerequisite found is a **hard** one (`!IsSoftPrerequisite(ptr)`) — i.e. it answers "is there at least one unsatisfied *hard* prerequisite," despite the name suggesting the opposite; read the implementation, not the name, before relying on this. |
| `AddPrerequisite(pimEntitlement* input, bool soft=false)` | Adds `input` to `PreRequisitesToHandle` keyed by `input->GetID()`, unless already present (returns `false` if a duplicate `GetID()` is added). If `soft`, also appends `input->GetTag()` to `SoftPreRequisites`. |
| `DropPrerequisite(const char* id)` | Removes and **`delete`s** the matching entry — note this actually destroys the referenced `pimEntitlement` object, not just the graph edge; callers must not hold onto that pointer elsewhere afterward. |
| `RefreshPrerequisites()` | For every unsatisfied prerequisite, synchronizes its platform/language selection to match *this* entitlement's own selection (`pimPlatformMgr`/`pimLanguageMgr`/`pimPackageMgr` reconstructed and applied to the prerequisite's XML), and propagates `MSIVERSION_PROPERTY` from the session into the prerequisite's XML. This keeps a prerequisite's install package selection consistent with whatever platforms/languages the dependent product was configured for. |
| `GetNextPrerequisite(pimEntitlement** out, int& start_index, bool satisified_or_not=false)` | Stateful iterator (caller owns `start_index`) over `PreRequisitesToHandle`; when `satisified_or_not` is `false` (the common case, e.g. in `InstallPreReqSilent`), only *unsatisfied* prerequisites are yielded. |
| `IsSoftPrerequisite(pimEntitlement* in)` | `SoftPreRequisites.Find(in->GetTag()) != -1`. |
| `HasPrerequisitesInstallSucceeded()` | The **blocking wait-and-verify** loop — see §5. |

## 4. The Satisfaction Dispatcher: `pimIsPrerequisiteSatisfied`

`pim_core/pim_core_src/pimPrerequisite.cxx:304`. Reads the DOM node's text content as
a **function-name string** and matches it literally:

```cxx
bool pimIsPrerequisiteSatisfied(DOMNode *input, bool enable_logs)
{
    btkString function_name = StrX(input->getTextContent()).localForm();
    if (function_name == "pimCreoTestPlatformAgent") { ... }
    else if (function_name == "pimWGMTestVFS") { ... }
    return false;   // anything else (including no match) is "not satisfied"
}
```

**Only two rules are wired into this dispatcher**, despite `pimPrerequisite.h`
declaring several more check functions (see §6, "Declared but not dispatched").

### Rule: `pimCreoTestPlatformAgent`

| | |
|---|---|
| **Description** | Confirms PTC's "Creo Agent"/"Platform Services" component is installed at a sufficient version. |
| **XML trigger** | `<IS_INSTALLED_FUNC creoagent_version="X" creosvcs_version="Y">pimCreoTestPlatformAgent</IS_INSTALLED_FUNC>` — both version attributes are required; if either is missing, the dispatcher silently falls through to `return false`. |
| **Implementation** | `pim_core/pim_core_src/pimPrerequisite.cxx:42` |
| **Validation logic** | Opens `HKEY_LOCAL_MACHINE\SOFTWARE\PTC\CreoAgent` (tries `KEY_WOW64_64KEY` then `KEY_WOW64_32KEY`), reads `AgentPackageVersion` and the `PLATFORM_PACKAGE_VERSION_REG`-named value, compares each against the required version via `pimCmpDottedVersions(...) != -1` (installed-version ≥ required). Both the agent *and* the platform-services version must meet their respective requirements. |
| **Failure logic** | If either registry key/value is absent or the version comparison fails, logs (`LG_ERROR`, `pimUIPreReqVerNeeded`) when `enable_logs` is set, and returns `false`. Also computes (but does not itself act on) a `new_install` out-flag indicating whether either key was entirely absent, for the caller's use. |
| **Recovery** | None automatic — the entitlement is left unsatisfied; per `HasPrerequisitesInstallSucceeded()` (§5), a hard (non-soft) failure here after the prerequisite claims "done" is treated as a fatal install error (`PIM_PREQUISITE_NOT_SATISFIED`). |
| **Risk** | Registry path/value names are hardcoded (`"SOFTWARE\\PTC\\CreoAgent"`, `"AgentPackageVersion"`) with no versioned key-namespace — the file's own revision history shows this key path/value set has been revised repeatedly release-over-release ("C4 to C5 change for creo agent registry keys", etc., $$12–$$22), meaning this rule requires an update almost every Creo major version. |

### Rule: `pimWGMTestVFS`

| | |
|---|---|
| **Description** | Confirms "Windchill File System" (VFS) is installed at a sufficient version, for Windchill Workgroup Manager (WGM) products. |
| **XML trigger** | `<IS_INSTALLED_FUNC>pimWGMTestVFS</IS_INSTALLED_FUNC>`, with the upgrade code and version read not from attributes on this node but from a **sibling `<DISTRIBUTION>/<MSI install="Y">` element** in the same product XML (walks `input->getParentNode()`'s children looking for `<DISTRIBUTION>`, then that node's children looking for an `<MSI>` with `install="Y"`, reading its `upgradecode`/`version` attributes). |
| **Implementation** | `pim_core/pim_core_src/pimPrerequisite.cxx:149` (`pimWGMTestVFS`), dispatch wiring at line 323. |
| **Validation logic** | Calls `pimGetVersionInstalled(upgradeCode, ...)` (MSI upgrade-code lookup, from `pim_util`'s `pimMSI`) to enumerate installed product codes/versions sharing that upgrade code, then checks whether any installed version satisfies `pimCmpDottedVersions(installedVer, requiredVer) != -1`. |
| **Failure logic** | Logs `pimUIPreReqVerNeeded` (found but too old) or `pimUIPreReqNeeded` (not found at all) when `enable_logs` is set; returns `false`. |
| **Recovery** | Same as above — no automatic recovery; treated as a hard blocker unless registered as a soft prerequisite. |
| **Risk** | The DOM-tree-walk to find the sibling `<MSI>` node is structurally fragile — it depends on exact document structure (a `<DISTRIBUTION>` element as a sibling of the `<IS_INSTALLED_FUNC>` node's parent, containing an `<MSI>` child) rather than a direct property lookup; a product XML restructure could silently break this check (it would just return `false`/"unsatisfied" rather than error). |

## 5. Waiting for Prerequisites: `HasPrerequisitesInstallSucceeded()`

Called from `pimEntitlement::OnExecute()`'s gate check (see
`docs/04_installation_flow.md` §1) before a dependent entitlement's own install
pipeline runs. This is a **blocking, polling loop on the calling (dependent
entitlement's own) thread**:

```cxx
do {
    Sleep(1);
    PR_not_done = false;
    for each remaining prerequisite ptr in a snapshot copy of PreRequisitesToHandle:
        if (ptr->IsDone() || !ptr->GetInstallMe()):
            if (!ptr->IsPrerequisiteSatisfied(true) && !IsSoftPrerequisite(ptr)):
                pimHitError(PIM_PREQUISITE_NOT_SATISFIED); return false;   // hard failure
            remove ptr from the pending copy                               // satisfied (or soft) -- move on
        else if (IsCancelFlagSet()):
            status = Cancelled; return false;
        else:
            PR_not_done = true; status = "pending prerequisite"
} while (PR_not_done);
return true;
```

Key behaviors confirmed from source:

- **Re-verification, not trust**: even after a prerequisite's own thread reports
  `IsDone()`, this loop independently calls `IsPrerequisiteSatisfied(true)` again — a
  prerequisite that finished but didn't actually leave the machine in a satisfying
  state is caught here, not assumed.
- **Soft prerequisites downgrade a failed re-check to a pass**: `if (!prereq_status &&
  !IsSoftPrerequisite(ptr))` — a soft prerequisite that's still unsatisfied after
  "finishing" is *not* treated as an error; the loop just removes it and moves on.
- **Cancellation does not propagate to the prerequisite**: the cancel branch has a
  commented-out `//ptr->Cancel();` — i.e., a considered-but-not-implemented behavior.
  If the *dependent* entitlement is canceled while waiting, the *prerequisite's* own
  install thread is left running untouched; only the caller's wait loop stops.
- **CPU-spin risk**: this is a plain `Sleep(1)` busy-wait per polling pass, not an
  event/semaphore wait — with many entitlements simultaneously waiting on
  prerequisites, this is a (minor but real) polling overhead pattern, not a blocking
  primitive.
- **Defensive fallback**: if `xmlPtr->IsPropertySet("UI_started")` and the tag-based
  lookup (`EntitlementsHasItemByTag`) fails to resolve a live pointer for a queued
  prerequisite, the code falls back to using the (possibly stale) pointer from the
  snapshot map directly, checks it once, and drops it from the pending set regardless
  of outcome — a "don't get stuck forever over a missing entitlement" safety valve.

## 6. Declared but Not Dispatched

`pim_core/includes/pimPrerequisite.h` declares several more functions that are
**not** matched by `pimIsPrerequisiteSatisfied`'s `if/else if` chain:

- `pimGetInstallStatusCreoViewExpress`, `pimGetInstallStatus_In`,
  `pimGetInstallStatus_SW`, `pimGetInstallStatus_NX` — likely used elsewhere (e.g.
  `pim_ui` status displays for CAD-interop viewers: Inventor ("In"), SolidWorks
  ("SW"), Siemens NX ("NX")) rather than through the `<IS_INSTALLED_FUNC>` mechanism.
  Not confirmed against call sites outside `pimPrerequisite.cxx` in this pass.
- `pimGetInstallStatusPlatformAgent` — a status-string variant of
  `pimCreoTestPlatformAgent`, presumably for UI display rather than the
  boolean satisfaction check.
- `pimGetQAgentPathFromReg` — looks up Quality Agent's installed executable path via
  `HKEY_LOCAL_MACHINE\SOFTWARE\PTC\Quality Agent\Agent`; used for cleanup/PHM
  purposes per its revision history comment ("$$7 added ... for cleanup phm"), not
  prerequisite satisfaction.

If a future feature needs one of these wired into the actual gating mechanism, an
`else if (function_name == "...")` branch must be added to
`pimIsPrerequisiteSatisfied` — these declared functions are not automatically reachable
from a product XML today.

## 7. How Silent Mode Drives This (cross-reference)

`InstallPreReqSilent` (`pim/pim_src/pimTop.cxx`, documented in
`docs/03_application_startup.md` §... and `docs/modules/pim.md`) is the consumer of
`GetNextPrerequisite`/`IsPrerequisiteSatisfied`/`StartOrRestartInstall` for
silent-mode installs — it does its own `Sleep(100)`-based polling loop *in addition
to* the one inside `HasPrerequisitesInstallSucceeded()`, meaning a silent-mode
prerequisite install can be polled at two different layers simultaneously (outer:
`pimTop.cxx`'s `InstallPreReqSilent`; inner: whichever *other* entitlement's
`OnExecute()` is itself waiting via `HasPrerequisitesInstallSucceeded()` for the same
prerequisite). This is consistent, not contradictory — both are polling the same
underlying `pimEntitlement` state — but worth knowing when tuning poll intervals or
diagnosing prerequisite-related install slowness.

## 8. Risk Analysis Summary

- Adding a new prerequisite rule requires a source change in two places kept in sync
  only by convention: the exact string in a product XML's `<IS_INSTALLED_FUNC>` and
  the matching `else if` branch in `pimIsPrerequisiteSatisfied` — no compiler-checked
  link between them; a typo in either place silently degrades to "never satisfied"
  (the function returns `false` for any unmatched name) rather than an error.
- `IsOnlySoftPrerequisiteNeeded()`'s naming is confusing relative to its actual
  behavior (§3) — a strong candidate for a doc-comment (added in this session's Phase
  6 patch for `pimEntitlement.h`) or a future rename, but the *behavior* must not be
  assumed from the name alone.
- `DropPrerequisite` deletes the referenced object — any other code path holding the
  same `pimEntitlement*` (e.g. `pimSessionInfo`'s own arrays) would be left with a
  dangling pointer if it isn't also removed there. Not confirmed as an actual bug in
  this pass (may always be called in a context where the arrays are updated together),
  but worth verifying before touching this method.

---
*Phase 8 of the requested 20-phase documentation set. No source was modified.*
