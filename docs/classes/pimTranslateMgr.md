# Class: `pimTranslateMgr`

**File:** `pim_core/includes/pimTranslateMgr.h` (32 lines, full read) /
`pim_core/pim_core_src/pimTranslateMgr.cxx` (129 lines, full read)
**Module:** `pim_core`
**Inherits:** none

> **First full-depth pass** (previously cited only via call-site signatures
> in `docs/classes/pimGetAvailable.md`, `docs/classes/pimEntitlementTree.md`,
> `docs/08_xml_configuration.md`, `docs/03_application_startup.md`, and
> `ai-context/xml_schema.yaml`, all flagging it as "not read this pass"). This
> pass reads both files in full and traces all 8 constructor/`TranslateFrom()`
> call sites across the archive. Headline finding: a mismatched-guard bug in
> `TranslateFrom(pimXmlFile*)` itself, confirmed reachable via 2 of
> `pimEntitlementTree::OnOptionMenuSelect()`'s 3 near-identical call
> sequences; plus a confirmed dead, compile-breaking typo in one of the 8
> call sites. See Risk Analysis.

## Purpose

`pimTranslateMgr` applies a single, generic localization pass: given a
"translation file" XML document (one `<TRANSLATION>` entry per localizable
value) and a "product" XML document (the object it was constructed with),
it walks every `<TRANSLATION>` entry, locates the corresponding node in the
product document by an `element`/`attribute`/`attributeMatch` triple, and
overwrites either that node's text content or one of its named attributes
with the translation entry's `<TEXT>` value. It is invoked wherever a
product-definition XML is loaded and the UI's configured language differs
from the OS/media's own language, to re-label the in-memory DOM before it
reaches the wizard UI.

## Responsibilities

- Hold a `pimXmlFile*` to the **product** document (`xmlProduct`, set once at
  construction, never reassigned) and a `btkString filename` (the current
  product's own filename, computed lazily — see Risk Analysis for a
  confirmed bug in how this is computed).
- `TranslateFrom(pimXmlFile *ptr)` — the core algorithm: iterate every
  `<TRANSLATION>` node in `ptr`'s document, skip any that are scoped
  (`file` attribute) to a different product filename, resolve the matching
  target node in `xmlProduct` via `FindNode()`, and apply the translation
  (either `setAttribute()` or `setTextContent()`, depending on whether a
  `modify` attribute is present). Returns `true` if at least one entry was
  successfully applied.
- `TranslateFrom(btkFSEntry &file)` — a convenience overload: reads `file`
  off disk into a freshly allocated, freshly `DoRead()`-parsed `pimXmlFile`,
  delegates to the pointer overload, and always deletes the temporary
  `pimXmlFile` regardless of outcome (no leak either way).
