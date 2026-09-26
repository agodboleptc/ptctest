# Class: `pimXmlFile`

**File:** `pim_core/includes/pimXmlFile.h` (285 lines) / `pim_core/pim_core_src/pimXmlFile.cxx` (not read line-by-line in this pass)
**Module:** `pim_core`

## Purpose

The single wrapper around Xerces-C++ DOM parsing/reading/writing used for every XML
document in PIM: product/entitlement definitions, session state, translation files,
image metadata. Provides a `<PROPERTY name="...">value</PROPERTY>`-style
get/set-property convenience API on top of raw DOM access, plus read/write locking so
the same document can be safely touched from multiple threads (each `pimLoop`
subclass holds a `pimXmlFile*` and may run on its own thread).

## Responsibilities

- Parse an XML file into a Xerces `DOMDocument*` (`DoRead()`, `DoRead(MemBufInputSource&)`).
- Serialize the document back out (`DoSave()`, `DoWrite(path)`, `DoWriteToDir(dir)`).
- Provide a convenience property API modeling `<PROPERTY name="X" attrib="Y">Z</PROPERTY>`
  elements: `SetProperty`/`GetProperty`/`IsPropertySet`, plus `GetTextNode` and
  `SetAllMatchingTextNodes` for bulk text-node operations.
- Provide raw DOM traversal helpers: `GetNextNodelistItem` (stateful iteration via
  `GNNI_nl`/`GNNI_index`/`GNNI_lastList`), `FindNodelistAttribMatch`,
  `GetChildNodeByNodeName`.
- Guard concurrent access with an explicit read/write locking protocol
  (`PreRead`/`PostRead`, `PreWrite`/`PostWrite`, plus `PushReadToWrite`/
  `PopWriteToRead` for lock upgrade) built on `thrRWLock`.
- Track and report parse errors (`DOMTreeErrorReporter`, `ErrorOccured()`,
  `ResetErrors()`) and content-changed state (`SetContentChangedFlag`/
  `ContentChanged()`).
- Support schema validation (`UseSchema()`, `SetSchema(xsd)`,
  `XercesDOMParser::ValSchemes gValScheme`) and namespace-aware parsing (`UseNS()`).
- Support pretty-printing and output filtering on serialization
  (`UsePrettyPrint()`, `SetFilter(DOMPrintFilter*)`, the nested `DOMPrintFilter`
  class controlling which node types are shown).
- Support restoring from a backup file (`RestoreFromBackup(path)`).

## Dependencies

- **Xerces-C++** directly: `DOMDocument`, `XercesDOMParser`, `DOMImplementation`,
  `DOMLSSerializer`, `DOMLSOutput`, `DOMConfiguration`, `XMLFormatTarget`,
  `MemBufInputSource`, `LocalFileFormatTarget`, `StdOutFormatTarget`, `ErrorHandler`,
  `SAXParseException`, `DOMError`/`DOMErrorHandler`, `DOMNodeFilter`/
  `DOMLSSerializerFilter`.
- `btkthrmutex.h` (external `btk`) — `thrRWLock`, `thrMutex`, `btkThrMutex` for the
  locking protocol.
- `pimDefs.h` (local XML tag/attribute name constants — not enumerated in this pass;
  belongs to Phase 11, `docs/08_xml_configuration.md`).
- `btk_u16stou8s_alloc`/`btk_u8stou16s_alloc`/`btkStrFree` (external `btk` string
  conversion, used by the nested `StrX` transcoding helper class in place of raw
  Xerces `XMLString::transcode`/`release`).

## Members

| Member | Type | Access | Purpose |
|---|---|---|---|
| `xmlname`, `xsdfile` | `btkString` | private | Document name / schema file path |
| `xmlfile` | `btkFSEntry` | private | Source file path |
| `xmlContents` | `btkWString` | private | Cached wide-string content (`GetXMLContents()`) |
| `gValScheme`, `gOutputEncoding`, `gDoNamespaces`, `gDoSchema`, `gSchemaFullChecking`, `gDoCreate`, `gSplitCdataSections`, `gDiscardDefaultContent`, `gUseFilter`, `gFormatPrettyPrint`, `gWriteBOM` | various | private | Parser/serializer configuration flags |
| `myFilter` | `DOMPrintFilter*` | private | Active output filter, if any |
| `xmlMutex` | `mutable thrRWLock*` | private | Read/write lock over the document |
| `classMutex` | `thrMutex` | private | Secondary lock (purpose not fully disambiguated from `xmlMutex` in this pass — likely guards the locking-state bookkeeping itself, e.g. `mReadingCount`/`wLockEngaged`, separately from the document content lock) |
| `wLockEngaged` | `bool` | private | Write-lock-currently-held flag (read by `pimLoop::Kill()`, see `docs/classes/pimLoop.md`) |
| `mReadingCount` | `int` | private | Active-reader count (backs `ReadLockEngaged()`) |
| `mReadLock` | `btkThrMutex` | private | Guards `mReadingCount` updates |
| `doc` | `DOMDocument*` | protected | The parsed document |
| `parser` | `XercesDOMParser*` | protected | Owned parser instance |
| `errReporter` | `DOMTreeErrorReporter*` | protected | Error callback target |
| `errorsOccured` | `bool` | protected | Parse-error flag |
| `impl`, `theSerializer`, `theOutputDesc`, `theDC`, `memTarget` | various Xerces types | protected | Serialization machinery |
| `changeInContent` | `bool` | protected | Dirty flag |
| `GNNI_nl`, `GNNI_index`, `GNNI_lastList` | `DOMNodeList*`, `int`, `btkString` | protected | Stateful cursor for `GetNextNodelistItem` |

## Public APIs

