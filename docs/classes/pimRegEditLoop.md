# `pimRegEditLoop`

`pim_core/includes/pimRegEditLoop.h` (54 lines) / `pim_core/pim_core_src/pimRegEditLoop.cxx` (742 lines, full read)

## Purpose

`pimRegEditLoop` is the install-step (`pimLoop` subclass) that creates and
removes Windows registry keys/values declared in product XML, and
additionally manages a legacy "numeric file-extension" registration scheme
(`.1` through `.250` under `Software\Classes`) for older Creo file-association
behavior. It is the last of the 9 Loop subclasses to receive full-depth
documentation in this effort, completing the set alongside its owner
`pimRegEdit` (`docs/classes/pimRegEdit.md`).

## Responsibilities

- Iterate `<REGISTRY>`/`<VALUE>` elements and, for each, create (`OnInstall()`)
  or remove (`OnRollback()`) the corresponding registry key/value.
- Resolve each `<VALUE key="[ROOT]SubKey">` attribute into a root-hive token
  (`[HKCR]`/`[HKLM]`/`[HKCU]`/`[HKU]`) plus a subkey path, matching the 4
  literal tokens documented in `docs/07_registry_usage.md` §2.1 and
  `ai-context/business_rules.yaml`'s `registry_root_tokens` rule — confirmed
  directly in `Create()`/`Remove()`'s own root-token `if`/`else if` chains
  (`pimRegEditLoop.cxx:362-383`, `:608-617`).
- Gate each `<VALUE package="X">` entry on package `X`'s own `install="Y"`
  attribute — an entry tied to a not-selected package is skipped
  (`pimRegEditLoop.cxx:127-149`), a package-conditional mechanism not
  documented elsewhere in this pass.
- Mirror every successful create into a human-readable `.reg` file via
  `pimRegFileCreate` (per-product `[LP]\bin\pim\xml\created.reg` plus a
  combined `[PIM_DLL_PATH]\CommonCreated.reg`), matching
  `ai-context/business_rules.yaml`'s `reg_file_admin_mirror` rule — this class
  is the confirmed concrete implementer of that rule.
- Optionally register/deregister the legacy `.1`-`.250` numeric file
  extensions (`CreateNumericExtensions()`/`RemoveNumericExtensions()`), gated
  by the `RegEdit1To250`/`RemoveNumericExtensions` XML properties, tracked via
  a persisted "highest registered extension" value at
  `HKCU\Software\PTC\creoFileVers`.

## Dependencies

- `pimLoop` (base class).
- `pimRegFileCreate` (`.reg` mirror-file writer; not independently re-traced
  to full depth in this pass beyond the call sites used here — its own
  `AppendKey()`/`Append_REG_SZ()`/`WriteFile()`/`WriteCombinedRegFile()`
  signatures were read but its internal implementation was not).
- `pimPlatformMgr` (`GetInstallPlatformNames()`, used to resolve the
  `[THIS_ARCH]` placeholder to `i486_nt`/`arm64_win64`/`x86e_win64`).
- `pimConvert` (property placeholder resolution).
- `pimEntitlement.h` is included but no direct call into `pimEntitlement` was
  observed in this file's body — likely included for a shared type/constant
  rather than active use; not confirmed further in this pass.
- Win32 registry API directly (`win32RegCreateKeyExA`, `win32RegSetValueExA`,
  `win32RegOpenKeyExA`, `win32RegQueryValueExA`, `win32RegDeleteValueA`,
  `win32RegDeleteKey`, `win32RegQueryInfoKey`) — thin wrappers over the native
  Win32 Registry API, called directly rather than through any further
  PIM-specific abstraction layer.

## Members

This class declares no additional data members beyond `pimLoop`'s own —
`int do_rollback` is its only field (`pimRegEditLoop.h:23`), identical in
shape to `pimScriptLoop`/`pimServiceLoop`/`pimShortcutLoop`/`pimPsfLoop`.

## Public/Protected APIs

- `pimRegEditLoop(pimXmlFile*)` — constructor; `do_rollback` initialized to
  `false`.
- `void SetRollback(bool tf)` — marks this Loop to run `OnRollback()` instead
  of `OnInstall()` when `Execute()`d, identical pattern to its Loop siblings.
- `void OnExecute()` (protected) — dispatches to `OnInstall()` or
  `OnRollback()` based on `do_rollback`.
- `void OnInstall()` (protected) — see Responsibilities; full walkthrough
  below.
- `void OnRollback()` (protected) — see Responsibilities and the **confirmed
  bug** in Risk Analysis.