- `FindNode(DOMNamedNodeMap*)` (private) — resolves one `<TRANSLATION>`
  entry's `element`/`attribute`/`attributeMatch` triple to a single DOM node
  in `xmlProduct`'s document via `pimXmlFile::FindNodelistAttribMatch()`
  (first document-order match; see `docs/classes/pimGetAvailable.md`'s
  already-established precedent for this exact "no duplicate-name
  detection" pattern elsewhere in this codebase).

## Members

- `xmlProduct` (`pimXmlFile*`) — the product document translations are
  applied to. Set once in the constructor, never reassigned, never owned
  (never `delete`d by this class — the caller owns it).
- `filename` (`btkString`) — the current product's own filename, used to
  filter `file`-scoped `<TRANSLATION>` entries. A class member (not a local),
  so it persists across multiple `TranslateFrom()` calls on the same
  instance — see Risk Analysis for why this is a latent (not live) design
  fragility.

## Dependencies

- `pimXmlFile` (`GetChildNodeByNodeName`, `FindNodelistAttribMatch`,
  `getDocument()`/`getFilePath()`, `SetXml`/`UseNS`/`UsePrettyPrint`/
  `DoRead`/`ErrorOccured`) — every DOM operation in this class goes through
  it; see `docs/classes/pimXmlFile.md`.
- Xerces-C++ DOM (`DOMNode`, `DOMNamedNodeMap`, `DOMElement`,
  `XMLString::compareString`) — used directly, not wrapped.
- `btkFSEntry` (external `btk` toolkit) — used to derive `filename` from a
  full path (`.GetTail()`) and to check/read a translation file on disk.
- XML tag/attribute name constants from `pim_core/includes/pimDefs.h`:
  `pimTRANSLATION`/`pimTEXT` (element names), `pimfile`/`pimmodify`/
  `pimelement`/`pimattribute`/`pimattributeMatch` (attribute names) — see
  `ai-context/xml_schema.yaml`'s `translation_files` entry for the confirmed
  schema this pass derived from them.

## Called By

8 confirmed constructor call sites across the archive; only 4 are live:

- **Live**: `pim/pim_src/pimTop.cxx:477` — inside
  `#ifdef PIM_TRANSLATE_XML`, which IS `#define`d in this same file
  (`:185`), so this call site is always active in the `pim` module's own
  build. `xmlProduct` = `SessionInfo.GetEntitlement(j)->GetXMLPtr()`, an
  entitlement loaded from a real file on disk — always has a file path.
- **Live**: `pim_core/pim_core_src/pimGetAvailable.cxx:582`
  (`pimGetAvailable::OnExecute()`) — not guarded by `PIM_TRANSLATE_XML` at
  all, just the plain OS/UI-language-mismatch check. `xmlProduct` =
  `ProductDefinitionXml`, which had `SetXml(Prod.GetXmlName())` called on it
  moments earlier (`:559`) — always has a file path. See
  `docs/classes/pimGetAvailable.md`.
- **Live**: `pim_ui/pim_ui_src/pimEntitlementTree.cxx:1885,1956,2006`
  (`pimEntitlementTree::OnOptionMenuSelect()`, 3 near-identical call
  sequences for "this application's media ID changed" / "a sibling sharing
  this parent" / "the parent itself"). **CONFIRMED BUG here** — see Risk
  Analysis and `docs/classes/pimEntitlementTree.md`.
- **Dead (macro never defined in this translation unit)**:
  `pim_core/pim_core_src/pimEntitlement.cxx:995` — inside
  `#ifdef PIM_TRANSLATE_XML`, but `PIM_TRANSLATE_XML` is `#define`d in
  exactly one place in this entire archive, `pim/pim_src/pimTop.cxx:185`, a
  plain `.cxx` file with no shared header — and `pimEntitlement.cxx` is
  compiled as part of `pim_core`'s own unity build
  (`pim_core/pim_core_src/pim_core_src.cxx`, which does not include
  `pimTop.cxx` or anything that does), a separate translation unit from
  `pim`'s own unity build (`pim/pim_src/pim_src.cxx`, which DOES include
  `pimTop.cxx`). This block can never see the macro and is always compiled
  out. See Risk Analysis for the compile-breaking typo this conceals.
- **Dead (`#if 0`, already documented elsewhere)**:
  `pim_ui/pim_ui_src/pimInstallMgrActions.cxx:1861,1931,1972` — all 3 sites
  fall inside this file's confirmed `#if 0`-disabled `uiApplicationsList`
  class (span `:1672-2019`), the same dead duplicate already flagged in
  `docs/classes/pimGetMediaDetails.md`'s Risk Analysis for its sibling
  `OnOptionMenuSelect()`'s null-deref bug.

## Calls Into

- `pimXmlFile::getDocument()->getElementsByTagName(pimTRANSLATION)`,
  `GetChildNodeByNodeName(node, pimTEXT)`, `FindNodelistAttribMatch(...)`,
  `getFilePath()` — all on either `ptr` (the translation document) or
  `xmlProduct` (the product document); see Risk Analysis for which is used
  where.
- `XMLString::compareString()` (Xerces) — used both for the `file`-scope
  filter and, indirectly via `FindNodelistAttribMatch()`, for the
  `attributeMatch` lookup.

## Lifetime / Ownership Model

- `pimTranslateMgr` never owns `xmlProduct` — every confirmed call site
  constructs a `pimTranslateMgr` as a stack-local, uses it exactly once, and
  lets it go out of scope; `xmlProduct` itself is owned and freed (or not —
  out of scope for this class) by the caller.
- The `TranslateFrom(btkFSEntry&)` overload owns and always frees its own
  temporary `pimXmlFile` (the parsed translation document), on both the
  success and failure paths — no leak.

## Thread Safety

Not documented/evidenced in this class itself — no locking, no static/shared
state (`filename` and `xmlProduct` are per-instance). Safety depends entirely
on whatever locking discipline the caller applies to `xmlProduct` itself
(see `docs/classes/pimXmlFile.md`).

## Risk Analysis

- **CONFIRMED BUG: mismatched-guard when computing `filename`.**
  `TranslateFrom(pimXmlFile *ptr)` (`pimTranslateMgr.cxx:22-30`):
  ```cpp
  if (ptr && xmlProduct)
  {
      if (ptr->getFilePath())
      {
          filename = btkFSEntry(xmlProduct->getFilePath()).GetTail();
      }
      ...
  ```
  The guard checks `ptr->getFilePath()` — the **translation** document's own
  file path — but the value assigned to `filename` is read from
  `xmlProduct->getFilePath()` — a **different object**, the product document
  this instance was constructed with. Confirmed directly from source,
  independent of reachability: the code checks one object's path presence
  and unconditionally dereferences the other, un-guarded, inside the
  guarded block.
- **CONFIRMED DOWNSTREAM CONSEQUENCE, reachability precisely characterized
  per call site**: this mismatch only matters when `xmlProduct`'s own file
  path is unset (a default-constructed, never-`SetXml()`'d `btkFSEntry`),
  since `ptr`'s own path is always set in every confirmed call chain (the
  `TranslateFrom(btkFSEntry&)` overload always calls `SetXml(file)` on its
  temporary `pimXmlFile` before delegating). Traced every LIVE call site's
  `xmlProduct` origin (see Called By):
  - `pimTop.cxx:477`, `pimGetAvailable.cxx:582`, and
    `pimEntitlementTree.cxx:1885` (the "this application changed" arm) —
    **NOT reachable**: each one's `xmlProduct` has `SetXml()` called on it
    (or is loaded from a real file) immediately before `pimTranslateMgr` is
    constructed.
  - `pimEntitlementTree.cxx:1956` and `:2006` (the "sibling"/"parent"
    media-ID-bump arms of the SAME function) — **CONFIRMED REACHABLE**:
    `eptr_new_xml` here comes straight from
    `pimGetMediaDetails::GetProductXml()`
    (`pim_core/pim_core_src/pimGetMediaDetails.cxx:143-238`), traced to
    build its returned `pimXmlFile` purely via `DoRead(*memblock)` against
    an in-memory downloaded byte buffer — `SetXml()` is **never** called on
    it internally, so its own file path starts unset. The subsequent
    `aPtr->Init(eptr_new_xml, new_xml, mediaid)` (`pimEntitlement.cxx:1038-1085`)
    only aliases `aPtr`'s own `xmlPtr` to `eptr_new_xml` (`xmlPtr = application_definition`,
    a plain pointer assignment, not a copy) on its **normal** path; but when
    `DownloadXmlBackups.Lookup(MediaID, backup)` finds an existing backup for
    this `MediaID` (`:1050-1066`, an early return), `xmlPtr` is instead set
    to `backup->GetXml()` — a **different** object — leaving `eptr_new_xml`
    itself un-aliased, with no file path ever set on it, exactly 2 lines
    before it is passed directly into `pimTranslateMgr`'s constructor.
    **Confirmed real-world trigger**: a MediaID this session has already
    downloaded/cached once before (a sibling or parent entitlement's media
    ID being changed a 2nd time, or one that shares a MediaID already seen
    via another product). **Confirmed consequence**: with `xmlProduct`'s
    path unset, `filename` becomes empty, so every `file`-scoped
    `<TRANSLATION>` entry (the majority of realistic entries, which target
    one specific product) fails the `AttribFile` match check
    (`:44-46`) and is silently skipped — while entries with no `file`
    attribute at all (applying to every product) are unaffected. A genuine
    within-function asymmetry: 3 near-identical call sequences in the same
    method, only 1 of which is correctly wired, matching this session's
    repeatedly-confirmed "sibling code blocks, not all correctly wired"
    pattern. See `docs/classes/pimEntitlementTree.md`.
