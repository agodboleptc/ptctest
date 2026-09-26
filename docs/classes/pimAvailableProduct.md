# Class: `pimAvailableProduct` (+ its inseparable collection, `pimGetAvailableProducts`)

**File:** `pim_core/includes/pimGetAvailableProduct.h` (91 lines, full read) / `pim_core/pim_core_src/pimGetAvailableProduct.cxx` (163 lines, full read)
**Module:** `pim_core`
**Inherits:** none (plain value object) / `pimGetAvailableProducts` wraps a `btkMap`-based `ProductXArray`

> **Correction/enrichment (found while giving this class its own full-depth
> pass)**: `docs/classes/pimGetAvailable.md` originally treated this pair as
> a brief "companion" — a small value-object/collection pair
> `pimGetAvailable` uses internally — without giving `pimAvailableProduct`
> its own Members/APIs/Risk Analysis at full depth. Both files were already
> read in full at that time, and the existing Doxygen patch
> (`pimGetAvailableProduct.h.patch`) already had complete per-method
> annotation — but a closer, dedicated pass over `AddInstance()` surfaced a
> confirmed bug that the earlier "companion" framing missed: see Risk
> Analysis. The annotated header and its patch have been updated accordingly
> in this pass.
>
> **2nd enrichment (a later, dedicated pass on `Print()` specifically)**:
> confirmed both `Print()` overloads (`pimAvailableProduct::Print()` and
> `pimGetAvailableProducts::Print()`) have zero live callers anywhere in
> this archive — their only 2 references
> (`pimGetAvailable.cxx:474,478`) are both commented out. See Risk Analysis
> and the annotated header, updated accordingly.

## Purpose

`pimAvailableProduct` is a small value object holding one discovered
web-media product's identity (name, reference-XML key) plus 2 parallel
arrays recording every distinct version/shipcode-and-media-ID pairing
`pimGetAvailable::OnExecute()` finds for it while walking a PTC.com
availability feed. `pimGetAvailableProducts` is the keyed collection of these
value objects that `pimGetAvailable` accumulates results into and exposes
read-only slices of to its own callers.

## Responsibilities

- `pimAvailableProduct::Init(N, xml)` sets the product's `Name` and dual-use
  `Key`/`ReferenceXml` (both set from the same `xml` argument — see Risk
  Analysis for why having 2 identically-valued members is a minor design
  smell, not a bug).
- `pimAvailableProduct::AddInstance(V, shipcode, media_id)` is meant to record
  one more (version, shipcode, media ID) triple for this product, deduplicated
  by version+shipcode — **but see Risk Analysis: the `shipcode` parameter is
  never actually used**, so deduplication and storage happen on the bare
  version string alone.
- `pimAvailableProduct::Sort()` selection-sorts the product's `VerShipcodes`
  array descending (via the external `pimCompareVerShipcode()` comparator,
  not traced further in this pass) while applying the identical index swaps
  to the parallel `MediaIDs` array, keeping the two in lockstep.
- `pimGetAvailableProducts::Create(Name, Xml)` is a find-or-create factory:
  returns the existing `pimAvailableProduct*` for `Xml` if already present
  (via `Products.Lookup()`), otherwise heap-allocates (`XNew`) and registers
  a new one.
- `pimGetAvailableProducts::GetShipcodeOptions(XmlName)` /
  `GetAssociatedMediaIDs(XmlName)` linear-scan the collection by
  `GetXmlName()` and return the matching product's `VerShipcodes`/`MediaIDs`
  array by const reference (a shared function-local `static StringXArray
  empty` on no match).
- `pimGetAvailableProducts::Sort()`/`Print()` simply fan out to every
  contained `pimAvailableProduct`'s own `Sort()`/`Print()`.

## Dependencies

- `StringXArray`/`btkMap`/`btkErrLog` (external `btk` container/logging types).
- `pimCompareVerShipcode()` (external comparator used by `Sort()`, declared
  elsewhere, not traced to its own implementation in this pass).
- `Cmp_cStrings` (an `extern int Cmp_cStrings(void *, void *);` forward
  declaration at `pimGetAvailableProduct.cxx:87`, used as `Products`'
  `btkMap` comparator — declared but not defined in either of this class's 2
  files; not traced to its actual definition in this pass).

## Members

**`pimAvailableProduct`** (all `private`):