See header for full signatures (§ above in this doc's Responsibilities lists the
grouped behavior); notable ones not otherwise called out:

- `SetXml(const char*)`, `SetSchema`, `UseNS`, `UsePrettyPrint`, `UseSchema`.
- `PreRead`/`PostRead`/`ReadLockEngaged`, `PreWrite`/`PostWrite`/`WriteLockEngaged`,
  `PushReadToWrite`/`PopWriteToRead` — the locking protocol; comment in the header
  explicitly notes `PreRead`/`PreWrite` "are automatically called within a class
  method such as `GetProperty()`" but must be called manually when doing raw
  multi-step DOM traversal outside a single property call.
- `GetXMLName()`, `getDocument()`, `getFilePath()`.

## Protected APIs

`GetPropertyNode(name, matchedNode=NULL)` is declared `private`, not protected —
listed here as it's the shared internal lookup both `SetProperty` and `GetProperty`
presumably delegate to (not confirmed against the `.cxx`, but implied by naming and
being the sole private method).

## Private Utilities

`GetPropertyNode` (see above) — the only private method.

## Called By

Nearly every other class in `pim_core` and several in `pim`: `pimEntitlement`
(via inherited `xmlPtr` from `pimLoop`), `pimSessionInfo` (its own `xmlPtr`),
`pim/pim_src/pimTop.cxx` (directly parses image XML, `pimPIMSETUP` node, etc.), every
`pim*Loop` subclass (each holds an `xmlPtr` and reads its configuration from it),
`pimTranslateMgr` (translation file XML).

## Calls Into

Xerces-C++ DOM/parser APIs directly; `btk` string-transcoding helpers (`StrX`); its
own nested `DOMTreeErrorReporter`/`DOMPrintErrorHandler`/`DOMPrintFilter` helper
classes.

## Lifetime

Typically heap-allocated with `XNew pimXmlFile()` and explicitly `delete`d by the
owner once done (seen repeatedly in `pim/pim_src/pimTop.cxx`: `pimXmlFile *ImageXml =
XNew pimXmlFile(); ...; delete ImageXml;`). Longer-lived instances (an entitlement's
or session's own XML) live as long as the owning `pimEntitlement`/`pimSessionInfo`.

## Ownership Model

Each `pimXmlFile` owns its own Xerces `DOMDocument*`/`XercesDOMParser*`/serializer
objects (constructed/destroyed in ctor/dtor, not shown in the header). Ownership of
the `pimXmlFile*` itself is external — `pimLoop`/`pimEntitlement`/`pimSessionInfo`
hold a pointer but the header gives no evidence any of them delete it (their own
dtors were not read in this pass).

## Thread Safety

This is the one class in the codebase with an explicit, documented concurrency
protocol rather than an implicit "whole object under one mutex" pattern:

- `PreRead()`/`PostRead()` — shared/read access, tracked via `mReadingCount` under
  `mReadLock`.
- `PreWrite()`/`PostWrite()` — exclusive access, tracked via `wLockEngaged`.
- `PushReadToWrite()`/`PopWriteToRead()` — an explicit lock-upgrade path (read → write
  → back to read), which is exactly the kind of operation that's a common source of
  deadlocks in RW-lock designs if not implemented as a true atomic upgrade.
- The header comment revision history includes "Fixed xml mutex issues" (Q-27-15,
  2026-06-26) and "Added temporary logs to investigate locks issue" (Q-27-06,
  2026-04-22) — **direct evidence from the codebase's own history that this class's
  locking has been a real, recent source of bugs**, not just a theoretical risk.

## Extension Points

- New property-style XML pattern: extend `GetPropertyNode`/`SetProperty`/
  `GetProperty` if a new attribute convention is needed beyond `name`/`attrib`/text
  content.
- New output filtering: extend `DOMPrintFilter::acceptNode`.

## Risk Analysis

- **Confirmed historical locking bugs** (see Thread Safety) — any change touching
  `PreRead`/`PreWrite`/`PushReadToWrite`/`PopWriteToRead` should be treated as
  high-risk and tested for both correctness and deadlock/starvation under concurrent
  Loop threads, given multiple `pimLoop` subclasses may hold and operate on the same
  `pimEntitlement`'s `xmlPtr` concurrently (e.g. one thread reading progress while
  another writes install-status).
- The comment "these are automatically called within a class method such as
  `GetProperty()`" places the burden on every *other* call site in the codebase to
  remember manual `PreRead`/`PreWrite` bracketing when doing raw `GetNextNodelistItem`/
  `FindNodelistAttribMatch` traversal — a missed bracketing call is a silent
  correctness/thread-safety bug, not a compile-time-checkable one.
- Two separate lock-like members (`xmlMutex` a `thrRWLock*`, plus `classMutex` a
  `thrMutex`, plus `mReadLock` a `btkThrMutex`) — three distinct synchronization
  primitives on one class is unusually intricate and increases the chance that a
  future change picks the wrong one or misses interactions between them.

## Usage Example (as evidenced by call sites)

```cxx
// pim/pim_src/pimTop.cxx pattern
pimXmlFile *ImageXml = XNew pimXmlFile();
ImageXml->SetXml(pimImageLocation);
ImageXml->UseNS();
ImageXml->UsePrettyPrint();
ImageXml->DoRead();
if (!ImageXml->ErrorOccured())
{
    StringXArray attribs;
    btkString text_value;
    if (ImageXml->GetTextNode(StrX(pimSHIPCODE).localForm(), attribs, text_value))
        SessionInfo.SetProperty(SHIPCODE_PROPERTY, text_value);
}
delete ImageXml;
```

---
*Phase 5 of the requested 20-phase documentation set.*