- **CONFIRMED, dead code containing a compile-breaking undeclared-identifier
  typo**: `pimEntitlement.cxx:995`'s `#ifdef PIM_TRANSLATE_XML` block reads
  `pimTranslateMgr MyTranslator(Eptr->GetXMLPtr());` — `Eptr` (lowercase
  `p`) is declared **nowhere** in this function, this file, or anywhere else
  in the `pim_core` module (an exhaustive grep confirms zero other
  occurrences); the only similarly-named identifier in scope is `EPtr`
  (capital `P`, the loop-local prerequisite-entitlement pointer declared at
  `:962`). This would fail to compile were the guarding macro ever active.
  **CONFIRMED DEAD, and will stay dead**: `PIM_TRANSLATE_XML` is `#define`d
  in exactly one place in this entire archive
  (`pim/pim_src/pimTop.cxx:185`), a plain `.cxx` file with no shared header
  — C preprocessor macros do not cross translation-unit boundaries without
  one — and `pimEntitlement.cxx` compiles as part of a demonstrably separate
  translation unit (`pim_core`'s own unity build, which never `#include`s
  `pimTop.cxx`). Contrast: `pimTop.cxx`'s own, same-file `#ifdef PIM_TRANSLATE_XML`
  call site (`:474-490`) is genuinely live and contains no such typo.
  UNKNOWN/unconfirmable from this archive: the project's actual build
  scripts (not present here, an already-flagged gap — see README item 55)
  would be the definitive confirmation that `pim_core` and `pim` are built
  as separate translation units in practice; this finding relies on the
  `.cxx` unity-build source structure alone, the same standard of evidence
  already used for every other "unity build" finding in this doc set (e.g.
  `pimCustomDlg`'s confirmed live status via `pim_ui_src.cxx`'s `#include`).