| Member | Type | Purpose |
|---|---|---|
| `Key` | `btkString` | Set identical to `ReferenceXml` in `Init()`; used as this product's lookup key in `pimGetAvailableProducts::Products`. |
| `Name` | `btkString` | Display name, set once in `Init()`, never mutated afterward. |
| `ReferenceXml` | `btkString` | The product-definition XML filename this instance represents; set identical to `Key` in `Init()` (see Risk Analysis). |
| `MediaIDs` | `StringXArray` | Parallel array of media IDs, one per `VerShipcodes` entry at the same index. |
| `VerShipcodes` | `StringXArray` | **Named for a combined version+shipcode identifier, but confirmed to hold only the bare version string** — see Risk Analysis. |

**`pimGetAvailableProducts`** (`private`):

| Member | Type | Purpose |
|---|---|---|
| `Products` | `ProductXArray` (`typedef btkMap<char*, pimAvailableProduct*>`) | Keyed collection, keyed by `GetKey()` (== `ReferenceXml`); owns each `pimAvailableProduct*` outright (heap-allocated via `Create()`, never explicitly `delete`d anywhere in this codebase — see Ownership Model). |

## Public APIs

**`pimAvailableProduct`**

| Method | Purpose |
|---|---|
| `pimAvailableProduct()` / `~pimAvailableProduct()` | Both empty/trivial — no explicit cleanup of `MediaIDs`/`VerShipcodes` beyond their own destructors. |
| `bool Init(const char *N, const char *ref_xml)` | Sets `Key`/`Name`/`ReferenceXml`; returns `false` if either argument is `NULL`. |
| `bool AddInstance(const char *V, const char *shipcode, const char *media_id)` | **Confirmed to ignore `shipcode`** — see Risk Analysis. Returns `false` if `V` or `media_id` is `NULL`, or if `V` (the version, alone) is already present in `VerShipcodes`. |
| `const char *GetKey() const` / `GetName() const` / `GetXmlName() const` | Simple accessors. |
| `int GetSize() const` | `VerShipcodes.GetSize()` — the comment `// number of shipcodes (aka same as number of Media ids)` is accurate only in the sense that the 2 arrays are always the same length; it does **not** mean distinct shipcodes are tracked (see Risk Analysis). |
| `void Sort()` | See Responsibilities. |
| `void Print(btkErrLog &log)` | Logs `Name`/`ReferenceXml` then each `MediaID : VerShipcode` pair. **Confirmed dead code — see Risk Analysis.** |
| `const StringXArray &GetVerShipcodes() const` / `GetMedia() const` | Accessors for the 2 parallel arrays. |

**`pimGetAvailableProducts`**

| Method | Purpose |
|---|---|
| `pimGetAvailableProducts()` | Constructs `Products` with the `Cmp_cStrings` comparator. |
| `~pimGetAvailableProducts()` | Empty — see Ownership Model for what this means for the contained `pimAvailableProduct*` pointers. |
| `pimAvailableProduct *Create(const char *Name, const char *Xml)` | Find-or-create factory; returns `NULL` if either argument is `NULL` or if the new object's own `Init()` fails. |
| `int GetSize() const` | `Products.GetSize()` — number of distinct products, not instances. |
| `void Sort()` / `void Print(btkErrLog &)` | Fan out to every contained product. `Print()` is **confirmed dead code — see Risk Analysis.** |
| `const pimAvailableProduct &operator[](dsSize idx) const` | Inline, by-index access into `Products` via `GetByIndex()`. |
| `const StringXArray &GetShipcodeOptions(const char *XmlName)` / `GetAssociatedMediaIDs(const char *XmlName)` | Linear scan by `XmlName`; see Responsibilities. |

## Private Utilities

None on either class — all members above are `private` data, not methods;
every method on both classes is `public`.

## Called By

Exclusively `pim_core/pim_core_src/pimGetAvailable.cxx`
(`pimGetAvailable::OnExecute()`, see `docs/classes/pimGetAvailable.md`) and,
through it, `pimGetAvailable`'s own 2 forwarding accessor methods:

- `AvailableProductsArray.Create(Name, Xml)` (`:430-433`) — once per
  `<ENTITLEMENT>` node found in a media's `image.xml`.
- `Item->AddInstance(version, IShipcode, MediaID)` (`:436`) — immediately
  after `Create()`, for the same node.
- `AvailableProductsArray.Sort()` (`:476`) — once, after the full
  availability-feed walk completes.
- `AvailableProductsArray.GetSize()` / `AvailableProductsArray[i]` (`:483-486`)
  — iterating every discovered product to resolve and register its full
  product-definition XML via `pimGetMediaDetails`.