- `void OnTerminate()` (protected) — standard thread teardown: releases the
  logger handle, marks `mDone` under `Mutex`, calls the inherited
  `ClearInProgress()`.

## Private/Protected Utilities

- `bool Create(pimRegFileCreate&, const btkString &Key, const btkString &Id, const btkString &ValueName, const btkString &DefValue, pimConvert*, bool &created, bool &existed, bool &default_changed, const btkString &plat, DWORD ValueType = REG_SZ)`
  (`pimRegEditLoop.cxx:351-467`) — the core create/write primitive. Maps `Key`
  to a real `HKEY` root; calls `win32RegCreateKeyExA()`, setting `created`/
  `existed` from the returned disposition
  (`REG_CREATED_NEW_KEY`/`REG_OPENED_EXISTING_KEY`). If `ValueName` is empty,
  unconditionally appends a "key created/visited" entry to the `.reg` mirror
  via `AppendKey()` — **using a separately-converted copy of `DefValue` that
  does NOT have `[THIS_ARCH]` registered on the `Converter` yet** (see Risk
  Analysis). If `DefValue` is non-empty, writes it via
  `win32RegSetValueExA()` (as `REG_DWORD` if `ValueType` says so, else as a
  raw byte string), converting `Input` a second time — this second conversion
  **does** register `[THIS_ARCH]→plat` on the `Converter` first if `plat` is
  non-empty. Only mirrors the named-value case (`Append_REG_SZ()`) into the
  `.reg` file when `ValueName` is non-empty.
- `bool GetHighestCreoFileVer(int &highest)` / `bool SetHighestCreoFileVer(int highest)`
  (`pimRegEditLoop.cxx:469-520`) — read/write a single `DWORD` at
  `HKCU\Software\PTC\creoFileVers`, tracking the highest numeric file
  extension (`.N`) already registered by `CreateNumericExtensions()`.
- `bool CreateNumericExtensions(bool do_create_reg_file)`
  (`pimRegEditLoop.cxx:522-581`) — per the `$$7 "Always do .1 to .250"`
  revision comment, unconditionally resets its working `highest` to `0` and
  re-walks the **entire** `.1`–`.250` range every time it runs (not just from
  the previously-recorded high-water mark), registering
  `Software\Classes\.N → "creoFile"` for each `N` in `[1,250]` except `N=0`
  (skipped as a loop-boundary artifact) and `N=123` (skipped for a reason not
  stated anywhere in this file or its revision history — **UNKNOWN**, marked
  here rather than guessed). Persists the new high-water mark only if it
  exceeds the previous one.
- `bool RemoveNumericExtensions()` (`pimRegEditLoop.cxx:583-601`) — walks
  from the persisted `highest` down to `1` (again skipping `123`), calling
  `Remove()` for each, then resets the persisted high-water mark to `0`.
