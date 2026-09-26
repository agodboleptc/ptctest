# Class: `pimGetMediaDetails`

**File:** `pim_core/includes/pimGetMediaDetails.h` (36 lines, full read) / `pim_core/pim_core_src/pimGetMediaDetails.cxx` (240 lines, full read)
**Module:** `pim_core`
**Inherits:** none (plain `GetInstance()`-style singleton, no `pimLoop` ancestry)

## Purpose

A process-wide cache/fetcher for one PTC.com web-service call: given a media ID,
retrieve (and cache) that media's "details" XML (display name/version, and —
critically — the download URLs for every file the media makes available), then
resolve a specific product-definition XML file's URL out of that cached details
document and download it. This is the shared lookup every web-media (download
from PTC.com, as opposed to physical CD/image) code path in this codebase goes
through to turn a bare `(MediaID, xmlFilename)` pair into an actual `pimXmlFile*`.

## Responsibilities

- Maintain a process-wide cache of per-media-ID "details" XML documents
  (`IDs`/`DetailRecords`, parallel arrays), populated on demand and reusable
  across every caller (`GetDetails()`).
- Build the web-service request XML (`PIM_DETAILS_XML`, a file-scope `#define`
  template) for a given media ID, submit it via `pimEntitlementRetrievalFromPTC()`,
  parse the response, and cache the result — or force a fresh fetch and replace
  the cached entry when `refresh=true` is requested.
- Given a cached details document and a target XML filename, find the matching
  `<url>` entry, download it, and parse it into a `pimXmlFile*`
  (`GetProductXml()`) — with a single-retry-on-timeout path that re-fetches the
  details document (to pick up presumably-refreshed download URLs) before
  retrying the download once.

## Dependencies

- `pimXmlFile` (both the cached details documents and the returned
  product-definition XML are `pimXmlFile*`).
- `pimEntitlementRetrievalFromPTC`/`pimRecvFromPTC` (external PTC.com HTTP
  transport functions, declared elsewhere in `pim_core`, not traced to their
  own implementations in this pass).
- `pimSessionInfo`/`pimGetSessionInfo()` (credentials, via callers — this
  class itself takes `wname`/`wpasswd` as parameters rather than reading
  `pimSessionInfo` directly).
- `pimDownloadData` (the byte-buffer type `pimRecvFromPTC()` fills, shared
  with `pimDownloadLoop` — see `docs/classes/pimDownloadLoop.md`).