- `pimGetAvailable::GetSize()` (`pimGetAvailable.cxx:644-648`) forwards to
  `AvailableProductsArray.GetSize()`.
- `pimGetAvailable::GetShipcodeOptions()`/`GetAssociatedMediaIDs()`
  (`pimGetAvailable.cxx:168-176`) forward directly to
  `pimGetAvailableProducts`' methods of the same name — themselves called
  from `pim_ui_src/pimEntitlementTree.cxx:682-683` (see
  `docs/classes/pimGetMediaDetails.md`'s Called By for that chain).
- **`Print()` is the one method on either class with *no* live caller** —
  see Risk Analysis. The only 2 references to
  `AvailableProductsArray.Print(pimDbgLog)` anywhere in this archive
  (`pimGetAvailable.cxx:474,478`) are both commented out.

No other file in this archive references either class (a coincidental
substring match, the XML message-name constant
`pimGetAvailableProductsResponseMsg` in `pim_core/includes/pimDefs.h:718`,
is unrelated).

## Calls Into

`StringXArray`'s own `+=`/`Find`/`GetSize`/`operator[]`, `btkMap`'s
`Lookup`/`Add`/`GetByIndex`/`GetSize`, and the external `pimCompareVerShipcode()`
comparator (used only by `Sort()`).

## Lifetime

`pimGetAvailableProducts` (`AvailableProductsArray`) is a **by-value member**
of `pimGetAvailable` (not a pointer) — it is constructed and destroyed
automatically with its owning `pimGetAvailable` instance, requiring no
separate lifetime management of its own. Each contained `pimAvailableProduct*`
is heap-allocated via `Create()`'s `XNew` and lives for as long as the
containing `pimGetAvailableProducts` does.

## Ownership Model

`pimGetAvailableProducts` owns every `pimAvailableProduct*` in `Products`
outright (it is the only code that ever constructs one, via `Create()`), but
**never explicitly deletes any of them** — `~pimGetAvailableProducts()` is
empty (`pimGetAvailableProduct.h:65`), and no other code in this archive
calls `delete` on a `pimAvailableProduct*`. Since `pimGetAvailableProducts`
itself is a by-value member of `pimGetAvailable`, and `pimGetAvailable`'s own
lifetime and destructor are already documented (heap-allocated singleton
member of `pimInstallMgrDlg`, itself never explicitly destroyed mid-process —
see `docs/classes/pimGetAvailable.md`), this is a 2nd-order leak riding on
top of an already-bounded, once-per-process allocation pattern: every
`pimAvailableProduct` created during a web-media search leaks for the rest
of the process, exactly like the `ImageXml`/`ProductDefinitionXml` leaks
already confirmed in `pimGetAvailable::OnExecute()` itself.

## Thread Safety

No lock of any kind on either class. `AvailableProductsArray` is mutated
exclusively from `pimGetAvailable::OnExecute()`'s background thread during
the search, then only read afterward (via `GetShipcodeOptions()`/
`GetAssociatedMediaIDs()`/`GetSize()`) once the UI thread has confirmed
`IsDone()` — consistent with the same "background thread writes, UI thread
only reads after polling `IsDone()`" convention already documented for
`pimGetAvailable` itself, rather than any explicit synchronization here.

## Extension Points

- Fixing the `AddInstance()` bug (see Risk Analysis) would mean building a
  combined version+shipcode key (e.g. concatenating `V` and `shipcode`) both
  for the dedup check and for what gets stored in `VerShipcodes` — every
  caller of `GetVerShipcodes()`/`Sort()`'s `pimCompareVerShipcode()` would
  need to agree on that combined format, since the external comparator's own
  expectations were not traced in this pass.
- A cache-eviction policy for `Products` would need to actually implement
  `~pimGetAvailableProducts()` and decide `pimAvailableProduct*` ownership
  semantics explicitly — today "owned but never freed" is implicit and
  undocumented in the source itself.

## Risk Analysis

- **CONFIRMED BUG: `AddInstance()`'s `shipcode` parameter is completely
  unused.**
  ```cpp
  bool pimAvailableProduct::AddInstance (const char *V, const char *shipcode, const char *media_id)
  {
      if (V && media_id)
      {
          btkString VerShip;
          VerShip  = V;                              // <-- shipcode never referenced
          if (VerShipcodes.Find(VerShip) == -1)
          {
              VerShipcodes += VerShip;                // <-- stores the bare version only
              MediaIDs += media_id;
              return true;
          }
      }
      return false;
  }
  ```
  Despite the parameter being named `shipcode`, explicitly passed by every
  confirmed caller (`Item->AddInstance(version, IShipcode, MediaID)`,
  `pimGetAvailable.cxx:436`), and despite the member it populates being
  named `VerShipcodes` (implying a combined version+shipcode identifier —
  further reinforced by `Sort()`'s use of a comparator literally named
  `pimCompareVerShipcode`), the function builds and dedups on `V` (the bare
  version string) alone. **Confirmed consequence**: if the same product
  version is discovered under 2 different shipcodes (a realistic scenario —
  e.g. 2 different regional/licensing SKUs sharing one version number, each
  with its own media ID), the 2nd `AddInstance()` call's
  `VerShipcodes.Find(VerShip)` finds the version already present from the
  1st call and returns `false` without ever adding the 2nd shipcode's own
  `media_id` — silently dropping a legitimate, distinct product instance and
  the one media ID needed to download it. Since `GetVerShipcodes()`/
  `GetMedia()` feed directly into `pimEntitlementTree`'s
  version/shipcode-selection dropdown (via `pimGetMediaDetails`'s already-
  documented callers), the practical effect is a shipcode option silently
  missing from that dropdown whenever this collision occurs.
- **Minor: `Key` and `ReferenceXml` are always identical.** `Init()`
  (`:13-23`) sets both from the same `xml` argument, with no confirmed case
  in this codebase where they ever diverge. Not a bug — `GetXmlName()` and
  the internal map key genuinely serve different call-site purposes — but a
  redundant member that could be a single field with 2 accessor names
  instead, if a future maintainer wanted to simplify.
- **CONFIRMED, low practical severity: every `pimAvailableProduct*` leaks.**
  See Ownership Model — `~pimGetAvailableProducts()` never frees its
  `Products` map's contents. Bounded by one process's lifetime, in the same
  category as the already-documented `ImageXml`/`ProductDefinitionXml` leaks
  in `pimGetAvailable::OnExecute()` (`docs/classes/pimGetAvailable.md` Risk
  Analysis) — this is effectively a 3rd instance of the same
  leak-per-web-media-search pattern in this small cluster of `pim_core`
  classes.
- **CONFIRMED DEAD CODE: `pimAvailableProduct::Print()` (and
  `pimGetAvailableProducts::Print()`, which merely fans out to it) have zero
  live callers anywhere in this archive.** An exhaustive grep for `.Print(`
  across every file in `pim`, `pim_core`, and `pim_ui` finds exactly 3
  matches: `pimGetAvailable.cxx:474` and `:478`
  (`//AvailableProductsArray.Print(pimDbgLog);`), **both commented out**,
  and one unrelated `Test.Print(output_file)` call in
  `pim/pim_src/pimTop.cxx:3042` on a completely different object. `pimDbgLog`
  itself — the argument these dead calls would have passed — is likewise
  referenced nowhere else in this codebase outside those same 2 commented
  lines and a 3rd commented `//pimDbgLog << endl;` at `pimGetAvailable.cxx:564`,
  so it is not confirmed to be a real, live logger object at all, merely a
  name left over in disabled debug-print statements. Both `Print()` overloads
  remain fully implemented and callable — this is confirmed-unreachable
  code, not a compile error waiting to happen, consistent with this
  documentation set's other confirmed-dead-but-still-compiled findings (e.g.
  `pimGetAvailable::IsAuthorized()`, `uiCheckButtonCell`'s entire
  implementation in `pimEntitlementRefresh.cxx`).

## Usage Example (as evidenced by call sites)

```cxx
// pim_core/pim_core_src/pimGetAvailable.cxx:430-436 (OnExecute, trimmed)
pimAvailableProduct *Item = AvailableProductsArray.Create(
    StrX(attribName->getNodeValue()).localForm(),
    str /* the entitlement's reference XML filename */);
if (Item)
{
    // CONFIRMED BUG: if this exact version was already added under a
    // different IShipcode for a different MediaID, this call silently
    // does nothing -- IShipcode itself is never even read.
    Item->AddInstance(StrX(attribVersion->getNodeValue()).localForm(), IShipcode, MediaID);
}
```

---
*Extends this documentation set's `pim_core`-focused extension series:
upgrades `pimAvailableProduct`/`pimGetAvailableProducts` from a "companion"
mention inside `docs/classes/pimGetAvailable.md` to their own full-depth
class doc, surfacing a confirmed parameter-ignored bug the earlier pass's
lighter treatment missed.*