- `bool Remove(const btkString &Key, const btkString &Id, const btkString &ValueName)`
  (`pimRegEditLoop.cxx:603-686`) — if `ValueName` is non-empty, deletes just
  that named value (`win32RegDeleteValueA`). If `ValueName` is empty, deletes
  the **whole key**, but only if it has zero subkeys
  (`win32RegQueryInfoKey`'s subkey-count output) — its own comment explains
  why: "does key have child keys if so we want to skip it... we only delete
  from the bottom of a tree up." A key with existing subkeys is silently left
  in place and `Remove()` returns `false` for it (see Risk Analysis for how
  `OnRollback()`'s outer loop is structured around this constraint).
- `#if 0`-disabled `bool IsOKToDelete(const btkString&, const btkString&)`
  (`pimRegEditLoop.cxx:688-734`) — confirmed dead code: a hardcoded allow-list
  of ~20 known top-level `HKCR` file-type/ProgID entries
  (`.psf`/`.pha`/`.asm`/.../`psfFile`/`phaFile`/...), intended to restrict
  deletion of top-level `HKCR` entries to only these known-safe ones. Entirely
  compiled out (`#if 0`) — **the safety check this represents is not active in
  the current build**; `Remove()` as actually compiled has no such allow-list
  and will delete any top-level `HKCR` key the XML names, provided it has no
  subkeys. This is the same "referenced-but-inactive safety mechanism" shape
  as the `ROLLBACK_CANCELLED_INSTALL`/`ERROR_ON_UNSIGNED` findings elsewhere
  in this codebase, though here the guard is `#if 0` (unconditionally
  disabled) rather than an undefined macro.

## OnInstall() Walkthrough

1. Reads `RegEdit1To250` (any presence → `do_1to250 = true`) and `NoLPXmlLog`
   (its **absence** → `do_create_reg_file = true`; note the inverted sense —
   the property that suppresses the `.reg` mirror is named for a different,
   more general log-suppression concept, not specifically about the registry
   mirror).
2. Resolves the install platform name via `pimPlatformMgr`, mapping to one of
   3 literal `[THIS_ARCH]` substitution values: `i486_nt`, `arm64_win64`, or
   the fallback `x86e_win64` (`pimRegEditLoop.cxx:62-67`) — this is the
   concrete implementation behind the `[THIS_ARCH]`-related revision history
   entries in both this file's and the header's changelog.
3. Iterates every `<REGISTRY>` element's `<VALUE>` children (single pass,
   correct `item(index)` with no double-increment — see Risk Analysis for the
   contrasting `OnRollback()` bug), checking `mCancel`/`mPause` each iteration.
4. For each eligible `<VALUE>` (package-gate permitting), parses
   `key="[ROOT]SubKey"` via `Pos("]")`, reads optional `id`/`valuename`/
   `default`/`valuetype` attributes, and calls `Create()`.
5. On success, marks `installed="Y"` on the XML node and logs one of 4
   distinct message variants (created/existed × default-value-changed or not)
   via `pimMessage`/`LG_INFO`. On failure, calls `AppendError()` with
   `pimErrorRegEdit`.
6. If `do_1to250`, additionally runs `CreateNumericExtensions()`.
7. Writes the combined `.reg` mirror file once at the end
   (`DotRegFile.WriteCombinedRegFile()`).

## Called By

- `pimRegEdit::Create_low()` (`pim_core/pim_core_src/pimRegEdit.cxx:38`) —
  the sole constructor/owner of this class, confirmed in
  `docs/classes/pimRegEdit.md`. `SetRollback(true)` is set when `Create_low`
  is invoked in uninstall mode.

## Calls Into

- `pimRegFileCreate` (`.reg` mirror file writer).
- `pimPlatformMgr` (`GetInstallPlatformNames()`).
- `pimConvert` (property resolution).
- Win32 Registry API directly (see Dependencies).
- Base `pimLoop` members: `Mutex`/`mCancel`/`mPause`/`mDone`, `pausePtr`,
  `AppendError()`, `ClearInProgress()`, `xmlPtr`'s `PreRead`/`PostRead`/
  `PreWrite`/`PostWrite` locking protocol (`docs/classes/pimXmlFile.md`).

## Lifetime

Transient: constructed fresh by `pimRegEdit::Create_low()` on each
TryLock-acquired operation, `Execute()`d asynchronously, and deleted by
`pimRegEdit::Status()` (on completion) or `pimRegEdit::Kill()` — see
`docs/classes/pimRegEdit.md`'s Lifetime section. Exactly one instance can
exist at a time, process-wide, since `pimRegEdit` is a singleton.

## Ownership Model

Owned exclusively by `pimRegEdit` (`docs/classes/pimRegEdit.md`), a
process-wide singleton — not by `pimEntitlement` directly, despite this
file's `#include <pimEntitlement.h>`. This class has no singleton or
concurrency logic of its own; all serialization is enforced one layer up, by
its owner.

## Thread Safety

- Cancellation is checked only in `OnInstall()`'s loop (via `mCancel` under
  `Mutex`); `OnRollback()`'s loop checks only `mPause`, **not `mCancel`** —
  confirmed by direct comparison of the two loops
  (`pimRegEditLoop.cxx:94-99` vs. `:257-260`). A cancel requested while a
  rollback (uninstall/rollback registry cleanup) is in progress will not be
  honored by this class itself — consistent with `pimRegEdit::Cancel()`
  being flag-only anyway (per `docs/classes/pimRegEdit.md`), but notable that
  even a hypothetical future fix to `pimRegEdit::Cancel()`'s propagation
  would still need a matching fix here, since `OnRollback()` has nowhere to
  read a cancel flag from mid-loop.
- No additional locking beyond the inherited `pimLoop::Mutex` guarding the
  cancel/pause flags — registry writes themselves rely on the single-instance
  guarantee from the owning `pimRegEdit` singleton, not any lock local to
  this class.

## Extension Points

- Any change to the `<VALUE key="...">` parsing (`Pos("]")`-based root/subkey
  split) must be mirrored in both `Create()`'s and `Remove()`'s copies of the
  same parsing logic — they are independently re-implemented, not shared via
  a helper.
- A future fix to the `OnRollback()` double-increment bug (see Risk Analysis)
  should be validated against the outer `while (rolled_back_something)`
  retry loop's own correctness — simply removing the extra `index++` changes
  the iteration count per pass and should be re-verified against the
  bottom-up subkey-deletion ordering constraint `Remove()` depends on.
