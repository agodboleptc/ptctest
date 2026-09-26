# `Cmp_cStrings`

**File:** No definition exists anywhere in this archive — see Scope note.
**Module:** Unknown — referenced from `pim`, `pim_core`, and `pim_ui`, but not declared in any shared header in any of them.

> **Not documentable at the usual depth — genuinely external, with no
> implementation anywhere in this archive.** Every other "external" function
> in this documentation set (e.g. `pimIsDigitiallySignedByPTC`,
> `pimSendNRecvFromPTC`, `pimCompareVerShipcode`) was at least declared in a
> header somewhere in `installmgr.zip`, even when its `.cxx` body lived
> outside the archive. `Cmp_cStrings` has **no header declaration anywhere**
> — it is independently forward-declared with a local `extern` statement in
> 4 separate `.cxx` files, and defined in none of them. An exhaustive
> case-insensitive grep for `Cmp_cStrings` across every file in `pim/`,
> `pim_core/`, `pim_ui/`, and `pim_util/` returns exactly 8 lines: the 4
> `extern` declarations below and their 4 corresponding uses (see Called
> By) — nothing else. This doc covers what can be confirmed (its declared
> signature and every site that declares/uses it) and is explicit about
> what is inferred rather than confirmed, per this project's "no invented
> behavior" rule.

## Purpose (confirmed)

A comparator callback, `int Cmp_cStrings(void *a, void *b)`, passed to the
constructor of several `btkMap<char*, T*>` collections across this codebase
(the same generic keyed-map container already documented as external in
this set, e.g. `pimGetAvailableProduct.h`'s `ProductXArray` typedef). Its
signature matches the generic 2-`void*`-argument comparator shape every
other `btkMap` construction site in this codebase uses.

## Purpose (inferred, not confirmed)

Given the name (`Cmp` + `cStrings`, i.e. "compare C strings") and every
confirmed call site using it to key a map by `char*` (a `btkFSEntry`, a
`btkString`, or a raw `const char*` implicitly converted), it almost
certainly performs a `strcmp()`-style ordering comparison between the two
`char*` keys cast from `void*`. **This is an inference from naming and usage
pattern, not a confirmed fact** — no implementation is present in this
archive to verify it against, and this documentation set's own discipline
(demonstrated repeatedly, e.g. `pimRegEdit.h`'s stale locking comment, or
`pimGetMediaDetails`'s wrongly-commented `auth_status`) is that names and
comments in this codebase are not always reliable guides to actual behavior.
No caller-visible bug can be attributed to this function, because there is
nothing to read.

## Dependencies

None traceable — it is the dependency, not the dependent, in every confirmed
relationship.

## Members / APIs / Private Utilities

Not applicable — this is a single free function with (per the declaration)
no state of its own, not a class.

## Called By (confirmed, exhaustive)

4 real construction sites, each independently declaring
`extern int Cmp_cStrings(void *, void *);` immediately before use (no shared
header), plus 1 confirmed-unused declaration:

- `pim_ui/pim_ui_src/upimDlg.cxx:44` (declaration) / `:57`
  (`upimInstalls::upimInstalls(btkFSEntry Path, pimEntitlement &E) :
  Applications(Cmp_cStrings)`) — `Applications`, a `btkMap<char*,
  pimEntitlement*>` keyed by installation path, in `upimInstalls` (per this
  file's own comment: "started for the master uninstaller or group
  uninstaller project (Creo 3)" — a class not otherwise documented in this
  set).
- `pim_core/pim_core_src/pimEntitlement.cxx:201` (declaration) / `:205-206`
  (`pimEntitlement::pimEntitlement() : ..., DownloadXmlBackups(Cmp_cStrings),
  PreRequisitesToHandle(Cmp_cStrings)`) — 2 `btkMap` members of
  `pimEntitlement` itself. `DownloadXmlBackups` is the map
  `UpdateMediaUrls()` looks up/adds/removes from by `MediaID`, already
  referenced in `docs/classes/pimGetMediaDetails.md`'s Called By.
  `PreRequisitesToHandle` was not traced further in this pass.
- `pim_core/pim_core_src/pimGetAvailableProduct.cxx:87` (declaration) /
  `:89-91` (`pimGetAvailableProducts::pimGetAvailableProducts() :
  Products(Cmp_cStrings)`) — `Products`, keyed by product-definition XML
  name; see `docs/classes/pimAvailableProduct.md`.
- `pim_ui/pim_ui_src/pimsilentDlg.cxx:24` — **declared but never used**: a
  full read of this file confirms no `btkMap` or other construction anywhere
  in it actually references `Cmp_cStrings` after the `extern` line. See Risk
  Analysis.

## Calls Into

Unknown — no implementation to trace.

## Lifetime / Ownership Model / Thread Safety

Not applicable to a stateless comparator function whose implementation is
outside this archive.

## Extension Points

Not applicable — nothing here to extend; any change to string-comparison
behavior for these `btkMap` collections would have to happen wherever
`Cmp_cStrings` is actually implemented (outside this archive).

## Risk Analysis

- **CONFIRMED: `Cmp_cStrings` is redeclared independently in 4 separate
  `.cxx` files, with no shared header.** Each of `upimDlg.cxx:44`,
  `pimsilentDlg.cxx:24`, `pimEntitlement.cxx:201`, and
  `pimGetAvailableProduct.cxx:87` writes its own local
  `extern int Cmp_cStrings(void *, void *);` line (2 with a space before the
  2nd parameter, 2 without — a trivial but confirmed textual inconsistency
  across the 4 copies). If this function's real signature or calling
  convention ever changed, every one of these 4 sites would need to be
  found and updated independently, with no compiler-enforced single source
  of truth — a maintenance hazard distinct from (but structurally similar
  in spirit to) the "no data-driven registry, hardcoded in N places"
  pattern already flagged elsewhere in this documentation set (e.g.
  `pimEntitlementTree::UpdateDisplay_low()`'s hardcoded product-tag special
  cases).
- **CONFIRMED: `pimsilentDlg.cxx`'s declaration is unused.** A full read of
  this file shows the `extern` line at `:24` with no corresponding use
  anywhere else in the file — dead/vestigial code, in the same category as
  other confirmed-unused declarations and dead code blocks catalogued
  throughout this documentation set (e.g. `pimGetMediaDetails.h`'s stale
  comments, `uiCheckButtonCell`'s fully dead implementation in
  `pimEntitlementRefresh.cxx`).
- **UNKNOWN, and unconfirmable in this archive**: whether `Cmp_cStrings`
  performs a case-sensitive or case-insensitive comparison, whether it
  handles `NULL` inputs safely, and whether it's stable/consistent across
  the 4 independent call sites (since each site has its own textually
  separate declaration, nothing in this codebase actually guarantees all 4
  resolve to the *same* linked implementation, though this would be
  extremely unusual for a plain C-linkage `extern` function and is not
  flagged as a live concern — merely noted as something this documentation
  set cannot rule out from source alone).

## Usage Example (as evidenced by call sites)

```cxx
// pim_core/pim_core_src/pimGetAvailableProduct.cxx:87-92
extern int Cmp_cStrings(void *,void *);
pimGetAvailableProducts::pimGetAvailableProducts() :
	Products(Cmp_cStrings)   // Products: btkMap<char*, pimAvailableProduct*>
{
}
```

---
*Extends this documentation set's `pim_core`-focused extension series with
an explicit "confirmed to be undocumentable at full depth" entry: this
function has no implementation anywhere in `installmgr.zip`, so this doc
records everything confirmable about its declaration and usage rather than
inventing behavior that cannot be verified from source, per this project's
governing rule.*