- `thrMutex` (external threading primitive — `Mutex` member; see Thread
  Safety for a confirmed asymmetry in how it's actually used).

## Members

| Member | Type | Purpose |
|---|---|---|
| `IDs` | `StringXArray` | Parallel array of cached media IDs; index into `DetailRecords`. |
| `DetailRecords` | `dsXArray<pimXmlFile*>` | The cached details documents, one per entry in `IDs`, at the same index. |
| `Mutex` | `thrMutex` (`protected`) | Intended to guard `IDs`/`DetailRecords` — see Thread Safety for a confirmed gap in its actual coverage. |
| `OnlyInstance` | `static pimGetMediaDetails*` | The singleton pointer, `XNew`'d once by `GetInstance()`, never explicitly deleted anywhere in this archive. |

## Public APIs

| Method | Purpose |
|---|---|
| `static pimGetMediaDetails& GetInstance()` | Lazily constructs `OnlyInstance` on first call; standard, unsynchronized (no lock around the `NULL` check/construction itself) lazy-singleton pattern, consistent with the other `GetInstance()`-style singletons already documented in this set. |
| `pimXmlFile *GetDetails(const char *MediaId, btkWString &wname, btkWString &wpasswd, bool refresh = false)` | Returns the cached details document for `MediaId` if present and `!refresh`; otherwise fetches fresh from PTC.com, caches the result (appending on a cache miss, `dsXArray::Replace()`-ing in place on a `refresh` of an existing entry), and returns it. Returns `NULL` on any fetch/parse/auth failure. **See Risk Analysis for a confirmed unsynchronized-read bug on the cache-hit path.** |
| `pimXmlFile *GetProductXml(const char *MediaId, const char *xml_filename)` | Looks up `MediaId`'s cached details document (does **not** fetch it — the header comment "`GetDetails` should be called first" documents this precondition, and it is confirmed to hold at every traced call site), finds the `<url>` entry matching `*xml_filename\?*`, downloads it, and parses it. On a `-418` (timeout) response, refreshes the details cache once and retries once — **see Risk Analysis for a confirmed bug in this retry path.** Returns `NULL` if `MediaId`/`xml_filename` is `NULL`, the media ID isn't cached, no URL matches, or the download fails twice in a row. |

## Protected APIs

| Method | Purpose |
|---|---|
| `pimGetMediaDetails()` | Empty; protected to force construction only through `GetInstance()`. |
| `~pimGetMediaDetails()` | **Empty, with a literal `// TODO cleanup` comment** — `DetailRecords`' heap-allocated `pimXmlFile*` entries are never freed here. Moot in practice since `OnlyInstance` is never explicitly destroyed (see Lifetime), but a confirmed, self-documented incompleteness matching this project's other explicitly-flagged `// TODO` gaps (e.g. `pimDownloadLoop`'s `// TODO handle TIMEOUT/ERRORS`). |

## Private Utilities

None — no `private` methods; `PIM_DETAILS_XML` is a file-scope `#define` (defined
directly above `GetDetails()`, `#undef`'d at end of file), not a class member.

## Called By

- `pim_ui_src/pimEntitlementTree.cxx:1850` (`pimEntitlementTree::OnOptionMenuSelect()`)
  — calls `GetDetails()` then `GetProductXml()` when the user changes a row's
  version/shipcode dropdown. **This is the confirmed source of the `NULL` that
  reaches the already-documented null-pointer-dereference bug in this same
  function** (`eptr_new_xml->SetXml(E->GetTag())` immediately after an
  `if (eptr_new_xml)` guard — see `docs/classes/pimEntitlementTree.md`'s Risk
  Analysis): `GetProductXml()` returns `NULL` whenever no `<url>` entry matches
  the target filename or the download fails twice, both real, reachable
  outcomes for a media whose feed doesn't yet reference the file being
  requested.
- `pim_core/pim_core_src/pimEntitlement.cxx:1163` (entitlement/prerequisite
  construction, `pimEntitlement::Init()`'s prerequisite-parsing loop) and
  `:1290` (`pimEntitlement::UpdateMediaUrls()`, explicitly for "web download
  when the download_url_references timeout and have to be regenerated" per
  its own comment) — **the correct comparison case for the `GetProductXml()`
  retry bug below**: this method calls `GetDetails(..., true)` to refresh,
  then explicitly calls `UpdateUrls(new_xml, UrlRoot)` to re-derive URLs from
  the freshly-fetched document before using them.
- `pim_ui_src/pimInstallMgrActions.cxx:1824` (`uiApplicationsList::OnOptionMenuSelect()`)
  — a near line-for-line duplicate of `pimEntitlementTree::OnOptionMenuSelect()`'s
  logic, including the identical `eptr_new_xml->SetXml(...)` unconditional
  dereference pattern. **Confirmed NOT live**: this entire function (and all of
  `uiApplicationsList`'s other out-of-line method bodies, `pimInstallMgrActions.cxx:1672-2019`)
  is wrapped in `#if 0`, mirroring the `uiApplicationsList` class declaration's
  own `#if 0` block in `pimInstallMgrDlg.h:298-330` (see
  `docs/classes/pimInstallMgrDlg.md`'s Risk Analysis on `uiCheckButtonCell`).
  Not a reachable 2nd instance of the bug, but strong evidence the null-deref
  pattern predates `pimEntitlementTree` — likely copied from (or to)
  `uiApplicationsList` before the latter was superseded and disabled.
- `pim_core/pim_core_src/pimCustomActions.cxx:1657` and
  `pim_core/pim_core_src/pimFlexAdminInstall.cxx:598` — each calls
  `GetDetails()` directly for a specific media ID; neither file was read in
  depth in this pass beyond confirming the call site exists.
- `pim_core/pim_core_src/pimGetAvailable.cxx:481` (`pimGetAvailable::OnExecute()`)
  — calls `GetDetails()` to resolve a discovered product's media details, then
  matches its own product-definition URL directly rather than calling
  `GetProductXml()` (see `docs/classes/pimGetAvailable.md`).
- `pim_core/pim_core_src/pimLocate.cxx` — `LocateXMLFile()` (`:75`, plus a 2nd
  call site inside the same function, `:104`, matching the "search all
  available MediaIDs" fallback already documented in
  `docs/classes/pimGetAvailable.md`) and `LocateTranslationFile()` (`:230`,
  `:277`, same 2-site shape) both call `GetProductXml()` as their
  web-media fallback when a file isn't found on a physical/local image.

## Calls Into

- `pimXmlFile` (`DoRead`/`ErrorOccured`/`GetNextNodelistItem`/`SetAllMatchingTextNodes`/
  `UseNS`/`UsePrettyPrint`).
- `pimEntitlementRetrievalFromPTC`, `pimRecvFromPTC` (PTC.com HTTP transport,
  external to this pass).
- `pimGetLocaleShort` (populates the request template's `<language>` field).

## Lifetime

Heap-allocated (`XNew`) on first `GetInstance()` call, lives for the rest of
the process — never explicitly destroyed anywhere in this archive (consistent
with the destructor's own incomplete `// TODO cleanup` state; there is simply
no code path that would ever invoke it). Its cached `DetailRecords` entries
therefore also live for the process's full lifetime once fetched, with no
eviction — a small, bounded cache in practice (one entry per distinct media ID
touched during one install session), not confirmed to be a practical problem.

## Ownership Model

Classic process-wide singleton, structurally identical in shape (private/
protected ctor+dtor, `static GetInstance()`, no explicit destruction) to the 7
already-documented `pim_core` owner-wrapper singletons (`pimCopier`,
`pimMSICopier`, `pimSFXCopier`, `pimShortcuts`, `pimRegEdit`, `pimServices`,
`pimDownloader`) — but functionally unrelated to that family (it wraps no
install-step `pimLoop`, and is not counted among those 7). `DetailRecords`
owns its `pimXmlFile*` entries outright (heap-allocated, cached by pointer);
callers that receive a pointer from `GetDetails()`/`GetProductXml()` do **not**
own it — nothing in this class or its confirmed callers ever `delete`s a
`pimXmlFile*` returned from either method (each caller reads from it and
discards the pointer, relying on the cache — or, for `GetProductXml()`'s
freshly-downloaded result, apparently just leaking it, since it is never added
to `DetailRecords` and never explicitly freed by any traced caller; not
re-verified against every possible caller in this archive).

## Thread Safety

**CONFIRMED: `Mutex` only protects writers against each other, not readers
against writers.** `GetDetails()`'s cache-hit fast path
(`if (idx != -1 && !refresh) return DetailRecords[idx];`, `:70-73`) returns
directly with **no `Mutex.Lock()` at all** — while every path that mutates
`DetailRecords` (the cache-miss fetch-and-append path, and the `refresh`
fetch-and-`Replace()` path) does so under `Mutex` (`:79` through the balanced
`Mutex.Unlock()` at each of that block's 5 return points, plus the fallthrough
at `:139`). If one thread calls `GetDetails()` for a cache hit while another
thread is concurrently inside the locked fetch/refresh block for the *same or
a different* media ID, the reader can observe `DetailRecords` mid-mutation
(e.g., mid-`Replace()`, or mid-array-growth from a concurrent `+=` append) with
no synchronization at all. Given this singleton is confirmed called from at
least `pim_ui` (UI thread) and `pim_core/pimGetAvailable.cxx`'s background
`OnExecute()` thread (see Called By), a genuine cross-thread race is
structurally possible, not merely theoretical. This is the same class of
finding as `pimServices`' confirmed early-`xmlMutex.Unlock()` race and
`pimRegEdit.h`'s stale locking-comment finding documented elsewhere in this
set — here the gap is an entirely unlocked fast path rather than a
too-early unlock.

## Extension Points

- A cache-eviction or TTL policy would need to be added to `GetDetails()`
  (there is currently none — entries live until process exit) and would also
  need to finally implement `~pimGetMediaDetails()`'s `// TODO cleanup`.
- Any future caller wanting a fresh, uncached fetch without mutating the
  shared cache would need a new method — `refresh=true` on `GetDetails()`
  today always replaces the shared cache entry, there is no "fetch but don't
  cache" mode.

## Risk Analysis

- **CONFIRMED, HIGH SEVERITY: `GetDetails()`'s cache-hit path reads
  `DetailRecords` with no lock**, while every mutating path holds `Mutex` —
  see Thread Safety. This is a genuine, confirmed incomplete-locking bug in a
  singleton called from both the UI thread and at least one background
  `pimLoop` thread (`pimGetAvailable::OnExecute()`).
- **CONFIRMED: `GetProductXml()`'s timeout-retry path refreshes the cache but
  then retries with the stale, pre-refresh URL.** On a `-418` response
  (`:172-202`), the code refreshes `DetailRecords[idx]` via a nested
  `GetDetails(MediaId, wname, wpasswd, true)` call — logging
  `"Replacing download URLs for : " << MediaID` — but the subsequent retry
  (`continue;` back to the top of the `while (ret != 1)` loop) calls
  `pimRecvFromPTC(UrlTmp, data)` using the **same `UrlTmp` string extracted
  before the refresh** (`:162-163`, outside and before this retry loop);
  `UrlTmp` is never re-derived from the newly-fetched details document. The
  log message's own stated intent ("Replacing download URLs") is never
  actually acted on before the retry that intent was supposed to fix. This is
  confirmed by direct contrast with the correct sibling pattern in
  `pimEntitlement::UpdateMediaUrls()` (`pimEntitlement.cxx:1284-1320`, see
  Called By), which performs the same "refresh on timeout" operation but
  **does** call `UpdateUrls(new_xml, UrlRoot)` afterward to actually apply the
  refreshed data before retrying — the same cross-comparison technique used
  throughout this documentation effort to distinguish a genuine defect from
  intentional design.
- **CONFIRMED, but unresolvable in this archive: a possible redundant (or
  worse) double-`Replace()` on the same timeout-retry path.** Immediately
  after the nested `GetDetails(..., true)` call above returns `new_xml`
  (`:186-188`), `GetProductXml()` itself calls `DetailRecords.Replace(idx, 1,
  pArr, 1)` again with `pArr[0] = new_xml` (`:191-197`) — but `GetDetails(...,
  true)` **already performed this exact `Replace()` internally**
  (`:108-116`) and its return value, `new_xml`, is precisely what is now
  already stored at `DetailRecords[idx]`. Whether this 2nd `Replace()` call is
  merely redundant (harmless if `dsXArray::Replace()` doesn't free the
  element it overwrites when old-pointer-equals-new-pointer) or actively
  dangerous (a double-free/dangling-pointer bug if `Replace()` unconditionally
  deletes the existing slot's element before inserting) **cannot be determined
  from this archive** — `dsXArray` is an external `btk`/`ds` toolkit
  container, and no other call site of `.Replace()` exists anywhere in this
  codebase to infer its ownership contract from. Flagged as UNKNOWN severity,
  not asserted as a confirmed crash — but confirmed, at minimum, as dead
  redundant code that suggests the original author was not confident
  `GetDetails(refresh=true)` already handled the replacement.
- **CONFIRMED, low practical severity: `~pimGetMediaDetails()`'s `// TODO
  cleanup`** — the singleton's `DetailRecords` are never freed, but this is
  moot since the singleton itself is never destroyed in any traced code path
  (consistent with the same "singletons live for the process, cleanup TODOs
  are dead code" pattern already observed in this documentation set for other
  singletons).
- **CONFIRMED, and directly relevant to an already-documented bug**:
  `GetProductXml()`'s multiple `NULL`-return paths are the concrete, confirmed
  source of the `NULL` that reaches `pimEntitlementTree::OnOptionMenuSelect()`'s
  already-documented unconditional-dereference crash (see Called By and
  `docs/classes/pimEntitlementTree.md`). This pass does not change that
  finding's severity, but confirms exactly which conditions in
  `GetProductXml()` (no matching `<url>` entry; 2 consecutive `-418` timeouts)
  make it reachable in practice.

## Usage Example (as evidenced by call sites)

```cxx
// pim_ui_src/pimEntitlementTree.cxx:1850-1881 (OnOptionMenuSelect, trimmed)
pimGetMediaDetails &MD = pimGetMediaDetails::GetInstance();
pimXmlFile *new_xml = MD.GetDetails(mediaid, wname, wpasswd); // GetDetails first, per the header's own comment
if (new_xml && ! E->Init(NULL, new_xml, mediaid))
{
    pimXmlFile *eptr_new_xml = MD.GetProductXml(mediaid, E->GetTag());
    // CONFIRMED BUG (see docs/classes/pimEntitlementTree.md): eptr_new_xml can be
    // NULL here (e.g. no matching <url> entry) and is dereferenced unconditionally
    // 2 lines later in the real source, outside the `if (eptr_new_xml)` guard.
}
```

---
*Extends this documentation set's `pim_core`-focused extension series:
discovered while documenting `pimGetAvailable` (both classes are used together
resolving web-media downloads), and given full-depth treatment as the 2nd
`pim_core` singleton class documented outside the original 7-class
owner-wrapper family.*