- The hardcoded `N=123` skip in `CreateNumericExtensions()`/
  `RemoveNumericExtensions()` should be confirmed with the original team
  before any refactor touches that range — its rationale is UNKNOWN here.

## Risk Analysis

- **CONFIRMED HIGH-SEVERITY BUG: `OnRollback()` silently skips roughly half
  of all `<REGISTRY>`/`<VALUE>` entries on every pass.** The inner loop reads
  `next_nl->item(index++)` (`pimRegEditLoop.cxx:267`) **in addition to** the
  enclosing `for` statement's own `index++` increment clause
  (`pimRegEditLoop.cxx:255`) — a double increment, advancing `index` by 2 per
  iteration instead of 1. By direct contrast, `OnInstall()`'s equivalent loop
  (`pimRegEditLoop.cxx:92-111`) uses the correct single-increment
  `next_nl->item(index)` with no post-increment. Since `index` always resets
  to `0` at the start of each pass of the outer `while (rolled_back_something)`
  loop, odd-indexed nodes (1, 3, 5, ...) are **never visited on any pass, no
  matter how many outer-loop retries occur** — this is not merely a
  slower-than-expected convergence, it is a permanent, total skip of every
  other registry entry during uninstall/rollback. Practical consequence: an
  uninstall or rollback of a product with registry entries can leave up to
  ~half of them behind in the registry, with `installed="Y"` never cleared on
  the skipped nodes (so a subsequent uninstall attempt would still skip the
  same ones, since the bug is deterministic and position-based, not
  data-dependent). This is the single highest-confidence, highest-impact
  finding in this class's documentation — recommend treating it as a
  priority fix candidate alongside the `pimServices` concurrency bug found in
  the wrapper-class documentation pass (`docs/classes/pimServices.md`).
- **`[THIS_ARCH]` placeholder ordering asymmetry in `Create()`**: the
  ValueName-empty branch's `.reg`-mirror `Input` conversion
  (`pimRegEditLoop.cxx:411-416`) happens using whatever placeholders are
  already registered on the passed-in `Converter`, `[THIS_ARCH]` is only
  added to that `Converter` inside the *second* branch just below it
  (`pimRegEditLoop.cxx:427-431`), and only when `plat` is non-empty. Because
  `Converter` is constructed once per `OnInstall()` call and passed by
  pointer into every `Create()` call across the whole node-list loop, this
  means: if `[THIS_ARCH]` is registered on the `Converter` by an *earlier*
  `Create()` call in the same `OnInstall()` invocation, later calls'
  ValueName-empty mirror entries would pick up the substitution correctly
  (since `pimConvert`'s placeholder table is presumed to persist across calls
  on the same instance — this specific persistence behavior was not
  independently re-verified against `pimConvert`'s own source in this pass).
  The concrete, confirmed risk is narrower than "always broken": it is that
  the **very first** `Create()` call in a given `OnInstall()` invocation whose
  `ValueName` is empty and whose `DefValue` contains a literal `[THIS_ARCH]`
  token would record that unresolved literal token in the `.reg` mirror file,
  while the live registry write in the same call (the second branch) still
  resolves it correctly. This is a `.reg`-mirror-file cosmetic/correctness
  gap, not a live-registry defect — the actual installed registry value is
  unaffected.
- **`#if 0`-disabled `IsOKToDelete()` allow-list is not active**: `Remove()`
  as compiled has no restriction on which top-level `HKCR` keys it may delete
  beyond the "must have zero subkeys" check — the intended allow-list of
  known PTC file-type ProgIDs is present in source but entirely inert. A
  product XML that (accidentally or maliciously) names an unrelated top-level
  `HKCR` key with `installed="Y"` and no subkeys would have it deleted on
  rollback with no additional safety check.
- **No iteration cap on `OnRollback()`'s outer `while (rolled_back_something)`
  loop**: in principle a pathological or corrupted node graph could keep this
  loop running indefinitely, though a normal registry-key tree (finite depth,
  no cycles) bounds it naturally in practice; still, there is no explicit
  safety valve.
- **`N=123` skip in the numeric-extension range is unexplained** — flagged as
  UNKNOWN rather than guessed; do not assume it is safe to remove without
  confirming with the original team what `.123` collides with.

## Usage Example

```cpp
// Traced from pimRegEdit::Create_low(), pim_core/pim_core_src/pimRegEdit.cxx:30-51
RegEditLoop = XNew pimRegEditLoop(xmlPtr);
if (uninstall)
    RegEditLoop->SetRollback(true);
RegEditLoop->Execute(); // asynchronous; caller polls pimRegEdit::Status()
```