- **Design characterization, not confirmed reachable**: in the `modify`
  (attribute-translation) branch, `((DOMElement*)Found)->setAttribute(...)`
  is only ever reached if the target attribute **already exists** on
  `Found` (`foundmap->getNamedItem(AttribModify->getNodeValue())` must be
  non-null, `:55-56`); if the target node doesn't already carry that
  attribute, the entry is silently skipped, with no new attribute ever
  created. This matches this session's repeatedly-confirmed "can only
  mutate an existing node/attribute, never create one" limitation (already
  found in `pimShortcutMgr`'s `SetShortcutProgramMenu()` and elsewhere) —
  not confirmed reachable absent a sample product/translations XML pair in
  this archive, but the mechanism is fully confirmed from source.
- **Verified non-issue**: `filename` is a class member and could in
  principle carry a stale value across multiple `TranslateFrom(pimXmlFile*)`
  calls on the same instance if a later call's `ptr->getFilePath()` were
  ever falsy — but every one of this archive's 8 confirmed call sites
  constructs a fresh, single-use `pimTranslateMgr` and calls
  `TranslateFrom()` exactly once, so this is a latent design fragility, not
  a live bug.
- **Verified non-issue**: `TranslateFrom(btkFSEntry&)`'s temporary `xmltmp`
  is `delete`d on both the success and failure paths (`:94,97`) — no leak
  either way, unlike several other `pimXmlFile*`-returning patterns already
  found elsewhere in this codebase.
- **Verified non-issue**: `FindNode()`'s underlying
  `FindNodelistAttribMatch()` returns only the first document-order match
  with no duplicate-name detection — consistent with this session's
  already-established "first match, no uniqueness enforced" pattern found
  elsewhere (e.g. `GetShortcutInfoByID()`), not itself a new confirmed bug.

## Usage Example (as evidenced by call sites)

```cpp
// pim_ui_src/pimEntitlementTree.cxx:1885 (the one arm that is correctly wired)
eptr_new_xml = MD.GetProductXml(mediaid, E->GetTag());
if (eptr_new_xml) { ... }
eptr_new_xml->SetXml(E->GetTag());              // sets xmlProduct's file path FIRST
if (btkString(pimGetLangDirectory(false)) != pimGetLangDirectory(true))
{
    pimTranslateMgr MyTranslator(eptr_new_xml);
    btkFSEntry tfile = "translations.xml";
    bool tfile_is_temp;
    if (LocateTranslationFile(tfile, tfile_is_temp, btkString(mediaid)))
    {
        if (MyTranslator.TranslateFrom(tfile))
            LG_DEBUG(LOG_SERVICE, E->GetXMLPtr()->GetXMLName() << " was translated for the UI language.");
        if (tfile_is_temp)
            tfile.Erase();
    }
}
```
